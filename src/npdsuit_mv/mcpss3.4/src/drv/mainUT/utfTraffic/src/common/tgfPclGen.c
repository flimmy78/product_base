/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfPclGen.c
*
* DESCRIPTION:
*       Generic API implementation for PCL
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <utf/private/prvUtfHelpers.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

#include <trafficEngine/tgfTrafficEngine.h>
#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfPclGen.h>
#include <common/tgfIpGen.h>


/******************************************************************************\
 *                              Marco definitions                             *
\******************************************************************************/

#ifdef CHX_FAMILY
/* convert policy rules modes into device specific format */
#define PRV_TGF_S2D_PCL_RULE_FORMAT_CONVERT_MAC(dstKey, srcKey)                \
    do                                                                         \
    {                                                                          \
        switch (srcKey)                                                        \
        {                                                                      \
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E,\
                                    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E);\
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)

/* convert policy rules modes from device specific format */
#define PRV_TGF_D2S_PCL_RULE_FORMAT_CONVERT_MAC(dstKey, srcKey)                \
    do                                                                         \
    {                                                                          \
        switch (dstKey)                                                        \
        {                                                                      \
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E,);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E);\
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)

/* copy commonStdIp field from source structure into destination structure  */
#define PRV_TGF_STC_COMMON_STD_IP_FIELD_COPY_MAC(dstStcPtr, srcStcPtr) \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), ipProtocol); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), dscp); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), isL4Valid); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), l4Byte2); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), l4Byte3); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), ipHeaderOk); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), ipv4Fragmented); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), ipv4Fragmented)

/* copy commonExt field from source structure into destination structure  */
#define PRV_TGF_STC_COMMON_EXT_FIELD_COPY_MAC(dstStcPtr, srcStcPtr) \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), isIpv6); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), ipProtocol); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), dscp); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), isL4Valid); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), l4Byte0); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), l4Byte1); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), l4Byte2); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), l4Byte3); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), l4Byte13); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), ipHeaderOk)

/* copy commonEgrStdIp field from source structure into destination structure  */
#define PRV_TGF_STC_COMMON_EGR_STD_IP_FIELD_COPY_MAC(dstStcPtr, srcStcPtr) \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), isIpv4); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), ipProtocol); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), dscp); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), isL4Valid); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), l4Byte2); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), l4Byte3); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), ipv4Fragmented); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonStdIp), &(srcStcPtr->commonStdIp), egrTcpUdpPortComparator)

/* copy commonEgrExt field from source structure into destination structure  */
#define PRV_TGF_STC_COMMON_EGR_EXT_FIELD_COPY_MAC(dstStcPtr, srcStcPtr) \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), isIpv6); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), ipProtocol); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), dscp); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), isL4Valid); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), l4Byte0); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), l4Byte1); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), l4Byte2); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), l4Byte3); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), l4Byte13); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonExt), &(srcStcPtr->commonExt), egrTcpUdpPortComparator)

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
/* convert policy rules modes into device specific format */
#define PRV_TGF_S2D_PCL_RULE_FORMAT_CONVERT_MAC(dstKey, srcKey)                \
    do                                                                         \
    {                                                                          \
        switch (srcKey)                                                        \
        {                                                                      \
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_NOT_IPV6_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L2_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L4_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IP_L2_QOS_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IPV4_L4_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L2_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L4_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_E);\
            PRV_TGF_SWITCH_CASE_MAC(dstKey, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_80B_E,\
                                    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTERNAL_80B_E)\
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)

/* convert policy rules modes from device specific format */
#define PRV_TGF_D2S_PCL_RULE_FORMAT_CONVERT_MAC(dstKey, srcKey)                \
    do                                                                         \
    {                                                                          \
        switch (dstKey)                                                        \
        {                                                                      \
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_NOT_IPV6_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L2_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L4_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IP_L2_QOS_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IPV4_L4_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L2_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L4_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);\
            PRV_TGF_SWITCH_CASE_MAC(srcKey, CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTERNAL_80B_E,\
                                    PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_80B_E);\
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)

/* copy commonIngrStdIp field from source structure into destination structure  */
#define PRV_TGF_STC_COMMON_STD_IP_FIELD_COPY_MAC(dstStcPtr, srcStcPtr) \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonIngrStdIp), &(srcStcPtr->commonStdIp), ipHeaderOk); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonIngrStdIp), &(srcStcPtr->commonStdIp), ipProtocol); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonIngrStdIp), &(srcStcPtr->commonStdIp), dscp); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonIngrStdIp), &(srcStcPtr->commonStdIp), isL4Valid); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonIngrStdIp), &(srcStcPtr->commonStdIp), l4Byte2); \
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dstStcPtr->commonIngrStdIp), &(srcStcPtr->commonStdIp), l4Byte3); \
                                                                                                       \
    dstStcPtr->commonIngrStdIp.isIpv4Fragment = srcStcPtr->commonStdIp.ipv4Fragmented

/* copy modify QoS attribute field from source structure into destination structure  */
#define PRV_TGF_STC_MODIFY_QOS_ATTR_FIELD_COPY_MAC(dstStcPtr, srcStcPtr, modify)\
    switch (srcStcPtr->modify)                                                 \
    {                                                                          \
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:                            \
            dstStcPtr->qosParamsModify.modify = GT_TRUE;                       \
            break;                                                             \
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:                           \
            dstStcPtr->qosParamsModify.modify = GT_FALSE;                      \
            break;                                                             \
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:                     \
            break;                                                             \
        default:                                                               \
            return GT_BAD_PARAM;                                               \
    }

#endif /* EXMXPM_FAMILY */


/******************************************************************************\
 *                           Private declarations                             *
\******************************************************************************/


/******************************************************************************\
 *                     Private function's implementation                      *
\******************************************************************************/

#ifdef CHX_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToDxChPclRuleCommon
*
* DESCRIPTION:
*       Convert generic into device specific common segment of all key formats
*
* INPUTS:
*       ruleCommonPtr - (pointer to) common part for all formats
*
* OUTPUTS:
*       dxChRuleCommonPtr - (pointer to) DxCh common part for all formats
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertGenericToDxChPclRuleCommon
(
    IN  PRV_TGF_PCL_RULE_FORMAT_COMMON_STC      *ruleCommonPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC    *dxChRuleCommonPtr
)
{
    /* convert common part into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, macToMe);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, sourcePort);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, portListBmp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, isTagged);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, qosProfile);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, isIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, isL2Valid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, isUdbValid);
}

/*******************************************************************************
* prvTgfConvertGenericToDxChPclRuleEgrCommon
*
* DESCRIPTION:
*       Convert generic into device specific common segment of all egress key formats
*
* INPUTS:
*       ruleCommonPtr - (pointer to) common part for all formats
*
* OUTPUTS:
*       dxChRuleCommonPtr - (pointer to) DxCh common part for all formats
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChPclRuleEgrCommon
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STC      *ruleCommonPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC    *dxChRuleCommonPtr
)
{
    /* convert common part into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, isTagged);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, isIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, isL2Valid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, egrPacketType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, srcDev);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, sourceId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleCommonPtr, ruleCommonPtr, isVidx);

    dxChRuleCommonPtr->sourcePort  = ruleCommonPtr->srcPort;
    dxChRuleCommonPtr->portListBmp = ruleCommonPtr->portListBmp;

    switch (ruleCommonPtr->egrPacketType)
    {
        case 0:
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleCommonPtr->egrPktType.toCpu),
                                           &(ruleCommonPtr->egrPktType.toCpu), cpuCode);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleCommonPtr->egrPktType.toCpu),
                                           &(ruleCommonPtr->egrPktType.toCpu), srcTrg);

            break;

        case 1:
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleCommonPtr->egrPktType.fromCpu),
                                           &(ruleCommonPtr->egrPktType.fromCpu), tc);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleCommonPtr->egrPktType.fromCpu),
                                           &(ruleCommonPtr->egrPktType.fromCpu), dp);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleCommonPtr->egrPktType.fromCpu),
                                           &(ruleCommonPtr->egrPktType.fromCpu), egrFilterEnable);

            break;

        case 2:
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleCommonPtr->egrPktType.toAnalyzer),
                                           &(ruleCommonPtr->egrPktType.toAnalyzer), rxSniff);

            break;

        case 3:
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleCommonPtr->egrPktType.fwdData),
                                           &(ruleCommonPtr->egrPktType.fwdData), qosProfile);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleCommonPtr->egrPktType.fwdData),
                                           &(ruleCommonPtr->egrPktType.fwdData), srcTrunkId);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleCommonPtr->egrPktType.fwdData),
                                           &(ruleCommonPtr->egrPktType.fwdData), srcIsTrunk);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleCommonPtr->egrPktType.fwdData),
                                           &(ruleCommonPtr->egrPktType.fwdData), isUnknown);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleCommonPtr->egrPktType.fwdData),
                                           &(ruleCommonPtr->egrPktType.fwdData), isRouted);

            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChIngStdNotIp
*
* DESCRIPTION:
*       Convert generic into device specific standard not IP packet key
*
* INPUTS:
*       ruleStdNotIpPtr - (pointer to) standard not IP packet key
*
* OUTPUTS:
*       dxChRuleStdNotIpPtr - (pointer to) DxCh standard not IP packet key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChIngStdNotIp
(
    IN  PRV_TGF_PCL_RULE_FORMAT_STD_NOT_IP_STC      *ruleStdNotIpPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC    *dxChRuleStdNotIpPtr
)
{
    /* convert common part into device specific format */
    prvTgfConvertGenericToDxChPclRuleCommon(&(ruleStdNotIpPtr->common), &(dxChRuleStdNotIpPtr->common));

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdNotIpPtr, ruleStdNotIpPtr, etherType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdNotIpPtr, ruleStdNotIpPtr, l2Encap);

    dxChRuleStdNotIpPtr->isArp  = ruleStdNotIpPtr->common.isArp;
    dxChRuleStdNotIpPtr->isIpv4 = ruleStdNotIpPtr->common.isIpv4;

    /* convert macDa into device specific format */
    cpssOsMemCpy(dxChRuleStdNotIpPtr->macDa.arEther,
                 ruleStdNotIpPtr->macDa.arEther,
                 sizeof(dxChRuleStdNotIpPtr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(dxChRuleStdNotIpPtr->macSa.arEther,
                 ruleStdNotIpPtr->macSa.arEther,
                 sizeof(dxChRuleStdNotIpPtr->macSa.arEther));

    /* convert UDB into device specific format */
    cpssOsMemCpy(dxChRuleStdNotIpPtr->udb,
                 ruleStdNotIpPtr->udb,
                 sizeof(dxChRuleStdNotIpPtr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChIngStdIpL2Qos
*
* DESCRIPTION:
*       Convert generic into device specific standard L2 and QOS styled key
*
* INPUTS:
*       ruleStdIpL2QosPtr - (pointer to) standard L2 and QOS styled key
*
* OUTPUTS:
*       dxChRuleStdIpL2QosPtr - (pointer to) DxCh standard L2 and QOS styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChIngStdIpL2Qos
(
    IN  PRV_TGF_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC   *ruleStdIpL2QosPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC *dxChRuleStdIpL2QosPtr
)
{
    /* convert common part into device specific format */
    prvTgfConvertGenericToDxChPclRuleCommon(&(ruleStdIpL2QosPtr->common), &(dxChRuleStdIpL2QosPtr->common));

    /* convert commonStdIp into device specific format */
    PRV_TGF_STC_COMMON_STD_IP_FIELD_COPY_MAC(dxChRuleStdIpL2QosPtr, ruleStdIpL2QosPtr);

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdIpL2QosPtr, ruleStdIpL2QosPtr, isIpv6ExtHdrExist);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdIpL2QosPtr, ruleStdIpL2QosPtr, isIpv6HopByHop);

    dxChRuleStdIpL2QosPtr->commonStdIp.isIpv4 = ruleStdIpL2QosPtr->common.isIpv4;
    dxChRuleStdIpL2QosPtr->isArp = ruleStdIpL2QosPtr->common.isArp;

    /* convert macDa into device specific format */
    cpssOsMemCpy(dxChRuleStdIpL2QosPtr->macDa.arEther,
                 ruleStdIpL2QosPtr->macDa.arEther,
                 sizeof(dxChRuleStdIpL2QosPtr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(dxChRuleStdIpL2QosPtr->macSa.arEther,
                 ruleStdIpL2QosPtr->macSa.arEther,
                 sizeof(dxChRuleStdIpL2QosPtr->macSa.arEther));

    /* convert UDB into device specific format */
    cpssOsMemCpy(dxChRuleStdIpL2QosPtr->udb,
                 ruleStdIpL2QosPtr->udb,
                 sizeof(dxChRuleStdIpL2QosPtr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChIngStdIpv4L4
*
* DESCRIPTION:
*       Convert generic into device specific standard IPV4 packet L4 styled key
*
* INPUTS:
*       ruleStdIpv4L4Ptr - (pointer to) standard IPV4 packet L4 styled key
*
* OUTPUTS:
*       dxChRuleStdIpv4L4Ptr - (pointer to) DxCh standard IPV4 packet L4 styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChIngStdIpv4L4
(
    IN  PRV_TGF_PCL_RULE_FORMAT_STD_IPV4_L4_STC     *ruleStdIpv4L4Ptr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC   *dxChRuleStdIpv4L4Ptr
)
{
    /* convert common part into device specific format */
    prvTgfConvertGenericToDxChPclRuleCommon(&(ruleStdIpv4L4Ptr->common), &(dxChRuleStdIpv4L4Ptr->common));

    /* convert commonStdIp into device specific format */
    PRV_TGF_STC_COMMON_STD_IP_FIELD_COPY_MAC(dxChRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr);

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleStdIpv4L4Ptr->sip), &(ruleStdIpv4L4Ptr->sip), u32Ip);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleStdIpv4L4Ptr->dip), &(ruleStdIpv4L4Ptr->dip), u32Ip);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr, isBc);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr, l4Byte0);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr, l4Byte1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr, l4Byte13);

    dxChRuleStdIpv4L4Ptr->commonStdIp.isIpv4 = ruleStdIpv4L4Ptr->common.isIpv4;
    dxChRuleStdIpv4L4Ptr->isArp = ruleStdIpv4L4Ptr->common.isArp;

    /* convert UDB into device specific format */
    cpssOsMemCpy(dxChRuleStdIpv4L4Ptr->udb,
                 ruleStdIpv4L4Ptr->udb,
                 sizeof(dxChRuleStdIpv4L4Ptr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChIngStdIpv6Dip
*
* DESCRIPTION:
*       Convert generic into device specific standard IPV6 packet DIP styled key
*
* INPUTS:
*       ruleStdIpv6DipPtr - (pointer to) standard IPV6 packet DIP styled key
*
* OUTPUTS:
*       dxChRuleStdIpv6DipPtr - (pointer to) DxCh standard IPV6 packet DIP styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChIngStdIpv6Dip
(
    IN  PRV_TGF_PCL_RULE_FORMAT_STD_IPV6_DIP_STC    *ruleStdIpv6DipPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC  *dxChRuleStdIpv6DipPtr
)
{
    /* convert common part into device specific format */
    prvTgfConvertGenericToDxChPclRuleCommon(&(ruleStdIpv6DipPtr->common), &(dxChRuleStdIpv6DipPtr->common));

    /* convert commonStdIp into device specific format */
    PRV_TGF_STC_COMMON_STD_IP_FIELD_COPY_MAC(dxChRuleStdIpv6DipPtr, ruleStdIpv6DipPtr);

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdIpv6DipPtr, ruleStdIpv6DipPtr, isIpv6ExtHdrExist);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdIpv6DipPtr, ruleStdIpv6DipPtr, isIpv6HopByHop);

    dxChRuleStdIpv6DipPtr->isArp = ruleStdIpv6DipPtr->common.isArp;

    /* convert Dip into device specific format */
    cpssOsMemCpy(dxChRuleStdIpv6DipPtr->dip.u32Ip,
                 ruleStdIpv6DipPtr->dip.u32Ip,
                 sizeof(dxChRuleStdIpv6DipPtr->dip.u32Ip));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChIngStdUdb
*
* DESCRIPTION:
*       Convert generic into device specific standard UDB styled packet key
*
* INPUTS:
*       ruleStdIpv6DipPtr - (pointer to) standard UDB styled packet key
*
* OUTPUTS:
*       dxChRuleStdIpv6DipPtr - (pointer to) DxCh standard UDB styled packet key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChIngStdUdb
(
    IN  PRV_TGF_PCL_RULE_FORMAT_STD_UDB_STC                 *ruleStdUdbPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC  *dxChRuleStdUdbPtr
)
{
    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), dscpOrExp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), isL2Valid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), isUdbValid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), pktTagging);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), l3OffsetInvalid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), l4ProtocolType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), pktType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), ipHeaderOk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), macDaType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), isIpv6Eh);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), isIpv6HopByHop);

    dxChRuleStdUdbPtr->commonIngrUdb.pclId       = ruleStdUdbPtr->commonStd.pclId;
    dxChRuleStdUdbPtr->commonIngrUdb.macToMe     = ruleStdUdbPtr->commonStd.macToMe;
    dxChRuleStdUdbPtr->commonIngrUdb.sourcePort  = ruleStdUdbPtr->commonStd.sourcePort;
    dxChRuleStdUdbPtr->commonIngrUdb.portListBmp = ruleStdUdbPtr->commonStd.portListBmp;
    dxChRuleStdUdbPtr->commonIngrUdb.vid         = ruleStdUdbPtr->commonStd.vid;
    dxChRuleStdUdbPtr->commonIngrUdb.up          = ruleStdUdbPtr->commonStd.up;
    dxChRuleStdUdbPtr->commonIngrUdb.isIp        = ruleStdUdbPtr->commonStd.isIp;
    dxChRuleStdUdbPtr->commonIngrUdb.l2Encapsulation = ruleStdUdbPtr->commonUdb.l2Encap;
    dxChRuleStdUdbPtr->isIpv4                   = ruleStdUdbPtr->commonStd.isIpv4;

    dxChRuleStdUdbPtr->commonIngrUdb.l4OffsetInalid = ruleStdUdbPtr->commonUdb.l4OffsetInvalid;

    /* convert UDB into device specific format */
    cpssOsMemCpy(dxChRuleStdUdbPtr->udb,
                 ruleStdUdbPtr->udb,
                 sizeof(dxChRuleStdUdbPtr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChIngExtNotIpv6
*
* DESCRIPTION:
*       Convert generic into device specific not IP and IPV4 packet key
*
* INPUTS:
*       ruleExtNotIpv6Ptr - (pointer to) not IP and IPV4 packet key
*
* OUTPUTS:
*       dxChRuleExtNotIpv6Ptr - (pointer to) DxCh not IP and IPV4 packet key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChIngExtNotIpv6
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC    *ruleExtNotIpv6Ptr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC  *dxChRuleExtNotIpv6Ptr
)
{
    /* convert common part into device specific format */
    prvTgfConvertGenericToDxChPclRuleCommon(&(ruleExtNotIpv6Ptr->common), &(dxChRuleExtNotIpv6Ptr->common));

    /* convert commonExt into device specific format */
    PRV_TGF_STC_COMMON_EXT_FIELD_COPY_MAC(dxChRuleExtNotIpv6Ptr, ruleExtNotIpv6Ptr);

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleExtNotIpv6Ptr->sip), &(ruleExtNotIpv6Ptr->sip), u32Ip);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleExtNotIpv6Ptr->dip), &(ruleExtNotIpv6Ptr->dip), u32Ip);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleExtNotIpv6Ptr, ruleExtNotIpv6Ptr, etherType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleExtNotIpv6Ptr, ruleExtNotIpv6Ptr, l2Encap);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleExtNotIpv6Ptr, ruleExtNotIpv6Ptr, ipv4Fragmented);

    /* convert macDa into device specific format */
    cpssOsMemCpy(dxChRuleExtNotIpv6Ptr->macDa.arEther,
                 ruleExtNotIpv6Ptr->macDa.arEther,
                 sizeof(dxChRuleExtNotIpv6Ptr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(dxChRuleExtNotIpv6Ptr->macSa.arEther,
                 ruleExtNotIpv6Ptr->macSa.arEther,
                 sizeof(dxChRuleExtNotIpv6Ptr->macSa.arEther));

    /* convert UDB into device specific format */
    cpssOsMemCpy(dxChRuleExtNotIpv6Ptr->udb,
                 ruleExtNotIpv6Ptr->udb,
                 sizeof(dxChRuleExtNotIpv6Ptr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChIngRuleExtIpv6L2
*
* DESCRIPTION:
*       Convert generic into device specific extended IPV6 packet L2 styled key
*
* INPUTS:
*       ruleExtIpv6L2Ptr - (pointer to) extended IPV6 packet L2 styled key
*
* OUTPUTS:
*       dxChRuleExtIpv6L2Ptr - (pointer to) DxCh extended IPV6 packet L2 styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChIngRuleExtIpv6L2
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EXT_IPV6_L2_STC     *ruleExtIpv6L2Ptr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC   *dxChRuleExtIpv6L2Ptr
)
{
    /* convert common part into device specific format */
    prvTgfConvertGenericToDxChPclRuleCommon(&(ruleExtIpv6L2Ptr->common), &(dxChRuleExtIpv6L2Ptr->common));

    /* convert commonExt into device specific format */
    PRV_TGF_STC_COMMON_EXT_FIELD_COPY_MAC(dxChRuleExtIpv6L2Ptr, ruleExtIpv6L2Ptr);

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleExtIpv6L2Ptr, ruleExtIpv6L2Ptr, isIpv6ExtHdrExist);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleExtIpv6L2Ptr, ruleExtIpv6L2Ptr, isIpv6HopByHop);

    dxChRuleExtIpv6L2Ptr->dipBits127to120 = ruleExtIpv6L2Ptr->dipBits;

    /* convert sip into device specific format */
    cpssOsMemCpy(dxChRuleExtIpv6L2Ptr->sip.u32Ip,
                 ruleExtIpv6L2Ptr->sip.u32Ip,
                 sizeof(dxChRuleExtIpv6L2Ptr->sip.u32Ip));

    /* convert macDa into device specific format */
    cpssOsMemCpy(dxChRuleExtIpv6L2Ptr->macDa.arEther,
                 ruleExtIpv6L2Ptr->macDa.arEther,
                 sizeof(dxChRuleExtIpv6L2Ptr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(dxChRuleExtIpv6L2Ptr->macSa.arEther,
                 ruleExtIpv6L2Ptr->macSa.arEther,
                 sizeof(dxChRuleExtIpv6L2Ptr->macSa.arEther));

    /* convert UDB into device specific format */
    cpssOsMemCpy(dxChRuleExtIpv6L2Ptr->udb,
                 ruleExtIpv6L2Ptr->udb,
                 sizeof(dxChRuleExtIpv6L2Ptr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChIngRuleExtIpv6L4
*
* DESCRIPTION:
*       Convert generic into device specific extended IPV6 packet L4 styled key
*
* INPUTS:
*       ruleExtIpv6L4Ptr - (pointer to) extended IPV6 packet L4 styled key
*
* OUTPUTS:
*       dxChRuleExtIpv6L4Ptr - (pointer to) DxCh extended IPV6 packet L4 styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChIngRuleExtIpv6L4
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EXT_IPV6_L4_STC     *ruleExtIpv6L4Ptr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC   *dxChRuleExtIpv6L4Ptr
)
{
    /* convert common part into device specific format */
    prvTgfConvertGenericToDxChPclRuleCommon(&(ruleExtIpv6L4Ptr->common), &(dxChRuleExtIpv6L4Ptr->common));

    /* convert commonExt into device specific format */
    PRV_TGF_STC_COMMON_EXT_FIELD_COPY_MAC(dxChRuleExtIpv6L4Ptr, ruleExtIpv6L4Ptr);

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleExtIpv6L4Ptr, ruleExtIpv6L4Ptr, isIpv6ExtHdrExist);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleExtIpv6L4Ptr, ruleExtIpv6L4Ptr, isIpv6HopByHop);

    /* convert sip into device specific format */
    cpssOsMemCpy(dxChRuleExtIpv6L4Ptr->sip.u32Ip,
                 ruleExtIpv6L4Ptr->sip.u32Ip,
                 sizeof(dxChRuleExtIpv6L4Ptr->sip.u32Ip));

    /* convert dip into device specific format */
    cpssOsMemCpy(dxChRuleExtIpv6L4Ptr->dip.u32Ip,
                 ruleExtIpv6L4Ptr->dip.u32Ip,
                 sizeof(dxChRuleExtIpv6L4Ptr->dip.u32Ip));

    /* convert UDB into device specific format */
    cpssOsMemCpy(dxChRuleExtIpv6L4Ptr->udb,
                 ruleExtIpv6L4Ptr->udb,
                 sizeof(dxChRuleExtIpv6L4Ptr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChIngRuleExtUdb
*
* DESCRIPTION:
*       Convert generic into device specific extended UDB styled packet key
*
* INPUTS:
*       ruleExtUdbPtr - (pointer to) extended UDB styled packet key
*
* OUTPUTS:
*       dxChRuleExtUdbPtr - (pointer to) DxCh extended UDB styled packet key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChIngRuleExtUdb
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EXT_UDB_STC                 *ruleExtUdbPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC  *dxChRuleExtUdbPtr
)
{
    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), dscpOrExp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), isL2Valid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), isUdbValid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), pktTagging);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), l4ProtocolType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), pktType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), ipHeaderOk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), macDaType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), isIpv6Eh);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), isIpv6HopByHop);

    dxChRuleExtUdbPtr->commonIngrUdb.pclId       = ruleExtUdbPtr->commonStd.pclId;
    dxChRuleExtUdbPtr->commonIngrUdb.macToMe     = ruleExtUdbPtr->commonStd.macToMe;
    dxChRuleExtUdbPtr->commonIngrUdb.sourcePort  = ruleExtUdbPtr->commonStd.sourcePort;
    dxChRuleExtUdbPtr->commonIngrUdb.portListBmp = ruleExtUdbPtr->commonStd.portListBmp;
    dxChRuleExtUdbPtr->commonIngrUdb.vid         = ruleExtUdbPtr->commonStd.vid;
    dxChRuleExtUdbPtr->commonIngrUdb.up          = ruleExtUdbPtr->commonStd.up;
    dxChRuleExtUdbPtr->commonIngrUdb.isIp        = ruleExtUdbPtr->commonStd.isIp;
    dxChRuleExtUdbPtr->commonIngrUdb.l2Encapsulation = ruleExtUdbPtr->commonIngrUdb.l2Encap;
    dxChRuleExtUdbPtr->commonIngrUdb.l4OffsetInalid  = ruleExtUdbPtr->commonIngrUdb.l4OffsetInvalid;

    dxChRuleExtUdbPtr->isIpv6     = ruleExtUdbPtr->commonExt.isIpv6;
    dxChRuleExtUdbPtr->ipProtocol = ruleExtUdbPtr->commonExt.ipProtocol;

    /* convert sip into device specific format */
    cpssOsMemCpy(dxChRuleExtUdbPtr->sipBits31_0,
                 ruleExtUdbPtr->sipBits31_0,
                 sizeof(dxChRuleExtUdbPtr->sipBits31_0));

    /* convert sip into device specific format */
    cpssOsMemCpy(dxChRuleExtUdbPtr->sipBits79_32orMacSa,
                 ruleExtUdbPtr->macSaOrSipBits79_32,
                 sizeof(dxChRuleExtUdbPtr->sipBits79_32orMacSa));

    /* convert sip into device specific format */
    cpssOsMemCpy(dxChRuleExtUdbPtr->sipBits127_80orMacDa,
                 ruleExtUdbPtr->macDaOrSipBits127_80,
                 sizeof(dxChRuleExtUdbPtr->sipBits127_80orMacDa));

    /* convert dip into device specific format */
    cpssOsMemCpy(dxChRuleExtUdbPtr->dipBits127_112,
                 ruleExtUdbPtr->dipBits127_112,
                 sizeof(dxChRuleExtUdbPtr->dipBits127_112));

    /* convert dip into device specific format */
    cpssOsMemCpy(dxChRuleExtUdbPtr->dipBits31_0,
                 ruleExtUdbPtr->dipBits31_0,
                 sizeof(dxChRuleExtUdbPtr->dipBits31_0));

    /* convert UDB into device specific format */
    cpssOsMemCpy(dxChRuleExtUdbPtr->udb,
                 ruleExtUdbPtr->udb,
                 sizeof(dxChRuleExtUdbPtr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChEgrRuleStdNotIp
*
* DESCRIPTION:
*       Convert generic into device specific egress standard not IP packet key
*
* INPUTS:
*       ruleStdNotIpPtr - (pointer to) standard not IP packet key
*
* OUTPUTS:
*       dxChRuleStdNotIpPtr - (pointer to) DxCh standard not IP packet key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChEgrRuleStdNotIp
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_STD_NOT_IP_STC      *ruleStdNotIpPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_NOT_IP_STC    *dxChRuleStdNotIpPtr
)
{
    GT_STATUS   rc = GT_OK;


    /* convert common part into device specific format */
    rc = prvTgfConvertGenericToDxChPclRuleEgrCommon(&(ruleStdNotIpPtr->common),
                                                    &(dxChRuleStdNotIpPtr->common));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPclRuleEgrCommon FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdNotIpPtr, ruleStdNotIpPtr, etherType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdNotIpPtr, ruleStdNotIpPtr, l2Encap);

    dxChRuleStdNotIpPtr->isIpv4 = ruleStdNotIpPtr->common.isIpv4;
    dxChRuleStdNotIpPtr->isArp  = ruleStdNotIpPtr->common.isArp;

    /* convert macDa into device specific format */
    cpssOsMemCpy(dxChRuleStdNotIpPtr->macDa.arEther,
                 ruleStdNotIpPtr->macDa.arEther,
                 sizeof(dxChRuleStdNotIpPtr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(dxChRuleStdNotIpPtr->macSa.arEther,
                 ruleStdNotIpPtr->macSa.arEther,
                 sizeof(dxChRuleStdNotIpPtr->macSa.arEther));

    return rc;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChEgrRuleStdIpL2Qos
*
* DESCRIPTION:
*       Convert generic into device specific egress standard IPV4 and IPV6 packets
*       L2 and QOS styled key
*
* INPUTS:
*       ruleStdIpL2QosPtr - (pointer to) standard IP packets L2 and QOS styled key
*
* OUTPUTS:
*       dxChRuleStdIpL2QosPtr - (pointer to) DxCh standard IP packets L2 and QOS styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChEgrRuleStdIpL2Qos
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC   *ruleStdIpL2QosPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC *dxChRuleStdIpL2QosPtr
)
{
    GT_STATUS   rc = GT_OK;


    /* convert common part into device specific format */
    rc = prvTgfConvertGenericToDxChPclRuleEgrCommon(&(ruleStdIpL2QosPtr->common),
                                                    &(dxChRuleStdIpL2QosPtr->common));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPclRuleEgrCommon FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert commonEgrStd into device specific format */
    PRV_TGF_STC_COMMON_EGR_STD_IP_FIELD_COPY_MAC(dxChRuleStdIpL2QosPtr, ruleStdIpL2QosPtr);

    /* convert into device specific format */
    dxChRuleStdIpL2QosPtr->isArp    = ruleStdIpL2QosPtr->common.isArp;
    dxChRuleStdIpL2QosPtr->l4Byte13 = ruleStdIpL2QosPtr->commonStdIp.l4Byte13;

    /* convert macDa into device specific format */
    cpssOsMemCpy(dxChRuleStdIpL2QosPtr->macDa.arEther,
                 ruleStdIpL2QosPtr->macDa.arEther,
                 sizeof(dxChRuleStdIpL2QosPtr->macDa.arEther));

    /* convert Sip into device specific format */
    cpssOsMemCpy(dxChRuleStdIpL2QosPtr->dipBits0to31,
                 ruleStdIpL2QosPtr->commonStdIp.dipBits,
                 sizeof(dxChRuleStdIpL2QosPtr->dipBits0to31));

    return rc;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChEgrRuleStdIpv4L4
*
* DESCRIPTION:
*       Convert generic into device specific egress standard IPV4 packet L4 styled key
*
* INPUTS:
*       ruleStdIpv4L4Ptr - (pointer to) standard IPV4 packet L4 styled key
*
* OUTPUTS:
*       dxChRuleStdIpv4L4Ptr - (pointer to) DxCh standard IPV4 packet L4 styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChEgrRuleStdIpv4L4
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC     *ruleStdIpv4L4Ptr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC   *dxChRuleStdIpv4L4Ptr
)
{
    GT_STATUS   rc = GT_OK;


    /* convert common part into device specific format */
    rc = prvTgfConvertGenericToDxChPclRuleEgrCommon(&(ruleStdIpv4L4Ptr->common),
                                                    &(dxChRuleStdIpv4L4Ptr->common));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPclRuleEgrCommon FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert commonEgrStd into device specific format */
    PRV_TGF_STC_COMMON_EGR_STD_IP_FIELD_COPY_MAC(dxChRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr);

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr, isBc);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr, l4Byte0);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr, l4Byte1);

    dxChRuleStdIpv4L4Ptr->isArp     = ruleStdIpv4L4Ptr->common.isArp;
    dxChRuleStdIpv4L4Ptr->l4Byte13  = ruleStdIpv4L4Ptr->commonStdIp.l4Byte13;
    dxChRuleStdIpv4L4Ptr->dip.u32Ip = ruleStdIpv4L4Ptr->dip.u32Ip;
    dxChRuleStdIpv4L4Ptr->sip.u32Ip = ruleStdIpv4L4Ptr->sip.u32Ip;

    return rc;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChEgrRuleExtNotIpv6
*
* DESCRIPTION:
*       Convert generic into device specific egress extended not IP and IPV4 packet key
*
* INPUTS:
*       ruleExtNotIpv6Ptr - (pointer to) extended not IP and IPV4 packet key
*
* OUTPUTS:
*       dxChRuleExtNotIpv6Ptr - (pointer to) DxCh extended not IP and IPV4 packet key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChEgrRuleExtNotIpv6
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC    *ruleExtNotIpv6Ptr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC  *dxChRuleExtNotIpv6Ptr
)
{
    GT_STATUS   rc = GT_OK;


    /* convert common part into device specific format */
    rc = prvTgfConvertGenericToDxChPclRuleEgrCommon(&(ruleExtNotIpv6Ptr->common),
                                                    &(dxChRuleExtNotIpv6Ptr->common));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPclRuleEgrCommon FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert commonEgrExt into device specific format */
    PRV_TGF_STC_COMMON_EGR_EXT_FIELD_COPY_MAC(dxChRuleExtNotIpv6Ptr, ruleExtNotIpv6Ptr);

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleExtNotIpv6Ptr, ruleExtNotIpv6Ptr, etherType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleExtNotIpv6Ptr, ruleExtNotIpv6Ptr, l2Encap);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleExtNotIpv6Ptr, ruleExtNotIpv6Ptr, ipv4Fragmented);

    dxChRuleExtNotIpv6Ptr->dip.u32Ip = ruleExtNotIpv6Ptr->dip.u32Ip;
    dxChRuleExtNotIpv6Ptr->sip.u32Ip = ruleExtNotIpv6Ptr->sip.u32Ip;

    /* convert macDa into device specific format */
    cpssOsMemCpy(dxChRuleExtNotIpv6Ptr->macDa.arEther,
                 ruleExtNotIpv6Ptr->macDa.arEther,
                 sizeof(dxChRuleExtNotIpv6Ptr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(dxChRuleExtNotIpv6Ptr->macSa.arEther,
                 ruleExtNotIpv6Ptr->macSa.arEther,
                 sizeof(dxChRuleExtNotIpv6Ptr->macSa.arEther));

    return rc;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChEgrRuleExtIpv6L2
*
* DESCRIPTION:
*       Convert generic into device specific egress extended IPV6 packet L2 styled key
*
* INPUTS:
*       ruleExtIpv6L2Ptr - (pointer to) extended IPV6 packet L2 styled key
*
* OUTPUTS:
*       dxChRuleExtIpv6L2Ptr - (pointer to) DxCh extended IPV6 packet L2 styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChEgrRuleExtIpv6L2
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC     *ruleExtIpv6L2Ptr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC   *dxChRuleExtIpv6L2Ptr
)
{
    GT_STATUS   rc = GT_OK;


    /* convert common part into device specific format */
    rc = prvTgfConvertGenericToDxChPclRuleEgrCommon(&(ruleExtIpv6L2Ptr->common),
                                                    &(dxChRuleExtIpv6L2Ptr->common));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPclRuleEgrCommon FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert commonEgrExt into device specific format */
    PRV_TGF_STC_COMMON_EGR_EXT_FIELD_COPY_MAC(dxChRuleExtIpv6L2Ptr, ruleExtIpv6L2Ptr);

    /* convert into device specific format */
    dxChRuleExtIpv6L2Ptr->dipBits127to120 = ruleExtIpv6L2Ptr->dipBits;

    /* convert Sip into device specific format */
    cpssOsMemCpy(dxChRuleExtIpv6L2Ptr->sip.u32Ip,
                 ruleExtIpv6L2Ptr->sip.u32Ip,
                 sizeof(dxChRuleExtIpv6L2Ptr->sip.u32Ip));

    /* convert macDa into device specific format */
    cpssOsMemCpy(dxChRuleExtIpv6L2Ptr->macDa.arEther,
                 ruleExtIpv6L2Ptr->macDa.arEther,
                 sizeof(dxChRuleExtIpv6L2Ptr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(dxChRuleExtIpv6L2Ptr->macSa.arEther,
                 ruleExtIpv6L2Ptr->macSa.arEther,
                 sizeof(dxChRuleExtIpv6L2Ptr->macSa.arEther));

    return rc;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChEgrRuleExtIpv6L4
*
* DESCRIPTION:
*       Convert generic into device specific egress extended IPV6 packet L4 styled key
*
* INPUTS:
*       ruleExtIpv6L4Ptr - (pointer to) extended IPV6 packet L4 styled key
*
* OUTPUTS:
*       dxChRuleExtIpv6L4Ptr - (pointer to) DxCh extended IPV6 packet L4 styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChEgrRuleExtIpv6L4
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_EXT_IPV6_L4_STC     *ruleExtIpv6L4Ptr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L4_STC   *dxChRuleExtIpv6L4Ptr
)
{
    GT_STATUS   rc = GT_OK;


    /* convert common part into device specific format */
    rc = prvTgfConvertGenericToDxChPclRuleEgrCommon(&(ruleExtIpv6L4Ptr->common),
                                                    &(dxChRuleExtIpv6L4Ptr->common));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPclRuleEgrCommon FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert commonEgrExt into device specific format */
    PRV_TGF_STC_COMMON_EGR_EXT_FIELD_COPY_MAC(dxChRuleExtIpv6L4Ptr, ruleExtIpv6L4Ptr);

    /* convert Sip into device specific format */
    cpssOsMemCpy(dxChRuleExtIpv6L4Ptr->sip.u32Ip,
                 ruleExtIpv6L4Ptr->sip.u32Ip,
                 sizeof(dxChRuleExtIpv6L4Ptr->sip.u32Ip));

    /* convert Dip into device specific format */
    cpssOsMemCpy(dxChRuleExtIpv6L4Ptr->dip.u32Ip,
                 ruleExtIpv6L4Ptr->dip.u32Ip,
                 sizeof(dxChRuleExtIpv6L4Ptr->dip.u32Ip));

    return rc;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChLookupConfig
*
* DESCRIPTION:
*       Convert generic into device specific configuration of IPCL lookups
*
* INPUTS:
*       lookupConfigPtr - (pointer to) configuration of IPCL lookups
*
* OUTPUTS:
*       dxChLookupConfigPtr - (pointer to) DxCh configuration of IPCL lookups
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChLookupConfig
(
    IN  PRV_TGF_PCL_ACTION_LOOKUP_CONFIG_STC    *lookupConfigPtr,
    OUT CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC  *dxChLookupConfigPtr
)
{
    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChLookupConfigPtr, lookupConfigPtr, ipclConfigIndex);

    /* convert pcl0_1OverrideConfigIndex into device specific format */
    switch (lookupConfigPtr->pcl0_1OverrideConfigIndex)
    {
        case PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E:
            dxChLookupConfigPtr->pcl0_1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
            break;

        case PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E:
            dxChLookupConfigPtr->pcl0_1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert pcl1OverrideConfigIndex into device specific format */
    switch (lookupConfigPtr->pcl1OverrideConfigIndex)
    {
        case PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E:
            dxChLookupConfigPtr->pcl1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
            break;

        case PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E:
            dxChLookupConfigPtr->pcl1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChQos
*
* DESCRIPTION:
*       Convert generic into device specific QoS attributes mark actions
*
* INPUTS:
*       lookupConfigPtr - (pointer to) QoS attributes mark actions
*       isEgressPolicy  - action is used for the Ingress/Egress Policy
 *
* OUTPUTS:
*       dxChLookupConfigPtr - (pointer to) DxCh QoS attributes mark actions
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertGenericToDxChQos
(
    IN  PRV_TGF_PCL_ACTION_QOS_STC      *qosAttributesPtr,
    IN  GT_BOOL                          isEgressPolicy,
    OUT CPSS_DXCH_PCL_ACTION_QOS_STC    *dxChQosAttributesPtr
)
{
    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChQosAttributesPtr, qosAttributesPtr, modifyDscp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChQosAttributesPtr, qosAttributesPtr, modifyUp);

    if (GT_TRUE == isEgressPolicy)
    {
        /* convert egress members into device specific format */
        dxChQosAttributesPtr->qos.egress.dscp = (GT_U8) (qosAttributesPtr->dscp);
        dxChQosAttributesPtr->qos.egress.up   = (GT_U8) (qosAttributesPtr->up);
    }
    else
    {
        /* convert ingress members into device specific format */
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChQosAttributesPtr->qos.ingress),
                                       qosAttributesPtr, profileIndex);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChQosAttributesPtr->qos.ingress),
                                       qosAttributesPtr, profileAssignIndex);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChQosAttributesPtr->qos.ingress),
                                       qosAttributesPtr, profilePrecedence);
    }
}

/*******************************************************************************
* prvTgfConvertGenericToDxChRedirect
*
* DESCRIPTION:
*       Convert generic into device specific redirection related actions
*
* INPUTS:
*       redirectPtr - (pointer to) redirection related actions
*
* OUTPUTS:
*       dxChRedirectPtr - (pointer to) DxCh redirection related actions
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChRedirect
(
    IN  PRV_TGF_PCL_ACTION_REDIRECT_STC     *redirectPtr,
    OUT CPSS_DXCH_PCL_ACTION_REDIRECT_STC   *dxChRedirectPtr
)
{
    cpssOsMemSet(dxChRedirectPtr, 0, sizeof(*dxChRedirectPtr));

    /* convert redirectCmd into device specific format */
    switch (redirectPtr->redirectCmd)
    {
        case PRV_TGF_PCL_ACTION_REDIRECT_CMD_NONE_E:
            dxChRedirectPtr->redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E;
            break;

        case PRV_TGF_PCL_ACTION_REDIRECT_CMD_OUT_IF_E:
            dxChRedirectPtr->redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
            /* convert into device specific format */
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRedirectPtr->data.outIf),
                                           &(redirectPtr->data.outIf), vntL2Echo);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRedirectPtr->data.outIf),
                                           &(redirectPtr->data.outIf), modifyMacDa);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRedirectPtr->data.outIf),
                                           &(redirectPtr->data.outIf), modifyMacSa);

            /* convert outInterface into device specific format */
            cpssOsMemCpy(&(dxChRedirectPtr->data.outIf.outInterface),
                         &(redirectPtr->data.outIf.outInterface),
                         sizeof(dxChRedirectPtr->data.outIf.outInterface));

            switch (redirectPtr->data.outIf.outlifType)
            {
                case PRV_TGF_OUTLIF_TYPE_LL_E:
                    dxChRedirectPtr->data.outIf.arpPtr =
                        redirectPtr->data.outIf.outlifPointer.arpPtr;

                    break;
                case PRV_TGF_OUTLIF_TYPE_TUNNEL_E:
                    dxChRedirectPtr->data.outIf.tunnelStart = GT_TRUE;
                    dxChRedirectPtr->data.outIf.tunnelPtr   =
                        redirectPtr->data.outIf.outlifPointer.tunnelStartPtr.ptr;
                    /* convert tunnelType into device specific format */
                    switch (redirectPtr->data.outIf.outlifPointer.tunnelStartPtr.tunnelType)
                    {
                        case PRV_TGF_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E:
                            dxChRedirectPtr->data.outIf.tunnelType =
                                CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E;
                            break;

                        case PRV_TGF_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E:
                            dxChRedirectPtr->data.outIf.tunnelType =
                                CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E;
                            break;

                        default:
                            return GT_BAD_PARAM;
                    }
                    break;
                /* DIT - Dowstream interface - not supported for DX devices */
                case PRV_TGF_OUTLIF_TYPE_DIT_E:
                default:
                    return GT_BAD_PARAM;
            }

            break;

        case PRV_TGF_PCL_ACTION_REDIRECT_CMD_ROUTER_E:
            dxChRedirectPtr->redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E;

            /* convert into device specific format */
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRedirectPtr->data),
                                           &(redirectPtr->data), routerLttIndex);

            break;

        case PRV_TGF_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E:
            dxChRedirectPtr->redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E;

            /* convert into device specific format */
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRedirectPtr->data),
                                           &(redirectPtr->data), vrfId);

            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChPolicer
*
* DESCRIPTION:
*       Convert generic into device specific policer related actions
*
* INPUTS:
*       policerPtr - (pointer to) policer related actions
*
* OUTPUTS:
*       dxChPolicerPtr - (pointer to) DxCh policer related actions
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChPolicer
(
    IN  PRV_TGF_PCL_ACTION_POLICER_STC      *policerPtr,
    OUT CPSS_DXCH_PCL_ACTION_POLICER_STC    *dxChPolicerPtr
)
{
    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChPolicerPtr, policerPtr, policerId);

    /* convert policerEnable into device specific format */
    switch (policerPtr->policerEnable)
    {
        case PRV_TGF_PCL_POLICER_DISABLE_ALL_E:
            dxChPolicerPtr->policerEnable = CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E;
            break;

        case PRV_TGF_PCL_POLICER_ENABLE_METER_AND_COUNTER_E:
            dxChPolicerPtr->policerEnable = CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E;
            break;

        case PRV_TGF_PCL_POLICER_ENABLE_METER_ONLY_E:
            dxChPolicerPtr->policerEnable = CPSS_DXCH_PCL_POLICER_ENABLE_METER_ONLY_E;
            break;

        case PRV_TGF_PCL_POLICER_ENABLE_COUNTER_ONLY_E:
            dxChPolicerPtr->policerEnable = CPSS_DXCH_PCL_POLICER_ENABLE_COUNTER_ONLY_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChRuleAction
*
* DESCRIPTION:
*       Convert generic into device specific Policy Engine Action
*
* INPUTS:
*       ruleActionPtr - (pointer to) Policy Engine Action
*
* OUTPUTS:
*       dxChRuleActionPtr - (pointer to) DxCh Policy Engine Action
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertGenericToDxChRuleAction
(
    IN  PRV_TGF_PCL_ACTION_STC      *ruleActionPtr,
    OUT CPSS_DXCH_PCL_ACTION_STC    *dxChRuleActionPtr
)
{
    GT_STATUS   rc = GT_OK;

    cpssOsMemSet(dxChRuleActionPtr,0,sizeof(*dxChRuleActionPtr));

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleActionPtr, ruleActionPtr, pktCmd);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleActionPtr, ruleActionPtr, actionStop);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleActionPtr, ruleActionPtr, bypassBridge);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleActionPtr, ruleActionPtr, bypassIngressPipe);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChRuleActionPtr, ruleActionPtr, egressPolicy);

    /* convert lookupConfig into device specific format */
    rc = prvTgfConvertGenericToDxChLookupConfig(&(ruleActionPtr->lookupConfig),
                                                &(dxChRuleActionPtr->lookupConfig));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChLookupConfig FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert mirroring related actions into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->mirror),
                                   &(ruleActionPtr->mirror), cpuCode);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->mirror),
                                   &(ruleActionPtr->mirror), mirrorToRxAnalyzerPort);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->mirror),
                                   &(ruleActionPtr->mirror), mirrorTcpRstAndFinPacketsToCpu);

    /* convert rule match counter into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->matchCounter),
                                   &(ruleActionPtr->matchCounter), enableMatchCount);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->matchCounter),
                                   &(ruleActionPtr->matchCounter), matchCounterIndex);

    /* convert QoS attributes mark actions into device specific format */
    prvTgfConvertGenericToDxChQos(&(ruleActionPtr->qos),
                                  ruleActionPtr->egressPolicy,
                                  &(dxChRuleActionPtr->qos));

    /* convert redirection related actions into device specific format */
    rc = prvTgfConvertGenericToDxChRedirect(&(ruleActionPtr->redirect),
                                            &(dxChRuleActionPtr->redirect));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChRedirect FAILED, rc = [%d]", rc);

        return rc;
    }

    if( CPSS_INTERFACE_PORT_E == ruleActionPtr->redirect.data.outIf.outInterface.type )
    {
        rc = prvUtfHwFromSwDeviceNumberGet(ruleActionPtr->redirect.data.outIf.outInterface.devPort.devNum,
                                           &(dxChRuleActionPtr->redirect.data.outIf.outInterface.devPort.devNum));
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC(
                "[TGF]: prvUtfHwFromSwDeviceNumberGet FAILED, rc = [%d]", rc);

            return rc;
        }
    }

    /* convert policer related actions into device specific format */
    rc = prvTgfConvertGenericToDxChPolicer(&(ruleActionPtr->policer),
                                            &(dxChRuleActionPtr->policer));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChPolicer FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert VLAN modification configuration into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->vlan),
                                   &(ruleActionPtr->vlan), modifyVlan);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->vlan),
                                   &(ruleActionPtr->vlan), nestedVlan);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->vlan),
                                   &(ruleActionPtr->vlan), vlanId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->vlan),
                                   &(ruleActionPtr->vlan), precedence);

    /* convert Ip Unicast Route into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->ipUcRoute),
                                   &(ruleActionPtr->ipUcRoute), doIpUcRoute);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->ipUcRoute),
                                   &(ruleActionPtr->ipUcRoute), arpDaIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->ipUcRoute),
                                   &(ruleActionPtr->ipUcRoute), decrementTTL);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->ipUcRoute),
                                   &(ruleActionPtr->ipUcRoute), bypassTTLCheck);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->ipUcRoute),
                                   &(ruleActionPtr->ipUcRoute), icmpRedirectCheck);

    /* convert packet source Id assignment into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->sourceId),
                                   &(ruleActionPtr->sourceId), assignSourceId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChRuleActionPtr->sourceId),
                                   &(ruleActionPtr->sourceId), sourceIdValue);

    return rc;
}
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToExMxPmPclRuleCommon
*
* DESCRIPTION:
*       Convert generic into device specific common segment of all key formats
*
* INPUTS:
*       ruleCommonPtr - (pointer to) common part for all formats
*
* OUTPUTS:
*       exMxPmRuleCommonPtr    - (pointer to) ExMxPm common part for all formats
*       exMxPmRuleCommonPktPtr - (pointer to) ExMxPm common part for all formats
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertGenericToExMxPmPclRuleCommon
(
    IN  PRV_TGF_PCL_RULE_FORMAT_COMMON_STC                                  *ruleCommonPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_INGRESS_STANDARD_STC             *exMxPmRuleCommonPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_INGRESS_STANDARD_PKT_TYPE_STC    *exMxPmRuleCommonPktPtr
)
{
    /* convert common part into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, macToMe);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, isIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, isIpv4);

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonPtr, isTagged);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonPtr, isArp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonPtr, isUdbValid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonPtr, isL2Valid);

    exMxPmRuleCommonPtr->srcPort = ruleCommonPtr->sourcePort;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmPclRuleCommonExt
*
* DESCRIPTION:
*       Convert generic into device specific extended common segment of all key formats
*
* INPUTS:
*       ruleCommonPtr    - (pointer to) common part for all formats
*       ruleCommonExtPtr - (pointer to) common extended part for all formats
*
* OUTPUTS:
*       exMxPmRuleCommonPtr    - (pointer to) ExMxPm common part for all formats
*       exMxPmRuleCommonPktPtr - (pointer to) ExMxPm common part for all formats
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertGenericToExMxPmPclRuleCommonExt
(
    IN  PRV_TGF_PCL_RULE_FORMAT_COMMON_STC                                  *ruleCommonPtr,
    IN  PRV_TGF_PCL_RULE_FORMAT_COMMON_EXT_STC                              *ruleCommonExtPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_INGRESS_EXTENDED_STC             *exMxPmRuleCommonPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_INGRESS_EXTENDED_PKT_TYPE_STC    *exMxPmRuleCommonPktPtr
)
{
    /* convert common part into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, macToMe);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, isIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonPtr, isL2Valid);

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonExtPtr, ipProtocol);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonExtPtr, isIpv6);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonExtPtr, isUdbValid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonExtPtr, isTagged);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonExtPtr, ipHeaderOk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonExtPtr, dscp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonExtPtr, isL4Valid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonExtPtr, l4Byte0);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonExtPtr, l4Byte1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonExtPtr, l4Byte2);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonExtPtr, l4Byte3);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPktPtr, ruleCommonExtPtr, l4Byte13);

    exMxPmRuleCommonPtr->srcPort = ruleCommonPtr->sourcePort;

    cpssOsMemCpy(exMxPmRuleCommonPktPtr->sipBits31_0,
                 ruleCommonExtPtr->sipBits,
                 sizeof(exMxPmRuleCommonPktPtr->sipBits31_0));
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmPclRuleEgrCommon
*
* DESCRIPTION:
*       Convert generic into device specific common segment of all egress key formats
*
* INPUTS:
*       ruleCommonPtr - (pointer to) common part for all formats
*
* OUTPUTS:
*       exMxPmRuleCommonPtr    - (pointer to) ExMxPm common part for all formats
*       exMxPmRuleCommonStdPtr - (pointer to) ExMxPm common std part for all formats
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmPclRuleEgrCommon
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STC                  *ruleCommonPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_EGRESS_STC           *exMxPmRuleCommonPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_EGRESS_STANDARD_STC  *exMxPmRuleCommonStdPtr
)
{
    /* convert common part into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, isMpls);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, srcPort);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, isTagged);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, isIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonStdPtr, ruleCommonPtr, isIpv4);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonStdPtr, ruleCommonPtr, isArp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonStdPtr, ruleCommonPtr, isL2Valid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonStdPtr, ruleCommonPtr, sourceId);

    exMxPmRuleCommonStdPtr->packetCmd = ruleCommonPtr->egrPacketType;

    switch (ruleCommonPtr->egrPacketType)
    {
        case 0:
            exMxPmRuleCommonStdPtr->trgOrCpuCode0_6OrCosOrSniff = (GT_U8) (ruleCommonPtr->egrPktType.toCpu.cpuCode & 0x7F);
            exMxPmRuleCommonStdPtr->srcTrgOrTxMirror            = ruleCommonPtr->egrPktType.toCpu.srcTrg;
            exMxPmRuleCommonStdPtr->srcInfo                     = ruleCommonPtr->srcDev;
            exMxPmRuleCommonStdPtr->egrFilterRegOrCpuCode7      = (GT_U8) (ruleCommonPtr->egrPktType.toCpu.cpuCode >> 7);

            break;

        case 1:
            exMxPmRuleCommonStdPtr->trgOrCpuCode0_6OrCosOrSniff = (GT_U8) ((ruleCommonPtr->egrPktType.fromCpu.dp & 3) |
                                                                  (ruleCommonPtr->egrPktType.fromCpu.tc & 7 << 2) |
                                                                  (ruleCommonPtr->egrPktType.fromCpu.egrFilterEnable & 7 << 5));
            exMxPmRuleCommonStdPtr->srcInfo                     = ruleCommonPtr->srcDev;
            exMxPmRuleCommonStdPtr->egrFilterRegOrCpuCode7      = ruleCommonPtr->egrPktType.fromCpu.egrFilterEnable;

            break;

        case 2:
            exMxPmRuleCommonStdPtr->trgOrCpuCode0_6OrCosOrSniff = ruleCommonPtr->egrPktType.toAnalyzer.rxSniff;
            exMxPmRuleCommonStdPtr->srcInfo                     = ruleCommonPtr->srcDev;

            break;

        case 3:
            exMxPmRuleCommonStdPtr->trgOrCpuCode0_6OrCosOrSniff = ruleCommonPtr->egrPktType.fwdData.srcTrunkId;
            exMxPmRuleCommonStdPtr->srcInfo                     = ruleCommonPtr->srcDev;
            exMxPmRuleCommonStdPtr->srcIsTrunk                  = ruleCommonPtr->egrPktType.fwdData.srcIsTrunk;
            exMxPmRuleCommonStdPtr->isIpOrMplsRouted            = ruleCommonPtr->egrPktType.fwdData.isRouted;

            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmPclRuleEgrStdIp
*
* DESCRIPTION:
*       Convert generic into device specific egress standard key formats
*
* INPUTS:
*       commonEgrStdIpPtr - (pointer to) common part for all standard IP formats
*
* OUTPUTS:
*       exMxPmCommonEgrStdIpPtr    - (pointer to) ExMxPm common part for all IP formats
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertGenericToExMxPmPclRuleEgrStdIp
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC               *commonEgrStdIpPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_EGRESS_STANDARD_IP_STC   *exMxPmCommonEgrStdIpPtr
)
{
    /* convert common part into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmCommonEgrStdIpPtr, commonEgrStdIpPtr, ipProtocol);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmCommonEgrStdIpPtr, commonEgrStdIpPtr, dscp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmCommonEgrStdIpPtr, commonEgrStdIpPtr, isL4Valid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmCommonEgrStdIpPtr, commonEgrStdIpPtr, l4Byte2);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmCommonEgrStdIpPtr, commonEgrStdIpPtr, l4Byte3);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmCommonEgrStdIpPtr, commonEgrStdIpPtr, l4Byte13);
    exMxPmCommonEgrStdIpPtr->tcpUdpPortComparators = commonEgrStdIpPtr->egrTcpUdpPortComparator;

    exMxPmCommonEgrStdIpPtr->isIpv4Fragment   = commonEgrStdIpPtr->ipv4Fragmented;
    exMxPmCommonEgrStdIpPtr->tosBits1_0_Extrn = commonEgrStdIpPtr->tosBits;

    /* convert dipBits into device specific format */
    cpssOsMemCpy(exMxPmCommonEgrStdIpPtr->dipBits31_0,
                 commonEgrStdIpPtr->dipBits,
                 sizeof(exMxPmCommonEgrStdIpPtr->dipBits31_0));
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmPclRuleEgrCommonExt
*
* DESCRIPTION:
*       Convert generic into device specific extended common segment of all key formats
*
* INPUTS:
*       ruleCommonPtr    - (pointer to) common part for all formats
*       ruleCommonExtPtr - (pointer to) common extended part for all formats
*
* OUTPUTS:
*       exMxPmRuleCommonPtr    - (pointer to) ExMxPm common part for all formats
*       exMxPmRuleExtCommonPtr - (pointer to) ExMxPm common ext part for all formats
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmPclRuleEgrCommonExt
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STC                  *ruleCommonPtr,
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC              *ruleCommonExtPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_EGRESS_STC           *exMxPmRuleCommonPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_EGRESS_EXTENDED_STC  *exMxPmRuleExtCommonPtr

)
{
    /* convert common part into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, isMpls);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, srcPort);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, isTagged);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleCommonPtr, ruleCommonPtr, isIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtCommonPtr, ruleCommonPtr, isArp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtCommonPtr, ruleCommonPtr, isL2Valid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtCommonPtr, ruleCommonPtr, sourceId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtCommonPtr, ruleCommonExtPtr, isIpv6);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtCommonPtr, ruleCommonExtPtr, ipProtocol);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtCommonPtr, ruleCommonExtPtr, dscp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtCommonPtr, ruleCommonExtPtr, isL4Valid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtCommonPtr, ruleCommonExtPtr, l4Byte0);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtCommonPtr, ruleCommonExtPtr, l4Byte1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtCommonPtr, ruleCommonExtPtr, l4Byte2);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtCommonPtr, ruleCommonExtPtr, l4Byte3);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtCommonPtr, ruleCommonExtPtr, l4Byte13);

    exMxPmRuleExtCommonPtr->packetCmd = ruleCommonPtr->egrPacketType;

    switch (ruleCommonPtr->egrPacketType)
    {
        case 0:
            exMxPmRuleExtCommonPtr->trgOrCpuCode0_6OrCosOrSniff = (GT_U8) (ruleCommonPtr->egrPktType.toCpu.cpuCode & 0x7F);
            exMxPmRuleExtCommonPtr->srcTrgOrTxMirror            = ruleCommonPtr->egrPktType.toCpu.srcTrg;
            exMxPmRuleExtCommonPtr->srcInfo                     = ruleCommonPtr->srcDev;
            exMxPmRuleExtCommonPtr->egrFilterRegOrCpuCode7      = (GT_U8) (ruleCommonPtr->egrPktType.toCpu.cpuCode >> 7);

            break;

        case 1:
            exMxPmRuleExtCommonPtr->trgOrCpuCode0_6OrCosOrSniff = (GT_U8) ((ruleCommonPtr->egrPktType.fromCpu.dp & 3) |
                                                                  (ruleCommonPtr->egrPktType.fromCpu.tc & 7 << 2) |
                                                                  (ruleCommonPtr->egrPktType.fromCpu.egrFilterEnable & 7 << 5));
            exMxPmRuleExtCommonPtr->srcInfo                     = ruleCommonPtr->srcDev;
            exMxPmRuleExtCommonPtr->egrFilterRegOrCpuCode7      = ruleCommonPtr->egrPktType.fromCpu.egrFilterEnable;

            break;

        case 2:
            exMxPmRuleExtCommonPtr->trgOrCpuCode0_6OrCosOrSniff = ruleCommonPtr->egrPktType.toAnalyzer.rxSniff;
            exMxPmRuleExtCommonPtr->srcInfo                     = ruleCommonPtr->srcDev;

            break;

        case 3:
            exMxPmRuleExtCommonPtr->trgOrCpuCode0_6OrCosOrSniff = ruleCommonPtr->egrPktType.fwdData.srcTrunkId;
            exMxPmRuleExtCommonPtr->srcInfo                     = ruleCommonPtr->srcDev;
            exMxPmRuleExtCommonPtr->srcIsTrunk                  = ruleCommonPtr->egrPktType.fwdData.srcIsTrunk;
            exMxPmRuleExtCommonPtr->isIpOrMplsRouted            = ruleCommonPtr->egrPktType.fwdData.isRouted;

            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmIngStdNotIp
*
* DESCRIPTION:
*       Convert generic into device specific standard not IP packet key
*
* INPUTS:
*       ruleStdNotIpPtr - (pointer to) standard not IP packet key
*
* OUTPUTS:
*       exMxPmRuleStdNotIpPtr - (pointer to) ExMxPm standard not IP packet key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmIngStdNotIp
(
    IN  PRV_TGF_PCL_RULE_FORMAT_STD_NOT_IP_STC                  *ruleStdNotIpPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_STC *exMxPmRuleStdNotIpPtr
)
{
    /* convert common part into device specific format */
    prvTgfConvertGenericToExMxPmPclRuleCommon(&(ruleStdNotIpPtr->common),
                                              &(exMxPmRuleStdNotIpPtr->commonIngrStd),
                                              &(exMxPmRuleStdNotIpPtr->commonIngrStdPkt));

    /* convert into device specific format */
    exMxPmRuleStdNotIpPtr->l2EncapType = ruleStdNotIpPtr->l2Encap;
    exMxPmRuleStdNotIpPtr->ethType     = ruleStdNotIpPtr->etherType;
    exMxPmRuleStdNotIpPtr->udb15Dup    = ruleStdNotIpPtr->udb15Dup;

    /* convert macDa into device specific format */
    cpssOsMemCpy(exMxPmRuleStdNotIpPtr->macDa.arEther,
                 ruleStdNotIpPtr->macDa.arEther,
                 sizeof(exMxPmRuleStdNotIpPtr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(exMxPmRuleStdNotIpPtr->macSa.arEther,
                 ruleStdNotIpPtr->macSa.arEther,
                 sizeof(exMxPmRuleStdNotIpPtr->macSa.arEther));

    /* convert UDB into device specific format */
    cpssOsMemCpy(exMxPmRuleStdNotIpPtr->udb,
                 ruleStdNotIpPtr->udb,
                 sizeof(exMxPmRuleStdNotIpPtr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmIngStdIpL2Qos
*
* DESCRIPTION:
*       Convert generic into device specific standard L2 and QOS styled key
*
* INPUTS:
*       ruleStdIpL2QosPtr - (pointer to) standard L2 and QOS styled key
*
* OUTPUTS:
*       exMxPmRuleStdIpL2QosPtr - (pointer to) ExMxPm standard L2 and QOS styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmIngStdIpL2Qos
(
    IN  PRV_TGF_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC                   *ruleStdIpL2QosPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_STC  *exMxPmRuleStdIpL2QosPtr
)
{
    /* convert common part into device specific format */
    prvTgfConvertGenericToExMxPmPclRuleCommon(&(ruleStdIpL2QosPtr->common),
                                              &(exMxPmRuleStdIpL2QosPtr->commonIngrStd),
                                              &(exMxPmRuleStdIpL2QosPtr->commonIngrStdPkt));

    /* convert commonIngrStdIp into device specific format */
    PRV_TGF_STC_COMMON_STD_IP_FIELD_COPY_MAC(exMxPmRuleStdIpL2QosPtr, ruleStdIpL2QosPtr);

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleStdIpL2QosPtr, ruleStdIpL2QosPtr, isIpv6HopByHop);

    exMxPmRuleStdIpL2QosPtr->isIpv6Eh = ruleStdIpL2QosPtr->isIpv6ExtHdrExist;

    /* convert macDa into device specific format */
    cpssOsMemCpy(exMxPmRuleStdIpL2QosPtr->macDa.arEther,
                 ruleStdIpL2QosPtr->macDa.arEther,
                 sizeof(exMxPmRuleStdIpL2QosPtr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(exMxPmRuleStdIpL2QosPtr->macSa.arEther,
                 ruleStdIpL2QosPtr->macSa.arEther,
                 sizeof(exMxPmRuleStdIpL2QosPtr->macSa.arEther));

    /* convert UDB into device specific format */
    cpssOsMemCpy(exMxPmRuleStdIpL2QosPtr->udb,
                 ruleStdIpL2QosPtr->udb,
                 sizeof(exMxPmRuleStdIpL2QosPtr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmIngStdIpv4L4
*
* DESCRIPTION:
*       Convert generic into device specific standard IPV4 packet L4 styled key
*
* INPUTS:
*       ruleStdIpv4L4Ptr - (pointer to) standard IPV4 packet L4 styled key
*
* OUTPUTS:
*       exMxPmRuleStdIpv4L4Ptr - (pointer to) ExMxPm standard IPV4 packet L4 styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmIngStdIpv4L4
(
    IN  PRV_TGF_PCL_RULE_FORMAT_STD_IPV4_L4_STC                     *ruleStdIpv4L4Ptr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_STC    *exMxPmRuleStdIpv4L4Ptr
)
{
    /* convert common part into device specific format */
    prvTgfConvertGenericToExMxPmPclRuleCommon(&(ruleStdIpv4L4Ptr->common),
                                              &(exMxPmRuleStdIpv4L4Ptr->commonIngrStd),
                                              &(exMxPmRuleStdIpv4L4Ptr->commonIngrStdPkt));

    /* convert commonIngrStdIp into device specific format */
    PRV_TGF_STC_COMMON_STD_IP_FIELD_COPY_MAC(exMxPmRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr);

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdIpv4L4Ptr->sip), &(ruleStdIpv4L4Ptr->sip), u32Ip);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdIpv4L4Ptr->dip), &(ruleStdIpv4L4Ptr->dip), u32Ip);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr, isBc);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr, l4Byte0);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr, l4Byte1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr, l4Byte13);

    /* convert UDB into device specific format */
    cpssOsMemCpy(exMxPmRuleStdIpv4L4Ptr->udb,
                 ruleStdIpv4L4Ptr->udb,
                 sizeof(exMxPmRuleStdIpv4L4Ptr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmIngExtNotIpv6
*
* DESCRIPTION:
*       Convert generic into device specific not IP and IPV4 packet key
*
* INPUTS:
*       ruleExtNotIpv6Ptr - (pointer to) not IP and IPV4 packet key
*
* OUTPUTS:
*       exMxPmRuleExtNotIpv6Ptr - (pointer to) ExMxPm not IP and IPV4 packet key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmIngExtNotIpv6
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC                    *ruleExtNotIpv6Ptr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_NOT_IPV6_STC   *exMxPmRuleExtNotIpv6Ptr
)
{
    /* convert common part into device specific format */
    prvTgfConvertGenericToExMxPmPclRuleCommonExt(&(ruleExtNotIpv6Ptr->common),
                                                 &(ruleExtNotIpv6Ptr->commonExt),
                                                 &(exMxPmRuleExtNotIpv6Ptr->commonIngrExt),
                                                 &(exMxPmRuleExtNotIpv6Ptr->commonIngrExtPkt));

    /* convert into device specific format */
    exMxPmRuleExtNotIpv6Ptr->l2Encapsulation0 = ruleExtNotIpv6Ptr->l2Encap;
    exMxPmRuleExtNotIpv6Ptr->ethType          = ruleExtNotIpv6Ptr->etherType;
    exMxPmRuleExtNotIpv6Ptr->isIpv4Fragment   = ruleExtNotIpv6Ptr->ipv4Fragmented;

    /* convert dip into device specific format */
    cpssOsMemCpy(exMxPmRuleExtNotIpv6Ptr->dipBits31_0,
                 ruleExtNotIpv6Ptr->dip.arIP,
                 sizeof(exMxPmRuleExtNotIpv6Ptr->dipBits31_0));

    /* convert macDa into device specific format */
    cpssOsMemCpy(exMxPmRuleExtNotIpv6Ptr->macDa.arEther,
                 ruleExtNotIpv6Ptr->macDa.arEther,
                 sizeof(exMxPmRuleExtNotIpv6Ptr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(exMxPmRuleExtNotIpv6Ptr->macSa.arEther,
                 ruleExtNotIpv6Ptr->macSa.arEther,
                 sizeof(exMxPmRuleExtNotIpv6Ptr->macSa.arEther));

    /* convert UDB into device specific format */
    cpssOsMemCpy(exMxPmRuleExtNotIpv6Ptr->udb,
                 ruleExtNotIpv6Ptr->udb,
                 sizeof(exMxPmRuleExtNotIpv6Ptr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmIngRuleExtIpv6L2
*
* DESCRIPTION:
*       Convert generic into device specific extended IPV6 packet L2 styled key
*
* INPUTS:
*       ruleExtIpv6L2Ptr - (pointer to) extended IPV6 packet L2 styled key
*
* OUTPUTS:
*       exMxPmRuleExtIpv6L2Ptr - (pointer to) ExMxPm extended IPV6 packet L2 styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmIngRuleExtIpv6L2
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EXT_IPV6_L2_STC                     *ruleExtIpv6L2Ptr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L2_STC    *exMxPmRuleExtIpv6L2Ptr
)
{
    /* convert common part into device specific format */
    prvTgfConvertGenericToExMxPmPclRuleCommonExt(&(ruleExtIpv6L2Ptr->common),
                                                 &(ruleExtIpv6L2Ptr->commonExt),
                                                 &(exMxPmRuleExtIpv6L2Ptr->commonIngrExt),
                                                 &(exMxPmRuleExtIpv6L2Ptr->commonIngrExtPkt));

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtIpv6L2Ptr, ruleExtIpv6L2Ptr, isIpv6HopByHop);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtIpv6L2Ptr, ruleExtIpv6L2Ptr, udb11Dup);

    exMxPmRuleExtIpv6L2Ptr->isIpv6Eh = ruleExtIpv6L2Ptr->isIpv6ExtHdrExist;
    exMxPmRuleExtIpv6L2Ptr->dipBits127_120 = ruleExtIpv6L2Ptr->dipBits;

    /* convert sip into device specific format */
    cpssOsMemCpy(exMxPmRuleExtIpv6L2Ptr->sipBits127_32,
                 ruleExtIpv6L2Ptr->sip.arIP,
                 sizeof(exMxPmRuleExtIpv6L2Ptr->sipBits127_32));

    /* convert macDa into device specific format */
    cpssOsMemCpy(exMxPmRuleExtIpv6L2Ptr->macDa.arEther,
                 ruleExtIpv6L2Ptr->macDa.arEther,
                 sizeof(exMxPmRuleExtIpv6L2Ptr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(exMxPmRuleExtIpv6L2Ptr->macSa.arEther,
                 ruleExtIpv6L2Ptr->macSa.arEther,
                 sizeof(exMxPmRuleExtIpv6L2Ptr->macSa.arEther));

    /* convert UDB into device specific format */
    cpssOsMemCpy(exMxPmRuleExtIpv6L2Ptr->udb,
                 ruleExtIpv6L2Ptr->udb,
                 sizeof(exMxPmRuleExtIpv6L2Ptr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmIngRuleExtIpv6L4
*
* DESCRIPTION:
*       Convert generic into device specific extended IPV6 packet L4 styled key
*
* INPUTS:
*       ruleExtIpv6L4Ptr - (pointer to) extended IPV6 packet L4 styled key
*
* OUTPUTS:
*       exMxPmRuleExtIpv6L4Ptr - (pointer to) ExMxPm extended IPV6 packet L4 styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmIngRuleExtIpv6L4
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EXT_IPV6_L4_STC                     *ruleExtIpv6L4Ptr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L4_STC    *exMxPmRuleExtIpv6L4Ptr
)
{
    /* convert common part into device specific format */
    prvTgfConvertGenericToExMxPmPclRuleCommonExt(&(ruleExtIpv6L4Ptr->common),
                                                 &(ruleExtIpv6L4Ptr->commonExt),
                                                 &(exMxPmRuleExtIpv6L4Ptr->commonIngrExt),
                                                 &(exMxPmRuleExtIpv6L4Ptr->commonIngrExtPkt));

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtIpv6L4Ptr, ruleExtIpv6L4Ptr, isIpv6HopByHop);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtIpv6L4Ptr, ruleExtIpv6L4Ptr, udb12Dup);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtIpv6L4Ptr, ruleExtIpv6L4Ptr, udb13Dup);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleExtIpv6L4Ptr, ruleExtIpv6L4Ptr, udb14Dup);

    exMxPmRuleExtIpv6L4Ptr->isIpv6Eh = ruleExtIpv6L4Ptr->isIpv6ExtHdrExist;

    /* convert sip into device specific format */
    cpssOsMemCpy(exMxPmRuleExtIpv6L4Ptr->sipBits127_32,
                 ruleExtIpv6L4Ptr->sip.arIP,
                 sizeof(exMxPmRuleExtIpv6L4Ptr->sipBits127_32));

    /* convert dip into device specific format */
    cpssOsMemCpy(exMxPmRuleExtIpv6L4Ptr->dip.u32Ip,
                 ruleExtIpv6L4Ptr->dip.u32Ip,
                 sizeof(exMxPmRuleExtIpv6L4Ptr->dip.u32Ip));

    /* convert UDB into device specific format */
    cpssOsMemCpy(exMxPmRuleExtIpv6L4Ptr->udb,
                 ruleExtIpv6L4Ptr->udb,
                 sizeof(exMxPmRuleExtIpv6L4Ptr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmEgrRuleStdNotIp
*
* DESCRIPTION:
*       Convert generic into device specific egress standard not IP packet key
*
* INPUTS:
*       ruleStdNotIpPtr - (pointer to) standard not IP packet key
*
* OUTPUTS:
*       exMxPmRuleStdNotIpPtr - (pointer to) ExMxPm standard not IP packet key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmEgrRuleStdNotIp
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_STD_NOT_IP_STC              *ruleStdNotIpPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_STC  *exMxPmRuleStdNotIpPtr
)
{
    GT_STATUS   rc = GT_OK;


    /* convert common part into device specific format */
    rc = prvTgfConvertGenericToExMxPmPclRuleEgrCommon(&(ruleStdNotIpPtr->common),
                                                      &(exMxPmRuleStdNotIpPtr->commonEgr),
                                                      &(exMxPmRuleStdNotIpPtr->commonEgrStd));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmPclRuleEgrCommon FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert into device specific format */
    exMxPmRuleStdNotIpPtr->ethType     = ruleStdNotIpPtr->etherType;
    exMxPmRuleStdNotIpPtr->l2EncapType = ruleStdNotIpPtr->l2Encap;
    exMxPmRuleStdNotIpPtr->isBc_Extrn  = ruleStdNotIpPtr->isBc;

    /* convert macDa into device specific format */
    cpssOsMemCpy(exMxPmRuleStdNotIpPtr->macDa.arEther,
                 ruleStdNotIpPtr->macDa.arEther,
                 sizeof(exMxPmRuleStdNotIpPtr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(exMxPmRuleStdNotIpPtr->macSa.arEther,
                 ruleStdNotIpPtr->macSa.arEther,
                 sizeof(exMxPmRuleStdNotIpPtr->macSa.arEther));

    return rc;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmEgrRuleStdIpL2Qos
*
* DESCRIPTION:
*       Convert generic into device specific egress standard IPV4 and IPV6 packets
*       L2 and QOS styled key
*
* INPUTS:
*       ruleStdIpL2QosPtr - (pointer to) standard IP packets L2 and QOS styled key
*
* OUTPUTS:
*       exMxPmRuleStdIpL2QosPtr - (pointer to) ExMxPm standard IP packets L2 and QOS styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmEgrRuleStdIpL2Qos
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC               *ruleStdIpL2QosPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IP_L2_QOS_STC   *exMxPmRuleStdIpL2QosPtr
)
{
    GT_STATUS   rc = GT_OK;


    /* convert common part into device specific format */
    rc = prvTgfConvertGenericToExMxPmPclRuleEgrCommon(&(ruleStdIpL2QosPtr->common),
                                                      &(exMxPmRuleStdIpL2QosPtr->commonEgr),
                                                      &(exMxPmRuleStdIpL2QosPtr->commonEgrStd));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmPclRuleEgrCommon FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert commonEgrStd into device specific format */
    prvTgfConvertGenericToExMxPmPclRuleEgrStdIp(&(ruleStdIpL2QosPtr->commonStdIp),
                                                &(exMxPmRuleStdIpL2QosPtr->commonEgrStdIp));

    /* convert macDa into device specific format */
    cpssOsMemCpy(exMxPmRuleStdIpL2QosPtr->macDa.arEther,
                 ruleStdIpL2QosPtr->macDa.arEther,
                 sizeof(exMxPmRuleStdIpL2QosPtr->macDa.arEther));

    /* convert Sip into device specific format */
    cpssOsMemCpy(exMxPmRuleStdIpL2QosPtr->sipBits31_0,
                 ruleStdIpL2QosPtr->sipBits,
                 sizeof(exMxPmRuleStdIpL2QosPtr->sipBits31_0));

    return rc;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmEgrRuleStdIpv4L4
*
* DESCRIPTION:
*       Convert generic into device specific egress standard IPV4 packet L4 styled key
*
* INPUTS:
*       ruleStdIpv4L4Ptr - (pointer to) standard IPV4 packet L4 styled key
*
* OUTPUTS:
*       exMxPmRuleStdIpv4L4Ptr - (pointer to) ExMxPm standard IPV4 packet L4 styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmEgrRuleStdIpv4L4
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC             *ruleStdIpv4L4Ptr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IPV4_L4_STC *exMxPmRuleStdIpv4L4Ptr
)
{
    GT_STATUS   rc = GT_OK;


    /* convert common part into device specific format */
    rc = prvTgfConvertGenericToExMxPmPclRuleEgrCommon(&(ruleStdIpv4L4Ptr->common),
                                                      &(exMxPmRuleStdIpv4L4Ptr->commonEgr),
                                                      &(exMxPmRuleStdIpv4L4Ptr->commonEgrStd));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmPclRuleEgrCommon FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert commonEgrStd into device specific format */
    prvTgfConvertGenericToExMxPmPclRuleEgrStdIp(&(ruleStdIpv4L4Ptr->commonStdIp),
                                                &(exMxPmRuleStdIpv4L4Ptr->commonEgrStdIp));

    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr, isBc);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr, l4Byte0);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmRuleStdIpv4L4Ptr, ruleStdIpv4L4Ptr, l4Byte1);

    /* convert Sip into device specific format */
    exMxPmRuleStdIpv4L4Ptr->sip.u32Ip = ruleStdIpv4L4Ptr->sip.u32Ip;

    return rc;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmEgrRuleExtNotIpv6
*
* DESCRIPTION:
*       Convert generic into device specific egress extended not IP and IPV4 packet key
*
* INPUTS:
*       ruleExtNotIpv6Ptr - (pointer to) extended not IP and IPV4 packet key
*
* OUTPUTS:
*       exMxPmRuleExtNotIpv6Ptr - (pointer to) ExMxPm extended not IP and IPV4 packet key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmEgrRuleExtNotIpv6
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC                *ruleExtNotIpv6Ptr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_STC    *exMxPmRuleExtNotIpv6Ptr
)
{
    GT_STATUS   rc = GT_OK;


    /* convert common part into device specific format */
    rc = prvTgfConvertGenericToExMxPmPclRuleEgrCommonExt(&(ruleExtNotIpv6Ptr->common),
                                                         &(ruleExtNotIpv6Ptr->commonExt),
                                                         &(exMxPmRuleExtNotIpv6Ptr->commonEgr),
                                                         &(exMxPmRuleExtNotIpv6Ptr->commonEgrExt));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmPclRuleEgrCommonExt FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert into device specific format */
    exMxPmRuleExtNotIpv6Ptr->l2EncapType     = ruleExtNotIpv6Ptr->l2Encap;
    exMxPmRuleExtNotIpv6Ptr->ethType         = ruleExtNotIpv6Ptr->etherType;
    exMxPmRuleExtNotIpv6Ptr->isIpv4Fragment  = ruleExtNotIpv6Ptr->ipv4Fragmented;
    exMxPmRuleExtNotIpv6Ptr->ipv4Options_Ext = ruleExtNotIpv6Ptr->ipv4Options;

    /* convert macDa into device specific format */
    cpssOsMemCpy(exMxPmRuleExtNotIpv6Ptr->macDa.arEther,
                 ruleExtNotIpv6Ptr->macDa.arEther,
                 sizeof(exMxPmRuleExtNotIpv6Ptr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(exMxPmRuleExtNotIpv6Ptr->macSa.arEther,
                 ruleExtNotIpv6Ptr->macSa.arEther,
                 sizeof(exMxPmRuleExtNotIpv6Ptr->macSa.arEther));

    /* convert dip into device specific format */
    cpssOsMemCpy(exMxPmRuleExtNotIpv6Ptr->dipBits31_0,
                 ruleExtNotIpv6Ptr->dip.arIP,
                 sizeof(exMxPmRuleExtNotIpv6Ptr->dipBits31_0));

    return rc;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmEgrRuleExtIpv6L2
*
* DESCRIPTION:
*       Convert generic into device specific egress extended IPV6 packet L2 styled key
*
* INPUTS:
*       ruleExtIpv6L2Ptr - (pointer to) extended IPV6 packet L2 styled key
*
* OUTPUTS:
*       exMxPmRuleExtIpv6L2Ptr - (pointer to) ExMxPm extended IPV6 packet L2 styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmEgrRuleExtIpv6L2
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC             *ruleExtIpv6L2Ptr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L2_STC *exMxPmRuleExtIpv6L2Ptr
)
{
    GT_STATUS   rc = GT_OK;


    /* convert common part into device specific format */
    rc = prvTgfConvertGenericToExMxPmPclRuleEgrCommonExt(&(ruleExtIpv6L2Ptr->common),
                                                         &(ruleExtIpv6L2Ptr->commonExt),
                                                         &(exMxPmRuleExtIpv6L2Ptr->commonEgr),
                                                         &(exMxPmRuleExtIpv6L2Ptr->commonEgrExt));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmPclRuleEgrCommonExt FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert into device specific format */
    exMxPmRuleExtIpv6L2Ptr->isIpv6Eh_Extrn = ruleExtIpv6L2Ptr->isIpv6Eh;
    exMxPmRuleExtIpv6L2Ptr->dipBits127_120 = ruleExtIpv6L2Ptr->dipBits;

    /* convert Sip into device specific format */
    cpssOsMemCpy(exMxPmRuleExtIpv6L2Ptr->sipBits127_32,
                 ruleExtIpv6L2Ptr->sip.arIP,
                 sizeof(exMxPmRuleExtIpv6L2Ptr->sipBits127_32));

    /* convert macDa into device specific format */
    cpssOsMemCpy(exMxPmRuleExtIpv6L2Ptr->macDa.arEther,
                 ruleExtIpv6L2Ptr->macDa.arEther,
                 sizeof(exMxPmRuleExtIpv6L2Ptr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(exMxPmRuleExtIpv6L2Ptr->macSa.arEther,
                 ruleExtIpv6L2Ptr->macSa.arEther,
                 sizeof(exMxPmRuleExtIpv6L2Ptr->macSa.arEther));

    return rc;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmEgrRuleExtIpv6L4
*
* DESCRIPTION:
*       Convert generic into device specific egress extended IPV6 packet L4 styled key
*
* INPUTS:
*       ruleExtIpv6L4Ptr - (pointer to) extended IPV6 packet L4 styled key
*
* OUTPUTS:
*       exMxPmRuleExtIpv6L4Ptr - (pointer to) ExMxPm extended IPV6 packet L4 styled key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmEgrRuleExtIpv6L4
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EGR_EXT_IPV6_L4_STC             *ruleExtIpv6L4Ptr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L4_STC *exMxPmRuleExtIpv6L4Ptr
)
{
    GT_STATUS   rc = GT_OK;


    /* convert common part into device specific format */
    rc = prvTgfConvertGenericToExMxPmPclRuleEgrCommonExt(&(ruleExtIpv6L4Ptr->common),
                                                         &(ruleExtIpv6L4Ptr->commonExt),
                                                         &(exMxPmRuleExtIpv6L4Ptr->commonEgr),
                                                         &(exMxPmRuleExtIpv6L4Ptr->commonEgrExt));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmPclRuleEgrCommonExt FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert into device specific format */
    exMxPmRuleExtIpv6L4Ptr->isIpv6Eh_Extrn = ruleExtIpv6L4Ptr->isIpv6Eh;

    /* convert Sip into device specific format */
    cpssOsMemCpy(exMxPmRuleExtIpv6L4Ptr->sipBits127_32,
                 ruleExtIpv6L4Ptr->sip.arIP,
                 sizeof(exMxPmRuleExtIpv6L4Ptr->sipBits127_32));

    /* convert Dip into device specific format */
    cpssOsMemCpy(exMxPmRuleExtIpv6L4Ptr->dip.u32Ip,
                 ruleExtIpv6L4Ptr->dip.u32Ip,
                 sizeof(exMxPmRuleExtIpv6L4Ptr->dip.u32Ip));

    return rc;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmIngStdUdb
*
* DESCRIPTION:
*       Convert generic into device specific standard UDB styled packet key
*
* INPUTS:
*       ruleStdUdbPtr - (pointer to) standard UDB styled packet key
*
* OUTPUTS:
*       exMxPmRuleStdUdbPtr - (pointer to) ExMxPm standard UDB styled packet key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmIngStdUdb
(
    IN  PRV_TGF_PCL_RULE_FORMAT_STD_UDB_STC                     *ruleStdUdbPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC    *exMxPmRuleStdUdbPtr
)
{
    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrStd), &(ruleStdUdbPtr->commonStd), pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrStd), &(ruleStdUdbPtr->commonStd), macToMe);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrStd), &(ruleStdUdbPtr->commonStd), vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrStd), &(ruleStdUdbPtr->commonStd), up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrStd), &(ruleStdUdbPtr->commonStd), isIpv4);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrStd), &(ruleStdUdbPtr->commonStd), isIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), pktTagging);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), l3OffsetInvalid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), l4ProtocolType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), pktType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), ipHeaderOk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), macDaType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), isIpv6Eh);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), isIpv6HopByHop);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), isUdbValid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), isL2Valid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleStdUdbPtr->commonIngrUdb), &(ruleStdUdbPtr->commonUdb), dscpOrExp);

    exMxPmRuleStdUdbPtr->commonIngrStd.srcPort         = ruleStdUdbPtr->commonStd.sourcePort;
    exMxPmRuleStdUdbPtr->commonIngrUdb.l4OffsetInalid  = ruleStdUdbPtr->commonUdb.l4OffsetInvalid;
    exMxPmRuleStdUdbPtr->commonIngrUdb.l2Encapsulation = ruleStdUdbPtr->commonUdb.l2Encap;

    /* convert UDB into device specific format */
    cpssOsMemCpy(exMxPmRuleStdUdbPtr->udb,
                 ruleStdUdbPtr->udb,
                 sizeof(exMxPmRuleStdUdbPtr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmIngRuleExtUdb
*
* DESCRIPTION:
*       Convert generic into device specific extended UDB styled packet key
*
* INPUTS:
*       ruleExtUdbPtr - (pointer to) extended UDB styled packet key
*
* OUTPUTS:
*       exMxPmRuleExtUdbPtr - (pointer to) ExMxPm extended UDB styled packet key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmIngRuleExtUdb
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EXT_UDB_STC                     *ruleExtUdbPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC    *exMxPmRuleExtUdbPtr
)
{
    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrExt), &(ruleExtUdbPtr->commonStd), pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrExt), &(ruleExtUdbPtr->commonStd), macToMe);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrExt), &(ruleExtUdbPtr->commonStd), vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrExt), &(ruleExtUdbPtr->commonStd), up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrExt), &(ruleExtUdbPtr->commonStd), isIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrExt), &(ruleExtUdbPtr->commonExt), isIpv6);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrExt), &(ruleExtUdbPtr->commonExt), ipProtocol);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), pktTagging);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), l3OffsetInvalid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), l4ProtocolType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), pktType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), ipHeaderOk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), macDaType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), isIpv6Eh);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), isIpv6HopByHop);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), isUdbValid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), isL2Valid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), dscpOrExp);

    exMxPmRuleExtUdbPtr->commonIngrExt.srcPort         = ruleExtUdbPtr->commonStd.sourcePort;
    exMxPmRuleExtUdbPtr->commonIngrUdb.l4OffsetInalid  = ruleExtUdbPtr->commonIngrUdb.l4OffsetInvalid;
    exMxPmRuleExtUdbPtr->commonIngrUdb.l2Encapsulation = ruleExtUdbPtr->commonIngrUdb.l2Encap;

    /* convert macSaOrSipBits79_32 into device specific format */
    cpssOsMemCpy(exMxPmRuleExtUdbPtr->macSaOrSipBits79_32,
                 ruleExtUdbPtr->macSaOrSipBits79_32,
                 sizeof(exMxPmRuleExtUdbPtr->macSaOrSipBits79_32));

    /* convert macDaOrSipBits127_80 into device specific format */
    cpssOsMemCpy(exMxPmRuleExtUdbPtr->macDaOrSipBits127_80,
                 ruleExtUdbPtr->macDaOrSipBits127_80,
                 sizeof(exMxPmRuleExtUdbPtr->macDaOrSipBits127_80));

    /* convert sipBits31_0 into device specific format */
    cpssOsMemCpy(exMxPmRuleExtUdbPtr->sipBits31_0,
                 ruleExtUdbPtr->sipBits31_0,
                 sizeof(exMxPmRuleExtUdbPtr->sipBits31_0));

    /* convert dipBits31_0 into device specific format */
    cpssOsMemCpy(exMxPmRuleExtUdbPtr->dipBits31_0,
                 ruleExtUdbPtr->dipBits31_0,
                 sizeof(exMxPmRuleExtUdbPtr->dipBits31_0));

    /* convert dipBits127_112 into device specific format */
    cpssOsMemCpy(exMxPmRuleExtUdbPtr->dipBits127_112,
                 ruleExtUdbPtr->dipBits127_112,
                 sizeof(exMxPmRuleExtUdbPtr->dipBits127_112));

    /* convert udb into device specific format */
    cpssOsMemCpy(exMxPmRuleExtUdbPtr->udb,
                 ruleExtUdbPtr->udb,
                 sizeof(exMxPmRuleExtUdbPtr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmIngRuleExtUdb80b
*
* DESCRIPTION:
*       Convert generic into device specific extended UDB styled packet key
*
* INPUTS:
*       ruleExtUdbPtr - (pointer to) extended UDB styled packet key
*
* OUTPUTS:
*       exMxPmRuleExtUdbPtr - (pointer to) ExMxPm extended UDB styled packet key
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmIngRuleExtUdb80b
(
    IN  PRV_TGF_PCL_RULE_FORMAT_EXT_80B_STC                     *ruleExtUdbPtr,
    OUT CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTERNAL_80B_STC    *exMxPmRuleExtUdbPtr
)
{
    /* convert into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrExt), &(ruleExtUdbPtr->commonExt), isIpv6);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrExt), &(ruleExtUdbPtr->commonExt), ipProtocol);

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrExt), &(ruleExtUdbPtr->commonIngrUdb), pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrExt), &(ruleExtUdbPtr->commonIngrUdb), macToMe);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrExt), &(ruleExtUdbPtr->commonIngrUdb), vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrExt), &(ruleExtUdbPtr->commonIngrUdb), up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrExt), &(ruleExtUdbPtr->commonIngrUdb), isIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), pktTagging);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), l3OffsetInvalid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), l4ProtocolType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), pktType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), ipHeaderOk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), macDaType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), isIpv6Eh);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), isIpv6HopByHop);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), isUdbValid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), isL2Valid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleExtUdbPtr->commonIngrUdb), &(ruleExtUdbPtr->commonIngrUdb), dscpOrExp);

    exMxPmRuleExtUdbPtr->commonIngrExt.srcPort         = ruleExtUdbPtr->commonIngrUdb.sourcePort;
    exMxPmRuleExtUdbPtr->commonIngrUdb.l4OffsetInalid  = ruleExtUdbPtr->commonIngrUdb.l4OffsetInvalid;
    exMxPmRuleExtUdbPtr->commonIngrUdb.l2Encapsulation = ruleExtUdbPtr->commonIngrUdb.l2Encap;

    exMxPmRuleExtUdbPtr->ethType = ruleExtUdbPtr->ethType;
    exMxPmRuleExtUdbPtr->l4Byte0 = ruleExtUdbPtr->l4Byte0;
    exMxPmRuleExtUdbPtr->l4Byte1 = ruleExtUdbPtr->l4Byte1;
    exMxPmRuleExtUdbPtr->l4Byte2 = ruleExtUdbPtr->l4Byte2;
    exMxPmRuleExtUdbPtr->l4Byte3 = ruleExtUdbPtr->l4Byte3;
    exMxPmRuleExtUdbPtr->l4Byte13 = ruleExtUdbPtr->l4Byte13;

    /* convert macDa into device specific format */
    cpssOsMemCpy(exMxPmRuleExtUdbPtr->macDa.arEther,
                 ruleExtUdbPtr->macDa.arEther,
                 sizeof(exMxPmRuleExtUdbPtr->macDa.arEther));

    /* convert macSa into device specific format */
    cpssOsMemCpy(exMxPmRuleExtUdbPtr->macSa.arEther,
                 ruleExtUdbPtr->macSa.arEther,
                 sizeof(exMxPmRuleExtUdbPtr->macSa.arEther));

    /* convert sip into device specific format */
    cpssOsMemCpy(exMxPmRuleExtUdbPtr->sip.u32Ip,
                 ruleExtUdbPtr->sip.u32Ip,
                 sizeof(exMxPmRuleExtUdbPtr->sip.u32Ip));

    /* convert dip into device specific format */
    cpssOsMemCpy(exMxPmRuleExtUdbPtr->dip.u32Ip,
                 ruleExtUdbPtr->dip.u32Ip,
                 sizeof(exMxPmRuleExtUdbPtr->dip.u32Ip));

    /* convert udb into device specific format */
    cpssOsMemCpy(exMxPmRuleExtUdbPtr->udb,
                 ruleExtUdbPtr->udb,
                 sizeof(exMxPmRuleExtUdbPtr->udb));

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmQos
*
* DESCRIPTION:
*       Convert generic into device specific QoS attributes mark actions
*
* INPUTS:
*       qosAttributesPtr - (pointer to) QoS attributes mark actions
*
* OUTPUTS:
*       exMxPmQosAttributesPtr - (pointer to) ExMxPm QoS attributes mark actions
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmQos
(
    IN  PRV_TGF_PCL_ACTION_QOS_STC      *qosAttributesPtr,
    OUT CPSS_EXMXPM_QOS_ENTRY_STC       *exMxPmQosAttributesPtr
)
{
    /* convert into device specific format */
    PRV_TGF_STC_MODIFY_QOS_ATTR_FIELD_COPY_MAC(exMxPmQosAttributesPtr,
                                               qosAttributesPtr, modifyDscp);
    PRV_TGF_STC_MODIFY_QOS_ATTR_FIELD_COPY_MAC(exMxPmQosAttributesPtr,
                                               qosAttributesPtr, modifyUp);
    PRV_TGF_STC_MODIFY_QOS_ATTR_FIELD_COPY_MAC(exMxPmQosAttributesPtr,
                                               qosAttributesPtr, modifyTc);
    PRV_TGF_STC_MODIFY_QOS_ATTR_FIELD_COPY_MAC(exMxPmQosAttributesPtr,
                                               qosAttributesPtr, modifyDp);
    PRV_TGF_STC_MODIFY_QOS_ATTR_FIELD_COPY_MAC(exMxPmQosAttributesPtr,
                                               qosAttributesPtr, modifyExp);

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmQosAttributesPtr->qosParams),
                                   qosAttributesPtr, tc);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmQosAttributesPtr->qosParams),
                                   qosAttributesPtr, dp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmQosAttributesPtr->qosParams),
                                   qosAttributesPtr, up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmQosAttributesPtr->qosParams),
                                   qosAttributesPtr, dscp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmQosAttributesPtr->qosParams),
                                   qosAttributesPtr, exp);

    exMxPmQosAttributesPtr->qosPrecedence = qosAttributesPtr->profilePrecedence;

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmRedirect
*
* DESCRIPTION:
*       Convert generic into device specific redirection related actions
*
* INPUTS:
*       redirectPtr - (pointer to) redirection related actions
*
* OUTPUTS:
*       exMxPmRedirectPtr - (pointer to) ExMxPm redirection related actions
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmRedirect
(
    IN  PRV_TGF_PCL_ACTION_REDIRECT_STC     *redirectPtr,
    OUT CPSS_EXMXPM_PCL_ACTION_REDIRECT_STC *exMxPmRedirectPtr
)
{
    /* convert redirectCmd into device specific format */
    switch (redirectPtr->redirectCmd)
    {
        case PRV_TGF_PCL_ACTION_REDIRECT_CMD_NONE_E:
            exMxPmRedirectPtr->redirectCmd = CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_NONE_E;
            break;

        case PRV_TGF_PCL_ACTION_REDIRECT_CMD_OUT_IF_E:
            exMxPmRedirectPtr->redirectCmd = CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;

            /* convert into device specific format */
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRedirectPtr->data.outIf),
                                           &(redirectPtr->data.outIf), vntL2Echo);

            /* convert outlifType into device specific format */
            switch (redirectPtr->data.outIf.outlifType)
            {
                case PRV_TGF_OUTLIF_TYPE_LL_E:
                    exMxPmRedirectPtr->data.outIf.outLifInfo.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
                    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRedirectPtr->data.outIf.outLifInfo.outlifPointer),
                                                   &(redirectPtr->data.outIf.outlifPointer), arpPtr);

                    break;

                case PRV_TGF_OUTLIF_TYPE_DIT_E:
                    exMxPmRedirectPtr->data.outIf.outLifInfo.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_DIT_E;
                    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRedirectPtr->data.outIf.outLifInfo.outlifPointer),
                                                   &(redirectPtr->data.outIf.outlifPointer), ditPtr);

                    break;

                case PRV_TGF_OUTLIF_TYPE_TUNNEL_E:
                    exMxPmRedirectPtr->data.outIf.outLifInfo.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E;
                    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRedirectPtr->data.outIf.outLifInfo.outlifPointer.tunnelStartPtr),
                                                   &(redirectPtr->data.outIf.outlifPointer.tunnelStartPtr), ptr);

                    switch (redirectPtr->data.outIf.outlifPointer.tunnelStartPtr.tunnelType)
                    {
                        case PRV_TGF_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E:
                            exMxPmRedirectPtr->data.outIf.outLifInfo.outlifPointer.tunnelStartPtr.passengerPacketType =
                                CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ETHERNET_E;
                            break;

                        case PRV_TGF_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E:
                            exMxPmRedirectPtr->data.outIf.outLifInfo.outlifPointer.tunnelStartPtr.passengerPacketType =
                                CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_OTHER_E;
                            break;

                        default:
                            return GT_BAD_PARAM;
                    }

                    break;

                default:
                    return GT_BAD_PARAM;
            }

            /* convert outInterface into device specific format */
            cpssOsMemCpy(&(exMxPmRedirectPtr->data.outIf.outLifInfo.interfaceInfo),
                         &(redirectPtr->data.outIf.outInterface),
                         sizeof(exMxPmRedirectPtr->data.outIf.outLifInfo.interfaceInfo));

            break;

        case PRV_TGF_PCL_ACTION_REDIRECT_CMD_ROUTER_E:
            exMxPmRedirectPtr->redirectCmd = CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_ROUTER_E;

            /* convert into device specific format */
            exMxPmRedirectPtr->data.ipNextHopEntryIndex = redirectPtr->data.routerLttIndex;

            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmPolicer
*
* DESCRIPTION:
*       Convert generic into device specific policer related actions
*
* INPUTS:
*       policerPtr - (pointer to) policer related actions
*
* OUTPUTS:
*       exMxPmChPolicerPtr - (pointer to) ExMxPm policer related actions
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmPolicer
(
    IN  PRV_TGF_PCL_ACTION_POLICER_STC      *policerPtr,
    OUT CPSS_EXMXPM_PCL_ACTION_POLICER_STC  *exMxPmChPolicerPtr
)
{
    /* convert into device specific format */
    exMxPmChPolicerPtr->policerPtr = policerPtr->policerId;

    /* convert policerEnable into device specific format */
    switch (policerPtr->policerEnable)
    {
        case PRV_TGF_PCL_POLICER_DISABLE_ALL_E:
            exMxPmChPolicerPtr->activateCounter = GT_FALSE;
            exMxPmChPolicerPtr->activateMeter   = GT_FALSE;
            break;

        case PRV_TGF_PCL_POLICER_ENABLE_METER_AND_COUNTER_E:
            exMxPmChPolicerPtr->activateCounter = GT_TRUE;
            exMxPmChPolicerPtr->activateMeter   = GT_TRUE;
            break;

        case PRV_TGF_PCL_POLICER_ENABLE_METER_ONLY_E:
            exMxPmChPolicerPtr->activateCounter = GT_FALSE;
            exMxPmChPolicerPtr->activateMeter   = GT_TRUE;
            break;

        case PRV_TGF_PCL_POLICER_ENABLE_COUNTER_ONLY_E:
            exMxPmChPolicerPtr->activateCounter = GT_TRUE;
            exMxPmChPolicerPtr->activateMeter   = GT_FALSE;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmRuleAction
*
* DESCRIPTION:
*       Convert generic into device specific Policy Engine Action
*
* INPUTS:
*       ruleActionPtr - (pointer to) Policy Engine Action
*
* OUTPUTS:
*       exMxPmRuleActionPtr - (pointer to) ExMxPm Policy Engine Action
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmRuleAction
(
    IN  PRV_TGF_PCL_ACTION_STC              *ruleActionPtr,
    OUT CPSS_EXMXPM_PCL_ACTION_UNT          *exMxPmRuleActionPtr
)
{
    GT_STATUS   rc = GT_OK;


    /* switch between policy */
    if (GT_FALSE == ruleActionPtr->egressPolicy)
    {
        /* convert into device specific format */
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->ingressStandard), ruleActionPtr, pktCmd);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->ingressStandard), ruleActionPtr, actionStop);

        /* convert mirroring related actions into device specific format */
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->ingressStandard.mirror),
                                       &(ruleActionPtr->mirror), cpuCode);
        exMxPmRuleActionPtr->ingressStandard.mirror.ingressPolicyAnalyzerNumber =
            ruleActionPtr->mirror.mirrorToRxAnalyzerPort;

        /* convert QoS attributes mark actions into device specific format */
        rc = prvTgfConvertGenericToExMxPmQos(&(ruleActionPtr->qos),
                                             &(exMxPmRuleActionPtr->ingressStandard.qos));
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmQos FAILED, rc = [%d]", rc);

            return rc;
        }

        /* convert VLAN modification configuration into device specific format */
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->ingressStandard.vlan),
                                       &(ruleActionPtr->vlan), precedence);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->ingressStandard.vlan),
                                       &(ruleActionPtr->vlan), nestedVlan);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->ingressStandard.vlan),
                                       &(ruleActionPtr->vlan), vlanCmd);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->ingressStandard.vlan),
                                       &(ruleActionPtr->vlan), vidOrInLif);

        /* convert redirection related actions into device specific format */
        rc = prvTgfConvertGenericToExMxPmRedirect(&(ruleActionPtr->redirect),
                                                  &(exMxPmRuleActionPtr->ingressStandard.redirect));
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChRedirect FAILED, rc = [%d]", rc);

            return rc;
        }

        /* convert policer related actions into device specific format */
        rc = prvTgfConvertGenericToExMxPmPolicer(&(ruleActionPtr->policer),
                                                 &(exMxPmRuleActionPtr->ingressStandard.policer));
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmPolicer FAILED, rc = [%d]", rc);

            return rc;
        }

        /* convert packet source Id assignment into device specific format */
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->ingressStandard.sourceId),
                                       &(ruleActionPtr->sourceId), assignSourceId);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->ingressStandard.sourceId),
                                       &(ruleActionPtr->sourceId), sourceIdValue);

        /* convert packet fabric assignment into device specific format */
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->ingressStandard.fabric),
                                       &(ruleActionPtr->fabric), flowId);
    }
    else
    {
        /* convert into device specific format */
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->egress), ruleActionPtr, pktCmd);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->egress), ruleActionPtr, actionStop);

        /* convert QoS attributes mark actions into device specific format */
        rc = prvTgfConvertGenericToExMxPmQos(&(ruleActionPtr->qos),
                                             &(exMxPmRuleActionPtr->egress.qos));
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmQos FAILED, rc = [%d]", rc);

            return rc;
        }

        /* convert VLAN modification configuration into device specific format */
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->egress.vlan),
                                       &(ruleActionPtr->vlan), precedence);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->egress.vlan),
                                       &(ruleActionPtr->vlan), nestedVlan);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->egress.vlan),
                                       &(ruleActionPtr->vlan), vlanCmd);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmRuleActionPtr->egress.vlan),
                                       &(ruleActionPtr->vlan), vidOrInLif);

        /* convert policer related actions into device specific format */
        rc = prvTgfConvertGenericToExMxPmPolicer(&(ruleActionPtr->policer),
                                                 &(exMxPmRuleActionPtr->egress.policer));
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmPolicer FAILED, rc = [%d]", rc);

            return rc;
        }
    }

    return rc;
}
#endif /* EXMXPM_FAMILY */


/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfPclInit
*
* DESCRIPTION:
*   The function initializes the Policy engine
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclInit
(
    GT_VOID
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChPclInit(devNum);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPclInit FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    return GT_OK;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclIngressPolicyEnable
*
* DESCRIPTION:
*    Enables Ingress Policy
*
* INPUTS:
*    devNum - device number
*    enable - enable ingress policy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - one of the input parameters is not valid
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclIngressPolicyEnable
(
    IN GT_BOOL                        enable
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;


#ifdef CHX_FAMILY
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChPclIngressPolicyEnable(devNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPclIngressPolicyEnable FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmPclPolicyEnableSet(devNum, CPSS_PCL_DIRECTION_INGRESS_E, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmPclPolicyEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclEgressPolicyEnable
*
* DESCRIPTION:
*    Enables Egress Policy
*
* INPUTS:
*    devNum - device number
*    enable - enable ingress policy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - one of the input parameters is not valid
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclEgressPolicyEnable
(
    IN GT_BOOL                        enable
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;


#ifdef CHX_FAMILY
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxCh2PclEgressPolicyEnable(devNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxCh2PclEgressPolicyEnable FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmPclPolicyEnableSet(devNum, CPSS_PCL_DIRECTION_EGRESS_E, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmPclPolicyEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclPortIngressPolicyEnable
*
* DESCRIPTION:
*    Enables/disables ingress policy per port.
*
* INPUTS:
*    devNum  - device number
*    portNum - port number
*    enable  - enable/disable Ingress Policy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - one of the input parameters is not valid
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclPortIngressPolicyEnable
(
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
)
{
    GT_U32  portIter = 0;


    /* find port index */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortsArray[portIter] == portNum)
        {
            break;
        }
    }

    /* check if port is valid */
    if (prvTgfPortsArray[portIter] != portNum)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", portNum);

        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChPclPortIngressPolicyEnable(prvTgfDevsArray[portIter], portNum, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmPclPortIngressPolicyEnableSet(prvTgfDevsArray[portIter], portNum, enable);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclEgressPclPacketTypesSet
*
* DESCRIPTION:
*    Enables/disables egress policy per packet type and port.
*
* INPUTS:
*    devNum    - device number
*    portNum   - port number
*    pktType   - packet type
*    enable    - enable/disable Ingress Policy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - one of the input parameters is not valid
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclEgressPclPacketTypesSet
(
    IN GT_U8                             devNum,
    IN GT_U8                             portNum,
    IN PRV_TGF_PCL_EGRESS_PKT_TYPE_ENT   pktType,
    IN GT_BOOL                           enable
)
{
#ifdef CHX_FAMILY
    CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT dxChPktType;

    switch (pktType)
    {
        case PRV_TGF_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E:
            dxChPktType = CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E;
            break;
        case PRV_TGF_PCL_EGRESS_PKT_FROM_CPU_DATA_E:
            dxChPktType = CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E;
            break;
        case PRV_TGF_PCL_EGRESS_PKT_TO_CPU_E:
            dxChPktType = CPSS_DXCH_PCL_EGRESS_PKT_TO_CPU_E;
            break;
        case PRV_TGF_PCL_EGRESS_PKT_TO_ANALYZER_E:
            dxChPktType = CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E;
            break;
        case PRV_TGF_PCL_EGRESS_PKT_TS_E:
            dxChPktType = CPSS_DXCH_PCL_EGRESS_PKT_TS_E;
            break;
        case PRV_TGF_PCL_EGRESS_PKT_NON_TS_E:
            dxChPktType = CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E;
            break;
        default: return GT_BAD_PARAM;
    }

    /* call device specific API */
    return cpssDxCh2EgressPclPacketTypesSet(
        devNum, portNum,
        dxChPktType, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(pktType);
    TGF_PARAM_NOT_USED(enable);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclPortLookupCfgTabAccessModeSet
*
* DESCRIPTION:
*    Configures VLAN/PORT access mode to Ingress or Egress PCL
*    configuration table perlookup.
*
* INPUTS:
*    devNum    - device number
*    portNum   - port number
*    direction - policy direction
*    lookupNum - lookup number
*    sublookup - sublookup 0 or 1 (relevant for DxCh Xcat only)
*    mode      - PCL Configuration Table access mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - one of the input parameters is not valid
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclPortLookupCfgTabAccessModeSet
(
    IN GT_U8                                        portNum,
    IN CPSS_PCL_DIRECTION_ENT                       direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT                   lookupNum,
    IN GT_U32                                       sublookup,
    IN PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT mode
)
{
    GT_U32  portIter = 0;
#ifdef CHX_FAMILY
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT      dxChMode;
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_ENT    exMxPmMode;
#endif /* EXMXPM_FAMILY */


    /* find port index */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortsArray[portIter] == portNum)
        {
            break;
        }
    }

    /* check if port is valid */
    if (prvTgfPortsArray[portIter] != portNum)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", portNum);

        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* convert table access mode into device specific format */
    switch (mode)
    {
        case PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E:
            dxChMode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E;
            break;

        case PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E:
            dxChMode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    return cpssDxChPclPortLookupCfgTabAccessModeSet(
        prvTgfDevsArray[portIter], portNum,
        direction, lookupNum, sublookup, dxChMode);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(sublookup);

    /* convert table access mode into device specific format */
    switch (mode)
    {
        case PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E:
            exMxPmMode = CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_BY_PORT_E;
            break;

        case PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E:
            exMxPmMode = CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_BY_VLAN_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    return cpssExMxPmPclPortLookupCfgTblAccessModeSet(prvTgfDevsArray[portIter], portNum,
                                                      direction, lookupNum, exMxPmMode);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* INPUTS:
*       devNum           - device number
*       interfaceInfoPtr - (pointer to) interface data
*       direction        - Policy direction
*       lookupNum        - Lookup number
*       lookupCfgPtr     - (pointer to) lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - one of the input parameters is not valid
*       GT_TIMEOUT   - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclCfgTblSet
(
    IN CPSS_INTERFACE_INFO_STC       *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT         direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT     lookupNum,
    IN PRV_TGF_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
    CPSS_INTERFACE_INFO_STC       interfaceInfo;
#ifdef CHX_FAMILY
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    dxChLookupCfg = {0};
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_PCL_LOOKUP_CFG_STC  exMxPmLookupCfg = {0};
#endif /* EXMXPM_FAMILY */

    cpssOsMemCpy(&interfaceInfo, interfaceInfoPtr, sizeof(interfaceInfo));

#ifdef CHX_FAMILY
    /* convert generic lookupCfg into device specific lookupCfg */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChLookupCfg, lookupCfgPtr, enableLookup);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChLookupCfg, lookupCfgPtr, pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChLookupCfg, lookupCfgPtr, dualLookup);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChLookupCfg, lookupCfgPtr, pclIdL01);

    /* convert generic groupKeyTypes into device specific groupKeyTypes */
    PRV_TGF_S2D_PCL_RULE_FORMAT_CONVERT_MAC(dxChLookupCfg.groupKeyTypes.nonIpKey,
                                            lookupCfgPtr->groupKeyTypes.nonIpKey);
    PRV_TGF_S2D_PCL_RULE_FORMAT_CONVERT_MAC(dxChLookupCfg.groupKeyTypes.ipv4Key,
                                            lookupCfgPtr->groupKeyTypes.ipv4Key);
    PRV_TGF_S2D_PCL_RULE_FORMAT_CONVERT_MAC(dxChLookupCfg.groupKeyTypes.ipv6Key,
                                            lookupCfgPtr->groupKeyTypes.ipv6Key);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        if( CPSS_INTERFACE_PORT_E == interfaceInfo.type )
        {
            rc = prvUtfHwFromSwDeviceNumberGet(interfaceInfo.devPort.devNum,
                                               &(interfaceInfo.devPort.devNum));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC(
                    "[TGF]: prvUtfHwFromSwDeviceNumberGet FAILED, rc = [%d]", rc);

                return rc;
            }
        }

        /* call device specific API */
        if(usePortGroupsBmp == GT_FALSE)
        {
            rc = cpssDxChPclCfgTblSet(devNum, &interfaceInfo, direction, lookupNum, &dxChLookupCfg);
        }
        else
        {
            rc = cpssDxChPclPortGroupCfgTblSet(devNum, currPortGroupsBmp, &interfaceInfo, direction, lookupNum, &dxChLookupCfg);
        }

        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPclPortGroupCfgTblSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* convert generic lookupCfg into device specific lookupCfg */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmLookupCfg, lookupCfgPtr, enableLookup);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmLookupCfg, lookupCfgPtr, externalLookup);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmLookupCfg, lookupCfgPtr, pclId);

    /* convert generic lookupType into device specific lookupType */
    switch (lookupCfgPtr->lookupType)
    {
        case PRV_TGF_PCL_LOOKUP_TYPE_SINGLE_E:
            exMxPmLookupCfg.lookupType = CPSS_EXMXPM_PCL_LOOKUP_TYPE_SINGLE_E;
            break;

        case PRV_TGF_PCL_LOOKUP_TYPE_DOUBLE_E:
            exMxPmLookupCfg.lookupType = CPSS_EXMXPM_PCL_LOOKUP_TYPE_DOUBLE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert generic groupKeyTypes into device specific groupKeyTypes */
    PRV_TGF_S2D_PCL_RULE_FORMAT_CONVERT_MAC(exMxPmLookupCfg.groupKeyTypes.nonIpKey,
                                            lookupCfgPtr->groupKeyTypes.nonIpKey);
    PRV_TGF_S2D_PCL_RULE_FORMAT_CONVERT_MAC(exMxPmLookupCfg.groupKeyTypes.ipv4Key,
                                            lookupCfgPtr->groupKeyTypes.ipv4Key);
    PRV_TGF_S2D_PCL_RULE_FORMAT_CONVERT_MAC(exMxPmLookupCfg.groupKeyTypes.ipv6Key,
                                            lookupCfgPtr->groupKeyTypes.ipv6Key);

    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmPclCfgTblSet(devNum, &interfaceInfo, direction, lookupNum, &exMxPmLookupCfg);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmPclCfgTblSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclRuleWithOptionsSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule
*       ruleIndex        - index of the rule in the TCAM
*       ruleOptionsBmp   - Bitmap of rule's options.
*       maskPtr          - (pointer to) rule mask
*       patternPtr       - (pointer to) rule pattern
*       actionPtr        - (pointer to) policy rule action
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong parameters
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_HW_ERROR     - on hardware error
*       GT_TIMEOUT      - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclRuleWithOptionsSet
(
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN GT_U32                             ruleIndex,
    IN PRV_TGF_PCL_RULE_OPTION_ENT        ruleOptionsBmp,
    IN PRV_TGF_PCL_RULE_FORMAT_UNT       *maskPtr,
    IN PRV_TGF_PCL_RULE_FORMAT_UNT       *patternPtr,
    IN PRV_TGF_PCL_ACTION_STC            *actionPtr
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  dxChRuleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       dxChMask;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       dxChPattern;
    CPSS_DXCH_PCL_ACTION_STC            dxChAction;
    CPSS_DXCH_PCL_RULE_OPTION_ENT       dxChRuleOptionsBmp;
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType         = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT   exMxPmRuleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;
    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT      actionFormat     = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;
    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT  exMxPmRuleInfo;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT        exMxPmMask;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT        exMxPmPattern;
    CPSS_EXMXPM_PCL_ACTION_UNT             exMxPmAction;
    GT_BOOL                                validRule;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    cpssOsMemSet(&dxChMask, 0, sizeof(dxChMask));
    cpssOsMemSet(&dxChPattern, 0, sizeof(dxChPattern));
    cpssOsMemSet(&dxChAction, 0, sizeof(dxChAction));
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
    cpssOsMemSet(&exMxPmRuleInfo, 0, sizeof(exMxPmRuleInfo));
    cpssOsMemSet(&exMxPmMask, 0, sizeof(exMxPmMask));
    cpssOsMemSet(&exMxPmPattern, 0, sizeof(exMxPmPattern));
    cpssOsMemSet(&exMxPmAction, 0, sizeof(exMxPmAction));
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    dxChRuleOptionsBmp = 0;
    if (ruleOptionsBmp & PRV_TGF_PCL_RULE_OPTION_WRITE_INVALID_E)
    {
        dxChRuleOptionsBmp |= CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E;
    }
    if (ruleOptionsBmp & PRV_TGF_PCL_RULE_OPTION_PORT_LIST_ENABLED_E)
    {
        dxChRuleOptionsBmp |= CPSS_DXCH_PCL_RULE_OPTION_PORT_LIST_ENABLED_E;
    }

    /* convert generic ruleFormat into device specific ruleFormat */
    PRV_TGF_S2D_PCL_RULE_FORMAT_CONVERT_MAC(dxChRuleFormat, ruleFormat);

    /* convert generic Pcl key into device specific Pcl key */
    switch (ruleFormat)
    {
        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChIngStdNotIp(&(patternPtr->ruleStdNotIp),
                                                       &(dxChPattern.ruleStdNotIp));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngStdNotIp FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChIngStdNotIp(&(maskPtr->ruleStdNotIp),
                                                       &(dxChMask.ruleStdNotIp));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngStdNotIp FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChIngStdIpL2Qos(&(patternPtr->ruleStdIpL2Qos),
                                                         &(dxChPattern.ruleStdIpL2Qos));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngStdIpL2Qos FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChIngStdIpL2Qos(&(maskPtr->ruleStdIpL2Qos),
                                                         &(dxChMask.ruleStdIpL2Qos));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngStdIpL2Qos FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChIngStdIpv4L4(&(patternPtr->ruleStdIpv4L4),
                                                        &(dxChPattern.ruleStdIpv4L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngStdIpv4L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChIngStdIpv4L4(&(maskPtr->ruleStdIpv4L4),
                                                        &(dxChMask.ruleStdIpv4L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngStdIpv4L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChIngStdIpv6Dip(&(patternPtr->ruleStdIpv6Dip),
                                                         &(dxChPattern.ruleStdIpv6Dip));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngStdIpv6Dip FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChIngStdIpv6Dip(&(maskPtr->ruleStdIpv6Dip),
                                                         &(dxChMask.ruleStdIpv6Dip));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngStdIpv6Dip FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChIngStdUdb(&(patternPtr->ruleStdUdb),
                                                     &(dxChPattern.ruleIngrStdUdb));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngStdUdb FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChIngStdUdb(&(maskPtr->ruleStdUdb),
                                                     &(dxChMask.ruleIngrStdUdb));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngStdUdb FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChIngExtNotIpv6(&(patternPtr->ruleExtNotIpv6),
                                                         &(dxChPattern.ruleExtNotIpv6));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngExtNotIpv6 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChIngExtNotIpv6(&(maskPtr->ruleExtNotIpv6),
                                                         &(dxChMask.ruleExtNotIpv6));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngExtNotIpv6 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChIngRuleExtIpv6L2(&(patternPtr->ruleExtIpv6L2),
                                                            &(dxChPattern.ruleExtIpv6L2));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngRuleExtIpv6L2 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChIngRuleExtIpv6L2(&(maskPtr->ruleExtIpv6L2),
                                                            &(dxChMask.ruleExtIpv6L2));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngRuleExtIpv6L2 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChIngRuleExtIpv6L4(&(patternPtr->ruleExtIpv6L4),
                                                            &(dxChPattern.ruleExtIpv6L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngRuleExtIpv6L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChIngRuleExtIpv6L4(&(maskPtr->ruleExtIpv6L4),
                                                            &(dxChMask.ruleExtIpv6L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngRuleExtIpv6L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChIngRuleExtUdb(&(patternPtr->ruleExtUdb),
                                                         &(dxChPattern.ruleIngrExtUdb));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngRuleExtUdb FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChIngRuleExtUdb(&(maskPtr->ruleExtUdb),
                                                         &(dxChMask.ruleIngrExtUdb));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIngRuleExtUdb FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChEgrRuleStdNotIp(&(patternPtr->ruleEgrStdNotIp),
                                                           &(dxChPattern.ruleEgrStdNotIp));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChEgrRuleStdNotIp FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChEgrRuleStdNotIp(&(maskPtr->ruleEgrStdNotIp),
                                                           &(dxChMask.ruleEgrStdNotIp));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChEgrRuleStdNotIp FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChEgrRuleStdIpL2Qos(&(patternPtr->ruleEgrStdIpL2Qos),
                                                             &(dxChPattern.ruleEgrStdIpL2Qos));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChEgrRuleStdIpL2Qos FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChEgrRuleStdIpL2Qos(&(maskPtr->ruleEgrStdIpL2Qos),
                                                             &(dxChMask.ruleEgrStdIpL2Qos));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChEgrRuleStdIpL2Qos FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChEgrRuleStdIpv4L4(&(patternPtr->ruleEgrStdIpv4L4),
                                                             &(dxChPattern.ruleEgrStdIpv4L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChEgrRuleStdIpv4L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChEgrRuleStdIpv4L4(&(maskPtr->ruleEgrStdIpv4L4),
                                                             &(dxChMask.ruleEgrStdIpv4L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChEgrRuleStdIpv4L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChEgrRuleExtNotIpv6(&(patternPtr->ruleEgrExtNotIpv6),
                                                             &(dxChPattern.ruleEgrExtNotIpv6));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChEgrRuleExtNotIpv6 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChEgrRuleExtNotIpv6(&(maskPtr->ruleEgrExtNotIpv6),
                                                             &(dxChMask.ruleEgrExtNotIpv6));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChEgrRuleExtNotIpv6 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChEgrRuleExtIpv6L2(&(patternPtr->ruleEgrExtIpv6L2),
                                                             &(dxChPattern.ruleEgrExtIpv6L2));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChEgrRuleExtIpv6L2 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChEgrRuleExtIpv6L2(&(maskPtr->ruleEgrExtIpv6L2),
                                                             &(dxChMask.ruleEgrExtIpv6L2));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChEgrRuleExtIpv6L2 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
        default:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToDxChEgrRuleExtIpv6L4(&(patternPtr->ruleEgrExtIpv6L4),
                                                             &(dxChPattern.ruleEgrExtIpv6L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChEgrRuleExtIpv6L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToDxChEgrRuleExtIpv6L4(&(maskPtr->ruleEgrExtIpv6L4),
                                                             &(dxChMask.ruleEgrExtIpv6L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChEgrRuleExtIpv6L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;
    }

    /* convert rule Action into device specific format */
    rc = prvTgfConvertGenericToDxChRuleAction(actionPtr, &dxChAction);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChRuleAction FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        if(usePortGroupsBmp == GT_FALSE)
        {
            rc = cpssDxChPclRuleSet(
                devNum, dxChRuleFormat, ruleIndex, dxChRuleOptionsBmp,
                &dxChMask, &dxChPattern, &dxChAction);
        }
        else
        {
            rc = cpssDxChPclPortGroupRuleSet(
                devNum, currPortGroupsBmp, dxChRuleFormat,
                ruleIndex, dxChRuleOptionsBmp,
                &dxChMask, &dxChPattern, &dxChAction);
        }

        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPclPortGroupRuleSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY

    validRule = GT_TRUE;
    if (ruleOptionsBmp & PRV_TGF_PCL_RULE_OPTION_WRITE_INVALID_E)
    {
        validRule = GT_FALSE;
    }

    /* set TCAM */
    tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    exMxPmRuleInfo.internalTcamRuleStartIndex = ruleIndex;

    /* convert generic ruleFormat into device specific ruleFormat */
    PRV_TGF_S2D_PCL_RULE_FORMAT_CONVERT_MAC(exMxPmRuleFormat, ruleFormat);

    /* set action format */
    actionFormat = (GT_TRUE == actionPtr->egressPolicy) ?
                    CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E :
                    CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;

    /* convert generic Pcl key into device specific Pcl key */
    switch (ruleFormat)
    {
        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmIngStdNotIp(&(patternPtr->ruleStdNotIp),
                                                         &(exMxPmPattern.ruleIngrStandardNotIp));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngStdNotIp FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmIngStdNotIp(&(maskPtr->ruleStdNotIp),
                                                         &(exMxPmMask.ruleIngrStandardNotIp));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngStdNotIp FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmIngStdIpL2Qos(&(patternPtr->ruleStdIpL2Qos),
                                                           &(exMxPmPattern.ruleIngrStandardIpL2Qos));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngStdIpL2Qos FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmIngStdIpL2Qos(&(maskPtr->ruleStdIpL2Qos),
                                                           &(exMxPmMask.ruleIngrStandardIpL2Qos));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngStdIpL2Qos FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmIngStdIpv4L4(&(patternPtr->ruleStdIpv4L4),
                                                          &(exMxPmPattern.ruleIngrStandardIpv4L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngStdIpv4L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmIngStdIpv4L4(&(maskPtr->ruleStdIpv4L4),
                                                          &(exMxPmMask.ruleIngrStandardIpv4L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngStdIpv4L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmIngExtNotIpv6(&(patternPtr->ruleExtNotIpv6),
                                                           &(exMxPmPattern.ruleIngrExtendedNotIpv6));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngExtNotIpv6 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmIngExtNotIpv6(&(maskPtr->ruleExtNotIpv6),
                                                           &(exMxPmMask.ruleIngrExtendedNotIpv6));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngExtNotIpv6 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmIngRuleExtIpv6L2(&(patternPtr->ruleExtIpv6L2),
                                                              &(exMxPmPattern.ruleIngrExtendedIpv6L2));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngRuleExtIpv6L2 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmIngRuleExtIpv6L2(&(maskPtr->ruleExtIpv6L2),
                                                              &(exMxPmMask.ruleIngrExtendedIpv6L2));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngRuleExtIpv6L2 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmIngRuleExtIpv6L4(&(patternPtr->ruleExtIpv6L4),
                                                              &(exMxPmPattern.ruleIngrExtendedIpv6L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngRuleExtIpv6L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmIngRuleExtIpv6L4(&(maskPtr->ruleExtIpv6L4),
                                                              &(exMxPmMask.ruleIngrExtendedIpv6L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngRuleExtIpv6L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmEgrRuleStdNotIp(&(patternPtr->ruleEgrStdNotIp),
                                                             &(exMxPmPattern.ruleEgrStandardNotIp));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmEgrRuleStdNotIp FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmEgrRuleStdNotIp(&(maskPtr->ruleEgrStdNotIp),
                                                             &(exMxPmMask.ruleEgrStandardNotIp));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmEgrRuleStdNotIp FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmEgrRuleStdIpL2Qos(&(patternPtr->ruleEgrStdIpL2Qos),
                                                               &(exMxPmPattern.ruleEgrStandardIpL2Qos));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmEgrRuleStdIpL2Qos FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmEgrRuleStdIpL2Qos(&(maskPtr->ruleEgrStdIpL2Qos),
                                                               &(exMxPmMask.ruleEgrStandardIpL2Qos));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmEgrRuleStdIpL2Qos FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmEgrRuleStdIpv4L4(&(patternPtr->ruleEgrStdIpv4L4),
                                                              &(exMxPmPattern.ruleEgrStandardIpv4L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmEgrRuleStdIpv4L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmEgrRuleStdIpv4L4(&(maskPtr->ruleEgrStdIpv4L4),
                                                              &(exMxPmMask.ruleEgrStandardIpv4L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmEgrRuleStdIpv4L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmEgrRuleExtNotIpv6(&(patternPtr->ruleEgrExtNotIpv6),
                                                               &(exMxPmPattern.ruleEgrExtendedNotIpv6));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmEgrRuleExtNotIpv6 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmEgrRuleExtNotIpv6(&(maskPtr->ruleEgrExtNotIpv6),
                                                               &(exMxPmMask.ruleEgrExtendedNotIpv6));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmEgrRuleExtNotIpv6 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmEgrRuleExtIpv6L2(&(patternPtr->ruleEgrExtIpv6L2),
                                                              &(exMxPmPattern.ruleEgrExtendedIpv6L2));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmEgrRuleExtIpv6L2 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmEgrRuleExtIpv6L2(&(maskPtr->ruleEgrExtIpv6L2),
                                                              &(exMxPmMask.ruleEgrExtendedIpv6L2));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmEgrRuleExtIpv6L2 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmEgrRuleExtIpv6L4(&(patternPtr->ruleEgrExtIpv6L4),
                                                              &(exMxPmPattern.ruleEgrExtendedIpv6L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmEgrRuleExtIpv6L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmEgrRuleExtIpv6L4(&(maskPtr->ruleEgrExtIpv6L4),
                                                              &(exMxPmMask.ruleEgrExtendedIpv6L4));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmEgrRuleExtIpv6L4 FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmIngStdUdb(&(patternPtr->ruleStdUdb),
                                                       &(exMxPmPattern.ruleIngrStandardUdb));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngStdUdb FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmIngStdUdb(&(maskPtr->ruleStdUdb),
                                                       &(exMxPmMask.ruleIngrStandardUdb));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngStdUdb FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmIngRuleExtUdb(&(patternPtr->ruleExtUdb),
                                                           &(exMxPmPattern.ruleIngrExtendedUdb));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngRuleExtUdb FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmIngRuleExtUdb(&(maskPtr->ruleExtUdb),
                                                           &(exMxPmMask.ruleIngrExtendedUdb));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngRuleExtUdb FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_80B_E:
            /* convert into device specific Pattern */
            rc = prvTgfConvertGenericToExMxPmIngRuleExtUdb80b(&(patternPtr->ruleExtUdb80B),
                                                              &(exMxPmPattern.ruleIngrExternalUdb80B));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngRuleExtUdb80b FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert into device specific Mask */
            rc = prvTgfConvertGenericToExMxPmIngRuleExtUdb80b(&(maskPtr->ruleExtUdb80B),
                                                              &(exMxPmMask.ruleIngrExternalUdb80B));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIngRuleExtUdb80b FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[dead_error_begin] */
        default:
            return GT_BAD_PARAM;
    }

    /* convert rule Action into device specific format */
    rc = prvTgfConvertGenericToExMxPmRuleAction(actionPtr, &exMxPmAction);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmRuleAction FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmPclRuleSet(
            devNum, tcamType, &exMxPmRuleInfo,
            exMxPmRuleFormat, actionFormat, validRule,
            &exMxPmMask, &exMxPmPattern, &exMxPmAction);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmPclRuleSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
* INPUTS:
*       devNum     - device number
*       ruleFormat - format of the Rule
*       ruleIndex  - index of the rule in the TCAM
*       maskPtr    - (pointer to) rule mask
*       patternPtr - (pointer to) rule pattern
*       actionPtr  - (pointer to) policy rule action
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong parameters
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_HW_ERROR     - on hardware error
*       GT_TIMEOUT      - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclRuleSet
(
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN GT_U32                             ruleIndex,
    IN PRV_TGF_PCL_RULE_FORMAT_UNT       *maskPtr,
    IN PRV_TGF_PCL_RULE_FORMAT_UNT       *patternPtr,
    IN PRV_TGF_PCL_ACTION_STC            *actionPtr
)
{
    return prvTgfPclRuleWithOptionsSet(
        ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
        maskPtr, patternPtr, actionPtr);
}

/*******************************************************************************
* prvTgfPclRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the Rule Action
*
* INPUTS:
*       devNum    - device number
*       ruleSize  - size of Rule
*       ruleIndex - index of the rule in the TCAM
*       actionPtr - policy rule action that applied on packet
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong parameters
*       GT_BAD_PTR      - on null pointer
*       GT_OUT_OF_RANGE - on the parameters is out of range
*       GT_HW_ERROR     - on hardware error
*       GT_TIMEOUT      - after max number of retries checking if PP ready
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS prvTgfPclRuleActionUpdate
(
    IN CPSS_PCL_RULE_SIZE_ENT         ruleSize,
    IN GT_U32                         ruleIndex,
    IN PRV_TGF_PCL_ACTION_STC        *actionPtr
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_PCL_ACTION_STC            dxChAction;
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_TCAM_TYPE_ENT           tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    CPSS_EXMXPM_TCAM_ACTION_INFO_UNT    actionInfo;
    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT   actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;
    CPSS_EXMXPM_PCL_ACTION_UNT          exMxPmAction;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* convert rule Action into device specific format */
    rc = prvTgfConvertGenericToDxChRuleAction(actionPtr, &dxChAction);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChRuleAction FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        if(usePortGroupsBmp == GT_FALSE)
        {
            rc = cpssDxChPclRuleActionUpdate(devNum, ruleSize, ruleIndex, &dxChAction);
        }
        else
        {
            rc = cpssDxChPclPortGroupRuleActionUpdate(devNum, currPortGroupsBmp, ruleSize, ruleIndex, &dxChAction);
        }

        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPclPortGroupRuleActionUpdate FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(ruleSize);

    /* set TCAM */
    tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    actionInfo.internalTcamRuleStartIndex = ruleIndex;

    /* set action format */
    actionFormat = (GT_TRUE == actionPtr->egressPolicy) ?
                    CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E :
                    CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;


    /* convert rule Action into device specific format */
    rc = prvTgfConvertGenericToExMxPmRuleAction(actionPtr, &exMxPmAction);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmRuleAction FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmPclRuleActionSet(devNum, tcamType, &actionInfo, actionFormat, &exMxPmAction);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmPclRuleActionSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclRuleValidStatusSet
*
* DESCRIPTION:
*     Validates/Invalidates the Policy rule
*
* INPUTS:
*       devNum    - device number
*       ruleSize  - size of Rule
*       ruleIndex - index of the rule in the TCAM.
*       valid     - new rule status: GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - on TCAM found rule of different size
*       GT_TIMEOUT   - on max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclRuleValidStatusSet
(
    IN CPSS_PCL_RULE_SIZE_ENT         ruleSize,
    IN GT_U32                         ruleIndex,
    IN GT_BOOL                        valid
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum = 0;
    GT_STATUS   rc     = GT_OK;
    GT_STATUS   rc1    = GT_OK;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        if(usePortGroupsBmp == GT_FALSE)
        {
            rc = cpssDxChPclRuleValidStatusSet(devNum, ruleSize, ruleIndex, valid);
        }
        else
        {
            rc = cpssDxChPclPortGroupRuleValidStatusSet(devNum, currPortGroupsBmp, ruleSize, ruleIndex, valid);
        }

        if (GT_OK != rc)
        {
            if(valid == 0 && rc == GT_BAD_STATE)
            {
                /* when entry is not initialized yet , this API will return
                    GT_BAD_STATE , but is seems logically to return GT_OK for the
                    'Invalid action' '*/
            }
            else
            {
                PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPclPortGroupRuleValidStatusSet FAILED, rc = [%d]", rc);

                rc1 = rc;
            }
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(ruleSize);
    TGF_PARAM_NOT_USED(ruleIndex);
    TGF_PARAM_NOT_USED(valid);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/* array of UDB (cfg, member) */
/* Should be synchronized with PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT */
static GT_U8    prvTgfUdbIndexArr[][2] =
{
     {3, 15}, /* PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E */
     {2, 18}, /* PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E */
     {3, 20}, /* PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E */
     {0,  0}, /* PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E */
     {6,  0}, /* PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E */
     {6,  6}, /* PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E */
     {3, 12}, /* PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E */
     {0,  0}, /* PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E */
     {0,  0}, /* PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E */
     {0,  0}, /* PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E */
     {0,  0}, /* PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E */
     {0,  0}, /* PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E */
     {0,  0}, /* PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E */
     {16, 0}, /* PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E */
     {16, 0}, /* PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E */
     {0,  0}  /* PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_80B_E */
};

/*******************************************************************************
* prvTgfPclUdbIndexConvert
*
* DESCRIPTION:
*   The function converts the UDB relative index (in rule format)
*   to UDB absolute index in UDB configuration table entry
*   Conversion needed for XCAT and Puma devices, not needed for CH1-3
*
* INPUTS:
*       ruleFormat - rule format
*       packetType - packet Type
*       udbIndex   - relative index of UDB in the rule
*
* OUTPUTS:
*       udbAbsIndexPtr - (pointer to) UDB absolute index in
*                        UDB configuration table entry
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - on parameter value more then HW bit field
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclUdbIndexConvert
(
    IN  PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT     ruleFormat,
    IN  GT_U32                               udbRelIndex,
    OUT GT_U32                               *udbAbsIndexPtr
)
{
    GT_STATUS   rc     = GT_OK;
    GT_U8       devNum;


#ifdef EXMXPM_FAMILY
    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);
#endif /* CHX_FAMILY */

    /* device found */
    if (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        if (udbRelIndex >= prvTgfUdbIndexArr[ruleFormat][0])
        {
            return GT_OUT_OF_RANGE;
        }

        *udbAbsIndexPtr = prvTgfUdbIndexArr[ruleFormat][1] + udbRelIndex;
        return GT_OK;
    }

    /* default */
    *udbAbsIndexPtr = udbRelIndex;
    return GT_OK;

}

/*******************************************************************************
* prvTgfPclUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* INPUTS:
*       ruleFormat - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType - packet Type
*                      Relevant for DxChXCat and above devices
*       udbIndex   - index of User Defined Byte to configure
*       offsetType - the type of offset
*       offset     - The offset of the user-defined byte
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong parameter
*       GT_OUT_OF_RANGE - on parameter value more then HW bit field
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclUserDefinedByteSet
(
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT     ruleFormat,
    IN PRV_TGF_PCL_PACKET_TYPE_ENT          packetType,
    IN GT_U32                               udbIndex,
    IN PRV_TGF_PCL_OFFSET_TYPE_ENT          offsetType,
    IN GT_U8                                offset
)
{
    GT_U8       devNum = 0;
    GT_STATUS   rc     = GT_OK;
    GT_STATUS   rc1    = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  dxChRuleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    CPSS_DXCH_PCL_PACKET_TYPE_ENT       dxChPacketType = CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E;
    CPSS_DXCH_PCL_OFFSET_TYPE_ENT       dxChOffsetType = CPSS_DXCH_PCL_OFFSET_L2_E;
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_PCL_UDB_TABLE_ENTRY_STC exMxPmUdbCfg;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* convert generic ruleFormat into device specific ruleFormat */
    PRV_TGF_S2D_PCL_RULE_FORMAT_CONVERT_MAC(dxChRuleFormat, ruleFormat);

    /* convert generic into device specific packetType */
    switch (packetType)
    {
        case PRV_TGF_PCL_PACKET_TYPE_IPV4_TCP_E:
            dxChPacketType = CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E;
            break;

        case PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E:
            dxChPacketType = CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E;
            break;

        case PRV_TGF_PCL_PACKET_TYPE_MPLS_E:
            dxChPacketType = CPSS_DXCH_PCL_PACKET_TYPE_MPLS_E;
            break;

        case PRV_TGF_PCL_PACKET_TYPE_IPV4_FRAGMENT_E:
            dxChPacketType = CPSS_DXCH_PCL_PACKET_TYPE_IPV4_FRAGMENT_E;
            break;

        case PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E:
            dxChPacketType = CPSS_DXCH_PCL_PACKET_TYPE_IPV4_OTHER_E;
            break;

        case PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E:
            dxChPacketType = CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E;
            break;

        case PRV_TGF_PCL_PACKET_TYPE_UDE_E:
            dxChPacketType = CPSS_DXCH_PCL_PACKET_TYPE_UDE_E;
            break;

        case PRV_TGF_PCL_PACKET_TYPE_IPV6_E:
            dxChPacketType = CPSS_DXCH_PCL_PACKET_TYPE_IPV6_E;
            break;

        case PRV_TGF_PCL_PACKET_TYPE_UDE_1_E:
            dxChPacketType = CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E;
            break;

        case PRV_TGF_PCL_PACKET_TYPE_UDE_2_E:
            dxChPacketType = CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E;
            break;

        case PRV_TGF_PCL_PACKET_TYPE_UDE_3_E:
            dxChPacketType = CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E;
            break;

        case PRV_TGF_PCL_PACKET_TYPE_UDE_4_E:
            dxChPacketType = CPSS_DXCH_PCL_PACKET_TYPE_UDE4_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert generic into device specific offsetType */
    switch (offsetType)
    {
        case PRV_TGF_PCL_OFFSET_L2_E:
            dxChOffsetType = CPSS_DXCH_PCL_OFFSET_L2_E;
            break;

        case PRV_TGF_PCL_OFFSET_L3_E:
            dxChOffsetType = CPSS_DXCH_PCL_OFFSET_L3_E;
            break;

        case PRV_TGF_PCL_OFFSET_L4_E:
            dxChOffsetType = CPSS_DXCH_PCL_OFFSET_L4_E;
            break;

        case PRV_TGF_PCL_OFFSET_IPV6_EXT_HDR_E:
            dxChOffsetType = CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E;
            break;

        case PRV_TGF_PCL_OFFSET_TCP_UDP_COMPARATOR_E:
            dxChOffsetType = CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E;
            break;

        case PRV_TGF_PCL_OFFSET_L3_MINUS_2_E:
            dxChOffsetType = CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E;
            break;

        case PRV_TGF_PCL_OFFSET_MPLS_MINUS_2_E:
            dxChOffsetType = CPSS_DXCH_PCL_OFFSET_MPLS_MINUS_2_E;
            break;

        case PRV_TGF_PCL_OFFSET_INVALID_E:
            dxChOffsetType = CPSS_DXCH_PCL_OFFSET_INVALID_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        if(usePortGroupsBmp == GT_FALSE)
        {
            rc = cpssDxChPclUserDefinedByteSet(devNum, dxChRuleFormat, packetType, udbIndex, offsetType, offset);
        }
        else
        {
            rc = cpssDxChPclPortGroupUserDefinedByteSet(devNum, currPortGroupsBmp, dxChRuleFormat, packetType, udbIndex, offsetType, offset);
        }

        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPclPortGroupUserDefinedByteSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    cpssOsBzero((char *)&exMxPmUdbCfg, sizeof(exMxPmUdbCfg));
    /* avoid warnings */
    TGF_PARAM_NOT_USED(ruleFormat);
    TGF_PARAM_NOT_USED(packetType);
    TGF_PARAM_NOT_USED(offsetType);
    TGF_PARAM_NOT_USED(offset);

    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmPclUdbTableEntrySet(devNum, udbIndex, &exMxPmUdbCfg);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmPclUdbTableEntrySet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclOverrideUserDefinedBytesSet
*
* DESCRIPTION:
*   The function sets overriding of  User Defined Bytes by predefined key fields
*
* INPUTS:
*       devNum         - device number
*       udbOverridePtr - (pointer to) UDB override structure
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameter
*       GT_BAD_PTR   - on null pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclOverrideUserDefinedBytesSet
(
    IN  PRV_TGF_PCL_OVERRIDE_UDB_STC *udbOverridePtr
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum = 0;
    GT_STATUS   rc     = GT_OK;
    GT_STATUS   rc1    = GT_OK;
    CPSS_DXCH_PCL_OVERRIDE_UDB_STC  dxChUdbOverride;


    /* convert generic into device specific udbOverridePtr */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdLsbEnableStdNotIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdMsbEnableStdNotIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdLsbEnableStdIpL2Qos);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdMsbEnableStdIpL2Qos);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdLsbEnableStdIpv4L4);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdMsbEnableStdIpv4L4);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdLsbEnableStdUdb);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdMsbEnableStdUdb);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdLsbEnableExtNotIpv6);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdMsbEnableExtNotIpv6);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdLsbEnableExtIpv6L2);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdMsbEnableExtIpv6L2);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdLsbEnableExtIpv6L4);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdMsbEnableExtIpv6L4);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdLsbEnableExtUdb);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, vrfIdMsbEnableExtUdb);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, qosProfileEnableStdUdb);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChUdbOverride), udbOverridePtr, qosProfileEnableExtUdb);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        if(usePortGroupsBmp == GT_FALSE)
        {
            rc = cpssDxChPclOverrideUserDefinedBytesSet(devNum, &dxChUdbOverride);
        }
        else
        {
            rc = cpssDxChPclPortGroupOverrideUserDefinedBytesSet(devNum, currPortGroupsBmp, &dxChUdbOverride);
        }

        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPclPortGroupOverrideUserDefinedBytesSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(udbOverridePtr);

    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclInvalidUdbCmdSet
*
* DESCRIPTION:
*    Set the command that is applied when the policy key <User-Defined>
*    field cannot be extracted from the packet due to lack of header depth
*
* INPUTS:
*    udbErrorCmd - command applied to a packet
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on one of the input parameters is not valid
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclInvalidUdbCmdSet
(
    IN PRV_TGF_UDB_ERROR_CMD_ENT    udbErrorCmd
)
{
    GT_U8       devNum = 0;
    GT_STATUS   rc     = GT_OK;
    GT_STATUS   rc1    = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_UDB_ERROR_CMD_ENT  dxChUdbErrorCmd = CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E;
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_UDB_ERROR_CMD_ENT  exMxPmUdbErrorCmd = CPSS_EXMXPM_UDB_ERROR_CMD_LOOKUP_E;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* convert generic into device specific udbErrorCmd */
    switch (udbErrorCmd)
    {
        case PRV_TGF_UDB_ERROR_CMD_LOOKUP_E:
            dxChUdbErrorCmd = CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E;
            break;

        case PRV_TGF_UDB_ERROR_CMD_TRAP_TO_CPU_E:
            dxChUdbErrorCmd = CPSS_DXCH_UDB_ERROR_CMD_TRAP_TO_CPU_E;
            break;

        case PRV_TGF_UDB_ERROR_CMD_DROP_HARD_E:
            dxChUdbErrorCmd = CPSS_DXCH_UDB_ERROR_CMD_DROP_HARD_E;
            break;

        case PRV_TGF_UDB_ERROR_CMD_DROP_SOFT_E:
            dxChUdbErrorCmd = CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChPclInvalidUdbCmdSet(devNum, dxChUdbErrorCmd);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPclInvalidUdbCmdSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* convert generic into device specific udbErrorCmd */
    switch (udbErrorCmd)
    {
        case PRV_TGF_UDB_ERROR_CMD_LOOKUP_E:
            exMxPmUdbErrorCmd = CPSS_EXMXPM_UDB_ERROR_CMD_LOOKUP_E;
            break;

        case PRV_TGF_UDB_ERROR_CMD_TRAP_TO_CPU_E:
            exMxPmUdbErrorCmd = CPSS_EXMXPM_UDB_ERROR_CMD_TRAP_TO_CPU_E;
            break;

        case PRV_TGF_UDB_ERROR_CMD_DROP_HARD_E:
            exMxPmUdbErrorCmd = CPSS_EXMXPM_UDB_ERROR_CMD_DROP_HARD_E;
            break;

        case PRV_TGF_UDB_ERROR_CMD_DROP_SOFT_E:
            exMxPmUdbErrorCmd = CPSS_EXMXPM_UDB_ERROR_CMD_DROP_SOFT_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmPclInvalidUdbCmdSet(devNum, exMxPmUdbErrorCmd);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmPclInvalidUdbCmdSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}


/******************************************************************************\
 *                       API for default settings                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfPclDefPortInitExt1
*
* DESCRIPTION:
*     Initialize PCL Engine
*
* INPUTS:
*       portNum   - port number
*       direction - PCL_DIRECTION
*       lookupNum - PCL_LOOKUP_NUMBER
*       pclId     - pcl Id
*       nonIpKey  - PCL_RULE_FORMAT_TYPE for PCL_LOOKUP_CFG_TABLE
*       ipv4Key   - PCL_RULE_FORMAT_TYPE for PCL_LOOKUP_CFG_TABLE
*       ipv6Key   - PCL_RULE_FORMAT_TYPE for PCL_LOOKUP_CFG_TABLE
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*       GT_FAIL      - on general failure error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclDefPortInitExt1
(
    IN GT_U8                            portNum,
    IN CPSS_PCL_DIRECTION_ENT           direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT       lookupNum,
    IN GT_U32                           pclId,
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT nonIpKey,
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT ipv4Key,
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT ipv6Key
)
{
    GT_STATUS                        rc = GT_OK;
    static GT_BOOL                   isDeviceInited = GT_FALSE;
    CPSS_INTERFACE_INFO_STC          interfaceInfo;
    PRV_TGF_PCL_LOOKUP_CFG_STC       lookupCfg;
    PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT accessMode = PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E;

    if (GT_FALSE == isDeviceInited)
    {
        /* init PCL */
        rc = prvTgfPclInit();
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPclInit FAILED, rc = [%d]", rc);

            return rc;
        }

        /* do not init next time */
        isDeviceInited = GT_TRUE;
    }

    if (direction == CPSS_PCL_DIRECTION_INGRESS_E)
    {
        /* enables ingress policy per devices */
        rc = prvTgfPclIngressPolicyEnable(GT_TRUE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPclIngressPolicyEnable FAILED, rc = [%d]", rc);

            return rc;
        }

        /* enables ingress policy */
        rc = prvTgfPclPortIngressPolicyEnable(portNum, GT_TRUE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPclPortIngressPolicyEnable FAILED, rc = [%d]", rc);

            return rc;
        }
    }
    else
    {
        /* enables egress policy per devices */
        rc = prvTgfPclEgressPolicyEnable(GT_TRUE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPclEgressPolicyEnable FAILED, rc = [%d]", rc);

            return rc;
        }
        /* enables egress policy per devices */
        rc = prvTgfPclEgressPclPacketTypesSet(
            prvTgfDevNum, portNum,
            PRV_TGF_PCL_EGRESS_PKT_NON_TS_E, GT_TRUE);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfPclEgressPclPacketTypesSet FAILED, rc = [%d]", rc);

            return rc;
        }
    }

    /* configure access accessMode */
    accessMode = PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E;

    rc = prvTgfPclPortLookupCfgTabAccessModeSet(
        portNum, direction, lookupNum, 0 /*sublookup*/, accessMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfPclPortLookupCfgTabAccessModeSet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* set PCL configuration table */
    cpssOsMemSet(&interfaceInfo, 0, sizeof(interfaceInfo));
    cpssOsMemSet(&lookupCfg, 0, sizeof(lookupCfg));

    interfaceInfo.type               = CPSS_INTERFACE_PORT_E;
    interfaceInfo.devPort.devNum     = prvTgfDevNum;
    interfaceInfo.devPort.portNum    = portNum;
    lookupCfg.enableLookup           = GT_TRUE;
    lookupCfg.pclId                  = pclId;
    lookupCfg.groupKeyTypes.nonIpKey = nonIpKey;
    lookupCfg.groupKeyTypes.ipv4Key  = ipv4Key;
    lookupCfg.groupKeyTypes.ipv6Key  = ipv6Key;

    rc = prvTgfPclCfgTblSet(
        &interfaceInfo, direction, lookupNum, &lookupCfg);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfPclCfgTblSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
}

/*******************************************************************************
* prvTgfPclDefPortInit
*
* DESCRIPTION:
*     Initialize PCL Engine
*
* INPUTS:
*       portNum   - port number
*       direction - PCL_DIRECTION
*       lookupNum - PCL_LOOKUP_NUMBER
*       nonIpKey  - PCL_RULE_FORMAT_TYPE for PCL_LOOKUP_CFG_TABLE
*       ipv4Key   - PCL_RULE_FORMAT_TYPE for PCL_LOOKUP_CFG_TABLE
*       ipv6Key   - PCL_RULE_FORMAT_TYPE for PCL_LOOKUP_CFG_TABLE
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*       GT_FAIL      - on general failure error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclDefPortInit
(
    IN GT_U8                            portNum,
    IN CPSS_PCL_DIRECTION_ENT           direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT       lookupNum,
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT nonIpKey,
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT ipv4Key,
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT ipv6Key
)
{
    return prvTgfPclDefPortInitExt1(
        portNum, direction, lookupNum,
        PRV_TGF_PCL_DEFAULT_ID_MAC(direction, lookupNum, portNum),
        nonIpKey, ipv4Key, ipv6Key);
}

/*******************************************************************************
* prvTgfPclUdeEtherTypeSet
*
* DESCRIPTION:
*       This function sets UDE Ethertype.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index, range 0..5
*       ethType       - Ethertype value (range 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - on out of range parameter value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclUdeEtherTypeSet
(
    IN  GT_U32          index,
    IN  GT_U32          ethType
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChPclUdeEtherTypeSet(devNum, index, ethType);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChPclUdeEtherTypeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(index);
    TGF_PARAM_NOT_USED(ethType);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclOverrideUserDefinedBytesByTrunkHashSet
*
* DESCRIPTION:
*   The function sets overriding of  User Defined Bytes
*   by packets Trunk Hash value.
*
* INPUTS:
*       udbOverTrunkHashPtr - (pointer to) UDB override trunk hash structure
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclOverrideUserDefinedBytesByTrunkHashSet
(
    IN  PRV_TGF_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
    CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC udbOverTrunkHash;

    udbOverTrunkHash.trunkHashEnableStdNotIp =
        udbOverTrunkHashPtr->trunkHashEnableStdNotIp;
    udbOverTrunkHash.trunkHashEnableStdIpv4L4 =
        udbOverTrunkHashPtr->trunkHashEnableStdIpv4L4;
    udbOverTrunkHash.trunkHashEnableStdUdb =
        udbOverTrunkHashPtr->trunkHashEnableStdUdb;
    udbOverTrunkHash.trunkHashEnableExtNotIpv6 =
        udbOverTrunkHashPtr->trunkHashEnableExtNotIpv6;
    udbOverTrunkHash.trunkHashEnableExtIpv6L2 =
        udbOverTrunkHashPtr->trunkHashEnableExtIpv6L2;
    udbOverTrunkHash.trunkHashEnableExtIpv6L4 =
        udbOverTrunkHashPtr->trunkHashEnableExtIpv6L4;
    udbOverTrunkHash.trunkHashEnableExtUdb =
        udbOverTrunkHashPtr->trunkHashEnableExtUdb;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet(
            devNum, &udbOverTrunkHash);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC(
                "[TGF]: cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(udbOverTrunkHashPtr);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclEgressKeyFieldsVidUpModeSet
*
* DESCRIPTION:
*       Sets Egress Policy VID and UP key fields content mode
*
* INPUTS:
*       vidUpMode      - VID and UP key fields content mode
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclEgressKeyFieldsVidUpModeSet
(
    IN  PRV_TGF_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
    CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT cpssVidUpMode;

    switch (vidUpMode)
    {
        case PRV_TGF_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E:
            cpssVidUpMode = CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E;
            break;
        case PRV_TGF_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E:
        cpssVidUpMode = CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E;
            break;
        default: return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChPclEgressKeyFieldsVidUpModeSet(
            devNum, cpssVidUpMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC(
                "[TGF]: cpssDxChPclEgressKeyFieldsVidUpModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(vidUpMode);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclLookupCfgPortListEnableSet
*
* DESCRIPTION:
*        The function enables/disables using port-list in search keys.
*
* INPUTS:
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*    enable            - GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclLookupCfgPortListEnableSet
(
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(
        &devNum, rc, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E
        | UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChPclLookupCfgPortListEnableSet(
            devNum, direction, lookupNum, subLookupNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC(
                "[TGF]: cpssDxChPclLookupCfgPortListEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(direction);
    TGF_PARAM_NOT_USED(lookupNum);
    TGF_PARAM_NOT_USED(subLookupNum);
    TGF_PARAM_NOT_USED(enable);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfPclRestore
*
* DESCRIPTION:
*       Function clears pcl settings.
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
GT_VOID prvTgfPclRestore
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;
    CPSS_INTERFACE_INFO_STC          interfaceInfo;
    PRV_TGF_PCL_LOOKUP_CFG_STC       lookupCfg;
    CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum;
    GT_U32                          portIndex;

    /* remove PCL configurations */
    cpssOsMemSet(&interfaceInfo, 0, sizeof(interfaceInfo));
    cpssOsMemSet(&lookupCfg, 0, sizeof(lookupCfg));

    interfaceInfo.type               = CPSS_INTERFACE_PORT_E;
    interfaceInfo.devPort.devNum     = prvTgfDevNum;
    lookupCfg.enableLookup           = GT_FALSE;  /* --> disable the PCL lookup */
    lookupCfg.pclId                  = 0;
    lookupCfg.groupKeyTypes.nonIpKey = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E /*nonIpKey*/;
    lookupCfg.groupKeyTypes.ipv4Key  = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E /*ipv4Key*/;
    lookupCfg.groupKeyTypes.ipv6Key  = PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E /*ipv6Key*/;

    for(portIndex = 0 ; portIndex < prvTgfPortsNum ;portIndex++)
    {
        interfaceInfo.devPort.portNum    = prvTgfPortsArray[portIndex];

        for(lookupNum = 0 ; lookupNum < 2 ; lookupNum ++)
        {
            rc = prvTgfPclCfgTblSet(
                &interfaceInfo, CPSS_PCL_DIRECTION_INGRESS_E, lookupNum, &lookupCfg);
            if (GT_OK != rc)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "[TGF]: prvTgfPclCfgTblSet FAILED: direction:%d, lookup:%d",
                                 CPSS_PCL_DIRECTION_INGRESS_E, lookupNum);
            }
        }
    }
}

/*******************************************************************************
* prvTgfPclPolicerCheck
*
* DESCRIPTION:
*    check if the device supports the policer pointed from the PCL/TTI
* INPUTS:
*       none
*
* RETURNS:
*       GT_TRUE - the device supports the policer pointed from the PCL/TTI
*       GT_FALSE - the device not supports the policer pointed from the PCL/TTI
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvTgfPclPolicerCheck(
    void
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc;
    PRV_TGF_IP_ROUTING_MODE_ENT     routingMode;
    /* get routing mode */
    rc = prvTgfIpRoutingModeGet(&routingMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC(
            "[TGF]: prvTgfIpRoutingModeGet FAILED, rc = [%d]", rc);
    }

    if(rc != GT_OK || routingMode == PRV_TGF_IP_ROUTING_MODE_PCL_ACTION_E)
    {
        return GT_FALSE;
    }

    return GT_TRUE;

#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)

    return GT_TRUE;
#endif /* !(defined CHX_FAMILY) */
}


