/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChHwInit.c
*
* DESCRIPTION:
*       Includes CPSS DXCH level basic Hw initialization functions.
*
* FILE REVISION NUMBER:
*       $Revision: 1.5 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChModuleConfig.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortBufMg.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>
#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiag.h>
#include <cpss/dxCh/dxChxGen/trunk/private/prvCpssDxChTrunk.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnel.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgMc.h>

#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsCheetah3.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsDxChXcat.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsLionB.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsDxChXcat2.h>

#ifdef ASIC_SIMULATION
    #define DEBUG_OPENED
#endif /*ASIC_SIMULATION*/

#ifdef DEBUG_OPENED
    #ifdef STR
        #undef STR
    #endif /*STR*/

    #define STR(strname)    \
        #strname

    #define ERRATA_NAMES                                                                 \
    STR(PRV_CPSS_DXCH_FDB_AGING_WITH_REMOVAL_MODE_WA_E                                ), \
    STR(PRV_CPSS_DXCH_TRUNK_CONFIG_FOR_TRUNK_NEXT_HOP_WA_E                            ), \
    STR(PRV_CPSS_DXCH_INCORRECT_SRCDEV_FIELD_IN_MAILBOX_TO_CPU_DSA_TAG_WA_E           ), \
    STR(PRV_CPSS_DXCH_TOKEN_BUCKET_RATE_SHAPING_DISABLE_WA_E                          ), \
    STR(PRV_CPSS_DXCH_JUMBO_FRAMES_MODE_SDWRR_WEIGHT_LIMITATION_WA_E                  ), \
    STR(PRV_CPSS_DXCH_PACKET_ORIG_BYTE_COUNT_WA_E                                     ), \
    STR(PRV_CPSS_DXCH_RATE_LIMIT_GIG_COUNTER_BITS_NUM_WA_E                            ), \
    STR(PRV_CPSS_DXCH_XG_MIB_READ_OVERSIZE_AND_JABBER_COUNTERS_WA_E                   ), \
    STR(PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E                  ), \
    STR(PRV_CPSS_DXCH2_READ_SRC_ID_ASSIGMENT_MODE_CONF_REG_WA_E                       ), \
    STR(PRV_CPSS_DXCH2_BRIDGE_ACCESS_MATRIX_LINE7_WRITE_WA_E                          ), \
    STR(PRV_CPSS_DXCH2_POLICY_ACTION_DIRECT_BURST_ONLY_READ_WA_E                      ), \
    STR(PRV_CPSS_DXCH2_DIRECT_ACCESS_TO_POLICER_ADDRESS_SPACE_WA_E                    ), \
    STR(PRV_CPSS_DXCH2_ROUTER_ARP_AND_TS_TABLE_NOT_SUPPORT_BURST_ACCESS_VIA_PCI_WA_E  ), \
    STR(PRV_CPSS_DXCH2_RDWRTRIG_BIT_IN_VLT_TABLES_ACCESS_CONTROL_REGISTER_IGNORED_E   ), \
    STR(PRV_CPSS_DXCH2_INDIRECT_ACCESS_ROUTER_AND_TT_TCAM_NOT_FUNCTION_CORRECTLY_WA_E ), \
    STR(PRV_CPSS_DXCH_RM_XG_PHY_PATH_FAILURE_WA_E                                     ), \
    STR(PRV_CPSS_DXCH2_RM_RESERVED_REGISTERS_0x0B820108_WA_E                          ), \
    STR(PRV_CPSS_DXCH_RM_FDB_TABLE_ENTRIES_WA_E                                       ), \
    STR(PRV_CPSS_DXCH2_PCL_EGRESS_ACTION_CNTR_VLAN_WRONG_BITS_POS_WA_E                ), \
    STR(PRV_CPSS_DXCH2_RM_BRG_LAYER_4_SANITY_CHECKS_WA_E                              ), \
    STR(PRV_CPSS_DXCH3_VLT_INDIRECT_ACCESS_WA_E                                       ), \
    STR(PRV_CPSS_DXCH3_SAMPLE_AT_RESET_NET_REF_CLOCK_SELECT_WA_E                      ), \
    STR(PRV_CPSS_DXCH3_RM_BM_GE_XG_PORT_GROUP_LIMIT_CFG_REG_WA_E                      ), \
    STR(PRV_CPSS_DXCH3_RM_GE_PORTS_MODE1_REG_WA_E                                     ), \
    STR(PRV_CPSS_DXCH3_RM_XG_PORTS_MODE_REG_WA_E                                      ), \
    STR(PRV_CPSS_DXCH3_RM_TCAM_PLL_REG_WA_E                                           ), \
    STR(PRV_CPSS_DXCH3_RM_LMS0_1_GROUP1_LED_INTERFACE_WA_E                            ), \
    STR(PRV_CPSS_DXCH3_RM_MAC2ME_LOW_REG_WA_E                                         ), \
    STR(PRV_CPSS_DXCH3_RM_AN_CNF_REG_WA_E                                             ), \
    STR(PRV_CPSS_DXCH3_DIRECT_ACCESS_TO_EQ_ADDRESS_SPACE_WA_E                         ), \
    STR(PRV_CPSS_DXCH3_TXQ_PARITY_CALCULATION_E                                       ), \
    STR(PRV_CPSS_DXCH3_SDMA_WA_E                                                      ), \
    STR(PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E                               ), \
    STR(PRV_CPSS_DXCH3_OUT_OF_PROFILE_QOS_ATTR_E                                      ), \
    STR(PRV_CPSS_DXCH3_TXQ_FULL_INTERRUPT_NOT_FUNCTION_WA_E                           ), \
    STR(PRV_CPSS_DXCH3_RM_GE_SERDES_MISC_CONF_REG_WA_E                                ), \
    STR(PRV_CPSS_DXCH3_RM_BM_TX_FIFO_THRESHOLD_CONF_REG_WA_E                          ), \
    STR(PRV_CPSS_DXCH3_TCAM_EFUSE_NOT_TRIGGERED_AUTO_WA_E                             ), \
    STR(PRV_CPSS_DXCH3_POLICY_AND_ROUTER_TCAM_TABLES_INDIRECT_READ_WA_E               ), \
    STR(PRV_CPSS_DXCH_RM_FDB_GLOBAL_CONF_REG_WA_E                                     ), \
    STR(PRV_CPSS_DXCH3_LIMITED_NUMBER_OF_POLICY_BASED_ROUTES_WA_E                     ), \
    STR(PRV_CPSS_DXCH3_TCAM_REPAIR_WA_E                                               ), \
    STR(PRV_CPSS_DXCH3_RM_CPU_ACCESS_TO_FDB_UNDER_TRAFFIC_WA_E                        ), \
    STR(PRV_CPSS_DXCH3_SERDES_YIELD_IMPROVEMENT_WA_E                                  ), \
    STR(PRV_CPSS_DXCH3_TOGGLE_DEV_EN_UNDER_TRAFFIC_WA_E                               ), \
                                                                                         \
    STR(PRV_CPSS_DXCH_XCAT_TABLE_SIZE_FDB_16K_INSTEAD_8K_WA_E                         ), \
    STR(PRV_CPSS_DXCH_XCAT_TABLE_SIZE_NH_4K_INSTEAD_8K_WA_E                           ), \
    STR(PRV_CPSS_DXCH_XCAT_TABLE_SIZE_TS_1K_INSTEAD_2K_WA_E                           ), \
    STR(PRV_CPSS_DXCH_XCAT_TABLE_CNC_NUM_BLOCKS_1_INSTEAD_2_WA_E                      ), \
    STR(PRV_CPSS_DXCH_XCAT_TABLE_POLICERS_NUM_1K_INSTEAD_2K_WA_E                      ), \
    STR(PRV_CPSS_DXCH_XCAT_FDB_AGING_TIMEOUT_WA_E                                     ), \
    STR(PRV_CPSS_DXCH_XCAT_TR101_WA_E                                                 ), \
    STR(PRV_CPSS_DXCH_XCAT_IPLR_SECOND_STAGE_SUPPORT_WA_E                             ), \
    STR(PRV_CPSS_DXCH_XCAT_SGMII_MODE_ON_STACKING_PORTS_WA_E                          ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_TOTAL_BUFFER_LIMIT_CONF_IN_TXQ_E                        ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_CNC_ENABLE_COUNTING_E                                   ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_TXQ_EXT_CNTR_REG_WA_E                                   ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_MIRROR_INTERFACE_PARAMETER_REGISTER_I_WA_E              ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_POLICY_ENGINE_CONFIGURATION_REGISTER_WA_E               ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_TXQ_DEQUEUE_SWRR_WEIGHT_REGISTER_3_WA_E                 ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL0_WA_E                           ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL1_WA_E                           ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_POLICER_CONTROL2_WA_E                                   ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_EGRESS_POLICER_GLOBAL_CONFIGURATION_WA_E                ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_BCN_CONTROL_WA_E                                        ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_PORTS_BCN_AWARENESS_TABLE_WA_E                          ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_TTI_UNIT_GLOBAL_CONFIGURATION_WA_E                      ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_EXTENDED_DSA_BYPASS_BRIDGE_WA_E                         ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_PORT_SERIAL_PARAMETERS_CONFIGURATION_WA_E               ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_LMS0_LMS1_MISC_CONFIGURATIONS_WA_E                      ), \
    STR(PRV_CPSS_DXCH_XCAT_TX_CPU_CORRUPT_BUFFER_WA_E                                 ), \
    STR(PRV_CPSS_DXCH_XCAT_TABLE_EPLR_NOT_SUPPORT_WA_E                                ), \
    STR(PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E                        ), \
    STR(PRV_CPSS_DXCH_XCAT_IEEE_RESERVED_MC_CONFG_REG_READ_WA_E                       ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E                     ), \
    STR(PRV_CPSS_DXCH_XCAT_GIGA_PORT_PRBS_PARAM_INIT_WA_E                             ), \
    STR(PRV_CPSS_DXCH_XCAT_RESTRICTED_ADDRESS_FILTERING_WA_E                          ), \
    STR(PRV_CPSS_DXCH_XCAT_RM_HEADER_ALTERATION_GLOBAL_CONFIGURATION_REGISTER_WA_E    ), \
    STR(PRV_CPSS_DXCH_XCAT_TOD_NANO_SEC_SET_WA_E                                      ), \
    STR(PRV_CPSS_DXCH_XCAT_ETH_OVER_IPV4_GRE_WA_E                                     ), \
    STR(PRV_CPSS_DXCH_XCAT_PASSENGER_ETH_PACKET_CRC_CANT_BE_RETAINED_WA_E             ), \
    STR(PRV_CPSS_DXCH_LION_RM_BUFFER_MANAGEMENT_INIT_VALUES_WA_E                      ), \
    STR(PRV_CPSS_DXCH_LION_RM_SDMA_ACTIVATION_WA_E                                    ), \
    STR(PRV_CPSS_DXCH_LION_TABLE_SIZE_ROUTER_TCAM_13K_INSTEAD_16K_WA_E                ), \
    STR(PRV_CPSS_DXCH_LION_INDIRECT_ACCESS_TO_TCAM_IS_NOT_SUPPORTED_WA_E              ), \
    STR(PRV_CPSS_DXCH_LION_TCAM_SYNCHRONIZATION_RESTRICTION_WA_E                      ), \
    STR(PRV_CPSS_DXCH_LION_FDB_AU_FU_MESSAGES_FROM_NON_SOURCE_PORT_GROUP_WA_E         ), \
    STR(PRV_CPSS_DXCH_LION_RATE_LIMITER_10G_WINDOW_SIZE_WA_E                          ), \
    STR(PRV_CPSS_DXCH_LION_FAST_FAILOVER_WA_E                                         ), \
    STR(PRV_CPSS_DXCH_LION_40G_MAC_NOT_SUPPORTED_WA_E                                 ), \
    STR(PRV_CPSS_DXCH_LION_20G_MAC_NOT_SUPPORTED_WA_E                                 ), \
    STR(PRV_CPSS_DXCH_LION_1G_SPEED_NOT_SUPPORTED_WA_E                                ), \
    STR(PRV_CPSS_DXCH_LION_RM_SYNC_ETHERNET_NONE_SELECT_WA_E                          ), \
    STR(PRV_CPSS_DXCH_LION_RM_TCAM_CONFIG_WA_E                                        ), \
    STR(PRV_CPSS_DXCH_LION_URPF_PER_VLAN_NOT_SUPPORTED_WA_E                           ), \
    STR(PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E                                      ), \
    STR(PRV_CPSS_DXCH_LION_PREVENT_REDUNDANT_AU_FU_MESSAGES_NOT_SUPPORTED_WA_E        ), \
    STR(PRV_CPSS_DXCH_LION_POLICER_COUNTING_MEM_ACCESS_WA_E                           ), \
    STR(PRV_CPSS_DXCH_LION_RM_MULTI_PORT_GROUP_FDB_LIMIT_WA_E                         ), \
    STR(PRV_CPSS_DXCH_LION_RM_ORIG_SRC_PORT_FILTERING_WA_E                            ), \
    STR(PRV_CPSS_DXCH_LION_RM_MULTI_PORT_GROUP_CPU_IN_VIDX_WA_E                       ), \
    STR(PRV_CPSS_DXCH_LION_CN_GLOBAL_REG_WA_E                                         ), \
    STR(PRV_CPSS_DXCH_LION_TEMPERATURE_SENSOR_INITIALIZATION_WA_E                     ), \
    STR(PRV_CPSS_DXCH_LION_RM_MCFIFO_DISTRIBUTION_REG_WA_E                            ), \
    STR(PRV_CPSS_DXCH_LION_RM_SD_PLL_REG_WA_E                                         ), \
    STR(PRV_CPSS_DXCH_LION_SLOW_RATE_ON_PORT_TB_NOT_SUPPORTED_WA_E                    ), \
    STR(PRV_CPSS_DXCH_LION_RM_UC_STRICT_OVER_MC_WA_E                                  ), \
    STR(PRV_CPSS_DXCH_LION_RM_SCHEDULER_DEFICIT_MODE_WA_E                             ), \
    STR(PRV_CPSS_DXCH_LION_RM_TX_MC_IN_CHUNKS_WA_E                                    ), \
    STR(PRV_CPSS_DXCH_LION_RM_BUFF_LIMIT_EGRESS_QUEUE_WA_E                            ), \
    STR(PRV_CPSS_DXCH_XCAT2_RM_STACK_PORT_MIB_CNTR_CONTROL_WA_E                       ), \
    STR(PRV_CPSS_DXCH_XCAT2_EPCL_GLOBAL_EN_NOT_FUNCTIONAL_WA_E                        ), \
    STR(PRV_CPSS_DXCH_XCAT2_RM_POLICER_COUNT_FORMAT_WA_E                              ), \
    STR(PRV_CPSS_DXCH_XCAT2_RM_INT_EXT_REF_CLK_WA_E                                   ), \
    STR(PRV_CPSS_DXCH_LION_RM_QCN_FOR_CN_FORMAT_WA_E                                  ), \
    STR(PRV_CPSS_DXCH_LION_XLG_WS_SUPPORT_WA_E                                        ), \
    STR(PRV_CPSS_DXCH_LION_PFC_FOR_MANY_PRIORITY_WA_E                                 ), \
    STR(PRV_CPSS_DXCH_LION_QCN_FORMAT_COMPLIANT_WA_E                                  ), \
    STR(PRV_CPSS_DXCH_LION_INACCURATE_PFC_XOFF_WA_E                                   ), \
    STR(PRV_CPSS_DXCH_LION_INCOMPLETE_CPU_MAILBOX_INFO_WA_E                           ), \
    STR(PRV_CPSS_DXCH_XCAT2_RM_POLICER_MEM_CONTROL_WA_E                               ), \
    STR(PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E                     ), \
    STR(PRV_CPSS_DXCH_XCAT2_RM_INCORRECT_XG_SHAPER_TOKEN_BUCKET_WA_E                  ), \
    STR(PRV_CPSS_DXCH_XCAT2_RM_BYPASS_PACKET_REFRESH_FDB_AGE_WA_E                     ), \
    STR(PRV_CPSS_DXCH_XCAT2_RM_PIPE_SELECT_WA_E                                       ), \
    STR(PRV_CPSS_DXCH_LION_IPCL_FALSE_PARITY_ERROR_WA_E                               ), \
                                                                                         \
    "--last one--"

    static char * errataNames[PRV_CPSS_DXCH_ERRATA_MAX_NUM_E+1] = {ERRATA_NAMES};
#endif /*DEBUG_OPENED*/

/* used print routine   */
#define DUMP_PRINT_MAC(x) cpssOsPrintf x
/* get the name (string) and value of field */
#define NAME_AND_VALUE_MAC(field)   #field , field

/* pointers of DXCH devices into prvCpssPpConfig[] --> for debugging only */
static PRV_CPSS_DXCH_PP_CONFIG_STC* dxCh_prvCpssPpConfig[PRV_CPSS_MAX_PP_DEVICES_CNS];

/* pointer to a function used for DXCH devices that will do enhanced
   initialization of SW and HW parameters --  for FUTURE compatibility */
PRV_CPSS_DXCH_PP_CONFIG_ENHANCED_INIT_FUNC prvCpssDxChPpConfigEnhancedInitFuncPtr = NULL;

/* define constant for quick search for code that not support device types */
#define GT_DEVICE_FAMILY_NOT_SUPPORTED_CNS      GT_NOT_SUPPORTED
#define HW_INIT_SMI_POOLING_TIMEOUT_CNS 200000

/* definitions for interrupt coalescing */
#define MIN_INTERRUPT_COALESCING_PERIOD_CNS 320
#define MAX_INTERRUPT_COALESCING_PERIOD_CNS 5242560
/* number of VIDXs in the device */
#define VIDX_NUM_CNS  (_4K-1)
/* number of STG (spanning tree groups) in the device */
#define STG_NUM_CNS     256
/* number of trunks in the device */
#define TRUNKS_NUM_CNS  127

/* 4 TX Queues number */
#define TX_QUEUE_NUM_4_CNS                4

/* 8 TX Queues number */
#define TX_QUEUE_NUM_8_CNS                8

/* not applicable (NA) table */
#define NA_TABLE_CNS    0

/* auto calc field */
#define AUTO_CALC_FIELD_CNS 0xFFFFFFFF

/* indication that the number of CNC blocks and size of block should be
    'auto calculated' from the 'total number of cnc counters ...*/
#define CNC_AUTO_CALC                       AUTO_CALC_FIELD_CNS

/* in Lion only portGroupId#3 has the RGMII registers for the CPU port */
#define LION_RGMII_PORT_GROUP_ID_CNS  3

/* indication that there was no need to replace a tcam row using the efuse */
#define NO_TCAM_ROW_TO_REPLACE_CNS 0xFFFFFFFF

/* maximum number of PLL recovery loops in the first level wrapper */
#define PLL_UNLOCK_WORKAROUND_MAX_LOOPS_1ST_WRAPPER_CNS 25

/* maximum number of PLL recovery loops in the second level wrapper for each setting change */
#define PLL_UNLOCK_WORKAROUND_MAX_LOOPS_2ND_WRAPPER_CNS 5

/* maximum number of PLL setting changes in the second level wrapper */
#define PLL_UNLOCK_WORKAROUND_MAX_LOOPS_SET_CHANGE_CNS 15

/* maximum number of PLL recovery loops in the third level wrapper for each setting change */
#define PLL_UNLOCK_WORKAROUND_MAX_LOOPS_3RD_WRAPPER_CNS 5

/* number of network SERDES */
#define PRV_CH3_NETWORK_SERDES_NUM_CNS 24

/* HW value of ports number used for resource calculations */
#define PRV_DXCH_XCAT_HW_NUM_OF_PORTS_CNS 29

#define _13K    (13 * _1K)

/* Maximal address range inside one unit in words ((bits 0..22) >> 2) */
#define PRV_DXCH_MAX_UNIT_ADDR_RANGE_CNS    (1 << 21)

/* Calculate the last row of the TCAM logical sub-block for the row that was replaced */
/* to invalidate. A TCAM logical sub-block comprises 128 rows.                        */
#define LAST_SUB_BLOCK_ROW_CALC_MAC(_row)  _row = (_row/128 + 1)*128 - 1

static GT_STATUS hwPpPhase1Part4
(
    IN  GT_U8               devNum
);

static GT_STATUS hwPpPhase1Part5
(
    IN  GT_U8               devNum
);

static GT_STATUS hwPpPhase1Part6
(
    IN  GT_U8               devNum,
    IN  CPSS_DXCH_PP_PHASE1_INIT_INFO_STC   *ppPhase1ParamsPtr
);

GT_STATUS prvCpssDxChBrgMcEntryWrite
(
    IN GT_U8                devNum,
    IN GT_U16               vidx,
    IN CPSS_PORTS_BMP_STC   *portBitmapPtr
);

/* Check for device's port existence and skip if port does not exist */
#define PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(_dev, _port) \
    if(!PRV_CPSS_PHY_PORT_IS_EXIST_MAC(_dev, _port)) \
        continue;

/* Check for SERDES existence and skip if SERDES does not exist */
#define PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(_serdes, _sbitmap) \
    if (((_sbitmap) & (1 << (_serdes))) == 0) \
        continue;

/* Check for power down SERDES bitmap and skip if SERDES should not be powered up */
#define PRV_CPSS_DXCH_SKIP_POWER_DOWN_PORTS_MAC(_port, _sbitmap) \
    if (((_sbitmap) & (1 << (_port))) != 0) \
        continue;

/* indication for the last device in devices lists */
#define LAST_DEV_IN_LIST_CNS   0xFFFFFFFF

/* Alleycat Unmanaged devs 24GE */
#define ALLEYCAT_UNMANAGED_DEVS_24GE_MAC \
    CPSS_98DX3001_CNS,             \
    CPSS_98DX3011_CNS,             \
    CPSS_98DX3021_CNS

/* Alleycat2 Unmanaged devs 24GE */
#define ALLEYCAT_XCAT2_UNMANAGED_DEVS_24GE_MAC \
    CPSS_98DX3011B_CNS,             \
    CPSS_98DX3021B_CNS

/* Alleycat WebSmart devs 24FE */
#define ALLEYCAT_WEBSMART_DEVS_24FE_MAC  \
    CPSS_98DX1005_CNS,             \
    CPSS_98DX1025_CNS,             \
    CPSS_98DX1026_CNS,             \
    CPSS_98DX1027_CNS

/* Alleycat xCat2 WebSmart devs 24FE */
#define ALLEYCAT_XCAT2_WEBSMART_DEVS_24FE_MAC  \
    CPSS_98DX1033_CNS,             \
    CPSS_98DX1034_CNS,             \
    CPSS_98DX1035_CNS,             \
    CPSS_98DX1055_CNS

/* Alleycat WebSmart devs 24GE */
#define ALLEYCAT_WEBSMART_DEVS_24GE_MAC  \
    CPSS_98DX3005_CNS,             \
    CPSS_98DX3015_CNS,             \
    CPSS_98DX3025_CNS,             \
    CPSS_98DX3026_CNS,             \
    CPSS_98DX3027_CNS,             \
    CPSS_98DXJ324_CNS

/* Alleycat xCat2 WebSmart devs 24GE */
#define ALLEYCAT_XCAT2_WEBSMART_DEVS_24GE_MAC  \
    CPSS_98DX3033B_CNS,            \
    CPSS_98DX3034B_CNS,            \
    CPSS_98DX3035B_CNS,            \
    CPSS_98DX3055_CNS,             \
    CPSS_98DX3036_CNS

/* the devices with up to 24GE+4stacking */
#define ALLEYCAT_UNMANAGED_DEVS_MAC \
    ALLEYCAT_UNMANAGED_DEVS_24GE_MAC,   \
    ALLEYCAT_XCAT2_UNMANAGED_DEVS_24GE_MAC

#define ALLEYCAT_WEBSMART_DEVS_MAC \
    ALLEYCAT_WEBSMART_DEVS_24GE_MAC,  /* the devices with up to 24GE+4stacking */\
    ALLEYCAT_WEBSMART_DEVS_24FE_MAC   /* the devices with up to 24FE+4stacking */\

#define ALLEYCAT2_WEBSMART_DEVS_MAC \
    ALLEYCAT_XCAT2_WEBSMART_DEVS_24FE_MAC, \
    ALLEYCAT_XCAT2_WEBSMART_DEVS_24GE_MAC

/* PONcat devs 24FE */
#define PONCAT_DEVS_24FE_MAC  \
    CPSS_98DX1002_CNS,       \
    CPSS_98DX1022_CNS,       \
    CPSS_98DX1023_CNS

/* PONcat2 devs 24FE */
#define PONCAT2_DEVS_24FE_MAC  \
    CPSS_98DX1133_CNS,         \
    CPSS_98DX1134_CNS,         \
    CPSS_98DX1135_CNS,         \
    CPSS_98DX1165_CNS

/* PONcat devs 24GE */
#define PONCAT_DEVS_24GE_MAC  \
    CPSS_98DX3002_CNS,       \
    CPSS_98DX3031_CNS,       \
    CPSS_98DX3032_CNS,       \
    CPSS_98DX3033_CNS,       \
    CPSS_98DX3034_CNS,       \
    CPSS_98DX3035_CNS

/* PONcat2 devs 24GE */
#define PONCAT2_DEVS_24GE_MAC  \
    CPSS_98DX3031B_CNS,       \
    CPSS_98DX3133_CNS,        \
    CPSS_98DX3134_CNS,        \
    CPSS_98DX3135_CNS,        \
    CPSS_98DX3165_CNS,        \
    CPSS_98DX3136_CNS

#define PONCAT_DEVS_MAC \
    PONCAT_DEVS_24FE_MAC, /* the devices with up to 24FE+4stacking */\
    PONCAT_DEVS_24GE_MAC  /* the devices with up to 24GE+4stacking */

#define VONCAT_DEVS_24FE_MAC \
    CPSS_98DX1083_CNS,     \
    CPSS_98DX1085_CNS

#define VONCAT_DEVS_24GE_MAC \
    CPSS_98DX3083_CNS,      \
    CPSS_98DX3085_CNS

/* tomcat devs 24GE */
#define TOMCAT_DEVS_24GE_MAC \
    CPSS_98DX3101_CNS,       \
    /*    CPSS_98DX3121_CNS,  AS WA --> moved to bobcat list  */   \
    CPSS_98DX3121_1_CNS,     \
    CPSS_98DX3122_CNS,       \
    CPSS_98DX3123_CNS,       \
    CPSS_98DX3124_CNS,       \
    CPSS_98DX3125_CNS

/* tomcat devs 24FE */
#define TOMCAT_DEVS_24FE_MAC \
    CPSS_98DX1101_CNS,       \
    CPSS_98DX1122_CNS,       \
    CPSS_98DX1123_CNS

/* tomcat metro devs 24FE */
#define TOMCAT_METRO_DEVS_24FE_MAC \
    CPSS_98DX1222_CNS,       \
    CPSS_98DX1223_CNS

/* tomcat metro devs 24GE */
#define TOMCAT_METRO_DEVS_24GE_MAC \
    CPSS_98DX3201_CNS,       \
    CPSS_98DX3221_CNS,       \
    CPSS_98DX3222_CNS,       \
    CPSS_98DX3223_CNS,       \
    CPSS_98DX3224_CNS,       \
    CPSS_98DX3225_CNS

#define TOMCAT_DEVS_MAC \
    TOMCAT_DEVS_24GE_MAC,  /* the devices with up to 24GE+4stacking */\
    TOMCAT_DEVS_24FE_MAC   /* the devices with up to 24FE+4stacking */

/* DXH devs 24FE */
#define DXH_DEVS_24FE_MAC  \
    CPSS_98DXH125_CNS,     \
    CPSS_98DXH126_CNS,     \
    CPSS_98DXH127_CNS

/* DXH devs 24GE */
#define DXH_DEVS_24GE_MAC  \
    CPSS_98DXH325_CNS,     \
    CPSS_98DXH326_CNS,     \
    CPSS_98DXH327_CNS,     \
    CPSS_98DXH335_CNS,     \
    CPSS_98DXH336_CNS

#define DXH_DEVS_MAC \
    DXH_DEVS_24FE_MAC, /* the devices with up to 24FE+4stacking */\
    DXH_DEVS_24GE_MAC  /* the devices with up to 24GE+4stacking */

/* bobcat devs 24GE */
#define BOBCAT_DEVS_24GE_MAC \
    CPSS_98DX3110_CNS,       \
    CPSS_98DX3110_1_CNS,     \
    CPSS_98DX3121_CNS, /* AS WA --> this is Tomcat device that Temporary will identify as Bobcat*/      \
    CPSS_98DX4111_CNS,       \
    CPSS_98DX4121_CNS,       \
    CPSS_98DX4122_CNS,       \
    CPSS_98DX4123_CNS,       \
    CPSS_98DX4124_CNS,       \
    CPSS_98DX4125_CNS,       \
    CPSS_98DX5248_CNS,       \
    CPSS_98DX5258_CNS,       \
    CPSS_98DX5158_CNS,       \
    CPSS_98DX5159_CNS,       \
    CPSS_98DX5168_CNS,       \
    CPSS_98DXH525_CNS,       \
    CPSS_98DXH426_CNS,       \
    CPSS_98DX5313_CNS

/* bobcat devs 24FE */
#define BOBCAT_DEVS_24FE_MAC \
    CPSS_98DX2101_CNS,       \
    CPSS_98DX2122_CNS,       \
    CPSS_98DX2123_CNS,       \
    CPSS_98DX2151_CNS

#define BOBCAT_DEVS_MAC \
    BOBCAT_DEVS_24GE_MAC,  /* the devices with up to 24GE+4stacking */\
    BOBCAT_DEVS_24FE_MAC   /* the devices with up to 24FE+4stacking */

/* tomcat metro devs */
#define TOMCAT_METRO_DEVS_MAC \
    TOMCAT_METRO_DEVS_24FE_MAC,  /* the devices with up to 24FE+4stacking */\
    TOMCAT_METRO_DEVS_24GE_MAC   /* the devices with up to 24GE+4stacking */

/* XCAT Unit 5, restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC unit5RangesArray[] =
{
    {0x02900000 , 0x0290FFFC, 0x0, 0x0},
    {0x02C00000 , 0x02CFFFFC, 0x0, 0x0}
};

/* XCAT Unit 15, restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC unit15RangesArray[] =
{
    {0x07E40000 , 0x07FC0000, 0x0, 0x0}
};

/* XCAT Unit 17, restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC unit17RangesArray[] =
{
    {0x08800000 , 0x08801700, 0x0, 0x0},
    {0x08801C00 , 0x08803F00, 0x0, 0x0}
};

/* XCAT Unit 18, restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC unit18RangesArray[] =
{
    {0x09000000 , 0x092FFFF0, 0x0, 0x0},
    {0x09380000 , 0x097FFFFC, 0x0, 0x0}
};

/* XCAT Unit 19, restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC unit19RangesArray[] =
{
    {0x09800800 , 0x09800C00, 0x0, 0x0},
    {0x09801800 , 0x09801C00, 0x0, 0x0},
    {0x09802800 , 0x09802C00, 0x0, 0x0},
    {0x09803800 , 0x09803C00, 0x0, 0x0},
    {0x09804800 , 0x09804C00, 0x0, 0x0},
    {0x09805800 , 0x09805C00, 0x0, 0x0},
    {0x09806800 , 0x09806C00, 0x0, 0x0},
    {0x09807800 , 0x09807C00, 0x0, 0x0},
    {0x09808800 , 0x09808C00, 0x0, 0x0},
    {0x09809800 , 0x09809C00, 0x0, 0x0},
    {0x0980F800 , 0x0980FC00, 0x0, 0x0}
};

/* XCAT Unit 21, restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC unit21RangesArray[] =
{
    {0x0A807000 , 0x0A80F800, 0x0, 0x0}
};


/* Lion, Unit 7, restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC lionUnit7RangesArray[] =
{
    {0x03800000 , 0x0387FFFC, 0x0, 0x0}
};

/* Lion, Unit 14, restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC lionUnit14RangesArray[] =
{
    {0x07000000 , 0x0707FFFC, 0x0, 0x0}
};

/* Lion, Unit 20, restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC lionUnit20RangesArray[] =
{
    {0x0A000000 , 0x0A7FFFFC, 0x0, 0x0}
};


/* Lion, Unit 33, restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC lionUnit33RangesArray[] =
{
    {0x10800000 , 0x10FFFFFC, 0x0, 0x0}
};

/* Lion, Unit 35, restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC lionUnit35RangesArray[] =
{

    {0x11800000 , 0x11FFFFFC, 0x0, 0x0}
};

/* xCat2, Unit 3, restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC xCat2Unit3RangesArray[] =
{
    {0x01B40000, 0x01B40140, 0x0, 0x0},
    {0x01B40160, 0x01B40160, 0x0, 0x0},
    {0x01B40188, 0x01B7FFFC, 0x0, 0x0},
    {0x01C00000, 0x01CBFFFC, 0x0, 0x0},
    {0x01D00000, 0x01D3FFFC, 0x0, 0x0},
    {0x01DC0000, 0x01DFFFFC, 0x0, 0x0},
    {0x01E4000C, 0x01E7FFFC, 0x0FFC000F, 0x01E4000C}
};


/* xCat2, Unit 7, restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC xCat2Unit7RangesArray[] =
{
    {0x03801000, 0x038017FC, 0x0, 0x0},
    {0x03901000, 0x03F017FC, 0x0, 0x0}
};


/* xCat2, Unit 0xC, restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC xCat2Unit12RangesArray[] =
{
    {0x06000100, 0x063FFFFC, 0x0, 0x0},
    {0x06500100, 0x067FFFFC, 0x0, 0x0}
};


/* xCat2, Unit 15 (0x13), restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC xCat2Unit15RangesArray[] =
{
    {0x07E40000, 0x07FFFFFC, 0x0, 0x0}
};

/* xCat2, Unit 19 (0x13), restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC xCat2Unit19RangesArray[] =
{
    {0x09805000, 0x0980FFFC, 0x0, 0x0}
};


/* xCat2, Unit 21 (0x15), restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC xCat2Unit21RangesArray[] =
{
    {0x0A807000, 0x0A80FBFC, 0x0, 0x0}
};


/* xCat2, Unit 23 (0x17), restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC xCat2Unit23RangesArray[] =
{
    {0x0B800800, 0x0B800FFC, 0x0, 0x0},
    {0x0B840000, 0x0BF481FC, 0x0, 0x0}
};

/* xCat2, Unit 24 (0x18), restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC xCat2Unit24RangesArray[] =
{
    {0x0C000600, 0x0C0008FC, 0x0, 0x0},
    {0x0C001800, 0x0C001FFC, 0x0, 0x0},
    {0x0C006000, 0x0C006FFC, 0x0, 0x0},
    {0x0C020000, 0x0C02106C, 0x0, 0x0},
    {0x0C100600, 0x0C72106C, 0x0, 0x0}
};


/* xCat2, Unit 25 (0x19), restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC xCat2Unit25RangesArray[] =
{
    {0x0C801000, 0x0C8017FC, 0x0, 0x0},
    {0x0C901000, 0x0CF017FC, 0x0, 0x0}
};


/* xCat2, Unit 26 (0x1A), restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC xCat2Unit26RangesArray[] =
{
    {0x0D001000, 0x0D0017FC, 0x0, 0x0},
    {0x0D101000, 0x0D7017FC, 0x0, 0x0}
};

/* xCat2, Unit 28 (0x1C), restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC xCat2Unit28RangesArray[] =
{
    {0x0E000400, 0x0E0010FC, 0x0, 0x0},
    {0x0E002000, 0x0E0027FC, 0x0, 0x0},
    {0x0E100400, 0x0E7027FC, 0x0, 0x0}
};


/* xCat2, Unit 29 (0x1D), restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC xCat2Unit29RangesArray[] =
{
    {0x0E800100, 0x0E8027FC, 0x0, 0x0},
    {0x0E900100, 0x0EF027FC, 0x0, 0x0}
};

/* xCat2, Unit 30 (0x1E), restricted ranges */
static PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC xCat2Unit30RangesArray[] =
{
    {0x0F018000, 0x0F0187FC, 0x0, 0x0},
    {0x0F118000, 0x0F7187FC, 0x0, 0x0}
};

/* definition for empty (restricted addresses) unit ranges */
#define EMPTY_UNIT_MAC  {0,NULL}

/* definition for total number of units */
#define UNITS_NUM_CNS   64

/* Units bad addresses ranges array for XCAT */
static PRV_CPSS_DXCH_ERRATA_UNIT_RANGES_STC xcatBadAddressRanges[UNITS_NUM_CNS];


/* Units bad addresses ranges array for XCAT, Lion */
static PRV_CPSS_DXCH_ERRATA_UNIT_RANGES_STC
        lionBadAddressRanges[CPSS_MAX_PORT_GROUPS_CNS][UNITS_NUM_CNS];

/* Units bad addresses ranges array for xCat2 */
static PRV_CPSS_DXCH_ERRATA_UNIT_RANGES_STC xCat2BadAddressRanges[UNITS_NUM_CNS];

/*
 * typedef: struct FINE_TUNING_STC
 *
 * Description:
 *     structure for the devices flavors, about table sizes
 *
 * Fields:
 *      devTypePtr   - (pointer to) device type
 *      config       - device parameters being object of fine tuning
 *
 * Comment:
 */
typedef struct
{
    CPSS_PP_DEVICE_TYPE                     *devTypePtr;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC config;
}FINE_TUNING_STC;


/*
 * typedef: struct BUFFER_MANAGEMENT_PARAMS_STC
 *
 * Description:
 *     structure for buffer,descriptors sizes/numbers
 *
 * Fields:
 *      bufferMemory   - Buffer Memory
 *      transmitDescr  - Transmit Descriptors
 * Comment:
 */
typedef struct
{
    GT_U32 bufferMemory;
    GT_U32 transmitDescr;
}BUFFER_MANAGEMENT_PARAMS_STC;

/*
 * typedef: struct FINE_TUNING_BUFFER_MANAGEMENT_PARAMS_STC
 *
 * Description:
 *     structure for the devices flavors,
 *     about buffer,descriptors sizes/numbers
 *
 * Fields:
 *      devTypePtr   - (pointer to) device type
 *      config       - buffer,descriptors sizes/numbers
 *
 * Comment:
 */
typedef struct
{
    CPSS_PP_DEVICE_TYPE             *devTypePtr;

    BUFFER_MANAGEMENT_PARAMS_STC    config;
}FINE_TUNING_BUFFER_MANAGEMENT_PARAMS_STC;

/* for those devices set 8M buffers */
static CPSS_PP_DEVICE_TYPE devOverrideDefaultTo_8M_buffers[]=
        {
            /* "GE AlleyCat WebSmart"  */
            CPSS_98DX3026_CNS,
            CPSS_98DX3027_CNS,
            /* add here other families */

            LAST_DEV_IN_LIST_CNS /* must be last one */
        };

/* array for 'Override buffers' numbers */
static FINE_TUNING_BUFFER_MANAGEMENT_PARAMS_STC dxChXcatBufferMemory[]=
{
    {  /* 8M buffers */
        devOverrideDefaultTo_8M_buffers,
        {
            _8M  /* bufferMemory */
           ,AUTO_CALC_FIELD_CNS /* transmitDescr -- calc from buffer memory size */
        }
    }
};

static GT_U32 dxChXcatBufferMemory_size = sizeof(dxChXcatBufferMemory)/sizeof(dxChXcatBufferMemory[0]);

/* list of the alleycat unmanaged devices */
static CPSS_PP_DEVICE_TYPE devs_alleycat_unmanaged[]=
        {ALLEYCAT_UNMANAGED_DEVS_MAC,

         LAST_DEV_IN_LIST_CNS /* must be last one */
         };

/* list of the alleycat websmart devices */
static CPSS_PP_DEVICE_TYPE devs_alleycat_websmart[]=
        {
            ALLEYCAT_WEBSMART_DEVS_MAC,

            LAST_DEV_IN_LIST_CNS /* must be last one */
        };


/* list of the alleycat2 websmart devices */
static CPSS_PP_DEVICE_TYPE devs_alleycat2_websmart[]=
        {
            ALLEYCAT2_WEBSMART_DEVS_MAC,

            LAST_DEV_IN_LIST_CNS /* must be last one */
        };

/* list of the poncat devices */
static CPSS_PP_DEVICE_TYPE devs_poncat[]=
        {PONCAT_DEVS_MAC,

         LAST_DEV_IN_LIST_CNS /* must be last one */
         };

/* list of the voncat and poncat2 devices */
static CPSS_PP_DEVICE_TYPE devs_voncat_poncat2[]=
        {
            VONCAT_DEVS_24FE_MAC,
            VONCAT_DEVS_24GE_MAC,
            PONCAT2_DEVS_24FE_MAC,
            PONCAT2_DEVS_24GE_MAC,

            LAST_DEV_IN_LIST_CNS /* must be last one */
        };

/* list of the tomcat devices */
static CPSS_PP_DEVICE_TYPE devs_tomcat[]=
        {TOMCAT_DEVS_MAC,

         LAST_DEV_IN_LIST_CNS /* must be last one */
         };

/* list of the dxh devices */
static CPSS_PP_DEVICE_TYPE devs_dxh[]=
        {DXH_DEVS_MAC,

         LAST_DEV_IN_LIST_CNS /* must be last one */
         };

/* list of the bobcat devices */
static CPSS_PP_DEVICE_TYPE devs_bobcat[]=
        {BOBCAT_DEVS_MAC,

         LAST_DEV_IN_LIST_CNS /* must be last one */
         };

/* list of the tomcat metro devices */
static CPSS_PP_DEVICE_TYPE devs_tomcat_metro[]=
        {TOMCAT_METRO_DEVS_MAC,

         LAST_DEV_IN_LIST_CNS /* must be last one */
         };

/* xCat, xCat2 - fine tuning table  sizes according to devices flavors */
static FINE_TUNING_STC dxChXcatTables[]=
{

    { /* start "AlleyCat Unmanaged" */
        devs_alleycat_unmanaged,
        {
            NULL,
            {
                /* bridge section                                             */
                _8K,            /* GT_U32  fdb;                               */
                NA_TABLE_CNS,   /* vidx                                       */
                NA_TABLE_CNS,   /* stgNum                                     */
                                /*                                            */
                /* tti section                                                */
                NA_TABLE_CNS,   /* GT_U32 routerAndTunnelTermTcam;            */
                                /*                                            */
                /* tunnel section                                             */
                NA_TABLE_CNS,   /* GT_U32  tunnelStart;                       */
                NA_TABLE_CNS,   /* GT_U32  routerArp;                         */
                                /*                                            */
                /* ip section                                                 */
                NA_TABLE_CNS,   /* GT_U32  routerNextHop;                     */
                NA_TABLE_CNS,   /* GT_U32  mllPairs;                          */
                                /*                                            */
                /* PCL section                                                */
                NA_TABLE_CNS,   /* GT_U32 policyTcamRaws;                     */
                                /*                                            */
                /* CNC section                                                */
                NA_TABLE_CNS,   /* GT_U32 cncBlocks;                          */
                NA_TABLE_CNS,   /* GT_U32 cncBlockNumEntries;                 */
                                /*                                            */
                NA_TABLE_CNS,   /* policersNum                                */
                NA_TABLE_CNS,   /* egressPolicersNum                          */
                NA_TABLE_CNS,   /* trunksNum                                  */
                /* Transmit Descriptors                                       */
                _2K,            /* GT_U32 transmitDescr;                      */
                                /*                                            */
                /* Buffer Memory                                              */
                _4M,            /* GT_U32 bufferMemory;                       */
                TX_QUEUE_NUM_4_CNS /* txQueuesNum - TX Queues number          */
            },

            {
                /* TR101 Feature support                                      */
                GT_FALSE, /*tr101Supported*/

                /* VLAN translation support                                   */
                GT_FALSE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag                  */
                GT_FALSE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_FALSE /*trunkCrcHashSupported*/
            }
        }
    }    /* end "AlleyCat Unmanaged" */
    ,
    { /* start "AlleyCat WebSmart" */
        devs_alleycat_websmart,
        {
            NULL,
            {
                /* bridge section                                             */
                _8K,            /* GT_U32  fdb;                               */
                256,            /* vidx                                       */
                32,             /* stgNum                                     */
                                /*                                            */
                /* tti section                                                */
                NA_TABLE_CNS,   /* GT_U32 routerAndTunnelTermTcam;            */
                                /*                                            */
                /* tunnel section                                             */
                NA_TABLE_CNS,   /* GT_U32  tunnelStart;                       */
                NA_TABLE_CNS,   /* GT_U32  routerArp;                         */
                                /*                                            */
                /* ip section                                                 */
                NA_TABLE_CNS,   /* GT_U32  routerNextHop;                     */
                NA_TABLE_CNS,   /* GT_U32  mllPairs;                          */
                                /*                                            */
                /* PCL section                                                */
                512/4,          /* GT_U32 policyTcamRaws;                     */
                                /*                                            */
                /* CNC section                                                */
                NA_TABLE_CNS,   /* GT_U32 cncBlocks;                          */
                NA_TABLE_CNS,   /* GT_U32 cncBlockNumEntries;                 */
                                /*                                            */
                256,            /* policersNum                                */
                NA_TABLE_CNS,   /* egressPolicersNum                          */
                32,             /* trunksNum                                  */
                /* Transmit Descriptors                                       */
                _2K,            /* GT_U32 transmitDescr;                      */
                                /*                                            */
                /* Buffer Memory                                              */
                _4M,/* for GE devices uses 8M --> devOverrideDefaultTo_8M_buffers *//* GT_U32 bufferMemory;*/
                TX_QUEUE_NUM_4_CNS /* txQueuesNum - TX Queues number          */
            },

            {
                /* TR101 Feature support                                      */
                GT_FALSE, /*tr101Supported*/

                /* VLAN translation support                                   */
                GT_FALSE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag                  */
                GT_FALSE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_FALSE /*trunkCrcHashSupported*/
            }
        }
    }    /* end "AlleyCat WebSmart" */
    ,
    { /* start "AlleyCat2 WebSmart" */
        devs_alleycat2_websmart,
        {
            NULL,
            {
                /* bridge section                                             */
                _16K,            /* GT_U32  fdb;                               */
                256,            /* vidx                                       */
                32,             /* stgNum                                     */
                                /*                                            */
                /* tti section                                                */
                NA_TABLE_CNS,   /* GT_U32 routerAndTunnelTermTcam;            */
                                /*                                            */
                /* tunnel section                                             */
                NA_TABLE_CNS,   /* GT_U32  tunnelStart;                       */
                _1K,            /* GT_U32  routerArp;                         */
                                /*                                            */
                /* ip section                                                 */
                NA_TABLE_CNS,   /* GT_U32  routerNextHop;                     */
                NA_TABLE_CNS,   /* GT_U32  mllPairs;                          */
                                /*                                            */
                /* PCL section                                                */
                512/4,          /* GT_U32 policyTcamRaws;                     */
                                /*                                            */
                /* CNC section                                                */
                NA_TABLE_CNS,   /* GT_U32 cncBlocks;                          */
                NA_TABLE_CNS,   /* GT_U32 cncBlockNumEntries;                 */
                                /*                                            */
                256,            /* policersNum                                */
                NA_TABLE_CNS,   /* egressPolicersNum                          */
                32,             /* trunksNum                                  */
                /* Transmit Descriptors                                       */
                _4K,            /* GT_U32 transmitDescr;                      */
                                /*                                            */
                /* Buffer Memory                                              */
                _8M,            /* GT_U32 bufferMemory;                       */
                TX_QUEUE_NUM_4_CNS /* txQueuesNum - TX Queues number          */
            },

            {
                /* TR101 Feature support                                      */
                GT_FALSE, /*tr101Supported*/

                /* VLAN translation support                                   */
                GT_FALSE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag                  */
                GT_FALSE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_FALSE /*trunkCrcHashSupported*/
            }
        }
    }    /* end "AlleyCat2 WebSmart" */
    ,
    { /* start "PONCat" */
        devs_poncat,
        {
            NULL,
            {
                /* bridge section                                             */
                _16K,            /* GT_U32  fdb;                               */
                _1K,            /* vidx                                       */
                STG_NUM_CNS,    /* stgNum                                     */
                                /*                                            */
                /* tti section                                                */
                _1K,            /* GT_U32 routerAndTunnelTermTcam;            */
                                /*                                            */
                /* tunnel section                                             */
                NA_TABLE_CNS,   /* GT_U32  tunnelStart;                       */
                _1K,            /* GT_U32  routerArp;                         */
                                /*                                            */
                /* ip section                                                 */
                NA_TABLE_CNS,   /* GT_U32  routerNextHop;                     */
                NA_TABLE_CNS,   /* GT_U32  mllPairs;                          */
                                /*                                            */
                /* PCL section                                                */
                _1K/4,          /* GT_U32 policyTcamRaws;                     */
                                /*                                            */
                /* CNC section                                                */
                CNC_AUTO_CALC,  /* GT_U32 cncBlocks;                          */
                _2K,            /* GT_U32 cncBlockNumEntries;                 */
                                /*                                            */
                (256+28),            /* policersNum                                */
                128,            /* egressPolicersNum                          */
                TRUNKS_NUM_CNS, /* trunksNum                                  */
                /* Transmit Descriptors                                       */
                _4K,            /* GT_U32 transmitDescr;                      */
                                /*                                            */
                /* Buffer Memory                                              */
                _8M,            /* GT_U32 bufferMemory;                       */
                TX_QUEUE_NUM_8_CNS /* txQueuesNum - TX Queues number          */
            },

            {
                /* TR101 Feature support                                      */
                GT_TRUE, /*tr101Supported*/

                /* VLAN translation support                                   */
                GT_TRUE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag                  */
                GT_TRUE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_FALSE /*trunkCrcHashSupported*/
            }
        }
    }    /* end "PONCat" */
    ,
    { /* start "VonCat && PonCat2" */
        devs_voncat_poncat2,
        {
            NULL,
            {
                /* bridge section                                             */
                _16K,           /* GT_U32  fdb;                               */
                _1K,            /* vidx                                       */
                STG_NUM_CNS,    /* stgNum                                     */
                                /*                                            */
                /* tti section                                                */
                _1K,            /* GT_U32 routerAndTunnelTermTcam;            */
                                /*                                            */
                /* tunnel section                                             */
                256,            /* GT_U32  tunnelStart;                       */
                _1K,            /* GT_U32  routerArp;                         */
                                /*                                            */
                /* ip section                                                 */
                NA_TABLE_CNS,   /* GT_U32  routerNextHop;  PBR only           */
                NA_TABLE_CNS,   /* GT_U32  mllPairs;                          */
                                /*                                            */
                /* PCL section                                                */
                _1K/4,          /* GT_U32 policyTcamRaws;                     */
                                /*                                            */
                /* CNC section                                                */
                CNC_AUTO_CALC,  /* GT_U32 cncBlocks;                          */
                _2K,            /* GT_U32 cncBlockNumEntries;                 */
                                /*                                            */
                (256+28),       /* policersNum                                */
                128,            /* egressPolicersNum                          */
                127,            /* trunksNum                                  */
                /* Transmit Descriptors                                       */
                _4K,            /* GT_U32 transmitDescr;                      */
                                /*                                            */
                /* Buffer Memory                                              */
                _8M,            /* GT_U32 bufferMemory;                       */
                TX_QUEUE_NUM_8_CNS /* txQueuesNum - TX Queues number          */
            },

            {
                /* TR101 Feature support                                      */
                GT_TRUE, /*tr101Supported*/

                /* VLAN translation support                                   */
                GT_TRUE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag                  */
                GT_TRUE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_FALSE /*trunkCrcHashSupported*/
            }
        }
    }    /* end "VonCat && PonCat2" */
    ,
    { /* start "dxh" */
        devs_dxh,
        {
            NULL,
            {
                /* bridge section                                             */
                _8K,            /* GT_U32  fdb;                               */
                _1K,            /* vidx                                       */
                32,             /* stgNum                                     */
                                /*                                            */
                /* tti section                                                */
                NA_TABLE_CNS,   /* GT_U32 routerAndTunnelTermTcam;            */
                                /*                                            */
                /* tunnel section                                             */
                NA_TABLE_CNS,   /* GT_U32  tunnelStart;                       */
                256,            /* GT_U32  routerArp;                         */
                                /*                                            */
                /* ip section                                                 */
                NA_TABLE_CNS,   /* GT_U32  routerNextHop;                     */
                NA_TABLE_CNS,   /* GT_U32  mllPairs;                          */
                                /*                                            */
                /* PCL section                                                */
                _1K/4,          /* GT_U32 policyTcamRaws;                     */
                                /*                                            */
                /* CNC section                                                */
                CNC_AUTO_CALC,  /* GT_U32 cncBlocks;                          */
                _2K,            /* GT_U32 cncBlockNumEntries;                 */
                                /*                                            */
                128,            /* policersNum                                */
                64,             /* egressPolicersNum                          */
                31,             /* trunksNum                                  */
                                /*                                            */
                /* Transmit Descriptors                                       */
                _2K,            /* GT_U32 transmitDescr;                      */
                                /*                                            */
                /* Buffer Memory                                              */
                _4M,            /* GT_U32 bufferMemory;                       */
                TX_QUEUE_NUM_4_CNS /* txQueuesNum - TX Queues number          */
            },

            {
                /* TR101 Feature support                                      */
                GT_FALSE, /*tr101Supported*/

                /* VLAN translation support                                   */
                GT_FALSE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag                  */
                GT_FALSE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_FALSE /*trunkCrcHashSupported*/
            }
        }
    }    /* end "dxh" */
    ,
    { /* start "TomCat L2+" */
        devs_tomcat,
        {
            NULL,
            {
                /* bridge section                                             */
                _8K,            /* GT_U32  fdb;                               */
                _1K,            /* vidx                                       */
                STG_NUM_CNS,    /* stgNum                                     */
                                /*                                            */
                /* tti section                                                */
                NA_TABLE_CNS,   /* GT_U32 routerAndTunnelTermTcam;            */
                                /*                                            */
                /* tunnel section                                             */
                NA_TABLE_CNS,   /* GT_U32  tunnelStart;                       */
                _1K,            /* GT_U32  routerArp;                         */
                                /*                                            */
                /* ip section                                                 */
                _1K,            /* GT_U32  routerNextHop;                     */
                NA_TABLE_CNS,   /* GT_U32  mllPairs;                          */
                                /*                                            */
                /* PCL section                                                */
                _2K/4,          /* GT_U32 policyTcamRaws;                     */
                                /*                                            */
                /* CNC section                                                */
                CNC_AUTO_CALC,  /* GT_U32 cncBlocks;                          */
                _2K,            /* GT_U32 cncBlockNumEntries;                 */
                                /*                                            */
                512,            /* policersNum                                */
                NA_TABLE_CNS,   /* egressPolicersNum                          */
                TRUNKS_NUM_CNS, /* trunksNum                                  */
                /* Transmit Descriptors                                       */
                _4K,            /* GT_U32 transmitDescr;                      */
                                /*                                            */
                /* Buffer Memory                                              */
                _8M,            /* GT_U32 bufferMemory;                       */
                TX_QUEUE_NUM_8_CNS  /* txQueuesNum - TX Queues number         */
            },

            {
                /* TR101 Feature support                                      */
                GT_FALSE, /*tr101Supported*/

                /* VLAN translation support                                   */
                GT_FALSE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag                  */
                GT_FALSE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_FALSE /*trunkCrcHashSupported*/
            }
        }
    }    /* end "TomCat L2+" */
    ,
    { /* start "BobCat L3+/Metro" */
        devs_bobcat,
        {
            NULL,
            {
                /* bridge section                                             */
                _16K,           /* GT_U32  fdb;                               */
                VIDX_NUM_CNS,   /* vidx                                       */
                STG_NUM_CNS,    /* stgNum                                     */
                                /*                                            */
                /* tti section                                                */
                _13K/4,         /* GT_U32 routerAndTunnelTermTcam;            */
                                /*                                            */
                /* tunnel section                                             */
                _1K,            /* GT_U32  tunnelStart;                       */
                AUTO_CALC_FIELD_CNS,/* GT_U32  routerArp;                     */
                                /*                                            */
                /* ip section                                                 */
                _4K,            /* GT_U32  routerNextHop;                     */
                _4K/2,          /* GT_U32  mllPairs;                          */
                                /*                                            */
                /* PCL section                                                */
                _3K/4,          /* GT_U32 policyTcamRaws;                     */
                                /*                                            */
                /* CNC section                                                */
                CNC_AUTO_CALC,  /* GT_U32 cncBlocks;                          */
                _4K,            /* GT_U32 cncBlockNumEntries;                 */
                                /*                                            */
                _2K,            /* policersNum                                */
                512,            /* egressPolicersNum                          */
                TRUNKS_NUM_CNS, /* trunksNum                                  */
                /* Transmit Descriptors                                       */
                _6K,            /* GT_U32 transmitDescr;                      */
                                /*                                            */
                /* Buffer Memory                                              */
                _12M,           /* GT_U32 bufferMemory;                       */
                TX_QUEUE_NUM_8_CNS  /* txQueuesNum - TX Queues number         */
            },

            {
                /* TR101 Feature support                                      */
                GT_TRUE, /*tr101Supported*/

                /* VLAN translation support                                   */
                GT_TRUE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag                  */
                GT_TRUE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_FALSE /*trunkCrcHashSupported*/
            }
        }
    }    /* end "BobCat L3+/Metro" */
    ,
    { /* start "TomCat Metro" */
        devs_tomcat_metro,
        {
            NULL,
            {
                /* bridge section                                             */
                _16K,           /* GT_U32  fdb;                               */
                VIDX_NUM_CNS,   /* vidx                                       */
                STG_NUM_CNS,    /* stgNum                                     */
                                /*                                            */
                /* tti section                                                */
                _2K,            /* GT_U32 routerAndTunnelTermTcam;            */
                                /*                                            */
                /* tunnel section                                             */
                _1K,            /* GT_U32  tunnelStart;                       */
                _2K,            /* GT_U32  routerArp;                         */
                                /*                                            */
                /* ip section                                                 */
                _2K,            /* GT_U32  routerNextHop;                     */
                NA_TABLE_CNS,   /* GT_U32  mllPairs;                          */
                                /*                                            */
                /* PCL section                                                */
                _3K/4,          /* GT_U32 policyTcamRaws;                     */
                                /*                                            */
                /* CNC section                                                */
                CNC_AUTO_CALC,  /* GT_U32 cncBlocks;                          */
                _4K,            /* GT_U32 cncBlockNumEntries;                 */
                                /*                                            */
                _2K,            /* policersNum                                */
                512,            /* egressPolicersNum                          */
                TRUNKS_NUM_CNS,   /* trunksNum                                */
                /* Transmit Descriptors                                       */
                _6K,            /* GT_U32 transmitDescr;                      */
                                /*                                            */
                /* Buffer Memory                                              */
                _12M,           /* GT_U32 bufferMemory;                       */
                TX_QUEUE_NUM_8_CNS  /* txQueuesNum - TX Queues number         */
            },

            {
                /* TR101 Feature support                                      */
                GT_TRUE, /*tr101Supported*/

                /* VLAN translation support                                   */
                GT_TRUE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag                  */
                GT_TRUE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_FALSE /*trunkCrcHashSupported*/
            }
        }
    }    /* end "TomCat Metro" */
};

static GT_U32 dxChXcatTables_size = sizeof(dxChXcatTables)/sizeof(dxChXcatTables[0]);

/* list of the lion group 0 devices */
static CPSS_PP_DEVICE_TYPE devs_Lion_group0[]=
        {
            CPSS_98CX8265_CNS,
            CPSS_98CX8266_CNS,
            CPSS_98CX8267_CNS,

            LAST_DEV_IN_LIST_CNS /* must be last one */
         };

/* list of the Lion-L2 Street Fighter/ATCA devices */
static CPSS_PP_DEVICE_TYPE devs_Lion_L2StreetFighter[]=
        {
            CPSS_98CX8113_CNS,

            LAST_DEV_IN_LIST_CNS /* must be last one */
         };

/* list of the Lion-DC/Metro Data Center/Metro devices */
static CPSS_PP_DEVICE_TYPE devs_Lion_DC_metro[]=
        {
            CPSS_98CX8248_1_CNS,
            CPSS_98CX8224_1_CNS,
            CPSS_98CX8203_CNS,

            LAST_DEV_IN_LIST_CNS /* must be last one */
         };

/* list of the Lion Metro GPON devices */
static CPSS_PP_DEVICE_TYPE devs_Lion_metro_GPON[]=
        {
            CPSS_98CX8248_CNS,
            CPSS_98CX8224_CNS,

            LAST_DEV_IN_LIST_CNS /* must be last one */
         };

/* list of the Lion-40G tier1/40GbE devices */
static CPSS_PP_DEVICE_TYPE devs_Lion_40G[]=
        {
            CPSS_98CX8222_CNS,
            CPSS_98CX8223_CNS,
            CPSS_98CX8234_CNS,

            LAST_DEV_IN_LIST_CNS /* must be last one */
         };

/* list of the Lion-Ch5-Metro */
static CPSS_PP_DEVICE_TYPE devs_Lion_Ch5_Metro[]=
        {
            CPSS_98DX5198_CNS,

            LAST_DEV_IN_LIST_CNS /* must be last one */
         };

/* list of the Lion-Ch5-Enterprise devices */
static CPSS_PP_DEVICE_TYPE devs_Lion_Ch5_Enterprise[]=
        {
            CPSS_98DX5178_CNS,

            LAST_DEV_IN_LIST_CNS /* must be last one */
         };

/* list of the Lion-Crossbar devices */
static CPSS_PP_DEVICE_TYPE devs_Lion_Crossbar[]=
        {
            CPSS_98FX9216_CNS,

            LAST_DEV_IN_LIST_CNS /* must be last one */
         };

/* Lion - fine tuning table  sizes according to devices flavors */
static FINE_TUNING_STC lionTables[]=
{
    { /* Lion-L2 Street Fighter/ATCA devices */
        devs_Lion_L2StreetFighter,
        {
            NULL,  /*enhancedInfoPtr*/
            {
                /* bridge section                             */
                _16K,            /*GT_U32  fdb;               */
                VIDX_NUM_CNS,    /*vidx                       */
                STG_NUM_CNS,     /*stgNum                     */
                                 /*                           */
                /* tti section                                */
                NA_TABLE_CNS,    /*GT_U32 routerAndTunnelTermTcam;*/
                                 /*                           */
                /* tunnel section                             */
                NA_TABLE_CNS,    /*GT_U32  tunnelStart;       */
                NA_TABLE_CNS,    /*GT_U32  routerArp;         */
                                 /*                           */
                /* ip section                                 */
                NA_TABLE_CNS,    /*GT_U32  routerNextHop;     */
                NA_TABLE_CNS,    /*GT_U32  mllPairs;          */
                                 /*                           */
                /* PCL section                                */
                _3K/4,           /*GT_U32 policyTcamRaws;     */
                                 /*                           */
                /* CNC section                                */
                CNC_AUTO_CALC,  /* GT_U32 cncBlocks;                          */
                _1K,             /*GT_U32 cncBlockNumEntries; */
                /*  Policer Section                           */
                _4K/4,           /*policersNum                */
                NA_TABLE_CNS,    /*egressPolicersNum          */
                                 /*                           */
                /* trunk Section                              */
                TRUNKS_NUM_CNS,  /*trunksNum                  */
                                 /*                           */
                /* Transmit Descriptors                       */
                _2K,             /*GT_U32 transmitDescr;      */
                                 /*                           */
                /* Buffer Memory                              */
                _4M,             /*GT_U32 bufferMemory;       */
                TX_QUEUE_NUM_8_CNS  /* txQueuesNum - TX Queues number*/
            },

            {
                /* TR101 Feature support */
                GT_FALSE, /*tr101Supported*/

                /* VLAN translation support */
                GT_TRUE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag                  */
                GT_TRUE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_FALSE /*trunkCrcHashSupported*/
            }
        }
    },    /* end "Lion-L2 Street Fighter/ATCA devices" */

    { /* Lion-DC/Metro Data Center/Metro devices */
        devs_Lion_DC_metro,
        {
            NULL,  /*enhancedInfoPtr*/
            {
                /* bridge section                             */
                _32K,            /*GT_U32  fdb;               */
                VIDX_NUM_CNS,    /*vidx                       */
                STG_NUM_CNS,     /*stgNum                     */
                                 /*                           */
                /* tti section                                */
                _16K/4,          /*GT_U32 routerAndTunnelTermTcam;*/
                                 /*                           */
                /* tunnel section                             */
                _8K/4,           /*GT_U32  tunnelStart;       */
                AUTO_CALC_FIELD_CNS,/*GT_U32  routerArp;      */
                                 /*                           */
                /* ip section                                 */
                _8K,             /*GT_U32  routerNextHop;     */
                _2K,             /*GT_U32  mllPairs;          */
                                 /*                           */
                /* PCL section                                */
                _3K/4,           /*GT_U32 policyTcamRaws;     */
                                 /*                           */
                /* CNC section                                */
                CNC_AUTO_CALC,  /* GT_U32 cncBlocks;                          */
                _1K,             /*GT_U32 cncBlockNumEntries; */
                /*  Policer Section                           */
                _2K/4,           /*policersNum                */
                512/4,           /*egressPolicersNum          */
                                 /*                           */
                /* trunk Section                              */
                TRUNKS_NUM_CNS,  /*trunksNum                  */
                                 /*                           */
                /* Transmit Descriptors                       */
                _2K,             /*GT_U32 transmitDescr;      */
                                 /*                           */
                /* Buffer Memory                              */
                _8M,             /*GT_U32 bufferMemory;       */
                TX_QUEUE_NUM_8_CNS  /* txQueuesNum - TX Queues number*/
            },

            {
                /* TR101 Feature support */
                GT_TRUE, /*tr101Supported*/

                /* VLAN translation support */
                GT_TRUE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag                  */
                GT_TRUE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_TRUE /*trunkCrcHashSupported*/
            }
        }
    },    /* end "Lion-DC/Metro Data Center/Metro devices" */

    { /* Lion Metro GPON */
        devs_Lion_metro_GPON,
        {
            NULL,  /*enhancedInfoPtr*/
            {
                /* bridge section                             */
                _32K,            /*GT_U32  fdb;               */
                VIDX_NUM_CNS,    /*vidx                       */
                STG_NUM_CNS,     /*stgNum                     */
                                 /*                           */
                /* tti section                                */
                _16K/4,          /*GT_U32 routerAndTunnelTermTcam;*/
                                 /*                           */
                /* tunnel section                             */
                _8K/4,           /*GT_U32  tunnelStart;       */
                AUTO_CALC_FIELD_CNS,/*GT_U32  routerArp;      */
                                 /*                           */
                /* ip section                                 */
                _8K,             /*GT_U32  routerNextHop;     */
                _2K,             /*GT_U32  mllPairs;          */
                                 /*                           */
                /* PCL section                                */
                _3K/4,           /*GT_U32 policyTcamRaws;     */
                                 /*                           */
                /* CNC section                                */
                CNC_AUTO_CALC,  /* GT_U32 cncBlocks;                          */
                _4K,             /*GT_U32 cncBlockNumEntries; */
                /*  Policer Section                           */
                _8K/4,           /*policersNum                */
                _2K/4,           /*egressPolicersNum          */
                                 /*                           */
                /* trunk Section                              */
                TRUNKS_NUM_CNS,  /*trunksNum                  */
                                 /*                           */
                /* Transmit Descriptors                       */
                _2K,             /*GT_U32 transmitDescr;      */
                                 /*                           */
                /* Buffer Memory                              */
                _8M,             /*GT_U32 bufferMemory;       */
                TX_QUEUE_NUM_8_CNS  /* txQueuesNum - TX Queues number*/
            },

            {
                /* TR101 Feature support */
                GT_TRUE, /*tr101Supported*/

                /* VLAN translation support */
                GT_TRUE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag                  */
                GT_TRUE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_FALSE /*trunkCrcHashSupported*/
            }
        }
    },    /* end "Lion Metro GPON devices" */

    { /* Lion-40G tier1/40GbE devices */
        devs_Lion_40G,
        {
            NULL,  /*enhancedInfoPtr*/
            {
                /* bridge section                             */
                _32K,            /*GT_U32  fdb;               */
                VIDX_NUM_CNS,    /*vidx                       */
                STG_NUM_CNS,     /*stgNum                     */
                                 /*                           */
                /* tti section                                */
                _16K/4,          /*GT_U32 routerAndTunnelTermTcam;*/
                                 /*                           */
                /* tunnel section                             */
                _8K/4,           /*GT_U32  tunnelStart;       */
                AUTO_CALC_FIELD_CNS,/*GT_U32  routerArp;      */
                                 /*                           */
                /* ip section                                 */
                _8K,             /*GT_U32  routerNextHop;     */
                _2K,             /*GT_U32  mllPairs;          */
                                 /*                           */
                /* PCL section                                */
                _3K/4,           /*GT_U32 policyTcamRaws;     */
                                 /*                           */
                /* CNC section                                */
                CNC_AUTO_CALC,  /* GT_U32 cncBlocks;                          */
                _4K,             /*GT_U32 cncBlockNumEntries; */
                /*  Policer Section                           */
                _8K/4,           /*policersNum                */
                _2K/4,           /*egressPolicersNum          */
                                 /*                           */
                /* trunk Section                              */
                TRUNKS_NUM_CNS,  /*trunksNum                  */
                                 /*                           */
                /* Transmit Descriptors                       */
                _2K,             /*GT_U32 transmitDescr;      */
                                 /*                           */
                /* Buffer Memory                              */
                _8M,             /*GT_U32 bufferMemory;       */
                TX_QUEUE_NUM_8_CNS  /* txQueuesNum - TX Queues number*/
            },

            {
                /* TR101 Feature support */
                GT_TRUE, /*tr101Supported*/

                /* VLAN translation support */
                GT_TRUE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag                  */
                GT_TRUE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_TRUE /*trunkCrcHashSupported*/
            }
        }
    },    /* end "Lion-40G tier1/40GbE devices" */

    { /* lion group 0 devices */
        devs_Lion_group0,
        {
            NULL,  /*enhancedInfoPtr*/
            {
                /* bridge section                             */
                _32K,            /*GT_U32  fdb;               */
                VIDX_NUM_CNS,    /*vidx                       */
                STG_NUM_CNS,     /*stgNum                     */
                                 /*                           */
                /* tti section                                */
                _16K/4,          /*GT_U32 routerAndTunnelTermTcam;*/
                                 /*                           */
                /* tunnel section                             */
                _8K/4,           /*GT_U32  tunnelStart;       */
                AUTO_CALC_FIELD_CNS,/*GT_U32  routerArp;      */
                                 /*                           */
                /* ip section                                 */
                _8K,             /*GT_U32  routerNextHop;     */
                _2K,             /*GT_U32  mllPairs;          */
                                 /*                           */
                /* PCL section                                */
                _3K/4,           /*GT_U32 policyTcamRaws;     */
                                 /*                           */
                /* CNC section                                */
                CNC_AUTO_CALC,  /* GT_U32 cncBlocks;                          */
                _4K,             /*GT_U32 cncBlockNumEntries; */
                /*  Policer Section                           */
                _8K/4,           /*policersNum                */
                _2K/4,           /*egressPolicersNum          */
                                 /*                           */
                /* trunk Section                              */
                TRUNKS_NUM_CNS,  /*trunksNum                  */
                                 /*                           */
                /* Transmit Descriptors                       */
                _2K,             /*GT_U32 transmitDescr;      */
                                 /*                           */
                /* Buffer Memory                              */
                _8M,             /*GT_U32 bufferMemory;       */
                TX_QUEUE_NUM_8_CNS  /* txQueuesNum - TX Queues number*/
            },

            {
                /* TR101 Feature support */
                GT_TRUE, /*tr101Supported*/

                /* VLAN translation support */
                GT_TRUE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag                  */
                GT_TRUE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_TRUE /*trunkCrcHashSupported*/
            }
        }
    },    /* end "lion group 0 devices" */

    { /* Lion-Ch5-Metro devices */
        devs_Lion_Ch5_Metro,
        {
            NULL,  /*enhancedInfoPtr*/
            {
                /* bridge section                             */
                _32K,            /*GT_U32  fdb;               */
                VIDX_NUM_CNS,    /* vidx                      */
                STG_NUM_CNS,     /*stgNum                     */
                                 /*                           */
                /* tti section                                */
                _16K/4,          /*GT_U32 routerAndTunnelTermTcam;*/
                                 /*                           */
                /* tunnel section                             */
                _8K/4,           /*GT_U32  tunnelStart;       */
                AUTO_CALC_FIELD_CNS,/*GT_U32  routerArp;      */
                                 /*                           */
                /* ip section                                 */
                _8K,             /*GT_U32  routerNextHop;     */
                _2K,             /*GT_U32  mllPairs;          */
                                 /*                           */
                /* PCL section                                */
                _3K/4,           /*GT_U32 policyTcamRaws;     */
                                 /*                           */
                /* CNC section                                */
                CNC_AUTO_CALC,  /* GT_U32 cncBlocks;                          */
                _4K,             /*GT_U32 cncBlockNumEntries; */
                /*  Policer Section                           */
                _8K/4,           /*policersNum                */
                _2K/4,           /*egressPolicersNum          */
                                 /*                           */
                /* trunk Section                              */
                TRUNKS_NUM_CNS,  /*trunksNum                  */
                                 /*                           */
                /* Transmit Descriptors                       */
                _2K,             /*GT_U32 transmitDescr;      */
                                 /*                           */
                /* Buffer Memory                              */
                _8M,             /*GT_U32 bufferMemory;       */
                TX_QUEUE_NUM_8_CNS  /* txQueuesNum - TX Queues number*/
            },

            {
                /* TR101 Feature support */
                GT_TRUE, /*tr101Supported*/

                /* VLAN translation support */
                GT_TRUE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag */
                GT_TRUE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_FALSE /*trunkCrcHashSupported*/
            }
        }
    },    /* end "Lion-Ch5-Metro devices" */

    { /* Lion-Ch5-Enterprise devices */
        devs_Lion_Ch5_Enterprise,
        {
            NULL,  /*enhancedInfoPtr*/
            {
                /* bridge section                             */
                _32K,            /*GT_U32  fdb;               */
                VIDX_NUM_CNS,    /*vidx                       */
                STG_NUM_CNS,     /*stgNum                     */
                                 /*                           */
                /* tti section                                */
                _16K/4,          /*GT_U32 routerAndTunnelTermTcam;*/
                                 /*                           */
                /* tunnel section                             */
                _8K/4,           /*GT_U32  tunnelStart;       */
                AUTO_CALC_FIELD_CNS,/*GT_U32  routerArp;      */
                                 /*                           */
                /* ip section                                 */
                _8K,             /*GT_U32  routerNextHop;     */
                _2K,             /*GT_U32  mllPairs;          */
                                 /*                           */
                /* PCL section                                */
                _3K/4,           /*GT_U32 policyTcamRaws;     */
                                 /*                           */
                /* CNC section                                */
                CNC_AUTO_CALC,  /* GT_U32 cncBlocks;                          */
                _4K,             /*GT_U32 cncBlockNumEntries; */
                /*  Policer Section                           */
                _8K/4,           /*policersNum                */
                _2K/4,           /*egressPolicersNum          */
                                 /*                           */
                /* trunk Section                              */
                TRUNKS_NUM_CNS,  /*trunksNum                  */
                                 /*                           */
                /* Transmit Descriptors                       */
                _2K,             /*GT_U32 transmitDescr;      */
                                 /*                           */
                /* Buffer Memory                              */
                _8M,             /*GT_U32 bufferMemory;       */
                TX_QUEUE_NUM_8_CNS /* txQueuesNum - TX Queues number*/
            },

            {
                /* TR101 Feature support */
                GT_TRUE, /*tr101Supported*/

                /* VLAN translation support */
                GT_TRUE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag */
                GT_TRUE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_TRUE /*trunkCrcHashSupported*/
            }
        }
    },    /* end "Lion-Ch5-Enterprise devices" */

    { /* Lion-Crossbar devices */
        devs_Lion_Crossbar,
        {
            NULL,  /*enhancedInfoPtr*/
            {
                /* bridge section                             */
                _32K,            /*GT_U32  fdb;               */
                0,               /*vidx                       */
                0,               /*stgNum                     */
                                 /*                           */
                /* tti section                                */
                0,               /*GT_U32 routerAndTunnelTermTcam;*/
                                 /*                           */
                /* tunnel section                             */
                0,               /*GT_U32  tunnelStart;       */
                0,               /*GT_U32  routerArp;         */
                                 /*                           */
                /* ip section                                 */
                0,               /*GT_U32  routerNextHop;     */
                0,               /*GT_U32  mllPairs;          */
                                 /*                           */
                /* PCL section                                */
                0,               /*GT_U32 policyTcamRaws;     */
                                 /*                           */
                /* CNC section                                */
                0,               /*GT_U32 cncBlocks;          */
                0,               /*GT_U32 cncBlockNumEntries; */
                /* Policer Section                            */
                0,               /*policersNum                */
                0,               /*egressPolicersNum          */
                                 /*                           */
                /* trunk Section                              */
                TRUNKS_NUM_CNS,  /*trunksNum                  */
                                 /*                           */
                /* Transmit Descriptors                       */
                0,               /*GT_U32 transmitDescr;      */
                                 /*                           */
                /* Buffer Memory                              */
                0,               /*GT_U32 bufferMemory;       */
                TX_QUEUE_NUM_8_CNS  /* txQueuesNum - TX Queues number*/
            },

            {
                /* TR101 Feature support */
                GT_FALSE, /*tr101Supported*/

                /* VLAN translation support */
                GT_FALSE, /*vlanTranslationSupported*/

                /* Policer Ingress second stage support flag */
                GT_FALSE, /*iplrSecondStageSupported*/

                /* trunk CRC hash support flag */
                GT_FALSE /*trunkCrcHashSupported*/
            }
        }
    }    /* end "Lion-Crossbar devices" */

};

static GT_U32 lionTables_size = sizeof(lionTables)/sizeof(lionTables[0]);

/* array of additional FEr/RM WA for the xcat and above */
static GT_U32 xcatA1ErrAndRmArray[]=
{
    /* RM - xcat */
    PRV_CPSS_DXCH_XCAT_RM_TOTAL_BUFFER_LIMIT_CONF_IN_TXQ_E,/*RM#3008*/
    PRV_CPSS_DXCH_XCAT_RM_CNC_ENABLE_COUNTING_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_TXQ_EXT_CNTR_REG_WA_E,/*RM#3009*/
    PRV_CPSS_DXCH_XCAT_RM_MIRROR_INTERFACE_PARAMETER_REGISTER_I_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_POLICY_ENGINE_CONFIGURATION_REGISTER_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_TXQ_DEQUEUE_SWRR_WEIGHT_REGISTER_3_WA_E,/*RM#3007*/
    PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL0_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL1_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_POLICER_CONTROL2_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_EGRESS_POLICER_GLOBAL_CONFIGURATION_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_BCN_CONTROL_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_PORTS_BCN_AWARENESS_TABLE_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_TTI_UNIT_GLOBAL_CONFIGURATION_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_EXTENDED_DSA_BYPASS_BRIDGE_WA_E,/*RM#3005*/
    PRV_CPSS_DXCH_XCAT_RM_PORT_SERIAL_PARAMETERS_CONFIGURATION_WA_E,/*RM#3004*/
    PRV_CPSS_DXCH_XCAT_RM_LMS0_LMS1_MISC_CONFIGURATIONS_WA_E,/*RM#3002*,RM#3003*/
    PRV_CPSS_DXCH_XCAT_RM_HEADER_ALTERATION_GLOBAL_CONFIGURATION_REGISTER_WA_E,/*RM#3024*/
    PRV_CPSS_DXCH_XCAT_TOD_NANO_SEC_SET_WA_E,/*FEr#2773*/
    PRV_CPSS_DXCH_XCAT_ETH_OVER_IPV4_GRE_WA_E,
    PRV_CPSS_DXCH_XCAT_PASSENGER_ETH_PACKET_CRC_CANT_BE_RETAINED_WA_E, /*FEr#2785*/

    PRV_CPSS_DXCH_XCAT_TX_CPU_CORRUPT_BUFFER_WA_E,/*FEr#3048*/
    PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E,/*FEr#3033*/
    PRV_CPSS_DXCH_XCAT_IEEE_RESERVED_MC_CONFG_REG_READ_WA_E,/*FEr#3060*/
    PRV_CPSS_DXCH_XCAT_RESTRICTED_ADDRESS_FILTERING_WA_E,


    /* FEr from legacy device */
    PRV_CPSS_DXCH_TOKEN_BUCKET_RATE_SHAPING_DISABLE_WA_E,/*FEr#47*/
    PRV_CPSS_DXCH_PACKET_ORIG_BYTE_COUNT_WA_E,/*FEr#89*/
    PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E,/*none*/

    PRV_CPSS_DXCH3_POLICY_AND_ROUTER_TCAM_TABLES_INDIRECT_READ_WA_E,/*FEr#2004*/
    PRV_CPSS_DXCH3_RM_LMS0_1_GROUP1_LED_INTERFACE_WA_E,/*RM#2024*/
    PRV_CPSS_DXCH3_DIRECT_ACCESS_TO_EQ_ADDRESS_SPACE_WA_E,/*FEr#2028*/
    PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E,/*FEr#2033,FEr#2050*/

    /* RM - from legacy device */
    PRV_CPSS_DXCH3_RM_BM_GE_XG_PORT_GROUP_LIMIT_CFG_REG_WA_E,/*RM#2003*/
    PRV_CPSS_DXCH3_RM_GE_PORTS_MODE1_REG_WA_E,/*RM#2007*/
    PRV_CPSS_DXCH3_RM_XG_PORTS_MODE_REG_WA_E,/*RM#2008*/
    PRV_CPSS_DXCH2_RM_BRG_LAYER_4_SANITY_CHECKS_WA_E,/*RM#3018*/
    PRV_CPSS_DXCH3_RM_CPU_ACCESS_TO_FDB_UNDER_TRAFFIC_WA_E,/*RM#3014*/

    LAST_DEV_IN_LIST_CNS/* must be last */
};

/* array of additional FEr/RM WA for the Lion */
static GT_U32 lionErrAndRmArray[]=
{
    /* Lion FEr */
    PRV_CPSS_DXCH_LION_URPF_PER_VLAN_NOT_SUPPORTED_WA_E,/*FEr#2732*/
    PRV_CPSS_DXCH_LION_PREVENT_REDUNDANT_AU_FU_MESSAGES_NOT_SUPPORTED_WA_E,/*2769*/
    PRV_CPSS_DXCH_LION_POLICER_COUNTING_MEM_ACCESS_WA_E, /*FEr#2771*/
    PRV_CPSS_DXCH_LION_CN_GLOBAL_REG_WA_E,/*FEr#2768*/
    PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E,/*???*/
    PRV_CPSS_DXCH_LION_TEMPERATURE_SENSOR_INITIALIZATION_WA_E , /*???*/
    PRV_CPSS_DXCH_LION_RM_MCFIFO_DISTRIBUTION_REG_WA_E, /*???*/
    PRV_CPSS_DXCH_LION_RM_SD_PLL_REG_WA_E, /* HWE#27060 */
    PRV_CPSS_DXCH_LION_SLOW_RATE_ON_PORT_TB_NOT_SUPPORTED_WA_E,
    PRV_CPSS_DXCH_LION_IPCL_FALSE_PARITY_ERROR_WA_E, /* FEr#2774 */

    /* RM Lion */
    PRV_CPSS_DXCH_LION_RM_SDMA_ACTIVATION_WA_E,/*Rm#2701*/
    PRV_CPSS_DXCH_LION_RM_SYNC_ETHERNET_NONE_SELECT_WA_E,/*RM#2705*/
    PRV_CPSS_DXCH_LION_RM_TCAM_CONFIG_WA_E,/*RM#3013*/
    PRV_CPSS_DXCH_LION_RM_MULTI_PORT_GROUP_FDB_LIMIT_WA_E,/*RM#2702*/
    PRV_CPSS_DXCH_LION_RM_ORIG_SRC_PORT_FILTERING_WA_E,/*RM#2706*/
    PRV_CPSS_DXCH_LION_RM_MULTI_PORT_GROUP_CPU_IN_VIDX_WA_E,/*RM#2707*/
    PRV_CPSS_DXCH_LION_RM_UC_STRICT_OVER_MC_WA_E, /*RM#2710*/
    PRV_CPSS_DXCH_LION_RM_SCHEDULER_DEFICIT_MODE_WA_E, /*RM#2709*/
    PRV_CPSS_DXCH_LION_RM_TX_MC_IN_CHUNKS_WA_E, /*RM#2711*/
    PRV_CPSS_DXCH_LION_RM_BUFF_LIMIT_EGRESS_QUEUE_WA_E, /*RM#2712*/
    PRV_CPSS_DXCH_LION_RM_QCN_FOR_CN_FORMAT_WA_E, /*RM#2713*/

    /* FEr from legacy devices */
    PRV_CPSS_DXCH_TOKEN_BUCKET_RATE_SHAPING_DISABLE_WA_E,/*FEr#47*/
    PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E,/*FEr#2033,FEr#2050*/
    PRV_CPSS_DXCH3_POLICY_AND_ROUTER_TCAM_TABLES_INDIRECT_READ_WA_E,/*FEr#2004 --> part of FEr#2744*/
    PRV_CPSS_DXCH_XCAT_IEEE_RESERVED_MC_CONFG_REG_READ_WA_E,/*FEr#3060*/
    PRV_CPSS_DXCH3_DIRECT_ACCESS_TO_EQ_ADDRESS_SPACE_WA_E,/*FEr#2028*/
    PRV_CPSS_DXCH_PACKET_ORIG_BYTE_COUNT_WA_E,/*FEr#89*/

    /* RM - from legacy device */
/*  not needed in Lion-B (like xcat) already in correct values.  PRV_CPSS_DXCH3_RM_MAC2ME_LOW_REG_WA_E, RM#2026*/
    PRV_CPSS_DXCH_RM_FDB_GLOBAL_CONF_REG_WA_E,/*RM# 2704*/
    PRV_CPSS_DXCH_XCAT_RM_EXTENDED_DSA_BYPASS_BRIDGE_WA_E,/*RM#3005*/
    PRV_CPSS_DXCH_XCAT_RM_TXQ_EXT_CNTR_REG_WA_E,/*RM#3009*/
    PRV_CPSS_DXCH_XCAT_RM_HEADER_ALTERATION_GLOBAL_CONFIGURATION_REGISTER_WA_E,/*RM#3024*/
    PRV_CPSS_DXCH_XCAT_TOD_NANO_SEC_SET_WA_E,/*FEr#2773*/

    PRV_CPSS_DXCH_XCAT_RM_CNC_ENABLE_COUNTING_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_MIRROR_INTERFACE_PARAMETER_REGISTER_I_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_POLICY_ENGINE_CONFIGURATION_REGISTER_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL0_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL1_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_POLICER_CONTROL2_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_EGRESS_POLICER_GLOBAL_CONFIGURATION_WA_E,/*(part of RM#3012)*/
/* not relevant to lion   PRV_CPSS_DXCH_XCAT_RM_BCN_CONTROL_WA_E,(part of RM#3012)*/
/* not relevant to lion   PRV_CPSS_DXCH_XCAT_RM_PORTS_BCN_AWARENESS_TABLE_WA_E,(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_TTI_UNIT_GLOBAL_CONFIGURATION_WA_E,/*(part of RM#3012)*/

    PRV_CPSS_DXCH2_RM_BRG_LAYER_4_SANITY_CHECKS_WA_E,/*RM#3018*/
    PRV_CPSS_DXCH3_RM_CPU_ACCESS_TO_FDB_UNDER_TRAFFIC_WA_E,/*RM#3014*/


    LAST_DEV_IN_LIST_CNS/* must be last */
};

/* array of additional FEr/RM WA for the xCat2 */
static GT_U32 xcat2ErrAndRmArray[]=
{
    /* FEr - xCat2 */
    PRV_CPSS_DXCH_XCAT2_EPCL_GLOBAL_EN_NOT_FUNCTIONAL_WA_E,/*FEr#3056*/
    PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E, /*FEr#3092*/

    /* RM - xCat2  */
    PRV_CPSS_DXCH_XCAT2_RM_POLICER_COUNT_FORMAT_WA_E,/*RM#3026*/
    PRV_CPSS_DXCH_XCAT2_RM_INT_EXT_REF_CLK_WA_E,/*RM#3027*/
    PRV_CPSS_DXCH_XCAT2_RM_STACK_PORT_MIB_CNTR_CONTROL_WA_E,/*RM#3028*/
    PRV_CPSS_DXCH_XCAT2_RM_POLICER_MEM_CONTROL_WA_E, /*RM#3029*/
    PRV_CPSS_DXCH_XCAT2_RM_INCORRECT_XG_SHAPER_TOKEN_BUCKET_WA_E,  /*RM#3030*/
    PRV_CPSS_DXCH_XCAT2_RM_BYPASS_PACKET_REFRESH_FDB_AGE_WA_E,  /*RM#3031*/
    PRV_CPSS_DXCH_XCAT2_RM_PIPE_SELECT_WA_E, /*RM#3032*/

    /* RM - from legacy device */
    PRV_CPSS_DXCH_XCAT_RM_TXQ_DEQUEUE_SWRR_WEIGHT_REGISTER_3_WA_E,/*RM#3007*/
    PRV_CPSS_DXCH_XCAT_RM_TXQ_EXT_CNTR_REG_WA_E,/*RM#3009*/
    PRV_CPSS_DXCH_LION_RM_TCAM_CONFIG_WA_E,/*RM#3013*/
    PRV_CPSS_DXCH3_RM_CPU_ACCESS_TO_FDB_UNDER_TRAFFIC_WA_E,/*RM#3014*/
    PRV_CPSS_DXCH2_RM_BRG_LAYER_4_SANITY_CHECKS_WA_E,/*RM#3018*/
    PRV_CPSS_DXCH_XCAT_RM_CNC_ENABLE_COUNTING_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL0_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL1_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_POLICER_CONTROL2_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_EGRESS_POLICER_GLOBAL_CONFIGURATION_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_BCN_CONTROL_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_PORTS_BCN_AWARENESS_TABLE_WA_E,/*(part of RM#3012)*/
    PRV_CPSS_DXCH_XCAT_RM_EXTENDED_DSA_BYPASS_BRIDGE_WA_E,/*RM#3005*/
    PRV_CPSS_DXCH_XCAT_RM_PORT_SERIAL_PARAMETERS_CONFIGURATION_WA_E,/*RM#3004*/
    PRV_CPSS_DXCH_XCAT_RM_LMS0_LMS1_MISC_CONFIGURATIONS_WA_E,/*RM#3002,RM#3003*/
    PRV_CPSS_DXCH3_RM_LMS0_1_GROUP1_LED_INTERFACE_WA_E,/*RM#2024*/

    /* FEr from legacy device */
    PRV_CPSS_DXCH_XCAT_IEEE_RESERVED_MC_CONFG_REG_READ_WA_E,/*FEr#3060*/
    PRV_CPSS_DXCH3_DIRECT_ACCESS_TO_EQ_ADDRESS_SPACE_WA_E,/*FEr#2028*/
    PRV_CPSS_DXCH3_POLICY_AND_ROUTER_TCAM_TABLES_INDIRECT_READ_WA_E,/*FEr#2004*/
    PRV_CPSS_DXCH_TOKEN_BUCKET_RATE_SHAPING_DISABLE_WA_E,/*FEr#47*/
    PRV_CPSS_DXCH_PACKET_ORIG_BYTE_COUNT_WA_E,/*FEr#89*/
    PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E,/*none*/
    PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E,/*FEr#2033,FEr#2050*/

    LAST_DEV_IN_LIST_CNS/* must be last */
};

/*******************************************************************************
* prvCpssDxCh3NetworkSerdesBitmapGet
*
* DESCRIPTION:
*       Get device available SERDES bitmap
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*       powerDownBmpPtr - (pointer to) bitmask of ports that have not to be powered up
*
* OUTPUTS:
*       networkSerdesPowerUpBmpPtr - (pointer to) device SERDES bitmap for powerUp operation
*       networkSerdesBmpPtr - (pointer to) device SERDES bitmap
*
* RETURNS:
*       None.
*
* COMMENTS:
*       SERDES group represents four SERDESes (6 Network SERDES groups, 1 - combo SERDES group)
*       This function creates bitmap only for Network SERDESes and not relevants for
*       combo SERDES group.
*
*******************************************************************************/
static GT_VOID prvCpssDxCh3NetworkSerdesBitmapGet
(
    IN GT_U8   devNum,
    IN CPSS_PORTS_BMP_STC   *powerDownBmpPtr,
    OUT GT_U32              *networkSerdesPowerUpBmpPtr,
    OUT GT_U32              *networkSerdesBmpPtr
)
{
    GT_U32 port;    /* Port */
    GT_U32 serdes;  /* Start SERDES in the group */
    GT_U32 lane;    /* SERDES group lane */

    /* Reset initial bitmap to zero */
    *networkSerdesBmpPtr = 0;
    *networkSerdesPowerUpBmpPtr = 0;

    /* Loop all network ports */
    for(port = 0; port < 24; port++)
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, port);

        if (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, port) == PRV_CPSS_PORT_XG_E)
        {
            /* Convert network port to corresponding SERDES */
            serdes = (port / 4) * 4;

            /* Set SERDES bitmap per existed XG port */
            for(lane = 0; lane < 4; lane++)
            {
                *networkSerdesBmpPtr |= (1 << (serdes + lane));
                if ((powerDownBmpPtr->ports[0] & (1 << port)) == 0 )
                {   /* port is not in power down bitmap. Need power UP SERDES for it */
                    *networkSerdesPowerUpBmpPtr |= (1 << (serdes + lane));
                }
            }
        }
        else
        {
            /* Set SERDES bitmap per network port */
            *networkSerdesBmpPtr |= (1 << port);
            *networkSerdesPowerUpBmpPtr |= ((1 << port) & ~powerDownBmpPtr->ports[0]);
        }
    }
}

/*******************************************************************************
* prvCpssDxXcatNetworkSerdesBitmapGet
*
* DESCRIPTION:
*       Get device available SERDES bitmap
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*       powerDownBmpPtr - bitmask of ports that have not to be powered up
*
* OUTPUTS:
*       networkSerdesBmpPtr - (pointer to) device SERDES bitmap
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvCpssDxXcatNetworkSerdesBitmapGet
(
    IN GT_U8   devNum,
    IN CPSS_PORTS_BMP_STC   *powerDownBmpPtr,
    OUT GT_U32              *networkSerdesBmpPtr
)
{
    GT_U32 port;    /* Port */
    GT_U32 serdes;  /* Start SERDES in the group */

    /* Reset initial bitmap to zero */
    *networkSerdesBmpPtr = 0;

    /* Loop all network ports */
    for(port = 0; port < 24; port++)
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, port);
        serdes = (port / 4);

        if ((powerDownBmpPtr->ports[0] & (1 << port)) == 0 )
        {   /* port is not in power down bitmap. Need power UP SERDES for it */
            *networkSerdesBmpPtr |= (1 << (serdes));
        }
    }
}

/*******************************************************************************
* prvCpssDxCh3TcamEfuseWa
*
* DESCRIPTION:
*       Implement the replacement operations for the tcam efuse WA (FEr#2191)
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum                  - device number
*       tcamTestPointerAddrPtr  - (pointer to) array of the tcam test pointer registers address
*       numOfPointerRegs        - number of tcam test pointer registers
*       tcamTestConfigAddr      - address of the tcam test config status register
*
* OUTPUTS:
*       replaceRowPtr   - (pointer to) the row number which is used to replace
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3TcamEfuseWa
(
    IN GT_U8    devNum,
    IN GT_U32   *tcamTestPointerAddrPtr,
    IN GT_U32   numOfPointerRegs,
    IN GT_U32   tcamTestConfigAddr,
    OUT GT_U32  *replaceRowPtr
)
{
    GT_STATUS rc;           /* return status */
    GT_U32    i;            /* loop counter */
    GT_U32    regData;      /* register data */
    GT_U32    busyIterator; /* Counter for busy wait loops */

    *replaceRowPtr = NO_TCAM_ROW_TO_REPLACE_CNS;

    for(i = 0; i < numOfPointerRegs ; i++)
    {
        rc = prvCpssDrvHwPpReadRegister(devNum, tcamTestPointerAddrPtr[i], &regData);
        if( rc != GT_OK )
        {
            return rc;
        }

        /* valid data at pointer register */
        if( (regData & 0x100) == 0x100 )
        {
            *replaceRowPtr = (regData & 0xFF) + (i << 8);
            break;
        }
    }

    /* no replacement needed */
    if( i == numOfPointerRegs )
    {
        return GT_OK;
    }

    /* The entries in the last row of the TCAM logical sub-block for the row that was */
    /* replaced. A TCAM logical sub-block comprises 128 rows.                         */
    LAST_SUB_BLOCK_ROW_CALC_MAC(*replaceRowPtr);

    /* The following sequence (inside the "for" clause) is done two times to ensure */
    /* that the replacement operation has been completed.                             */
    for(i = 0; i < 2 ; i++)
    {
        rc = prvCpssDrvHwPpWriteRegister(devNum, tcamTestConfigAddr, 0x1038B);
        if( rc != GT_OK )
        {
            return rc;
        }

        busyIterator = 0;
        /* now busy wait until the write process is finished */
        do
        {
            rc = prvCpssDrvHwPpReadRegister(devNum, tcamTestConfigAddr, &regData);
            if( rc != GT_OK )
            {
                return rc;
            }
            /* check that number of iteration not over the limit */
            PRV_CPSS_DXCH_MAX_NUM_ITERATIONS_CHECK_CNS(devNum,(busyIterator++));
        } while (regData != 0x1028B);
    }

    rc = prvCpssDrvHwPpWriteRegister(devNum, tcamTestConfigAddr, 0x28B);

    return rc;
}

/*******************************************************************************
* prvCpssDxCh3TcamWa
*
* DESCRIPTION:
*       runs BISTs on PCL & Router TCAM if efuse not burned.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum      - The device number
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK         - on success
*       GT_HW_ERROR   - on hardware error
*       GT_BAD_PARAM  - on bad devNum
*       GT_UNFIXABLE_BIST_ERROR  - on unfixable problem in the TCAM
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3TcamWa
(
    IN GT_U8 devNum
)
{
    GT_BOOL blockFixed = GT_FALSE;
    GT_U32 replacedIndex;
    GT_U32 i;
    GT_STATUS rc;

#ifdef ASIC_SIMULATION
    /* no WA needed */
    if (blockFixed == GT_FALSE)
    {
        return GT_OK;
    }
#endif /*ASIC_SIMULATION*/

    /* Checks if Efuse was burned - PCL TCAM */
    for (i=0;i < 14;i++)
    {
        rc = cpssDxChDiagMemoryBistBlockStatusGet(devNum,
              CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E,i,&blockFixed,&replacedIndex);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (blockFixed == GT_TRUE)
        {
            break;
        }
    }

    /* Efuse has not been burnt - run BISTs on PCL TCAM */
    if (blockFixed == GT_FALSE)
    {
        rc = cpssDxChDiagMemoryBistsRun(devNum,CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E,
               2000,GT_TRUE);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* Checks if Efuse was burned - ROUTER TCAM */
    for (i=0;i < 20;i++)
    {
        rc = cpssDxChDiagMemoryBistBlockStatusGet(devNum,
              CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E,i,&blockFixed,&replacedIndex);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (blockFixed == GT_TRUE)
        {
            break;
        }
    }

    /* Efuse has not been burnt - run BISTs on ROUTER TCAM */
    if (blockFixed == GT_FALSE)
    {
        rc = cpssDxChDiagMemoryBistsRun(devNum,CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E,
               2000,GT_TRUE);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* hwPpCheetahCommonRegsDefault
*
* DESCRIPTION:
*       Set mandatory default values for Cheetah registers
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpCheetahCommonRegsDefault
(
    IN GT_U8   devNum
)
{
    GT_U32 regAddr;      /* register's address */

    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
              PRV_CPSS_DXCH_RM_FDB_GLOBAL_CONF_REG_WA_E) == GT_TRUE)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 7, 1, 0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpCheetahRegsDefault
*
* DESCRIPTION:
*       Set mandatory default values for Cheetah registers
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpCheetahRegsDefault
(
    IN GT_U8   devNum
)
{
    GT_STATUS rc;     /* return code */
    GT_U32 regAddr;   /* register's address */
    GT_U8  port;      /* iterator */

    /* start with the common settings */
    rc =  hwPpCheetahCommonRegsDefault(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Mirrored Packets to Analyzer Port Descriptors Limit
       Configuration Register.
       Mirrored Packets to Analyzer Port Descriptors Limit is too high.
       This value should be low enough in order not to generate FC packets
       due to Mirroring.
       Since Xoff value is 70, and 1.5K packets require 6 buffers, this value
       should be set to 0xA. (70-10*6=10 spare).
       (RM#9) - relevant for all DxCh devices
       WA: default value of of bits[0..11] should be 4 */

    if (prvCpssDrvHwPpWriteRegister(devNum,0x01800024,0xA004) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* GE and XG ports configurations */
    for( port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
    {
        if ((PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portType ==
             PRV_CPSS_PORT_NOT_EXISTS_E) ||
            (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portType ==
             PRV_CPSS_PORT_FE_E))
            continue;

        if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portType ==
             PRV_CPSS_PORT_GE_E)
        {

            /* Set Port SERDES Config Register3
              - Bits [10:9] <EQ_BIAS> => 0x3
              - Bits[15:14] <BG_VSelBits> => 0x2
              - Bits [13:11] <TPVL> => 0x0 */
            regAddr = 0x0A800034 + (port << 8);
            if (prvCpssDrvHwPpWriteRegister(devNum,regAddr,0x8600) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            /* Set Port SERDES Config Register0 - set amplitude = 600mv,
               field OutAmp = 3 */
            regAddr = 0x0A800028 + (port << 8);
            if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 13, 3, 3) != GT_OK)
                return GT_HW_ERROR;
        }
        else
        {
            /* Port<n> FIFOs Thresholds Configuration Register:
               Bits [14:10], <TXRdThr> => 0x6, Avoid underruns in the fifo*/
            /* FIFOs Thresholds Configuration register (24<=n<27) default value
               is incorrect (RM#7). WA - Change default value */
            regAddr = 0x0A800010 + (port << 8);
            if (prvCpssDrvHwPpSetRegField(devNum, regAddr,0, 16, 0x000019EA)
                != GT_OK)
                return GT_HW_ERROR;

            if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portTypeOptions !=
                PRV_CPSS_XG_PORT_HX_QX_ONLY_E)
            {
                /***********  Midway Settings ********
                  Set pre-emphasis to 0% and reset the lane.- HWCFG_MODE = 0xA  */

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,0, GT_FALSE,
                                                 0x8000,
                                                 PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                 0x800A) != GT_OK)
                    return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,0, GT_FALSE,
                                                 0x8001,
                                                 PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                 0x800A) != GT_OK)
                    return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,0, GT_FALSE,
                                                 0x8002,
                                                 PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                 0x800A) != GT_OK)
                    return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,0, GT_FALSE,
                                                 0x8003,
                                                 PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                 0x800A) != GT_OK)
                    return GT_HW_ERROR;


                /* Loop Gain (Cheetah-A1/A2) - Loop Gain = 0x0 */
                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,0,GT_FALSE,
                                                0xff2c,
                                                PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                0x2C01) != GT_OK)
                    return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,0,GT_FALSE,
                                                0xff2c,
                                                PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                0x2C01)!= GT_OK)
                    return GT_HW_ERROR;

                /* Amplitude / Pre-Emphasis / Interpolator setting
                 - Cheetah A0 -Interpolator input 7 bits control/Ampl=4/PreEmp=0
                 - Cheetah A1 / Cheetah+ A1 Interpolator input 6 bits
                                            control/Ampl=4/PreEmp=0/ */
                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,0, GT_FALSE,
                                                0xff28,
                                                PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                0xC000) != GT_OK)
                     return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                0, GT_FALSE,
                                                0xff28,
                                                PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                0xC000) != GT_OK)
                     return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                0, GT_FALSE,
                                                0xff29,
                                                PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                0xC000) != GT_OK)
                     return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                0, GT_FALSE,
                                                0xff29,
                                                PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                0xC000) != GT_OK)
                     return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                0, GT_FALSE,
                                                0xff2A,
                                                PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                0xC000) != GT_OK)
                     return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                0, GT_FALSE,
                                                0xff2A,
                                                PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                0xC000) != GT_OK)
                     return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                0, GT_FALSE,
                                                0xff2B,
                                                PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                0xC000) != GT_OK)
                     return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                0, GT_FALSE,
                                                0xff2B,
                                                PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                0xC000) != GT_OK)
                     return GT_HW_ERROR;

               /* SERDES Rx equalizer settings. (RM#161) */

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                0, GT_FALSE,
                                                0xff40,
                                                PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                0x3D) != GT_OK)
                     return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                0, GT_FALSE,
                                                0xff41,
                                                PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                0x3D) != GT_OK)
                     return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                0, GT_FALSE,
                                                0xff42,
                                                PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                0x3D) != GT_OK)
                     return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                0, GT_FALSE,
                                                0xff43,
                                                PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                0x3D) != GT_OK)
                     return GT_HW_ERROR;

                if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH_RM_XG_PHY_PATH_FAILURE_WA_E))
                {
                    /* Fix XG PHY path failure, Rm#83 */
                    if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                     0, GT_FALSE,
                                                     0xff34,
                                                     PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                     0xff00) != GT_OK)
                        return GT_HW_ERROR;

                    if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                     0, GT_FALSE,
                                                     0xff34,
                                                     PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                     0xff00) != GT_OK)
                       return GT_HW_ERROR;

                    if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                     0, GT_FALSE,
                                                     0xff34,
                                                     PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                     0xe000) != GT_OK)
                       return GT_HW_ERROR;
                }

                /* Improve the XAUI performance for A2 silicon and for DX249/269
                   A0, by implementing RN#169. */
                if((PRV_CPSS_PP_MAC(devNum)->revision >= 2) ||
                   (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX249_CNS) ||
                   (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX269_CNS) ||
                   (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX169_CNS))
                {
                    if(cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                 0, GT_FALSE,
                                                 0xff44,
                                                 PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                 0x18) != GT_OK)
                        return GT_HW_ERROR;
                }
            }
        }
    }

    /* CPU port configuration
       Port<63> Auto-Negotiation Configuration Register:
       Bit [11], AnFcEn - For CPU port, this field should be 0x0 and not 0x1. */
    regAddr = 0x0A803F0C;
    if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 11, 1, 0) != GT_OK)
        return GT_HW_ERROR;

    /* Buffer Memory MPPM Configuration Register:
      Bits [15:11], <GigEFIFORdthreshold> => 0xA,
      Avoid bufmem-port protocol violations in case of collisions.*/
    /* Buffer Memory MPPM Configuration Register default value is incorrect.
       (RM#8) */
    regAddr = 0x0780000C;
    if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00005042) != GT_OK)
        return GT_HW_ERROR;

    /*  (RM#26) Setting buffer DRAM to 1Mb for Dx106 and Dx107 devices */
    if(PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX106_CNS ||
       PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX107_CNS)
    {
        /* Buffer Management Global Buffers Limits Configuration Register,
           offset 0x3000000 */
        regAddr = 0x3000000;
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 31, 0xFC70896)
            != GT_OK)
            return GT_HW_ERROR;

        /* Buffer Management GigE Port Group Limits Configuration Register,
           offset 0x3000004  */
        regAddr = 0x3000004;
        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x1F880100) != GT_OK)
            return GT_HW_ERROR;

        /* Transmit Queue Control Register - Set total desriptors number  */
        regAddr = 0x01800000;
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 16, 16,  0x1A2) != GT_OK)
            return GT_HW_ERROR;

        /* Total buffer limit configuration register  */
        regAddr = 0x01800084;
        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x15E) != GT_OK)
            return GT_HW_ERROR;

    if(PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX107_CNS)
    {
       /* (RM#163) - zero SERDES configuration registers for not existing ports */
        for(port = 10; port < 24; port++)
        {
            if (prvCpssDrvHwPpWriteRegister(devNum, 0x0A800028 + (port << 8), 0)
                 != GT_OK)
                return GT_HW_ERROR;
        }
    }
    }
     /*  (RM#27) Correct memory configuration for DX249/169 devices */
    else if((PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX249_CNS) ||
            (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX169_CNS) ||
            (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX163_CNS) ||
            (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX243_CNS))
    {
        /* Buffer Management Global Buffers Limits Configuration Register,
           offset 0x03000000 */
        regAddr = 0x03000000;
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 31, 0x301591CC) !=
            GT_OK)
            return GT_HW_ERROR;

        /* Buffer Management GigE Port Group Limits Configuration Register,
           offset 0x03000004  */
        regAddr = 0x03000004;
        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x60180300) != GT_OK)
            return GT_HW_ERROR;

        /* Transmit Queue Control Register - Set total desriptors number  */
        regAddr = 0x01800000;
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 16, 16, 0x53A) != GT_OK)
            return GT_HW_ERROR;

        /* Total buffer limit configuration register  */
        regAddr = 0x01800084;
        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x498) != GT_OK)
            return GT_HW_ERROR;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX133_CNS)
    {
       /* zero SERDES configuration registers for not existing ports */
        for(port = 12; port < 24; port++)
        {
            if (prvCpssDrvHwPpWriteRegister(devNum, 0x0A800028 + (port << 8), 0)
                 != GT_OK)
                return GT_HW_ERROR;
        }
    }

    return GT_OK;


}

/*******************************************************************************
* hwPpCheetah2RegsDefault
*
* DESCRIPTION:
*       Set mandatory default values for Cheetah registers
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*
*******************************************************************************/

static GT_STATUS hwPpCheetah2RegsDefault
(
    IN GT_U8   devNum
)
{
    GT_STATUS rc;        /* return code */
    GT_U32 regAddr;      /* register's address */
    GT_U8  port;         /* iterator           */
    GT_U8  gigaPortsNum; /* giga Ports Num     */
    GT_U8  xgPortsNum;   /* xg Ports Num       */
    unsigned int regValue;/* register's value read back */

    /* start with the common settings */
    rc =  hwPpCheetahCommonRegsDefault(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

      /* GE and XG ports configurations */
    for( port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
    {
        if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portType ==
             PRV_CPSS_PORT_NOT_EXISTS_E)
            continue;

        if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portType ==
             PRV_CPSS_PORT_GE_E)
        {

            /* Set Port SERDES Config Register3
              - Bits [10:9] <EQ_BIAS> => 0x3
              - Bits[15:14] <BG_VSelBits> => 0x2
              - Bits [13:11] <TPVL> => 0x0 */
            regAddr = 0x0A800034 + (port << 8);
            if (prvCpssDrvHwPpWriteRegister(devNum,regAddr,0x8600) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            /* Set Port SERDES Config Register0 - set amplitude = 600mv,
               field OutAmp = 3 */
            regAddr = 0x0A800028 + (port << 8);
            if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 13, 3, 3) != GT_OK)
                return GT_HW_ERROR;
            
#ifdef AX_FULL_DEBUG
			osPrintf("hwPpCheetah2RegsDefault::port %d reg %#0x type GE_PORT write ok\r\n",port,regAddr);
#endif
#ifndef __AX_PLATFORM__
        /* Set Port<n> Serdes Config Register0 
          * set not reset Serdes
          * field SerdesReset = 3,bit0 = 0  
          */
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, 0) != GT_OK)
        {
            osPrintf("hwPpCheetah2RegsDefault::Serdes not reset write %#0x error GE_PORT\r\n",regAddr);
            return GT_HW_ERROR;
        }
#ifdef AX_FULL_DEBUG
        osPrintf("hwPpCheetah2RegsDefault::Serde not reset write port %d reg %#0x type GE_PORT write ok\r\n",port,regAddr);
#endif
        /* 
          * Power up transmiter/receiver/PLL/IVREF
          * field PU_TX = 1,bit1 = 1
          * field PU_RX = 1,bit2 = 1
          * field PU_PLL = 1,bit3 = 1
          * field PU_IVREF = 1,bit4 = 1
          */
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 1, 4, 0xF) != GT_OK)
        {
            osPrintf("hwPpCheetah2RegsDefault::Serdes power up Tx/Rx/PLL/IVREF write %#0x error GE_PORT\r\n",regAddr);
            return GT_HW_ERROR;
        }
        prvCpssDrvHwPpReadRegister(devNum, regAddr, &regValue);
#ifdef AX_FULL_DEBUG
        osPrintf("hwPpCheetah2RegsDefault::Serdes power up Tx/Rx/PLL/IVREF write port %d reg %#0x type GE_PORT write ok\r\n",port,regAddr);
        osPrintf("hwPpCheetah2RegsDefault::read after write reg %#0x value %#0x\r\n",regAddr,regValue);
#endif
#endif

        }
        else
        {


            /***********  Midway Settings ********
              Set pre-emphasis to 0% and reset the lane.- HWCFG_MODE = 0xA  */
            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,0, GT_FALSE,
                                             0x8000,
                                             PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                             0x800A) != GT_OK)
                return GT_HW_ERROR;

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,0, GT_FALSE,
                                             0x8001,
                                             PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                             0x800A) != GT_OK)
                return GT_HW_ERROR;

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,0, GT_FALSE,
                                             0x8002,
                                             PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                             0x800A) != GT_OK)
                return GT_HW_ERROR;

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,0, GT_FALSE,
                                             0x8003,
                                             PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                             0x800A) != GT_OK)
                return GT_HW_ERROR;


            /* Loop Gain (Cheetah-A1/Cheetah+-A0) - Loop Gain = 0x0 */
                /* set data for next settings- Cheetah A0 */


            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,0,GT_FALSE,
                                        0xff2c,
                                        PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                        0x2C01) != GT_OK)
                return GT_HW_ERROR;

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,0,GT_FALSE,
                                        0xff2c,
                                        PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                        0x2C01)!= GT_OK)
                return GT_HW_ERROR;


            /* Amplitude / Pre-Emphasis / Interpolator setting
             - Cheetah A1 /  A2 Interpolator input 6 bits
                                        control/Ampl=4/PreEmp=0/ */
            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,0, GT_FALSE,
                                            0xff28,
                                            PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                            0xC000) != GT_OK)
                 return GT_HW_ERROR;

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                            0, GT_FALSE,
                                            0xff28,
                                            PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                            0xC000) != GT_OK)
                 return GT_HW_ERROR;

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                            0, GT_FALSE,
                                            0xff29,
                                            PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                            0xC000) != GT_OK)
                 return GT_HW_ERROR;

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                            0, GT_FALSE,
                                            0xff29,
                                            PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                            0xC000) != GT_OK)
                 return GT_HW_ERROR;

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                            0, GT_FALSE,
                                            0xff2A,
                                            PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                            0xC000) != GT_OK)
                 return GT_HW_ERROR;

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                            0, GT_FALSE,
                                            0xff2A,
                                            PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                            0xC000) != GT_OK)
                 return GT_HW_ERROR;

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                            0, GT_FALSE,
                                            0xff2B,
                                            PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                            0xC000) != GT_OK)
                 return GT_HW_ERROR;

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                            0, GT_FALSE,
                                            0xff2B,
                                            PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                            0xC000) != GT_OK)
                 return GT_HW_ERROR;



            /* SERDES Rx equalizer settings (RM#161) */

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                            0, GT_FALSE,
                                            0xff40,
                                            PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                            0x3D) != GT_OK)
                 return GT_HW_ERROR;

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                            0, GT_FALSE,
                                            0xff41,
                                            PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                            0x3D) != GT_OK)
                 return GT_HW_ERROR;

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                            0, GT_FALSE,
                                            0xff42,
                                            PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                            0x3D) != GT_OK)
                 return GT_HW_ERROR;

            if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                            0, GT_FALSE,
                                            0xff43,
                                            PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                            0x3D) != GT_OK)
                 return GT_HW_ERROR;

            if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH_RM_XG_PHY_PATH_FAILURE_WA_E))
            {
                /* Fix XG PHY path failure, Rm#83 */
                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                 0, GT_FALSE,
                                                 0xff34,
                                                 PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                 0xff00) != GT_OK)
                    return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                 0, GT_FALSE,
                                                 0xff34,
                                                 PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                 0xff00) != GT_OK)
                   return GT_HW_ERROR;

                if (cpssDxChPhyPort10GSmiRegisterWrite(devNum,port,
                                                 0, GT_FALSE,
                                                 0xff34,
                                                 PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS,
                                                 0xe000) != GT_OK)
                   return GT_HW_ERROR;
            }


        }

    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH2_RM_RESERVED_REGISTERS_0x0B820108_WA_E))
    {
        /* (RM#1081) */
        if (prvCpssDrvHwPpWriteRegister(devNum,0x0B820108,0xB80) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    if(GT_TRUE ==
        PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH2_RM_BRG_LAYER_4_SANITY_CHECKS_WA_E))
    {
        /* Reset NST( Network Shield ) registers  -
           Bridge Global Configuration Registers */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     bridgeRegs.bridgeGlobalConfigRegArray[0];

        /* Disable dropping ARP packets with MAC SA not equal to the ARP
           payload data contains a field called <source hardware address>
           + all TCP Sanity checks */
        if(prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0x20030000, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* Disable all TCP related bits(0-4) + disable ICMP dropping(bit 5) */
        if(prvCpssDrvHwPpWriteRegBitMask(devNum,(regAddr + 0xC),0x0000003F, 0x0)
            != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    gigaPortsNum = 24;
    xgPortsNum   = 0;
    switch (PRV_CPSS_PP_MAC(devNum)->devType)
    {
        case CPSS_98DX125_CNS: gigaPortsNum = 12; break;
        case CPSS_98DX145_CNS: gigaPortsNum = 12; xgPortsNum = 2; break;
        case CPSS_98DX255_CNS: xgPortsNum = 0; break;
        case CPSS_98DX265_CNS: xgPortsNum = 2; break;
        case CPSS_98DX275_CNS: xgPortsNum = 3; break;
        case CPSS_98DX285_CNS: xgPortsNum = 4; break;
        case CPSS_98DX804_CNS: gigaPortsNum = 0; xgPortsNum = 4; break;
        default: break;
    }

    /* power off SERDES of not existing GIGA ports */
    for (port = gigaPortsNum; (port < 24); port++)
    {
        if (prvCpssDrvHwPpWriteRegister(
            devNum, (0x0A800028 + (port << 8)), 1)
            != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }


    /* power off SERDES of not existing 10-GIGA ports */
    for (port = (GT_U8)(24 + xgPortsNum); (port < 28); port++)
    {
        if (prvCpssDrvHwPpWriteRegister(
            devNum, (0x0A80181C + ((port - 24) * 0x100)), 0x804)
            != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
              PRV_CPSS_DXCH2_READ_SRC_ID_ASSIGMENT_MODE_CONF_REG_WA_E) == GT_TRUE)
    {
         regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                   bridgeRegs.srcIdReg.srcIdAssignedModeConfReg;
        if (prvCpssDrvHwPpWriteRegister(devNum,regAddr,0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        (PRV_CPSS_DXCH_PP_MAC(devNum))->errata.srcIdAssignedModeConfRegData = 0;

    }
    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
              PRV_CPSS_DXCH2_BRIDGE_ACCESS_MATRIX_LINE7_WRITE_WA_E) == GT_TRUE)
    {
        if (prvCpssDrvHwPpWriteRegister(devNum,0x0200061C,0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        (PRV_CPSS_DXCH_PP_MAC(devNum))->
                                      errata.bridgeAccessMatrixLine7RegData = 0;
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpCheetah3PortsAndSerdesRegsDefault
*
* DESCRIPTION:
*       Set mandatory default values for Cheetah3 ports and SERDES registers
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*       ppPhase1ParamsPtr  - (pointer to) Packet processor hardware specific
*                            parameters.
*
* OUTPUTS:
*       networkSerdesPowerUpBmpPtr - (pointer to) bitmap of network SERDES to
*                                    be power UP.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpCheetah3PortsAndSerdesRegsDefault
(
    IN GT_U8                                   devNum,
    IN CPSS_DXCH_PP_PHASE1_INIT_INFO_STC       *ppPhase1ParamsPtr,
    OUT GT_U32                                 *networkSerdesPowerUpBmpPtr
)
{
    GT_U32 regAddr;     /* register's address   */
    GT_U32 regValue;    /* register value       */
    GT_U32 portNum;     /* port number          */
    GT_U32 lane;        /* lane number          */
    GT_U32 devType;     /* device type          */
    GT_BOOL ch3XgDevice; /* XG Cheetah 3 family */
    GT_U32 serdesNum;   /* SERDES number        */
    GT_U32 networkSerdesBmp = 0;   /* device SERDES bitmap */

    /* define type of DxCh3 device either Gig or XG */
    devType = PRV_CPSS_PP_MAC(devNum)->devType;

    switch(devType)
    {
        case PRV_CPSS_CHEETAH3_XG_PORTS_DEVICES_CASES_MAC:
            ch3XgDevice = GT_TRUE;
            break;
        default:
            ch3XgDevice = GT_FALSE;
            break;
    }

    /* Set device SERDES bitmap */
    prvCpssDxCh3NetworkSerdesBitmapGet(devNum,
                                       &ppPhase1ParamsPtr->powerDownPortsBmp,
                                       networkSerdesPowerUpBmpPtr,
                                       &networkSerdesBmp);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH3_SERDES_YIELD_IMPROVEMENT_WA_E))
    {
        /* SERDES yield improvement due to PLL unlocking issue. */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[0].serdesReservConf[0];
        for( serdesNum = 0 ; serdesNum < 24 ; serdesNum++ )
        {
            if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 12, 1, 0) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            regAddr+=0x400;
        }
    }

    if (ppPhase1ParamsPtr->initSerdesDefaults == GT_TRUE)
    {
    /* FEr#2014 - Select the 156.25 MHz differential clock */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_SAMPLE_AT_RESET_NET_REF_CLOCK_SELECT_WA_E))
    {
        /* NP ref clock configurations */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[0].serdesSpeed1[0];
        for( serdesNum = 0 ; serdesNum < 24 ; serdesNum++ )
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesBmp);

            /* Set register value for all DxCh3 devices with XG ports  */
            if(ch3XgDevice == GT_TRUE)
            {
                regValue = 0x0000264A;
            }
            else
            {
                regValue = 0x0000213A;
            }

            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            regAddr += 0x400;
        }

        /* NP SERDES common register, change NP clock to differential mode */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[0].serdesCommon[0];

        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x0000000B) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* NP SERDES electrical configuration
     * Change TxSyncEn to 1.
     * RM#2106 - SERDES Tx Sync Conf Registers
    */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[0].serdesTxSyncConf[0];
     for( serdesNum = 0 ; serdesNum < 24 ; serdesNum++ )
     {
         PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesBmp);
         if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000003) != GT_OK)
         {
             return GT_HW_ERROR;
         }
         regAddr += 0x400;
    }

    /* NP SERDES electrical configuration
     * RX Conf. Register - FFE_RES_SEL=4, FFE_CAP_SEL=0
    */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[0].serdesRxConfReg1[0];
    for( serdesNum = 0 ; serdesNum < 24 ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesBmp);

        /* SERDES Rx Configuration register */
        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000C22) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        regAddr += 0x400;
    }

    if(ch3XgDevice == GT_TRUE)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[0].serdesIvrefConf1[0];

        for( serdesNum = 0 ; serdesNum < 24 ; serdesNum ++ )
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesBmp);

            /* NP SERDES electrical configuration
             * IVREF Conf Register1 - TxVdd=3, for 98DX81xx
             * RM#2107 - Network ports electrical settings violation at defaults
            */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x0000575A) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            regAddr += 0x400;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[0].serdesIvrefConf2[0];

        for( serdesNum = 0 ; serdesNum < 24 ; serdesNum ++ )
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesBmp);

            /* NP SERDES electrical configuration
             * IIVREF Conf Register2 - Vddr10Pll=2, for 98DX81xx
             * RM#2107 - Network ports electrical settings violation at defaults
            */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000003) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            regAddr += 0x400;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[0].serdesMiscConf[0];

        for( serdesNum = 0 ; serdesNum < 24 ; serdesNum ++ )
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesBmp);

            /* NP SERDES electrical configuration
             * Misc Conf Register - CLK_DET_EN=0, for 98DX81xx
             * RM#2107 - Network ports electrical settings violation at defaults
            */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000008) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            regAddr += 0x400;
        }
    }

    /* NP SERDES powerup and calibration */
    for( serdesNum = 0 ; serdesNum < 24 ; serdesNum += 4 )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, (*networkSerdesPowerUpBmpPtr));

        /* PowerUp Register 1.
        SerdesPowerUp1 and serdesPowerUp2 or serdesReset registers are per SERDES group.
        Group is 4 SERDESes. If first SERDES exists in SERDES bitmap then all exist in a group */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[serdesNum].serdesPowerUp1[0];

        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x000000FF) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* PowerUp Register 2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[serdesNum].serdesPowerUp2[0];

        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x000000FF) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait at least 10 usec */
    cpssOsTimerWkAfter(1);

    for( serdesNum = 0 ; serdesNum < 24 ; serdesNum += 4 )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, (*networkSerdesPowerUpBmpPtr));

        /* Analog Reset.
        SerdesPowerUp1 and serdesPowerUp2 or serdesReset registers are per SERDES group.
        Group is 4 SERDESes. If first SERDES exists in SERDES bitmap then all exist in a group */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[serdesNum].serdesReset[0];

        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000FF0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* Calibration Conf register, start automatic impedance calibration and PLL calibration */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[0].serdesCalibConf1[0];
    for( serdesNum = 0 ; serdesNum < 24 ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, (*networkSerdesPowerUpBmpPtr));

        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00003333) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        regAddr += 0x400;
    }

    /* wait at least 5 msec */
    cpssOsTimerWkAfter(5);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[0].serdesMiscConf[0];

    if (ch3XgDevice == GT_TRUE)
    {   /* XG - 98DX81x */

        /* Misc. Configuration Register: Reset TxSyncFIFO; Sample Tx Data at Posedge; CLK_DET_EN=0 */
        regValue = 0x4;
    }
    else
    {   /* GIG - 98DX5x */

        /* Misc. Configuration Register: Reset TxSyncFIFO; Sample Tx Data at Posedge */
        regValue = 0x5;
    }

    for( serdesNum = 0 ; serdesNum < 24 ; serdesNum ++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, (*networkSerdesPowerUpBmpPtr));

        /* Reset TxSyncFIFO */
        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        regAddr += 0x400;
    }

    for( serdesNum = 0 ; serdesNum < 24 ; serdesNum += 4 )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, (*networkSerdesPowerUpBmpPtr));

        /* Rx\DTL --> Normal operation.
        SerdesPowerUp1 and serdesPowerUp2 or serdesReset registers are per SERDES group.
        Group is 4 SERDESes. If first SERDES exists in SERDES bitmap then all exist in a group */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[serdesNum].serdesReset[0];

        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000000) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[0].serdesMiscConf[0];

    if (ch3XgDevice == GT_TRUE)
    {   /* XG - 98DX81x */

        /* Misc. Configuration Register: DeAssert Reset TxSyncFIFO; Sample Tx Data at Posedge; CLK_DET_EN=0 */
        regValue = 0x0000000C;
    }
    else
    {   /* GIG - 98DX5x */

        /* "Misc. Configuration Register: DeAssert Reset TxSyncFIFO and Sample Tx Data at Posedge */
        regValue = 0x0000000D;
    }

    for( serdesNum = 0 ; serdesNum < 24 ; serdesNum ++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, (*networkSerdesPowerUpBmpPtr));

        /* DeAssert Reset */
        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        regAddr += 0x400;
    }

    /* RX Conf. Register */

    if (ch3XgDevice == GT_TRUE)
    {

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[0].serdesRxConfReg1[0];

        for( serdesNum = 0 ; serdesNum < 24 ; serdesNum ++ )
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, (*networkSerdesPowerUpBmpPtr));

            /* RX Conf. Register - FFE_RES_SEL=4, FFE_CAP_SEL=0, RESET_INTP_EXT_RX=1 */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00004C22) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            regAddr += 0x400;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[0].serdesRxConfReg1[0];

        for( serdesNum = 0 ; serdesNum < 24 ; serdesNum ++ )
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, (*networkSerdesPowerUpBmpPtr));

            /* RX Conf. Register - FFE_RES_SEL=4, FFE_CAP_SEL=0, RESET_INTP_EXT_RX=0 */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000C22) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            regAddr += 0x400;
        }
    }
    } /* ppPhase1ParamsPtr->initSerdesDefaults == GT_TRUE */
    else
        *networkSerdesPowerUpBmpPtr = 0;

    /* NP MAC/PCS Reset */
    if(ch3XgDevice == GT_FALSE)
    {
        regAddr = 0x0A800008; /* macCtrl2 */
        for( serdesNum = 0 ; serdesNum < 24 ; serdesNum++ )
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesBmp);

            /* De-assert port reset */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x0000C008) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            regAddr += 0x400;
        }

    }
    else
    {
        /*De-assert reset on PCS, for 98DX81xx only
          De-assert reset on XG MAC, for 98DX81xx only */
        for( portNum = 0 ; portNum < 24 ; portNum++ )
        {
            if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType !=
               PRV_CPSS_PORT_NOT_EXISTS_E)
            {
                /* Activate NP XPCS - Deassert reset on group XPCS */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].xgGlobalConfReg0;

                if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000047) != GT_OK)
                {
                    return GT_HW_ERROR;
                }

                /* Activate NP XG MAC - Deassert reset on group XG MAC */
                PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);


                if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00002403) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
            }
        }
    }

    if (ppPhase1ParamsPtr->initSerdesDefaults == GT_TRUE)
    {
    /* HGS SERDES electrical configurations, 3.125G configurations */
    for( portNum = 24 ; portNum < 28 ; portNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);

        for( lane = 0 ; lane < 4 ; lane++ )
        {
            /* Speed Register 1, bit 10, FBDIV = 0x08 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesSpeed1[lane];

            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00002421) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            /* Speed Register 3, FcEn bit to SAS/SATA mode and bit 1 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesSpeed3[lane];

            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000003) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            /* Tx Conf Register 1, Amp=0x31, 50% pre-emphasis */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesTxConfReg1[lane];

            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x000027BE) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            /* Rx Conf Register 1, Ffelsel=2, FfeResSel=0, FfeCapSel=0 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesRxConfReg1[lane];

            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000C04) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            /* DTL Conf Register 2, Selmupi=1 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesDtlConfReg2[lane];

            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00002800) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            /* DTL Conf Register 3, Selmupf = 0x1, selmufi = 0x2, Selmuff = 0x2 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesDtlConfReg3[lane];

            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x000002A9) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            /* Reserv Conf Register, DFE_UPDATE_EN=0 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesReservConf[lane];

            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x0000F070) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            /* IVREF Conf Register 1, RxVdd10=11 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesIvrefConf1[lane];

            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x0000557B) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
    }

    /*********************/
    /* SP SERDES Powerup */
    /*********************/
    for( portNum = 24 ; portNum < 28 ; portNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);
        PRV_CPSS_DXCH_SKIP_POWER_DOWN_PORTS_MAC(portNum, ppPhase1ParamsPtr->powerDownPortsBmp.ports[0]);

        /* PowerUp Register 1.
        SerdesPowerUp1 and serdesPowerUp2 or serdesReset registers are per SERDES group.
        Group is 4 SERDESes. If first SERDES exists in SERDES bitmap then all exist in a group */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesPowerUp1[0];

        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x000000FF) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* PowerUp Register 2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesPowerUp2[0];

        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x000000FF) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* Wait at least 10 usec */
        cpssOsTimerWkAfter(1);

        /* Analog Reset, Reset calibration logic.
        SerdesPowerUp1 and serdesPowerUp2 or serdesReset registers are per SERDES group.
        Group is 4 SERDESes. If first SERDES exists in SERDES bitmap then all exist in a group */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesReset[0];

        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x0000FFF0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    for( portNum = 24 ; portNum < 28 ; portNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);
        PRV_CPSS_DXCH_SKIP_POWER_DOWN_PORTS_MAC(portNum, ppPhase1ParamsPtr->powerDownPortsBmp.ports[0]);

        for( lane = 0 ; lane < 4 ; lane++ )
        {
            /* Start PLL Calibration */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[portNum].serdesCalibConf1[lane];

            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000400) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        for( lane = 0 ; lane < 4 ; lane++ )
        {
                /* Start DLL Calibration */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    macRegs.perPortRegs[portNum].serdesCalibConf2[lane];

                if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000001) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
        }

        for( lane = 0 ; lane < 4 ; lane++ )
        {
                /* Start Impedance Calibration */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    macRegs.perPortRegs[portNum].serdesCalibConf1[lane];

                if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000401) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
        }

        for( lane = 0 ; lane < 4 ; lane++ )
        {
                /* Start Process Calibration */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    macRegs.perPortRegs[portNum].serdesProcessMon[lane];

                if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00001B61) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
        }

        for( lane = 0 ; lane < 4 ; lane++ )
        {
                /* Start Sampler Calibration */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    macRegs.perPortRegs[portNum].serdesCalibConf2[lane];

                if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000041) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
        }

        for( lane = 0 ; lane < 4 ; lane++ )
        {
                /* Reset TxSyncFIFO; Sample Tx Data at Posedge */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    macRegs.perPortRegs[portNum].serdesMiscConf[lane];

                if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000004) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
        }
    }

    /* Wait at least 10 usec */
    cpssOsTimerWkAfter(1);

    /* Rx/DTL --> Normal operation, per Group (each group contains 4 ports) */
    for( portNum = 24 ; portNum < 28 ; portNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);
        PRV_CPSS_DXCH_SKIP_POWER_DOWN_PORTS_MAC(portNum, ppPhase1ParamsPtr->powerDownPortsBmp.ports[0]);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesReset[0];

        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000000) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* Wait at least 10 msec */
    cpssOsTimerWkAfter(10);

    /* DeAssert Reset TxSyncFIFO; Sample Tx Data at Posedge */
    for( portNum = 24 ; portNum < 28 ; portNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);
        PRV_CPSS_DXCH_SKIP_POWER_DOWN_PORTS_MAC(portNum, ppPhase1ParamsPtr->powerDownPortsBmp.ports[0]);

        for( lane = 0 ; lane < 4 ; lane++ )
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesMiscConf[lane];

            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x0000000C) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
    }
    } /* ppPhase1ParamsPtr->initSerdesDefaults == GT_TRUE */

    /* HGS MAC/PCS Reset */
    for( portNum = 24 ; portNum < 28 ; portNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);

        /* Disable reset on PCS */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].xgGlobalConfReg0;

        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000047) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* De-assert port reset */
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);


        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00002403) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3HwPpPortSerdesWriteRegBitMask
*
* DESCRIPTION:
*       Write to all related port SERDES the masked value
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number
*       xgDevice    - GT_TRUE for XG device, GT_FALSE for GE device
*       regAddrPtr  - (pointer to) register address or array start of
*                     register addresses.
*       mask        - Mask for selecting the written bits.
*       value       - Data to be written to register or registers
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       The bits in value to be written are the masked bit of 'mask'.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3HwPpPortSerdesWriteRegBitMask
(
    IN GT_U8    devNum,
    IN GT_BOOL  xgDevice,
    IN GT_U32   *regAddrPtr,
    IN GT_U32   mask,
    IN GT_U32   value
)
{
    GT_U32      laneNum;    /* number of lanes per port */
    GT_U32      i;          /* loop index */

    if( xgDevice == GT_TRUE )
    {
        /* XG port */
        laneNum = 4;
    }
    else /* xgDevice == GT_FALSE */
    {
        /* GE port */
        laneNum = 1;
    }

    for( i = 0 ; i < laneNum ; i++ )
    {
        if ( prvCpssDrvHwPpWriteRegBitMask(devNum, regAddrPtr[i], mask, value) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3HwPpPllCalibrationSeq
*
* DESCRIPTION:
*       Perform the PLL calibration sequence which repeats in several places:
*       1. Turn SERDES PLL and Impedance calibration OFF
*       2. Turn SERDES PLL calibration ON
*       3. Turn SERDES Impedance calibration ON
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum                  - device number
*       networkSerdesPowerUpBmp - bitmap of network SERDES to be power UP
*       xgDevice                - GT_TRUE for XG device, GT_FALSE for GE device
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC usage has actual influence
*       only for XG devices, where it is used to get the port number from
*       the power up SERDES bitmap (networkSerdesPowerUpBmp).
*       For GE devices the macro does nothing usefull since previouse one
*       (PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC) make all necessary checks.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3HwPpPllCalibrationSeq
(
    IN GT_U8    devNum,
    IN GT_U32   networkSerdesPowerUpBmp,
    IN GT_BOOL  xgDevice
)
{
    GT_U32  *regAddrPtr; /* pointer to register address */
    GT_U32  serdesNum;   /* SERDES number - for GE devices SERDES number is a */
                         /* actually also the port number, for XG devices it  */
                         /* is used like port number literaly */

    /* Turn SERDES PLL and Impedance calibration OFF */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                       macRegs.perPortRegs[serdesNum].serdesCalibConf1[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, xgDevice,
                                                      regAddrPtr, 0x3, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    /* Turn SERDES PLL calibration ON */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                       macRegs.perPortRegs[serdesNum].serdesCalibConf1[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, xgDevice,
                                                      regAddrPtr, 0x3, 0x2) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    /* Turn SERDES Impedance calibration ON */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                       macRegs.perPortRegs[serdesNum].serdesCalibConf1[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, xgDevice,
                                                      regAddrPtr, 0x3, 0x3) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 2 ms */
    cpssOsTimerWkAfter(2);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3HwPpCheckAllPortSyncOk
*
* DESCRIPTION:
*       Check if all ports are in sync OK state
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum                  - device number
*       networkSerdesPowerUpBmp - bitmap of network SERDES to be power UP
*       xgDevice                - GT_TRUE for XG device, GT_FALSE for GE device
*
* OUTPUTS:
*       syncOkPtr               - (pointer to) GT_TRUE: all port in sync OK,
*                                              GT_FALSE: at least one port not
*                                                        in sync OK
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC usage has actual influence
*       only for XG devices, where it is used to get the port number from
*       the power up SERDES bitmap (networkSerdesPowerUpBmp).
*       For GE devices the macro does nothing usefull since previouse one
*       (PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC) make all necessary checks.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3HwPpCheckAllPortSyncOk
(
    IN GT_U8    devNum,
    IN GT_U32   networkSerdesPowerUpBmp,
    IN GT_BOOL  xgDevice,
    OUT GT_BOOL *syncOkPtr
)
{
    GT_U32  regAddr;     /* register's address   */
    GT_U32  regValue;    /* register value       */
    GT_U32  serdesNum;   /* SERDES number - for GE devices SERDES number is a */
                         /* actually also the port number, for XG devices it  */
                         /* is used like port number literaly */
    GT_U32  lane;        /* lane number          */

    /* Check Port Sync Status */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        if( xgDevice == GT_TRUE )
        {
            for( lane = 0 ; lane < 4 ; lane++ )
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[serdesNum].laneStatus[lane];

                if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 4, 1, &regValue) != GT_OK)
                {
                    return GT_HW_ERROR;
                }

#ifdef  ASIC_SIMULATION
                /* simulation not need to simulate 'syncOk' */
                regValue = 1;
#endif /*ASIC_SIMULATION*/
                if( regValue == 0 )
                {
                    break;
                }
            }
            if( lane < 4 )
            {
                break;
            }
        }
       else
       {
           regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[serdesNum].macStatus;

           if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 14, 1, &regValue) != GT_OK)
           {
               return GT_HW_ERROR;
           }

#ifdef  ASIC_SIMULATION
           /* simulation not need to simulate 'syncOk' */
           regValue = 1;
#endif /*ASIC_SIMULATION*/

           if( regValue == 0 )
           {
               break;
           }
       }
    }

    if( serdesNum == PRV_CH3_NETWORK_SERDES_NUM_CNS )
    {
        /* all ports received SyncOk indication */
        *syncOkPtr = GT_TRUE;
    }
    else
    {
        /* at least one port did not receive SyncOk indication */
        *syncOkPtr = GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3HwPpPllUnlockWorkaroundSave
*
* DESCRIPTION:
*       Save values for later restore after PLL unlock workaround.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum                  - device number
*       networkSerdesPowerUpBmp - bitmap of network SERDES to be power UP
*
* OUTPUTS:
*       speedPllPtr             - (pointer to) restore SERDES speed PLL values
*       ffePtr                  - (pointer to) restore SERDES FFE values
*       txPreEmphasisBmpPtr     - (pointer to) restore SERDES Tx Pre-emphasis values
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC usage has actual influence
*       only for XG devices, where it is used to get the port number from
*       the power up SERDES bitmap (networkSerdesPowerUpBmp).
*       For GE devices the macro does nothing usefull since previouse one
*       (PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC) make all necessary checks.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3HwPpPllUnlockWorkaroundSave
(
    IN GT_U8    devNum,
    IN GT_U32   networkSerdesPowerUpBmp,
    OUT GT_U16   *speedPllPtr,
    OUT GT_U16   *ffePtr,
    OUT GT_U32   *txPreEmphasisBmpPtr
)
{
    GT_U32  regAddr;     /* register's address  */
    GT_U32  regData;     /* register data       */
    GT_U32  serdesNum;   /* SERDES number - for GE devices SERDES number is a */
                         /* actually also the port number, for XG devices it  */
                         /* is used like port number literaly */

    *txPreEmphasisBmpPtr = 0x0;

    /* For XG devices settings for all 4 lanes in the group must be same, */
    /* therefore we can save data from one of the group lanes for all.    */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        /* save SERDES Speed PLL value   */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesSpeed1[0];

        if (prvCpssDrvHwPpReadRegBitMask(devNum, regAddr, 0x7FF, &regData) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        speedPllPtr[serdesNum] = (GT_U16)regData;

        /* save SERDES FFE value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesRxConfReg1[0];

        if (prvCpssDrvHwPpReadRegBitMask(devNum, regAddr, 0x3FF, &regData) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        ffePtr[serdesNum] = (GT_U16)regData;

        /* save SERDES Tx Pre-emphasis value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesTxConfReg1[0];

        if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 7, 1, &regData) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        *txPreEmphasisBmpPtr |= ((0x1 & regData) << serdesNum);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3HwPpPllUnlockWorkaroundRestore
*
* DESCRIPTION:
*       Back to normal operation mode and restore previously saved values for
*       PLL unlock workaround.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum                  - device number
*       networkSerdesPowerUpBmp - bitmap of network SERDES to be power UP
*       xgDevice                - GT_TRUE for XG device, GT_FALSE for GE device
*       speedPllPtr             - (pointer to) restore SERDES speed PLL values
*       ffePtr                  - (pointer to) restore SERDES FFE values
*       txPreEmphasisBmp        - restore SERDES Tx Pre-emphasis values
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*       PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC usage has actual influence
*       only for XG devices, where it is used to get the port number from
*       the power up SERDES bitmap (networkSerdesPowerUpBmp).
*       For GE devices the macro does nothing usefull since previouse one
*       (PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC) make all necessary checks.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3HwPpPllUnlockWorkaroundRestore
(
    IN GT_U8    devNum,
    IN GT_U32   networkSerdesPowerUpBmp,
    IN GT_BOOL  xgDevice,
    IN GT_U16   *speedPllPtr,
    IN GT_U16   *ffePtr,
    IN GT_U32   txPreEmphasisBmp
)
{
    GT_U32  regAddr;     /* register's address   */
    GT_U32  *regAddrPtr; /* pointer to register address */
    GT_U32  serdesNum;   /* SERDES number - for GE devices SERDES number is a */
                         /* actually also the port number, for XG devices it  */
                         /* is used like port number literaly */
    GT_U32  laneNum;     /* number of lanes per port */
    GT_U32  i;           /* loop index */

    if( xgDevice == GT_TRUE )
    {
        /* XG port */
        laneNum = 4;
    }
    else /* xgDevice == GT_FALSE */
    {
        /* GE port */
        laneNum = 1;
    }

    /* For XG devices settings for all 4 lanes in the group must be same, */
    /* therefore we can use data from one of the group lanes for all.     */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        /* restore SERDES Speed PLL value   */
        for( i = 0 ; i < laneNum ; i++ )
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[serdesNum].serdesSpeed1[i];

            if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0x400, (GT_U32)speedPllPtr[serdesNum]) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        /* restore SERDES FFE value, unset loopback */
        for( i = 0 ; i < laneNum ; i++ )
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[serdesNum].serdesRxConfReg1[i];

            if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0x3FF, (GT_U32)ffePtr[serdesNum]) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        /* restore SERDES Tx Pre-emphasis value */
        for( i = 0 ; i < laneNum ; i++ )
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[serdesNum].serdesTxConfReg1[i];

            if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 7, 1,
                                          ((txPreEmphasisBmp >> serdesNum) & 0x1)) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        /* Set Squelch Detector to control the CDR */
        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                       macRegs.perPortRegs[serdesNum].serdesDtlConfReg5[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, xgDevice,
                                                      regAddrPtr, 0x100, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        if( xgDevice == GT_TRUE )
        {
            /* Don't Bypass Signal Detect indication from the SERDES */
            regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                           macRegs.perPortRegs[serdesNum].laneConfig0[0]);

            if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, xgDevice,
                                                          regAddrPtr, 0x40, 0x0) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
    }

    /* wait 3 ms */
    cpssOsTimerWkAfter(3);

    if (prvCpssDxCh3HwPpPllCalibrationSeq(devNum, networkSerdesPowerUpBmp, xgDevice) != GT_OK)
    {
        return GT_HW_ERROR;
    }


    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3HwPpPllUnlockWorkaround3rdWrapper
*
* DESCRIPTION:
*       3rd wrapper of workaround to eliminate the PLL unlocking issue for G ports.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum                  - device number
*       networkSerdesPowerUpBmp - bitmap of network SERDES to be power UP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3HwPpPllUnlockWorkaround3rdWrapper
(
    IN GT_U8    devNum,
    IN GT_U32   networkSerdesPowerUpBmp
)
{
    GT_U32  regAddr;     /* register's address  */
    GT_U32  serdesNum;   /* SERDES number       */

    /* Reset Rx & Digital*/
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesReset[0];

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xFF0, 0xFF0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 5 ms */
    cpssOsTimerWkAfter(5);

    /* Unreset Digital */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesReset[0];

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xFF0, 0xF00) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    /* Unreset Rx */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesReset[0];

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xFF0, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3HwPpPllUnlockWorkaround1stWrapper
*
* DESCRIPTION:
*       1st wrapper of workaround to eliminate the PLL unlocking issue for XG ports.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum                  - device number
*       networkSerdesPowerUpBmp - bitmap of network SERDES to be power UP
*       variableDelay           - delay used during the sequence the value
*                                 of which varies between diffrent runs.
*       speedPllPtr             - (pointer to) restore SERDES speed PLL values
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*       PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC usage has actual influence
*       only for XG devices, where it is used to get the port number from
*       the power up SERDES bitmap (networkSerdesPowerUpBmp).
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3HwPpPllUnlockWorkaround1stWrapper
(
    IN GT_U8    devNum,
    IN GT_U32   networkSerdesPowerUpBmp,
    IN GT_U32   variableDelay,
    IN GT_U16   *speedPllPtr
)
{
    GT_U32  regAddr;     /* register's address   */
    GT_U32  *regAddrPtr; /* pointer to register address */
    GT_U32  serdesNum;   /* SERDES number - for XG devices it  */
                         /* is used like port number literaly  */
    GT_U32  lane;        /* lane loop index      */

    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        /* Set PLL to 1.25G mode */
        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                       macRegs.perPortRegs[serdesNum].serdesSpeed1[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, GT_TRUE,
                                                      regAddrPtr, 0x7FF, 0x13A) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* Change Icp */
        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                       macRegs.perPortRegs[serdesNum].serdesSpeed2[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, GT_TRUE,
                                                      regAddrPtr, 0xF, 0xF) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 3 ms */
    cpssOsTimerWkAfter(3);

    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        /* Reset XPCS */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].xgGlobalConfReg0;

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0x1, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* Reset Analog, Rx & Digital */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesReset[0];

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xFFF, 0xFFF) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }


    /* Power down the Common current and voltage reference circuits power control */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[serdesNum].serdesCommon[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, GT_TRUE,
                                                      regAddrPtr, 0x1, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    /* Power down SERDES current and voltage reference circuits power control, */
    /* and PLL frequency synthesizer power control */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesPowerUp1[0];

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xF, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* Power down SERDES Rx and Tx power control */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesPowerUp2[0];

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xF, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    /* Reset Rx Phase-Interpolator */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                       macRegs.perPortRegs[serdesNum].serdesRxConfReg1[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, GT_TRUE,
                                                      regAddrPtr, 0x4000, 0x4000) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    /* Unreset Rx Phase-Interpolator */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                       macRegs.perPortRegs[serdesNum].serdesRxConfReg1[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, GT_TRUE,
                                                      regAddrPtr, 0x4000, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    /* Power up the Common current and voltage reference circuits power control */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[serdesNum].serdesCommon[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, GT_TRUE,
                                                      regAddrPtr, 0x1, 0x1) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 4,8 or 40 ms */
    cpssOsTimerWkAfter(2*variableDelay);

    /* Power up SERDES current and voltage reference circuits power control, */
    /* and PLL frequency synthesizer power control */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesPowerUp1[0];

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xFF, 0xFF) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* Power up SERDES Rx power control */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesPowerUp2[0];

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xFF, 0xF) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 2,4 or 20 ms */
    cpssOsTimerWkAfter(variableDelay);

    /* Unreset Analog */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesReset[0];

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xFFF, 0xFF0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    if (prvCpssDxCh3HwPpPllCalibrationSeq(devNum, networkSerdesPowerUpBmp, GT_TRUE) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        /* Set PLL to 3.125G mode */
        for( lane = 0 ; lane < 4 ; lane++ )
        {
            /* For XG devices settings for all 4 lanes in the group must be same, */
            /* therefore we can use data from one of the group lanes for all.     */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[serdesNum].serdesSpeed1[lane];

            if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0x7FF, (GT_U32)speedPllPtr[serdesNum]) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        /* Change Icp */
        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                       macRegs.perPortRegs[serdesNum].serdesSpeed2[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, GT_TRUE,
                                                      regAddrPtr, 0xF, 0x6) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 3 ms */
    cpssOsTimerWkAfter(3);

    /* Power up SERDES Tx power control */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesPowerUp2[0];

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xFF, 0xFF) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    /* Reset TxSyncFIFO */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                       macRegs.perPortRegs[serdesNum].serdesMiscConf[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, GT_TRUE,
                                                      regAddrPtr, 0x8, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    /* Unreset Rx */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesReset[0];

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xFFF, 0x0F0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    if (prvCpssDxCh3HwPpPllCalibrationSeq(devNum, networkSerdesPowerUpBmp, GT_TRUE) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Unreset Digital */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesReset[0];

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xFFF, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    /* Unreset TxSyncFIFO */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                       macRegs.perPortRegs[serdesNum].serdesMiscConf[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, GT_TRUE,
                                                      regAddrPtr, 0x8, 0x8) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    /* Unreset XPCS */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].xgGlobalConfReg0;

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0x1, 0x1) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3HwPpPllUnlockWorkaroundWrapper
*
* DESCRIPTION:
*       Wrapper of workaround to eliminate the PLL unlocking issue.
*       The sequence of operations in this wrapper is used during:
*       1. 1st wrapper for GE ports.
*       2. 2nd wrapper for GE ports.
*       3. 2nd wrapper for XG ports.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum                  - device number
*       networkSerdesPowerUpBmp - bitmap of network SERDES to be power UP
*       xgDevice                - GT_TRUE for XG device, GT_FALSE for GE device
*       variableDelay           - delay used during the sequence the value
*                                 of which varies between diffrent runs.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*       PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC usage has actual influence
*       only for XG devices, where it is used to get the port number from
*       the power up SERDES bitmap (networkSerdesPowerUpBmp).
*       For GE devices the macro does nothing usefull since previouse one
*       (PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC) make all necessary checks.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3HwPpPllUnlockWorkaroundWrapper
(
    IN GT_U8    devNum,
    IN GT_U32   networkSerdesPowerUpBmp,
    IN GT_BOOL  xgDevice,
    IN GT_U32   variableDelay
)
{
    GT_U32  regAddr;     /* register address */
    GT_U32  *regAddrPtr; /* pointer to register address */
    GT_U32  serdesNum;   /* SERDES number - for GE devices SERDES number is a */
                         /* actually also the port number, for XG devices it  */
                         /* is used like port number literaly */

    /* Power down the Common current and voltage reference circuits power control */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[serdesNum].serdesCommon[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, xgDevice,
                                                      regAddrPtr, 0x1, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    /* Power down SERDES current and voltage reference circuits power control */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesPowerUp1[0];

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xF, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    /* Power up the Common current and voltage reference circuits power control */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[serdesNum].serdesCommon[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, xgDevice,
                                                      regAddrPtr, 0x1, 0x1) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 4, 8 or 40 ms */
    cpssOsTimerWkAfter(2*variableDelay);

    /* Power up SERDES current and voltage reference circuits power control */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[serdesNum].serdesPowerUp1[0];

        if (prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xF, 0xF) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* wait 2, 4 or 20 ms */
    cpssOsTimerWkAfter(variableDelay);

    if (prvCpssDxCh3HwPpPllCalibrationSeq(devNum, networkSerdesPowerUpBmp, xgDevice) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3HwPpPllUnlockWorkaround
*
* DESCRIPTION:
*       Workaround to eliminate the PLL unlocking issue.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum                  - device number
*       networkSerdesPowerUpBmp - bitmap of network SERDES to be power UP
*       xgDevice                - GT_TRUE for XG device, GT_FALSE for GE device
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*       PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC usage has actual influence
*       only for XG devices, where it is used to get the port number from
*       the power up SERDES bitmap (networkSerdesPowerUpBmp).
*       For GE devices the macro does nothing usefull since previouse one
*       (PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC) make all necessary checks.
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3HwPpPllUnlockWorkaround
(
    IN GT_U8    devNum,
    IN GT_U32   networkSerdesPowerUpBmp,
    IN GT_BOOL  xgDevice
)
{
    GT_U32  *regAddrPtr; /* pointer to register address */
    GT_U32  regData;     /* register data */
    GT_U32  serdesNum;   /* SERDES number - for GE devices SERDES number is a */
                         /* actually also the port number, for XG devices it  */
                         /* is used like port number literaly */
    GT_U32  i, j;        /* loop iteration indexes */
    GT_U16  speedPll[PRV_CH3_NETWORK_SERDES_NUM_CNS];       /* array to save SERDES speed PLL */
    GT_U16  ffe[PRV_CH3_NETWORK_SERDES_NUM_CNS];            /* array to save SERDES FFE       */
    GT_U32  txPreEmphasisBmp;   /* bitmap to save SERDES Tx Pre-emphasis */
    GT_BOOL syncOk;             /* sync OK indication for all ports */
    GT_U32  variableDelay;      /* delay used during the sequence the value */
                                /* of which varies between diffrent runs.   */

    if( prvCpssDxCh3HwPpPllUnlockWorkaroundSave(devNum,
                                                networkSerdesPowerUpBmp,
                                                &speedPll[0],
                                                &ffe[0],
                                                &txPreEmphasisBmp) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Set Serdes Analog Loopback Testing Mode */
    for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

        /* Set loopback, Set FFE */
        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                       macRegs.perPortRegs[serdesNum].serdesRxConfReg1[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, xgDevice,
                                                      regAddrPtr, 0x3FF, 0x7) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* Disable Tx Pre-emphasis */
        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                       macRegs.perPortRegs[serdesNum].serdesTxConfReg1[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, xgDevice,
                                                      regAddrPtr, 0x80, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* Unset Squelch Detector to control the CDR */
        regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                       macRegs.perPortRegs[serdesNum].serdesDtlConfReg5[0]);

        if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, xgDevice,
                                                      regAddrPtr, 0x100, 0x100) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        if( xgDevice == GT_TRUE )
        {
            /* Bypass Signal Detect indication from the SERDES */
            regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                           macRegs.perPortRegs[serdesNum].laneConfig0[0]);

           if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, xgDevice,
                                                      regAddrPtr, 0x40, 0x40) != GT_OK)
           {
               return GT_HW_ERROR;
           }
        }
    }

    /* wait 1 ms */
    cpssOsTimerWkAfter(1);

    for( i = 0 ; i < PLL_UNLOCK_WORKAROUND_MAX_LOOPS_1ST_WRAPPER_CNS ; i++ )
    {
        if (prvCpssDxCh3HwPpCheckAllPortSyncOk(devNum,
                                               networkSerdesPowerUpBmp,
                                               xgDevice,
                                               &syncOk) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        if( syncOk == GT_TRUE )
        {
            /* all ports received SyncOk indication */
            break;
        }

        if( xgDevice == GT_TRUE )
        {
            if (prvCpssDxCh3HwPpPllUnlockWorkaround1stWrapper(devNum,
                                                              networkSerdesPowerUpBmp,
                                                              2, &speedPll[0]) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
        else
        {
            if (prvCpssDxCh3HwPpPllUnlockWorkaroundWrapper(devNum, networkSerdesPowerUpBmp,
                                                           xgDevice, 2) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
    }

    if( i < PLL_UNLOCK_WORKAROUND_MAX_LOOPS_1ST_WRAPPER_CNS )
    {
        /* no PLL unlock problem occured or first wrapper solved the problem */
        return prvCpssDxCh3HwPpPllUnlockWorkaroundRestore(devNum,
                                                          networkSerdesPowerUpBmp,
                                                          xgDevice,
                                                          &speedPll[0],
                                                          &ffe[0],
                                                          txPreEmphasisBmp);
    }

    for( i = 0 ; i < PLL_UNLOCK_WORKAROUND_MAX_LOOPS_SET_CHANGE_CNS ; i++ )
    {
        if( i < 5 )
        {
            variableDelay = 2;
        }
        else if ( i < 10)
        {
            variableDelay = 4;
        }
        else
        {
            variableDelay = 20;
        }

        if( xgDevice == GT_TRUE )
        {
            for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
            {
                PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
                PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

                regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                               macRegs.perPortRegs[serdesNum].serdesSpeed1[0]);

                /* Set Speed PLL to 1.25G */
                if (prvCpssDxCh3HwPpPortSerdesWriteRegBitMask(devNum, xgDevice,
                                                              regAddrPtr, 0x400, 0x0) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
            }

            /* wait 3 ms */
            cpssOsTimerWkAfter(3);

            if (prvCpssDxCh3HwPpPllUnlockWorkaroundWrapper(devNum, networkSerdesPowerUpBmp,
                                                           xgDevice, 2) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
        else /* xgDevice == GT_FALSE */
        {
            for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
            {
                PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);

                regAddrPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                               macRegs.perPortRegs[serdesNum].serdesSpeed1[0]);

                /* Toggle Speed PLL */
                if (prvCpssDrvHwPpReadRegBitMask(devNum, *regAddrPtr, 0x400, &regData) != GT_OK)
                {
                    return GT_HW_ERROR;
                }

                regData ^= 0x400;

                if (prvCpssDrvHwPpWriteRegBitMask(devNum, *regAddrPtr, 0x400, regData) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
            }

            /* wait 3 ms */
            cpssOsTimerWkAfter(3);
        }

        for( j = 0 ; j < PLL_UNLOCK_WORKAROUND_MAX_LOOPS_2ND_WRAPPER_CNS ; j++ )
        {
            if( xgDevice == GT_TRUE )
            {
                if (prvCpssDxCh3HwPpPllUnlockWorkaround1stWrapper(devNum,
                                                                  networkSerdesPowerUpBmp,
                                                                  variableDelay, &speedPll[0]) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
            }
            else
            {
                if (prvCpssDxCh3HwPpPllUnlockWorkaroundWrapper(devNum, networkSerdesPowerUpBmp,
                                                               xgDevice, variableDelay) != GT_OK)
                {
                   return GT_HW_ERROR;
                }
            }

            if (prvCpssDxCh3HwPpCheckAllPortSyncOk(devNum,
                                                   networkSerdesPowerUpBmp,
                                                   xgDevice,
                                                   &syncOk) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            if( syncOk == GT_TRUE )
            {
                /* all ports received SyncOk indication */
                break;
            }
        }

        if( j < PLL_UNLOCK_WORKAROUND_MAX_LOOPS_2ND_WRAPPER_CNS )
        {
            /* all ports received SyncOk indication */
            break;
        }
    }

    if( i < PLL_UNLOCK_WORKAROUND_MAX_LOOPS_SET_CHANGE_CNS )
    {
        /* second wrapper solved the PLL unlock problem */
        return prvCpssDxCh3HwPpPllUnlockWorkaroundRestore(devNum,
                                                          networkSerdesPowerUpBmp,
                                                          xgDevice,
                                                          &speedPll[0],
                                                          &ffe[0],
                                                          txPreEmphasisBmp);
    }


    if( xgDevice == GT_FALSE )
    {
        /* restore SERDES Speed PLL value   */
        for( serdesNum = 0 ; serdesNum < PRV_CH3_NETWORK_SERDES_NUM_CNS ; serdesNum++ )
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesPowerUpBmp);
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, serdesNum);

            regAddrPtr = &PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[serdesNum].serdesSpeed1[0];

            if (prvCpssDrvHwPpWriteRegBitMask(devNum, *regAddrPtr,
                                              0x400, (GT_U32)speedPll[serdesNum]) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        for( i = 0 ; i < PLL_UNLOCK_WORKAROUND_MAX_LOOPS_3RD_WRAPPER_CNS ; i++)
        {
            if (prvCpssDxCh3HwPpPllUnlockWorkaround3rdWrapper(devNum,
                                                              networkSerdesPowerUpBmp) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            if (prvCpssDxCh3HwPpCheckAllPortSyncOk(devNum,
                                                   networkSerdesPowerUpBmp,
                                                   xgDevice,
                                                   &syncOk) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            if( syncOk == GT_TRUE )
            {
                /* all ports received SyncOk indication */
                break;
            }
        }

        if( i < PLL_UNLOCK_WORKAROUND_MAX_LOOPS_3RD_WRAPPER_CNS )
        {
            /* third wrapper solved the PLL unlock problem */
            return prvCpssDxCh3HwPpPllUnlockWorkaroundRestore(devNum,
                                                          networkSerdesPowerUpBmp,
                                                          xgDevice,
                                                          &speedPll[0],
                                                          &ffe[0],
                                                          txPreEmphasisBmp);
        }
    }

    return GT_FAIL;
}

/*******************************************************************************
* hwPpCheetah3RegsDefault
*
* DESCRIPTION:
*       Set mandatory default values for Cheetah registers
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum  - device number
*       ppPhase1ParamsPtr  - Packet processor hardware specific parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpCheetah3RegsDefault
(
    IN GT_U8                                       devNum,
    IN CPSS_DXCH_PP_PHASE1_INIT_INFO_STC           *ppPhase1ParamsPtr
)
{
    GT_U32 regAddr;     /* register's address */
    GT_U32 devType;     /* device type          */
    GT_BOOL ch3XgDevice; /* XG Cheetah 3 family */
    GT_STATUS rc;       /* return code */
    GT_U32    replacedRow; /* row number replaced in the eFuse WA */
    GT_U32    xValid;      /* X value to invalidate row */
    GT_U32    yValid;      /* Y value to invalidate row */
    GT_U32    networkSerdesPowerUpBmp;  /* bitmap of network SERDES to be power UP */
    GT_U32    i;           /* loop counter */

    /* start with the common settings */
    rc =  hwPpCheetahCommonRegsDefault(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* CPSS_98DX8110_CNS for CH3 XG device, else CH3 G device */
    devType = PRV_CPSS_PP_MAC(devNum)->devType;
    switch(devType)
    {
        case PRV_CPSS_CHEETAH3_XG_PORTS_DEVICES_CASES_MAC:
            ch3XgDevice = GT_TRUE;
            break;
        default:
            ch3XgDevice = GT_FALSE;
            break;
    }

    if(GT_TRUE ==
        PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH2_RM_BRG_LAYER_4_SANITY_CHECKS_WA_E))
    {
        /* Reset NST( Network Shield ) registers  -
           Bridge Global Configuration Registers */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     bridgeRegs.bridgeGlobalConfigRegArray[0];

        /* Disable dropping ARP packets with MAC SA not equal to the ARP
           payload data contains a field called <source hardware address>
           + all TCP Sanity checks */
        if(prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0x20030000, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     bridgeRegs.bridgeGlobalConfigRegArray[2];
        /* Disable all TCP related bits(0-4) + disable ICMP dropping(bit 5) */
        if(prvCpssDrvHwPpWriteRegBitMask(devNum,regAddr,0x0000003F, 0x0)
            != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* Enable tagged packets transmitted from the device have their CFI bit set
     according to the received packet CFI bit if the received packet was tagged,
     or set to 0 if the received packet is untagged.*/
    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        if(prvCpssDrvHwPpSetRegField(devNum, regAddr, 22, 1, 1) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
        if(prvCpssDrvHwPpSetRegField(devNum, regAddr, 28, 1, 1) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* RM#2003 */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_RM_BM_GE_XG_PORT_GROUP_LIMIT_CFG_REG_WA_E))
    {
        /* Buffer Management GbE Ports Group Limits Configuration Registers */
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x03000004, 0xBFEFFDFF) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        /* Buffer Management HyperG.Stack Ports Group Limits Configuration Registers */
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x03000008, 0xBFEFFDFF) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }
    /* RM#2007 */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_RM_GE_PORTS_MODE1_REG_WA_E))
    {
        /* LMS GigE Ports Mode Register1 */
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x05804144, 0x00000000) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }
    /* RM#2008 */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_RM_XG_PORTS_MODE_REG_WA_E))
    {
        /* LMS HyperG.Stack Ports Mode Register */
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x04804144, 0x000000FF) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }
    /* RM#2024 */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_RM_LMS0_1_GROUP1_LED_INTERFACE_WA_E))
    {
        /* LED Interface0 Control Register 1 and Class6 Manipulation Register (for ports 0 through 11) */
        /* <LEDClkInvert> */
        if (prvCpssDrvHwPpSetRegField(devNum, 0x04804100, 16, 1, 1) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        /* LED Interface1 Control Register1 and class6 manipulation (for ports 12 through 23) */
        /* <LEDClkInvert> */
        if (prvCpssDrvHwPpSetRegField(devNum, 0x05804100, 16, 1, 1) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }
    /* RM#2011 */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_RM_TCAM_PLL_REG_WA_E))
    {
        /* Stop TCAM PLL */
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x000000BC, 0x00258D24) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        /* Stop MPPM PLL */
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x000000B8, 0x00258D24) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        /* Configure PLL clocks to use register values */
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x0000005C, 0x0000051D) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        /* Keep MPPM PLL stoped & Start configuration to 250MHz */
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x000000B8, 0x60258D24) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        /* Configure TCAM and MPPM PLL to the 250 MHz clock */
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x000000D0, 0x18E8743A) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        /* Start TCAM PLL */
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x000000BC, 0x08258D24) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        /* Start MPPM PLL */
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x000000B8, 0x68258D24) != GT_OK)
        {
            return GT_HW_ERROR;
        }

    }

    /* RM#2026 */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_RM_MAC2ME_LOW_REG_WA_E))
    {
        /* Change the default value of MAC2ME Registers Low register */
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x0B800700, 0x0000FFF0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x0B800710, 0x0000FFF0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x0B800720, 0x0000FFF0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x0B800730, 0x0000FFF0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x0B800740, 0x0000FFF0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x0B800750, 0x0000FFF0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x0B800760, 0x0000FFF0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x0B800770, 0x0000FFF0) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* RM#2061 */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_RM_AN_CNF_REG_WA_E))
    {
        /* Change the default value of PHY Auto-Negotiation Configuration Register0 and PHY Auto-
           Negotiation Configuration Register2 */
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x04004034, 0x00000140) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        if (prvCpssDrvHwPpWriteRegister(devNum, 0x05004034, 0x00000140) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* FEr#2191 */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_TCAM_EFUSE_NOT_TRIGGERED_AUTO_WA_E))
    {
        /* WA for Policy TCAM */
        rc = prvCpssDxCh3TcamEfuseWa(devNum,
                                     PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.policyTcamTest.pointerRelReg,
                                     14,
                                     PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.policyTcamTest.configStatusReg,
                                     &replacedRow);

        if ( rc != GT_OK )
        {
            return rc;
        }

        if( replacedRow != NO_TCAM_ROW_TO_REPLACE_CNS )
        {
            /* any "valid bit" value in the key not will be matched */
            xValid = PRV_CPSS_DXCH_PCL_MP_TO_X_MAC(0, 1);
            yValid = PRV_CPSS_DXCH_PCL_MP_TO_Y_MAC(0, 1);

            for( i = 0; i < 4 ; i++)
            {
                /* invalidate standard part of row */
                rc = prvCpssDxChPclTcamStdRuleValidStateXandYSet(
                    devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                    (replacedRow + (i * PRV_DXCH3_PCL_TCAM_RAW_NUMBER_CNS)),
                    xValid, yValid);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }

        /* WA for Router TCAM */
        rc = prvCpssDxCh3TcamEfuseWa(devNum,
                                     PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerTcamTest.pointerRelReg,
                                     14,
                                     PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerTcamTest.configStatusReg,
                                     &replacedRow);

        if ( rc != GT_OK )
        {
            return rc;
        }

        if( replacedRow != NO_TCAM_ROW_TO_REPLACE_CNS )
        {
            rc = prvCpssDxChRouterTunnelTermTcamInvalidateLine(
                devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS, replacedRow);

            if ( rc != GT_OK )
            {
                return rc;
            }
        }
    }

    if (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_TCAM_REPAIR_WA_E))
    {
        rc = prvCpssDxCh3TcamWa(devNum);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if(ch3XgDevice == GT_TRUE)
    {
        /* RM#2063 */
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                    PRV_CPSS_DXCH3_RM_BM_TX_FIFO_THRESHOLD_CONF_REG_WA_E))
        {
            /* RM#2063 : Buffer Memory Tx Fifo Threshold and MPPM Access Arbiter Configuration Register */
            /* Update number of lines for NP in the Tx FIFO (bits 0-3, from 0x6 to 0xC) */
            if (prvCpssDrvHwPpWriteRegister(devNum, 0x0780006C, 0x444E610C) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH3_RM_CPU_ACCESS_TO_FDB_UNDER_TRAFFIC_WA_E))
    {
        /* WA for: Under heavy traffic stress, FDB accesses from the CPU can get stuck */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 6, 1, 1);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        if (hwPpCheetah3PortsAndSerdesRegsDefault(devNum, ppPhase1ParamsPtr,
                                                  &networkSerdesPowerUpBmp) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        if (prvCpssDxCh3HwPpPllUnlockWorkaround(devNum,
                                                networkSerdesPowerUpBmp,
                                                ch3XgDevice) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH3_TOGGLE_DEV_EN_UNDER_TRAFFIC_WA_E))
    {
        /* Toggling Device Enable under traffic may cause system hang. */
        /* (FEr#2261) */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 8, 1, 1);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpXcatA1PortsAndSerdesRegsDefault
*
* DESCRIPTION:
*       Set mandatory default values for Xcat ports and SERDES registers
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum  - device number
*       ppPhase1ParamsPtr  - Packet processor hardware specific parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpXcatA1PortsAndSerdesRegsDefault
(
    IN GT_U8                                   devNum,
    IN CPSS_DXCH_PP_PHASE1_INIT_INFO_STC       *ppPhase1ParamsPtr

)
{
    GT_U32 regAddr;     /* register's address   */
    GT_U32 regValue;    /* register value       */
    GT_U8  portNum;     /* port number          */
    GT_U32 devType;     /* device type          */
    GT_BOOL xcatGigDevice;
    GT_U32 serdesNum;   /* SERDES number        */
    GT_U32 networkSerdesBmp;   /* device SERDES bitmap */
    GT_STATUS rc;
    GT_U32 i;
    GT_U32 startSeredesIndx;
    GT_U32 xgMode;
    GT_BOOL xcat2Dev = GT_FALSE;
    GT_U32 (*lpSerdesConfigArr)[PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E];
    GT_U32 (*lpSerdesExtConfigArr)[2];
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed;
    GT_U32 numOfSredesOnStackPort;

    /* define type of xCat device either Gig or Fast */
    devType = PRV_CPSS_PP_MAC(devNum)->devType;
    switch(devType)
    {
        case PRV_CPSS_DXCH_XCAT_FE_DEVICES_CASES_MAC:
        case PRV_CPSS_DXCH_XCAT2_FE_DEVICES_CASES_MAC:
            xcatGigDevice = GT_FALSE;
            startSeredesIndx = PRV_CPSS_XCAT_NETWORK_PORTS_SERDES_NUM_CNS;
            break;
        default:
            xcatGigDevice = GT_TRUE;
            startSeredesIndx = 0;
            break;
    }
    /* Set device SERDES bitmap */
    prvCpssDxXcatNetworkSerdesBitmapGet(devNum,
                                       &ppPhase1ParamsPtr->powerDownPortsBmp,
                                       &networkSerdesBmp);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        xcat2Dev = GT_TRUE;

    /* set Serdes reference clock */
    switch(ppPhase1ParamsPtr->serdesRefClock)
    {
    case CPSS_DXCH_PP_SERDES_REF_CLOCK_INTERNAL_125_E:
        /* set device_cnfg register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.sampledAtResetReg;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,24,2,2);
        if (rc != GT_OK)
            return rc;
        break;
    case CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_125_SINGLE_ENDED_E:

        if(GT_TRUE == xcat2Dev)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.sampledAtResetReg;
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,30,1,0);
            if (rc != GT_OK)
                return rc;

            /* disable PECL receiver */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.analogCfgReg;
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,5,1,0);
            if (rc != GT_OK)
                return rc;
        }
        else
        {
            /* set device_cnfg register */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.sampledAtResetReg;
            rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,24,2,1);
            if (rc != GT_OK)
                return rc;
        }

        /* set single ended clock input in analog_cnfg register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.analogCfgReg;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,9,1,0);
        if (rc != GT_OK)
            return rc;

        break;
    case CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_125_DIFF_E:
        /* set device_cnfg register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.sampledAtResetReg;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,24,2,1);
        if (rc != GT_OK)
            return rc;

        /* set differential clock input in analog_cnfg register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.analogCfgReg;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,9,1,1);
        if (rc != GT_OK)
            return rc;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* check if cpss makes serdes default initialization */
    if (ppPhase1ParamsPtr->initSerdesDefaults == GT_TRUE)
    {
        /* get flexLink ports mode */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.sampledAtResetReg;
        if (prvCpssDrvHwPpGetRegField(devNum,regAddr,26,4,&xgMode) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        if(GT_TRUE == xcat2Dev)
        {
            serdesNum = PRV_CPSS_XCAT2_SERDES_NUM_CNS;
            lpSerdesConfigArr = lpSerdesConfig_xCat2;
            lpSerdesExtConfigArr = lpSerdesExtConfig_xCat2_125Clk;
        }
        else
        {
            serdesNum = PRV_CPSS_XCAT_SERDES_NUM_CNS;
            lpSerdesConfigArr = lpSerdesConfig;
            lpSerdesExtConfigArr = lpSerdesExtConfig;
        }

        /* set default values to all serdes */
        for (i=startSeredesIndx;i<serdesNum;i++)
        {
            if(i < PRV_CPSS_XCAT_NETWORK_PORTS_SERDES_NUM_CNS)
            {
                /* Pre Configuration enables access to serdes registers */
                rc = prvCpssDxChLpPreInitSerdesSequence(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                        i,
                                                        lpSerdesExtConfigArr[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG1_E][1]);
                if (rc != GT_OK)
                    return rc;

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].transmitterReg1;
                rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr,
                                                    lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG1_E]
                                                                        [PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_E]);
                if (rc != GT_OK)
                    return rc;

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].ffeReg;
                rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr,
                                                    lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_FFE_REG0_E]
                                                                        [PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_E]);
                if (rc != GT_OK)
                    return rc;
            }
            else
            {
                /* for ports in SGMII mode set serdes speed to 1.25 */
                /* for ports in HGS mode set serdes speed to 3.125 */
                numOfSredesOnStackPort = (GT_TRUE == xcat2Dev) ? 1 : 4;
                serdesSpeed = ((xgMode >> ((i - PRV_CPSS_XCAT_NETWORK_PORTS_SERDES_NUM_CNS)/numOfSredesOnStackPort)) & 0x1)
                                ? PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_125_E : PRV_CPSS_DXCH_PORT_SERDES_SPEED_1_25_E;

                /* Pre Configuration enables access to serdes registers */
                rc = prvCpssDxChLpPreInitSerdesSequence(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                        i,
                                                        lpSerdesExtConfigArr[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG1_E]
                                                        [(PRV_CPSS_DXCH_PORT_SERDES_SPEED_1_25_E == serdesSpeed) ? 0 : 1]);
                if (rc != GT_OK)
                    return rc;

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].pllIntpReg2;
                rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr, lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_E][serdesSpeed]);
                if (rc != GT_OK)
                    return rc;

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].pllIntpReg3;
                rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr, lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_0_E][serdesSpeed]);
                if (rc != GT_OK)
                    return rc;

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].transmitterReg1;
                rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr,
                        lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG1_E][serdesSpeed]);
                if (rc != GT_OK)
                    return rc;

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].ffeReg;
                rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr,
                        lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_FFE_REG0_E][serdesSpeed]);
                if (rc != GT_OK)
                    return rc;

                if(GT_TRUE == xcat2Dev)
                {
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].transmitterReg2;
                    rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr,
                            lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG2_E][serdesSpeed]);
                    if (rc != GT_OK)
                        return rc;
                }
            }
        }

        if(xcatGigDevice == GT_TRUE)
        {
            for( serdesNum = 0 ; serdesNum < PRV_CPSS_XCAT_NETWORK_PORTS_SERDES_NUM_CNS ; serdesNum ++ )
            {
                PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesBmp);
                rc = cpssDxChPortSerdesPowerStatusSet(devNum,(GT_U8)(serdesNum*4),CPSS_PORT_DIRECTION_BOTH_E,0Xff,GT_TRUE);
                if (rc!= GT_OK)
                {
                    return rc;
                }

                if((GT_TRUE == xcat2Dev) && (xcatGigDevice == GT_TRUE))
                {
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].transmitterReg1;
                    rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr, /* 0x750C */
                            lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG1_E][PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_E]);
                    if (rc != GT_OK)
                        return rc;

                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].transmitterReg2;
                    rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr, /* 0x0 */
                            lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG2_E][PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_E]);
                    if (rc != GT_OK)
                        return rc;

                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].ffeReg;
/* TBD: here must be lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_FFE_REG0_E][PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_E]
 * but final value not calibrated yet, so let it for now be hardcoded */
                    rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr, 0x363);
                    if (rc != GT_OK)
                        return rc;
                }
            }

        } /* if (xcatGigDevice == GT_TRUE) */
    }
    if (xcatGigDevice == GT_TRUE)
    {
        regValue = (xcat2Dev) ? 0xC009 : 0xC008;
    }
    else
    {
        /* disable GPCS */
        regValue = 0xC000;
    }

    /* NP Port De-Assert MAC reset */
    for (portNum = 0 ; portNum < 24 ; portNum++)
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

        /* De-Assert MAC_Reset */
        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    if(GT_TRUE == xcat2Dev)
    {
        for (portNum = 24 ; portNum <= 27 ; portNum++)
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);
            if((rc = cpssDxChPortInbandAutoNegEnableSet(devNum,portNum,GT_TRUE)) != GT_OK)
            {
                return rc;
            }
        }
    }

    /* 
        *  These configration are moved to npd. Now, this function 
        *  is the same as original mcpss3.4
        *                                luoxun@autelan.com  03/25/2011
        */
    #if 0
    #ifndef __AX_PLATFORM_XCAT__
    /* 
        *  XCAT Port12-23 is Fiber port, 'InBandAnEn' must be set to enable.   
        *                                                              luoxun@autelan.com 
        */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        for (portNum = 12 ; portNum <= 23 ; portNum++)
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);
            if((rc = cpssDxChPortInbandAutoNegEnableSet(devNum,portNum,GT_TRUE)) != GT_OK)
            {
                osPrintfErr("XCAT Port12-23 InBandAnEn set enable ERROR.\n");
                return rc;
            }
        }
        osPrintfDbg("XCAT Port12-23 InBandAnEn set enable OK.\n");
    }
    #endif
    #endif

    return GT_OK;
}

/*******************************************************************************
* hwPpXcatPortsAndSerdesRegsDefault
*
* DESCRIPTION:
*       Set mandatory default values for Xcat ports and SERDES registers
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum  - device number
*       ppPhase1ParamsPtr  - Packet processor hardware specific parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpXcatPortsAndSerdesRegsDefault
(
    IN GT_U8                                   devNum,
    IN CPSS_DXCH_PP_PHASE1_INIT_INFO_STC       *ppPhase1ParamsPtr

)
{
    GT_U32 regAddr;     /* register's address   */
    GT_U32 regValue;    /* register value       */
    GT_U32 portNum;     /* port number          */
    GT_U32 devType;     /* device type          */
    GT_BOOL xcatGigDevice;
    GT_U32 serdesNum;   /* SERDES number        */
    GT_U32 networkSerdesBmp;   /* device SERDES bitmap */
    GT_U32 speed1RegValue;
    GT_U32 txConfRegValue;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        return hwPpXcatA1PortsAndSerdesRegsDefault(devNum,ppPhase1ParamsPtr);
    }

    /* define type of xCat device either Gig or Fast */
    devType = PRV_CPSS_PP_MAC(devNum)->devType;
    switch(devType)
    {
        case PRV_CPSS_DXCH_XCAT_FE_DEVICES_CASES_MAC:
            xcatGigDevice = GT_FALSE;
            break;
        default:
            xcatGigDevice = GT_TRUE;
            break;
    }

    /* 156.25Mhz frequancy is not qualified,
        Marvell strongly recommend to use the 125Mhz frequency for xCat */
    if(ppPhase1ParamsPtr->serdesRefClock == CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_125_SINGLE_ENDED_E)
    {
        speed1RegValue = 0x2449;
        txConfRegValue = 0x3EAA;
    }
    else
    {
        return GT_NOT_SUPPORTED;
    }

    /* Set device SERDES bitmap */
    prvCpssDxXcatNetworkSerdesBitmapGet(devNum,
                                       &ppPhase1ParamsPtr->powerDownPortsBmp,
                                       &networkSerdesBmp);

    if(xcatGigDevice == GT_TRUE)
    {
        /* NP serdes (HSGMII 5G) ref-clk configurations (for 125MHz refclk)*/
        for( serdesNum = 0 ; serdesNum < PRV_CPSS_XCAT_NETWORK_PORTS_SERDES_NUM_CNS ; serdesNum ++ )
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesBmp);

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesSpeed1[0];

            /* Change FBDIV to 0x12 and RefDiv to 0x1 */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, speed1RegValue) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesSpeed3[0];

            /* FcEn bit to SAS/SATA mode and bit 1 */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x03) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        /* NP serdes electrical configurations */
        for( serdesNum = 0 ; serdesNum < PRV_CPSS_XCAT_NETWORK_PORTS_SERDES_NUM_CNS ; serdesNum ++ )
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesBmp);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesTxConfReg1[0];

            /* Tx Conf Register 1, Amp=0x15, De-emphasis = 0x7 */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, txConfRegValue) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesRxConfReg1[0];

            /* Rx Conf Register 1, Ffelsel=0, FfeResSel=7, FfeCapSel=0 */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0xC38) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesDtlConfReg2[0];

            /* Selmupi=1 */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x2800) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesDtlConfReg3[0];

            /* Selmupf = 0x1, selmufi = 0x2, Selmuff = 0x2 */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x2A9) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesReservConf[0];

            /* DFE_UPDATE_EN=0 */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0xf070) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesIvrefConf1[0];

            /* RxVdd10=11 */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x557B) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        /* NP serdes Power Up*/
        for( serdesNum = 0 ; serdesNum < PRV_CPSS_XCAT_NETWORK_PORTS_SERDES_NUM_CNS ; serdesNum ++ )
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesBmp);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesPowerUp1[0];

            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x44) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesPowerUp2[0];

            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x44) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        /* wait at least 20 usec */
        cpssOsTimerWkAfter(2);

        /* NP serdes reset & calibration */
        for( serdesNum = 0 ; serdesNum < PRV_CPSS_XCAT_NETWORK_PORTS_SERDES_NUM_CNS ; serdesNum ++ )
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesBmp);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesReset[0];

            /* Analog Reset --> Normal operation */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x1110) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesMiscConf[0];

            /* Reset TxSyncFIFO; Sample Tx Data at Posedge */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x04) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesReset[0];

            /* Rx --> Normal operation */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x1110) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            /* Cal --> Normal operation */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x0010) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        /* wait at least 140 usec */
        cpssOsTimerWkAfter(14);

        /* NP serdes reset & calibration */
        for( serdesNum = 0 ; serdesNum < PRV_CPSS_XCAT_NETWORK_PORTS_SERDES_NUM_CNS ; serdesNum ++ )
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC(serdesNum, networkSerdesBmp);
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesReset[0];

            /* DTL --> Normal operation */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x0) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[serdesNum*4].serdesMiscConf[0];

            /* DeAssert Reset TxSyncFIFO; Sample Tx Data at Posedge */
            if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x0C) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
    } /* if (xcatGigDevice == GT_TRUE) */

    if (xcatGigDevice == GT_TRUE)
    {
        regValue = 0xC008;
    }
    else
    {
        regValue = 0xC000;
    }
    /* NP Port De-Assert MAC reset */
    for (portNum = 0 ; portNum < 24 ; portNum++)
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

        /* De-Assert MAC_Reset */
        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_XCAT_SGMII_MODE_ON_STACKING_PORTS_WA_E))
    {

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.ftdllReg;
        if (prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x5c601180) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* sets back serdes sync fifo reset */
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 29, 1, 1) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpDxChXcatRegsDefault
*
* DESCRIPTION:
*       Set mandatory default values for xCat registers
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       ppPhase1ParamsPtr  - (pointer to) Packet processor hardware specific
*                            parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpDxChXcatRegsDefault
(
    IN GT_U8                                    devNum,
    IN  CPSS_DXCH_PP_PHASE1_INIT_INFO_STC    *ppPhase1ParamsPtr
)
{
    GT_U32      i;
    GT_STATUS   rc;
    GT_U32      port;   /* iterator */
    GT_U32      regAddr;/* register address */

    /* start with the same ch3 settings */
    rc =  hwPpCheetah3RegsDefault(devNum, ppPhase1ParamsPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
        || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        /* do xcat specific settings */
        rc = hwPpXcatPortsAndSerdesRegsDefault(devNum, ppPhase1ParamsPtr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_RM_TXQ_EXT_CNTR_REG_WA_E))
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            rc = prvCpssDxChHwPpSetRegField(
                devNum,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.shaper.tokenBucketUpdateRate,
                30, 1, 1);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            /* Wrong default value of bit#28 of Transmit Queue Extended Control
               register. The bit should be set by default. */
            rc = prvCpssDrvHwPpSetRegField(
                devNum,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt,
                28, 1, 1);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /*********************/
    /* xcat and above */
    /*********************/

    for (i = 0; i <= CPSS_DXCH_POLICER_STAGE_EGRESS_E; i++)
    {
        /*(part of RM#3012)*/

        if( i != CPSS_DXCH_POLICER_STAGE_EGRESS_E )/* not for egress policer */
        {
            /*
                1. Set Qos model to be Qos Profile in Policer Control0 reg.
                2. Enable QosProfile update for IP packets in Policer Control1 reg.
                   (for Policer Ingress #0 and Policer Ingress #1
            */
            if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                    PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL0_WA_E))
            {
                rc = prvCpssDrvHwPpSetRegField(
                    devNum,
                    PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.PLR[i].policerControlReg,
                    21, 1, 1);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }

            if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                    PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL1_WA_E))
            {
                rc = prvCpssDrvHwPpSetRegField(
                    devNum,
                    PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.PLR[i].policerControl1Reg,
                    0, 1, 1);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }

        /* skip not supported IPLR1 */
        if ((CPSS_DXCH_POLICER_STAGE_INGRESS_1_E == i) &&
            (PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.iplrSecondStageSupported == GT_FALSE))
        {
            continue;
        }

        /* skip not supported EPLR */
        if ((CPSS_DXCH_POLICER_STAGE_EGRESS_E == i) &&
            (PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.egressPolicersNum == 0))
        {
            continue;
        }

        /* for all policers configure Flow mode */
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                PRV_CPSS_DXCH_XCAT_RM_POLICER_CONTROL2_WA_E))
        {
            rc = prvCpssDrvHwPpSetRegField(
                devNum,
                PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.PLR[i].policerControl2Reg,
                0, 1, 1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* RM#3026 - for all policers*/
        /* The default value of the <Counting Entry Format Select> field, bit[4] */
        /* in Policer Control1 register (EPLR: 0x03800004, IPLR0: 0x0C800004,    */
        /* IPLR1: 0x0D000004) is incorrect.                                      */
        /* Change the value to "Compressed format" (0x1).                        */
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                PRV_CPSS_DXCH_XCAT2_RM_POLICER_COUNT_FORMAT_WA_E))
        {
            rc = prvCpssDrvHwPpSetRegField(
                devNum,
                PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.PLR[i].policerControl1Reg,
                4, 1, 1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }


    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_XCAT_RM_MIRROR_INTERFACE_PARAMETER_REGISTER_I_WA_E))
    {
        /*(part of RM#3012)*/

        /* Enable Mirror on Drop */
        for(i = 0; i < 7; i++)
        {
            rc = prvCpssDrvHwPpSetRegField(
                devNum,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.eqBlkCfgRegs.mirrorInterfaceParameterReg[i],
                20, 1, 1);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_RM_TOTAL_BUFFER_LIMIT_CONF_IN_TXQ_E))
        {
            /* TxQ Total Buffer Limit */
            rc = prvCpssDrvHwPpWriteRegister(devNum,
                                            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                            egrTxQConf.totalBufferLimitConfig,
                                            0x00001800);
            if (rc != GT_OK)
            {
                return rc;
            }
        }


    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_RM_CNC_ENABLE_COUNTING_E))
    {
        /*(part of RM#3012)*/
        /* PCL Counting Mode Configuration register,
           field Enable Counting */
        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode,
            4, 1, 1);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_RM_TTI_UNIT_GLOBAL_CONFIGURATION_WA_E))
    {
        /*(part of RM#3012)*/

        /* TTI Unit Global Configuration register,
           field disable CCFC */
        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig,
            19, 1, 0);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* TTI Unit Global Configuration register,
           field Enable Counting */
        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig,
            3, 1, 1);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_RM_POLICY_ENGINE_CONFIGURATION_REGISTER_WA_E))
    {
        /*(part of RM#3012)*/

        /* Enable full lookup for Ingress PCL lookup0 sublookup1 */
        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclEngineConfig,
            12, 1, 1);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_RM_EGRESS_POLICER_GLOBAL_CONFIGURATION_WA_E))
    {
        /*(part of RM#3012)*/

        /* Enable Egress Policer and set ERLR counting mode to be
           Billing/IP FIX */
        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.eplrGlobalConfig,
            0, 3, 3);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_RM_TXQ_DEQUEUE_SWRR_WEIGHT_REGISTER_3_WA_E))
    {
        /* fields of XG and Gig1 weights are swapped */
        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            0x018000BC,
            0, 9, 0x4b);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_RM_BCN_CONTROL_WA_E))
    {
        /*(part of RM#3012)*/

        /* BCN Aware Packet Type */
        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnControlReg,
            1, 1, 0);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* En MC BCN Aware */
        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnControlReg,
            3, 1, 1);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_RM_PORTS_BCN_AWARENESS_TABLE_WA_E))
    {
        /*(part of RM#3012)*/

        /* BCN Awareness Entry */
        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.portsBcnAwarenessTbl,
            0, 28, 0x0FFFFFFF);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_RM_PORT_SERIAL_PARAMETERS_CONFIGURATION_WA_E))
    {
        for( port = 0; port <= PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
        {
            if(port == PRV_CPSS_PP_MAC(devNum)->numOfPorts)
            {
                port = CPSS_CPU_PORT_NUM_CNS; /* CPU port */
            }
            else
            {
                PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, port);
            }

            PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,port,&regAddr);

            rc = prvCpssDrvHwPpSetRegField(
                devNum,regAddr, 1, 1, 0);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_RM_LMS0_LMS1_MISC_CONFIGURATIONS_WA_E))
    {
        GT_U32  lmsAddr[2];

        lmsAddr[0] = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;
        lmsAddr[1] = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms1MiscConfig;

        for(i = 0 ; i < 2 ; i++)
        {
            rc = prvCpssDrvHwPpWriteRegBitMask(
                devNum,
                lmsAddr[i],
                0x118000,
                0x010000);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_RM_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E))
    {
        rc = prvCpssDrvHwPpSetRegField(devNum, 0x0C0002B0, 17, 1, 1);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_GIGA_PORT_PRBS_PARAM_INIT_WA_E))
    {
        /* FEr#3054: Gig port PRBS is not functional. */
        /* XCAT A2 and above, initial PRBS parameters */
        for (port = 24 ; port < 28 ; port++)
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, port);

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                        perPortRegs[port].serialParameters;

            /* set PBRS parameters */
            if (prvCpssDrvHwPpSetRegField(
                devNum, regAddr, 14 /*offset*/, 2/*length*/, 3/*value*/)
                != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_RM_HEADER_ALTERATION_GLOBAL_CONFIGURATION_REGISTER_WA_E))
    {
        /* Set the < Enable Passenger Vlan Translation For TS > field, bit[10] in the Header Alteration Global Configuration
            register (offset: 0x0E800100) to 0x0 (disable). */
        rc = cpssDxChTunnelStartPassengerVlanTranslationEnableSet(devNum,GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if((GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_ETH_OVER_IPV4_GRE_WA_E)) &&
       (GT_TRUE == PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(devNum)))
    {
        /* Enable HA fix - tunneling support for Eth-over-IPv4 */
        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig,
            15, 1, 1);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT2_RM_INT_EXT_REF_CLK_WA_E))
    {
        switch(PRV_CPSS_PP_MAC(devNum)->devType)
        {
            case PRV_CPSS_DXCH_XCAT2_GE_DEVICES_CASES_MAC:
            /* The default value of bit[30] (<int_ext_ref_clk_125> field) in the    */
            /* Device_Cfg register (0x28) is incorrect.                             */
            /* Change the value to 0x0 (input_pin).                                 */
            /* RM#3027 */
                rc = prvCpssDrvHwPpSetRegField(devNum, 0x28, 30, 1, 0);
                if(rc != GT_OK)
                {
                    return rc;
                }
                break;
            default: break;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT2_RM_INCORRECT_XG_SHAPER_TOKEN_BUCKET_WA_E))
    {
        /* Ports 24-27 shaping rate defaults are different than that of the ports */
        /* 0-23.                                                                  */
        /* To enable the ports 24-27 to support shaping rates identical to ports  */
        /* 0-23, set the <XgPortsTbAsGig> field, bit[31] in the Token Bucket      */
        /* Update Rate and MC FIFO Configuration Register (0x0180000C) to         */
        /* "Gig mode" (0x1).                                                      */
        /* (RM#3030)                                                              */
        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQMcFifoEccConfig,
            31, 1, 1);
        if(GT_OK != rc)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT2_RM_BYPASS_PACKET_REFRESH_FDB_AGE_WA_E))
    {
        /* Packets that bypass the Ingress Pipe perform a refresh of the age bit   */
        /* in the FDB.                                                             */
        /* Set the value of the <Avoid_Age_Refresh> field, bit[22] in the Bridge   */
        /* Global Configuration2, to 0x1 (enable), so that packets that bypass the */
        /* Ingress Pipe will not perform refresh of the age bit in the FDB.        */
        /* (RM#3031)                                                               */
        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            bridgeRegs.bridgeGlobalConfigRegArray[2],
            22, 1, 1);
        if(GT_OK != rc)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT2_RM_PIPE_SELECT_WA_E))
    {
        /* Wrong default in Pipe Select register and some new DFX settings. */
        /* (RM#3032)                                                        */
        rc = prvCpssDrvHwPpWriteRegister(
             devNum,
             PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.pipeSelect,
             0x00000001);
        if(GT_OK != rc)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpWriteRegister(
             devNum,
             PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                dfxUnits.ingressLower.tcBist.tcamBistConfigStatus,
             0x00020003);
        if(GT_OK != rc)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpWriteRegister(
             devNum,
             PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                dfxUnits.ingressUpper.tcBist.tcamBistConfigStatus,
             0x00020003);
        if(GT_OK != rc)
        {
            return rc;
        }
        rc = prvCpssDrvHwPpWriteRegister(
             devNum,
             PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.gppPadControl,
             0x00000048);
        if(GT_OK != rc)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpWriteRegister(
             devNum,
             PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.ssmiiPadControl,
             0x00000048);
        if(GT_OK != rc)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpWriteRegister(
             devNum,
             PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.rgmiiPadControl,
             0x00000048);
        if(GT_OK != rc)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpWriteRegister(
             devNum,
             PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.ledPadControl,
             0x00000048);
        if(GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpDxChLionRegsDefault
*
* DESCRIPTION:
*       Set mandatory default values for Lion registers
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - device number
*       ppPhase1ParamsPtr  - (pointer to) Packet processor hardware specific
*                            parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpDxChLionRegsDefault
(
    IN GT_U8                                    devNum,
    IN  CPSS_DXCH_PP_PHASE1_INIT_INFO_STC    *ppPhase1ParamsPtr
)
{
    GT_STATUS   rc;             /* return code      */
    GT_U32      regAddr;        /* register address */
    GT_U32      portGroupId;    /* port group id    */
    GT_U32      serdes;         /* SERDES number    */
    GT_U32      data;
    GT_U8       port;/* port number */
    GT_U32      pclUnitBaseAddr;/* pcl unit base address */
    GT_U32      routerUnitBaseAddr;/* router unit base address */
    CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC shaperConfig; /* shaper configuration */

    /* start with the same xCat settings */
    rc =  hwPpDxChXcatRegsDefault(devNum, ppPhase1ParamsPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* add here DxCh Lion specifics */
    /*RM#2700: Buffer management init values*/
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_RM_BUFFER_MANAGEMENT_INIT_VALUES_WA_E))
    {
        /*
            Change bit0 of 0x03000060 to 0x1 to enable the next write.
            Change offset 0x03000200 to 0x7FF
            Change bit 0 of 0x03000060 to 0x0
            Change bits[30:22] of 0x03000000 to 0x3F
            Change bits[21:11] of 0x03000000 to 0x7FF
        */
        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.bufferMng.bufMngMetalFix,
            0, 1, 1);
        if (rc != GT_OK)
        {
            return rc;
        }

        regAddr = 0x03000200;
        rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr,0x7FF);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.bufferMng.bufMngMetalFix,
            0, 1, 0);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.bufferMng.bufMngGlobalLimit,
            22, 9, 0x3f);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.bufferMng.bufMngGlobalLimit,
            11, 11, 0x7ff);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /*FEr#2742: TCAM synchronization restriction */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_TCAM_SYNCHRONIZATION_RESTRICTION_WA_E))
    {
        /*
           The device TCAM can operate at a frequency similar to or higher
           than the core frequency.
           In addition, the TCAM holds a FIFO of pending requests for
           synchronizing the core domain with the TCAM domain.
           However, due to this erratum, the TCAM clock frequency must be
           higher than the core clock frequency.
           Functional Impact : TCAM search results will be erroneous.
           WA : With the 25 MHz reference clock, the following sequence
           needs to be implemented per control pipe:
           1. Power down PLL: change address 0x0000036C bits[3:0] to 0xD.
           2. Wait for at least 20 uSec.
           3. Change PLL frequency to 364 MHz: change address 0x00000368 to 0x30B0013.
           4. Power up PLL, but block the clock: change address 0x0000036C bits[3:0] to 0xC.
           5. Wait at least 50 uSec for clock stabilization.
           6. Enable clock to the TCAM: change address 0x0000036C bits[3:0] to 0xE.
        */

        /*1. Power down PLL: change address 0x0000036C bits[3:0] to 0xD.*/
        regAddr = 0x0000036C;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,0, 4, 0xD);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* 2. wait at least 20 usec */
        cpssOsTimerWkAfter(1);/*1 msec --> 1000 usec */


        /*3. Change PLL frequency to 364 MHz: change address 0x00000368 to 0x30B0013.*/
        regAddr = 0x00000368;
        rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr,0x30B0013);
        if (rc != GT_OK)
        {
            return rc;
        }


        /*4. Power up PLL, but block the clock: change address 0x0000036C bits[3:0] to 0xC.*/
        regAddr = 0x0000036C;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,0, 4, 0xC);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* wait at least 50 usec --> for clock stabilization */
        cpssOsTimerWkAfter(1);/*1 msec --> 1000 usec */

        /*6. Enable clock to the TCAM: change address 0x0000036C bits[3:0] to 0xE.*/
        regAddr = 0x0000036C;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,0, 4, 0xE);
        if (rc != GT_OK)
        {
            return rc;
        }

    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                               PRV_CPSS_DXCH_LION_RM_SYNC_ETHERNET_NONE_SELECT_WA_E))
    {
        for ( serdes = 0; serdes < 24 ; serdes++ )
        {
            rc = prvCpssDrvHwPpSetRegField(
                 devNum,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdes].serdesExternalReg2,
                 11,
                 1,
                 1);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                               PRV_CPSS_DXCH_LION_RM_TCAM_CONFIG_WA_E))
    {
        pclUnitBaseAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclTcamConfig0 & 0xFFF00000;
        routerUnitBaseAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ipTcamConfig0 & 0xFFF00000;


        /*Change bits[9:8] (Read Delay field) in Policy TCAM Configuration 0 Register*/
        regAddr = pclUnitBaseAddr + 0x208;/*0x0D000208*/
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,8,2,0x3);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*Change bits[9:8] (Read Delay field) in Router TCAM Configuration 0 Register*/
        regAddr = routerUnitBaseAddr + 0x96C;/*0x0D80096C*/
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,8,2,0x3);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*Change bits[15:8] (RdToRdDelay field) in Policy TCAM Configuration 2 Register Offset*/
        regAddr = pclUnitBaseAddr + 0x220;/*0x0D000220*/
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,8,8,0x17);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* fix the compare read delay after read for the TCAM:
            Register CPU 0x0D800978 Bits[15:8] should be set to 0x24.
            This is important for proper TCAM bist & Router activation */
        regAddr = routerUnitBaseAddr + 0x978;/*0x0D800978*/
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,8,8,0x24);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*Change bits[27:24] (ECCCycleReset field) in Policy TCAM Control Register*/
        regAddr = pclUnitBaseAddr + 0x148;/*0x0D000148*/
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,24,4,0xf);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*Change bits[27:24] (ECCCycleReset field) in Router TCAM Control Register*/
        regAddr = routerUnitBaseAddr + 0x978;/*0x0D800978*/
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,24,4,0xf);
        if(rc != GT_OK)
        {
            return rc;
        }

    }

    /* WA for "Token Buckets are not supported" */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E))
    {
        /* check that device Lion B0A revision */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.chipId;/*0xD4*/
        portGroupId = 0;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                                                 regAddr, &data);
        if(rc != GT_OK)
        {
            return rc;
        }

        if ( (data != 0) || (PRV_CPSS_PP_MAC(devNum)->revision > 2) )
        {
            /* this is Lion B0A revision or B1 and above.
               Set the <TxQStartInit> field to '0x1' */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl4;/*0x00000364*/
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                    3, 1, 1);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* disable the old WA in order to enable shaper API */
            PRV_CPSS_DXCH_ERRATA_CLEAR_MAC(devNum,
                                           PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E);
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_RM_MULTI_PORT_GROUP_FDB_LIMIT_WA_E))
    {
        /* Disable Auto-learning, per port */
        for( port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum,port);

            rc = cpssDxChBrgFdbPortLearnStatusSet(devNum, port, GT_FALSE, CPSS_LOCK_FRWRD_E);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /*Set the Age mode to Age-without-delete*/
        rc = cpssDxChBrgFdbActionModeSet(devNum,CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_RM_ORIG_SRC_PORT_FILTERING_WA_E))
    {
        /* set the device to filter the srcPort of own device on DSA tag frames */
        rc = cpssDxChCscdOrigSrcPortFilterEnableSet(devNum,GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_CN_GLOBAL_REG_WA_E))
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            /*Set the CnDropEn field, bit[5] in the CN Global Configuration register,
            (offset: 0x0A0B0000) to 0x1 (enable).*/
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
                congestNotification.cnGlobalConfigReg;
            rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 5, 1, 1);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_TEMPERATURE_SENSOR_INITIALIZATION_WA_E))
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            /* need to Calibrate sensor on init in every power up
             (all below registers are in PortGroup0) */
            portGroupId = 0;

            /* 1) Change Temperature sensor average num from 2 to 8 and enable On-The-Fly
                  calibration -
                  Register 0x118F0D88 Bits[21:19] = 0x3 & & Bit[23] = 0x1 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.sht.dfx.dfx2;/*0x118F0D88*/
            rc = prvCpssDxChHwPpPortGroupWriteRegBitMask(devNum, portGroupId ,regAddr,
                                                         0xB80000, 0x980000);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* 2) Change Temperature sensor RefCalCount from 0x113 to 0xf1 -
                  Register 0x118F0D8C Bits[17:9] = 0xF1 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.sht.dfx.dfx3;/*0x118F0D8C*/
            rc = prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId ,regAddr, 9, 9, 0xf1);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* 3) Start init -
                  Register 0x118F0D8C Bit[31] = 0x0*/
            rc = prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId ,regAddr, 31, 1, 0);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* 4) Start init -
                  Register 0x118F0D8C Bit[31] = 0x1 */
            rc = prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId ,regAddr, 31, 1, 1);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_RM_MCFIFO_DISTRIBUTION_REG_WA_E))
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.
                        mcFifoConfig.distributionMask[0];

            /* need to configure all ports in the MCFIFO distribution mask register
               (0x01801010+ 0x4*n) as follow:
               1) configure ports 0-63
                   a) EVEN Ports - FIFO 0
                   b) ODD Ports - FIFO 1
            */

            data = 0xAAAAAAAA;/* all ODD bits are set , all even bits are cleared */

            /* set register for ports 0..31 */
            rc = prvCpssDxChHwPpWriteRegister(devNum, regAddr, data);
            if(rc != GT_OK)
            {
                return rc;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.
                        mcFifoConfig.distributionMask[1];
            /* set register for ports 32..63 */
            rc = prvCpssDxChHwPpWriteRegister(devNum, regAddr, data);
            if(rc != GT_OK)
            {
                return rc;
            }

        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_RM_SD_PLL_REG_WA_E))
    {
        /*
            for XLG port proper work :
            Set the following registers in portGroup 0:
            1) Set register 0x378 = 0xD
            2) Delay 20uSec
            3) Set register 0x374 = 0x36203001
            4) Set register 0x378 = 0xC
            5) Delay 50uSec
            6) Set register 0x378 = 0x9E
        */
        portGroupId = 0;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.sdPllCtrl; /* 0x378 */

        /*1) Set register 0x378 = 0xD*/
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr,0xD);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* 2) Delay 20uSec */
        cpssOsTimerWkAfter(1);/*1 msec --> 1000 usec */

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.sdPllParams; /* 0x374 */

        /* 3) Set register 0x374 = 0x36203001 */
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr,0x36203001);
        if(rc != GT_OK)
        {
            return rc;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.sdPllCtrl; /* 0x378 */

        /* 4) Set register 0x378 = 0xC */
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr,0xC);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* 5) Delay 50uSec */
        cpssOsTimerWkAfter(1);/*1 msec --> 1000 usec */

        /* 6) Set register 0x378 = 0x9E */
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr,0x9E);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_RM_UC_STRICT_OVER_MC_WA_E))
    {
        /*  With default configurations, the UC are not served at WS in presence of */
        /*  MC, therfore give UC Strict Priority over MC.                           */
        /*  Set bit 2 in the MC FIFO Global Configurations Register (0x01801000).   */
        /* (RM#2710)                                                                */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.egr.mcFifoConfig.globalConfig;
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 2, 1, 1);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_RM_SCHEDULER_DEFICIT_MODE_WA_E))
    {
        /* The TxDMA isn’t always ready to receive descriptor from the TxQ when      */
        /* instantaneous burstiness of data occurs.                                  */
        /* (RM#2709)                                                                 */
        /* The workaround consists of 2 steps:                                       */
        /* 1. Enabling the Deficit mode (handles higher burstiness of data on the    */
        /*    ports).                                                                */
        rc = cpssDxChPortTxSchedulerDeficitModeEnableSet(devNum, GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* 2. Set <Token Bucket Base Line> value to at least eight times the MTU.     */
        /* Get MTU value */
        rc = cpssDxChPortTxShaperConfigurationGet(devNum,&shaperConfig);
        if(rc != GT_OK)
        {
            return rc;
        }

        if( shaperConfig.portsPacketLength >= _2M )
        {
            /* MTU is too large and multiply by 8 will cause overflow. */
            /* Set max value for Base Line.                            */
            data = BIT_24 - 1;
        }
        else
        {
            /* Get Base Line value */
            rc = cpssDxChPortTxShaperBaselineGet(devNum, &data);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* If <Base Line> >= 8*MTU, do nothing, */
            /* Else <Base Line> = 8*MTU.            */
            if( data >= (8 * shaperConfig.portsPacketLength) )
            {
                data = 0;
            }
            else
            {
                data = (8 * shaperConfig.portsPacketLength);
            }
        }

        if( data > 0 )
        {
            /* Set Base Line value */
            rc = cpssDxChPortTxShaperBaselineSet(devNum, data);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_RM_TX_MC_IN_CHUNKS_WA_E))
    {
        /* WS is not achieved in “one to many?traffic at 316 Bytes and below.        */
        /* Set the number of MC descriptors duplications done per UC descriptor       */
        /* to 4:1.                                                                    */
        /* Clear bits 1:0 in the MC FIFO Global Configurations Register (0x01801000). */
        /* (RM#2711)                                                                  */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.egr.mcFifoConfig.globalConfig;
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 2, 0);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_RM_BUFF_LIMIT_EGRESS_QUEUE_WA_E))
    {
        /* Wrong total number of buffers limit in egress queues.                     */
        /* Set bits 13:0 in the Global Buffer Limit Register (0x0A0A0310) to 0x1e70. */
        /* (RM#2712)                                                                 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.tailDrop.limits.globalBufferLimit;
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 14, 0x1e70);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_RM_QCN_FOR_CN_FORMAT_WA_E))
    {
        /* Since QECN mode is obsolete, CN frame format must be set to QCN. */
        /* Set bit 29 in the Extended Global Control 2 Register (0x8C).     */
        /* (RM#2713)                                                        */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                globalRegs.extendedGlobalControl2;
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 29, 1, 1);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_PFC_FOR_MANY_PRIORITY_WA_E))
    {
          /* Only one priority queue can be specified for pausing at the same */
          /* time in PFC frame.                                               */
          /* (FEr#2787)                                                       */


          for( port = 0;
               (port < PRV_CPSS_PP_MAC(devNum)->numOfPorts) && (port <= 0xb);
               port++)
          {
              PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum,port);

              /* XG treatment */
              regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                  macRegs.perPortRegs[port].macRegsPerType[PRV_CPSS_PORT_XG_E].miscConfig;
              rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 1, 1);
              if(rc != GT_OK)
              {
                  return rc;
              }

              /* XLG treatment */
              if ( 10 == port )
              {
                  regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                      macRegs.perPortRegs[port].macRegsPerType[PRV_CPSS_PORT_XLG_E].miscConfig;
                  rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 1, 1);
                  if(rc != GT_OK)
                  {
                      return rc;
                  }
              }
          }
     }

     if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_QCN_FORMAT_COMPLIANT_WA_E))
     {
         /* QCN frame format is partially compliant with IEEE 802.1 Qau.   */
         /* Set bit 0 in the HA Misc Configuration Register (0xE800710). */
         /* (FEr#2703)                                                     */
         regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.miscConfig;
         rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 1, 1);
         if(rc != GT_OK)
         {
             return rc;
         }
     }

     if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_INCOMPLETE_CPU_MAILBOX_INFO_WA_E))
     {
         /* CPU Mailbox to Neighbor CPU Device source information is not */
         /* complete.                                                    */
         /* (FEr#2776)                                                   */
         regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     txqVer1.dq.global.debug.metalFix;
         rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 21, 1, 1);
         if(rc != GT_OK)
         {
             return rc;
         }
     }

     if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_INACCURATE_PFC_XOFF_WA_E))
     {
         /* The response to the XOFF PFC frame is inaccurate.              */
         /* Set bit 19 in the TTI Misc Confuguration Register (0x1000200). */
         /* (RM#2788)                                                      */
         regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttiMiscConfig;
         rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 19, 1, 1);
         if(rc != GT_OK)
         {
             return rc;
         }
     }

     /* FEr#2774: WA for "False parity error indication at
        Ingress PCL Configuration table initialization" */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
       PRV_CPSS_DXCH_LION_IPCL_FALSE_PARITY_ERROR_WA_E))
    {
         /* Upon Ingress PCL Configuration tables initialization,
         clear by read the following registers:
            - PCL config table 0/1 parity error,
              bits[24:23] in the Policy Engine Interrupt Cause register (offset: 0x0B800004)
            - Config Table Parity Error Counter i Register (i=0?),
              offset: PclConfigTable0: 0x0B80006C, PclConfigTable1: 0x0B800070 */
         regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ipclEngineInterruptCause; /*0x0b800004*/

         /* the per port group information taken from ppPhase1ParamsPtr->multiPortGroupsInfoPtr[portGroupId] */
         PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
         {
             rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &data);
             if(rc != GT_OK)
             {
                return rc;
             }
             rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, 0x0B80006C, &data);
             if(rc != GT_OK)
             {
                return rc;
             }
             rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, 0x0B800070, &data);
             if(rc != GT_OK)
             {
                return rc;
             }
         }
         PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
     }

     return GT_OK;
}

/*******************************************************************************
* hwPpDxChXcat2RegsDefault
*
* DESCRIPTION:
*       Set mandatory default values for xCat2 registers
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
*
* INPUTS:
*       devNum  - device number
*       ppPhase1ParamsPtr  - (pointer to) Packet processor hardware specific
*                            parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpDxChXcat2RegsDefault
(
    IN GT_U8                                    devNum,
    IN  CPSS_DXCH_PP_PHASE1_INIT_INFO_STC    *ppPhase1ParamsPtr
)
{
    GT_STATUS   rc;   /* return code */
    GT_U8       port; /* port number */
    GT_U32      regAddr;

    rc = hwPpDxChLionRegsDefault(devNum, ppPhase1ParamsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT2_RM_STACK_PORT_MIB_CNTR_CONTROL_WA_E))
    {
        /* Enable MAC MIB Counters Clear on read for stack ports 24..27 */
        for( port = 24; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum,port);

            rc = cpssDxChPortMacCountersClearOnReadSet(devNum, port, GT_TRUE);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /* setting bit 9 to be 0 - disable SRR init */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.serverSkipInitMatrix;
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 9, 1, 0);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* setting bit 9 to be 0 - CPU skipInit */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.genSkipInitMatrix0;
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 9, 1, 0);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpPhase1NeededWaSettings
*
* DESCRIPTION:
*       Part 1 of "phase 1"
*       initialize all the WA that needed for the device
*
*       function called from the "phase 1" sequence.
*
* INPUTS:
*       devNum          - The Pp's device number to set the errata Wa needed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_OUT_OF_CPU_MEM - on failing memory allocation
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS hwPpPhase1NeededWaSettings
(
    IN GT_U8    devNum
)
{
    GT_U32  ii;/*iterator*/
    PRV_CPSS_DXCH_ERRATA_STC *errataPtr =
        &PRV_CPSS_DXCH_PP_MAC(devNum)->errata;/*pointer to errata info of the device */
    PRV_CPSS_DXCH_ERRATA_STACK_MAC_COUNT_SHADOW_STC *macCountShadowPtr; /*mac Count Shadow Ptr*/

    /* set that the device need the fix of the :
       Errata FEr# */

      /* Disabling the Egress Rate Shaper under traffic may hang its
     relevant transmit queue. (FEr#47) */
    PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                 PRV_CPSS_DXCH_TOKEN_BUCKET_RATE_SHAPING_DISABLE_WA_E);

    /* When a packet is received from a remote device, the CPU should calculate
       the correct byte count by decrementing the DSA tagsize from the PktOrigBC.
       (FEr#89) */
    PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                PRV_CPSS_DXCH_PACKET_ORIG_BYTE_COUNT_WA_E);

    /* needed WA for deadlock in the PP , between 'learning' and 'scanning' in
        FDB .
        see detailed info in description of:
        PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E */
    PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E);

    /* Wrong default in FDB Global Configuration register - RM# 2704*/
    PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH_RM_FDB_GLOBAL_CONF_REG_WA_E);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* fill additional errata/RM for the Lion */
        for(ii = 0 ; lionErrAndRmArray[ii] != LAST_DEV_IN_LIST_CNS ; ii++)
        {
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum, lionErrAndRmArray[ii]);
        }

        /* XLG port may not support wire speed in all cases. */
        /* (FEr#2709)                                        */
        PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                PRV_CPSS_DXCH_LION_XLG_WS_SUPPORT_WA_E);

        if( PRV_CPSS_PP_MAC(devNum)->revision > 2 )
        {
            /* Only one priority queue can be specified for pausing at the same */
            /* time in PFC frame.                                               */
            /* (FEr#2787)                                                       */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH_LION_PFC_FOR_MANY_PRIORITY_WA_E);

            /* QCN frame format is partially compliant with IEEE 802.1 Qau.     */
            /* Set bit 0 in the HA Misc Configuration Register (0xE800710).   */
            /* (FEr#2703)                                                       */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH_LION_QCN_FORMAT_COMPLIANT_WA_E);

            /* The response to the XOFF PFC frame is inaccurate.                */
            /* Set bit 19 in the TTI Misc Confuguration Register (0x1000200).   */
            /* (RM#2788)                                                        */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH_LION_INACCURATE_PFC_XOFF_WA_E);

            /* CPU Mailbox to Neighbor CPU device source information is not     */
            /* complete - wrong source port assigment.                          */
            /* (FEr#2776)                                                       */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH_LION_INCOMPLETE_CPU_MAILBOX_INFO_WA_E);
        }

        /* must fill info for PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E */
        /* PCL */
        errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.pclInterruptCauseRegAddr =
            0x0D0001A4; /* TCC (lower) for PCL register */
        errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.pclInterruptIndex =
            PRV_CPSS_LION_B_TCC_LOWER_GEN_TCAM_ERROR_DETECTED_E;/* value for lion (non lion A) */

        /* Router */
        errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.routerInterruptCauseRegAddr =
            0x0D800060; /* TCC (upper) for router register */
        errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.routerInterruptIndex =
            PRV_CPSS_LION_B_TCC_UPPER_GEN_TCAM_ERROR_DETECTED_E;/* value for lion (non lion A) */

    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* fill additional errata/RM for the xCat2 */
        for(ii = 0 ; xcat2ErrAndRmArray[ii] != LAST_DEV_IN_LIST_CNS ; ii++)
        {
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum, xcat2ErrAndRmArray[ii]);
        }

        /* PCL */
        errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.pclInterruptCauseRegAddr =
            0x0E0001A4; /* TCC (lower) for PCL register */
        errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.pclInterruptIndex =
            PRV_CPSS_XCAT2_TCC_LOWER_UNIT_TCAM_ERROR_DETECTED_E;

        /* Router */
        errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.routerInterruptCauseRegAddr =
            0x0E800060; /* TCC (upper) for router register */
        errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.routerInterruptIndex =
            PRV_CPSS_XCAT2_TCC_UPPER_UNIT_TCAM_ERROR_DETECTED_E;

        /* Stack Ports Mac counters shadow */

        macCountShadowPtr = (PRV_CPSS_DXCH_ERRATA_STACK_MAC_COUNT_SHADOW_STC*)
            cpssOsMalloc(sizeof(PRV_CPSS_DXCH_ERRATA_STACK_MAC_COUNT_SHADOW_STC));

        if (macCountShadowPtr == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }
        /* assume initional state of counters - zeros */
        cpssOsMemSet(
            macCountShadowPtr, 0,
            sizeof(PRV_CPSS_DXCH_ERRATA_STACK_MAC_COUNT_SHADOW_STC));

        /* assume clear-on-read option is off */
        for (ii = 0; (ii < PRV_CPSS_DXCH_ERRATA_STACK_PORT_AMOUNT_CNS); ii++)
        {
            macCountShadowPtr->stackPortArr[ii].clearOnRead = GT_FALSE;
        }

        errataPtr->
            info_PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E.
            stackMacCntPtr = macCountShadowPtr;
    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E ||
             (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E))
    {

        PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH3_RM_CPU_ACCESS_TO_FDB_UNDER_TRAFFIC_WA_E);

        /* layer 4 sanity checks are enabled by default - RM#3018*/
        PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH2_RM_BRG_LAYER_4_SANITY_CHECKS_WA_E);

        /* FEr#2018 - Policy base routing LTT index can be written only to array0 of the LTT line */
        PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH3_LIMITED_NUMBER_OF_POLICY_BASED_ROUTES_WA_E);

        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
        {
            if(PRV_CPSS_PP_MAC(devNum)->revision == 0)
            {
                /* RM#2011:  TCAM PLL Register has wrong default value
                   FEr#2012: TCAM/CORE/MPPM clock frequencies above 250 MHz
                             are not operational */
                PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                            PRV_CPSS_DXCH3_RM_TCAM_PLL_REG_WA_E);
            }

            /* Change the default value of Network Port SERDES Misc Conf Register:
               0x0000000D at addresses 0x09800014 + n*0x400: where n (0-23) represents Port
               (RM#2062) */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH3_RM_GE_SERDES_MISC_CONF_REG_WA_E);

            /* FEr#2014: Sampled at Reset Network reference clock select is not functional */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH3_SAMPLE_AT_RESET_NET_REF_CLOCK_SELECT_WA_E);

            /* FEr#2191 - TCAM eFUSE mechanism is not triggered automatically */
                PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH3_TCAM_EFUSE_NOT_TRIGGERED_AUTO_WA_E);

            /* TB#233 PCL and Router TCAM Software Repair Update */
                PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH3_TCAM_REPAIR_WA_E);

            /* Change the default value of Buffer Memory Tx Fifo Threshold
               and MPPM Access Arbiter Configuration Register:
               0x444e610c at address 0x0780006C
               (RM#2063) */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH3_RM_BM_TX_FIFO_THRESHOLD_CONF_REG_WA_E);

            /* Fixes for device revision B2 */
            if( 1 == PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.mg.metalFix )
            {
                /* SERDES yield improvement due to PLL unlocking issue. */
                PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                         PRV_CPSS_DXCH3_SERDES_YIELD_IMPROVEMENT_WA_E);

                /* Toggling Device Enable under traffic may cause system hang.
                   (FEr#2261) */
                PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                         PRV_CPSS_DXCH3_TOGGLE_DEV_EN_UNDER_TRAFFIC_WA_E);
            }
        }

        if (0 == PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* those NOT relevant to the xcat and above */

            /* FEr#2006: VLT Tables Indirect Access is not functional */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                PRV_CPSS_DXCH3_VLT_INDIRECT_ACCESS_WA_E);

            /* RM#2007: LMS GigE Ports Mode Register1 */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                PRV_CPSS_DXCH3_RM_GE_PORTS_MODE1_REG_WA_E);

            /* RM#2008: LMS HyperG.Stack Ports Mode Register */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                PRV_CPSS_DXCH3_RM_XG_PORTS_MODE_REG_WA_E);

            /* FEr#2009 - SDMA resource errors may cause PCI Express packets reception
             malfunction */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum, PRV_CPSS_DXCH3_SDMA_WA_E);

            /* RM#2026: MAC2ME Low Registers */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                PRV_CPSS_DXCH3_RM_MAC2ME_LOW_REG_WA_E);

            /* RM#2061: PHY Auto-Negotiation Configuration Register0 and Register2 */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                PRV_CPSS_DXCH3_RM_AN_CNF_REG_WA_E);

            /* FE#2046 policer - QoS attributes incorrect modification for out-of-profile traffic */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                PRV_CPSS_DXCH3_OUT_OF_PROFILE_QOS_ATTR_E);

            /* FEr#1098 - A write to the Transmit Queue Full Interrupt Mask register
              (offset: 0x018001EC) does not function */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                PRV_CPSS_DXCH3_TXQ_FULL_INTERRUPT_NOT_FUNCTION_WA_E);

            /* FEr#204 - TxQ Parity calculation is wrong for Descriptors & FLL memories */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                PRV_CPSS_DXCH3_TXQ_PARITY_CALCULATION_E);
        }

        /* RM#2003: Buffer Management GbE and HyperG.Stack Ports Group Limits Configuration Registers */
        PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH3_RM_BM_GE_XG_PORT_GROUP_LIMIT_CFG_REG_WA_E);

        /* FEr#2004 - Policy and Router TCAM Tables indirect read is not functional*/
        PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH3_POLICY_AND_ROUTER_TCAM_TABLES_INDIRECT_READ_WA_E);

        /* RM#2024: LMS0/1 Group1LED Interface0 Control Register 1 and Class6 Manipulation Register */
        PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH3_RM_LMS0_1_GROUP1_LED_INTERFACE_WA_E);

        /* FEr#2028 - Direct access to EQ (Pre-Egress) engine address space is
           not functional under traffic */
        PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH3_DIRECT_ACCESS_TO_EQ_ADDRESS_SPACE_WA_E);

        /* FEr#2033,FEr#2050: When the CPU reads the POLICY TCAM or ROUTER TCAM
           via direct address space the ECC error exception signaled */
        PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E);

        /* must fill info for PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E */
        if (0 == PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* ch3 */

            /* PCL */
            errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.pclInterruptCauseRegAddr =
                0x0B800010; /* PCL interrupt register */
            errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.pclInterruptIndex =
                PRV_CPSS_CH3_PCL_TCC_ECC_ERR_E;/* value for ch3 but also used for xcat A0 */

            /* Router */
            errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.routerInterruptCauseRegAddr =
                0x02040130; /* L2 Bridge interrupt register */
            errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.routerInterruptIndex =
                PRV_CPSS_CH3_EB_TCC_E;/* value for ch3 but also used for xcat A0 */
        }
        else
        {
            /* xcat and above */

            /* PCL */
            errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.pclInterruptCauseRegAddr =
                0x0E0001A4; /* TCC (lower) for PCL register */
            errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.pclInterruptIndex =
                PRV_CPSS_XCAT_TCC_LOWER_SUM_TCAM_ERROR_DETECTED_E;/* value for xcat (non A0) */

            /* Router */
            errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.routerInterruptCauseRegAddr =
                0x0E800060; /* TCC (upper) for router register */
            errataPtr->info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.routerInterruptIndex =
                PRV_CPSS_XCAT_TCC_UPPER_SUM_TCAM_ERROR_DETECTED_E;/* value for xcat (non A0) */
        }


        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            /* fill additional errata/RM for the xcat */
            for(ii = 0 ; xcatA1ErrAndRmArray[ii] != LAST_DEV_IN_LIST_CNS ; ii++)
            {
                PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,xcatA1ErrAndRmArray[ii]);
            }

            /* check revision ID */
            if (PRV_CPSS_PP_MAC(devNum)->revision >= 3)
            {
               /* XCAT A2 and above, do not set 256*n+[1..4] padding WA */
                PRV_CPSS_DXCH_ERRATA_CLEAR_MAC(devNum, PRV_CPSS_DXCH_XCAT_TX_CPU_CORRUPT_BUFFER_WA_E);

               /* XCAT A2 and above, do not Trunk To CPU WA but enable HW fix */
                PRV_CPSS_DXCH_ERRATA_CLEAR_MAC(devNum, PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E);
               /* RM#3017 - Enable CPU lacks source port information on traffic received from trunk workaround */
                PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum, PRV_CPSS_DXCH_XCAT_RM_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E);
                /* FEr#3054: Gig port PRBS is not functional. */
                PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum, PRV_CPSS_DXCH_XCAT_GIGA_PORT_PRBS_PARAM_INIT_WA_E);
            }
        }

        return GT_OK;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        /* Rm#83 - XG PHY path failure.Wrong value of PHY register 0xFF34. */
        PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                    PRV_CPSS_DXCH_RM_XG_PHY_PATH_FAILURE_WA_E);

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH2_E)
        {
            /* layer 4 sanity checks are enabled by default - RM#3018*/
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                            PRV_CPSS_DXCH2_RM_BRG_LAYER_4_SANITY_CHECKS_WA_E);

            /* FEr#1084 - Wrong data when reading Source-ID Assignment Mode Conf. reg.
               Shadow of the register should be used by SW. */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH2_READ_SRC_ID_ASSIGMENT_MODE_CONF_REG_WA_E);

            /* FEr#1006 -  Bridge Access Matrix Line <7> register is write only.
               Shadow of the register should be used by SW. */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH2_BRIDGE_ACCESS_MATRIX_LINE7_WRITE_WA_E);

            /* FEr#1001 - Direct access to policer engine address space is not functional */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH2_DIRECT_ACCESS_TO_POLICER_ADDRESS_SPACE_WA_E);

            /* FEr#1003 - Router ARP and Tunnel Start table do not support burst access via PCI */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH2_ROUTER_ARP_AND_TS_TABLE_NOT_SUPPORT_BURST_ACCESS_VIA_PCI_WA_E);

            /* FEr#1007 - RdWrTrig, bit[15] in the VLT Tables Access Control register is ignored */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH2_RDWRTRIG_BIT_IN_VLT_TABLES_ACCESS_CONTROL_REGISTER_IGNORED_E);

            /* FEr#1062 - Indirect access for Router and Tunnel Termination (TT)
                          TCAM does not function correctly */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH2_INDIRECT_ACCESS_ROUTER_AND_TT_TCAM_NOT_FUNCTION_CORRECTLY_WA_E);

            /* RM#1081 - Reserved registers 0x0B820108 */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH2_RM_RESERVED_REGISTERS_0x0B820108_WA_E);

            /* FEr#1087 - Mismatch in Policy Action entry table for Egress PCL */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH2_PCL_EGRESS_ACTION_CNTR_VLAN_WRONG_BITS_POS_WA_E);

           /* Direct access to Policy Action entry table non-burst */
            /* is not functional (FEr#1090)                         */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH2_POLICY_ACTION_DIRECT_BURST_ONLY_READ_WA_E);

            return GT_OK;

        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* In Jumbo frames mode the maximal SDWRR weight that may be
               configured is 254. (FEr#29) */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH_JUMBO_FRAMES_MODE_SDWRR_WEIGHT_LIMITATION_WA_E);

            /* Trunk configuration for Trunk Next Hop (FEr#137) */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH_TRUNK_CONFIG_FOR_TRUNK_NEXT_HOP_WA_E);

            /*In FDB Aging with removal mode, AA messages are sent for entries
              removed due to an invalid device (FEr#22)*/
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH_FDB_AGING_WITH_REMOVAL_MODE_WA_E);

            /* The maximum value of the 1000MWindow field, in the Ingress Rate
                limit Configuration Register0 is 31. (FEr#140) */
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH_RATE_LIMIT_GIG_COUNTER_BITS_NUM_WA_E);

            /* Wrong read value for oversize and jabber counters in XG MIBs (FEr#85)*/
            PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH_XG_MIB_READ_OVERSIZE_AND_JABBER_COUNTERS_WA_E);

            if((PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX163_CNS) ||
               (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX243_CNS) ||
               (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX262_CNS))
            {
                /* Some FDB table entries must be invalidated (RM#28) */
                PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                        PRV_CPSS_DXCH_RM_FDB_TABLE_ENTRIES_WA_E);
            }


            return GT_OK;
        }
    }

    return GT_OK;
}
/*******************************************************************************
* hwPpAutoDetectCoreClock
*
* DESCRIPTION:
*       Retrieves Core Clock value
*
* INPUTS:
*       devNum         - The Pp's device numer.
*
* OUTPUTS:
*       coreClkPtr     - Pp's core clock in MHz
*
* RETURNS:
*       GT_OK        - on success,
*       GT_BAD_STATE - can't map HW value to core clock value.
*       GT_FAIL      - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpAutoDetectCoreClock
(
    IN  GT_U8   devNum,
    OUT GT_U32  *coreClkPtr
)
{

    GT_U32 data;
    PRV_CPSS_GEN_PP_CONFIG_STC *devPtr;/* pointer to common device info */

    devPtr = PRV_CPSS_PP_MAC(devNum);

    switch(devPtr->devFamily)
    {
        case CPSS_PP_FAMILY_CHEETAH_E:
           /* retrieve PP Core Clock from Sampled at Reset reg 0x00000004 field PLL_CONFIG */
            if (prvCpssDrvHwPpGetRegField(devNum,0x000004,16,4,&data) != GT_OK)
                return GT_FAIL;

            if (devPtr->devType == CPSS_98DX106_CNS)
            {
                switch (data)
                {
                    case 0 : *coreClkPtr = 144; break;
                    case 1 : *coreClkPtr = 141; break;
                    case 2 : *coreClkPtr = 156; break;
                    default:  return GT_BAD_STATE;
                }
            }
            else
            {
                switch (data)
                {
                    case 0 : *coreClkPtr = 141; break;
                    case 1 : *coreClkPtr = 150; break;
                    case 2 : *coreClkPtr = 166; break;
                    case 3 : *coreClkPtr = 175; break;
                    case 4 : *coreClkPtr = 187; break;
                    case 5 : *coreClkPtr = 200; break;
                    case 6 : *coreClkPtr = 220; break;
                    case 8 : *coreClkPtr = 195; break;
                    case 9 :
                        if (devPtr->devType == CPSS_98DX249_CNS ||
                            devPtr->devType == CPSS_98DX269_CNS )
                            *coreClkPtr = 191;
                        else
                            *coreClkPtr = 197;
                        break;
                    case 10 : *coreClkPtr = 198; break;
                    case 11 : *coreClkPtr = 206; break;
                    case 12 : *coreClkPtr = 200; break;
                    case 13 : *coreClkPtr = 222; break;
                    case 14 : *coreClkPtr = 225; break;
                    case 15 : *coreClkPtr = 250; break;

                    default:  return GT_BAD_STATE;
                }
            }
            break;

        case CPSS_PP_FAMILY_CHEETAH2_E:
           /* retrieve PP Core Clock from Sampled at Reset reg 0x00000004 field PLL_CONFIG */
            if (prvCpssDrvHwPpGetRegField(devNum,0x000004,16,4,&data) != GT_OK)
                return GT_FAIL;

            switch (data)
            {
                case 0 :    *coreClkPtr = 141; break;
                case 1 :    *coreClkPtr = 154; break;
                case 2 :    *coreClkPtr = 167; break;
                case 3 :    *coreClkPtr = 180; break;
                case 4 :    *coreClkPtr = 187; break;
                case 5 :    *coreClkPtr = 194; break;
                case 6 :    *coreClkPtr = 207; break;
                case 8 :    *coreClkPtr = 221; break;
                case 9 :    *coreClkPtr = 210; break;
                case 10 :   *coreClkPtr = 214; break;
                case 11 :   *coreClkPtr = 217; break;
                case 12 :   *coreClkPtr = 200; break;
                case 13 :   *coreClkPtr = 224; break;
                case 14 :   *coreClkPtr = 228; break;
                case 15 :   *coreClkPtr = 233; break;

                default:    return GT_BAD_STATE;
            }
            break;

        case CPSS_PP_FAMILY_CHEETAH3_E:
           /* retrieve PP Core Clock from Sampled at Reset - CorePllConfig */
            if (prvCpssDrvHwPpGetRegField(devNum,0x00000028,22,4,&data) != GT_OK)
                return GT_FAIL;
            switch (data)
            {
                case 0 :    *coreClkPtr = 271; break;
                case 1 :    *coreClkPtr = 275; break;
                case 2 :    *coreClkPtr = 267; break;
                case 3 :    *coreClkPtr = 250; break;
                case 4 :    *coreClkPtr = 221; break;
                case 5 :    *coreClkPtr = 204; break;
                case 6 :    *coreClkPtr = 200; break;
                case 7 :    *coreClkPtr = 196; break;
                case 8 :    *coreClkPtr = 187; break;
                case 9 :    *coreClkPtr = 179; break;
                case 10 :   *coreClkPtr = 175; break;
                case 11 :   *coreClkPtr = 321; break;
                case 12 :   *coreClkPtr = 317; break;
                case 13 :   *coreClkPtr = 308; break;
                case 14 :   *coreClkPtr = 300; break;

                    case 15:  /*Bypass; PLL0 is bypassed.*/
                default:    return GT_BAD_STATE;
            }
            break;

        case CPSS_PP_FAMILY_DXCH_XCAT_E:
            /* retrieve PP Core Clock from Sampled at Reset - CorePllConfig */
            if (prvCpssDrvHwPpGetRegField(devNum,0x0000002C,2,3,&data) != GT_OK)
                return GT_FAIL;
            switch(data)
            {
                case 0x0:  *coreClkPtr = 250; break;
                case 0x1:  *coreClkPtr = 222; break;
                case 0x2:  *coreClkPtr = 200; break;
                case 0x3:  *coreClkPtr = 182; break;
                case 0x4:  *coreClkPtr = 167; break;
                case 0x5:  *coreClkPtr = 154; break;
                case 0x6:  *coreClkPtr = 143; break;
                case 0x7:  *coreClkPtr = 133; break;
                default:    return GT_BAD_STATE;
            }
            break;
        case CPSS_PP_FAMILY_DXCH_LION_E:
                /* retrieve PP Core Clock from Sampled at Reset - CorePllConfig */
                if (prvCpssDrvHwPpGetRegField(
                    devNum, 0x00000028 /*globalRegs.sampledAtResetReg*/,
                    2, 2, &data) != GT_OK)
                    return GT_FAIL;
                switch(data)
                {
                    case 0x0:  *coreClkPtr = 360; break; /* 0x0 - REF_CLK_25 x 14.4; Frequency = 360 MHz */
                    case 0x1:  *coreClkPtr = 320; break; /* 0x1 - REF_CLK_25 x 12.8; Frequency = 320 MHz */
                    case 0x2:  *coreClkPtr = 300; break; /* 0x2 - REF_CLK_25 x 12; Frequency = 300 MHz   */
                    case 0x3:  /*Bypass; PLL0 is bypassed.*/
                    default:    return GT_BAD_STATE;
                }
            break;

        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            /* retrieve PP Core Clock from Sampled at Reset - CorePllConfig */
            if (prvCpssDrvHwPpGetRegField(devNum,0x0000002C,2,3,&data) != GT_OK)
                return GT_FAIL;
            switch(data)
            {
                case 0x0:  *coreClkPtr = 250; break;
                case 0x1:  *coreClkPtr = 222; break;
                case 0x2:  *coreClkPtr = 200; break;
                case 0x3:  *coreClkPtr = 182; break;
                case 0x4:  *coreClkPtr = 167; break;
                case 0x5:  *coreClkPtr = 154; break;
                case 0x6:  *coreClkPtr = 143; break;
                case 0x7:  *coreClkPtr = 133; break;
                default:    return GT_BAD_STATE;
            }
            break;

        default:
            return GT_NOT_SUPPORTED;
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpPhase1Part1
*
* DESCRIPTION:
*       Part 1 of "phase 1"
*       Performs configures that should be done at the start of "Phase 1" :
*       initialize the cpssDriver
*       build the DB needed for the device
*
*       function called from the "phase 1" sequence.
*
* INPUTS:
*       ppPhase1ParamsPtr  - Packet processor hardware specific parameters.
*       multiPortGroupDevice    - indication that the device supposed to be 'multi port groups' device.
*
* OUTPUTS:
*       deviceTypePtr      - (pointer to) The Pp's device type.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_NOT_SUPPORTED - the device not supported by CPSS
*       GT_BAD_VALUE - the driver found unknown device type
*       GT_NOT_IMPLEMENTED - the CPSS was not compiled properly
*
* COMMENTS:
*       was called "pre start init"
*
*******************************************************************************/
static GT_STATUS hwPpPhase1Part1
(
    IN      CPSS_DXCH_PP_PHASE1_INIT_INFO_STC   *ppPhase1ParamsPtr,
    IN      GT_BOOL                             multiPortGroupDevice,
    OUT     CPSS_PP_DEVICE_TYPE                 *deviceTypePtr
)
{
    GT_STATUS           rc;
    GT_U8               devNum; /* device number of the device */
    GT_U32              port;   /* iterator */
    GT_BOOL             devMatched;/* did we matched the device --> use for loop on port types*/
    GT_U32              portGroupId;/*iterator for port group Id*/
    GT_U32              firstActivePortGroupId;/* port group Id for the first active port group */
    GT_U32              stackPortsMode = 0;
    GT_U32              metalFixData;

/* info needed by the cpssDriver */
    PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC drvPpPhase1InInfo;
    /* info returned from the cpssDriver */
    PRV_CPSS_DRV_PP_PHASE_1_OUTPUT_INFO_STC drvPpPhase1OutInfo;
    PRV_CPSS_GEN_PP_CONFIG_STC *devPtr;/* pointer to common device info */
    PRV_CPSS_DXCH_MODULE_CONFIG_STC *moduleCfgPtr;/* module config of the device */
    GT_BOOL         pciCompatible;

    devNum = ppPhase1ParamsPtr->devNum;

    /* prepare INPUT info for the cpssDriver */
    drvPpPhase1InInfo.mngInterfaceType = ppPhase1ParamsPtr->mngInterfaceType;

    if((ppPhase1ParamsPtr->mngInterfaceType == CPSS_CHANNEL_PCI_E) ||
       (ppPhase1ParamsPtr->mngInterfaceType == CPSS_CHANNEL_PEX_E))
    {
        pciCompatible = GT_TRUE;
    }
    else
    {
        pciCompatible = GT_FALSE;
    }

    if(multiPortGroupDevice == GT_FALSE)
    {
        firstActivePortGroupId = 0;
        drvPpPhase1InInfo.busBaseAddr[0] = ppPhase1ParamsPtr->busBaseAddr;
        if(pciCompatible == GT_TRUE)
        {
            drvPpPhase1InInfo.internalPciBase[0] = ppPhase1ParamsPtr->internalPciBase;
        }
        else
        {
            drvPpPhase1InInfo.internalPciBase[0] = 0;/* not relevant - for non PCI */
        }
        drvPpPhase1InInfo.numOfPortGroups = 1; /* non multi-port-group device */
    }
    else
    {
        firstActivePortGroupId = 0xFF;
        /* the per port group information taken from ppPhase1ParamsPtr->multiPortGroupsInfoPtr[portGroupId] */
        /* NOTE: can't use macro PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
                at this stage because the macro uses DB that was not created yet */
        for(portGroupId = 0 ; portGroupId < ppPhase1ParamsPtr->numOfPortGroups ; portGroupId++)
        {
            drvPpPhase1InInfo.busBaseAddr[portGroupId] = ppPhase1ParamsPtr->multiPortGroupsInfoPtr[portGroupId].busBaseAddr;

            if(firstActivePortGroupId == 0xFF &&/*not initialized yet*/
               drvPpPhase1InInfo.busBaseAddr[portGroupId] != CPSS_PARAM_NOT_USED_CNS)/*active port group*/
            {
                firstActivePortGroupId = portGroupId;
            }


            if(pciCompatible == GT_TRUE)
            {
                drvPpPhase1InInfo.internalPciBase[portGroupId] = ppPhase1ParamsPtr->multiPortGroupsInfoPtr[portGroupId].internalPciBase;
            }
            else
            {
                drvPpPhase1InInfo.internalPciBase[portGroupId] = 0;/* not relevant - for non PCI */
            }
        }

        drvPpPhase1InInfo.numOfPortGroups = ppPhase1ParamsPtr->numOfPortGroups;
    }

    drvPpPhase1InInfo.ppHAState = ppPhase1ParamsPtr->ppHAState;

    drvPpPhase1InInfo.devIdLocationType = PRV_CPSS_DRV_DEV_ID_LOCATION_DEVICE_ADDR_0x0000004C_E;

    /* call cpssDriver to initialize itself - phase 1 */
    rc = prvCpssDrvHwPpPhase1Init(devNum,
                                  &drvPpPhase1InInfo,
                                  &drvPpPhase1OutInfo);
    if(rc != GT_OK)
    {
        return rc;
    }
    /* set the out put parameter */
    *deviceTypePtr = drvPpPhase1OutInfo.devType;


    if(! CPSS_IS_DXCH_FAMILY_MAC(drvPpPhase1OutInfo.devFamily))
    {
            /* we got bad value from the cpssDriver !
                that means that there is problem in the cpssDriver or that the
                PP , is not Dx supported device.
            */
            return GT_BAD_VALUE;
    }

    /* allocate the memory for the device DB */
    prvCpssPpConfig[devNum] = cpssOsMalloc(sizeof(PRV_CPSS_DXCH_PP_CONFIG_STC));

    if (PRV_CPSS_PP_MAC(devNum) == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    dxCh_prvCpssPpConfig[devNum] = prvCpssPpConfig[devNum];

    /* reset the device DB */
    cpssOsMemSet(PRV_CPSS_PP_MAC(devNum) , 0 ,
                                sizeof(PRV_CPSS_DXCH_PP_CONFIG_STC));

    devPtr = PRV_CPSS_PP_MAC(devNum);
    moduleCfgPtr = PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(devNum);
    /*******************************************/
    /* start filling the info to the device DB */
    /*******************************************/
    /* set the device number */
    devPtr->devNum = devNum;

    /* fill info retrieved from the cpssDriver */
    devPtr->devType = drvPpPhase1OutInfo.devType;
    devPtr->revision = drvPpPhase1OutInfo.revision;
    devPtr->devFamily = drvPpPhase1OutInfo.devFamily;
    devPtr->numOfPorts = drvPpPhase1OutInfo.numOfPorts;
    devPtr->existingPorts = drvPpPhase1OutInfo.existingPorts;
    devPtr->numOfVirtPorts = devPtr->numOfPorts;

    /* set this device to support the "DxCh" functions */
    devPtr->functionsSupportedBmp = PRV_CPSS_DXCH_FUNCTIONS_SUPPORT_CNS;
    devPtr->appDevFamily = CPSS_CH1_E;
    if (drvPpPhase1OutInfo.devType == CPSS_98DX106_CNS)
    {
        devPtr->appDevFamily = CPSS_CH1_DIAMOND_E;
    }

    PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) = 31;
    PRV_CPSS_DXCH_PP_HW_INFO_L2I_MAC(devNum).supportIngressRateLimiter = GT_TRUE;

    if(drvPpPhase1OutInfo.devFamily >= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        /* set this device ALSO support the "Cheetah 2" functions */
        devPtr->functionsSupportedBmp |= PRV_CPSS_DXCH2_FUNCTIONS_SUPPORT_CNS;
        devPtr->appDevFamily = CPSS_CH2_E;
    }

    if(drvPpPhase1OutInfo.devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
    {
        /* set this device ALSO support the "Cheetah 3" functions */
        devPtr->functionsSupportedBmp |= PRV_CPSS_DXCH3_FUNCTIONS_SUPPORT_CNS;
        devPtr->appDevFamily = CPSS_CH3_E;
    }

    if(drvPpPhase1OutInfo.devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        /* set this device ALSO support the "DxCh xCat" functions */
        devPtr->functionsSupportedBmp |= PRV_CPSS_DXCH_XCAT_FUNCTIONS_SUPPORT_CNS;
        devPtr->appDevFamily = CPSS_XCAT_E;
    }

    if(drvPpPhase1OutInfo.devFamily >= CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* set this device ALSO support the "DxCh Lion" functions */
        devPtr->functionsSupportedBmp |= PRV_CPSS_DXCH_LION_FUNCTIONS_SUPPORT_CNS;
        devPtr->appDevFamily = CPSS_LION_E;

        /* L2I not support rate limiter */
        PRV_CPSS_DXCH_PP_HW_INFO_L2I_MAC(devNum).supportIngressRateLimiter = GT_FALSE;

        /* TXQ upgraded */
        PRV_CPSS_DXCH_PP_HW_INFO_TXQ_MAC(devNum).revision = 1;

        /* bit index 63 for cpu port */
        PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) = 63;

        switch(drvPpPhase1OutInfo.devFamily)
        {
            case CPSS_PP_FAMILY_DXCH_XCAT2_E:

                /* TXQ revision */
                PRV_CPSS_DXCH_PP_HW_INFO_TXQ_MAC(devNum).revision = 0;

                /* L2I support rate limiter */
                PRV_CPSS_DXCH_PP_HW_INFO_L2I_MAC(devNum).supportIngressRateLimiter = GT_TRUE;

                break;
            /* ADD HERE new devices that need to override parameters (like for xCat2) */
            default:
                break;
        }

    }

    if(drvPpPhase1OutInfo.devFamily >= CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* set this device ALSO support the "DxCh xCat2" functions */
        devPtr->functionsSupportedBmp |= PRV_CPSS_DXCH_XCAT2_FUNCTIONS_SUPPORT_CNS;
        devPtr->appDevFamily = CPSS_XCAT2_E;
    }

    /* set default CPU port mode to none */
    devPtr->cpuPortMode = PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_NONE_E;

    /* Events config */
    if(multiPortGroupDevice == GT_FALSE)
    {
        /* set the device as 'non multi-port-groups' device '*/
        rc = prvCpssNonMultiPortGroupsDeviceSet(devNum);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* from this point on we can start use the macros of:
           PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC,
           PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC
        */

        moduleCfgPtr->ppEventsCfg[CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS].intVecNum = ppPhase1ParamsPtr->intVecNum;
        moduleCfgPtr->ppEventsCfg[CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS].intMask   = ppPhase1ParamsPtr->intMask;

        devPtr->useIsr = (ppPhase1ParamsPtr->intVecNum == CPSS_PP_INTERRUPT_VECTOR_NOT_USED_CNS) ?
                        GT_FALSE : GT_TRUE;

        /* attach the device with it base address , needed to support re-init after
           'shutdown' */
        rc = prvCpssPpConfigDevDbBusBaseAddrAttach(devNum,ppPhase1ParamsPtr->busBaseAddr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        devPtr->portGroupsInfo.isMultiPortGroupDevice = GT_TRUE;

        devPtr->portGroupsInfo.numOfPortGroups = ppPhase1ParamsPtr->numOfPortGroups;
        for(portGroupId = 0 ; portGroupId < ppPhase1ParamsPtr->numOfPortGroups ; portGroupId++)
        {
            if(ppPhase1ParamsPtr->multiPortGroupsInfoPtr[portGroupId].busBaseAddr !=
               CPSS_PARAM_NOT_USED_CNS)
            {
                devPtr->portGroupsInfo.activePortGroupsBmp |= (1 << portGroupId);

                if(devPtr->portGroupsInfo.firstActivePortGroup > portGroupId)
                {
                    devPtr->portGroupsInfo.firstActivePortGroup = portGroupId;
                }

                if(devPtr->portGroupsInfo.lastActivePortGroup < portGroupId)
                {
                    devPtr->portGroupsInfo.lastActivePortGroup = portGroupId;
                }
            }
        }

        firstActivePortGroupId = devPtr->portGroupsInfo.firstActivePortGroup;

        /* set the devPtr->portGroupsInfoPtr->cpuPortPortGroupId

           NOTE: 1. For RGMII should be 3
                 2. For SDMA should be CPSS_PORT_GROUP_UNAWARE_MODE_CNS
                 3. This portGroupId used only for 'cpu port' and not for all networkIf
                    configurations that may use firstActivePortGroup!!!
        */
        if(pciCompatible == GT_TRUE)
        {
            devPtr->portGroupsInfo.cpuPortPortGroupId       = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }
        else/* non PCI/PEX */
        {
            devPtr->portGroupsInfo.cpuPortPortGroupId       = LION_RGMII_PORT_GROUP_ID_CNS;

            /* check that this port group is active ... */
            if (0==(devPtr->portGroupsInfo.activePortGroupsBmp
                   & (1 <<devPtr->portGroupsInfo.cpuPortPortGroupId)))
            {
                /* the needed port group is not active , so can't be used for RGMII */
                return GT_BAD_PARAM;
            }
        }

        devPtr->portGroupsInfo.auqPortGroupIdLastServed = firstActivePortGroupId;
        devPtr->portGroupsInfo.fuqPortGroupIdLastServed = firstActivePortGroupId;
        devPtr->portGroupsInfo.secureBreachNextPortGroupToServe = firstActivePortGroupId;
        devPtr->portGroupsInfo.ptpNextPortGroupToServe = firstActivePortGroupId;


        /* from this point on we can start use the macros of:
           PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC,
           PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC
        */

        devPtr->useIsr = GT_FALSE;

        /* the per port group information taken from ppPhase1ParamsPtr->multiPortGroupsInfoPtr[portGroupId] */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            moduleCfgPtr->ppEventsCfg[portGroupId].intVecNum  = ppPhase1ParamsPtr->multiPortGroupsInfoPtr[portGroupId].intVecNum;
            moduleCfgPtr->ppEventsCfg[portGroupId].intMask    = ppPhase1ParamsPtr->multiPortGroupsInfoPtr[portGroupId].intMask;
            if(ppPhase1ParamsPtr->multiPortGroupsInfoPtr[portGroupId].intVecNum != CPSS_PP_INTERRUPT_VECTOR_NOT_USED_CNS)
            {
                /* at least one active port group , needs the ISR */
                devPtr->useIsr = GT_TRUE;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

        /* attach the device with it base address , needed to support re-init after
           'shutdown' */
        /* NOTE: we use a 'designated' for a 'multi-port-groups' device base address , to identify the device .
                 so we not need to change implementation */
        rc = prvCpssPpConfigDevDbBusBaseAddrAttach(devNum,
                ppPhase1ParamsPtr->multiPortGroupsInfoPtr[firstActivePortGroupId].busBaseAddr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* sets HW interface parameters , only after this point you can use the
        PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC , to know that the device is PCICompatible*/
    rc = prvCpssHwIfSet(devNum,ppPhase1ParamsPtr->mngInterfaceType);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* save HW devNum to the DB */
    PRV_CPSS_HW_DEV_NUM_MAC(devNum) = devNum;

    /* must be done before doing any busy wait on a device  */
    switch(ppPhase1ParamsPtr->mngInterfaceType)
    {
        case CPSS_CHANNEL_SMI_E:
        case CPSS_CHANNEL_TWSI_E:
            moduleCfgPtr->miscellaneous.maxIterationsOfBusyWait =
                78125; /* For SMI MDC 20Mhz the read/write register longs
                         then near 12.8 micro seconds.
                         78125 read registers give us about 1 second */
            break;
        case CPSS_CHANNEL_PCI_E:
        case CPSS_CHANNEL_PEX_E:
            moduleCfgPtr->miscellaneous.maxIterationsOfBusyWait =
                66000000;/* 66 million for PCI bus with rate of 66MHz ,
                            and every read operation is about 2 clocks
                            give us about 2 second.
                            For PEX x1 it give us about 1 second */
            break;
        default:
            return GT_BAD_PARAM;
    }


    if(drvPpPhase1OutInfo.devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        devPtr->baseCoreClock   = 200;
    }
    else if(drvPpPhase1OutInfo.devFamily == CPSS_PP_FAMILY_CHEETAH2_E)
    {
        devPtr->baseCoreClock   = 220;
    }
    else if(drvPpPhase1OutInfo.devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        devPtr->baseCoreClock   = 270;
    }
    else if(drvPpPhase1OutInfo.devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        if(devPtr->revision != 0)
        {
            devPtr->baseCoreClock   = 222;
        }
        else
        {
            devPtr->baseCoreClock   = 200;
        }
    }
    else if(drvPpPhase1OutInfo.devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        devPtr->baseCoreClock   = 360;
    }
    else if(drvPpPhase1OutInfo.devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        devPtr->baseCoreClock   = 222;
    }
    else
    {
        return GT_NOT_SUPPORTED;
    }

    /* check if core clock needed to be read from hardware */
    if (ppPhase1ParamsPtr->coreClock == CPSS_DXCH_AUTO_DETECT_CORE_CLOCK_CNS)
    {
        rc = hwPpAutoDetectCoreClock(devNum,&(devPtr->coreClock));
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        devPtr->coreClock = ppPhase1ParamsPtr->coreClock;
    }

    PRV_CPSS_DXCH_PP_MAC(devNum)->port.serdesRefClock = ppPhase1ParamsPtr->serdesRefClock;

    metalFixData = 0;
    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        /* save power on metal fix register           */
        /* needed for xCat A3 revision identification */
        rc = prvCpssDrvHwPpReadRegister(
            devNum, 0x54 /* MG metal Fix register */,
            &metalFixData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
             == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        /* If dxCh3 B2 device set value to 1 */
        rc =  prvCpssDrvHwPpGetRegField(devNum,
                                        0x780000C, /* BM MPPM configuration register */
                                        16, 1, &metalFixData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* default value for devices other then XCAT or dxCh3 B2 is 0 */
    PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.mg.metalFix = metalFixData;

    /* initialize all the WA that needed for the device */
    rc = hwPpPhase1NeededWaSettings(devNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* allocation of physical ports information - phyPortInfoArray */
    devPtr->phyPortInfoArray = (PRV_CPSS_PORT_INFO_ARRAY_STC*)
        cpssOsMalloc(sizeof(PRV_CPSS_PORT_INFO_ARRAY_STC) * devPtr->numOfPorts);

    if (devPtr->phyPortInfoArray == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }
    cpssOsMemSet(devPtr->phyPortInfoArray, 0,
                 (sizeof(PRV_CPSS_PORT_INFO_ARRAY_STC) * devPtr->numOfPorts));

    if (devPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        /* get the stacking ports mode*/
        if (prvCpssDrvHwPpGetRegField(devNum,0x00000028,26,4,&stackPortsMode) != GT_OK)
                return GT_FAIL;
    }

    /* init The port info struct to all ports */
    for (port = 0; port < devPtr->numOfPorts; port++)
    {
        if(!CPSS_PORTS_BMP_IS_PORT_SET_MAC(&devPtr->existingPorts , port))
        {
            /* set the non-exists ports*/
            devPtr->phyPortInfoArray[port].portType = PRV_CPSS_PORT_NOT_EXISTS_E;
                continue;
        }
        devPtr->phyPortInfoArray[port].portType = PRV_CPSS_PORT_GE_E;
        devPtr->phyPortInfoArray[port].portTypeOptions = PRV_CPSS_GE_PORT_GE_ONLY_E;
        devPtr->phyPortInfoArray[port].vctLengthOffset = 0;
        devPtr->phyPortInfoArray[port].portIfMode = CPSS_PORT_INTERFACE_MODE_NA_E;
        /* initializing port SMI and PHY address DB to indicate that these parameters have */
        /* not been set yet. */
        PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,port) =  PRV_CPSS_PHY_SMI_NO_PORT_ADDR_CNS;
        PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,port) = PRV_CPSS_PHY_SMI_NO_CTRL_ADDR_CNS;

        devMatched = GT_FALSE;

        switch(devPtr->devType)
        {
            case PRV_CPSS_CHEETAH3_XG_PORTS_DEVICES_CASES_MAC:
               /* Cheetah 3 XG device have only XG ports */
               devPtr->phyPortInfoArray[port].portType =
                   PRV_CPSS_PORT_XG_E;
               devPtr->phyPortInfoArray[port].portTypeOptions =
                   PRV_CPSS_XG_PORT_XG_ONLY_E;
                devMatched = GT_TRUE;
            break;
        }

        if (port < 24 &&
            devPtr->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
        {
            switch(devPtr->devType)
            {
                case PRV_CPSS_DXCH_XCAT_FE_DEVICES_CASES_MAC:
                case PRV_CPSS_DXCH_XCAT2_FE_DEVICES_CASES_MAC:
                    devPtr->phyPortInfoArray[port].portType =
                        PRV_CPSS_PORT_FE_E;
                    break;
                default:
                    devPtr->phyPortInfoArray[port].portType =
                        PRV_CPSS_PORT_GE_E;
                    break;
            }
        }
        else
        {
            if(devMatched == GT_TRUE)
            {
                /* stack port type is already set */
                continue;
            }

           devPtr->phyPortInfoArray[port].portType =
               PRV_CPSS_PORT_XG_E;

           devPtr->phyPortInfoArray[port].portTypeOptions =
               PRV_CPSS_XG_PORT_XG_ONLY_E;

           /* special cases for DX249/269/169 */
           if (devPtr->devType == CPSS_98DX249_CNS ||
               devPtr->devType == CPSS_98DX169_CNS)
           {
               if (port >= 25)
               {
                   devPtr->phyPortInfoArray[port].portTypeOptions =
                       PRV_CPSS_XG_PORT_HX_QX_ONLY_E;
               }
           }
           else if (devPtr->devType == CPSS_98DX269_CNS)
           {
               if (port == 25)
               {
                   devPtr->phyPortInfoArray[port].portTypeOptions =
                       PRV_CPSS_XG_PORT_XG_HX_QX_E;
               }
               else if (port == 26)
               {
                   devPtr->phyPortInfoArray[port].portTypeOptions =
                       PRV_CPSS_XG_PORT_HX_QX_ONLY_E;
               }
           }
           else if((devPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
                    || (devPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
           {
                devPtr->phyPortInfoArray[port].isFlexLink = GT_TRUE;

                if ((stackPortsMode & (1 << (port - 24))) == 0 )
                {
                    devPtr->phyPortInfoArray[port].portType =
                        PRV_CPSS_PORT_GE_E;
                }

                /* set the device with all ports with XG/DQX/DHX/XAUI  */
                /* in XCAT all flex links XG capable and we can't
                 * restrict application to hard coded order, e.g.: 24,25 - QX
                 * and 26,27 - XG etc. application may want to define 24,25 - XG
                 * and 26,27 - QX */
                /* in XCAT2 stack ports are GE only */
                devPtr->phyPortInfoArray[port].portTypeOptions =
                    (devPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E) ?
                                                    PRV_CPSS_GE_PORT_GE_ONLY_E :
                                                        PRV_CPSS_XG_PORT_XG_HX_QX_E;

            }/*end of PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E*/
            else if(devPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                devPtr->phyPortInfoArray[port].portTypeOptions = PRV_CPSS_XG_PORT_XLG_SGMII_E;
                devPtr->phyPortInfoArray[port].isFlexLink = GT_TRUE;
            }
        }/*end of port >= 24 || lion ports */
    }

    /* initialize info about those PP's family */
    if(prvCpssFamilyInfoArray[devPtr->devFamily] == NULL)
    {
        /* this family was not initialized yet */
        prvCpssFamilyInfoArray[devPtr->devFamily] =
            cpssOsMalloc(sizeof(PRV_CPSS_FAMILY_INFO_STC));

        if(prvCpssFamilyInfoArray[devPtr->devFamily] == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }

        cpssOsMemSet(prvCpssFamilyInfoArray[devPtr->devFamily],
                        0,sizeof(PRV_CPSS_FAMILY_INFO_STC));

        prvCpssFamilyInfoArray[devPtr->devFamily]->numReferences = 1;

        /* initialize specific family trunk info */
        prvCpssDxChTrunkFamilyInit(devPtr->devFamily);
    }
    else
    {
        prvCpssFamilyInfoArray[devPtr->devFamily]->numReferences++;
    }

    /* CNC parameter initialization */
    switch (devPtr->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlocks = 8;
            PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlockNumEntries = 256;
        break;

        case CPSS_PP_FAMILY_DXCH_LION_E:
            PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlocks = 8;
            PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlockNumEntries = 512;
        break;

        case CPSS_PP_FAMILY_DXCH_XCAT_E:
            PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlocks = 2;
            PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlockNumEntries = _2K;
        break;/* end xcat */

        case CPSS_PP_FAMILY_CHEETAH3_E:
            PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlocks = 10;
            PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlockNumEntries = _2K;
        break;

        default:
            /* the device not supports CNC */
            PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlocks = 0;
            PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlockNumEntries = 0;
            break;
    }

    /* allocate and fill the values of the registers addresses of the device */
        /* init the registers addresses inside
            PRV_CPSS_EXMX_DEV_REGS_MAC(devNum) and get the number of
           virtual ports */
    switch (devPtr->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            /* this code call to xCat2 specific function */
             rc = prvCpssDxChXcat2HwRegAddrInit(devNum);
             if(rc != GT_OK)
             {
                 return rc;
             }

        break;/* end xcat */

        case CPSS_PP_FAMILY_DXCH_LION_E:
            /* this code call to Lion specific function */
             rc = prvCpssDxChLionHwRegAddrInit(devNum);
             if(rc != GT_OK)
             {
                 return rc;
             }

        break;/* end Lion */

        case CPSS_PP_FAMILY_DXCH_XCAT_E:
            /* this code call to xCat specific function */
             rc = prvCpssDxChXcatHwRegAddrInit(devNum);
             if(rc != GT_OK)
             {
                 return rc;
             }

        break;/* end xcat */

        case CPSS_PP_FAMILY_CHEETAH3_E:
        /* this code call to cheetah3 specific function */
             rc = prvCpssDxCh3HwRegAddrInit(devNum);
             if(rc != GT_OK)
             {
                 return rc;
             }

        break;/* end cheetah3 */

        case CPSS_PP_FAMILY_CHEETAH2_E:
        /* this code call to cheetah2 specific function */
            rc = prvCpssDxCh2HwRegAddrInit(devNum);
            if(rc != GT_OK)
            {
                return rc;
            }

        break;/* end cheetah2 */

        case CPSS_PP_FAMILY_CHEETAH_E:
            /* call to Cheetah specific function */
            rc = prvCpssDxChHwRegAddrInit(devNum);
            if(rc != GT_OK)
            {
                return rc;
            }
        break;

        default:
            /* we may get here due to improper compilation flag used ! */
            return GT_DEVICE_FAMILY_NOT_SUPPORTED_CNS;
    }

    /* For Lion B check that correct base address per port groug as configured */
    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            /* The 2 LSbs - bits [10:9] - of field <DevicePhy Addr> - bits [13:9] */
            /* are the port group number.                                         */
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,
                                                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                                        globalRegs.globalControl,
                                                    9, 2, &port);
            if(rc != GT_OK)
            {
                return rc;
            }

            if( port != portGroupId )
            {
                return GT_BAD_PARAM;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    }


#ifdef DEBUG_OPENED
    /* check that all enum value also appear in the 'strings' for debug */
    if(errataNames[PRV_CPSS_DXCH_ERRATA_MAX_NUM_E - 1] == NULL)
    {
        /* don't fail the initialization , only prompt about the problem */
        DUMP_PRINT_MAC(("hwPpPhase1Part1: errataNames[] not filled with all names properly \n"));
    }
#endif /*DEBUG_OPENED*/

    return GT_OK;
}


/*******************************************************************************
* hwPpPhase1Part2Config
*
* DESCRIPTION:
*       This function performs basic hardware configurations on the given PP, in
*       Hw registration phase.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPhase1Part2Config
(
    IN      GT_U8                                   devNum
)
{
	osPrintfDbg("%s %s %d devNum %d num of ports %d \n", __FILE__, __FUNCTION__, __LINE__,devNum,PRV_CPSS_PP_MAC(devNum)->numOfPorts); /*xcat debug*/	
    osPrintfDbg("*******Here is the first time we set Device number !!! \n");
#if 0	
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC  * regsAddrPtr;
    GT_U32  port; /* iterator */
    PRV_CPSS_GEN_PP_CONFIG_STC *devPtr = PRV_CPSS_PP_MAC(devNum);/* pointer to common device info */

    /* init The port info struct to all ports */
    for (port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
    {
       if (port<24){
           PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portType =
           PRV_CPSS_PORT_GE_E;
		   osPrintfDbg("part2config %s %d dev %d port %d\n",__func__,__LINE__,devNum,port);
       	}
       else
       {
		   osPrintfDbg("part2config %s %d dev %d port %d\n",__func__,__LINE__,devNum,port);
           PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portType =
               PRV_CPSS_PORT_XG_E;
           PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portTypeOptions =
               PRV_CPSS_XG_PORT_XG_ONLY_E;

           /* special cases for DX249/269 */
           if (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX249_CNS)
           {
               if (port >= 25)
               {
                   PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portTypeOptions =
                       PRV_CPSS_XG_PORT_HX_QX_ONLY_E;
               }
           }
           else if (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX269_CNS)
           {
               if (port == 25)
               {
                   PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portTypeOptions =
                       PRV_CPSS_XG_PORT_XG_HX_QX_E;
               }
               else if (port == 26)
               {
                   PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portTypeOptions =
                       PRV_CPSS_XG_PORT_HX_QX_ONLY_E;
               }
           }
       }
    }

    /* set the non-exists ports*/
    for (port = 0; port < devPtr->numOfPorts; port++)
    {
        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&devPtr->existingPorts , port))
        {
            /* port exist and already set with proper value */
            continue;
        }

        /* override values for non-exists ports */
        devPtr->phyPortInfoArray[port].portType = PRV_CPSS_PORT_NOT_EXISTS_E;
    }

    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);
#endif	
    /* Device number        */
    cpssDxChCfgHwDevNumSet(devNum,devNum);
    osPrintfDbg("******* and globalControl: CPU enable  !!! \n");

    /* CPU enable           */
    if(prvCpssDrvHwPpSetRegField(devNum,
                                 PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.
                                 globalControl,1,1,1) != GT_OK)
        return GT_HW_ERROR;

    return GT_OK;
}

/*******************************************************************************
* hwPpPhase1Part2Enhanced_ch1_ch2
*
* DESCRIPTION:
*       This function performs Enhanced SW configurations for the given PP. - ch1,2
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPhase1Part2Enhanced_ch1_ch2
(
    IN      GT_U8                                   devNum
)
{
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    fineTuningPtr->tableSize.fdb                        = _16K;
    fineTuningPtr->tableSize.tunnelStart                = _1K; /* 1K TS entries */
    fineTuningPtr->tableSize.routerArp                  = _4K; /* ARP entries = 4 * TS entries */
    fineTuningPtr->tableSize.routerAndTunnelTermTcam    = _1K;
    fineTuningPtr->tableSize.routerNextHop              = _4K;
    fineTuningPtr->tableSize.mllPairs                   = _1K;
    fineTuningPtr->tableSize.policyTcamRaws             = 512;/* 512*2 --> '1K short rules' */
    fineTuningPtr->tableSize.cncBlocks                  = 0;
    fineTuningPtr->tableSize.transmitDescr              = 0;
    fineTuningPtr->tableSize.bufferMemory               = 0;

    fineTuningPtr->tableSize.vidxNum                    = VIDX_NUM_CNS;/* 0..0xffe */
    fineTuningPtr->tableSize.stgNum                     = STG_NUM_CNS;
    fineTuningPtr->tableSize.policersNum                = 256;
    fineTuningPtr->tableSize.trunksNum                  = TRUNKS_NUM_CNS;
    fineTuningPtr->tableSize.txQueuesNum                = TX_QUEUE_NUM_8_CNS;

    if(((PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX163_CNS) ||
        (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX243_CNS) ||
        (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX262_CNS))&&
       (PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
           PRV_CPSS_DXCH_RM_FDB_TABLE_ENTRIES_WA_E) == GT_FALSE))
    {
        fineTuningPtr->tableSize.fdb = _8K;
    }
    else if(PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX106_CNS)
    {
        fineTuningPtr->tableSize.fdb = _8K;
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpPhase1Part2Enhanced_ch3
*
* DESCRIPTION:
*       This function performs Enhanced SW configurations for the given PP. -- ch3
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPhase1Part2Enhanced_ch3
(
    IN      GT_U8                                   devNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    fineTuningPtr->tableSize.fdb                        = _32K;
    fineTuningPtr->tableSize.tunnelStart                = _4K;/* 4K TS entries */
    fineTuningPtr->tableSize.routerArp                  = _16K;/* ARP entries = 4 * TS entries */
    fineTuningPtr->tableSize.routerAndTunnelTermTcam    = _4K;/* 16K/4 rows '16k short rules'*/
    fineTuningPtr->tableSize.routerNextHop              = _16K;
    fineTuningPtr->tableSize.mllPairs                   = _2K;/*4K MLLs*/
    fineTuningPtr->tableSize.policyTcamRaws             = _2K;/*2048*4 --> '8K short rules' */
    fineTuningPtr->tableSize.cncBlocks                  = 8;/* 8 blocks of 2K counters */
    fineTuningPtr->tableSize.cncBlockNumEntries         = _2K;/*each CNC block is 2K entries*/
    fineTuningPtr->tableSize.transmitDescr              = _12K;
    fineTuningPtr->tableSize.bufferMemory               = _16M;

    fineTuningPtr->tableSize.vidxNum                    = VIDX_NUM_CNS;/* 0..0xffe */
    fineTuningPtr->tableSize.stgNum                     = STG_NUM_CNS;

    fineTuningPtr->tableSize.policersNum                = _1K;/*1K meters and another 1K counters*/
    fineTuningPtr->tableSize.trunksNum                  = TRUNKS_NUM_CNS;
    fineTuningPtr->tableSize.txQueuesNum                = TX_QUEUE_NUM_8_CNS;

    fineTuningPtr->featureInfo.vlanTranslationSupported = GT_TRUE;

    /* POLICY TCAM  size */
    rc = prvCpssDrvHwPpSetRegField(
        devNum,
        PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.pclRegs.pclTcamConfig0,
        1, 4, 3);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* ROUTER TCAM size */
    rc = prvCpssDrvHwPpSetRegField(
        devNum,
        PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.ipRegs.ipTcamConfig0,
        1, 4, 2);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpPhase1Part2Enhanced_common
*
* DESCRIPTION:
*       This function performs Enhanced SW configurations for the given
*       any device with features set in table format.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number.
*       tablesSize - number of elements in the 'fine tunning' table array(parameter tablePtr)
*       tablePtr - (array of) the table with the 'fine tuning' parameters
*       bufferMemoryTableSize - number of elements in the 'buffer memory' table array(parameter bufferMemoryTablePtr)
*       bufferMemoryTablePtr - (array of) the table with the 'buffer memory' parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_NOT_SUPPORTED - the device not found in the DB
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPhase1Part2Enhanced_common
(
    IN GT_U8   devNum,
    IN GT_U32  tablesSize,
    IN FINE_TUNING_STC *tablePtr,
    IN GT_U32  bufferMemoryTableSize,
    IN FINE_TUNING_BUFFER_MANAGEMENT_PARAMS_STC *bufferMemoryTablePtr
)
{
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;
    GT_U32  ii,jj;/*iterators*/
    GT_BOOL found = GT_FALSE;
    GT_U32  totalCncCountersNeeded;

    /* first set the 'Fine tuning' parameters , from the DB */
    for(ii = 0 ; ii < tablesSize ; ii++)
    {
        for(jj = 0 ; tablePtr[ii].devTypePtr[jj] != LAST_DEV_IN_LIST_CNS ; jj++)
        {
            if(tablePtr[ii].devTypePtr[jj] == PRV_CPSS_PP_MAC(devNum)->devType)
            {
                /* found the device type */

                /* copy it's info */
                fineTuningPtr->tableSize = tablePtr[ii].config.tableSize;

                fineTuningPtr->featureInfo = tablePtr[ii].config.featureInfo;

                found = GT_TRUE;
                break;
            }
        }

        if(found == GT_TRUE)
        {
            break;
        }
    }

    if(found == GT_FALSE)
    {
        /* the device type not supported in the DB */
        return GT_NOT_SUPPORTED;
    }

    found = GT_FALSE;

    /* now set the 'Buffer memory' parameters of the 'Fine tuning' parameters , from the DB */

    for(ii = 0 ; ii < bufferMemoryTableSize ; ii++)
    {
        for(jj = 0 ; bufferMemoryTablePtr[ii].devTypePtr[jj] != LAST_DEV_IN_LIST_CNS ; jj++)
        {
            if(bufferMemoryTablePtr[ii].devTypePtr[jj] == PRV_CPSS_PP_MAC(devNum)->devType)
            {
                /* found the device type */

                /* copy it's info */
                fineTuningPtr->tableSize.bufferMemory = bufferMemoryTablePtr[ii].config.bufferMemory;
                fineTuningPtr->tableSize.transmitDescr = bufferMemoryTablePtr[ii].config.transmitDescr;

                found = GT_TRUE;
                break;
            }
        }

        if(found == GT_TRUE)
        {
            break;
        }
    }

    if(fineTuningPtr->tableSize.transmitDescr == AUTO_CALC_FIELD_CNS)
    {
        fineTuningPtr->tableSize.transmitDescr =
            fineTuningPtr->tableSize.bufferMemory / _2K;/* 2K = 8bits * 256Byets per buffer */
    }

    if(fineTuningPtr->tableSize.routerArp == AUTO_CALC_FIELD_CNS)
    {
        fineTuningPtr->tableSize.routerArp =
            fineTuningPtr->tableSize.tunnelStart * 4; /* the number of ARP entries: ARP = TS*4 */
    }

    /*  fix errata -- table sizes */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_XCAT_TABLE_SIZE_FDB_16K_INSTEAD_8K_WA_E))
    {
        if(fineTuningPtr->tableSize.fdb != _16K)
        {
            fineTuningPtr->tableSize.fdb = _16K;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_XCAT_TABLE_SIZE_NH_4K_INSTEAD_8K_WA_E))
    {
        if(fineTuningPtr->tableSize.routerNextHop > _4K)
        {
            fineTuningPtr->tableSize.routerNextHop = _4K;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_XCAT_TABLE_SIZE_TS_1K_INSTEAD_2K_WA_E))
    {
        if(fineTuningPtr->tableSize.tunnelStart >= _1K)
        {
            fineTuningPtr->tableSize.tunnelStart = _1K;
        }
    }

    if(fineTuningPtr->tableSize.cncBlocks == CNC_AUTO_CALC)
    {
        /* support auto calculation */
        totalCncCountersNeeded = fineTuningPtr->tableSize.cncBlockNumEntries;
    }
    else
    {
        /* support explicit value definitions */
        totalCncCountersNeeded = fineTuningPtr->tableSize.cncBlockNumEntries *
                                 fineTuningPtr->tableSize.cncBlocks;
    }

    if(totalCncCountersNeeded != 0)
    {
        /* update value - to support actual device silicon values */
        fineTuningPtr->tableSize.cncBlockNumEntries = PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlockNumEntries;
        /* calculate the number of CNC blocks needed to support the total number of CNC counters */
        fineTuningPtr->tableSize.cncBlocks =
            (totalCncCountersNeeded + (PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlockNumEntries-1)) /
            PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlockNumEntries;

        /* check that the fine tuning value not overrun the HW max support */
        if(fineTuningPtr->tableSize.cncBlocks >
           PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlocks)
        {
            /* Error in fine tuning or in calculation */
            return GT_BAD_STATE;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_XCAT_TABLE_CNC_NUM_BLOCKS_1_INSTEAD_2_WA_E))
    {
        if (fineTuningPtr->tableSize.cncBlocks > 1)
        {
            fineTuningPtr->tableSize.cncBlocks = 1;
        }
        fineTuningPtr->tableSize.cncBlockNumEntries = PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(devNum).cncBlockNumEntries;
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_XCAT_TABLE_POLICERS_NUM_1K_INSTEAD_2K_WA_E))
    {
        if(fineTuningPtr->tableSize.policersNum > _1K)
        {
            fineTuningPtr->tableSize.policersNum = _1K;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_XCAT_TABLE_EPLR_NOT_SUPPORT_WA_E))
    {
        fineTuningPtr->tableSize.egressPolicersNum = 0;
    }


    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_XCAT_TR101_WA_E))
    {
        fineTuningPtr->featureInfo.tr101Supported = GT_FALSE;
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_XCAT_IPLR_SECOND_STAGE_SUPPORT_WA_E))
    {
        fineTuningPtr->featureInfo.iplrSecondStageSupported = GT_FALSE;
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_LION_TABLE_SIZE_ROUTER_TCAM_13K_INSTEAD_16K_WA_E))
    {
        if(fineTuningPtr->tableSize.routerAndTunnelTermTcam > (_13K / 4))
        {
            fineTuningPtr->tableSize.routerAndTunnelTermTcam = (_13K / 4);
        }
    }


    return GT_OK;
}

/*******************************************************************************
* hwPpPhase1Part2Enhanced_xcat
*
* DESCRIPTION:
*       This function performs Enhanced SW configurations for the given
*       DxCh xCat device.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPhase1Part2Enhanced_xcat
(
    IN      GT_U8     devNum
)
{
    GT_STATUS rc;       /* return code       */
    GT_U32    hwValue;  /* HW Value to write */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr =
        &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    rc = hwPpPhase1Part2Enhanced_common(devNum,
            dxChXcatTables_size,dxChXcatTables,
            dxChXcatBufferMemory_size,dxChXcatBufferMemory);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* PCL TCAM size in 4-STD-rule rows */
    switch (fineTuningPtr->tableSize.policyTcamRaws)
    {
        case 768: hwValue = 0; break;
        case 512: hwValue = 1; break;
        case 384: hwValue = 1; break;
        case 256: hwValue = 2; break;
        case 128: hwValue = 2; break;
        case 0: hwValue = 0xFFFFFFFF; break; /* skip configuration */
        default: return GT_FAIL;
    }

    if (hwValue != 0xFFFFFFFF)
    {
        /* POLICY TCAM  size */
        rc = prvCpssDrvHwPpSetRegField(
            devNum,
            PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.pclRegs.pclTcamConfig0,
            1, 4, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpPhase1Part2Enhanced_lion
*
* DESCRIPTION:
*       This function performs Enhanced SW configurations for the given
*       DxCh Lion device.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPhase1Part2Enhanced_lion
(
    IN      GT_U8     devNum
)
{
    GT_STATUS rc;

    rc = hwPpPhase1Part2Enhanced_common(devNum,
            lionTables_size,lionTables, 0 , NULL);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* add here lion B specifics */

    /* set the CPU as extended cascade port */
    rc = cpssDxChCscdPortTypeSet(devNum,
        (GT_U8)CPSS_CPU_PORT_NUM_CNS,
        CPSS_CSCD_PORT_DSA_MODE_EXTEND_E);
        return rc;
}

/*******************************************************************************
* hwPpPhase1Part2Enhanced_xcat2
*
* DESCRIPTION:
*       This function performs Enhanced SW configurations for the given
*       DxCh xCat2 device.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPhase1Part2Enhanced_xcat2
(
    IN      GT_U8     devNum
)
{
    GT_STATUS rc;       /* return code       */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr =
        &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    rc = hwPpPhase1Part2Enhanced_common(devNum,
            dxChXcatTables_size,dxChXcatTables,
            dxChXcatBufferMemory_size,dxChXcatBufferMemory);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* all xCat2 devices supports CRC Hash */
    fineTuningPtr->featureInfo.trunkCrcHashSupported = GT_TRUE;

    /* set the CPU as extended cascade port */
    rc = cpssDxChCscdPortTypeSet(devNum,
        (GT_U8)CPSS_CPU_PORT_NUM_CNS,
        CPSS_CSCD_PORT_DSA_MODE_EXTEND_E);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpPhase1Part2Enhanced
*
* DESCRIPTION:
*       This function performs Enhanced SW configurations for the given PP.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPhase1Part2Enhanced
(
    IN      GT_U8                                   devNum
)
{
    cpssOsMemSet(&PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning,0,
                sizeof(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning));

    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_CHEETAH_E:
        case CPSS_PP_FAMILY_CHEETAH2_E:
            return hwPpPhase1Part2Enhanced_ch1_ch2(devNum);
        case CPSS_PP_FAMILY_CHEETAH3_E:
            return  hwPpPhase1Part2Enhanced_ch3(devNum);
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
            return hwPpPhase1Part2Enhanced_xcat(devNum);
        case CPSS_PP_FAMILY_DXCH_LION_E:
            return hwPpPhase1Part2Enhanced_lion(devNum);
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            return hwPpPhase1Part2Enhanced_xcat2(devNum);
        default:
            return GT_OK;
    }
}

/*******************************************************************************
* hwPpPhase1Part2
*
* DESCRIPTION:
*       Part 2 of "phase 1"
*       This routine initialize the board specific parameters and registers
*       the packet processor for pre-discovery phase access and diagnostics.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - The Pp's device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on failure.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS hwPpPhase1Part2
(
    IN  GT_U8                               devNum
)
{
    GT_STATUS           rc;

    /* Set basic device registration parameters.     */
    rc = hwPpPhase1Part2Config(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Set Enhanced device capabilities parameters.     */
    rc = hwPpPhase1Part2Enhanced(devNum);

    return rc;
}


/*******************************************************************************
* hwPpPhase1Part3
*
* DESCRIPTION:
*       Part 3 of "phase 1"
*       Performs configures that should be done at the end of "Phase 1" and
*       before Pp "start-init" operation.
*       Set Errata issues and registers settings.
*
*       function called from the "phase 1" sequence.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - The Pp's device number to run the errata Wa on.
*       ppPhase1ParamsPtr  - Packet processor hardware specific parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       was called "pre start init"
*
*******************************************************************************/
static GT_STATUS hwPpPhase1Part3
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PP_PHASE1_INIT_INFO_STC       *ppPhase1ParamsPtr
)
{
     /* set default values of registers */
    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_CHEETAH_E:
            return hwPpCheetahRegsDefault(devNum);

        case CPSS_PP_FAMILY_CHEETAH2_E:
            return hwPpCheetah2RegsDefault(devNum);

        case CPSS_PP_FAMILY_CHEETAH3_E:
            return hwPpCheetah3RegsDefault(devNum, ppPhase1ParamsPtr);

        case CPSS_PP_FAMILY_DXCH_XCAT_E:
            return hwPpDxChXcatRegsDefault(devNum, ppPhase1ParamsPtr);

        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            return hwPpDxChXcat2RegsDefault(devNum, ppPhase1ParamsPtr);

        case CPSS_PP_FAMILY_DXCH_LION_E:
            return hwPpDxChLionRegsDefault(devNum, ppPhase1ParamsPtr);

        default:
            return GT_BAD_PARAM;
    }
}


/*******************************************************************************
* hwPpPhase1RegisterMisconfigurationAfterTablesInit
*
* DESCRIPTION:
*       RM (Register Miss-configuration) after calling to
*       prvCpssDxChTablesAccessInit(...)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - The Pp's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpPhase1RegisterMisconfigurationAfterTablesInit
(
    IN  GT_U8               devNum
)
{
    GT_STATUS   rc;
    /* data that will be written to the table */
    GT_U32 hwDataArray[6] = {0,0,0,0,0,0};
    GT_U32    port;    /* port number */
    CPSS_PORTS_BMP_STC        portBitmap;/* bit map of ports */
    GT_U16      vidx;/* vidx */
    GT_U32      regAddr;        /* register address */
    GT_U32      data;
    GT_U32      portGroupId;    /* port group id    */
    GT_U32      routerVlanUrpfTableEntryData[1]; /* size of each entry is 1 word */

    if(0 == PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* not xcat and above */
        return GT_OK;
    }

    /* clear entry for VLAN 0 in VLAN Table */
    /* The VLAN#0 is not initialized in HW */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH_TABLE_VLAN_E,
                                        0,/* vid */
                                        &hwDataArray[0]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* reset entry in the ingress */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
                                        0,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                        0,/* bit 0*/
                                        1,
                                        0);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* reset entry in the egress */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
                                        0,/* vid */
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                        0,/* bit 0*/
                                        1,
                                        0);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_XCAT_RM_EXTENDED_DSA_BYPASS_BRIDGE_WA_E))
    {
        /* enable Bypass Bridge for Extended DSA tag packets */
        for( port = 0; port <= PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
        {
            if(port == PRV_CPSS_PP_MAC(devNum)->numOfPorts)
            {
                port = CPSS_CPU_PORT_NUM_CNS; /* CPU port */
            }
            else
            {
                PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, port);
            }

            rc = prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                           port, /* index is port number */
                                           0, /* start at word 0 */
                                           8, /* field offset */
                                           1, /* 1 bit */
                                           1); /* enable bypass */
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_LION_RM_MULTI_PORT_GROUP_CPU_IN_VIDX_WA_E))
    {
        vidx = 0xFFF;
        rc = cpssDxChBrgMcEntryRead(devNum, vidx, &portBitmap);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* remove ports : 15,31,47 */
        CPSS_PORTS_BMP_PORT_CLEAR_MAC(&portBitmap,15);
        CPSS_PORTS_BMP_PORT_CLEAR_MAC(&portBitmap,31);
        CPSS_PORTS_BMP_PORT_CLEAR_MAC(&portBitmap,47);

        /* use private function to avoid flood VIDX check */
        rc = prvCpssDxChBrgMcEntryWrite(devNum, vidx, &portBitmap);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                               PRV_CPSS_DXCH_LION_URPF_PER_VLAN_NOT_SUPPORTED_WA_E))
    {
        /* Run a procedure that sets the VLAN uRPF table output to "uRPF Disable",
           thus effectively support uRPF check per route using the LTT.
           The procedure is write 0 to one of the accessible entries,
           read it and then turn off the RAM.
           By doing so the RAM output turns to '0' constantly. */

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            /* read register 0x02800D70 to verify that WA is not already activated (bits 30 and 19)*/
            regAddr = 0x02800D70  ;
            rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,
                                                     portGroupId,
                                                     regAddr,
                                                     &data);
            if(rc != GT_OK)
            {
                return rc;
            }

            if ((0x40080000 & data) > 0 )
            {
                continue;
            }

            /* set value 0 to regAddr = 0x2801400 --> PRV_CPSS_DXCH_LION_TABLE_ROUTER_VLAN_URPF_STC_E */
            routerVlanUrpfTableEntryData[0] = 0;

            rc = prvCpssDxChPortGroupWriteTableEntry(devNum,
                                                     portGroupId,
                                                     PRV_CPSS_DXCH_LION_TABLE_ROUTER_VLAN_URPF_STC_E,
                                                     0,
                                                     routerVlanUrpfTableEntryData);
            if(rc != GT_OK)
            {
                return rc;
            }

            rc = prvCpssDxChPortGroupReadTableEntry(devNum,
                                                 portGroupId,
                                                 PRV_CPSS_DXCH_LION_TABLE_ROUTER_VLAN_URPF_STC_E,
                                                 0,
                                                 routerVlanUrpfTableEntryData);
            if(rc != GT_OK)
            {
                return rc;
            }

            regAddr = 0x02800D70  ;
            rc = prvCpssDrvHwPpPortGroupWriteRegister(
                 devNum,
                 portGroupId,
                 regAddr,
                 0x58380000);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    }

    return GT_OK;
}

/*******************************************************************************
* xcatA1ErrataAddrCheckWaDbInit
*
* DESCRIPTION:
*       Initialize Unit Address Ranges Data Base for Address Check WA.
*       The DB will be initialized according to the following Address ranges
*       list:
*           Where Unit Id is defined by bits 23-28.
*
*           1.        0x02900000 - 0x0290FFFF   (Unit 5)
*               0x02C00000 - 0x02CFFFFF
*           2.        0x07E40000 - 0x07FC0000         (Unit 15)
*           3.        0x08800000 - 0x08801700         (Unit 17)
*               0x08801C00 - 0x08803F00
*           4.        0x09000000 - 0x092FFFF0         (Unit 18)
*               0x09380000 - 0x097FFFFF
*           5.        0x098007FF - 0x09800C00         (Unit 19)
*               0x098017FF - 0x09801C00
*               0x098027FF - 0x09802C00
*               0x098037FF - 0x09803C00
*               0x098047FF - 0x09804C00
*               0x098057FF - 0x09805C00
*               0x098067FF - 0x09806C00
*               0x098077FF - 0x09807C00
*               0x098087FF - 0x09808C00
*               0x098097FF - 0x09809C00
*               0x0980F7FF - 0x0980FC00
*           6.        0x0A807000 - 0x0A80F800        (Unit 21)
*
* APPLICABLE DEVICES:
*        xCat;
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3Lion; xCat2.
*
* INPUTS:
*       devNum          - The Pp's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       Note: The ranges should be in growing order.
*
*******************************************************************************/
static GT_VOID xcatA1ErrataAddrCheckWaDbInit
(
    IN  GT_U8               devNum
)
{
    GT_U32      unitId;     /* loop iterator */
    PRV_CPSS_DXCH_ERRATA_UNIT_RANGES_STC  emptyUnit = EMPTY_UNIT_MAC; /* empty unit ranges */


    /* initialize XCAT DB */
    for (unitId = 0; unitId < UNITS_NUM_CNS; unitId++)
    {
        xcatBadAddressRanges[unitId] = emptyUnit;
    }

    /* Unit 5 */
    xcatBadAddressRanges[5].numOfRanges = (sizeof(unit5RangesArray) / sizeof(unit5RangesArray[0]));
    xcatBadAddressRanges[5].rangesPtr = unit5RangesArray;

    /* Unit 15 */
    xcatBadAddressRanges[15].numOfRanges = (sizeof(unit15RangesArray) / sizeof(unit15RangesArray[0]));
    xcatBadAddressRanges[15].rangesPtr = unit15RangesArray;

    /* Unit 17 */
    xcatBadAddressRanges[17].numOfRanges = (sizeof(unit17RangesArray) / sizeof(unit17RangesArray[0]));
    xcatBadAddressRanges[17].rangesPtr = unit17RangesArray;

    /* Unit 18 */
    xcatBadAddressRanges[18].numOfRanges = (sizeof(unit18RangesArray) / sizeof(unit18RangesArray[0]));
    xcatBadAddressRanges[18].rangesPtr = unit18RangesArray;

    /* Unit 19 */
    xcatBadAddressRanges[19].numOfRanges = (sizeof(unit19RangesArray) / sizeof(unit19RangesArray[0]));
    xcatBadAddressRanges[19].rangesPtr = unit19RangesArray;

    /* Unit 21 */
    xcatBadAddressRanges[21].numOfRanges = (sizeof(unit21RangesArray) / sizeof(unit21RangesArray[0]));
    xcatBadAddressRanges[21].rangesPtr = unit21RangesArray;



    PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
        info_PRV_CPSS_DXCH_XCAT_RESTRICTED_ADDRESS_FILTERING_WA_E.unitRangesPtr[0]
                                                            = xcatBadAddressRanges;
}

/*******************************************************************************
* lionErrataAddrCheckWaDbInit
*
* DESCRIPTION:
*       Initialize Unit Address Ranges Data Base for Address Check WA.
*       The DB will be initialized according to the following Address ranges
*       list:
*           Where Unit Id is defined by bits 23-28.
*           PortGroups 1,2,3:
*           1.        0x03800000 - 0x038FFFFC   (Unit 7)
*           2.        0x07000000 - 0x0707FFFC   (Unit 14)
*           3.        0x0A000000 - 0x0A7FFFFC   (Unit 20)
*           4.        0x10800000 - 0x10FFFFFC   (Unit 33)
*           5.        0x11800000 - 0x11FFFFFC   (Unit 35) only for PortGroup 2
*
* APPLICABLE DEVICES:
*        Lion;
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - The Pp's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       Note: The ranges should be in growing order.
*
*******************************************************************************/
static GT_VOID lionErrataAddrCheckWaDbInit
(
    IN  GT_U8               devNum
)
{
    GT_U32  portGroupId; /* the port group Id - support multi-port-groups device */
    GT_U32  unitId;      /* loop iterator */
    PRV_CPSS_DXCH_ERRATA_UNIT_RANGES_STC  emptyUnit = EMPTY_UNIT_MAC; /* empty unit ranges */


    /* initialize Lion B0 bad addresses DB */
    /* loop on all active port groups */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        for (unitId = 0; unitId < UNITS_NUM_CNS; unitId++)
        {
            lionBadAddressRanges[portGroupId][unitId] = emptyUnit;
        }

        if (portGroupId > 0)
        {
            /* Unit 7 */
            lionBadAddressRanges[portGroupId][7].numOfRanges = (sizeof(lionUnit7RangesArray) /
                                                                  sizeof(lionUnit7RangesArray[0]));
            lionBadAddressRanges[portGroupId][7].rangesPtr = lionUnit7RangesArray;


            /* Unit 14 */
            lionBadAddressRanges[portGroupId][14].numOfRanges = (sizeof(lionUnit14RangesArray) /
                                                                   sizeof(lionUnit14RangesArray[0]));
            lionBadAddressRanges[portGroupId][14].rangesPtr = lionUnit14RangesArray;

            /* Unit 20 */
            lionBadAddressRanges[portGroupId][20].numOfRanges = (sizeof(lionUnit20RangesArray) /
                                                                   sizeof(lionUnit20RangesArray[0]));
            lionBadAddressRanges[portGroupId][20].rangesPtr = lionUnit20RangesArray;

            /* Unit 33 */
            lionBadAddressRanges[portGroupId][33].numOfRanges = (sizeof(lionUnit33RangesArray) /
                                                                   sizeof(lionUnit33RangesArray[0]));
            lionBadAddressRanges[portGroupId][33].rangesPtr = lionUnit33RangesArray;
        }

        if (portGroupId > 1)
        {
            /* Unit 35, only for port group 2 and 3 */
            lionBadAddressRanges[portGroupId][35].numOfRanges = (sizeof(lionUnit35RangesArray) /
                                                                   sizeof(lionUnit35RangesArray[0]));
            lionBadAddressRanges[portGroupId][35].rangesPtr = lionUnit35RangesArray;
        }

        PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT_RESTRICTED_ADDRESS_FILTERING_WA_E.unitRangesPtr[portGroupId]
                                                                = lionBadAddressRanges[portGroupId];
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

}


/*******************************************************************************
* xCat2ErrataAddrCheckWaDbInit
*
* DESCRIPTION:
*       Initialize Unit Address Ranges Data Base for Address Check WA.
*       The DB will be initialized according to the following Address ranges
*       list:
*           Where Unit Id is defined by bits 23-28.
*
*           1.      Unit 3
*               {0x01B40000, 0x01B7FFFC}
*               {0x01C00000, 0x01CBFFFC}
*               {0x01D00000, 0x01D3FFFC}
*               {0x01DC0000, 0x01DFFFFC}
*               {0x01E4000C, 0x01E7FFFC}
*
*           2.      Unit 7
*               {0x03801000, 0x038017FC}
*               {0x03901000, 0x03F017FC}
*
*           3.      Unit 12
*               {0x06000100, 0x063FFFFC}
*               {0x06500100, 0x067FFFFC}
*
*           4.      Unit 15
*               {0x07E40000, 0x07FFFFFC}
*
*           5.      Unit 19
*               {0x09805000, 0x0980FFFC}
*
*           6.      Unit 21
*               {0x0A807000, 0x0A80FBFC}
*
*           7.      Unit 23
*               {0x0B800800, 0x0B800FFC}
*               {0x0B840000, 0x0BF481FC}
*
*           8.      Unit 24
*               {0x0C000600, 0x0C0008FC}
*               {0x0C001800, 0x0C001FFC}
*               {0x0C006000, 0x0C006FFC}
*               {0x0C020000, 0x0C02106C}
*               {0x0C100600, 0x0C72106C}
*
*           9.      Unit 25
*               {0x0C801000, 0x0C8017FC}
*               {0x0C901000, 0x0C9017FC}
*
*           10.     Unit 26
*               {0x0D001000, 0x0D0017FC}
*               {0x0D101000, 0x0D1017FC}
*
*           11.     Unit 28
*               {0x0E000400, 0x0E0010FC}
*               {0x0E002000, 0x0E0027FC}
*               {0x0E100400, 0x0E7027FC}
*
*           12.     Unit 29
*               {0x0E800100, 0x0E8027FC}
*               {0x0E900100, 0x0EF027FC}
*
*           13.     Unit 30
*               {0x0F018000, 0x0F0187FC}
*               {0x0F118000, 0x0F7187FC}
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum          - The Pp's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       Note: The ranges should be in growing order.
*
*******************************************************************************/
static GT_VOID xCat2ErrataAddrCheckWaDbInit
(
    IN  GT_U8               devNum
)
{
    GT_U32      unitId;     /* loop iterator */
    PRV_CPSS_DXCH_ERRATA_UNIT_RANGES_STC  emptyUnit = EMPTY_UNIT_MAC; /* empty unit ranges */


    /* initialize XCAT DB */
    for (unitId = 0; unitId < UNITS_NUM_CNS; unitId++)
    {
        xCat2BadAddressRanges[unitId] = emptyUnit;
    }

    /* Unit 3 */
    xCat2BadAddressRanges[3].numOfRanges = (sizeof(xCat2Unit3RangesArray) / sizeof(xCat2Unit3RangesArray[0]));
    xCat2BadAddressRanges[3].rangesPtr = xCat2Unit3RangesArray;

    /* Unit 7 */
    xCat2BadAddressRanges[7].numOfRanges = (sizeof(xCat2Unit7RangesArray) / sizeof(xCat2Unit7RangesArray[0]));
    xCat2BadAddressRanges[7].rangesPtr = xCat2Unit7RangesArray;

    /* Unit 12 */
    xCat2BadAddressRanges[12].numOfRanges = (sizeof(xCat2Unit12RangesArray) / sizeof(xCat2Unit12RangesArray[0]));
    xCat2BadAddressRanges[12].rangesPtr = xCat2Unit12RangesArray;

    /* Unit 15 */
    xCat2BadAddressRanges[15].numOfRanges = (sizeof(xCat2Unit15RangesArray) / sizeof(xCat2Unit15RangesArray[0]));
    xCat2BadAddressRanges[15].rangesPtr = xCat2Unit15RangesArray;

    /* Unit 19 */
    xCat2BadAddressRanges[19].numOfRanges = (sizeof(xCat2Unit19RangesArray) / sizeof(xCat2Unit19RangesArray[0]));
    xCat2BadAddressRanges[19].rangesPtr = xCat2Unit19RangesArray;

    /* Unit 21 */
    xCat2BadAddressRanges[21].numOfRanges = (sizeof(xCat2Unit21RangesArray) / sizeof(xCat2Unit21RangesArray[0]));
    xCat2BadAddressRanges[21].rangesPtr = xCat2Unit21RangesArray;

    /* Unit 23 */
    xCat2BadAddressRanges[23].numOfRanges = (sizeof(xCat2Unit23RangesArray) / sizeof(xCat2Unit23RangesArray[0]));
    xCat2BadAddressRanges[23].rangesPtr = xCat2Unit23RangesArray;

    /* Unit 24 */
    xCat2BadAddressRanges[24].numOfRanges = (sizeof(xCat2Unit24RangesArray) / sizeof(xCat2Unit24RangesArray[0]));
    xCat2BadAddressRanges[24].rangesPtr = xCat2Unit24RangesArray;

    /* Unit 25 */
    xCat2BadAddressRanges[25].numOfRanges = (sizeof(xCat2Unit25RangesArray) / sizeof(xCat2Unit25RangesArray[0]));
    xCat2BadAddressRanges[25].rangesPtr = xCat2Unit25RangesArray;

    /* Unit 26 */
    xCat2BadAddressRanges[26].numOfRanges = (sizeof(xCat2Unit26RangesArray) / sizeof(xCat2Unit26RangesArray[0]));
    xCat2BadAddressRanges[26].rangesPtr = xCat2Unit26RangesArray;

    /* Unit 28 */
    xCat2BadAddressRanges[28].numOfRanges = (sizeof(xCat2Unit28RangesArray) / sizeof(xCat2Unit28RangesArray[0]));
    xCat2BadAddressRanges[28].rangesPtr = xCat2Unit28RangesArray;

    /* Unit 29 */
    xCat2BadAddressRanges[29].numOfRanges = (sizeof(xCat2Unit29RangesArray) / sizeof(xCat2Unit29RangesArray[0]));
    xCat2BadAddressRanges[29].rangesPtr = xCat2Unit29RangesArray;

    /* Unit 30 */
    xCat2BadAddressRanges[30].numOfRanges = (sizeof(xCat2Unit30RangesArray) / sizeof(xCat2Unit30RangesArray[0]));
    xCat2BadAddressRanges[30].rangesPtr = xCat2Unit30RangesArray;

    PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
        info_PRV_CPSS_DXCH_XCAT_RESTRICTED_ADDRESS_FILTERING_WA_E.unitRangesPtr[0]
                                                            = xCat2BadAddressRanges;
}





/*******************************************************************************
* hwPpStartInitSet
*
* DESCRIPTION:
*
*       Trigger the device to do 'start init' then wait for the device to finish
*       the operation . the function will wait up to 1 second for operation to
*       end
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
* INPUTS:
*       devNum          - The Pp's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_TIMEOUT  - the device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpStartInitSet
(
    IN  GT_U8               devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC  *regsAddrPtr;
    GT_U32              tmpData = 3;
    GT_U32              timeout;        /* Timeout for waiting for the      */
                                        /* startinit operation to be done.  */
    GT_STATUS           rc;             /* Return code */
    GT_U32              portGroupId=0;  /* the port group Id - support multi-port-groups device */

    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* Check if start-Init was already done .
       check if register 0x5C bits [14:13] is 0x3 --> only in port group 0 */

    rc = prvCpssDrvHwPpGetRegField(devNum,
                                   PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl,
                                   13, 2, &tmpData);
    if(rc != GT_OK)
        return rc;

    if(tmpData == 0x3)
    {
        return GT_OK;
    }

    /* set 1 second timeout */
    timeout = 100;
    tmpData = 0;

    while(tmpData != 1)
    {
        if(PRV_CPSS_PP_MAC(devNum)->ppHAState == CPSS_SYS_HA_MODE_STANDBY_E)
        {
            break;
        }

        if(timeout == 0)
        {
            /* time over, the device is not finished init.
               This means HW problem... */
            return GT_TIMEOUT;
        }

       /* wait till : EEPROM initialization is done and Internal memory initialization is not done.
          Wait for register 0x5C bits [14:13] = 0x1 --> only in port group 0 */
        rc = prvCpssDrvHwPpGetRegField(devNum,
                                   PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl,
                                   13, 2, &tmpData);
        if(rc != GT_OK)
            return rc;


#ifdef _FreeBSD
        /* under FreeBSD required longer timeout untill
         * ASIC reset finished
         */
        cpssOsTimerWkAfter(1000);
#else
        cpssOsTimerWkAfter(10);
#endif
        timeout--;
    }

    /* go over all group ports and do device init sequence */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        /****************/
        /* Start Init   */
        /****************/

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl,
                                                31, 1, 0);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    /* set 1 second timeout */
    timeout = 100;
    tmpData = 0;
    while(tmpData != 3)
    {
        if(PRV_CPSS_PP_MAC(devNum)->ppHAState == CPSS_SYS_HA_MODE_STANDBY_E)
        {
            break;
        }

        if(timeout == 0)
        {
            /* time over, the device is not finished init.
               This means HW problem... */
            return GT_TIMEOUT;
        }

        /* wait till : Initialize is fully functional.
           Wait for register 0x5C bits [14:13] = 0x3 --> only in port group 0 */
        rc = prvCpssDrvHwPpGetRegField(devNum,
                                   PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl,
                                   13, 2, &tmpData);
        if(rc != GT_OK)
            return rc;

#ifdef _FreeBSD
        /* under FreeBSD required longer timeout untill
         * ASIC reset finished
         */
        cpssOsTimerWkAfter(1000);
#else
        cpssOsTimerWkAfter(10);
#endif
        timeout--;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChHwPpPhase1Init
*
* DESCRIPTION:
*       This function performs basic hardware configurations on the given PP, in
*       Hw registration phase.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ppPhase1ParamsPtr  - Packet processor hardware specific parameters.
*
* OUTPUTS:
*       deviceTypePtr - The Pp's device Id.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_ALREADY_EXIST         - the devNum already in use
*       GT_NOT_SUPPORTED         - the device not supported by CPSS
*       GT_OUT_OF_CPU_MEM        - failed to allocate CPU memory,
*       GT_BAD_VALUE             - the driver found unknown device type
*       GT_NOT_IMPLEMENTED       - the CPSS was not compiled properly
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpPhase1Init
(
    IN      CPSS_DXCH_PP_PHASE1_INIT_INFO_STC   *ppPhase1ParamsPtr,
    OUT     CPSS_PP_DEVICE_TYPE                 *deviceTypePtr
)
{
    GT_STATUS rc ;
    GT_U8     devNum;
    GT_BOOL   multiPortGroupDevice = GT_FALSE;/*indication that the device is 'multi-port-groups' device */
    GT_U32    portGroupId;/*iterator for port group Id*/
    PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_ENT prePhase1InitMode;
    CPSS_NULL_PTR_CHECK_MAC(ppPhase1ParamsPtr);
    CPSS_NULL_PTR_CHECK_MAC(deviceTypePtr);

    rc = prvCpssPrePhase1PpInitModeGet(&prePhase1InitMode);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(ppPhase1ParamsPtr->busBaseAddr == CPSS_PARAM_NOT_USED_CNS)
    {
        /* this is indication that the device should be 'multi-port-groups' device */
        /* so we need to use the parameters of :
            ppPhase1ParamsPtr->numOfPortGroups
            ppPhase1ParamsPtr->multiPortGroupsInfoPtr
        */

        if(ppPhase1ParamsPtr->numOfPortGroups == 0 ||
           ppPhase1ParamsPtr->numOfPortGroups > CPSS_MAX_PORT_GROUPS_CNS)
        {
            return GT_BAD_PARAM;
        }

        CPSS_NULL_PTR_CHECK_MAC(ppPhase1ParamsPtr->multiPortGroupsInfoPtr);

        multiPortGroupDevice = GT_TRUE;
    }

    devNum = ppPhase1ParamsPtr->devNum;

    /* check input parameters */
    if(devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        /* bad device number -- out of range */
        return GT_BAD_PARAM;
    }
    else if(PRV_CPSS_PP_MAC(devNum))
    {
        /* the device occupied by some device ... */
        return GT_ALREADY_EXIST;
    }

    /* initialize the cpssDriver , build the DB needed for the device */
    rc = hwPpPhase1Part1(ppPhase1ParamsPtr,multiPortGroupDevice,deviceTypePtr);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    /* Trigger the device for the start-init operation. */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* start init sequence for Lion */
        rc = hwPpStartInitSet(devNum);
        if(rc != GT_OK)
        {
            return rc;
        }
     }

    /* initialize the board specific parameters and registers the packet
    processor for pre-discovery phase access and diagnostics */
    rc = hwPpPhase1Part2(devNum);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    if(multiPortGroupDevice == GT_FALSE)
    {
        /* Initialize the interrupts mechanism for a given device. */
        rc = prvCpssDrvInterruptsInit(devNum,CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS,
                                 ppPhase1ParamsPtr->intVecNum,
                             ppPhase1ParamsPtr->intMask);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }
    }
    else
    {
        /* the per port group information taken from ppPhase1ParamsPtr->multiPortGroupsInfoPtr[portGroupId] */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            /* Initialize the interrupts mechanism for a given device. */
            rc = prvCpssDrvInterruptsInit(devNum,portGroupId,
                                     ppPhase1ParamsPtr->multiPortGroupsInfoPtr[portGroupId].intVecNum,
                                     ppPhase1ParamsPtr->multiPortGroupsInfoPtr[portGroupId].intMask);
            if(rc != GT_OK)
            {
                goto exit_cleanly_lbl;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    }

    if (prePhase1InitMode != PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_NO_PP_INIT_E)
    {
        /* Set Errata issues and registers settings.*/
        rc = hwPpPhase1Part3(devNum, ppPhase1ParamsPtr);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }
    }
    /* wait for the device to be ready for traffic */
    rc = hwPpPhase1Part4(devNum);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    if (prePhase1InitMode != PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_NO_PP_INIT_E)
    {
        /* do enhanced initialization of SW and HW parameters , if needed */
        if(prvCpssDxChPpConfigEnhancedInitFuncPtr)
        {
            rc = prvCpssDxChPpConfigEnhancedInitFuncPtr(devNum);
            if(rc != GT_OK)
            {
                goto exit_cleanly_lbl;
            }
        }
    }
    /* init Cheetah tables database .
       called after : prvCpssDxChPpConfigEnhancedInitFuncPtr
       but should be called even if prvCpssDxChPpConfigEnhancedInitFuncPtr
       is NULL
    */
    rc = prvCpssDxChTablesAccessInit(devNum);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    /* set RM (Register Miss-configuration) after tables initialization */
    if (prePhase1InitMode != PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_NO_PP_INIT_E)
    {
        rc = hwPpPhase1RegisterMisconfigurationAfterTablesInit(devNum);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }
    }
    /* set final phase 1 register settings :
       called after : prvCpssDxChPpConfigEnhancedInitFuncPtr
       but should be called even if prvCpssDxChPpConfigEnhancedInitFuncPtr
       is NULL
       */
    if (prePhase1InitMode != PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_NO_PP_INIT_E)
    {
        rc = hwPpPhase1Part5(devNum);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }

        /* set final phase 1 register settings for devices with internal CPU (xCat etc.) where this CPI is disabled    */
        rc = hwPpPhase1Part6(devNum, ppPhase1ParamsPtr);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }
    }
exit_cleanly_lbl:
    if(rc != GT_OK && PRV_CPSS_DXCH_PP_MAC(devNum))
    {
        /* restore all the DB state to start point */

        /* free the registers addresses memory */
        switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            case CPSS_PP_FAMILY_CHEETAH_E:
                prvCpssDxChHwRegAddrRemove(devNum);
                break;
            case CPSS_PP_FAMILY_CHEETAH2_E:
                prvCpssDxCh2HwRegAddrRemove(devNum);
                break;
            case CPSS_PP_FAMILY_CHEETAH3_E:
                prvCpssDxCh3HwRegAddrRemove(devNum);
                break;
            case CPSS_PP_FAMILY_DXCH_XCAT_E:
                prvCpssDxChXcatHwRegAddrRemove(devNum);
                break;
            case CPSS_PP_FAMILY_DXCH_XCAT2_E:
                prvCpssDxChXcat2HwRegAddrRemove(devNum);
                break;
            case CPSS_PP_FAMILY_DXCH_LION_E:
                prvCpssDxChLionHwRegAddrRemove(devNum);
                break;
            default:
                return GT_DEVICE_FAMILY_NOT_SUPPORTED_CNS;
        }

        /* free the memory of this device */
        cpssOsFree(PRV_CPSS_PP_MAC(devNum));

        /* set this index as "not occupied" */
        prvCpssPpConfig[devNum] = NULL;
        dxCh_prvCpssPpConfig[devNum] = prvCpssPpConfig[devNum];
    }

    return rc;
}

/*******************************************************************************
* hwPpPhase1Part4
*
* DESCRIPTION:
*       Part 4 of "phase 1"
*       wait for the device , until it is ready for traffic,
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - The Pp's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpPhase1Part4
(
    IN  GT_U8               devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC  *    regsAddrPtr;
    GT_U32              tmpData;
    GT_U32              timeout;        /* Timeout for waiting for the      */
                                        /* startinit operation to be done.  */
    GT_U32              portGroupId;/*port group Id*/

    regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* read Init State field */
    if(prvCpssDrvHwPpGetRegField(devNum, regsAddrPtr->globalRegs.globalControl,
                              17, 2, &tmpData) != GT_OK)
         return GT_HW_ERROR;

    /*need to support multi port groups device,to loop on all port groups*/
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        /* set 1 second timeout */
        timeout = 100;
        while(tmpData != 3)
        {
            if(timeout == 0)
            {
                /* time over, the device is not finished init.
                   This means HW problem... */
                return GT_FAIL;
            }

            /* wait till : Initialize is fully functional. */
            if(prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regsAddrPtr->globalRegs.globalControl,
                                      17, 2, &tmpData) != GT_OK)
                 return GT_HW_ERROR;

            cpssOsTimerWkAfter(10);
            timeout--;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)


    return GT_OK;
}

/*******************************************************************************
* hwPpPhase1Part5
*
* DESCRIPTION:
*       Part 5 of "phase 1"
*       set part 5 phase 1 register settings -
*       called after : prvCpssDxChPpConfigEnhancedInitFuncPtr
*      but should be called even if prvCpssDxChPpConfigEnhancedInitFuncPtr
*      is NULL
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - The Pp's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpPhase1Part5
(
    IN  GT_U8               devNum
)
{
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC  *    regsAddrPtr;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;
    GT_U32    i;
    GT_U32    hwValue; /* hardware value */
    GT_U32    txDescNum;    /* number of transmit descriptors */
    GT_STATUS rc; /* returned status */
    GT_U32 fieldLength; /* The number of bits to be written to register */
    GT_BOOL   didHwReset; /* flag indicating whether HW reset was done */

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    /* FDB size configuration for XCAT A1 and above */
    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* FDB size configuration for Lion and above */
        if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum)
           && (PRV_CPSS_PP_MAC(devNum)->devFamily
               != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            switch(fineTuningPtr->tableSize.fdb)
            {
                case _16KB:
                    hwValue = 1;
                    break;
                case _32KB:
                    hwValue = 0;
                    break;
                default:
                    return GT_BAD_PARAM;
            }

            fieldLength = 1;
        }
        else
        {
            /* FDB size configuration for XCAT A1 and XCAT2 */
            switch(fineTuningPtr->tableSize.fdb)
            {
                case _4KB:
                    hwValue = 3;
                    break;
                case _8KB:
                    hwValue = 2;
                    break;
                case _16KB:
                    hwValue = 1;
                    break;
                case _32KB:
                    hwValue = 0;
                    break;
                default:
                    return GT_BAD_PARAM;
            }
            fieldLength = 2;

        }
        /* set The size of the FDB Table */
        rc = prvCpssDrvHwPpSetRegField(devNum,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg,
                                       24,
                                       fieldLength,
                                       hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }


        /* TR101 feature configuration */
        rc = prvCpssDrvHwPpSetRegField(devNum,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig,
                                       11,
                                       1,
               BOOL2BIT_MAC(fineTuningPtr->featureInfo.tr101Supported));
        if (rc != GT_OK)
        {
            return rc;
        }


        for (i = 0; i < CPSS_DXCH_POLICER_STAGE_EGRESS_E; i++)
        {
            /* not for egress policer */

            if(fineTuningPtr->featureInfo.iplrSecondStageSupported == GT_FALSE)
            {
                /* Policer memory control */
                rc = prvCpssDrvHwPpSetRegField(
                    devNum,
                    PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.PLR[i].policerControlReg,
                    11, 2, 1);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }

        /* Resources - buffers & descriptors - size configuration for xCat devices */
        if( CPSS_PP_FAMILY_DXCH_XCAT_E == PRV_CPSS_PP_MAC(devNum)->devFamily ||
            CPSS_PP_FAMILY_DXCH_XCAT2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);
            txDescNum = fineTuningPtr->tableSize.transmitDescr;

            /* MaxBufLimit */
            rc = prvCpssDrvHwPpSetRegField(devNum,
                                           regsAddrPtr->bufferMng.bufMngGlobalLimit,
                                           22, 9,
                                           (txDescNum >> 5 ) - ((txDescNum == _2K) ? 1 : 2 ));
            if (rc != GT_OK)
            {
                return rc;
            }

            /* GlobalXoff */
            rc = cpssDxChGlobalXoffLimitSet(devNum,(txDescNum - 8)/2);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* GlobalXon */
            rc = cpssDxChGlobalXonLimitSet(devNum,(txDescNum - 8)/2);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* GPortsMaxBufLimit */
            rc = cpssDxChPortGroupRxBufLimitSet(devNum, CPSS_DXCH_PORT_GROUP_GIGA_E,
                                                txDescNum - 16);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* GPortsXoff */
            rc = cpssDxChPortGroupXoffLimitSet(devNum, CPSS_DXCH_PORT_GROUP_GIGA_E,
                                               txDescNum - 8);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* GPortsXon */
            rc = cpssDxChPortGroupXonLimitSet(devNum, CPSS_DXCH_PORT_GROUP_GIGA_E,
                                              txDescNum - 8);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* HGSPortsMaxBufLimit */
            rc = cpssDxChPortGroupRxBufLimitSet(devNum, CPSS_DXCH_PORT_GROUP_HGS_E,
                                                txDescNum - 16);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* HGSPortsXoff */
            rc = cpssDxChPortGroupXoffLimitSet(devNum, CPSS_DXCH_PORT_GROUP_HGS_E,
                                               txDescNum - 8);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* HGSPortsXon */
            rc = cpssDxChPortGroupXonLimitSet(devNum, CPSS_DXCH_PORT_GROUP_HGS_E,
                                              txDescNum - 8);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* MaxSharedBuffersLimit */
            rc = cpssDxChPortTxBuffersSharingMaxLimitSet(devNum, txDescNum - 1);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = prvCpssPpConfigDevDbHwResetGet(devNum, &didHwReset);
            if (rc != GT_OK)
            {
                return rc;
            }

            if( GT_TRUE == didHwReset )
            {
                /* FllWrEn set:Write to fll memory, fll_head_ptr and fll_tail_ptr enabled */
                rc = prvCpssDrvHwPpSetRegField(devNum,
                                               regsAddrPtr->bufferMng.bufMngMetalFix,
                                               0, 1, 1);
                if (rc != GT_OK)
                {
                    return rc;
                }

                /* LlTailPtr */
                rc = prvCpssDrvHwPpSetRegField(devNum, 0x03000200, 0, 14, txDescNum - 1);
                if (rc != GT_OK)
                {
                    return rc;
                }

                /* FllWrEn unset */
                rc = prvCpssDrvHwPpSetRegField(devNum,
                                               regsAddrPtr->bufferMng.bufMngMetalFix,
                                               0, 1, 0);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }

            /* TotalBuffersLimit */
            rc = prvCpssDrvHwPpSetRegField(devNum,
                                           regsAddrPtr->egrTxQConf.totalBufferLimitConfig,
                                           0, 18, txDescNum - 1);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* TotalDescLimit */
            rc = prvCpssDrvHwPpSetRegField(devNum,
                                           regsAddrPtr->egrTxQConf.txQueueConfig,
                                           16, 16,
                  txDescNum - (fineTuningPtr->tableSize.txQueuesNum)*PRV_DXCH_XCAT_HW_NUM_OF_PORTS_CNS - 10);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* LinkListSize */
            rc = prvCpssDrvHwPpSetRegField(devNum, 0x018000D0, 0, 2,
                                           (txDescNum == _2K) ? 1 : ((txDescNum == _4K) ? 2 : 0) );
            if (rc != GT_OK)
            {
                return rc;
            }

            /* SharePoolBufLimit, SharePoolDescLimit */
            rc = cpssDxChPortTxSharedGlobalResourceLimitsSet(devNum,
                                                             txDescNum - 4,
                                                             txDescNum - 4);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    else if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
    {
        /* cheetah3 and XCAT A0 */
        regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);
        /* set The size of the FDB Table */
        rc = prvCpssDrvHwPpSetRegField(devNum,
                                       regsAddrPtr->bridgeRegs.macControlReg,
                                       24, 1,
                                       (fineTuningPtr->tableSize.fdb == _16K) ?
                                       1 : 0);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;

}

/*******************************************************************************
* hwPpPhase1Part6
*
* DESCRIPTION:
*       Part 6 of "phase 1"
*       Set final phase 1 register settings for devices with internal CPU (xCat etc.) where this CPU is disabled
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum            - The Pp's device number.
*       ppPhase1ParamsPtr - (pointer to) phase1 parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpPhase1Part6
(
    IN  GT_U8               devNum,
    IN  CPSS_DXCH_PP_PHASE1_INIT_INFO_STC   *ppPhase1ParamsPtr
)
{
    GT_STATUS rc; /* returned status */
    GT_BOOL pexWindUpd = GT_FALSE;
    /* GT_U32 baseAddrVal = 0, sizeVal = 0xFFFF0000, baseAddrEn = 0x3e; */
    GT_U32 baseAddrVal = 0, sizeVal = 0x7FFF0000, baseAddrEn = 0x3e;   /* SDRM = 2G */

	osPrintfDbg("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);/*xcat debug*/	
    osPrintfDbg("******** Here we init the BAR in SW, and then SW can R/W SDRAM !!!\n");

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {/* For xCat: if internal CPU is disabled (bit 17 of device type is 1) or
        external CPU is connected - set address decoding register */
        pexWindUpd = ((PRV_CPSS_PP_MAC(devNum)->devType) & 0x20000)
                        || (ppPhase1ParamsPtr->isExternalCpuConnected == GT_TRUE);
        baseAddrVal = 0x0000080F; /* set window’s target to PEX [0xF] with memory attribute [0x08] */
    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {/* for xcat2: in xcat2 BTB CPU's of both devices enabled, so chose
        hard-coded device #1 */
        pexWindUpd = (devNum == 1);
        baseAddrVal = 0xe804;/* attribute 0xe8, target 0x4 */
    }

    if(pexWindUpd)
    {
        /* Base_Address_0 register 0x30C */
        rc = prvCpssDrvHwPpWriteRegister(devNum, 0x30C, baseAddrVal);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Size_0 register - set to max windows size */
        rc = prvCpssDrvHwPpWriteRegister(devNum, 0x310, sizeVal);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Base_Address_Enable register - window0 enable */
        rc = prvCpssDrvHwPpWriteRegister(devNum, 0x34C, baseAddrEn);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;

}

/*******************************************************************************
* hwPpPhase2
*
* DESCRIPTION:
*       Set new devNumber to the PP in Hw and in PP,
*       update the cpssDriver
*       initialize the AU descriptors
*       initialize the FU descriptors
*       initialize the network interface
*
*       Assume that we are under "locked interrupts"
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       oldDevNum          - The PP's "old" device number.
*       ppPhase2ParamsPtr  - Phase2 initialization parameters..
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       function called when PP's interrupts are disabled !
*
*******************************************************************************/
static GT_STATUS hwPpPhase2
(
    IN  GT_U8                                oldDevNum,
    IN  CPSS_DXCH_PP_PHASE2_INIT_INFO_STC    *ppPhase2ParamsPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_MODULE_CONFIG_STC *moduleCfgPtr;/* module config of the device */
    GT_U8   newDevNum = ppPhase2ParamsPtr->newDevNum;/* the new devNum*/
    PRV_CPSS_GEN_PP_CONFIG_STC *devPtr;/* pointer to common device info */

    /* update the driver about this action */
    /* let the cpss driver know that the device changed number */
    rc = prvCpssDrvHwPpPhase2Init(oldDevNum,newDevNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* update the DB of the CPSS */

    if(oldDevNum != newDevNum)
    {
        /* set the DB of "old" device number to the "new" device number */
        prvCpssPpConfig[newDevNum] = PRV_CPSS_PP_MAC(oldDevNum);
        /* invalidate the "old" place */
        prvCpssPpConfig[oldDevNum] = NULL;

        dxCh_prvCpssPpConfig[newDevNum] = prvCpssPpConfig[newDevNum];
        dxCh_prvCpssPpConfig[oldDevNum] = prvCpssPpConfig[oldDevNum];

        /* swap the device number at the special DB */
        rc = prvCpssPpConfigDevDbRenumber(oldDevNum,newDevNum);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /*************************************************/
    /* from now on we only use the new device number */
    /*************************************************/
    devPtr = PRV_CPSS_PP_MAC(newDevNum);

    /* update the devNum of the device */
    devPtr->devNum = newDevNum;

    if(devPtr->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* ignore this parameter for cheetah devices , because no HW support anyway */
        ppPhase2ParamsPtr->fuqUseSeparate = GT_FALSE;
    }

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(newDevNum))
    {
        /* check requested port group is active one */
        if( (ppPhase2ParamsPtr->netifSdmaPortGroupId <
             PRV_CPSS_PP_MAC(newDevNum)->portGroupsInfo.firstActivePortGroup) ||
            (ppPhase2ParamsPtr->netifSdmaPortGroupId >
             PRV_CPSS_PP_MAC(newDevNum)->portGroupsInfo.lastActivePortGroup)  ||
            (0 == (PRV_CPSS_PP_MAC(newDevNum)->portGroupsInfo.activePortGroupsBmp &
                                     (1<<(ppPhase2ParamsPtr->netifSdmaPortGroupId)))) )
        {
            return GT_FAIL;
        }
        devPtr->netifSdmaPortGroupId = ppPhase2ParamsPtr->netifSdmaPortGroupId;
    }
    else
    {
        devPtr->netifSdmaPortGroupId =
            PRV_CPSS_FIRST_ACTIVE_PORT_GROUP_ID_MAC(newDevNum);
    }

    /* update the special DB -- must be called after the device numbers swapped
       in prvCpssPpConfig and after prvCpssPpConfigDevDbRenumber was called */
    rc = prvCpssPpConfigDevDbInfoSet(newDevNum,
                                    &ppPhase2ParamsPtr->netIfCfg,
                                    &ppPhase2ParamsPtr->auqCfg,
                                    ppPhase2ParamsPtr->fuqUseSeparate,
                                    &ppPhase2ParamsPtr->fuqCfg);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* update the HW about the new devNum */
    /* Device number  */

    rc = cpssDxChCfgHwDevNumSet(newDevNum,newDevNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(ppPhase2ParamsPtr->useSecondaryAuq == GT_FALSE)
    {
        /* the WA not requested by the application , even if may be required */
        PRV_CPSS_DXCH_ERRATA_CLEAR_MAC(newDevNum,
            PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E);
    }

    /* Now set the rest of the parameters into the  */
    /* module configuration struct in ppConfig.     */
    moduleCfgPtr = PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(newDevNum);

    moduleCfgPtr->fuqUseSeparate = ppPhase2ParamsPtr->fuqUseSeparate;
    /* AUQ config               */
    moduleCfgPtr->auCfg.auDescBlock     = ppPhase2ParamsPtr->auqCfg.auDescBlock;
    moduleCfgPtr->auCfg.auDescBlockSize = ppPhase2ParamsPtr->auqCfg.auDescBlockSize;

    /* FUQ config               */
    moduleCfgPtr->fuCfg.fuDescBlock     = ppPhase2ParamsPtr->fuqCfg.auDescBlock;
    moduleCfgPtr->fuCfg.fuDescBlockSize = ppPhase2ParamsPtr->fuqCfg.auDescBlockSize;


    /* Network interface config */
    cpssOsMemCpy(&(moduleCfgPtr->netIfCfg.rxBufInfo),
             &(ppPhase2ParamsPtr->netIfCfg.rxBufInfo),sizeof(CPSS_RX_BUF_INFO_STC));

    moduleCfgPtr->netIfCfg.rxDescBlock     = ppPhase2ParamsPtr->netIfCfg.rxDescBlock;
    moduleCfgPtr->netIfCfg.rxDescBlockSize =
        ppPhase2ParamsPtr->netIfCfg.rxDescBlockSize;
    moduleCfgPtr->netIfCfg.txDescBlock     = ppPhase2ParamsPtr->netIfCfg.txDescBlock;
    moduleCfgPtr->netIfCfg.txDescBlockSize =
        ppPhase2ParamsPtr->netIfCfg.txDescBlockSize;

    /* configure Policer memories sizes */
    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(newDevNum))
    {
        /* xCat */

        if (PRV_CPSS_DXCH_PP_MAC(newDevNum)->
            fineTuning.featureInfo.iplrSecondStageSupported)
        {
            /* There are two Policer Ingress stages, 256 to lower and the rest to
               upper memory */
            PRV_CPSS_DXCH_PP_MAC(newDevNum)->policer.memSize[0] =
                (PRV_CPSS_DXCH_PP_MAC(newDevNum)->fineTuning.tableSize.policersNum - 256);
            PRV_CPSS_DXCH_PP_MAC(newDevNum)->policer.memSize[1] = 256;

            /* fix xCat2 HW default for memory configuration to be
                PLR0_28_PLR1_172_84; Policer 0 uses one memory 28
                Policer 1 uses two memories 172+84 = 256  */
            if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(newDevNum,
                     PRV_CPSS_DXCH_XCAT2_RM_POLICER_MEM_CONTROL_WA_E))
            {
                rc = prvCpssDrvHwPpSetRegField(newDevNum,
                  PRV_CPSS_DXCH_PP_MAC(newDevNum)->regsAddr.PLR[0].policerControl2Reg,
                  5, 3, 5);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        else
        {
            /* only one Policer Ingress stage */
            PRV_CPSS_DXCH_PP_MAC(newDevNum)->policer.memSize[0] =
                PRV_CPSS_DXCH_PP_MAC(newDevNum)->fineTuning.tableSize.policersNum;
            PRV_CPSS_DXCH_PP_MAC(newDevNum)->policer.memSize[1] = 0;
        }

        /* Policer Egress memory size */
        PRV_CPSS_DXCH_PP_MAC(newDevNum)->policer.memSize[2] =
            PRV_CPSS_DXCH_PP_MAC(newDevNum)->fineTuning.tableSize.egressPolicersNum;
    }
    else
    {
        /* DxCh1-DxCh3 */

        PRV_CPSS_DXCH_PP_MAC(newDevNum)->policer.memSize[0] =
            PRV_CPSS_DXCH_PP_MAC(newDevNum)->fineTuning.tableSize.policersNum;
        /* No Egress nor Ingress #1 Policer memories */
        PRV_CPSS_DXCH_PP_MAC(newDevNum)->policer.memSize[1] = 0;
        PRV_CPSS_DXCH_PP_MAC(newDevNum)->policer.memSize[2] = 0;
    }

    PRV_CPSS_DXCH_PP_MAC(newDevNum)->policer.meteringCalcMethod =
        CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_ONLY_E;

    rc = prvCpssDrvInterruptsAfterPhase2(oldDevNum,newDevNum);
    if(rc != GT_OK)
    {
        return rc;
    }

     return GT_OK;
}

/*******************************************************************************
* cpssDxChHwPpPhase2Init
*
* DESCRIPTION:
*       This function performs basic hardware configurations on the given PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - The PP's device number.
*       ppPhase2ParamsPtr  - Phase2 initialization parameters..
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpPhase2Init
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PP_PHASE2_INIT_INFO_STC       *ppPhase2ParamsPtr
)
{

    GT_STATUS  rc;
    GT_32      intKey;/* Interrupt key.                   */
    GT_U8   oldDevNum;/* the "old" device number -- as was given in "phase 1" */

    oldDevNum =  devNum;

    CPSS_NULL_PTR_CHECK_MAC(ppPhase2ParamsPtr);
    PRV_CPSS_DXCH_DEV_CHECK_MAC(oldDevNum);

    if(ppPhase2ParamsPtr->newDevNum != oldDevNum)
    {
        /* we need to check the new device number */

        /* check input parameters */
        if(ppPhase2ParamsPtr->newDevNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
        {
            /* bad device number -- out of range */
            return GT_BAD_PARAM;
        }
        else if(PRV_CPSS_PP_MAC(ppPhase2ParamsPtr->newDevNum))
        {
            /* the device occupied by some device ... */
            return GT_ALREADY_EXIST;
        }
    }

    /* Lock the interrupts, this phase changes the interrupts nodes       */
    /* pool data. And relocates the port group / driverPpDevs elements.   */
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E,&intKey);

    /*
    *       Set new devNumber to the PP in Hw and in PP,
    *       update the cpssDriver
    *       initialize the AU descriptors
    *       initialize the network interface
    */
    rc = hwPpPhase2(oldDevNum,ppPhase2ParamsPtr);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

exit_cleanly_lbl:
    /* UnLock the interrupts */
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E,&intKey);

    return rc;

}

/*******************************************************************************
* hwPpPhyRegisterWrite
*
* DESCRIPTION:
*       Performs smi Phy write operation including poling on Buzy bit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number.
*       addr - the smi phy configuration register.
*       data - data to write including the phy address.
*       mask - perform read modify write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_TIMEOUT - on timed out retries
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpPhyRegisterWrite
(
    IN GT_U8  devNum,
    IN GT_U32 addr,
    IN GT_U32 mask,
    IN GT_U32 data
)
{
    GT_U32      smiRegConfig;     /* holds the content of the
                                  configuration register*/
    GT_U32      busyBitNumber;    /* holds the smi busy valid bit number*/
    GT_BOOL     isXgPort;         /* GT_TRUE - XG port PHY is configured
                                     GT_FALSE - GE port PHY is configured*/
#ifndef _WIN32
    GT_U32      comp;
    GT_U32      j;                /* Time out pooling index*/
#endif
    /* Control register or Phy register - device with Single XG port*/
    static GT_BOOL setPhyRegBeforeControlReg = GT_FALSE;

    /* Get the Xg smi control register address*/
    if ((addr == PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeControl) ||
        (addr == PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeAddr))
    {
        isXgPort = GT_TRUE;
    }
    else
    {
        isXgPort = GT_FALSE;
    }

    /* Zero SMi control word*/
    smiRegConfig = 0;

    /* Set the busy bit, opcode for xg phy*/
    if (isXgPort == GT_TRUE)
    {
        busyBitNumber = 30;
    }
    else
    {
        busyBitNumber = 28;
    }

#ifndef _WIN32
    /* Perform write operation - check that SMI of XSMI Master is finished
       previous transaction by polling the busy bit */
    if (((isXgPort == GT_TRUE) && (addr == PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeAddr))||
        (isXgPort == GT_FALSE))
    {
        j = 0;
        do
        {
            if (prvCpssDrvHwPpReadRegister(devNum, addr, &smiRegConfig) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            comp = (smiRegConfig >> busyBitNumber) & 0x1;

            if( (j++) == HW_INIT_SMI_POOLING_TIMEOUT_CNS )
                return GT_TIMEOUT;

        }while (comp);
    }
#endif


    /* Set the data according to the mask*/
    mask &= 0xFFFF;                     /* Zero 16 msbis*/
    smiRegConfig &= (~mask);            /* Turn the field off.   */
    smiRegConfig |= (data & mask);

    if (isXgPort == GT_TRUE)
    {
        if (addr == PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeAddr)
        {
            if(setPhyRegBeforeControlReg == GT_TRUE)
            {
                /*OpCode 0x5 - issue 'Address' SMI Frame the issue 'Write' SMI frame*/
                U32_SET_FIELD_MAC(smiRegConfig, 26, 3, 5);
                setPhyRegBeforeControlReg = GT_FALSE;
            }
            else
            {
                /*OpCode 0x1 - issue 'WRITE' SMI frame*/
                U32_SET_FIELD_MAC(smiRegConfig, 26, 3, 1);
            }

            /* Set the address from the input*/
            U32_SET_FIELD_MAC(smiRegConfig, 16, 10, (data >> 16));
        }
        else
        {
            setPhyRegBeforeControlReg = GT_TRUE;
        }
    }
    else
    {
        U32_SET_FIELD_MAC(smiRegConfig, 26, 1, 0);      /*OpCode 0x0 - issue 'WRITE' SMI frame*/
        /* Set the address from the input*/
        U32_SET_FIELD_MAC(smiRegConfig, 16, 10, (data >> 16));
    }

    /* Perform the phy write operation*/
    if (prvCpssDrvHwPpWriteRegister(devNum, addr, smiRegConfig) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpStartInit
*
* DESCRIPTION:
*       This phase performs the start-init operation on a given device, and sets
*       it's registers according to the registers value list passed by user.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - The device number to which the start-init operation
*                         should be done.
*       initDataListPtr - (pointer to) List of registers values to be set to the device.
*       initDataListLen - Number of elements in ppRegConfigList
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       This function should be invoked after cpssExMxHwPpPhase1Init() and
*       before cpssDxChHwPpPhase2Init().
*       1.  Before the device is enabled, all ports will be set to disable state
*           to make sure that no packets are forwarded before the system is
*           fully initialized.
*
*******************************************************************************/
static GT_STATUS hwPpStartInit
(
    IN  GT_U8                       devNum,
    IN  CPSS_REG_VALUE_INFO_STC     *initDataListPtr,
    IN  GT_U32                      initDataListLen
)
{
    GT_STATUS   rc = GT_OK;         /* Function's return value.                 */
    GT_BOOL     setMatch;       /* Indicates if the device's set matches    */
                                /* the next section.                        */
    GT_BOOL     deviceFound;    /* Indicates if the device's set was found  */
                                /* in the data-list.                        */
    GT_U32      i;              /* Loops index.                             */
    GT_U32      j;              /* Loops index.                             */
    GT_U32      cnt;            /* Repeat counter index.                    */
    CPSS_REG_VALUE_INFO_STC  *regValuePtr; /* pointer to the current reg value*/
    PRV_CPSS_GEN_PP_CONFIG_STC *devPtr = PRV_CPSS_PP_MAC(devNum);/* pointer to common device info */

    deviceFound = GT_FALSE;

    /* Search for the corresponding set to configure.   */
    /* Currently the set number is the device's         */
    /* revision number.                                 */
    /* SHOULD BE FIXED TO CONSIDER THE DEVICE'S SET #   */
    i = 0;
    while(i < initDataListLen)
    {
        /* For now, just find the device's revision number  */
        setMatch = GT_FALSE;
        while((initDataListPtr[i].addrType != CPSS_REG_LIST_DELIMITER_CNS) &&
              (i < initDataListLen))
        {
            if(devPtr->revision == initDataListPtr[i].addrType)
            {
                setMatch = GT_TRUE;
                break;
            }
            i++;
        }

        if(i == initDataListLen)
        {
            return GT_BAD_PARAM;
        }

        if(setMatch == GT_FALSE)
        {
            /* Go to the next section.          */
            /* Count 3 delimiters.              */
            j = 0;
            while((j < 3) && (i < initDataListLen))
            {
                if(initDataListPtr[i].addrType == CPSS_REG_LIST_DELIMITER_CNS)
                {
                    j++;
                }
                i++;
            }

            if((i == initDataListLen) && (deviceFound == GT_FALSE))
            {
                return GT_BAD_PARAM;
            }

            continue;
        }

        deviceFound = GT_TRUE;

        /* set Match == GT_TRUE         */
        /* Find the first delimiter.    */
        while((initDataListPtr[i].addrType != CPSS_REG_LIST_DELIMITER_CNS) &&
              (i < initDataListLen))
        {
            i++;
        }

        if(i == initDataListLen)
        {
            return GT_BAD_PARAM;
        }

        /* Jump over the delimiter.     */
        i++;

        /* Write the pre start-init register values.        */
        while((initDataListPtr[i].addrType != CPSS_REG_LIST_DELIMITER_CNS) &&
              (i < initDataListLen))
        {
            /* This is a phy read/write */
            if (initDataListPtr[i].addrType == CPSS_REG_PHY_ADDR_TYPE_CNS)
            {
                /* Search for the next delimiter*/
                j=i+1;
                while((initDataListPtr[j].addrType != CPSS_REG_PHY_ADDR_TYPE_CNS) &&
                      (j < initDataListLen))
                {
                    /* section delimiter found before phy delimiter*/
                    if (initDataListPtr[i].addrType == CPSS_REG_LIST_DELIMITER_CNS)
                    {
                        return GT_BAD_PARAM;
                    }
                    j++;
                }

                /* If phy second delimiter was not found */
                if (j == initDataListLen)
                {
                    return GT_BAD_PARAM;
                }

                /* Perform the write operation for all the phy whithin the
                    section*/
                while(initDataListPtr[i].addrType != CPSS_REG_PHY_ADDR_TYPE_CNS)
                {
                    regValuePtr = &initDataListPtr[i];
                    /* section delimiter found before phy delimiter*/
                    rc = hwPpPhyRegisterWrite(devNum,
                                              regValuePtr->addrType,
                                              regValuePtr->regMask,
                                              regValuePtr->regVal);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                    i++;
                }
                /* Skip the delimiter*/
                i++;
                continue;
            }

            for(cnt = 0; cnt < initDataListPtr[i].repeatCount; cnt++)
            {
                rc = prvCpssDrvHwPpWriteRegBitMask(devNum,initDataListPtr[i].addrType,
                                             initDataListPtr[i].regMask,
                                             initDataListPtr[i].regVal);

                if(rc != GT_OK)
                {
                    return rc;
                }
            }
            i++;
        }

        if(i == initDataListLen)
        {
            return GT_BAD_PARAM;
        }

        /* Reached the end of a section.            */
        /* Jump over the delimiter.                 */
        i++;

        /* Find the delimiter of the section end.   */
        while((initDataListPtr[i].addrType != CPSS_REG_LIST_DELIMITER_CNS) &&
              (i < initDataListLen))
        {
            i++;
        }

        if(i == initDataListLen)
        {
            return GT_BAD_PARAM;
        }

        i++;
    }

    if((deviceFound == GT_FALSE) && (initDataListLen != 0))
    {
        return GT_NOT_FOUND;
    }

    /* Search for the corresponding set to configure.   */
    /* Currently the set number is the device's         */
    /* revision number.                                 */
    /* SHOULD BE FIXED TO CONSIDER THE DEVICE'S SET #   */
    i = 0;
    while(i < initDataListLen)
    {
        /* For now, just find the device's revision number  */
        setMatch = GT_FALSE;
        while((initDataListPtr[i].addrType != CPSS_REG_LIST_DELIMITER_CNS) &&
              (i < initDataListLen))
        {
            if(devPtr->revision == initDataListPtr[i].addrType)
            {
                setMatch = GT_TRUE;
                break;
            }
            i++;
        }

        if(i == initDataListLen)
        {
            return GT_BAD_PARAM;
        }

        if(setMatch == GT_FALSE)
        {
            /* Go to the next section.          */
            /* Count 3 delimiters.              */
            j = 0;
            while((j < 3) && (i < initDataListLen))
            {
                if(initDataListPtr[i].addrType == CPSS_REG_LIST_DELIMITER_CNS)
                {
                    j++;
                }
                i++;
            }

            if((i == initDataListLen) && (deviceFound == GT_FALSE))
            {
                return GT_BAD_PARAM;
            }

            continue;
        }

        /* set Match == GT_TRUE         */
        /* Count 2 delimiters.          */
        j = 0;
        while((j < 2) && (i < initDataListLen))
        {
            if(initDataListPtr[i].addrType == CPSS_REG_LIST_DELIMITER_CNS)
            {
                j++;
            }
            i++;
        }

        if(i == initDataListLen)
        {
            return GT_BAD_PARAM;
        }

        /* Write the post start-init register values.       */
        while((initDataListPtr[i].addrType != CPSS_REG_LIST_DELIMITER_CNS) &&
              (i < initDataListLen))
        {
            /* Phy delimiter operation*/
            if (initDataListPtr[i].addrType == CPSS_REG_PHY_ADDR_TYPE_CNS)
            {
                /* Search for the next delimiter*/
                j=i+1;
                while((initDataListPtr[j].addrType != CPSS_REG_PHY_ADDR_TYPE_CNS) &&
                      (j < initDataListLen))
                {
                    /* section delimiter found before phy delimiter*/
                    if (initDataListPtr[i].addrType == CPSS_REG_LIST_DELIMITER_CNS)
                    {
                        return GT_BAD_PARAM;
                    }
                    j++;
                }

                /* If phy second delimiter was not found */
                if (j == initDataListLen)
                {
                    return GT_BAD_PARAM;
                }

                /* Perform the write operation for all the phy whithin the
                    section*/
                i++;
                while(initDataListPtr[i].addrType != CPSS_REG_PHY_ADDR_TYPE_CNS)
                {
                    regValuePtr = &initDataListPtr[i];
                    /* section delimiter found before phy delimiter*/
                    rc = hwPpPhyRegisterWrite(devNum,
                                                       regValuePtr->addrType,
                                                       regValuePtr->regMask,
                                                       regValuePtr->regVal);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                    i++;
                }
                /* Skip the delimiter*/
                i++;
                continue;
            }

            for(cnt = 0; cnt < initDataListPtr[i].repeatCount; cnt++)
            {
                rc = prvCpssDrvHwPpWriteRegBitMask(devNum,initDataListPtr[i].addrType,
                                             initDataListPtr[i].regMask,
                                             initDataListPtr[i].regVal);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
            i++;
        }

        if(i == initDataListLen)
        {
            return GT_BAD_PARAM;
        }


        /* Reached the end of a section.            */
        /* Jump over the delimiter.                 */
        i++;
    }


    return GT_OK;
}

/*******************************************************************************
* hwPpStartInitEpromReload
*
* DESCRIPTION:
*       Trigers reload registers from EPROM
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - The device number reload should be done.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
static GT_STATUS hwPpStartInitEpromReload
(
    IN  GT_U8               devNum
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    return prvCpssDrvHwPpSetRegField(devNum,
                              PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->i2cReg.
                              serInitCtrl,22, 1, 1);
}

/*******************************************************************************
* hwPpUnitAddressCheck
*
* DESCRIPTION:
*       Check if single unit address range comprises restricted addresses.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number
*       portGroupId - The port group Id. relevant only to 'multi-port-groups' devices,
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS.
*       addr        - Start offset address to check inside the unit
*       length      - The length of the memory to check inside the unit (the legth is in words).
*
* OUTPUTS:
*       none.

* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on restricted address
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpUnitAddressCheck
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length
)
{
    PRV_CPSS_DXCH_ERRATA_UNIT_RANGES_STC    *unitRangesPtr; /* unit ranges and
                                                               their number */
    GT_U32                                  unitId;         /* unit Id */
    GT_U32                                  rangeId;        /* range Id, inside
                                                               the unit */
    GT_U32                                  portGroupsBmp;

    /* Ignore 2 LSB */
    addr &= 0xFFFFFFFC;/* ignore 2 LSB */
    unitId = (addr >> 23) & 0x3F;

    if (unitId >= UNITS_NUM_CNS)
    {
        return GT_OK;
    }

    if (!PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* XCAT A1 */
        portGroupId = 0;
    }

    if (portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        portGroupsBmp = PRV_CPSS_PP_MAC(devNum)->
                                    portGroupsInfo.activePortGroupsBmp;
    }
    else
    {
        portGroupsBmp = (1 << portGroupId);
    }

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        unitRangesPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                        info_PRV_CPSS_DXCH_XCAT_RESTRICTED_ADDRESS_FILTERING_WA_E.unitRangesPtr[portGroupId];

        if (unitRangesPtr[unitId].numOfRanges == 0)
        {
            /* no restricted ranges in this unit, move to the next portGroup */
            continue;
        }

        /* There are some restricted ranges in this unit */

        /* check if there is restricted range inside the address range given */
        for (rangeId = 0; rangeId < unitRangesPtr[unitId].numOfRanges; rangeId++)
        {
            if (addr > unitRangesPtr[unitId].rangesPtr[rangeId].addrEnd)
            {
                /* move to the next range */
                continue;
            }

            /* check if the address range ends before start of the restricted range */
            if ((addr + (length * 4) - 4) < unitRangesPtr[unitId].rangesPtr[rangeId].addrStart)
            {
                /* the address range has no restricted ranges */
                return GT_OK;
            }

            /* the restricted range is fully or partially inside the address range */
            if (unitRangesPtr[unitId].rangesPtr[rangeId].addrMask == 0x0)
            {
                /* the whole range is restricted */
                return GT_BAD_PARAM;
            }
            else
            {
                /* Not the whole range is restricted, there are valid and not
                   valid adrdesses inside the range */

                if ((addr & unitRangesPtr[unitId].rangesPtr[rangeId].addrMask) ==
                     unitRangesPtr[unitId].rangesPtr[rangeId].addrPattern)
                {
                    /* this specific address is not valid */
                    return GT_BAD_PARAM;
                }

                /* this specific address is valid */
                return GT_OK;
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)


    return GT_OK;
}

/*******************************************************************************
* hwPpAddressCheck
*
* DESCRIPTION:
*       Check if address range comprises restricted addresses.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number
*       portGroupId - The port group Id. relevant only to 'multi-port-groups' devices,
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS.
*       addr        - Start offset address to check
*       length      - The length of the memory to check (the legth is in words).
*
* OUTPUTS:
*       none.

* RETURNS:
*       GT_OK         - on success
*       GT_HW_ERROR   - on hardware error.
*       GT_BAD_PARAM  - on bad devNum or restricted address
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hwPpAddressCheck
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length
)
{
    GT_U32      unitId;         /* unit Id, loop iterator           */
    GT_U32      unitIdStart;    /* start unit Id of the range       */
    GT_U32      unitIdEnd;      /* end unit Id of the range         */
    GT_U32      startAddr;      /* start address of the range       */
    GT_U32      endAddr;        /* end address of the range         */
    GT_U32      unitLen;        /* The length inside specific unit  */
    GT_STATUS   rc;             /* return status                    */


    /* calculate end address of the range */
    endAddr = (addr + (length * 4) - 4) & 0xFFFFFFFC;

    /* Unit Id is defined by bits 23-28 */
    unitIdStart = ((addr >> 23) & 0x3F);
    unitIdEnd = ((endAddr >> 23) & 0x3F);


    if (unitIdStart == unitIdEnd)
    {
        /* There is only one unit to check */
        rc = hwPpUnitAddressCheck(devNum,
                                  portGroupId,
                                  addr,
                                  length);
        return rc;
    }

    /* In case there are more then one unit in a range */
    for (unitId = unitIdStart; unitId <= unitIdEnd; unitId++)
    {
        if (unitId == unitIdStart)
        {
            /* The range ends in the start of the next unit */
            startAddr = addr;
            unitLen = (((unitId + 1) << 23) - (addr & 0xFFFFFFFC)) / 4 ;
        }
        else if (unitId == unitIdEnd)
        {
            /* The address starts at the start of unit */
            startAddr = (unitId << 23);
            unitLen = ((endAddr - startAddr) / 4) + 1;
        }
        else
        {
            /* all units that are inside the range */
            startAddr = (unitId << 23);
            /* The whole unit range should be checked */
            unitLen = PRV_DXCH_MAX_UNIT_ADDR_RANGE_CNS;
        }

        rc = hwPpUnitAddressCheck(devNum,
                                  portGroupId,
                                  startAddr,
                                  unitLen);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChHwPpStartInit
*
* DESCRIPTION:
*       This phase performs the start-init operation on a given device, and sets
*       it's registers according to the registers value list passed by user.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - The device number to which the start-init operation
*                         should be done.
*       loadFromEeprom  - GT_TRUE, trigger the device to re-load the eeprom.
*       initDataListPtr - List of registers values to be set to the device.
*       initDataListLen - Number of elements in ppRegConfigList
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - on timed out retries
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function should be invoked after cpssDxChHwPpPhase1Init() and
*       before cpssDxChHwPpPhase2Init().
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpStartInit
(
    IN  GT_U8                       devNum,
    IN  GT_BOOL                     loadFromEeprom,
    IN  CPSS_REG_VALUE_INFO_STC     *initDataListPtr,
    IN  GT_U32                      initDataListLen
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(initDataListPtr);

    /* set registers according to the registers value list passed by user */
    rc = hwPpStartInit(devNum,initDataListPtr,initDataListLen);
    if(rc != GT_OK)
    {
        return rc;
    }

    if (loadFromEeprom == GT_TRUE)
    {
        /* Triggers reload registers from EPROM */
        rc = hwPpStartInitEpromReload(devNum);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChHwPpImplementWaInit
*
* DESCRIPTION:
*       This function allow application to state which WA (workarounds) the CPSS
*       should implement.
*       NOTEs:
*       1. The function may be called several times with different needed WA.
*       2. The CPSS will implement the WA for the requested WA , even when the HW
*          not require the 'WA' anymore.
*          for example:
*                assume the xcat A1 has erratum regarding "wrong trunk id
*                source port information of packet to CPU" , and in the WA the CPSS
*                will limit application to specific trunk-IDs.
*                but if no longer have this problem , but the application
*                still request for the WA , the CPSS will continue to handle the
*                WA.
*                further more - application may ask the WA for ch3...
*       3. The function can be invoked only after cpssDxChHwPpPhase1Init().
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number.
*       numOfWa             - number of WA needed (number of elements in
*                             waArr[] and additionalInfoBmpArr[] arrays).
*       waArr[]     - (array of) WA needed to implement
*       additionalInfoBmpArr[] - (array of) bitmap for additional WA info.
*                              The meaning of the additional info is located
*                              in the description of the specific WA. May be NULL.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum or numOfWa = 0 or bad WA value
*       GT_BAD_STATE             - the WA can't be called at this stage ,
*                                  because it's relate library already initialized.
*                                  meaning it is 'too late' to request for the WA.
*       GT_NOT_IMPLEMENTED       - the WA can't be implemented for the current device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - on timed out retries
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpImplementWaInit
(
    IN GT_U8                        devNum,
    IN GT_U32                       numOfWa,
    IN CPSS_DXCH_IMPLEMENT_WA_ENT   waArr[],
    IN GT_U32                       additionalInfoBmpArr[]
)
{
    GT_STATUS rc;
    GT_U32    waIndex;/* WA index */
    GT_U32    waInfoBmp;/*bmp info. indicate if additionalInfoBmpArr is NULL*/
    GT_U32    regAddr; /* register's address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(waArr);

    if(numOfWa == 0)
    {
        /* no need to call the API , if nothing to do ...*/
        return GT_BAD_PARAM;
    }


    /* loop on the WA needed from CPSS and set internal flags according to them */
    for(waIndex = 0 ; waIndex < numOfWa; waIndex++)
    {
        waInfoBmp = (additionalInfoBmpArr) ? additionalInfoBmpArr[waIndex] : 0;
        switch(waArr[waIndex])
        {
            case CPSS_DXCH_IMPLEMENT_WA_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E:
                if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E)
                {
                    /* the implementation exists only for XCAT devices */
                    return GT_NOT_IMPLEMENTED;
                }

                if(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
                        enabled == GT_TRUE)
                {
                    /* the WA already stated , no need to check that trunk init
                       was not done */
                    break;
                }

                if(PRV_CPSS_PP_MAC(devNum)->trunkInfo.initDone == GT_TRUE)
                {
                    /* indication that cpssDxChTrunkInit(...) was already called */
                    return GT_BAD_STATE;
                }

                PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
                        enabled = GT_TRUE;

                PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
                        additionalInfoBmp = waInfoBmp;

                /* initialize the WA */
                rc = prvCpssDxChTrunkTrafficToCpuWaUpInit(devNum,
                                                          waInfoBmp);
                if(rc != GT_OK)
                {
                    return rc;
                }
                break;

            case CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_E:

			    printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);/*xcat debug*/
                if(PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.initDone == GT_TRUE)
                {
                    /* indication that cpssDxChNetIfInit(...) was already called */
                    return GT_BAD_STATE;
                }

                /* change the enabled = GT_FALSE,so cpssDxChNetIfInit() into SDMA*/
                PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH_XCAT_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_E.
                        enabled = GT_FALSE;
				
                PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH_XCAT_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_E.
                        devNum = (GT_U8)waInfoBmp;
                break;
				
#if 0
                PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH_XCAT_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_E.
                        enabled = GT_TRUE;

#endif


            case CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FIFO_E:

                 if(PRV_CPSS_PP_MAC(devNum)->
                    intCtrl.auDescCtrl[PRV_CPSS_FIRST_ACTIVE_PORT_GROUP_ID_MAC(devNum)].blockAddr != 0)
                {
                    /* indication cpssDxChCfgPpLogicalInit(...)
                       that inits AU queues was already called */
                    return GT_BAD_STATE;
                }

                PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.
                        enabled = GT_TRUE;

                break;

            case CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_PADDING_E:

                /* Enable packet from CPU padding WA */
                PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,
                                             PRV_CPSS_DXCH_XCAT_TX_CPU_CORRUPT_BUFFER_WA_E);

                break;

            case CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FU_FROM_NON_SOURCE_PORT_GROUP_E:
                /* check that the device is 'multi-port groups' device -->
                   otherwise return GT_NOT_APPLICABLE_DEVICE */
                PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);

                PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH_LION_FDB_AU_FU_MESSAGES_FROM_NON_SOURCE_PORT_GROUP_WA_E.
                        enabled = GT_TRUE;

                break;

            case CPSS_DXCH_IMPLEMENT_WA_RESTRICTED_ADDRESS_FILTERING_E:

                /* Check if the device is XCAT */
                if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum) &&
                    (! PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum)))
                {
                    /* Initialize WA DB */
                    xcatA1ErrataAddrCheckWaDbInit(devNum);

                    /* Bind callback to driver errata db */
                    rc = prvCpssDrvAddrCheckWaBind(devNum, &hwPpAddressCheck);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }

                if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
                {
                    /* Initialize WA DB */
                    lionErrataAddrCheckWaDbInit(devNum);

                    /* Bind callback to driver errata db */
                    rc = prvCpssDrvAddrCheckWaBind(devNum, &hwPpAddressCheck);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }

                if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
                {
                    /* Initialize WA DB for xCat2*/
                    xCat2ErrataAddrCheckWaDbInit(devNum);

                    /* Bind callback to driver errata db */
                    rc = prvCpssDrvAddrCheckWaBind(devNum, &hwPpAddressCheck);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }
                break;

            case CPSS_DXCH_IMPLEMENT_WA_RGMII_EDGE_ALIGN_MODE_E:
                /* Check if the device is XCAT */
                if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum) &&
                    (! PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum)))
                {
                    /* Set the FTDLL register to 0x0 */
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.ftdllReg;
                    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }
                break;

            case CPSS_DXCH_IMPLEMENT_WA_IP_MC_UNDER_STRESS_E:
                /* Check if the device is dxCh3 B2 */
                if( (CPSS_PP_FAMILY_CHEETAH3_E == PRV_CPSS_PP_MAC(devNum)->devFamily) &&
                    (1 == PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.mg.metalFix) )
                {
                    /* IP Multicast traffic under stress conditions may lead to device hang. */
                    /* (FEr#2263) */
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;
                    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 6, 1, 1);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
                else
                {
                    return GT_NOT_APPLICABLE_DEVICE;
                }
                break;

            default:
                /* unknown WA */
                return GT_BAD_PARAM;
        }
    }


    return GT_OK;
}

/*******************************************************************************
* cpssDxChHwAuDescSizeGet
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Address Update
*       descriptor, for a given device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSizePtr- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChHwAuDescSizeGet
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSizePtr
)
{
    /* the devType is not used yet */
    (void)devType;

    CPSS_NULL_PTR_CHECK_MAC(descSizePtr);

    *descSizePtr = AU_DESC_SIZE;
    return GT_OK;
}


/*******************************************************************************
* cpssDxChHwRxDescSizeGet
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Rx descriptor,
*       for a given device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSizePtr- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChHwRxDescSizeGet
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSizePtr
)
{
    /* the devType is not used yet */
    (void)devType;

    CPSS_NULL_PTR_CHECK_MAC(descSizePtr);

    *descSizePtr = RX_DESC_SIZE;
    return GT_OK;
}


/*******************************************************************************
* cpssDxChHwTxDescSizeGet
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Tx descriptor,
*       for a given device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSizePtr- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChHwTxDescSizeGet
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSizePtr
)
{
    /* the devType is not used yet */
    (void)devType;

    CPSS_NULL_PTR_CHECK_MAC(descSizePtr);

    *descSizePtr = TX_DESC_SIZE + TX_SHORT_BUFF_SIZE + (TX_HEADER_SIZE / 2);
    return GT_OK;
}



/*******************************************************************************
* cpssDxChHwRxBufAlignmentGet
*
* DESCRIPTION:
*       This function returns the required alignment in bytes of a RX buffer,
*       for a given device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       byteAlignmentPtr - The buffer alignment (in bytes).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChHwRxBufAlignmentGet
(
    IN  CPSS_PP_DEVICE_TYPE devType,
    OUT GT_U32              *byteAlignmentPtr
)
{
    /* the devType is not used yet */
    (void)devType;

    CPSS_NULL_PTR_CHECK_MAC(byteAlignmentPtr);

    *byteAlignmentPtr = 128;
    return GT_OK;
}


/*******************************************************************************
* cpssDxChHwPpSoftResetTrigger
*
* DESCRIPTION:
*       This routine issue soft reset for a specific pp.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to reset.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The soft reset is the
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpSoftResetTrigger
(
    IN  GT_U8 devNum
)
{
    GT_U32    resetRegAddr;   /* register address                */
    GT_U32    bitOffset;      /* bit offset or value in register */
    GT_U32    triggerVal;     /* trigger value                   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* xCat2 */
        resetRegAddr =
             PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.resetControl;
        bitOffset  = 0;
        triggerVal = 0;
    }
    else
    {
        /* all devices exclude xCat2 */
        resetRegAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;
        bitOffset  = 16;
        triggerVal = 1;
    }

    /* state to the special DB that the device did HW reset */
    prvCpssPpConfigDevDbHwResetSet(devNum,GT_TRUE);

    return  prvCpssDrvHwPpSetRegField(
        devNum, resetRegAddr, bitOffset, 1, triggerVal) ;
}

/*******************************************************************************
* cpssDxChHwPpSoftResetSkipParamSet
*
* DESCRIPTION:
*       This routine configure skip parameters related to soft reset.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       skipType    - the skip parameter to set
*                     see comments in CPSS_HW_PP_RESET_SKIP_TYPE_ENT.
*       skipEnable  - GT_FALSE: Do Not Skip
*                     GT_TRUE:  Skip
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, or skipType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpSoftResetSkipParamSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_HW_PP_RESET_SKIP_TYPE_ENT  skipType,
    IN  GT_BOOL                         skipEnable

)
{
    GT_STATUS rc = GT_OK;/* return code */
    GT_U32    regAddr;   /* register address */
    GT_U32    resetRegAddr;   /* register address */
    GT_U32    bitOffset; /* bit offset or value in register */
    GT_U32    i;         /* loop index */
    GT_U32    devType;   /* device type */
    GT_U32    hwValue;   /* HW value            */
    GT_U32    targetIdx; /* reset target index  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* fix relevant only for dxCh3, revision B2*/
    if ((CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E == skipType)
        && (CPSS_PP_FAMILY_CHEETAH3_E
           == PRV_CPSS_PP_MAC(devNum)->devFamily)
        && (1 == PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.mg.metalFix))
    {
            bitOffset = ( GT_TRUE == skipEnable ? 1 : 0 );

            for( i = 0 ; i < 24 ; i++ )
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[i].serdesMetalFix;
                rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 8, 1,
                                               bitOffset);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            devType = PRV_CPSS_PP_MAC(devNum)->devType;
            switch(devType)
            {
                case PRV_CPSS_CHEETAH3_XG_PORTS_DEVICES_CASES_MAC:
                    for( i = 0 ; i < 24 ; i++ )
                    {
                        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, i);

                        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                            macRegs.perPortRegs[i].metalFix;
                        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1,
                                                       bitOffset);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                    break;
                default:
                    for( i = 0 ; i < 24 ; i++ )
                    {
                        /* if GT_TRUE  == skipEnable write value is 0b10 (0x2). */
                        /* if GT_FALSE == skipEnable write value is 0b01 (0x1). */
                        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,i,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,i),&regAddr);

                        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 2, 2,
                                                       bitOffset+1);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                    break;
            }

        return GT_OK;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily
             == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* xCat2 */
        switch(skipType)
        {
            case CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E:
                targetIdx = 0;
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E:
                targetIdx = 3;
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E:
                targetIdx = 2;
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E:
                targetIdx = 7;
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E:
                return GT_NOT_SUPPORTED;
            default:
                return GT_BAD_PARAM;
        }

        /* the matrix is a pair of registers 5 sources multiple 12 targets */
        /* each register contains 6 5-bit-groups for each of 6 targets     */
        /* the management (host CPU) source index is 4                     */
        resetRegAddr = (targetIdx < 6)
            ? PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                dfxUnits.server.genSkipInitMatrix0
            : PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                dfxUnits.server.genSkipInitMatrix1;
        bitOffset = (((targetIdx % 6) * 5) + 4);
        hwValue = ((~ BOOL2BIT_MAC(skipEnable)) & 1);
    }
    else
    {
        /* all devices exclude xCat2 */
        switch(skipType)
        {
            case CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E:
                bitOffset = 21;
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E:
                bitOffset = 22;
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E:
                bitOffset = 23;
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E:
                if((PRV_CPSS_PP_MAC(devNum)->devFamily  == CPSS_PP_FAMILY_CHEETAH_E)||
                   (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH2_E))
                {
                    return GT_NOT_SUPPORTED;
                }
                else
                {
                    bitOffset = 24;
                }
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E:
                return GT_NOT_SUPPORTED;
            default:
                return GT_BAD_PARAM;
        }

        resetRegAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;
        hwValue = BOOL2BIT_MAC(skipEnable);
    }

    rc =  prvCpssDrvHwPpSetRegField(
        devNum, resetRegAddr, bitOffset, 1, hwValue) ;

    return rc;

}

/*******************************************************************************
* cpssDxChHwPpSoftResetSkipParamGet
*
* DESCRIPTION:
*       This routine return configuration of skip parameters related to soft reset.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       skipType    - the skip parameter to set
*                     see comments in CPSS_HW_PP_RESET_SKIP_TYPE_ENT.
*
* OUTPUTS:
*       skipEnablePtr   - GT_FALSE: Do Not Skip
*                         GT_TRUE:  Skip
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, or skipType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpSoftResetSkipParamGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_HW_PP_RESET_SKIP_TYPE_ENT  skipType,
    OUT GT_BOOL                         *skipEnablePtr

)
{
    GT_STATUS rc;        /* return code */
    GT_U32    bitOffset; /* bit offset in register */
    GT_U32    value;     /* register data */
    GT_U32    resetRegAddr;   /* register address */
    GT_U32    targetIdx; /* reset target index  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(skipEnablePtr);

    if ((CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E == skipType)
        && (CPSS_PP_FAMILY_CHEETAH3_E
            == PRV_CPSS_PP_MAC(devNum)->devFamily)
        && (1 == PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.mg.metalFix))
        {
            rc = prvCpssDrvHwPpGetRegField(devNum,
                        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[0].serdesMetalFix,
                        8, 1, &value);
            if( GT_OK != rc )
            {
                return rc;
            }

        *skipEnablePtr = BIT2BOOL_MAC(value);
        return GT_OK;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily
             == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* xCat2 */
        switch(skipType)
        {
            case CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E:
                targetIdx = 0;
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E:
                targetIdx = 3;
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E:
                targetIdx = 2;
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E:
                targetIdx = 7;
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E:
                return GT_NOT_SUPPORTED;
            default:
                return GT_BAD_PARAM;
        }

        /* the matrix is a pair of registers 5 sources multiple 12 targets */
        /* each register contains 6 5-bit-groups for each of 6 targets     */
        /* the management (host CPU) source index is 4                     */
        resetRegAddr = (targetIdx < 6)
            ? PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                dfxUnits.server.genSkipInitMatrix0
            : PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                dfxUnits.server.genSkipInitMatrix1;
        bitOffset = (((targetIdx % 6) * 5) + 4);
    }
    else
    {
        switch(skipType)
        {
            case CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E:
                bitOffset = 21;
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E:
                bitOffset = 22;
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E:
                bitOffset = 23;
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E:
                if((PRV_CPSS_PP_MAC(devNum)->devFamily  == CPSS_PP_FAMILY_CHEETAH_E)||
                   (PRV_CPSS_PP_MAC(devNum)->devFamily  == CPSS_PP_FAMILY_CHEETAH2_E))
                {
                    return GT_NOT_SUPPORTED;
                }
                else
                {
                    bitOffset = 24;
                }
                break;
            case CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E:
                return GT_NOT_SUPPORTED;
            default:
                return GT_BAD_PARAM;
        }

         /* all devices exclude xCat2 */
        resetRegAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;
   }

    rc =  prvCpssDrvHwPpGetRegField(
        devNum, resetRegAddr, bitOffset, 1, &value) ;
    if (rc != GT_OK)
    {
        return rc;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* xCat2 - invert the retrieved bit */
        value = ((~ value) & 1);
    }
    *skipEnablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChHwPpInitStageGet
*
* DESCRIPTION:
*       Indicates the initialization stage of the device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number to reset.
*
* OUTPUTS:
*       initStagePtr - pointer to the stage of the device
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpInitStageGet
(
    IN  GT_U8                      devNum,
    OUT CPSS_HW_PP_INIT_STAGE_ENT  *initStagePtr
)
{
    GT_STATUS rc;
    GT_U32    value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(initStagePtr);

    rc =  prvCpssDrvHwPpGetRegField(devNum,
                   PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl,
                   17, 2, &value) ;

    if(rc != GT_OK)
        return rc;

    switch(value)
    {
        case 0:
            *initStagePtr =  CPSS_HW_PP_INIT_STAGE_INIT_DURING_RESET_E;
            break;
        case 1:
            *initStagePtr =  CPSS_HW_PP_INIT_STAGE_EEPROM_DONE_INT_MEM_DONE_E;
            break;
        case 2:
            *initStagePtr =  CPSS_HW_PP_INIT_STAGE_EEPROM_NOT_DONE_INT_MEM_DONE_E;
            break;
        case 3:
            *initStagePtr =  CPSS_HW_PP_INIT_STAGE_FULLY_FUNC_E;
            break;
        default:
            return GT_FAIL;
    }

    return rc;

}

/*******************************************************************************
* cpssDxChCfgHwDevNumSet
*
* DESCRIPTION:
*       write HW device number
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       hwDevNum - HW device number (0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_OUT_OF_RANGE          - on hwDevNum > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgHwDevNumSet
(
    IN GT_U8    devNum,
    IN GT_U8    hwDevNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(hwDevNum >= BIT_5)
    {
        /* 5 bits in HW */
        return GT_OUT_OF_RANGE;
    }

    /* The function must be called before DB update by new HW device number. */
    rc = prvCpssDxChTrunkHwDevNumSet(devNum, hwDevNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* save HW devNum to the DB */
    PRV_CPSS_HW_DEV_NUM_MAC(devNum) = hwDevNum;

    /* update "ownDevNum" number */
    return prvCpssDrvHwPpSetRegField(devNum,
                                   PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                   globalRegs.globalControl,4,5,hwDevNum);
}

/*******************************************************************************
* cpssDxChCfgHwDevNumGet
*
* DESCRIPTION:
*       Read HW device number
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       hwDevNumPtr - Pointer to HW device number
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgHwDevNumGet
(
    IN GT_U8    devNum,
    OUT GT_U8   *hwDevNumPtr
)
{
    GT_U32    regVal; /* register field value */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(hwDevNumPtr);

    /* Get "ownDevNum" number */
    rc = prvCpssDrvHwPpGetRegField(devNum,
                                   PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                   globalRegs.globalControl,4,5,&regVal);
    if(rc != GT_OK)
    {
        return rc;
    }

    *hwDevNumPtr = (GT_U8)regVal;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChHwCoreClockGet
*
* DESCRIPTION:
*       This function returns the core clock value from cpss DB and from HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - The Pp's device number.
*
* OUTPUTS:
*       coreClkDbPtr    - Pp's core clock from cpss DB (MHz)
*       coreClkHwPtr    - Pp's core clock read from HW (MHz) or zero
*                         if PLL configuration as sampled at reset could
*                         not be mapped to core clock value.
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChHwCoreClockGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *coreClkDbPtr,
    OUT GT_U32  *coreClkHwPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(coreClkDbPtr);
    CPSS_NULL_PTR_CHECK_MAC(coreClkHwPtr);

    *coreClkDbPtr = PRV_CPSS_PP_MAC(devNum)->coreClock;
    rc = hwPpAutoDetectCoreClock(devNum,coreClkHwPtr);
    if (rc == GT_BAD_STATE)
    {
        *coreClkHwPtr = 0;
        rc = GT_OK;
    }
    return rc;
}

/*******************************************************************************
* cpssDxChHwInterruptCoalescingSet
*
* DESCRIPTION:
*       Configures the interrupt coalescing parameters and enable\disable the
*       functionality.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE: interrupt coalescing is enabled
*                 GT_FALSE: interrupt coalescing is disabled
*       period  - Minimal IDLE period between two consecutive interrupts.
*                 The units of this input parameter is in nSec, the
*                 resolution is in 320 nSec (the HW resolution for the
*                 interrupt coalescing period is 0.32uSec). In case interrupt
*                 coalescing is enabled (enable == GT_TRUE) should be in the
*                 range 320 - 5242560. Otherwise (enable == GT_FALSE),
*                 ignored.
*       linkChangeOverride  - GT_TRUE: A link change in one of the ports
*                             results interrupt regardless of the (coalescing)
*                             period.
*                             GT_FALSE: A link change in one of the ports
*                             does not results interrupt immediately but
*                             according to the (coalescing) period.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - parameter out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChHwInterruptCoalescingSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable,
    IN GT_U32   period,
    IN GT_BOOL  linkChangeOverride
)
{
    GT_U32 regAddr;   /* register's address */
    GT_U32 regData;   /* register's data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if( enable == GT_TRUE )
    {
        if( period < MIN_INTERRUPT_COALESCING_PERIOD_CNS ||
            period > MAX_INTERRUPT_COALESCING_PERIOD_CNS )
        {
            return GT_OUT_OF_RANGE;
        }
        regData = period/MIN_INTERRUPT_COALESCING_PERIOD_CNS;
    }
    else /* enable == GT_FALSE */
    {
        regData = 0;
    }

    U32_SET_FIELD_MAC(regData, 16, 1, ((linkChangeOverride == GT_TRUE) ? 0 : 1));

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.interruptCoalescing;

    return prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0x13FFF, regData);
}

/*******************************************************************************
* cpssDxChHwInterruptCoalescingGet
*
* DESCRIPTION:
*       Gets the interrupt coalescing configuration parameters.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) GT_TRUE: interrupt coalescing is enabled
*                                 GT_FALSE: interrupt coalescing is disabled
*       periodPtr  - (pointer to) Minimal IDLE period between two consecutive
*                    interrupts. The units are in nSec with resolution of
*                    320nSec (due to HW resolution) and is relevant only in
*                    case interrupt coalescing is enabled (*enablePtr == GT_TRUE).
*                    Range 320 - 5242560.
*       linkChangeOverridePtr  - (pointer to)
*                                GT_TRUE: A link change in one of the ports
*                                results interrupt regardless of the (coalescing)
*                                period.
*                                GT_FALSE: A link change in one of the ports
*                                does not results interrupt immediately but
*                                according to the (coalescing) period.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The HW resolution for the interrupt coalescing period is 0.32uSec.
*
*******************************************************************************/
GT_STATUS cpssDxChHwInterruptCoalescingGet
(
    IN GT_U8        devNum,
    OUT GT_BOOL     *enablePtr,
    OUT GT_U32      *periodPtr,
    OUT GT_BOOL     *linkChangeOverridePtr
)
{
    GT_U32 regAddr;   /* register's address */
    GT_U32 regData;   /* register's data */
    GT_STATUS rc;     /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(periodPtr);
    CPSS_NULL_PTR_CHECK_MAC(linkChangeOverridePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.interruptCoalescing;

    rc = prvCpssDrvHwPpReadRegBitMask(devNum, regAddr, 0x13FFF, &regData);
    if( rc != GT_OK )
        return rc;

    *periodPtr = (GT_U32)(U32_GET_FIELD_MAC(regData, 0, 14) * MIN_INTERRUPT_COALESCING_PERIOD_CNS);

    *enablePtr = (*periodPtr == 0 ) ? GT_FALSE : GT_TRUE ;

    *linkChangeOverridePtr =
        (U32_GET_FIELD_MAC(regData, 16, 1) == 0) ? GT_TRUE : GT_FALSE ;

    return GT_OK;
}




#ifdef DEBUG_OPENED
/*******************************************************************************
* cpssDxChPrintFineTuningInfo
*
* DESCRIPTION:
*       DxCh prints information about Packet processor , fine tuning values
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - PP's device number .
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - invalid devNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPrintFineTuningInfo
(
    IN GT_U8                    devNum
)
{
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    PRV_CPSS_DXCH_PP_HW_INFO_STC            *ppHwInfoPtr;
    PRV_CPSS_GEN_PP_CONFIG_STC *devInfoPtr = PRV_CPSS_PP_MAC(devNum);
    PRV_CPSS_DXCH_PP_CONFIG_STC *dxChDevInfoPtr = PRV_CPSS_DXCH_PP_MAC(devNum);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    fineTuningPtr = &dxChDevInfoPtr->fineTuning;
    ppHwInfoPtr = &dxChDevInfoPtr->hwInfo;

    DUMP_PRINT_MAC((" devNum = [%d] \n",devInfoPtr->devNum));
    DUMP_PRINT_MAC((" devType = [0x%8.8x] \n",devInfoPtr->devType));
    DUMP_PRINT_MAC((" revision = [%d] \n",devInfoPtr->revision));
    DUMP_PRINT_MAC((" devFamily = [%s] \n",
        devInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH_E   ? " CPSS_PP_FAMILY_CHEETAH_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH2_E  ? " CPSS_PP_FAMILY_CHEETAH2_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH3_E  ? " CPSS_PP_FAMILY_CHEETAH3_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E  ? " CPSS_PP_FAMILY_DXCH_XCAT_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION_E  ? " CPSS_PP_FAMILY_DXCH_LION_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E  ? " CPSS_PP_FAMILY_DXCH_XCAT2_E ":
        " unknown "
    ));

    /************************/
    /* fine tuning - tables */
    /************************/

        /* bridge section */
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.fdb                     )
        ));
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.vidxNum                 )
        ));
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.stgNum                  )
        ));
        /* tti section */
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.routerAndTunnelTermTcam )
        ));
        /* tunnel section*/
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.tunnelStart )
        ));
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.routerArp )
        ));
        /* ip section */
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.routerNextHop           )
        ));
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.mllPairs                )
        ));
        /* PCL section */
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.policyTcamRaws          )
        ));
        /* CNC section */
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.cncBlocks               )
        ));
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.cncBlockNumEntries      )
        ));
        /* Policer Section */
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.policersNum             )
        ));
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.egressPolicersNum       )
        ));
        /* trunk Section */
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.trunksNum               )
        ));
        /* Transmit Descriptors */
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.transmitDescr           )
        ));
        /* Buffer Memory */
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.bufferMemory            )
        ));
        /*  Transmit Queues */
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->tableSize.txQueuesNum             )
        ));

    /*********************************/
    /* fine tuning - feature support */
    /*********************************/

        /* TR101 Feature support */
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->featureInfo.tr101Supported)
        ));
        /* VLAN translation support */
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->featureInfo.vlanTranslationSupported)
        ));
        /* Policer Ingress second stage support flag */
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->featureInfo.iplrSecondStageSupported)
        ));
        /* trunk CRC hash support flag */
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(fineTuningPtr->featureInfo.trunkCrcHashSupported)
        ));

    /***************/
    /* other info  */
    /***************/

        /*policer 0 - Memory size per stage*/
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(dxChDevInfoPtr->policer.memSize[0])
        ));
        /*policer 1 - Memory size per stage*/
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(dxChDevInfoPtr->policer.memSize[1])
        ));
        /*policer 2 - Memory size per stage*/
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(dxChDevInfoPtr->policer.memSize[2])
        ));


    /************/
    /* HW info  */
    /************/
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(ppHwInfoPtr->l2i.supportIngressRateLimiter)
        ));
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(ppHwInfoPtr->cnc.cncBlocks)
        ));
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(ppHwInfoPtr->cnc.cncBlockNumEntries)
        ));
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(ppHwInfoPtr->txq.revision)
        ));
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(ppHwInfoPtr->ha.cpuPortBitIndex)
        ));
    DUMP_PRINT_MAC((" [%s] = [%d] \n",
        NAME_AND_VALUE_MAC(ppHwInfoPtr->mg.metalFix)
        ));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPrintErrataNamesInfo
*
* DESCRIPTION:
*       DxCh prints information about which errata WA implemented for the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - PP's device number .
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - invalid devNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPrintErrataNamesInfo
(
    IN GT_U8                    devNum
)
{
    GT_U32  ii;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    DUMP_PRINT_MAC(("cpssDxChPrintErrataNamesInfo : the errata,RM WA for the device \n"));

    for(ii = 0 ; ii < PRV_CPSS_DXCH_ERRATA_MAX_NUM_E; ii++)
    {
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,ii))
        {
            if(errataNames[ii] == NULL)
            {
                DUMP_PRINT_MAC((" index %d is unknown \n" ,ii));
            }
            else
            {
                DUMP_PRINT_MAC((" %s \n" , errataNames[ii]));
            }
        }
    }

    return GT_OK;
}

#endif /*DEBUG_OPENED*/


