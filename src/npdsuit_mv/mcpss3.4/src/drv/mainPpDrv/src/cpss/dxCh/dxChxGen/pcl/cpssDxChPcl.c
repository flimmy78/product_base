/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssDxChPcl.c
*
* DESCRIPTION:
*       CPSS CH general PCL API implementation
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPcl.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>

/* used in structures at the places of not relevant values */
/* as offset of not used register address                  */
#define DUMMY8_CNS  0xFF
#define DUMMY32_CNS 0xFFFFFFFF

/* different devices have the same structure of register addresses */
/* but register addresses                                          */
/* the macros below helps to keep in DB not the register address   */
/* but the offset of register address in the structure of register */
/* addresses                                                       */

/* offset from the beginning of structure */
#define MEMBER_OFFSET_MAC(_type, _member)        (GT_U32)&(((_type*)0)->_member)

/* extract member address by pointer      */
#define MEMBER_POINTER_MAC(_ptr, _offset, _type) \
    (_type*)(((char*)(_ptr)) + (_offset))

/* offset of PCL register */
#define PCL_REG_OFFSET_MAC(_reg) \
    MEMBER_OFFSET_MAC(PRV_CPSS_DXCH_PP_REGS_ADDR_STC, pclRegs._reg)

/* address of PCL register */
#define PCL_REG_ADDRESS_MAC(_devNum, _offset) \
    *(MEMBER_POINTER_MAC(PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum), _offset, GT_U32))

/* HW values for Cheetah offset types in UDBs */
#define UDB_HW_L2_CNS      0
#define UDB_HW_L3_CNS      1
#define UDB_HW_L4_CNS      2
#define UDB_HW_IPV6EH_CNS  3
/* HW values for XCAT offset types in UDBs */
#define UDB_XCAT_HW_L2_CNS              0
#define UDB_XCAT_HW_MPLS_CNS            1
#define UDB_XCAT_HW_L3_CNS              2
#define UDB_XCAT_HW_L4_CNS              3

#define UDB_DB_L2_CNS      (1 << UDB_HW_L2_CNS)
#define UDB_DB_L3_CNS      (1 << UDB_HW_L3_CNS)
#define UDB_DB_L4_CNS      (1 << UDB_HW_L4_CNS)
#define UDB_DB_IPV6EH_CNS (1 << UDB_HW_IPV6EH_CNS)

#define UDB_DB_L2_L3_CNS   (UDB_DB_L2_CNS | UDB_DB_L3_CNS)
#define UDB_DB_L3_L4_CNS   (UDB_DB_L3_CNS | UDB_DB_L4_CNS)
#define UDB_DB_L2_L3_L4_CNS  \
    (UDB_DB_L2_CNS | UDB_DB_L3_CNS | UDB_DB_L4_CNS)
#define UDB_DB_L2_L3_L4_IPV6EH_CNS \
    (UDB_DB_L2_CNS | UDB_DB_L3_CNS | UDB_DB_L4_CNS | UDB_DB_IPV6EH_CNS)
#define UDB_DB_L3_L4_IPV6EH_CNS \
    (UDB_DB_L3_CNS | UDB_DB_L4_CNS | UDB_DB_IPV6EH_CNS)

/* static variables */

/* Key User Defined Bytes DB */
static struct
{
    GT_U32  regAddrOff[2]; /* address of UDB configuration registers */
    GT_U8   udbAmount;     /* number of UDBs */
    GT_U8   comparatorBit; /* bit to switch some UDB to comparator  */
    GT_U8   comparatorUdb; /*index of UDB that can contain comparator*/
    struct
    {
        GT_U8 regIdx;         /* register 0 or 1 of pair                   */
        GT_U8 udbCfgStartBit; /* 0-31 bit shift for pair <off-type, offset>*/
        GT_U8 validOffTypesBmp; /* bitmap (1 << hw_value)                  */
    } udbInfo[6];
} keyUdbInfo[6] =
{
    /* key0 - EXT_NOT_IPV6 */
    {
        {
            PCL_REG_OFFSET_MAC(udbConfigKey0bytes0_2),
            PCL_REG_OFFSET_MAC(udbConfigKey0bytes3_5),
        },
        6 /*udbAmount*/, 31 /*comparatorBit*/, 2 /*comparatorUdb*/,
        {
            {0, 0,  UDB_DB_L2_L3_L4_CNS},
            {0, 11, UDB_DB_L2_L3_L4_CNS},
            {0, 22, UDB_DB_L3_L4_CNS},
            {1, 0,  UDB_DB_L3_L4_CNS},
            {1, 11, UDB_DB_L3_L4_CNS},
            {1, 22, UDB_DB_L3_L4_CNS},
        }
    },
    /* key1 - EXT_IPV6_L2 */
    {
        {
            PCL_REG_OFFSET_MAC(udbConfigKey1bytes0_2),
            PCL_REG_OFFSET_MAC(udbConfigKey1bytes3_5),
        },
        6 /*udbAmount*/, 31 /*comparatorBit*/, 2 /*comparatorUdb*/,
        {
            {0, 0,  UDB_DB_L2_L3_L4_IPV6EH_CNS},
            {0, 11, UDB_DB_L2_L3_L4_IPV6EH_CNS},
            {0, 22, UDB_DB_L3_L4_IPV6EH_CNS},
            {1, 0,  UDB_DB_L3_L4_IPV6EH_CNS},
            {1, 11, UDB_DB_L3_L4_IPV6EH_CNS},
            {1, 22, UDB_DB_L3_L4_IPV6EH_CNS},
        }
    },
    /* key2 - EXT_IPV6_L4 */
    {
        {
            PCL_REG_OFFSET_MAC(udbConfigKey2bytes0_2),
            DUMMY32_CNS,
        },
        3 /*udbAmount*/, 31 /*comparatorBit*/, 2 /*comparatorUdb*/,
        {
            {0, 0,  UDB_DB_L2_L3_L4_IPV6EH_CNS},
            {0, 11, UDB_DB_L2_L3_L4_IPV6EH_CNS},
            {0, 22, UDB_DB_L3_L4_IPV6EH_CNS},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
        }
    },
    /* key3 - STD_NOT_IP */
    {
        {
            PCL_REG_OFFSET_MAC(udbConfigKey3bytes0_2),
            DUMMY32_CNS,
        },
        3 /*udbAmount*/, DUMMY8_CNS /*comparatorBit*/,
        DUMMY8_CNS /*comparatorUdb*/,
        {
            {0, 0,  UDB_DB_L2_L3_CNS},
            {0, 11, UDB_DB_L2_L3_CNS},
            {0, 22, UDB_DB_L3_CNS},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
        }
    },
    /* key4 - STD_IPV4_IPV6_L2_QOS */
    {
        {
            PCL_REG_OFFSET_MAC(udbConfigKey4bytes0_1),
            DUMMY32_CNS,
        },
        2 /*udbAmount*/, 20 /*comparatorBit*/, 0 /*comparatorUdb*/,
        {
            {0, 11, UDB_DB_L2_L3_L4_IPV6EH_CNS},
            {0, 22, UDB_DB_L2_L3_L4_IPV6EH_CNS},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
        }
    },
    /* key5 - STD_IPV4_L4 */
    {
        {
            PCL_REG_OFFSET_MAC(udbConfigKey5bytes0_2),
            DUMMY32_CNS,
        },
        3 /*udbAmount*/, 9 /*comparatorBit*/, 0 /*comparatorUdb*/,
        {
            {0, 0,  UDB_DB_L2_L3_L4_CNS},
            {0, 11, UDB_DB_L2_L3_L4_CNS},
            {0, 22, UDB_DB_L3_L4_CNS},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
            {DUMMY8_CNS, 0,  0},
        }
    },
};

/* HW <==> SW conversion tables for PCL CFG tables key formats */
/* Index is the HW value                                       */

/* DxCh3 IPCL LOOKUP NOT IP */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxCh3SwKeyType_IPCL_NOT_IP[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
    /* 1 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    /* 3 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* DxCh2 IPCL LOOKUP NOT IP */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxCh2SwKeyType_IPCL_NOT_IP[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    /* 2 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 3 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* xCat IPCL xCat LOOKUP NOT IP */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChXCatSwKeyType_IPCL_NOT_IP[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
    /* 1 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    /* 3 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
    /* 4 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* DxCh2, DxCh3 IPCL LOOKUP IPV4 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChSwKeyType_IPCL_IPV4[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    /* 3 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* xCat IPCL xCat LOOKUP IPV4 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChXCatSwKeyType_IPCL_IPV4[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    /* 3 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
    /* 4 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* xCat A0 IPCL LOOKUP IPV6 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChXCatA0SwKeyType_IPCL_IPV6[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
    /* 3 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* DxCh3 IPCL LOOKUP IPV6 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxCh3SwKeyType_IPCL_IPV6[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    /* 1 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
    /* 3 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* DxCh2 IPCL LOOKUP IPV6 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxCh2SwKeyType_IPCL_IPV6[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
    /* 3 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* xCat IPCL LOOKUP IPV6 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChXCatSwKeyType_IPCL_IPV6[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,
    /* 3 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
    /* 4 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E
};

/* DxCh2, DxCh3, xCat EPCL LOOKUP NOT IP */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChSwKeyType_EPCL_NOT_IP[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E,
    /* 2 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 3 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* DxCh2, DxCh3, xCat EPCL LOOKUP IPV4 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChSwKeyType_EPCL_IPV4[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E,
    /* 1 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E,
    /* 3 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/* DxCh2, DxCh3, xCat EPCL LOOKUP IPV6 */
static CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT prvCpssDxChSwKeyType_EPCL_IPV6[
    PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS] =
{
    /* 0 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E,
    /* 1 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 2 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E,
    /* 3 */ CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E,
    /* 4 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 5 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 6 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS,
    /* 7 */ PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS
};

/*******************************************************************************
* prvCpssDxChPclHwToSwCnvArrGet
*
* DESCRIPTION:
*   The function calculates address of 8-elements array of SW values
*   for 3-bit key type HW fields in PCL Configuration tables
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - device number
*       direction          - ingress or egress PCL key
*       pktType            - NOT-IP, IPV4 or IPV6 packet
*
* OUTPUTS:
*       hwToSwCnvPtrPtr    - pointer to array of 8 SW values
*
* RETURNS:
*       GT_OK              - on success
*       GT_BAD_PARAM       - on wrong parameters
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclHwToSwCnvArrGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_DIRECTION_ENT             direction,
    IN  GT_U32                             pktType,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT **hwToSwCnvPtrPtr
)
{
    GT_BOOL isCh2;     /* is Cheeta2 device */

    isCh2 = (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        == CPSS_PP_FAMILY_CHEETAH2_E)
        ? GT_TRUE : GT_FALSE;

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* xCat A1 and above */
                switch (pktType)
                {
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_NOT_IP_CNS:
                        *hwToSwCnvPtrPtr = prvCpssDxChXCatSwKeyType_IPCL_NOT_IP;
                        break;
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV4_CNS:
                        *hwToSwCnvPtrPtr = prvCpssDxChXCatSwKeyType_IPCL_IPV4;
                        break;
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV6_CNS:
                        *hwToSwCnvPtrPtr = prvCpssDxChXCatSwKeyType_IPCL_IPV6;
                        break;
                    default: return GT_BAD_PARAM;
                }
            }
            else
            {
                /* DxCh, DxCh2, DxCh3 */
                switch (pktType)
                {
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_NOT_IP_CNS:
                        *hwToSwCnvPtrPtr =
                            (isCh2 == GT_FALSE)
                                ? prvCpssDxCh3SwKeyType_IPCL_NOT_IP
                                : prvCpssDxCh2SwKeyType_IPCL_NOT_IP;
                        break;
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV4_CNS:
                        *hwToSwCnvPtrPtr = prvCpssDxChSwKeyType_IPCL_IPV4;
                        break;
                    case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV6_CNS:
                        *hwToSwCnvPtrPtr =
                            (isCh2 == GT_TRUE)
                                ? prvCpssDxCh2SwKeyType_IPCL_IPV6
                                : (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)
                                   ? prvCpssDxChXCatA0SwKeyType_IPCL_IPV6
                                   : prvCpssDxCh3SwKeyType_IPCL_IPV6);
                        break;
                    default: return GT_BAD_PARAM;
                }
            }
            break;
        case CPSS_PCL_DIRECTION_EGRESS_E:
            switch (pktType)
            {
                case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_NOT_IP_CNS:
                    *hwToSwCnvPtrPtr = prvCpssDxChSwKeyType_EPCL_NOT_IP;
                    break;
                case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV4_CNS:
                    *hwToSwCnvPtrPtr = prvCpssDxChSwKeyType_EPCL_IPV4;
                    break;
                case PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV6_CNS:
                    *hwToSwCnvPtrPtr = prvCpssDxChSwKeyType_EPCL_IPV6;
                    break;
                default: return GT_BAD_PARAM;
            }
            break;
        default: return GT_BAD_PARAM;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclKeyTypeHwToSw
*
* DESCRIPTION:
*   The function converts HW CFG table key type value to SW value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - device number
*       direction          - ingress or egress PCL key
*       pktType            - NOT-IP, IPV4 or IPV6 packet
*       hwKeyType          - HW CFG table key type value
*
* OUTPUTS:
*       swKeyTypePtr       - pointer to SW CFG table key type value
*
* RETURNS:
*       GT_OK              - on success
*       GT_BAD_PARAM       - on wrong parameters
*       GT_BAD_STATE       - unsupported HW value
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclKeyTypeHwToSw
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_DIRECTION_ENT             direction,
    IN  GT_U32                             pktType,
    IN  GT_U32                             hwKeyType,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT *swKeyTypePtr
)
{
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT *hwToSwCnvArrPtr; /* CNV table   */
    GT_STATUS                           rc;              /* return code  */

    if (hwKeyType >= PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS)
    {
        return GT_BAD_STATE;
    }

    /* select HW <==> SW convertion table */
    rc = prvCpssDxChPclHwToSwCnvArrGet(
        devNum, direction, pktType, &hwToSwCnvArrPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* convert using HW value as index */
    *swKeyTypePtr = hwToSwCnvArrPtr[hwKeyType];

    return (*swKeyTypePtr == PRV_CPSS_DXCH_PCL_CFG_DUMMY_SW_KEY_TYPE_CNS)
            ? GT_BAD_STATE : GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclKeyTypeSwToHw
*
* DESCRIPTION:
*   The function converts SW CFG table key type value to HW value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - device number
*       direction          - ingress or egress PCL key
*       pktType            - NOT-IP, IPV4 or IPV6 packet
*       swKeyType          - SW CFG table key type value
*
* OUTPUTS:
*       hwKeyTypePtr       - pointer to HW CFG table key type value
*
* RETURNS:
*       GT_OK              - on success
*       GT_BAD_PARAM       - on wrong parameters
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclKeyTypeSwToHw
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_DIRECTION_ENT             direction,
    IN  GT_U32                             pktType,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT swKeyType,
    OUT GT_U32                             *hwKeyTypePtr
)
{
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT *hwToSwCnvArrPtr; /* CNV table    */
    GT_STATUS                           rc;              /* return code  */
    GT_U32                              i;               /* loop counter */

    /* select HW <==> SW convertion table */
    rc = prvCpssDxChPclHwToSwCnvArrGet(
        devNum, direction, pktType, &hwToSwCnvArrPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* search SW value in the table - the index is HW value */
    for (i = 0; (i < PRV_CPSS_DXCH_PCL_CFG_MAX_HW_KEY_TYPES_CNS); i++)
    {
        if (hwToSwCnvArrPtr[i] == swKeyType)
        {
            *hwKeyTypePtr = i;
            return GT_OK;
        }
    }

    return GT_BAD_PARAM;
}

/*******************************************************************************
* cpssDxChPclInit
*
* DESCRIPTION:
*   The function initializes the device for following configuration
*   and using Policy engine
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - device number
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclInit
(
    IN GT_U8                           devNum
)
{
    /* this function is reserved */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCheetahPclUDBRegGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte HW configuration position
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       regAddrPtr       - (pointer to) register address
*       startBitPtr      - (pointer to) start bit in the register
*       validTypesPtr    - (pointer to) mask of valid Offset types
*       comparatorBitPtr - (pointer to) number of bit contains the
*                          TCP/UDB port comparator flag in register
*                          or 0xFFFFFFFF if none
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCheetahPclUDBRegGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  GT_U32                               udbIndex,
    OUT GT_U32                               *regAddrPtr,
    OUT GT_U32                               *startBitPtr,
    OUT GT_U32                               *validTypesPtr,
    OUT GT_U32                               *comparatorBitPtr
)
{
    GT_U32    keyIdx;        /* key format index 0-5     */
    GT_U32    regOff;        /* register address offset  */
    GT_BOOL   isNotCheetah1; /* GT_TRUE for not Cheetah1 */

    /* get device type */
     isNotCheetah1 =
         (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
          == CPSS_PP_FAMILY_CHEETAH_E)
         ? GT_FALSE : GT_TRUE;

    /* For the given user defined byte exactly one register must be updated. */
    /* If this byte cannot be used for comparator the updated bits in the    */
    /* register can be represented as subfield .                             */
    /* For the byte that may be was used for comparator but now will be      */
    /* configured as regular user defined byte both the "comparator" bit     */
    /* must be set to 0 and regular <offset_type, offeset> bits must be set. */
    /* get Lookup Key index */
    switch (ruleFormat)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            keyIdx = 3;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            keyIdx = 4;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            keyIdx = 5;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            keyIdx = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            keyIdx = 1;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            keyIdx = 2;
            break;
        default: return GT_BAD_PARAM;
    }

    /* check UDB index by DB */
    if (udbIndex >= keyUdbInfo[keyIdx].udbAmount)
    {
        return GT_BAD_PARAM;
    }

    /* retrieve register address */
    regOff  =
        keyUdbInfo[keyIdx].regAddrOff[
            keyUdbInfo[keyIdx].udbInfo[udbIndex].regIdx];
    *regAddrPtr    = PCL_REG_ADDRESS_MAC(devNum, regOff);

    *startBitPtr   = keyUdbInfo[keyIdx].udbInfo[udbIndex].udbCfgStartBit;
    *validTypesPtr = keyUdbInfo[keyIdx].udbInfo[udbIndex].validOffTypesBmp;

    if ((isNotCheetah1 == GT_TRUE)
        && (keyUdbInfo[keyIdx].comparatorUdb == udbIndex))
    {
        *comparatorBitPtr = keyUdbInfo[keyIdx].comparatorBit;
    }
    else
    {
        *comparatorBitPtr = 0xFFFFFFFF;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXCatPclUDBEntryIndexGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte Entry index
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number
*       packetType   - packet Type
*
* OUTPUTS:
*       entryIndexPtr    - (pointer to) User Defined Byte Entry index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChXCatPclUDBEntryIndexGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    OUT GT_U32                               *entryIndexPtr
)
{

    if (packetType > CPSS_DXCH_PCL_PACKET_TYPE_IPV6_E)
    {
        /* assumed thaqt the device is XCAT and above               */
        /* the UDE1-4 packet types supported by Lion and above only */
        if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum) == 0)
        {
            return GT_BAD_PARAM;
        }
    }

    switch (packetType)
    {
        case CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E:
            *entryIndexPtr = 0;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E:
            *entryIndexPtr = 1;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_MPLS_E:
            *entryIndexPtr = 2;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_IPV4_FRAGMENT_E:
            *entryIndexPtr = 3;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_IPV4_OTHER_E:
            *entryIndexPtr = 4;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E:
            *entryIndexPtr = 5;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_UDE_E:
            *entryIndexPtr = 6;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_IPV6_E:
            *entryIndexPtr = 7;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E:
            *entryIndexPtr = 8;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E:
            *entryIndexPtr = 9;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E:
            *entryIndexPtr = 10;
            break;
        case CPSS_DXCH_PCL_PACKET_TYPE_UDE4_E:
            *entryIndexPtr = 11;
            break;
        default: return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*       offsetType   - the type of offset (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - parameter value more then HW bit field
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
)
{
    return cpssDxChPclPortGroupUserDefinedByteSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleFormat, packetType, udbIndex,
        offsetType, offset);

}

/*******************************************************************************
* cpssDxChPclUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*                         (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_PCL_OFFSET_TYPE_ENT        *offsetTypePtr,
    OUT GT_U8                                *offsetPtr
)
{
    return cpssDxChPclPortGroupUserDefinedByteGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleFormat, packetType, udbIndex,
        offsetTypePtr, offsetPtr);
}

/*******************************************************************************
* cpssDxChPclRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule.
*
*       ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case
*                          With reference to Standard and Extended rules
*                          indexes, the partitioning is as follows:
*
*                          For DxCh and DxCh2 devices:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1023.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 511.
*                          Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n <= 511
*                            is placed in the space of two standard rules with
*                            indexes n and n + 512.
*
*                          For DxCh3 device:
*                          The TCAM has up to 3.5K (3584) rows.
*                          See datasheet of particular device for TCAM size.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 14335.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 7167.
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n < 3584
*                            is placed in the space of two standard rules with
*                            indexes n and n + 3584.
*                          - Extended rule with index  3584 <= n < 7168
*                            is placed in the space of two standard rules with
*                            indexes n + 3584, n + 7168.
*
*                          For xCat and above devices:
*                          See datasheet of particular device for TCAM size.
*                          For example describe the TCAM that has 1/4K (256) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1K (1024).
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 0.5K (512).
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index 2n (even index)
*                            is placed in the space of two standard rules with
*                            indexes 4n and 4n + 1.
*                          - Extended rule with index 2n+1 (odd index)
*                            is placed in the space of two standard rules with
*                            indexes 4n+2 and 4n + 3.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*                               Write rule to TCAM not using port-list format.
*                               Rule state is valid.
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E -
*                               write all fields of rule to TCAM but rule's
*                               state is invalid (no match during lookups).
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E |
*                                             CPSS_DXCH_PCL_RULE_OPTION_PORT_LIST_ENABLED_E -
*                               write all fields of rule to TCAM using port-list format
*                               but rule's state is invalid
*                              (no match during lookups).
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*                          The format of mask is defined by ruleFormat
*
*       patternPtr       - rule pattern.
*                          The format of pattern is defined by ruleFormat
*
*       actionPtr        - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleSet
(
    IN GT_U8                              devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_RULE_OPTION_ENT      ruleOptionsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    return cpssDxChPclPortGroupRuleSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleFormat, ruleIndex, ruleOptionsBmp,
        maskPtr, patternPtr, actionPtr);
}

/*******************************************************************************
* cpssDxChPclRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    return cpssDxChPclPortGroupRuleActionUpdate(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleSize, ruleIndex, actionPtr);
}

/*******************************************************************************
* cpssDxChPclRuleActionGet
*
* DESCRIPTION:
*   The function gets the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*       direction       - Policy direction:
*                         Ingress or Egress
*                         Needed for parsing
*
* OUTPUTS:
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    IN CPSS_PCL_DIRECTION_ENT              direction,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    return cpssDxChPclPortGroupRuleActionGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleSize, ruleIndex, direction, actionPtr);
}

/*******************************************************************************
* cpssDxChPclRuleInvalidate
*
* DESCRIPTION:
*   The function invalidates the Policy Rule.
*   For DxCh and DxCh2 devices start indirect rule write operation
*   with "garbage" pattern and mask content and "valid-flag" == 0
*   For DxCh3 devices calculates the TCAM position by ruleSize and ruleIndex
*   parameters and updates the first "valid" bit matching X/Y pair to (1,1)
*   i.e. don't match any value. If origin of valid or invalid rule found at the
*   specified TCAM position it will be invalid and available to back validation.
*   If the specified TCAM position contains the not first 24 byte segment of
*   extended rule, the rule also will be invalidated,
*   but unavailable to back validation.
*   If the garbage found in TCAM the X/Y bits will be updated to be as
*   in invalid rule.
*   The function does not check the TCAM contents.
*   GT_OK will be returned in each of described cases.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Backward compatible styled API.
*       The cpssDxChPclRuleValidStatusSet recommended for using instead.
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleInvalidate
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex
)
{
    return cpssDxChPclPortGroupRuleInvalidate(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleSize, ruleIndex);
}

/*******************************************************************************
* cpssDxChPclRuleValidStatusSet
*
* DESCRIPTION:
*     Validates/Invalidates the Policy rule.
*        The validation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Valid indication set.
*           The function does not check content of the rule before
*           write it back to TCAM
*        The invalidation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Invalid indication set.
*        If the given the rule found already in needed valid state
*        no write done. If the given the rule found with size
*        different from the given rule-size an error code returned.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM.
*       valid           - new rule status: GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - if in TCAM found rule of size different
*                        from the specified
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleValidStatusSet
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN GT_BOOL                            valid
)
{
    return cpssDxChPclPortGroupRuleValidStatusSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleSize, ruleIndex, valid);
}

/*******************************************************************************
* cpssDxChPclRuleCopy
*
* DESCRIPTION:
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first cpssDxChPclRuleCopy should be
*  called. And after this cpssDxChPclRuleInvalidate should be used to invalidate
*  Rule in old position
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleSize         - size of Rule.
*       ruleSrcIndex     - index of the rule in the TCAM from which pattern,
*                           mask and action are taken.
*       ruleDstIndex     - index of the rule in the TCAM to which pattern,
*                           mask and action are placed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleCopy
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
)
{
    return cpssDxChPclPortGroupRuleCopy(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleSize, ruleSrcIndex, ruleDstIndex);
}

/*******************************************************************************
* cpssDxChPclRuleMatchCounterGet
*
* DESCRIPTION:
*       Get rule matching counters.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum       - Device number.
*       counterIndex - Counter Index (0..31)
*
* OUTPUTS:
*       counterPtr   - pointer to the counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleMatchCounterGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterIndex,
    OUT GT_U32                        *counterPtr
)
{
    GT_U32    regAddr;       /* register address                    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(counterPtr);

    if(counterIndex > 31)
    {
        return GT_BAD_PARAM;
    }

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ruleMatchCounersBase
        + (counterIndex * 4);

    return prvCpssDxChPortGroupsCounterSummary(devNum, regAddr, 0,32,counterPtr,NULL);
}

/*******************************************************************************
* cpssDxChPclRuleMatchCounterSet
*
* DESCRIPTION:
*       Set rule matching counters.
*       To reset value of counter use counterValue = 0
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum       - Device number.
*       counterIndex - Counter Index (0..31)
*       counterValue - counter value.
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
GT_STATUS cpssDxChPclRuleMatchCounterSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterIndex,
    IN  GT_U32                        counterValue
)
{
    GT_U32    regAddr;       /* register address                    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);


    if(counterIndex > 31)
    {
        return GT_BAD_PARAM;
    }

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ruleMatchCounersBase
        + (counterIndex * 4);

    return prvCpssDxChPortGroupsCounterSet(devNum, regAddr,0,32, counterValue);
}

/*******************************************************************************
* cpssDxChPclPortIngressPolicyEnable
*
* DESCRIPTION:
*    Enables/disables ingress policy per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*    enable          - GT_TRUE - Ingress Policy enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortIngressPolicyEnable
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
{
    GT_U32 bitNum; /* number of "PCL enable" bit in VLAN QoS entry */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat devices */
        bitNum = 10;
    }
    else
    {
        /* default for DxCh - DxCh3 devices */
        bitNum = 17;
    }

    return prvCpssDxChWriteTableEntryField(
        devNum, PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E, port,
        0 /*fieldWordNum*/,   bitNum /*fieldOffset*/,
        1 /*fieldLength*/,    ((enable == GT_FALSE) ? 0 : 1) /*fieldValue*/);

}

/*******************************************************************************
* cpssDxChPclPortIngressPolicyEnableGet
*
* DESCRIPTION:
*    Get the Enable/Disable ingress policy status per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*
* OUTPUTS:
*    enablePtr       - Pointer to ingress policy status.
*                      GT_TRUE - Ingress Policy is enabled.
*                      GT_FALSE - Ingress Policy is disabled.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortIngressPolicyEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 bitNum; /* number of "PCL enable" bit in VLAN QoS entry */
    GT_U32 value;  /* register field value */
    GT_STATUS rc;         /* return code                                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat devices */
        bitNum = 10;
    }
    else
    {
        /* default for DxCh - DxCh3 devices */
        bitNum = 17;
    }
    rc = prvCpssDxChReadTableEntryField(
        devNum,
        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
        port,
        0 /*fieldWordNum*/,
        bitNum /*fieldOffset*/,
        1 /*fieldLength*/,
        &value /*fieldValue*/);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortLookupCfgTabAccessModeSet
*
* DESCRIPTION:
*    Configures VLAN/PORT access mode to Ingress or Egress PCL
*    configuration table per lookup.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*    direction       - Policy direction:
*                      Ingress or Egress
*    lookupNum       - Lookup number:
*                      lookup0 or lookup1
*    subLookupNum    - sub lookup number for lookup.
*                      Supported only for  xCat and above devices.
*                      Ignored for other devices.
*                      xCat and above devices supports two sub lookups only for
*                      ingress lookup CPSS_PCL_LOOKUP_0_E , acceptable range 0..1.
*                      All other lookups have not sub lookups,
*                      acceptable value is 0.
*    mode            - PCL Configuration Table access mode
*                      Port or VLAN ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   - for DxCh2 and above - full support,
*   - for DxCh1 supports Ingress direction only and set same access type
*     for both lookups regardless the <lookupNum> parameter value
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortLookupCfgTabAccessModeSet
(
    IN GT_U8                                          devNum,
    IN GT_U8                                          port,
    IN CPSS_PCL_DIRECTION_ENT                         direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT                     lookupNum,
    IN GT_U32                                         subLookupNum,
    IN CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT mode
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_BOOL   isNotCh1;      /* GT_TRUE - Not Cheetah1 device       */
    GT_BOOL   isXCat;        /* GT_TRUE - XCat or above             */
    GT_U32    vlanBit;       /* vlan - 1, port - 0                  */
    GT_U32    bitPos;        /* bit position                        */
    GT_U32    portGroupId;        /* port group Id - support multi-port-groups device    */
    GT_U8     localPort;     /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    isNotCh1 =
        (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
          == CPSS_PP_FAMILY_CHEETAH_E)
         ? GT_FALSE : GT_TRUE;

    isXCat =
        (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        ? GT_TRUE : GT_FALSE;

    if (port == CPSS_CPU_PORT_NUM_CNS)
    {
        port = 63;
    }

    switch (mode)
    {
        case CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E:
            vlanBit = 0;
            break;
        case CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E:
            vlanBit = 1;
            break;
        default: return GT_BAD_PARAM;
    }

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:

            /* both lookups for cheetah+ */
            bitPos = 20;
            if (isNotCh1 != GT_FALSE)
            {
                if (isXCat == GT_FALSE)
                {
                    /* DxCh - DxCh3 devices*/
                    switch (lookupNum)
                    {
                        case CPSS_PCL_LOOKUP_0_E:
                            bitPos = 20;
                            break;
                        case CPSS_PCL_LOOKUP_1_E:
                            bitPos = 16;
                            break;
                        default: return GT_BAD_PARAM;
                    }
                }
                else
                {

                    /* xCat devices*/
                    switch (lookupNum)
                    {
                        case CPSS_PCL_LOOKUP_0_E:
                            if (subLookupNum > 1)
                            {
                                return GT_BAD_PARAM;
                            }
                            bitPos = (subLookupNum == 0) ? 11 : 12;
                            break;
                        case CPSS_PCL_LOOKUP_1_E:
                            if (subLookupNum > 0)
                            {
                                return GT_BAD_PARAM;
                            }
                            bitPos = 13;
                            break;
                        case CPSS_PCL_LOOKUP_0_0_E:
                            if (subLookupNum > 0)
                            {
                                return GT_BAD_PARAM;
                            }
                            bitPos = 11;
                            break;
                        case CPSS_PCL_LOOKUP_0_1_E:
                            if (subLookupNum > 0)
                            {
                                return GT_BAD_PARAM;
                            }
                            bitPos = 12;
                            break;
                        default: return GT_BAD_PARAM;
                    }
                }
            }

            if (port == CPSS_CPU_PORT_NUM_CNS)
            {
                port = 63;
            }

            return prvCpssDxChWriteTableEntryField(
                devNum,
                PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                port, 0 /*fieldWordNum*/,
                bitPos, 1 /*fieldLength*/, vlanBit);


        case CPSS_PCL_DIRECTION_EGRESS_E:

            if (isNotCh1 == GT_FALSE)
            {
                return GT_BAD_PARAM;
            }

            if ((isXCat != GT_FALSE) && (subLookupNum > 0))
            {
                return GT_BAD_PARAM;
            }

            /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);

            /* get the start bit 0..63 (in the bmp of registers) */
            bitPos = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                     PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
                     localPort;

            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclCfgTblAccess[OFFSET_TO_WORD_MAC(bitPos)];

            return prvCpssDrvHwPpPortGroupSetRegField(
                devNum, portGroupId, regAddr, OFFSET_TO_BIT_MAC(bitPos), 1, vlanBit);

        default: return GT_BAD_PARAM;
    }
}

/*******************************************************************************
* cpssDxChPclCfgTblAccessModeSet
*
* DESCRIPTION:
*    Configures global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    accModePtr      - global configuration of access mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*   - for DxCh2 and above - full support,
*   - for DxCh1 supports Ingress direction only with 32 ports per device and the
*     non-local-device-entries-segment-base == 0
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblAccessModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   *accModePtr
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    hwMask;        /* HW mask                             */
    GT_STATUS res;           /* return code                         */
    GT_U32    swData;        /* work                                */
    GT_U32    nonLocalBitPos;/* positon of nonLocal bit             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(accModePtr);

    /* ingress PCL */

    hwValue = hwMask = 0;

    /* LOCAL / NON_LOCAL ACCESS */

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
         == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* Cheetah+ - for Cheeta2 another values */
        hwMask |= (1 << 1);
        switch (accModePtr->ipclAccMode)
        {
            case CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E:
                hwValue |= (1 << 1);
                break;
            case CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E:
                /* hwValue already 0*/
                break;
            default: return GT_BAD_PARAM;
        }

        /* all other configurations not relevant for Cheetah+ */
        return prvCpssDrvHwPpWriteRegBitMask(
            devNum, regAddr, hwMask, hwValue);
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        nonLocalBitPos = 2;
    }
    else
    {
        nonLocalBitPos = 1;
    }

    hwMask |= (1 << nonLocalBitPos);

    switch (accModePtr->ipclAccMode)
    {
        case CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E:
            /* hwValue already 0 */
            break;
        case CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E:
            hwValue |= (1 << nonLocalBitPos);
            break;
        default: return GT_BAD_PARAM;
    }

    /* max ports per device for non-local access */
    /* base of segment entries for non-local access */

    swData = 0;

    switch (accModePtr->ipclMaxDevPorts)
    {
        case CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E:
            /* swData already 0 */
            break;
        case CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E:
            swData |= (1 << 2);
            break;
        default: return GT_BAD_PARAM;
    }

    switch (accModePtr->ipclDevPortBase)
    {
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E:
            /* swData already 0 */
            break;
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E:
            swData |= 1;
            break;
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E:
            swData |= 2;
            break;
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E:
            swData |= 3;
            break;
        default: return GT_BAD_PARAM;
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        hwMask |= (1 << 7) | (3 << 3);
        switch (swData)
        {
            case 0:    /* 32 ports base 0 */
                /* hwValue bits 7, 4, 3 already 0 */
                break;
            case 1:    /* 32 ports base 1024 */
                hwValue |= (1 << 3);
                break;
            case 2:    /* 32 ports base 2048 */
                hwValue |= (2 << 3);
                break;
            case 3:    /* 32 ports base 3072 */
                hwValue |= (3 << 3);
                break;
            case 4:    /* 64 ports base 0 */
                hwValue |= (1 << 7);
                break;
            case 6:    /* 64 ports base 2048 */
                hwValue |= (1 << 7) | (1 << 3);
                break;
            /* all other parameter values are invalid */
            default: return GT_BAD_PARAM;
        }
    }
    else
    {
        /* DxCh2 and DxCh3 */
        hwMask |= (1 << 18) | (3 << 16);
        switch (swData)
        {
            case 0:    /* 32 ports base 0 */
                /* hwValue bits 18, 17, 16 already 0 */
                break;
            case 1:    /* 32 ports base 1024 */
                hwValue |= (1 << 16);
                break;
            case 2:    /* 32 ports base 2048 */
                hwValue |= (2 << 16);
                break;
            case 3:    /* 32 ports base 3072 */
                hwValue |= (3 << 16);
                break;
            case 4:    /* 64 ports base 0 */
                hwValue |= (1 << 18);
                break;
            case 6:    /* 64 ports base 2048 */
                hwValue |= (1 << 18) | (1 << 16);
                break;
            /* all other parameter values are invalid */
            default: return GT_BAD_PARAM;
        }
    }


    res = prvCpssDrvHwPpWriteRegBitMask(
        devNum, regAddr, hwMask, hwValue);
    if (res != GT_OK)
    {
        return res;
    }

    /* egress PCL */

    hwValue = hwMask = 0;

    /* LOCAL / NON_LOCAL ACCESS */

    hwMask |= (1 << 1);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;

    switch (accModePtr->epclAccMode)
    {
        case CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E:
            /* hwValue already 0 */
            break;
        case CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E:
            hwValue |= (1 << 1);
            break;
        default: return GT_BAD_PARAM;
    }

    /* max ports per device for non-local access */
    /* base of segment entries for non-local access */

    hwMask |= (1 << 18) | (3 << 16);
    swData = 0;

    switch (accModePtr->epclMaxDevPorts)
    {
        case CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E:
            /* swData already 0 */
            break;
        case CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E:
            swData |= (1 << 2);
            break;
        default: return GT_BAD_PARAM;
    }

    switch (accModePtr->epclDevPortBase)
    {
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E:
            /* swData already 0 */
            break;
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E:
            swData |= 1;
            break;
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E:
            swData |= 2;
            break;
        case CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E:
            swData |= 3;
            break;
        default: return GT_BAD_PARAM;
    }


    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        switch (swData)
        {
            case 0:    /* 32 ports base 0 */
                /* hwValue bits 18, 17, 16 already 0 */
                break;
            case 2:    /* 32 ports base 2048 */
                hwValue |= (1 << 16);
                break;
            case 4:    /* 64 ports base 0 */
                hwValue |= (1 << 18);
                break;
            case 6:    /* 64 ports base 2048 */
                hwValue |= (1 << 18) | (1 << 16);
                break;
            /* Not supported */
            case 1:    /* 32 ports base 1024 */
            case 3:    /* 32 ports base 3072 */
            /* all other parameter values are invalid */
            default: return GT_BAD_PARAM;
        }
    }
    else
    {
        /* DxCh2 and DxCh3 */
        switch (swData)
        {
            case 0:    /* 32 ports base 0 */
                /* hwValue bits 18, 17, 16 already 0 */
                break;
            case 1:    /* 32 ports base 1024 */
                hwValue |= (1 << 16);
                break;
            case 2:    /* 32 ports base 2048 */
                hwValue |= (2 << 16);
                break;
            case 3:    /* 32 ports base 3072 */
                hwValue |= (3 << 16);
                break;
            case 4:    /* 64 ports base 0 */
                hwValue |= (1 << 18);
                break;
            case 6:    /* 64 ports base 2048 */
                hwValue |= (1 << 18) | (1 << 16);
                break;
            /* all other parameter values are invalid */
            default: return GT_BAD_PARAM;
        }
    }


    res = prvCpssDrvHwPpWriteRegBitMask(
        devNum, regAddr, hwMask, hwValue);
    if (res != GT_OK)
    {
        return res;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclCfgTblAccessModeGet
*
* DESCRIPTION:
*    Get global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*
* OUTPUTS:
*    accModePtr      - global configuration of access mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*    see cpssDxChPclCfgTblAccessModeSet
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblAccessModeGet
(
    IN  GT_U8                                   devNum,
    OUT CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   *accModePtr
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    hwData;        /* HW Data                             */
    GT_STATUS res;           /* return code                         */
    GT_U32    nonLocalBitPos;/* positon of nonLocal bit             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(accModePtr);

    /* ingress PCL */

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;

    res = prvCpssDrvHwPpReadRegister(devNum, regAddr, &hwValue);
    if (res != GT_OK)
    {
        return res;
    }

    /* LOCAL / NON_LOCAL ACCESS */

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* Cheetah+ and Cheetah2 has different values */

        /* LOCAL / NON_LOCAL ACCESS */

            accModePtr->ipclAccMode =
            ((hwValue & (1 << 1)) == 0)
            ? CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E
            : CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E;

        /* not relevant structure fields */
        accModePtr->ipclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
        accModePtr->ipclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
        accModePtr->epclAccMode = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E;
        accModePtr->epclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
        accModePtr->epclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;

        return GT_OK;
    }

    /* LOCAL / NON_LOCAL ACCESS */

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        nonLocalBitPos = 2;
    }
    else
    {
        nonLocalBitPos = 1;
    }

    accModePtr->ipclAccMode =
        ((hwValue & (1 << nonLocalBitPos)) == 0)
        ? CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E
        : CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E;

    /* max ports per device for non-local access */
    /* base of segment entries for non-local access */

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        /* bits 7 and 4:3 */
        hwData = (((hwValue >> 3) & 3) | ((hwValue >> 5) & 4));
    }
    else
    {
        /* DxCh2 and DxCh3 */
        hwData = ((hwValue >> 16) & 7);
    }

    switch (hwData)
    {
        case 0:
            accModePtr->ipclMaxDevPorts =
                CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
            accModePtr->ipclDevPortBase =
                CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
            break;
        case 1:
            accModePtr->ipclMaxDevPorts =
                CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
            accModePtr->ipclDevPortBase =
                CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E;
            break;
        case 2:
            accModePtr->ipclMaxDevPorts =
                CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
            accModePtr->ipclDevPortBase =
                CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E;
            break;
        case 3:
            accModePtr->ipclMaxDevPorts =
                CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
            accModePtr->ipclDevPortBase =
                CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E;
            break;
        case 4:
            accModePtr->ipclMaxDevPorts =
                CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E;
            accModePtr->ipclDevPortBase =
                CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
            break;
        case 5:
        case 6:
        case 7:
            /* CASES 6 AND 7 NEVER CONFIGURED BY CPSS AND UNDEFINED */
            accModePtr->ipclMaxDevPorts =
                CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E;
            accModePtr->ipclDevPortBase =
                CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E;
            break;
        /* must never occure */
        default: return GT_BAD_STATE;
    }


    /* egress PCL */

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;

    res = prvCpssDrvHwPpReadRegister(devNum, regAddr, &hwValue);
    if (res != GT_OK)
    {
        return res;
    }

    /* LOCAL / NON_LOCAL ACCESS */

    accModePtr->epclAccMode =
        ((hwValue & (1 << 1)) == 0)
        ? CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E
        : CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E;

    /* max ports per device for non-local access */
    /* base of segment entries for non-local access */

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        switch ((hwValue >> 16) & 7)
        {
            case 0:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
                break;
            case 1:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E;
                break;
            case 4:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
                break;
            case 5:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E;
                break;
            default: return GT_BAD_STATE;
        }
    }
    else
    {
        /* DxCh2 And DxCh3 */
        switch ((hwValue >> 16) & 7)
        {
            case 0:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
                break;
            case 1:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E;
                break;
            case 2:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E;
                break;
            case 3:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E;
                break;
            case 4:
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
                break;
            case 5:
            case 6:
            case 7:
                /* CASES 6 AND 7 NEVER CONFIGURED BY CPSS AND UNDEFINED */
                accModePtr->epclMaxDevPorts =
                    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E;
                accModePtr->epclDevPortBase =
                    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E;
                break;
            /* must never occure */
            default: return GT_BAD_STATE;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: port, VLAN, or index for setting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be of the same size, Standard IPV6 DIP key allowed only on lookup1
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblSet
(
    IN GT_U8                           devNum,
    IN CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT          direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    return cpssDxChPclPortGroupCfgTblSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        interfaceInfoPtr, direction, lookupNum, lookupCfgPtr);
}

/*******************************************************************************
* cpssDxChPclCfgTblGet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: port, VLAN, or index for getting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*
* OUTPUTS:
*       lookupCfgPtr      - (pointer to) lookup configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be the same size, Standard IPV6 DIP key allowed only on lookup1
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    OUT CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    return cpssDxChPclPortGroupCfgTblGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        interfaceInfoPtr, direction, lookupNum, lookupCfgPtr);
}

/*******************************************************************************
* cpssDxChPclIngressPolicyEnable
*
* DESCRIPTION:
*    Enables Ingress Policy.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum           - device number
*    enable           - enable ingress policy
*                      GT_TRUE  - enable,
*                      GT_FALSE - disable
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
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIngressPolicyEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitPos;        /* the position of bit                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* XCAT and above */

        bitPos = 1;

        /* 1 - enable, 0 - disable */
        hwValue = (enable == GT_FALSE) ? 0 : 1;
    }
    else
    {
        /* CH1-3 */

        bitPos = 7;

        /* 0 - enable, 1 - disable */
        hwValue = (enable == GT_FALSE) ? 1 : 0;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, bitPos, 1, hwValue);
}

/*******************************************************************************
* cpssDxChPclTcamRuleSizeModeSet
*
* DESCRIPTION:
*    Set TCAM Rules size mode.
*    The rules TCAM may be configured to contain short rules (24 bytes), long
*    rules (48 bytes), or a mix of short rules and long rules.
*    The rule size may be set by global configuration or may be defined
*    dynamical according to PCL Configuration table settings.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*    devNum               - device number
*    mixedRuleSizeMode    - TCAM rule size mode
*                           GT_TRUE  - mixed Rule Size Mode, rule size defined
*                                      by PCL configuration table
*                           GT_FALSE - Rule size defined by ruleSize parameter
*    ruleSize             - Rule size for not mixed mode.
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
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclTcamRuleSizeModeSet
(
    IN GT_U8                       devNum,
    IN GT_BOOL                     mixedRuleSizeMode,
    IN CPSS_PCL_RULE_SIZE_ENT      ruleSize
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;

    hwValue = ((mixedRuleSizeMode == GT_FALSE) ? 0 : (1 << 8));

    switch (ruleSize)
    {
        case CPSS_PCL_RULE_SIZE_STD_E: /*bit0 is 0*/ break;
        case CPSS_PCL_RULE_SIZE_EXT_E: hwValue |= 1; break;
        default: return GT_BAD_PARAM;
    }

    return prvCpssDrvHwPpWriteRegBitMask(
        devNum, regAddr, ((1 << 8) | 1) /*mask*/, hwValue);
}

/*******************************************************************************
* prvCpssDxChPclUdeEtherTypeRegAddeGet
*
* DESCRIPTION:
*       This function gets UDE Ethertype register address
*       and bit offset of this subfield in register.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index
*                       The valid range documented in Functional Specification.
*
* OUTPUTS:
*       regAddrPtr    - (pointer to) address of register.
*       bitOffsetPtr  - (pointer to) bit offset in register.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclUdeEtherTypeRegAddeGet
(
    IN   GT_U8           devNum,
    IN   GT_U32          index,
    OUT  GT_U32          *regAddrPtr,
    OUT  GT_U32          *bitOffsetPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    if (index == 0)
    {
        /* UDE for index == 0 */
        *regAddrPtr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ttiRegs.specialEthertypes;

        *bitOffsetPtr = 0;

        return GT_OK;
    }

    if (index < 5)
    {
        /* UDE for index == 1..4                      */
        /* the case of 0 already treated upper        */

        /* for index 1,2 - word0, for index 3,4 - word1 */
        *regAddrPtr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ttiRegs.udeEthertypesBase
            + (4 * ((index - 1) / 2));

        /* for index 1,3 - 0, for index 2,4 - 16 */
        *bitOffsetPtr = ((index - 1) & 1) ? 16 : 0;

        return GT_OK;
    }

    return GT_BAD_PARAM;
}

/*******************************************************************************
* cpssDxChPclUdeEtherTypeSet
*
* DESCRIPTION:
*       This function sets UDE Ethertype.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index, range 0..4
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
GT_STATUS cpssDxChPclUdeEtherTypeSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    IN  GT_U32          ethType
)
{
    GT_STATUS   rc;           /* return code      */
    GT_U32      regAddr;      /* register address */
    GT_U32      bitOffset;    /* bit offset       */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);


    if (ethType >= BIT_16)
        return GT_OUT_OF_RANGE;

    /* check devNum and index validity */
    /* calculate regAddr and bitOffset */
    rc = prvCpssDxChPclUdeEtherTypeRegAddeGet(
        devNum, index, &regAddr, &bitOffset);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* write value */
    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, bitOffset, 16, ethType);
}

/*******************************************************************************
* cpssDxChPclUdeEtherTypeGet
*
* DESCRIPTION:
*       This function gets the UDE Ethertype.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index, range 0..4
*
* OUTPUTS:
*       ethTypePtr    - points to Ethertype value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclUdeEtherTypeGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    OUT GT_U32          *ethTypePtr
)
{
    GT_STATUS   rc;           /* return code      */
    GT_U32      regAddr;      /* register address */
    GT_U32      bitOffset;    /* bit offset       */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(ethTypePtr);

    /* check devNum and index validity */
    /* calculate regAddr and bitOffset */
    rc = prvCpssDxChPclUdeEtherTypeRegAddeGet(
        devNum, index, &regAddr, &bitOffset);
    if (rc != GT_OK)
    {
        return rc;
    }

    return prvCpssDrvHwPpGetRegField(
        devNum, regAddr, bitOffset, 16, ethTypePtr);
}

/*******************************************************************************
* cpssDxChPclTwoLookupsCpuCodeResolution
*
* DESCRIPTION:
*    Resolve the result CPU Code if both lookups has action commands
*    are either both TRAP or both MIRROR To CPU
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum               - device number
*    lookupNum            - lookup number from which the CPU Code is selected
*                           when action commands are either both TRAP or
*                           both MIRROR To CPU
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
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclTwoLookupsCpuCodeResolution
(
    IN GT_U8                       devNum,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT  lookupNum
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitNum;        /* bit number                          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
        bitNum = 6;
    }
    else
    {
        /* DxCh1-3 devices */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        bitNum = 5;
    }

    switch (lookupNum)
    {
        case CPSS_PCL_LOOKUP_0_E: hwValue = 0; break;
        case CPSS_PCL_LOOKUP_1_E: hwValue = 1; break;
        default: return GT_BAD_PARAM;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, bitNum, 1, hwValue);
}

/*******************************************************************************
* cpssDxChPclInvalidUdbCmdSet
*
* DESCRIPTION:
*    Set the command that is applied when the policy key <User-Defined>
*    field cannot be extracted from the packet due to lack of header
*    depth (i.e, the field resides deeper than 128 bytes into the packet).
*    This command is NOT applied when the policy key <User-Defined>
*    field cannot be extracted due to the offset being relative to a layer
*    start point that does not exist in the packet. (e.g. the offset is relative
*    to the IP header but the packet is a non-IP).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum         - device number
*    udbErrorCmd    - command applied to a packet:
*                      continue lookup, trap to CPU, hard drop or soft drop
*
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
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclInvalidUdbCmdSet
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_UDB_ERROR_CMD_ENT  udbErrorCmd
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitNum;        /* Bit number                          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        bitNum = 8;
    }
    else
    {
        /* DxCh - DxCh3 */
        bitNum = 12;
    }

    switch (udbErrorCmd)
    {
        case CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E:      hwValue = 0; break;
        case CPSS_DXCH_UDB_ERROR_CMD_TRAP_TO_CPU_E: hwValue = 1; break;
        case CPSS_DXCH_UDB_ERROR_CMD_DROP_HARD_E:   hwValue = 3; break;
        case CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E:   hwValue = 2; break;
        default: return GT_BAD_PARAM;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, bitNum, 2, hwValue);
}

/*******************************************************************************
* cpssDxChPclIpLengthCheckModeSet
*
* DESCRIPTION:
*    Set the mode of checking IP packet length.
*    To determine if an IP packet is a valid IP packet, one of the checks is a
*    length check to find out if the total IP length field reported in the
*    IP header is less or equal to the packet's length.
*    This function determines the check mode.
*    The results of this check sets the policy key <IP Header OK>.
*    The result is also used by the router engine to determine whether
*    to forward or trap/drop the packet.
*    There are two check modes:
*     1. For IPv4: ip_total_length <= packet's byte count
*        For IPv6: ip_total_length + 40 <= packet's byte count
*     2. For IPv4: ip_total_length + L3 Offset + 4 (CRC) <= packet's byte count,
*        For IPv6: ip_total_length + 40 +L3 Offset + 4 (CRC) <= packet's
*        byte count
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum           - device number
*    mode             - IP packet length checking mode
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
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIpLengthCheckModeSet
(
    IN GT_U8                                   devNum,
    IN CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT  mode
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitNum;        /* bit number                          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        /* Lion and above */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
        bitNum = 10;
    }
    else if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
        bitNum = 9;
    }
    else
    {
        /* DxCh1-3 devices */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        bitNum = 15;
    }

    switch (mode)
    {
        case CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L3_E:
            hwValue = 1;
            break;
        case CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E:
            hwValue = 0;
            break;
        default: return GT_BAD_PARAM;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, bitNum, 1, hwValue);
}

/*******************************************************************************
* cpssDxCh2PclEgressPolicyEnable
*
* DESCRIPTION:
*    Enables Egress Policy.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum           - device number
*    enable           - enable Egress Policy
*                       GT_TRUE  - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported enable parameter value
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh2PclEgressPolicyEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above  */
        /* 1 - enable, 0 - disable */
        hwValue = (enable == GT_FALSE) ? 0 : 1;

        if((GT_FALSE == enable) &&
           (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                      PRV_CPSS_DXCH_XCAT2_EPCL_GLOBAL_EN_NOT_FUNCTIONAL_WA_E)))
        {
            return GT_NOT_SUPPORTED;
        }

    }
    else
    {
        /* CH2, CH3 */
        /* 0 - enable, 1 - disable */
        hwValue = (enable != GT_FALSE) ? 0 : 1;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 7, 1, hwValue);
}

/*******************************************************************************
* cpssDxChPclEgressForRemoteTunnelStartEnableSet
*
* DESCRIPTION:
*       Enable/Disable the Egress PCL processing for the packets,
*       which Tunnel Start already done on remote ingress device.
*       These packets are ingessed with DSA Tag contains source port 60.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number
*       enable         - enable Remote Tunnel Start Packets Egress Pcl
*                        GT_TRUE  - enable
*                        GT_FALSE - disable
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressForRemoteTunnelStartEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_BOOL                       enable
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;

    /* 1 - enable, 0 - disable */
    hwValue = (enable == GT_FALSE) ? 0 : 1;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 19, 1, hwValue);
}

/*******************************************************************************
* cpssDxChPclEgressForRemoteTunnelStartEnableGet
*
* DESCRIPTION:
*       Gets Enable/Disable of the Egress PCL processing for the packets,
*       which Tunnel Start already done on remote ingress device.
*       These packets are ingessed with DSA Tag contains source port 60.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       enablePtr      - (pointer to) enable Remote Tunnel Start Packets Egress Pcl
*                        GT_TRUE  - enable
*                        GT_FALSE - disable
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressForRemoteTunnelStartEnableGet
(
    IN  GT_U8                         devNum,
    OUT GT_BOOL                       *enablePtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 19, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* 1 - enable, 0 - disable */
    *enablePtr = (hwValue == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet
*
* DESCRIPTION:
*       Sets Egress Policy Configuration Table Access Mode for
*       Tunnel Start packets
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       cfgTabAccMode  - PCL Configuration Table access mode
*                        Port or VLAN ID
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet
(
    IN  GT_U8                                            devNum,
    IN  CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT   cfgTabAccMode
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;

    switch (cfgTabAccMode)
    {
        case CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E:
            hwValue = 0;
            break;
        case CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E:
            hwValue = 1;
            break;
        default: return GT_BAD_PARAM;
    }


    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 26, 1, hwValue);
}

/*******************************************************************************
* cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
*
* DESCRIPTION:
*       Gets Egress Policy Configuration Table Access Mode for
*       Tunnel Start packets
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum             - device number
* OUTPUTS:
*       cfgTabAccModePtr   - (pointer to) PCL Configuration Table access mode
*                            Port or VLAN ID
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
(
    IN  GT_U8                                            devNum,
    OUT CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT   *cfgTabAccModePtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(cfgTabAccModePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 26, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *cfgTabAccModePtr = (hwValue == 0) ?
                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E :
                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclEgressKeyFieldsVidUpModeSet
*
* DESCRIPTION:
*       Sets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum         - device number
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
GT_STATUS cpssDxChPclEgressKeyFieldsVidUpModeSet
(
    IN  GT_U8                                      devNum,
    IN  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
)
{
    return cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, vidUpMode);
}

/*******************************************************************************
* cpssDxChPclEgressKeyFieldsVidUpModeGet
*
* DESCRIPTION:
*       Gets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum             - device number
* OUTPUTS:
*       vidUpModePtr       - (pointer to) VID and UP key fields
*                            calculation mode
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressKeyFieldsVidUpModeGet
(
    IN   GT_U8                                      devNum,
    OUT  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   *vidUpModePtr
)
{
    return cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, vidUpModePtr);
}

/*******************************************************************************
* cpssDxCh2EgressPclPacketTypesSet
*
* DESCRIPTION:
*   Enables/disables Egress PCL (EPCL) for set of packet types on port
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*   devNum        - device number
*   port          - port number
*   pktType       - packet type to enable/disable EPCL for it
*   enable        - enable EPCL for specific packet type
*                   GT_TRUE - enable
*                   GT_FALSE - disable
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
*   - after reset EPCL disabled for all packet types on all ports
*
*
*******************************************************************************/
GT_STATUS cpssDxCh2EgressPclPacketTypesSet
(
    IN GT_U8                             devNum,
    IN GT_U8                             port,
    IN CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT pktType,
    IN GT_BOOL                           enable
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */
    GT_U32  bitPos;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);

    /* get the start bit 0..63 (in the bmp of registers) */
    bitPos = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
             PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
             localPort;

    switch (pktType)
    {
        case CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktFromCpuControl[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktFromCpuData[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_TO_CPU_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktToCpu[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktToAnalyzer[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_TS_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktTs[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        case CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E:
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                    epclEnPktNonTs[OFFSET_TO_WORD_MAC(bitPos)];
            break;
        default: return GT_BAD_PARAM;
    }

    /* 1 - enable, 0 - disable */
    hwValue = (enable == GT_TRUE) ? 1 : 0;

    return prvCpssDrvHwPpPortGroupSetRegField(
        devNum, portGroupId,regAddr, OFFSET_TO_BIT_MAC(bitPos), 1, hwValue);
}

/*******************************************************************************
* cpssDxCh3PclTunnelTermForceVlanModeEnableSet
*
* DESCRIPTION:
*   The function enables/disables forcing of the PCL ID configuration
*   for all TT packets according to the VLAN assignment.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       enable         - force TT packets assigned to PCL
*                        configuration table entry
*                        GT_TRUE  - By VLAN
*                        GT_FALSE - according to ingress port
*                                   per lookup settings
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh3PclTunnelTermForceVlanModeEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_BOOL                       enable
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitNum;        /* bit number                          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;
        bitNum = 10;
    }
    else
    {
        /* DxCh1-3 and xCat devices */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        bitNum = 28;
    }


    /* 1 - enable, 0 - disable */
    hwValue = (enable == GT_FALSE) ? 0 : 1;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, bitNum, 1, hwValue);
}

/*******************************************************************************
* cpssDxCh3PclTunnelTermForceVlanModeEnableGet
*
* DESCRIPTION:
*   The function gets enable/disable of the forcing of the PCL ID configuration
*   for all TT packets according to the VLAN assignment.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       enablePtr      - (pointer to) force TT packets assigned to PCL
*                        configuration table entry
*                        GT_TRUE  - By VLAN
*                        GT_FALSE - according to ingress port
*                                   per lookup settings
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh3PclTunnelTermForceVlanModeEnableGet
(
    IN  GT_U8                         devNum,
    OUT GT_BOOL                       *enablePtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    bitNum;        /* bit number                          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E );

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;
        bitNum = 10;
    }
    else
    {
        /* DxCh1-3 devices */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        bitNum = 28;
    }


    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, bitNum, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* 1 - enable, 0 - disable */
    *enablePtr = (hwValue == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxCh2PclTcpUdpPortComparatorSet
*
* DESCRIPTION:
*        Configure TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (0-7)
*    l4PortType        - TCP/UDP port type, source or destination port
*    compareOperator   - compare operator FALSE, LTE, GTE, NEQ
*    value             - 16 bit value to compare with
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
GT_STATUS cpssDxCh2PclTcpUdpPortComparatorSet
(
    IN GT_U8                             devNum,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    IN CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN GT_U8                             entryIndex,
    IN CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType,
    IN CPSS_COMPARE_OPERATOR_ENT         compareOperator,
    IN GT_U16                            value
)
{
    return cpssDxChPclPortGroupTcpUdpPortComparatorSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        direction, l4Protocol, entryIndex,
        l4PortType, compareOperator, value);
}

/*******************************************************************************
* cpssDxChPclRuleStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*       Old function for DxCh1 and DxCh2 devices and for
*       standard rules on DxCh3 devices.
*       The general function is cpssDxChPclRuleAnyStateGet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
)
{
    return cpssDxChPclRuleAnyStateGet(
        devNum, CPSS_PCL_RULE_SIZE_STD_E,
        ruleIndex, validPtr, ruleSizePtr);
}

/*******************************************************************************
* cpssDxChPclRuleAnyStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleSize       - size of rule
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleAnyStateGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
)
{
    return cpssDxChPclPortGroupRuleAnyStateGet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleSize, ruleIndex,
        validPtr, ruleSizePtr);
}

/*******************************************************************************
* cpssDxChPclRuleGet
*
* DESCRIPTION:
*   The function gets the Policy Rule Mask, Pattern and Action in Hw format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleSize         - size of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*
* OUTPUTS:
*       maskArr          - rule mask          - 14  words.
*       patternArr       - rule pattern       - 14  words.
*       actionArr        - Policy rule action - 4 words.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleGet
(
    IN  GT_U8                  devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                 ruleIndex,
    OUT GT_U32                 maskArr[],
    OUT GT_U32                 patternArr[],
    OUT GT_U32                 actionArr[]
)
{
    return cpssDxChPclPortGroupRuleGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        ruleSize, ruleIndex,
        maskArr, patternArr, actionArr);
}

/*******************************************************************************
* cpssDxChPclCfgTblEntryGet
*
* DESCRIPTION:
*    Gets the PCL configuration table entry in Hw format.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    direction       - Ingress Policy or Egress Policy
*                        (CPSS_PCL_DIRECTION_ENT member),
*    lookupNum       - Lookup number: lookup0 or lookup1
*                      DxCh1-DxCh3 devices ignores the parameter
*                      xCat and above supports the parameter
*                      xCat and above devices has separate HW entries
*                      for both ingress lookups.
*                      Only lookup 0 is supported for egress.
*    entryIndex      - PCL configuration table entry Index
*
* OUTPUTS:
*    pclCfgTblEntryPtr    - pcl Cfg Tbl Entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_STATE             - in case of already bound pclHwId with same
*                            combination of slot/direction
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       DxCh devices support ingress direction only
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblEntryGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN  GT_U32                          entryIndex,
    OUT GT_U32                          *pclCfgTblEntryPtr
)
{
    return cpssDxChPclPortGroupCfgTblEntryGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        direction, lookupNum, entryIndex, pclCfgTblEntryPtr);
}

/*******************************************************************************
* cpssDxChPclLookupCfgPortListEnableSet
*
* DESCRIPTION:
*        The function enables/disables using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*    devNum            - device number
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
GT_STATUS cpssDxChPclLookupCfgPortListEnableSet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
)
{
    return cpssDxChPclPortGroupLookupCfgPortListEnableSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        direction, lookupNum, subLookupNum, enable);
}

/*******************************************************************************
* cpssDxChPclLookupCfgPortListEnableGet
*
* DESCRIPTION:
*        The function gets enable/disable state of
*        using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*
* OUTPUTS:
*    enablePtr         - (pointer to)
*                        GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookupCfgPortListEnableGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_BOOL                       *enablePtr
)
{
    return cpssDxChPclPortGroupLookupCfgPortListEnableGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        direction, lookupNum, subLookupNum, enablePtr);
}

/*******************************************************************************
* cpssDxChPclOverrideUserDefinedBytesSet
*
* DESCRIPTION:
*   The function sets overriding of  User Defined Bytes
*   by predefined optional key fields.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number
*       udbOverridePtr - (pointer to) UDB override structure
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclOverrideUserDefinedBytesSet
(
    IN  GT_U8                          devNum,
    IN  CPSS_DXCH_PCL_OVERRIDE_UDB_STC *udbOverridePtr
)
{
    return cpssDxChPclPortGroupOverrideUserDefinedBytesSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        udbOverridePtr);
}

/*******************************************************************************
* cpssDxChPclOverrideUserDefinedBytesGet
*
* DESCRIPTION:
*   The function gets overriding of  User Defined Bytes
*   by predefined optional key fields.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       udbOverridePtr - (pointer to) UDB override structure
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclOverrideUserDefinedBytesGet
(
    IN  GT_U8                          devNum,
    OUT CPSS_DXCH_PCL_OVERRIDE_UDB_STC *udbOverridePtr
)
{
    return cpssDxChPclPortGroupOverrideUserDefinedBytesGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        udbOverridePtr);
}

/*******************************************************************************
* cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet
*
* DESCRIPTION:
*   The function sets overriding of  User Defined Bytes
*   by packets Trunk Hash value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum              - device number
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
*       The Trunk Hash field cannot be included in the key of the
*       first IPCL lookup (Lookup0_0).
*       Thus, when the configuration for Trunk Hash in the Policy keys
*       is enabled, it applies only to the second and third lookups.
*
*******************************************************************************/
GT_STATUS cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
)
{
    return cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, udbOverTrunkHashPtr);
}

/*******************************************************************************
* cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet
*
* DESCRIPTION:
*   The function gets overriding of  User Defined Bytes
*   by packets Trunk Hash value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum              - device number
* OUTPUTS:
*       udbOverTrunkHashPtr - (pointer to) UDB override trunk hash structure
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
GT_STATUS cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
)
{
    return cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, udbOverTrunkHashPtr);
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleFormat       - format of the Rule.
*
*       ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case
*                          With reference to Standard and Extended rules
*                          indexes, the partitioning is as follows:
*
*                          For DxCh and DxCh2 devices:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1023.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 511.
*                          Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n <= 511
*                            is placed in the space of two standard rules with
*                            indexes n and n + 512.
*
*                          For DxCh3 device:
*                          The TCAM has 3.5K (3584) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 14335.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 7167.
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n < 3584
*                            is placed in the space of two standard rules with
*                            indexes n and n + 3584.
*                          - Extended rule with index  3584 <= n < 7168
*                            is placed in the space of two standard rules with
*                            indexes n + 3584, n + 7168.
*
*                          For xCat and above devices:
*                          See datasheet of particular device for TCAM size.
*                          For example describe the TCAM that has 1/4K (256) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1K (1024).
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 0.5K (512).
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index 2n (even index)
*                            is placed in the space of two standard rules with
*                            indexes 4n and 4n + 1.
*                          - Extended rule with index 2n+1 (odd index)
*                            is placed in the space of two standard rules with
*                            indexes 4n+2 and 4n + 3.
*
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*                               Write rule to TCAM not using port-list format.
*                               Rule state is valid.
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E -
*                               write all fields of rule to TCAM but rule's
*                               state is invalid (no match during lookups).
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E |
*                                             CPSS_DXCH_PCL_RULE_OPTION_PORT_LIST_ENABLED_E -
*                               write all fields of rule to TCAM using port-list format
*                               but rule's state is invalid
*                              (no match during lookups).
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*                          The format of mask is defined by ruleFormat
*
*       patternPtr       - rule pattern.
*                          The format of pattern is defined by ruleFormat
*
*       actionPtr        - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleSet
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_RULE_OPTION_ENT      ruleOptionsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    GT_STATUS    rc;            /* return code                        */
    GT_U32       ruleSize;      /* ruleSize (in STD rules)            */
    GT_U32       hwAction[PRV_CPSS_DXCH_PCL_ACTION_SIZE_IN_WORDS_CNS];
                                /* action on HW format                */
    GT_U32       hwMask[14];    /* mask on HW format                  */
    GT_U32       hwPattern[14]; /* pattern on HW format               */
    GT_BOOL      egressRule;    /* egress Rule                        */
    GT_BOOL      validRule;     /* write valid rule                   */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);
    CPSS_NULL_PTR_CHECK_MAC(patternPtr);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* check rule format and get extRule value */
    switch (ruleFormat)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
            ruleSize = 1;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
            ruleSize = 2;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
            PRV_CPSS_DXCH_PCL_XCAT_DEV_CHECK_MAC(devNum);
            ruleSize = 1;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
            PRV_CPSS_DXCH_PCL_XCAT_DEV_CHECK_MAC(devNum);
            ruleSize = 2;
            break;
        default: return GT_BAD_PARAM;
    }

    PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(devNum, ruleSize, ruleIndex);

    rc = prvCpssDxChPclRuleDataSw2HwConvert(
        devNum, ruleFormat, ruleOptionsBmp,
        maskPtr, patternPtr, actionPtr,
        hwMask, hwPattern, hwAction);
    if (rc != GT_OK)
    {
        return rc;
    }

    if ((ruleFormat >= CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E)
        && (ruleFormat <= CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E))
    {
        /* Egress Pcl rule */
        egressRule = GT_TRUE;
    }
    else
    {
        /* Iggress Pcl rule */
        egressRule = GT_FALSE;
    }

    validRule =
        (ruleOptionsBmp & CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E)
        ? GT_FALSE : GT_TRUE;

    /* write rule */
    return prvCpssDxChPclTcamRuleModifiedWrite(
        devNum, portGroupsBmp,
        ruleSize, ruleIndex, egressRule, validRule,
        GT_TRUE /*waitPrevReady*/, hwAction, hwMask, hwPattern);
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    GT_STATUS    rc;          /* return code                 */
    GT_U32       portGroupId; /*the port group Id            */
    GT_U32       hwAction[PRV_CPSS_DXCH_PCL_ACTION_SIZE_IN_WORDS_CNS];
                              /* action on HW format       */
    GT_U32       ruleSizeVal; /* ruleSize (in STD rules)   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);
    PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(ruleSize, ruleSizeVal);
    PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(devNum, ruleSizeVal, ruleIndex);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* convert action to HW format */
    rc = prvCpssDxChPclTcamRuleActionSw2HwConvert(
        devNum, actionPtr, hwAction);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* write action */
        rc = prvCpssDxChPclTcamRuleActionUpdate(
            devNum, portGroupId,
            ruleSizeVal, ruleIndex, hwAction);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleActionGet
*
* DESCRIPTION:
*   The function gets the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
*       direction       - Policy direction:
*                         Ingress or Egress
*                         Needed for parsing
*
* OUTPUTS:
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    IN  CPSS_PCL_DIRECTION_ENT             direction,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    GT_STATUS    rc;          /* return code                 */
    GT_U32       portGroupId; /*the port group Id            */
    GT_U32       ruleSizeVal; /* rule  size (in STD rules)   */
    GT_U32       action[PRV_CPSS_DXCH_PCL_ACTION_SIZE_IN_WORDS_CNS];
                               /* action on HW format       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);
    PRV_CPSS_DXCH_PCL_DIRECTION_CHECK_MAC(direction);
    PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(ruleSize, ruleSizeVal);
    PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(devNum, ruleSizeVal, ruleIndex);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    rc = prvCpssDxChPclTcamActionGet(
        devNum, portGroupId,
        ruleSizeVal, ruleIndex, GT_TRUE /*waitPrevReady*/, action);
    if (rc != GT_OK)
    {
        return rc;
    }

    return prvCpssDxChPclTcamRuleActionHw2SwConvert(
        devNum, direction, action, actionPtr);
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleInvalidate
*
* DESCRIPTION:
*   The function invalidates the Policy Rule.
*   For DxCh and DxCh2 devices start indirect rule write operation
*   with "garbage" pattern and mask content and "valid-flag" == 0
*   For DxCh3 devices calculates the TCAM position by ruleSize and ruleIndex
*   parameters and updates the first "valid" bit matching X/Y pair to (1,1)
*   i.e. don't match any value. If origin of valid or invalid rule found at the
*   specified TCAM position it will be invalid and available to back validation.
*   If the specified TCAM position contains the not first 24 byte segment of
*   extended rule, the rule also will be invalidated,
*   but unavailable to back validation.
*   If the garbage found in TCAM the X/Y bits will be updated to be as
*   in invalid rule.
*   The function does not check the TCAM contents.
*   GT_OK will be returned in each of described cases.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Backward compatible styled API.
*       The cpssDxChPclRuleValidStatusSet recommended for using instead.
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleInvalidate
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex
)
{
    GT_STATUS    rc;            /* return code                             */
    GT_U32       portGroupId;   /*the port group Id                        */
    GT_U32       ruleSizeVal;   /* rule size (in std rules)                */
    GT_U32       absIndex;      /* absolute rule index (in STD rules)      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(ruleSize, ruleSizeVal);
    PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(devNum, ruleSizeVal, ruleIndex);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        /* CH and CH2 devices */
        /* invalidate rule    */
        rc = prvCpssDxChPclTcamOperationStart(
            devNum,
            ruleSizeVal /*ruleSize*/,
            ruleIndex, GT_TRUE /*waitPrevReady*/,
            PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_INVALID  /*tcamOperation*/);

        return rc;
    }

    /* CH3 devices and above */

    /* get index of first Standard part of rule */
    rc = prvCpssDxChPclTcamRuleAbsoluteIndexGet(
        devNum, ruleSizeVal, ruleIndex, 0/*ruleSegment*/, &absIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* invalidate first standard part of rule */
        rc = prvCpssDxChPclTcamStdRuleValidStateSet(
            devNum, portGroupId, absIndex, GT_FALSE);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleValidStatusSet
*
* DESCRIPTION:
*     Validates/Invalidates the Policy rule.
*        The validation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Valid indication set.
*           The function does not check content of the rule before
*           write it back to TCAM
*        The invalidation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Invalid indication set.
*        If the given the rule found already in needed valid state
*        no write done. If the given the rule found with size
*        different from the given rule-size an error code returned.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM.
*       valid           - new rule status: GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - if in TCAM found rule of size different
*                        from the specified
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleValidStatusSet
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN GT_BOOL                            valid
)
{
    GT_STATUS    rc;                           /* return code              */
    GT_U32       portGroupId;                  /* the port group Id        */
    GT_U32       ruleSizeVal;                  /* rule size (in std rules) */
    GT_U32       ruleSizeFound;                /* rule size found          */
    GT_BOOL      validFound;                   /* is rule found valid      */
    GT_U32       mask[12];                     /* TCAM mask of rule        */
    GT_U32       pattern[12];                  /* TCAM pattern of rule     */
    PRV_CPSS_DXCH_PCL_TCAM_OPERATION tcamOper; /* write valid or invalid   */
    GT_U32       absIndex;      /* absolute rule inex (in STD rules)       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(ruleSize, ruleSizeVal);
    PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(devNum, ruleSizeVal, ruleIndex);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssDxChPclTcamRuleStateGet(
            devNum, portGroupId,
            ruleSizeVal, ruleIndex, &validFound, &ruleSizeFound);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (ruleSizeVal != ruleSizeFound)
        {
            /* source rule has another size */
            return GT_BAD_STATE;
        }

        if (valid == validFound)
        {
            /* the rule is aready in requred valid state */
            return GT_OK;
        }

        if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            tcamOper = (valid == GT_FALSE)
                ? PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_INVALID
                : PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_VALID;

            rc = prvCpssDxChPclTcamRuleDataGet(
                devNum, portGroupId,
                ruleSizeVal, ruleIndex, mask, pattern);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* put pattern data to buffer registers */
            rc = prvCpssDxChPclTcamOperDataPut(
                devNum, ruleSizeVal, tcamOper, pattern);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* validate/invalidate rule */
            rc = prvCpssDxChPclTcamOperationStart(
                devNum, ruleSizeVal, ruleIndex, GT_TRUE /*waitPrevReady*/, tcamOper);
            {
                return rc;
            }
        }
        else
        {
            /* Cheetah3 and above */
            rc = prvCpssDxChPclTcamRuleAbsoluteIndexGet(
                devNum, ruleSizeVal, ruleIndex, 0/*ruleSegment*/, &absIndex);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* modify validity of first standard part of rule*/
            rc = prvCpssDxChPclTcamStdRuleValidStateSet(
                devNum, portGroupId, absIndex, valid);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleAnyStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize       - size of rule
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleAnyStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
)
{
    GT_U32    ruleSizeVal;   /* rule size (in STD rules)              */
    GT_U32    ruleSizeFound; /* rule size (in STD rules)              */
    GT_STATUS rc;            /* return code                           */
    GT_U32    portGroupId;   /* the port group Id                     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(ruleSizePtr);
    PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(ruleSize, ruleSizeVal);
    PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(devNum, ruleSizeVal, ruleIndex);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    rc = prvCpssDxChPclTcamRuleStateGet(
        devNum, portGroupId,
        ruleSizeVal, ruleIndex, validPtr, &ruleSizeFound);
    if (rc != GT_OK)
    {
        return rc;
    }

    *ruleSizePtr = (ruleSizeFound == 1)
        ? CPSS_PCL_RULE_SIZE_STD_E : CPSS_PCL_RULE_SIZE_EXT_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleCopy
*
* DESCRIPTION:
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first cpssDxChPclPortGroupRuleCopy should be
*  called. And after this cpssDxChPclPortGroupRuleInvalidate should be used to invalidate
*  Rule in old position
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize         - size of Rule.
*       ruleSrcIndex     - index of the rule in the TCAM from which pattern,
*                           mask and action are taken.
*       ruleDstIndex     - index of the rule in the TCAM to which pattern,
*                           mask and action are placed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleCopy
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
)
{
    GT_STATUS    rc;          /* return code                 */
    GT_U32       portGroupId; /* port group Id for multi-port-group devices */
    GT_U32       ruleSizeVal; /* rule size (in std rules)      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(ruleSize, ruleSizeVal);
    PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(
        devNum, ruleSizeVal, ruleSrcIndex);
    PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(
        devNum, ruleSizeVal, ruleDstIndex);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssDxChPclRuleCopy(
            devNum, portGroupId,
            ruleSizeVal, ruleSrcIndex, ruleDstIndex);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupRuleGet
*
* DESCRIPTION:
*   The function gets the Policy Rule Mask, Pattern and Action in Hw format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize         - size of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*
* OUTPUTS:
*       maskArr          - rule mask          - 14  words.
*       patternArr       - rule pattern       - 14  words.
*       actionArr        - Policy rule action - 4 words.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleGet
(
    IN  GT_U8                  devNum,
    IN  GT_PORT_GROUPS_BMP     portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                 ruleIndex,
    OUT GT_U32                 maskArr[],
    OUT GT_U32                 patternArr[],
    OUT GT_U32                 actionArr[]
)
{
    GT_STATUS  rc;            /* return code                 */
    GT_U32     portGroupId;   /* port group Id               */
    GT_BOOL    valid;         /* valid source rule           */
    GT_U32     ruleSizeVal;   /* rule size (in STD rules)    */
    GT_U32     ruleSizeFound; /* rule size (in STD rules)    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(maskArr);
    CPSS_NULL_PTR_CHECK_MAC(patternArr);
    CPSS_NULL_PTR_CHECK_MAC(actionArr);
    PRV_CPSS_DXCH_PCL_GET_RULE_SIZE_VALUE_MAC(ruleSize, ruleSizeVal);
    PRV_CPSS_DXCH_PCL_RULE_SIZE_INDEX_CHECK_MAC(
        devNum, ruleSizeVal, ruleIndex);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    rc = prvCpssDxChPclTcamRuleStateGet(
        devNum, portGroupId,
        ruleSizeVal, ruleIndex, &valid, &ruleSizeFound);
    if (rc != GT_OK)
    {
        return rc;
    }

    if ((ruleSizeVal != ruleSizeFound) && (valid != GT_FALSE))
    {
        /* source rule is valid, but has another size */
        return GT_BAD_STATE;
    }

    rc = prvCpssDxChPclTcamActionGet(
        devNum, portGroupId,
        ruleSizeVal, ruleIndex, GT_TRUE /*waitPrevReady*/, actionArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPclTcamRuleDataGet(
        devNum, portGroupId,
        ruleSizeVal, ruleIndex, maskArr, patternArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       interfaceInfoPtr  - interface data: port, VLAN, or index for setting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be of the same size, Standard IPV6 DIP key allowed only on lookup1
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupCfgTblSet
(
    IN GT_U8                           devNum,
    IN GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN CPSS_INTERFACE_INFO_STC         *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT          direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_STC    lookupHwCfg;
    GT_STATUS res;           /* return code                                       */
    GT_U32    entryIndex;    /* pcl configuration table entry Index               */
    GT_BOOL   extKey;        /* GT_TRUE - extended key, GT_FALSE - standard       */
    GT_BOOL   l4Key;         /* GT_TRUE - l4 style key, GT_FALSE - l2 style       */
    GT_BOOL   dipKey;        /* GT_TRUE - ipv6 DIP style key, GT_FALSE - l2 style */
    GT_U32    hwValue;       /* pcl configuration table HW value                  */
    GT_U32    portGroupId;        /*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(interfaceInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(lookupCfgPtr);
    PRV_CPSS_DXCH_PCL_LOOKUP_NUM_CHECK_MAC(lookupNum);

    res = prvCpssDxChPclCfgTblEntryIndexGet(
        devNum, interfaceInfoPtr, direction, &entryIndex , &portGroupId);
    if (res != GT_OK)
    {
        return res;
    }

    if (portGroupId != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        if ((portGroupsBmp  != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
            && (portGroupsBmp != (GT_U32)(1 << portGroupId)))
        {
            /* valid only "All port groups" or the correct port group */
            return GT_BAD_PARAM;
        }
        /* update bitmap to correct port group */
        portGroupsBmp = (1 << portGroupId);
    }

    /* common for all cases */
    lookupHwCfg.enableLookup = (GT_U8)BOOL2BIT_MAC(lookupCfgPtr->enableLookup);
    lookupHwCfg.pclId        = (GT_U16)lookupCfgPtr->pclId;

    if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat2 and above - 3 configuration tables */
        lookupHwCfg.dualLookup   = 0;
        lookupHwCfg.pclIdL01     = 0;
    }
    else if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and Lion */
        lookupHwCfg.dualLookup   =
            (GT_U8)BOOL2BIT_MAC(lookupCfgPtr->dualLookup);
        lookupHwCfg.pclIdL01     = (GT_U16)lookupCfgPtr->pclIdL01;
    }
    else
    {
        /* DxCh1-3 */
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
        {
            /* DxCh3 */
            lookupHwCfg.dualLookup   =
                (GT_U8)BOOL2BIT_MAC(lookupCfgPtr->dualLookup);
        }
        else
        {
            /* DxCh1-2 devices */
            lookupHwCfg.dualLookup   = 0;
        }
        lookupHwCfg.pclIdL01     = 0;
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
         == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* check Non IP Key type and set extKey variable */
        switch (lookupCfgPtr->groupKeyTypes.nonIpKey)
        {
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
                extKey = GT_FALSE;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
                extKey = GT_TRUE;
                break;
            /* all other types not relevant */
            default: return GT_BAD_PARAM;
        }

        /* Set default for l4Key and prevent compiler warning */
        l4Key = GT_FALSE;

        switch (lookupCfgPtr->groupKeyTypes.ipv4Key)
        {
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
                if (extKey == GT_TRUE)
                {
                    /* both IPv4 and nonIp must be same size */
                    return GT_BAD_PARAM;
                }
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
                if (extKey == GT_TRUE)
                {
                    /* both IPv4 and nonIp must be same size */
                    return GT_BAD_PARAM;
                }
                l4Key  = GT_TRUE;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
                if (extKey == GT_FALSE)
                {
                    /* both IPv4 and nonIp must be same size */
                    return GT_BAD_PARAM;
                }
                break;
            /* all other types not relevant */
            default: return GT_BAD_PARAM;
        }

        /* Set default for dipKey and prevent compiler warning */
        dipKey = GT_FALSE;

        switch (lookupCfgPtr->groupKeyTypes.ipv6Key)
        {
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
                if (extKey == GT_TRUE)
                {
                    /* both IPv6 and nonIp must be same size */
                    return GT_BAD_PARAM;
                }
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
                if (extKey == GT_TRUE)
                {
                    /* both IPv6 and nonIp must be same size */
                    return GT_BAD_PARAM;
                }
                if (lookupNum == CPSS_PCL_LOOKUP_0_E)
                {
                    /* DIP only for lookup1*/
                    return GT_BAD_PARAM;
                }
                dipKey = GT_TRUE;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
                if (extKey == GT_FALSE)
                {
                    /* both IPv6 and nonIp must be same size */
                    return GT_BAD_PARAM;
                }
                l4Key  = GT_FALSE;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
                if (extKey == GT_FALSE)
                {
                    /* both IPv6 and nonIp must be same size */
                    return GT_BAD_PARAM;
                }
                l4Key  = GT_TRUE;
                break;
            /* all other types not relevant */
            default: return GT_BAD_PARAM;
        }

        /* DxChPlus specific field values use */
        lookupHwCfg.nonIpKeyType = (GT_U8)((extKey == GT_FALSE) ? 0 : 1);
        lookupHwCfg.ipv4KeyType  = (GT_U8)((l4Key == GT_FALSE) ? 0 : 1);
        lookupHwCfg.ipv6KeyType  = (GT_U8)((dipKey == GT_FALSE) ? 0 : 1);
    }
    else
    {  /* DxCh2 and above */

        /* non IP key style */
        res = prvCpssDxChPclKeyTypeSwToHw(
            devNum, direction, PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_NOT_IP_CNS,
            lookupCfgPtr->groupKeyTypes.nonIpKey,
            &hwValue);
        if (res != GT_OK)
        {
            return res;
        }
        lookupHwCfg.nonIpKeyType = (GT_U8)hwValue;

        /* IPv4 key style */
        res = prvCpssDxChPclKeyTypeSwToHw(
            devNum, direction, PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV4_CNS,
            lookupCfgPtr->groupKeyTypes.ipv4Key,
            &hwValue);
        if (res != GT_OK)
        {
            return res;
        }
        lookupHwCfg.ipv4KeyType = (GT_U8)hwValue;

        /* IPv6 key style */
        res = prvCpssDxChPclKeyTypeSwToHw(
            devNum, direction, PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV6_CNS,
            lookupCfgPtr->groupKeyTypes.ipv6Key,
            &hwValue);
        if (res != GT_OK)
        {
            return res;
        }
        lookupHwCfg.ipv6KeyType = (GT_U8)hwValue;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        res = prvCpssDxChPclCfgTblHwWrite(
            devNum, portGroupId , direction, entryIndex,
            lookupNum, 0 /*subLookupNum*/, &lookupHwCfg);
        if (res != GT_OK)
        {
            return res;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    if ((0 == PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
        || (direction != CPSS_PCL_DIRECTION_INGRESS_E)
        || (lookupNum != CPSS_PCL_LOOKUP_0_E))
    {
        /* only one configuration table needed update */
        return GT_OK;
    }

    /* xCat2 ingress lookup01 table update              */
    /* Ingress PCL CPSS_PCL_LOOKUP_0_E - configure      */
    /* lookup_0_1 with the same key types as lookup_0_0 */
    lookupHwCfg.enableLookup = (GT_U8)BOOL2BIT_MAC(lookupCfgPtr->dualLookup);
    lookupHwCfg.pclId        = (GT_U16)lookupCfgPtr->pclIdL01;
    /* key types alredy converted for lookup_0_0 */
    /* lookupHwCfg.nonIpKeyType;                 */
    /* lookupHwCfg.ipv4KeyType;                  */
    /* lookupHwCfg.ipv6KeyType;                  */

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        res = prvCpssDxChPclCfgTblHwWrite(
            devNum, portGroupId , direction, entryIndex,
            lookupNum, 1 /*subLookupNum*/, &lookupHwCfg);
        if (res != GT_OK)
        {
            return res;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupCfgTblGet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       interfaceInfoPtr  - interface data: port, VLAN, or index for getting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*
* OUTPUTS:
*       lookupCfgPtr      - (pointer to) lookup configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be the same size, Standard IPV6 DIP key allowed only on lookup1
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupCfgTblGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    OUT CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_STC    lookupHwCfg;
    GT_STATUS res;           /* return code                            */
    GT_U32    entryIndex;    /* index of PCL configuration table entry */
    GT_U32  portGroupId;     /* the port group Id                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(interfaceInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(lookupCfgPtr);
    PRV_CPSS_DXCH_PCL_LOOKUP_NUM_CHECK_MAC(lookupNum);

    /* clear output data */
    cpssOsMemSet(lookupCfgPtr, 0, sizeof(CPSS_DXCH_PCL_LOOKUP_CFG_STC));

    res = prvCpssDxChPclCfgTblEntryIndexGet(
        devNum, interfaceInfoPtr, direction, &entryIndex , &portGroupId);
    if (res != GT_OK)
    {
        return res;
    }

    if (portGroupId != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        if ((portGroupsBmp  != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
            && (portGroupsBmp != (GT_U32)(1 << portGroupId)))
        {
            /* valid only "All port groups" or the correct port group */
            return GT_BAD_PARAM;
        }
        /* update bitmap to correct port group */
        portGroupsBmp = (1 << portGroupId);
    }
    else
    {
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
            devNum, portGroupsBmp, portGroupId);
    }

    res = prvCpssDxChPclCfgTblHwRead(
        devNum, portGroupId , direction, entryIndex,
        lookupNum, 0 /*subLookupNum*/, &lookupHwCfg);
    if (res != GT_OK)
    {
        return res;
    }

    /* common for all devices */
    lookupCfgPtr->enableLookup = BIT2BOOL_MAC(lookupHwCfg.enableLookup);
    lookupCfgPtr->pclId        = lookupHwCfg.pclId;
    lookupCfgPtr->dualLookup   = BIT2BOOL_MAC(lookupHwCfg.dualLookup);
    lookupCfgPtr->pclIdL01     = lookupHwCfg.pclIdL01;

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
         == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* nonIpKey */
        switch (lookupHwCfg.nonIpKeyType)
        {
            case 0: /* standard */
                lookupCfgPtr->groupKeyTypes.nonIpKey =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
                break;
            case 1: /* extended */
                lookupCfgPtr->groupKeyTypes.nonIpKey =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
                break;
            default: return GT_BAD_STATE; /* must never occure */
        }

        /* IPV4 */
        switch (lookupHwCfg.nonIpKeyType | (lookupHwCfg.ipv4KeyType << 1))
        {
            case 0: /* standard, IP L2 */
                lookupCfgPtr->groupKeyTypes.ipv4Key =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
                break;
            case 2: /* standard, IP L4 */
                lookupCfgPtr->groupKeyTypes.ipv4Key =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
                break;
            default: /* extended, any */
                lookupCfgPtr->groupKeyTypes.ipv4Key =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
                break;
        }

        /* IPV6 */
        switch (lookupHwCfg.nonIpKeyType
                | (lookupHwCfg.ipv4KeyType << 1)
                | (lookupHwCfg.ipv6KeyType << 2))
        {
            case 0: /* standard, IP L2 */
            case 2: /* standard, IP L4 */
                lookupCfgPtr->groupKeyTypes.ipv6Key =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
                break;
            case 4: /* standard, IP L2, DIP */
            case 6: /* standard, IP L4, DIP */
                lookupCfgPtr->groupKeyTypes.ipv6Key =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E;
                break;
            case 1: /* standard, IP L2, */
            case 5: /* extended, IP L2, DIP - presents, but not works */
                lookupCfgPtr->groupKeyTypes.ipv6Key =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;
                break;
            case 3: /* extended, IP L4 */
            case 7: /* extended, IP L4, DIP - presents, but not works */
                lookupCfgPtr->groupKeyTypes.ipv6Key =
                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E;
                break;
            default: return GT_BAD_STATE; /* must never occur */
        }

        return GT_OK;
    }

    /* CH2 and above code */

    /* non IP key style */
    res = prvCpssDxChPclKeyTypeHwToSw(
        devNum, direction, PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_NOT_IP_CNS,
        lookupHwCfg.nonIpKeyType,
        &lookupCfgPtr->groupKeyTypes.nonIpKey);
    if (res != GT_OK)
    {
        return res;
    }

    /* IPv4 key style */
    res = prvCpssDxChPclKeyTypeHwToSw(
        devNum, direction, PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV4_CNS,
        lookupHwCfg.ipv4KeyType,
        &lookupCfgPtr->groupKeyTypes.ipv4Key);
    if (res != GT_OK)
    {
        return res;
    }

    /* IPv6 key style */
    res = prvCpssDxChPclKeyTypeHwToSw(
        devNum, direction, PRV_CPSS_DXCH_PCL_CFG_PKT_TYPE_IPV6_CNS,
        lookupHwCfg.ipv6KeyType,
        &lookupCfgPtr->groupKeyTypes.ipv6Key);
    if (res != GT_OK)
    {
        return res;
    }

    if ((0 == PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
        || (direction != CPSS_PCL_DIRECTION_INGRESS_E)
        || (lookupNum != CPSS_PCL_LOOKUP_0_E))
    {
        /* only one configuration table needed retrieve */
        return GT_OK;
    }

    /* xCat2 ingress lookup01 table read                       */
    /* Ingress PCL CPSS_PCL_LOOKUP_0_E - means both sublookups */

    res = prvCpssDxChPclCfgTblHwRead(
        devNum, portGroupId , direction, entryIndex,
        lookupNum, 1 /*subLookupNum*/, &lookupHwCfg);
    if (res != GT_OK)
    {
        return res;
    }

    /* lookup_0_1 related data                    */
    /* lookup_0_1 key types assumed as lookup_0_0 */
    lookupCfgPtr->dualLookup = BIT2BOOL_MAC(lookupHwCfg.enableLookup);
    lookupCfgPtr->pclIdL01   = lookupHwCfg.pclId;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupCfgTblEntryGet
*
* DESCRIPTION:
*    Gets the PCL configuration table entry in Hw format.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction       - Ingress Policy or Egress Policy
*                        (CPSS_PCL_DIRECTION_ENT member),
*    lookupNum       - Lookup number: lookup0 or lookup1
*                      DxCh1-DxCh3 devices ignores the parameter
*                      xCat and above supports the parameter
*                      xCat and above devices has separate HW entries
*                      for both ingress lookups.
*                      Only lookup 0 is supported for egress.
*    entryIndex      - PCL configuration table entry Index
*
* OUTPUTS:
*    pclCfgTblEntryPtr    - pcl Cfg Tbl Entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_STATE             - in case of already bound pclHwId with same
*                            combination of slot/direction
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       DxCh devices support ingress direction only
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupCfgTblEntryGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN  GT_U32                          entryIndex,
    OUT GT_U32                          *pclCfgTblEntryPtr
)
{
    GT_STATUS rc;              /* return code                              */
    GT_U32    portGroupId;     /* the port group Id                        */
    PRV_CPSS_DXCH_TABLE_ENT tableId; /* id of ingress of egress Policy Cfg */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(pclCfgTblEntryPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if ((PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
        && (direction == CPSS_PCL_DIRECTION_INGRESS_E)
        && (lookupNum == CPSS_PCL_LOOKUP_0_E))
    {
        /* xCat2 read entries from both lookup0 and lookup01 tables */

        /* lookup0 */
        tableId = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;
        rc = prvCpssDxChPortGroupReadTableEntry(
            devNum, portGroupId, tableId, entryIndex, pclCfgTblEntryPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* lookup01 - next two words */
        tableId = PRV_CPSS_DXCH_XCAT2_TABLE_INGRESS_PCL_LOOKUP01_CONFIG_E;
        rc = prvCpssDxChPortGroupReadTableEntry(
            devNum, portGroupId, tableId, entryIndex, &(pclCfgTblEntryPtr[2]));

        return rc;
    }

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
            {
                /* xCat2 and above */
                switch (lookupNum)
                {
                    /* case CPSS_PCL_LOOKUP_0_E treated above */
                    case CPSS_PCL_LOOKUP_0_0_E:
                        tableId = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;
                        break;
                    case CPSS_PCL_LOOKUP_0_1_E:
                        tableId =
                            PRV_CPSS_DXCH_XCAT2_TABLE_INGRESS_PCL_LOOKUP01_CONFIG_E;
                        break;
                    case CPSS_PCL_LOOKUP_1_E:
                        tableId =
                            PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E;
                        break;
                    default: return GT_BAD_PARAM;
                }
            }
            else if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* xCat and above */
                switch (lookupNum)
                {
                    case CPSS_PCL_LOOKUP_0_E:
                        tableId = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;
                        break;
                    case CPSS_PCL_LOOKUP_1_E:
                        tableId =
                            PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E;
                        break;
                    default: return GT_BAD_PARAM;
                }
            }
            else
            {
                /* DxCh1-DxCh3 */
                tableId = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;
            }
            break;
        case CPSS_PCL_DIRECTION_EGRESS_E:
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
                 == CPSS_PP_FAMILY_CHEETAH_E)
            {
                return GT_BAD_PARAM;
            }
            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* xCat and above */
                if (lookupNum != CPSS_PCL_LOOKUP_0_E)
                {
                    return GT_BAD_PARAM;
                }
            }
            tableId = PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E;
            break;
        default: return GT_BAD_PARAM;
    }

    return prvCpssDxChPortGroupReadTableEntry(
        devNum, portGroupId, tableId, entryIndex, pclCfgTblEntryPtr);
}

/*******************************************************************************
* cpssDxChPclPortGroupTcpUdpPortComparatorSet
*
* DESCRIPTION:
*        Configure TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (0-7)
*    l4PortType        - TCP/UDP port type, source or destination port

*    compareOperator   - compare operator FALSE, LTE, GTE, NEQ
*    value             - 16 bit value to compare with
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupTcpUdpPortComparatorSet
(
    IN GT_U8                             devNum,
    IN GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    IN CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN GT_U8                             entryIndex,
    IN CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType,
    IN CPSS_COMPARE_OPERATOR_ENT         compareOperator,
    IN GT_U16                            value
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    directonNum;   /* direction => 0 or 1                 */
    GT_U32    protocolNum ;  /* protocol => 0 or 1                  */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    portGroupId;   /* the port group Id                   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if(entryIndex > 7)
    {
        return GT_BAD_PARAM;
    }

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E: directonNum = 0; break;
        case CPSS_PCL_DIRECTION_EGRESS_E:  directonNum = 1; break;
        default: return GT_BAD_PARAM;
    }

    switch (l4Protocol)
    {
        case CPSS_L4_PROTOCOL_TCP_E: protocolNum = 0; break;
        case CPSS_L4_PROTOCOL_UDP_E: protocolNum = 1; break;
        default: return GT_BAD_PARAM;
    }

    switch ((direction << 1) | protocolNum)
    {
        case 0: /* ingress TCP */
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                ipclTcpPortComparator0 + (entryIndex * 4);
            break;
        case 1: /* ingress UDP */
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                ipclUdpPortComparator0 + (entryIndex * 4);
            break;
        case 2: /* egress TCP */
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                epclTcpPortComparator0 + (entryIndex * 4);
            break;
        case 3: /* egress UDP */
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                epclUdpPortComparator0 + (entryIndex * 4);
            break;
        default: return GT_BAD_PARAM;
    }

    hwValue = (value & 0xFFFF);

    switch (l4PortType)
    {
        case CPSS_L4_PROTOCOL_PORT_SRC_E: /* bit18 aleady 0 */  break;
        case CPSS_L4_PROTOCOL_PORT_DST_E: hwValue |= (1 << 18); break;
        default: return GT_BAD_PARAM;
    }

    switch (compareOperator)
    {
        case CPSS_COMPARE_OPERATOR_INVALID_E: /*bit16-17 aleady 0*/ break;
        case CPSS_COMPARE_OPERATOR_LTE:       hwValue |= (2 << 16); break;
        case CPSS_COMPARE_OPERATOR_GTE:       hwValue |= (1 << 16); break;
        case CPSS_COMPARE_OPERATOR_NEQ:       hwValue |= (3 << 16); break;
        default: return GT_BAD_PARAM;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, portGroupId, regAddr, 0, 19, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*       offsetType   - the type of offset (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_OUT_OF_RANGE          - parameter value more then HW bit field
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
)
{
    GT_STATUS rc;              /* return code              */
    GT_U32    regAddr;         /* register address         */
    GT_U32    regMask;         /* mask to update register  */
    GT_U32    regValue;        /* value to update register */
    GT_U32    startBit;        /* start Bit                */
    GT_U32    startWord;       /* start Word               */
    GT_U32    validTypesMask;  /* valid Types Mask         */
    GT_U32    comparatorBit;   /* comparator Bit           */
    GT_U32    hwOffType;       /* offset type in HW format */
    GT_U32    udbEntryIndex;   /* XCat HW UDB entry index  */
    GT_U32    udbEntryArray[8];/* XCat HW UDB entry        */
    GT_U32    portGroupId;     /*the port group Id         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* for all Cheetah devices */
    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        if (offset >= BIT_7)
        {
            return GT_OUT_OF_RANGE;
        }

        rc = prvCpssDxChCheetahPclUDBRegGet(
            devNum, ruleFormat, udbIndex,
            &regAddr, &startBit, &validTypesMask, &comparatorBit);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* The updates gathered in "regMask" and "regValue" step by step.        */
        regMask = regValue = 0;

        /* special case of comparator */
        if (offsetType == CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E)
        {
            if (comparatorBit > 31)
            {
                return GT_BAD_PARAM;
            }

            /* set "comparator" bit to 1 */
            regMask  |= (1 << comparatorBit) ;
            regValue |= (1 << comparatorBit) ;
        }
        else
        {
            /* drop comparator if was */
            if (comparatorBit <= 31)
            {
                /* set "comparator" bit to 0 */
                regMask |= (1 << comparatorBit);
                /* regValue assumed == 0 */
            }

            switch (offsetType)
            {
                case CPSS_DXCH_PCL_OFFSET_L2_E:
                    hwOffType = UDB_HW_L2_CNS;
                    break;
                case CPSS_DXCH_PCL_OFFSET_L3_E:
                    hwOffType = UDB_HW_L3_CNS;
                    break;
                case CPSS_DXCH_PCL_OFFSET_L4_E:
                    hwOffType = UDB_HW_L4_CNS;
                    break;
                case CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E:
                    hwOffType = UDB_HW_IPV6EH_CNS;
                    break;
                default: return GT_BAD_PARAM;
            }

            if ((validTypesMask & (1 << hwOffType)) == 0)
            {
                return GT_BAD_PARAM;
            }

            /* set offset-type and offset */
            regMask  |= (0x1FF << startBit);
            regValue |= (((hwOffType << 7) | offset) << startBit);
        }

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(
                devNum, portGroupId, regAddr, regMask, regValue);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)

        return GT_OK;
    }

    /* xCat devices */

    if (udbIndex >= 23)
    {
        return GT_BAD_PARAM;
    }

    if (offset >= BIT_7)
    {
        return GT_OUT_OF_RANGE;
    }

    /* Calculate entry index by packet type */
    rc = prvCpssDxChXCatPclUDBEntryIndexGet(
        devNum, packetType, &udbEntryIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    regMask  = 0x07FF;
    regValue = (offset << 3);

    /* bit 0 means "valid", bit 10 - "comparator" */
    switch (offsetType)
    {
        case CPSS_DXCH_PCL_OFFSET_L2_E:
            regValue |= (UDB_XCAT_HW_L2_CNS << 1) | 1;
            break;
        case CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E:
            regValue |= (UDB_XCAT_HW_L3_CNS << 1) | 1;
            break;
        case CPSS_DXCH_PCL_OFFSET_L4_E:
            regValue |= (UDB_XCAT_HW_L4_CNS << 1) | 1;
            break;
        case CPSS_DXCH_PCL_OFFSET_MPLS_MINUS_2_E:
            regValue |= (UDB_XCAT_HW_MPLS_CNS << 1) | 1;
            break;
        case CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E:
            regValue |= (UDB_XCAT_HW_L2_CNS << 1) | 1 | (1 << 10);
            break;
        case CPSS_DXCH_PCL_OFFSET_INVALID_E:
            /* L2 without valid bit*/
            regValue |= (UDB_XCAT_HW_L2_CNS << 1);
            break;
        default: return GT_BAD_PARAM;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* read all packet type related 23-UDB entry */
        rc = prvCpssDxChPortGroupReadTableEntry(
            devNum, portGroupId,
            PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E,
            udbEntryIndex, udbEntryArray);
        if (rc != GT_OK)
        {
            return rc;
        }

        startBit  =  (11 * udbIndex);
        startWord =  startBit >> 5; /* / 32 */
        startBit  &= 0x001F;        /* % 32 */

        udbEntryArray[startWord] &= (GT_U32)(~ (regMask << startBit));
        udbEntryArray[startWord] |= (GT_U32)(regValue << startBit);

        if ((startBit + 11) > 32)
        {
            /* amount of already copied bits */
            startBit  = 32 - startBit;
            startWord ++;
            udbEntryArray[startWord] &= (GT_U32)(~ (regMask >> startBit));
            udbEntryArray[startWord] |= (GT_U32)(regValue >> startBit);
        }

        /* Wrire updated packet type related 23-UDB entry */
        rc =  prvCpssDxChPortGroupWriteTableEntry(
            devNum, portGroupId,
            PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E,
            udbEntryIndex, udbEntryArray);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*                         (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_PCL_OFFSET_TYPE_ENT        *offsetTypePtr,
    OUT GT_U8                                *offsetPtr
)
{
    GT_STATUS rc;               /* return code              */
    GT_U32    regAddr;          /* register address         */
    GT_U32    regMask;          /* mask to update register  */
    GT_U32    regValue;         /* value to update register */
    GT_U32    startBit;         /* start Bit                */
    GT_U32    startWord;        /* start Word               */
    GT_U32    validTypesMask;   /* valid Types Mask         */
    GT_U32    comparatorBit;    /* comparator Bit           */
    GT_U32    udbEntryIndex;    /* XCat HW UDB entry index  */
    GT_U32    udbEntryArray[8]; /* XCat HW UDB entry        */
    GT_U32    portGroupId;      /*the port group Id         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(offsetTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(offsetPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    /* for all Cheetah devices */
    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        rc = prvCpssDxChCheetahPclUDBRegGet(
            devNum, ruleFormat, udbIndex,
            &regAddr, &startBit, &validTypesMask, &comparatorBit);
        if (rc != GT_OK)
        {
            return rc;
        }

        regMask  = (0x1FF << startBit);
        if (comparatorBit < 32)
        {
            regMask  |= (1 << comparatorBit) ;
        }

        rc = prvCpssDrvHwPpPortGroupReadRegBitMask(
            devNum, portGroupId, regAddr, regMask, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        *offsetPtr = (GT_U8)((regValue >> startBit) & 0x7F);

        if ((comparatorBit < 32) && ((regValue & (1 << comparatorBit)) != 0))
        {
            *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E;
        }
        else
        {
            switch ((regValue >> (startBit + 7)) & 3)
            {
                case UDB_HW_L2_CNS:
                    *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_L2_E;
                    break;
                case UDB_HW_L3_CNS:
                    *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_L3_E;
                    break;
                case UDB_HW_L4_CNS:
                    *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_L4_E;
                    break;
                case UDB_HW_IPV6EH_CNS:
                    *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E;
                    break;
                /* cannot occur */
                default: return GT_BAD_STATE;
            }
        }

        return GT_OK;
    }

    /* xCat devices */

    if (udbIndex >= 23)
    {
        return GT_BAD_PARAM;
    }

    /* Calculate entry index by packet type */
    rc = prvCpssDxChXCatPclUDBEntryIndexGet(
        devNum, packetType, &udbEntryIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* read all packet type related 23-UDB entry */
    rc = prvCpssDxChPortGroupReadTableEntry(
        devNum, portGroupId,
        PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E,
        udbEntryIndex, udbEntryArray);
    if (rc != GT_OK)
    {
        return rc;
    }

    startBit = 11 * udbIndex;

    startWord =  startBit >> 5; /* / 32 */
    startBit  &= 0x001F;        /* % 32 */

    regMask  = 0x07FF;
    regValue = (udbEntryArray[startWord] >> startBit) & regMask;

    if ((startBit + 11) > 32)
    {
        /* amount of already copied bits */
        startBit  = 32 - startBit;
        startWord ++;
        regValue &= (~ (regMask << startBit));
        regValue |= ((udbEntryArray[startWord] << startBit) & regMask);
    }

    *offsetPtr = (GT_U8)((regValue >> 3) & 0x7F);

    if ((regValue & 1) == 0)
    {
        /* invalid UDB */
        *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_INVALID_E;
    }
    else if ((regValue & (1 << 10)) != 0)
    {
        /* TCP/UDB comparator */
        *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E;
    }
    else
    {
        switch ((regValue >> 1) & 3)
         {
             case UDB_XCAT_HW_L2_CNS:
                 *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_L2_E;
                 break;
             case UDB_XCAT_HW_L3_CNS:
                 *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E;
                 break;
             case UDB_XCAT_HW_L4_CNS:
                 *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_L4_E;
                 break;
             case UDB_XCAT_HW_MPLS_CNS:
                 *offsetTypePtr = CPSS_DXCH_PCL_OFFSET_MPLS_MINUS_2_E;
                 break;
             /* cannot occur */
             default: return GT_BAD_STATE;
         }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupOverrideUserDefinedBytesSet
*
* DESCRIPTION:
*   The function sets overriding of  User Defined Bytes
*   by predefined optional key fields.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       udbOverridePtr - (pointer to) UDB override structure
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
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesSet
(
    IN  GT_U8                          devNum,
    IN  GT_PORT_GROUPS_BMP             portGroupsBmp,
    IN  CPSS_DXCH_PCL_OVERRIDE_UDB_STC *udbOverridePtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    portGroupId;   /*the port group Id                    */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    hwMask;        /* HW Value  mask                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(udbOverridePtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclUDBConfig;

    hwMask = 0x0300FFFF;
    hwValue =
        BOOL2BIT_MAC(udbOverridePtr->vrfIdLsbEnableStdNotIp)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdMsbEnableStdNotIp)   << 1)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdLsbEnableStdIpL2Qos) << 2)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdMsbEnableStdIpL2Qos) << 3)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdLsbEnableStdIpv4L4)  << 4)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdMsbEnableStdIpv4L4)  << 5)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdLsbEnableStdUdb)     << 6)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdMsbEnableStdUdb)     << 7)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdLsbEnableExtNotIpv6) << 8)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdMsbEnableExtNotIpv6) << 9)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdLsbEnableExtIpv6L2)  << 10)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdMsbEnableExtIpv6L2)  << 11)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdLsbEnableExtIpv6L4)  << 12)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdMsbEnableExtIpv6L4)  << 13)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdLsbEnableExtUdb)     << 14)
        | (BOOL2BIT_MAC(udbOverridePtr->vrfIdMsbEnableExtUdb)     << 15)
        | (BOOL2BIT_MAC(udbOverridePtr->qosProfileEnableStdUdb)   << 24)
        | (BOOL2BIT_MAC(udbOverridePtr->qosProfileEnableExtUdb)   << 25);

    if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum)
        && ((hwValue & 0xFFFF) != 0))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc =  prvCpssDrvHwPpPortGroupWriteRegBitMask(
            devNum, portGroupId, regAddr, hwMask, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupOverrideUserDefinedBytesGet
*
* DESCRIPTION:
*   The function gets overriding of  User Defined Bytes
*   by predefined optional key fields.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
* OUTPUTS:
*       udbOverridePtr - (pointer to) UDB override structure
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
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesGet
(
    IN  GT_U8                          devNum,
    IN  GT_PORT_GROUPS_BMP             portGroupsBmp,
    OUT CPSS_DXCH_PCL_OVERRIDE_UDB_STC *udbOverridePtr
)
{
    GT_U32    portGroupId;   /*the port group Id                    */
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(udbOverridePtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclUDBConfig;

    rc = prvCpssDrvHwPpPortGroupReadRegister(
        devNum, portGroupId, regAddr, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    udbOverridePtr->vrfIdLsbEnableStdNotIp    = BIT2BOOL_MAC(hwValue & 1);
    udbOverridePtr->vrfIdMsbEnableStdNotIp    = BIT2BOOL_MAC((hwValue >> 1) & 1);
    udbOverridePtr->vrfIdLsbEnableStdIpL2Qos  = BIT2BOOL_MAC((hwValue >> 2) & 1);
    udbOverridePtr->vrfIdMsbEnableStdIpL2Qos  = BIT2BOOL_MAC((hwValue >> 3) & 1);
    udbOverridePtr->vrfIdLsbEnableStdIpv4L4   = BIT2BOOL_MAC((hwValue >> 4) & 1);
    udbOverridePtr->vrfIdMsbEnableStdIpv4L4   = BIT2BOOL_MAC((hwValue >> 5) & 1);
    udbOverridePtr->vrfIdLsbEnableStdUdb      = BIT2BOOL_MAC((hwValue >> 6) & 1);
    udbOverridePtr->vrfIdMsbEnableStdUdb      = BIT2BOOL_MAC((hwValue >> 7) & 1);
    udbOverridePtr->vrfIdLsbEnableExtNotIpv6  = BIT2BOOL_MAC((hwValue >> 8) & 1);
    udbOverridePtr->vrfIdMsbEnableExtNotIpv6  = BIT2BOOL_MAC((hwValue >> 9) & 1);
    udbOverridePtr->vrfIdLsbEnableExtIpv6L2   = BIT2BOOL_MAC((hwValue >> 10) & 1);
    udbOverridePtr->vrfIdMsbEnableExtIpv6L2   = BIT2BOOL_MAC((hwValue >> 11) & 1);
    udbOverridePtr->vrfIdLsbEnableExtIpv6L4   = BIT2BOOL_MAC((hwValue >> 12) & 1);
    udbOverridePtr->vrfIdMsbEnableExtIpv6L4   = BIT2BOOL_MAC((hwValue >> 13) & 1);
    udbOverridePtr->vrfIdLsbEnableExtUdb      = BIT2BOOL_MAC((hwValue >> 14) & 1);
    udbOverridePtr->vrfIdMsbEnableExtUdb      = BIT2BOOL_MAC((hwValue >> 15) & 1);
    udbOverridePtr->qosProfileEnableStdUdb    = BIT2BOOL_MAC((hwValue >> 24) & 1);
    udbOverridePtr->qosProfileEnableExtUdb    = BIT2BOOL_MAC((hwValue >> 25) & 1);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet
*
* DESCRIPTION:
*   The function sets overriding of  User Defined Bytes
*   by packets Trunk Hash value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum             - device number
*       portGroupsBmp      - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
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
*       The Trunk Hash field cannot be included in the key of the
*       first IPCL lookup (Lookup0_0).
*       Thus, when the configuration for Trunk Hash in the Policy keys
*       is enabled, it applies only to the second and third lookups.
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet
(
    IN  GT_U8                                     devNum,
    IN  GT_PORT_GROUPS_BMP                        portGroupsBmp,
    IN  CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    portGroupId;   /*the port group Id                    */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */
    GT_U32    hwMask;        /* HW Value  mask                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(udbOverTrunkHashPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclEngineExtUdbConfig;

    hwMask = 0x000000FD;
    hwValue =
        BOOL2BIT_MAC(udbOverTrunkHashPtr->trunkHashEnableStdNotIp)
        | (BOOL2BIT_MAC(udbOverTrunkHashPtr->trunkHashEnableStdIpv4L4)  << 2)
        | (BOOL2BIT_MAC(udbOverTrunkHashPtr->trunkHashEnableStdUdb)     << 3)
        | (BOOL2BIT_MAC(udbOverTrunkHashPtr->trunkHashEnableExtNotIpv6) << 4)
        | (BOOL2BIT_MAC(udbOverTrunkHashPtr->trunkHashEnableExtIpv6L2)  << 5)
        | (BOOL2BIT_MAC(udbOverTrunkHashPtr->trunkHashEnableExtIpv6L4)  << 6)
        | (BOOL2BIT_MAC(udbOverTrunkHashPtr->trunkHashEnableExtUdb)     << 7);

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc =  prvCpssDrvHwPpPortGroupWriteRegBitMask(
            devNum, portGroupId, regAddr, hwMask, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet
*
* DESCRIPTION:
*   The function gets overriding of  User Defined Bytes
*   by packets Trunk Hash value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum             - device number
*       portGroupsBmp      - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
* OUTPUTS:
*       udbOverTrunkHashPtr - (pointer to) UDB override trunk hash structure
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
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet
(
    IN  GT_U8                                     devNum,
    IN  GT_PORT_GROUPS_BMP                        portGroupsBmp,
    OUT CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
)
{
    GT_U32    portGroupId;   /*the port group Id                    */
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(udbOverTrunkHashPtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclEngineExtUdbConfig;

    rc = prvCpssDrvHwPpPortGroupReadRegister(
        devNum, portGroupId, regAddr, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    udbOverTrunkHashPtr->trunkHashEnableStdNotIp    =
        BIT2BOOL_MAC(hwValue & 1);
    udbOverTrunkHashPtr->trunkHashEnableStdIpv4L4   =
        BIT2BOOL_MAC((hwValue >> 2) & 1);
    udbOverTrunkHashPtr->trunkHashEnableStdUdb      =
        BIT2BOOL_MAC((hwValue >> 3) & 1);
    udbOverTrunkHashPtr->trunkHashEnableExtNotIpv6  =
        BIT2BOOL_MAC((hwValue >> 4) & 1);
    udbOverTrunkHashPtr->trunkHashEnableExtIpv6L2   =
        BIT2BOOL_MAC((hwValue >> 5) & 1);
    udbOverTrunkHashPtr->trunkHashEnableExtIpv6L4   =
        BIT2BOOL_MAC((hwValue >> 6) & 1);
    udbOverTrunkHashPtr->trunkHashEnableExtUdb      =
        BIT2BOOL_MAC((hwValue >> 7) & 1);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet
*
* DESCRIPTION:
*       Sets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
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
GT_STATUS cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet
(
    IN  GT_U8                                      devNum,
    IN  GT_PORT_GROUPS_BMP                         portGroupsBmp,
    IN  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    portGroupId;   /*the port group Id                    */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;

    switch (vidUpMode)
    {
        case CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E:
            hwValue = 0;
            break;
        case CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E:
            hwValue = 1;
            break;
        default: return GT_BAD_PARAM;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, portGroupId, regAddr, 21, 1, hwValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet
*
* DESCRIPTION:
*       Gets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum             - device number
*       portGroupsBmp      - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
* OUTPUTS:
*       vidUpModePtr       - (pointer to) VID and UP key fields
*                            calculation mode
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet
(
    IN   GT_U8                                      devNum,
    IN   GT_PORT_GROUPS_BMP                         portGroupsBmp,
    OUT  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   *vidUpModePtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    portGroupId;   /*the port group Id                    */
    GT_U32    regAddr;       /* register address                    */
    GT_U32    hwValue;       /* HW Value                            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(vidUpModePtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, portGroupId, regAddr, 21, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *vidUpModePtr =
        (hwValue == 0)
           ? CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E
           : CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclLookupCfgPortListEnableRegAddrAndPosGet
*
* DESCRIPTION:
*        The function gets register addtress and bit position
*        for enable bit of using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*
* OUTPUTS:
*       regAddrPtr     - (pointer to) register address.
*       bitPosPtr      - (pointer to) bit position.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclLookupCfgPortListEnableRegAddrAndPosGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_U32                        *regAddrPtr,
    OUT GT_U32                        *bitPosPtr
)
{
    GT_STATUS rc; /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
        CPSS_XCAT_E | CPSS_LION_E);

    rc = prvCpssDxChPclLopokupParamCheck(
        devNum, direction, lookupNum, subLookupNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (direction == CPSS_PCL_DIRECTION_INGRESS_E)
    {
        /* ingress PCL */
        *regAddrPtr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclEngineConfig;
        switch (lookupNum)
        {
            case CPSS_PCL_LOOKUP_0_E:
                *bitPosPtr = ((subLookupNum == 0) ? 14 : 15);
                break;
            case CPSS_PCL_LOOKUP_1_E:
                *bitPosPtr = 16;
                break;
            case CPSS_PCL_LOOKUP_0_0_E:
                *bitPosPtr = 14;
                break;
            case CPSS_PCL_LOOKUP_0_1_E:
                *bitPosPtr = 15;
                break;
            default: return GT_BAD_PARAM;
        }
    }
    else
    {
        /* egress PCL */
        *regAddrPtr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;
        *bitPosPtr = 27;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupLookupCfgPortListEnableSet
*
* DESCRIPTION:
*        The function enables/disables using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
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
GT_STATUS cpssDxChPclPortGroupLookupCfgPortListEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
)
{
    GT_STATUS    rc;            /* returned code               */
    GT_U32       data;          /* register data               */
    GT_U32       regAddr;       /* register address            */
    GT_U32       bitPos;        /* bit position                */
    GT_U32       portGroupId;   /*the port group Id            */

    rc = prvCpssDxChPclLookupCfgPortListEnableRegAddrAndPosGet(
        devNum, direction, lookupNum, subLookupNum,
        &regAddr, &bitPos);
    if (rc != GT_OK)
    {
        return rc;
    }

    data = (enable == GT_FALSE) ? 0 : 1;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, portGroupId, regAddr, bitPos, 1, data);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclPortGroupLookupCfgPortListEnableGet
*
* DESCRIPTION:
*        The function gets enable/disable state of
*        using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*
* OUTPUTS:
*    enablePtr         - (pointer to)
*                        GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupLookupCfgPortListEnableGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_BOOL                       *enablePtr
)
{
    GT_STATUS    rc;            /* returned code               */
    GT_U32       data;          /* register data               */
    GT_U32       regAddr;       /* register address            */
    GT_U32       bitPos;        /* bit position                */
    GT_U32       portGroupId;   /*the port group Id            */

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    rc = prvCpssDxChPclLookupCfgPortListEnableRegAddrAndPosGet(
        devNum, direction, lookupNum, subLookupNum,
        &regAddr, &bitPos);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, portGroupId, regAddr, bitPos, 1, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = ((data != 0) ? GT_TRUE : GT_FALSE);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclL3L4ParsingOverMplsEnableSet
*
* DESCRIPTION:
*       If enabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as IP packets for key selection and UDB usage.
*       If disabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as MPLS packets for key selection and UDB usage.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - enable, GT_FALSE - disable
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclL3L4ParsingOverMplsEnableSet
(
    IN  GT_U8                              devNum,
    IN  GT_BOOL                            enable
)
{
    GT_U32       data;      /* register data               */
    GT_U32       regAddr;   /* register address            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    data = (enable == GT_FALSE) ? 0 : 1;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 23, 1, data);
}


/*******************************************************************************
* cpssDxChPclL3L4ParsingOverMplsEnableGet
*
* DESCRIPTION:
*       If enabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as IP packets for key selection and UDB usage.
*       If disabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as MPLS packets for key selection and UDB usage.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclL3L4ParsingOverMplsEnableGet
(
    IN  GT_U8                              devNum,
    OUT GT_BOOL                            *enablePtr
)
{
    GT_U32       data;      /* register data               */
    GT_STATUS    rc;        /* return code                 */
    GT_U32       regAddr;   /* register address            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 23, 1, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

