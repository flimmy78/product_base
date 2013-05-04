/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPclUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChPcl.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPcl.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Valid value for Rule format that is used in tests */
#define PCL_TESTED_RULE_FORMAT      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E

#define PCL_TCAM_RAWS(_dev) \
    PRV_CPSS_DXCH_PP_MAC(_dev)->fineTuning.tableSize.policyTcamRaws

/* Valid value for Rule index that is used in tests */
#define PCL_TESTED_RULE_INDEX       1023

/* Invalid value for Rule index for std rule format */
#define PCL_INVALID_STD_RULE_INDEX(_dev)  (PCL_TCAM_RAWS(_dev) * 4)
#define PCL_INVALID_EXT_RULE_INDEX(_dev)  (PCL_TCAM_RAWS(_dev) * 2)

/* Tests use this value as default valid value for port */
#define DXCH_PCL_VALID_PHY_PORT_CNS 0

/* Tests use this value as out-of-range value for vlanId */
#define PRV_CPSS_MAX_NUM_VLANS_CNS  4096

/* out of range trunk value */
#define MAX_TRUNK_ID_CNS            128

/* Macro to define is device of 98Dx2x3 type                    */
/* devType  - type of device (CPSS_PP_DEVICE_TYPE) */
#define IS_98DX2X3_DEV_MAC(dev)                 \
                ((CPSS_98DX243_CNS == PRV_CPSS_PP_MAC(dev)->devType) ||   \
                 (CPSS_98DX253_CNS == PRV_CPSS_PP_MAC(dev)->devType) ||   \
                 (CPSS_98DX263_CNS == PRV_CPSS_PP_MAC(dev)->devType) ||   \
                 (CPSS_98DX273_CNS == PRV_CPSS_PP_MAC(dev)->devType))


/*  Internal function. Is used for filling PCL action structure     */
/*  with default values which are used for most of all tests.       */
static void pclActionDefaultSet(IN CPSS_DXCH_PCL_ACTION_STC  *actionPtr);

/*  Internal function. Is used to set rule for testing.             */
/*  Set default values for structures and call cpssDxChPclRuleSet.  */
/*  Returns result of  cpssDxChPclRuleSet function.                 */
static GT_STATUS pclRuleTestingDefaultSet(IN GT_U8  dev);

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclUserDefinedByteSet)
{
/*
    ITERATE_DEVICES
    1.1. Call with ruleFormat
         [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E], udbIndex [0],
         offsetType [CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
    Expected: GT_OK for all type of devices.
    1.2. Call with ruleFormat
         [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E], udbIndex [1],
         offsetType [CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E], offset [5].
    Expected: GT_OK for ch/ch2/ch3 devices, GT_BAD_PARAM for others.
    1.3. Call with ruleFormat
         [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E], udbIndex [2],
         offsetType [CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E], offset [5].
    Expected: GT_OK for ch/ch2/ch3 devices, GT_BAD_PARAM for others.
    1.4. Call with ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E],
         udbIndex [5], offsetType [CPSS_DXCH_PCL_OFFSET_L3_E], offset [5]
         and wrong enum values packetType.
    Expected: GT_OK for 98DX2x3 devices, GT_BAD_PARAM for others.
    1.5. Call with ruleFormat
         [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E], udbIndex [2],
         offsetType [CPSS_DXCH_PCL_OFFSET_L4_E], offset [5].
    Expected: GT_OK for 98DX2x5 (Cheetah2) devices, NON GT_OK for others.
    1.6. Call with ruleFormat
         [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E], udbIndex [0],
         offsetType [CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E], offset [5].
    Expected: GT_OK for Cheetah2 and above devices, NOT GT_OK for ch1.
    1.7. Check for invalid udbIndex. Call with
         ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E],
         udbIndex [2 for all dev families and 23 for xCat and above],
         offsetType [CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
    Expected: NON GT_OK.
    1.8. Check for invalid udbIndex. Call with
         ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E],
         udbIndex [6], offsetType [CPSS_DXCH_PCL_OFFSET_L4_E], offset [5].
    Expected: NON GT_OK.
    1.9. Check for invalid offsetType. Call with
    ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
    udbIndex [0], offsetType [CPSS_DXCH_PCL_OFFSET_L4_E], offset [5].
    Expected: NON GT_OK.
    1.10. Check for invalid offsetType. Call with
    ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E],
    udbIndex [2], offsetType [CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
    Expected: NON GT_OK.
    1.11. Check out-of-range ruleFormat. Call with ruleFormat [wrong enum values],
          udbIndex [0], offsetType[CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
    Expected: GT_BAD_PARAM for ch, ch2, ch3 and GT_OK for xCat and above.
    1.12. Check out-of-range offsetType. Call with
          offsetType [wrong enum values], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.13. Check for big value for offset. Call with offset [0xFE] and other
          parameters the same as in 1.1.
    Expected: GT_OUT_OF_RANGE.
*/
    GT_STATUS                           st = GT_OK;

    GT_U8                               dev;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    CPSS_DXCH_PCL_PACKET_TYPE_ENT       packetType = CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E;
    GT_U32                              udbIndex;
    CPSS_DXCH_PCL_OFFSET_TYPE_ENT       offsetType;
    GT_U8                               offset;

    CPSS_DXCH_PCL_OFFSET_TYPE_ENT       offsetTypeGet;
    GT_U8                               offsetGet;
    CPSS_PP_FAMILY_TYPE_ENT             devFamily;
    CPSS_PP_DEVICE_TYPE                 devType;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        st = prvUtfDeviceTypeGet(dev, &devType);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceTypeGet: %d", dev);

        /*
            1.1. Call with ruleFormat
                 [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E], udbIndex [0],
                 offsetType [CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
            Expected: GT_OK for all type of devices.
        */
        offset     = 5;
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        packetType = CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E;
        udbIndex   = 0;
        offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;

        st = cpssDxChPclUserDefinedByteSet(dev, ruleFormat, packetType,
                                           udbIndex, offsetType, offset);

        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                     offsetType, offset);

        if(st == GT_OK)
        {
            /* verifying values */
            st = cpssDxChPclUserDefinedByteGet(dev, ruleFormat, packetType,
                                               udbIndex, &offsetTypeGet, &offsetGet);

            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex);

            UTF_VERIFY_EQUAL1_STRING_MAC(offsetType, offsetTypeGet,
                 "cpssDxChPclUserDefinedByteGet: "
                 "get another offsetType than was set: dev = %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(offset, offsetGet,
                 "cpssDxChPclUserDefinedByteGet: "
                 "get another offset than was set: dev = %d", dev);
        }

        /*
            1.2. Call with ruleFormat
                 [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E], udbIndex [1],
                 offsetType [CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E], offset [5].
            Expected: GT_OK for ch/ch2/ch3 devices, GT_BAD_PARAM for others.
        */
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
        udbIndex   = 1;
        offsetType = CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E;
        offset     = 5;

        st = cpssDxChPclUserDefinedByteSet(dev, ruleFormat, packetType,
                                           udbIndex, offsetType, offset);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleFormat, udbIndex,
                                        offsetType, offset);
        }
        else
        {
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                        offsetType, offset);
        }

        if(st == GT_OK)
        {
            /* verifying values */
            st = cpssDxChPclUserDefinedByteGet(dev, ruleFormat, packetType,
                                               udbIndex, &offsetTypeGet, &offsetGet);

            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex);

            UTF_VERIFY_EQUAL1_STRING_MAC(offsetType, offsetTypeGet,
                 "cpssDxChPclUserDefinedByteGet: "
                 "get another offsetType than was set: dev = %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(offset,     offsetGet,
                 "cpssDxChPclUserDefinedByteGet: "
                 "get another offset than was set: dev = %d", dev);
        }

        /*
            1.3. Call with ruleFormat
                 [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E], udbIndex [2],
                 offsetType [CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E], offset [5].
            Expected: GT_OK for ch/ch2/ch3 devices, GT_BAD_PARAM for others.
        */
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E;
        udbIndex   = 2;
        offsetType = CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E;
        offset     = 5;

        st = cpssDxChPclUserDefinedByteSet(dev, ruleFormat, packetType,
                                           udbIndex, offsetType, offset);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleFormat, udbIndex,
                                        offsetType, offset);
        }
        else
        {
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                        offsetType, offset);
        }

        if(st == GT_OK)
        {
            /* verifying values */
            st = cpssDxChPclUserDefinedByteGet(dev, ruleFormat, packetType,
                                               udbIndex, &offsetTypeGet, &offsetGet);

            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex);

            UTF_VERIFY_EQUAL1_STRING_MAC(offsetType, offsetTypeGet,
                 "cpssDxChPclUserDefinedByteGet: "
                 "get another offsetType than was set: dev = %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(offset,     offsetGet,
                 "cpssDxChPclUserDefinedByteGet: "
                 "get another offset than was set: dev = %d", dev);
        }

        /*
            1.4. Call with ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E],
                 udbIndex [5], offsetType [CPSS_DXCH_PCL_OFFSET_L3_E], offset [5]
                 and wrong enum values packetType.
            Expected: GT_OK for 98DX2x3 devices, GT_BAD_PARAM for others.
        */
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E;
        udbIndex   = 5;
        offsetType = CPSS_DXCH_PCL_OFFSET_L3_E;
        offset     = 5;

        if(IS_98DX2X3_DEV_MAC(dev))
        {
            for(enumsIndex = 0; enumsIndex < utfInvalidEnumArrSize; enumsIndex++)
            {
                packetType = utfInvalidEnumArr[enumsIndex];

                st = cpssDxChPclUserDefinedByteSet(dev, ruleFormat, packetType,
                                                   udbIndex, offsetType, offset);

                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                            offsetType, offset);
                if (GT_OK == st)
                {
                    /* verifying values */
                    st = cpssDxChPclUserDefinedByteGet(dev, ruleFormat, packetType,
                                                udbIndex, &offsetTypeGet, &offsetGet);
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex);

                    UTF_VERIFY_EQUAL1_STRING_MAC(offsetType, offsetTypeGet,
                         "cpssDxChPclUserDefinedByteGet: "
                         "get another offsetType than was set: dev = %d", dev);
                    UTF_VERIFY_EQUAL1_STRING_MAC(offset,     offsetGet,
                         "cpssDxChPclUserDefinedByteGet: "
                         "get another offset than was set: dev = %d", dev);
                }

                packetType = CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E;
            }
        }
        else
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChPclUserDefinedByteSet
                                (dev, ruleFormat, packetType, udbIndex, offsetType, offset),
                                packetType);
        }

        /*
            1.5. Call with ruleFormat
                 [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E], udbIndex [2],
                 offsetType [CPSS_DXCH_PCL_OFFSET_L4_E], offset [5].
            Expected: GT_OK for 98DX2x5 (Cheetah2) devices, NON GT_OK for others.
        */
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
        udbIndex   = 2;
        offsetType = CPSS_DXCH_PCL_OFFSET_L4_E;
        offset     = 5;

        st = cpssDxChPclUserDefinedByteSet(dev, ruleFormat, packetType,
                                           udbIndex, offsetType, offset);

        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                    offsetType, offset);
        if(st == GT_OK)
        {
            /* verifying values */
            st = cpssDxChPclUserDefinedByteGet(dev, ruleFormat, packetType,
                                               udbIndex, &offsetTypeGet, &offsetGet);

            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex);

            UTF_VERIFY_EQUAL1_STRING_MAC(offsetType, offsetTypeGet,
                 "cpssDxChPclUserDefinedByteGet: "
                 "get another offsetType than was set: dev = %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(offset,     offsetGet,
                 "cpssDxChPclUserDefinedByteGet: "
                 "get another offset than was set: dev = %d", dev);
        }
        /*
            1.6. Call with ruleFormat
                 [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E], udbIndex [0],
                 offsetType [CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E], offset [5].
            Expected: GT_OK for Cheetah2 and above devices, NOT GT_OK for ch1.
        */
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
        udbIndex   = 0;
        offsetType = CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E;
        offset     = 5; /* offset is ignored for TCP_UDP_COMPARATOR */

        st = cpssDxChPclUserDefinedByteSet(dev, ruleFormat, packetType,
                                           udbIndex, offsetType, offset);

        if(devFamily >= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                        offsetType, offset);

            /* verifying values */
            st = cpssDxChPclUserDefinedByteGet(dev, ruleFormat, packetType,
                                               udbIndex, &offsetTypeGet, &offsetGet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex);

            UTF_VERIFY_EQUAL1_STRING_MAC(offsetType, offsetTypeGet,
                 "cpssDxChPclUserDefinedByteGet: "
                 "get another offsetType than was set: dev = %d", dev);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                            offsetType, offset);
        }

        /*
            1.7. Check for invalid udbIndex. Call with
                 ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E],
                 udbIndex [2 for all dev families and 23 for xCat and above],
                 offsetType [CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
            Expected: NON GT_OK.
        */
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
        udbIndex   = 2;
        offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;
        offset     = 5;

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            udbIndex = 23;
        }

        st = cpssDxChPclUserDefinedByteSet(dev, ruleFormat, packetType,
                                           udbIndex, offsetType, offset);
        UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                         offsetType, offset);
        /*
            1.8. Check for invalid udbIndex. Call with
                 ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E],
                 udbIndex [6], offsetType [CPSS_DXCH_PCL_OFFSET_L4_E], offset [5].
            Expected: NON GT_OK.
        */
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;
        udbIndex   = 6;
        offsetType = CPSS_DXCH_PCL_OFFSET_L4_E;
        offset     = 5;

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            offsetType = CPSS_DXCH_PCL_OFFSET_L3_E;
        }

        st = cpssDxChPclUserDefinedByteSet(dev, ruleFormat, packetType,
                                   udbIndex, offsetType, offset);
        UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                        offsetType, offset);
        /*
            1.9. Check for invalid offsetType. Call with
            ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
            udbIndex [0], offsetType [CPSS_DXCH_PCL_OFFSET_L4_E], offset [5].
            Expected: NON GT_OK.
        */
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        udbIndex   = 0;
        offsetType = CPSS_DXCH_PCL_OFFSET_L4_E;
        offset     = 5;

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            offsetType = CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E;
        }

        st = cpssDxChPclUserDefinedByteSet(dev, ruleFormat, packetType,
                                           udbIndex, offsetType, offset);
        UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                        offsetType, offset);
        /*
            1.10. Check for invalid offsetType. Call with
            ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E],
            udbIndex [2], offsetType [CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
            Expected: NON GT_OK.
        */
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E;
        udbIndex   = 2;
        offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;
        offset     = 5;

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            /* to cause "BAD_PARAM", for XCat the key type ignored */
            offsetType = CPSS_DXCH_PCL_OFFSET_L3_E;
        }

        st = cpssDxChPclUserDefinedByteSet(dev, ruleFormat, packetType,
                                 udbIndex, offsetType, offset);
        UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                         offsetType, offset);
        /*
            1.11. Check out-of-range ruleFormat. Call with ruleFormat [wrong enum values],
                  udbIndex [0], offsetType[CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
            Expected: GT_BAD_PARAM for ch, ch2, ch3 and GT_OK for xCat and above.
        */
        packetType = CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E;
        udbIndex   = 0;
        offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;
        offset     = 5;

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            /* to cause "BAD_PARAM", for XCat the key type ignored */
            offsetType = CPSS_DXCH_PCL_OFFSET_L3_E;
        }

        UTF_ENUMS_CHECK_MAC(cpssDxChPclUserDefinedByteSet
                            (dev, ruleFormat, packetType, udbIndex, offsetType, offset),
                            ruleFormat);

        /*
            1.12. Check out-of-range offsetType. Call with
                  offsetType [wrong enum values], other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        udbIndex   = 0;
        offset     = 5;

        UTF_ENUMS_CHECK_MAC(cpssDxChPclUserDefinedByteSet
                            (dev, ruleFormat, packetType, udbIndex, offsetType, offset),
                            offsetType);

        /*
            1.13. Check for big value for offset. Call with offset [0xFE] and other
                  parameters the same as in 1.1.
            Expected: GT_OUT_OF_RANGE.
        */
        offset     = 0xFE;
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        udbIndex   = 0;
        offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;

        st = cpssDxChPclUserDefinedByteSet(dev, ruleFormat, packetType,
                                           udbIndex, offsetType, offset);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, ruleFormat, udbIndex,
                                    offsetType, offset);
    }

    /* set correct values for all the parameters (except of device number) */
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    udbIndex   = 0;
    offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;
    offset     = 5;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. Call function for non active device and valid parameters.*/
        /*  Expected: GT_BAD_PARAM.  */
        st = cpssDxChPclUserDefinedByteSet(dev, ruleFormat, packetType,
                                            udbIndex, offsetType, offset);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclUserDefinedByteSet(dev, ruleFormat, packetType,
                                        udbIndex, offsetType, offset);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclRuleSet
(
    IN GT_U8                              devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclRuleSet)
{
/*
    ITERATE_DEVICES
    1.1. Check standard rule for all active devices with legal parameters.
    Call with ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
    ruleIndex [PCL_TESTED_RULE_INDEX],
    maskPtr->ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ...],
    patternPtr-> ruleStdNotIp [ common[pclId=0;
                                        sourcePort=5;
                                        isTagged=1;
                                        vid=100;
                                        up=0;
                                        qosProfile=0;
                                        isIp=0;
                                        isL2Valid=1;
                                        isUdbValid =1],
                                isIpv4 = 0;
                                etherType=0;
                                isArp=0;
                                l2Encap=0;
                                macDa=AB:CD:EF:00:00:02;
                                macSa=AB:CD:EF:00:00:01;
                                udb[10,20,30] ],
    actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
                matchCounter { enableMatchCount = GT_FALSE,
                               matchCounterIndex = 0 },
                qos { egressPolicy=GT_FALSE,
                      modifyDscp=GT_FALSE,
                      modifyUp=GT_FALSE ,
                      qos [ ingress[profileIndex=0,
                            profileAssignIndex=GT_FALSE,
                            profilePrecedence=GT_FALSE] ] },
                redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                           data[routerLttIndex=0] },
                policer  { policerEnable=GT_FALSE,
                           policerId=0 },
                vlan { egressTaggedModify=GT_FALSE,
                       modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                       nestedVlan=GT_FALSE,
                       vlanId=100,
                       precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                ipUcRoute { doIpUcRoute=GT_FALSE,
                            0, GT_FALSE, GT_FALSE, GT_FALSE } ].

    Expected: GT_OK.
    1.2. Check for out of range ruleIndex. Other parameters is the same as in 1.1.
    Expected: NON GT_OK.
    1.3. Call with out of range actionPtr->matchCounter.matchCounterIndex [32]
       and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for other.
        1.4. For Ch3 and above call with out of range
        actionPtr->matchCounter.matchCounterIndex [16384] and other params from 1.1.
        Expected: NOT GT_OK.
    1.5. Call with out of range actionPtr->policer.policerId [policersTableSize],
    action->policer.policerEnable [CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E]
    and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for other.
    1.6. Call with actionPtr->redirect.redirectCmd
                        [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                   actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                   out of range actionPtr->redirect.outIf.tunnelPtr[1024]
                   and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3 and above.
        1.7. For Ch3 and above call with actionPtr->redirect.redirectCmd
             [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                       actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                       out of range actionPtr->redirect.outIf.tunnelPtr[8192]
                       and other params from 1.1.
        Expected: NOT GT_OK.
    1.8. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                   out of range actionPtr->redirect.data.routerLttIndex[8192]
    and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2/ch3.
        1.9. For Ch3 and above call with
                actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                       out of range actionPtr->redirect.data.routerLttIndex[32768]
                       and other params from 1.1.
        Expected: NOT GT_OK.
    1.10. For Ch3 and above call with actionPtr->redirect.redirectCmd
          [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E],
                   out of range actionPtr->redirect.data.vrfId [4096]
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.11. Call with actionPtr->vlan.modifyVlan [CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E],
                   out of range actionPtr->vlan.vlanId [4096]
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.12. For Ch3 and above call with out of range
    actionPtr->sourceId.sourceIdValue [32] and other params from 1.1.
    Expected: NOT GT_OK.
    1.13. Call with maskPtr [NULL], other parameters is the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.14. Call with patternPtr [NULL], other parameters is the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.15. Call with actionPtr [NULL], other parameters is the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.16. Call with ruleFormat [wrong enum values],
    other parameters is the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.17.  call cpssDxChPclRuleInvalidate
    with ruleSize [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX]
    to cleanup after testing.
    Expected: GT_OK.
    1.18. Check extended rule. For all active device ids call with
        ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E],
        ruleIndex [511],
        maskPtr->ruleExtNotIpv6.common is the same as in 1.1., other fileds are zeroed,
        patternPtr->ruleExtNotIpv6 [ common[the same as in 1.1],
        commonExt[ isIpv6=0, ipProtocol=1, dscp=0, isL4Valid=1, l4Byte0=0, l4Byte1=0,
                    l4Byte2=0, l4Byte3=0, l4Byte13=0, ipHeaderOk=1 ],
        sip={ 64.233.167.1},
        dip={ 213.180.204.1},
        etherType=0,
        l2Encap=1,
        macDa=AB:CD:EF:00:00:02,
        macSa=AB:CD:EF:00:00:01,
        ipv4Fragmented=1,
        udb[6]={10,20,30,40,50,60} ],
        actionPtr is the same as in 1.1.
    Expected: GT_OK.
    1.19. Check for out of range ruleIndex for extended rule format.
    Call with wrong ruleIndex, other parameters is the same as 1.8.
    Expected: NON GT_OK.
    1.20. Check for out of range ruleIndex for standard rule format.
    Call with wrong ruleIndex, other parameters is the same as 1.8.
    Expected: NON GT_OK.
    1.21.  call cpssDxChPclRuleInvalidate
    with ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], ruleIndex [511]
    to cleanup after testing.
    Expected: GT_OK.
    1.22. Check standard EGRESS rule for all active devices with legal parameters.
    Call with ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E],
    ruleIndex [PCL_TESTED_RULE_INDEX],
    maskPtr->ruleEgrStdNotIp [0xFF, ..., 0xFF],
    patternPtr->ruleEgrStdNotIp
            [ common[pclId=0; sourcePort=5; isTagged=1; vid=100; up=0;
              isIp=0; isL2Valid =1;  egrPacketType=0; cpuCode=1;srcTrg=1;
              srcDev = 10; sourceId = 1; isVidx=0; others -zeroid],
    isIpv4 = 0;
    etherType=0;
    isArp=0;
    l2Encap=0;
    macDa=AB:CD:EF:00:00:02;
    macSa=AB:CD:EF:00:00:01],
    actionPtr - the same as in 1.1.
    Expected: GT_OK for all except ch1.
        1.13. For all devices except ch1 call cpssDxChPclRuleInvalidate with
        ruleSize [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX]
        to cleanup after testing.
        Expected: GT_OK.
    1.23. Check extended EGRESS rule for all active devices with legal parameters.
    Call with ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E],
    ruleIndex [511],
    maskPtr-> ruleEgrExtIpv6L2 [0xFF, ..., 0xFF],
    patternPtr-> ruleEgrExtIpv6L2 [common [pclId=0; sourcePort=5;
        isTagged=1; vid=100; up=0; isIp=1; isL2Valid =1;  egrPacketType=0;
        cpuCode=1;srcTrg=1; srcDev = 10; sourceId = 1; isVidx=0; others -zeroid],
        commonExt [isIpv6=1; ipProtocol=1; dscp=2; isL4Valid=0;
                    egrTcpUdpPortComparator=0; others- theroid]
        sip[100; 1000; 10000; 100000];
        dipBits127to120=0xAA;
        macDa=AB:CD:EF:00:00:02;
        macSa=AB:CD:EF:00:00:01],
        actionPtr - the same as in 1.1.
    Expected: GT_OK for all except ch1.
        1.15. For all devices except ch1 call cpssDxChPclRuleInvalidate
        with ruleSize [CPSS_PCL_RULE_SIZE_EXT_E] to cleanup after testing.
        Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;
    GT_U32          policersTableSize = 0;

    GT_U8                               dev;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleIndex;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       mask;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       pattern;
    CPSS_DXCH_PCL_ACTION_STC            action;
    GT_ETHERADDR                        mac1 = {{0xAB, 0xCD, 0xEF, 0x00, 0x00, 0x01}};
    GT_ETHERADDR                        mac2 = {{0xAB, 0xCD, 0xEF, 0x00, 0x00, 0x02}};

    CPSS_PP_FAMILY_TYPE_ENT             devFamily;
    CPSS_PCL_RULE_SIZE_ENT              ruleSize;

    pclActionDefaultSet(&action);

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,"prvUtfDeviceFamilyGet: %d", dev);

        /* get table sizes for current device */
        st = cpssDxChCfgTableNumEntriesGet(dev,
                    CPSS_DXCH_CFG_TABLE_POLICER_METERS_E, &policersTableSize);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChCfgTableNumEntriesGet: %d", dev);

        /*
            1.1. Check standard rule for all active devices with legal parameters.
            Call with ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
            ruleIndex [PCL_TESTED_RULE_INDEX],
            maskPtr->ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ...],
            patternPtr-> ruleStdNotIp [ common[pclId=0;
                                                sourcePort=5;
                                                isTagged=1;
                                                vid=100;
                                                up=0;
                                                qosProfile=0;
                                                isIp=0;
                                                isL2Valid=1;
                                                isUdbValid =1],
                                        isIpv4 = 0;
                                        etherType=0;
                                        isArp=0;
                                        l2Encap=0;
                                        macDa=AB:CD:EF:00:00:02;
                                        macSa=AB:CD:EF:00:00:01;
                                        udb[10,20,30] ],
            actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                        mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
                        matchCounter { enableMatchCount = GT_FALSE,
                                       matchCounterIndex = 0 },
                        qos { egressPolicy=GT_FALSE,
                              modifyDscp=GT_FALSE,
                              modifyUp=GT_FALSE ,
                              qos [ ingress[profileIndex=0,
                                    profileAssignIndex=GT_FALSE,
                                    profilePrecedence=GT_FALSE] ] },
                        redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                                   data[routerLttIndex=0] },
                        policer  { policerEnable=GT_FALSE,
                                   policerId=0 },
                        vlan { egressTaggedModify=GT_FALSE,
                               modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                               nestedVlan=GT_FALSE,
                               vlanId=100,
                               precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                        ipUcRoute { doIpUcRoute=GT_FALSE,
                                    0, GT_FALSE, GT_FALSE, GT_FALSE } ].

            Expected: GT_OK.
        */
        ruleFormat = PCL_TESTED_RULE_FORMAT;
        ruleIndex = PCL_TESTED_RULE_INDEX;

        cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
        mask.ruleStdNotIp.common.pclId = 0xFFFF;

        cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
        pattern.ruleStdNotIp.common.pclId = 0;
        pattern.ruleStdNotIp.common.sourcePort = 5;
        pattern.ruleStdNotIp.common.isTagged = 1;
        pattern.ruleStdNotIp.common.vid = 100;
        pattern.ruleStdNotIp.common.up = 0;
        pattern.ruleStdNotIp.common.qosProfile = 0;
        pattern.ruleStdNotIp.common.isIp = 0;
        pattern.ruleStdNotIp.common.isL2Valid = 1;
        pattern.ruleStdNotIp.common.isUdbValid = 1;
        pattern.ruleStdNotIp.isIpv4 = 0;
        pattern.ruleStdNotIp.etherType = 0;
        pattern.ruleStdNotIp.isArp = 0;
        pattern.ruleStdNotIp.l2Encap = 0;
        pattern.ruleStdNotIp.macDa = mac2;
        pattern.ruleStdNotIp.macSa = mac1;
        pattern.ruleStdNotIp.udb[0] = 10;
        pattern.ruleStdNotIp.udb[1] = 20;
        pattern.ruleStdNotIp.udb[2] = 30;

        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

        /*
            1.2. Check for out of range ruleIndex. Other parameters is the same as in 1.1.
            Expected: NON GT_OK.
        */
        ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex,  0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

        ruleIndex = PCL_TESTED_RULE_INDEX; /* restore valid value */

        /*
            1.3. Call with out of range actionPtr->matchCounter.matchCounterIndex [32]
               and other params from 1.1.
            Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for other.
        */
        action.matchCounter.enableMatchCount = GT_TRUE;
        action.matchCounter.matchCounterIndex = 32;

        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex,  0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);

        if(devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->matchCounter.matchCounterIndex = %d",
                    dev, ruleFormat, ruleIndex, action.matchCounter.matchCounterIndex);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->matchCounter.matchCounterIndex = %d",
                    dev, ruleFormat, ruleIndex, action.matchCounter.matchCounterIndex);

            /*
                1.4. For Ch3 and above call with out of range
                actionPtr->matchCounter.matchCounterIndex [16384] and other params from 1.1.
                Expected: NOT GT_OK.
            */
            action.matchCounter.matchCounterIndex = 16384;

            st = cpssDxChPclRuleSet(
                dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->matchCounter.matchCounterIndex = %d",
                    dev, ruleFormat, ruleIndex, action.matchCounter.matchCounterIndex);
        }

        action.matchCounter.matchCounterIndex = 0; /* restore valid value */

        /*
            1.5. Call with out of range actionPtr->policer.policerId [policersTableSize],
            action->policer.policerEnable [CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E]
            and other params from 1.1.
            Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for other.
        */
        action.policer.policerEnable = CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E;
        /* out of HW field range - real policersTableSize not checked */
        action.policer.policerId = BIT_12;

        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                            "%d, %d, %d, actionPtr->policer.policerId = %d",
                                dev, ruleFormat, ruleIndex, action.policer.policerId);

        action.policer.policerId = 0; /* restore valid value */

        /*
            1.6. Call with actionPtr->redirect.redirectCmd
                                [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                           actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                           out of range actionPtr->redirect.outIf.tunnelPtr[1024]
                           and other params from 1.1.
            Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3 and above.
        */
        action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
        action.redirect.data.outIf.tunnelStart = GT_TRUE;
        action.redirect.data.outIf.tunnelPtr = 1024;

        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, &pattern,&action);

        if (devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.outIf.tunnelPtr = %d",
                    dev, ruleFormat, ruleIndex, action.redirect.data.outIf.tunnelPtr);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.outIf.tunnelPtr = %d",
                    dev, ruleFormat, ruleIndex, action.redirect.data.outIf.tunnelPtr);

            /*
                1.7. For Ch3 and above call with actionPtr->redirect.redirectCmd
                     [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                               actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                               out of range actionPtr->redirect.outIf.tunnelPtr[8192]
                               and other params from 1.1.
                Expected: NOT GT_OK.
            */
            action.redirect.data.outIf.tunnelPtr = 8192;

            st = cpssDxChPclRuleSet(
                dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern,&action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->redirect.data.outIf.tunnelPtr = %d",
                        dev, ruleFormat, ruleIndex, action.redirect.data.outIf.tunnelPtr);
        }

        action.redirect.data.outIf.tunnelPtr = 0; /* restore valid value */

        /*
            1.8. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                           out of range actionPtr->redirect.data.routerLttIndex[8192]
            and other params from 1.1.
            Expected: NOT GT_OK for Ch1/Ch2/ch3.
        */
        action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E;
        action.redirect.data.routerLttIndex = 8192;
        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);

        if ((devFamily <= CPSS_PP_FAMILY_CHEETAH3_E)
            || (devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->redirect.data.routerLttIndex = %d",
                        dev, ruleFormat, ruleIndex, action.redirect.data.routerLttIndex);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->redirect.data.routerLttIndex = %d",
                        dev, ruleFormat, ruleIndex, action.redirect.data.routerLttIndex);

            /*
                1.9. For Ch3 and above call with
                        actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                               out of range actionPtr->redirect.data.routerLttIndex[32768]
                               and other params from 1.1.
                Expected: NOT GT_OK.
            */
            action.redirect.data.routerLttIndex = 32768;

            st = cpssDxChPclRuleSet(
                dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.routerLttIndex = %d",
                    dev, ruleFormat, ruleIndex, action.redirect.data.routerLttIndex);
        }

        action.redirect.data.routerLttIndex = 0; /* restore valid value */
        if (devFamily == CPSS_PP_FAMILY_CHEETAH_E)
        {
            action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
            action.redirect.data.outIf.outInterface.type = CPSS_INTERFACE_PORT_E;
            action.redirect.data.outIf.outInterface.devPort.devNum = dev;
            action.redirect.data.outIf.outInterface.devPort.portNum = 0;
            action.redirect.data.outIf.tunnelStart = GT_FALSE;
            action.redirect.data.outIf.vntL2Echo = GT_FALSE;
        }

        /*
            1.10. For Ch3 and above call with actionPtr->redirect.redirectCmd
                  [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E],
                           out of range actionPtr->redirect.data.vrfId [4096]
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        if ((devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            && (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E;
            action.redirect.data.vrfId = 4096;

            st = cpssDxChPclRuleSet(
                dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                            "%d, %d, %d, actionPtr->redirect.data.vrfId= %d",
                            dev, ruleFormat, ruleIndex, action.redirect.data.vrfId);

            action.redirect.data.vrfId = 0; /* restore valid value */
        }

        if (devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E;
        }

        /*
            1.11. Call with actionPtr->vlan.modifyVlan [CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E],
                           out of range actionPtr->vlan.vlanId [4096]
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        action.vlan.modifyVlan = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;
        action.vlan.vlanId = 4096;

        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, actionPtr->vlan.vlanId = %d",
                                         dev, ruleFormat, ruleIndex, action.vlan.vlanId);

        action.vlan.vlanId = 100; /* restore valid value */

        /*
            1.12. For Ch3 and above call with out of range
            actionPtr->sourceId.sourceIdValue [32] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
        {
            action.sourceId.assignSourceId = GT_TRUE;
            action.sourceId.sourceIdValue = 32;

            st = cpssDxChPclRuleSet(
                dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                "%d, %d, %d, actionPtr->sourceId.sourceIdValue = %d",
                                dev, ruleFormat, ruleIndex, action.sourceId.sourceIdValue);

            action.sourceId.sourceIdValue = 0; /* restore valid value */
        }

        /*
            1.13. Call with maskPtr [NULL], other parameters is the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            NULL, &pattern, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, maskPtr = NULL",
                                     dev, ruleFormat, ruleIndex);

        /*
            1.14. Call with patternPtr [NULL], other parameters is the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, NULL, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, patternPtr = NULL",
                                     dev, ruleFormat, ruleIndex);

        /*
            1.15. Call with actionPtr [NULL], other parameters is the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, &pattern, NULL);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, actionPtr = NULL",
                                     dev, ruleFormat, ruleIndex);

        /*
            1.16. Call with ruleFormat [wrong enum values],
            other parameters is the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclRuleSet
                            (dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                             &mask, &pattern, &action),
                            ruleFormat);

        /*
            1.17.  call cpssDxChPclRuleInvalidate
            with ruleSize [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX]
            to cleanup after testing.
            Expected: GT_OK.
        */

        if(devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            ruleSize  = CPSS_PCL_RULE_SIZE_STD_E;
            ruleIndex = PCL_TESTED_RULE_INDEX;

            st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                "cpssDxChPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
        }

        /*
            1.18. Check extended rule. For all active device ids call with
                ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E],
                ruleIndex [511],
                maskPtr->ruleExtNotIpv6.common is the same as in 1.1., other fileds are zeroed,
                patternPtr->ruleExtNotIpv6 [ common[the same as in 1.1],
                commonExt[ isIpv6=0, ipProtocol=1, dscp=0, isL4Valid=1, l4Byte0=0, l4Byte1=0,
                            l4Byte2=0, l4Byte3=0, l4Byte13=0, ipHeaderOk=1 ],
                sip={ 64.233.167.1},
                dip={ 213.180.204.1},
                etherType=0,
                l2Encap=1,
                macDa=AB:CD:EF:00:00:02,
                macSa=AB:CD:EF:00:00:01,
                ipv4Fragmented=1,
                udb[6]={10,20,30,40,50,60} ],
                actionPtr is the same as in 1.1.
            Expected: GT_OK.
        */
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
        ruleIndex = 511;

        cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
        mask.ruleExtNotIpv6.common.pclId = 0xFFFF;

        /* cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern)); */ /*don't clean ruleStdNotIp*/
        pattern.ruleExtNotIpv6.common.pclId = 0;
        pattern.ruleExtNotIpv6.common.sourcePort = 5;
        pattern.ruleExtNotIpv6.common.isTagged = 1;
        pattern.ruleExtNotIpv6.common.vid = 100;
        pattern.ruleExtNotIpv6.common.up = 0;
        pattern.ruleExtNotIpv6.common.qosProfile = 0;
        pattern.ruleExtNotIpv6.common.isIp = 0;
        pattern.ruleExtNotIpv6.common.isL2Valid = 1;
        pattern.ruleExtNotIpv6.common.isUdbValid = 1;

        pattern.ruleExtNotIpv6.commonExt.isIpv6 = 0;
        pattern.ruleExtNotIpv6.commonExt.ipProtocol = 1;
        pattern.ruleExtNotIpv6.commonExt.dscp = 0;
        pattern.ruleExtNotIpv6.commonExt.isL4Valid = 1;
        pattern.ruleExtNotIpv6.commonExt.l4Byte0 = 0;
        pattern.ruleExtNotIpv6.commonExt.l4Byte1 = 0;
        pattern.ruleExtNotIpv6.commonExt.l4Byte2 = 0;
        pattern.ruleExtNotIpv6.commonExt.l4Byte3 = 0;
        pattern.ruleExtNotIpv6.commonExt.l4Byte13 = 0;
        pattern.ruleExtNotIpv6.commonExt.ipHeaderOk = 1;

        pattern.ruleExtNotIpv6.sip.arIP[0]= 64;
        pattern.ruleExtNotIpv6.sip.arIP[1]= 233;
        pattern.ruleExtNotIpv6.sip.arIP[2]= 167;
        pattern.ruleExtNotIpv6.sip.arIP[3]= 1;

        pattern.ruleExtNotIpv6.dip.arIP[0]= 213;
        pattern.ruleExtNotIpv6.dip.arIP[1]= 180;
        pattern.ruleExtNotIpv6.dip.arIP[2]= 204;
        pattern.ruleExtNotIpv6.dip.arIP[3]= 1;

        pattern.ruleExtNotIpv6.etherType = 0;
        pattern.ruleExtNotIpv6.l2Encap = 1;
        pattern.ruleExtNotIpv6.macDa = mac2;
        pattern.ruleExtNotIpv6.macSa = mac1;
        pattern.ruleExtNotIpv6.ipv4Fragmented = 1;

        pattern.ruleExtNotIpv6.udb[0] = 10;
        pattern.ruleExtNotIpv6.udb[1] = 20;
        pattern.ruleExtNotIpv6.udb[2] = 30;
        pattern.ruleExtNotIpv6.udb[3] = 40;
        pattern.ruleExtNotIpv6.udb[4] = 50;
        pattern.ruleExtNotIpv6.udb[5] = 60;

        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);


        /*
            1.19. Check for out of range ruleIndex for extended rule format.
            Call with wrong ruleIndex, other parameters is the same as 1.8.
            Expected: NON GT_OK.
        */
        ruleIndex = PCL_INVALID_EXT_RULE_INDEX(dev);

        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

        ruleIndex = 511; /* restore valid value */

        /*
            1.20. Check for out of range ruleIndex for standard rule format.
            Call with wrong ruleIndex, other parameters is the same as 1.8.
            Expected: NON GT_OK.
        */
        ruleFormat = PCL_TESTED_RULE_FORMAT;
        ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

        cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
        mask.ruleStdNotIp.common.pclId = 0xFFFF;

        /* pattern.ruleStdNotIp was set previously and wasn't cleaned */

        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

        /* restore valid values */
        ruleIndex = 511;
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;

        /*
            1.21.  call cpssDxChPclRuleInvalidate
            with ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], ruleIndex [511]
            to cleanup after testing.
            Expected: GT_OK.
        */
        if(devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;

            st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
        }

        /*
            1.22. Check standard EGRESS rule for all active devices with legal parameters.
            Call with ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E],
            ruleIndex [PCL_TESTED_RULE_INDEX],
            maskPtr->ruleEgrStdNotIp [0xFF, ..., 0xFF],
            patternPtr->ruleEgrStdNotIp
                    [ common[pclId=0; sourcePort=5; isTagged=1; vid=100; up=0;
                      isIp=0; isL2Valid =1;  egrPacketType=0; cpuCode=1;srcTrg=1;
                      srcDev = 10; sourceId = 1; isVidx=0; others -zeroid],
            isIpv4 = 0;
            etherType=0;
            isArp=0;
            l2Encap=0;
            macDa=AB:CD:EF:00:00:02;
            macSa=AB:CD:EF:00:00:01],
            actionPtr - the same as in 1.1.
            Expected: GT_OK for all except ch1.
        */
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E;
        ruleIndex = PCL_TESTED_RULE_INDEX;

        cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
        cpssOsMemSet((GT_VOID*)&(mask.ruleEgrStdNotIp), 0xFF, sizeof(mask.ruleEgrStdNotIp));

        cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
        pattern.ruleEgrStdNotIp.common.pclId = 0;
        pattern.ruleEgrStdNotIp.common.sourcePort = 5;
        pattern.ruleEgrStdNotIp.common.isTagged = 1;
        pattern.ruleEgrStdNotIp.common.vid = 100;
        pattern.ruleEgrStdNotIp.common.up = 0;
        pattern.ruleEgrStdNotIp.common.isIp = 0;
        pattern.ruleEgrStdNotIp.common.isL2Valid = 1;
        pattern.ruleEgrStdNotIp.common.egrPacketType = 0;
        pattern.ruleEgrStdNotIp.common.egrPktType.toCpu.cpuCode = 1;
        pattern.ruleEgrStdNotIp.common.egrPktType.toCpu.srcTrg = 1;
        pattern.ruleEgrStdNotIp.common.srcDev = 10;
        pattern.ruleEgrStdNotIp.common.sourceId = 1;
        pattern.ruleEgrStdNotIp.common.isVidx = 0;
        pattern.ruleEgrStdNotIp.common.sourceId = 0;

        pattern.ruleEgrStdNotIp.isIpv4 = 0;
        pattern.ruleEgrStdNotIp.etherType = 0;
        pattern.ruleEgrStdNotIp.isArp = 0;
        pattern.ruleEgrStdNotIp.l2Encap = 0;
        pattern.ruleEgrStdNotIp.macDa = mac2;
        pattern.ruleEgrStdNotIp.macSa = mac1;

        if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
        {   /* supported on CHEETAH 3 */
            action.qos.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            action.qos.modifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        }

        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);
        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);
        }
        else
        {
            /* for CH1 result ignored EPCL not supported     */
            /* but EPCL rule formats translated to HW format */
        }
        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /*
                1.13. For all devices except ch1 call cpssDxChPclRuleInvalidate with
                ruleSize [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX]
                to cleanup after testing.
                Expected: GT_OK.
            */
            ruleSize = CPSS_PCL_RULE_SIZE_STD_E;

            st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                "cpssDxChPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
        }

        /*
            1.23. Check extended EGRESS rule for all active devices with legal parameters.
            Call with ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E],
            ruleIndex [511],
            maskPtr-> ruleEgrExtIpv6L2 [0xFF, ..., 0xFF],
            patternPtr-> ruleEgrExtIpv6L2 [common [pclId=0; sourcePort=5;
                isTagged=1; vid=100; up=0; isIp=1; isL2Valid =1;  egrPacketType=0;
                cpuCode=1;srcTrg=1; srcDev = 10; sourceId = 1; isVidx=0; others -zeroid],
                commonExt [isIpv6=1; ipProtocol=1; dscp=2; isL4Valid=0;
                            egrTcpUdpPortComparator=0; others- theroid]
                sip[100; 1000; 10000; 100000];
                dipBits127to120=0xAA;
                macDa=AB:CD:EF:00:00:02;
                macSa=AB:CD:EF:00:00:01],
                actionPtr - the same as in 1.1.
            Expected: GT_OK for all except ch1.
        */
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E;
        ruleIndex = 511;

        cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
        cpssOsMemSet((GT_VOID*)&(mask.ruleEgrExtIpv6L2), 0xFF, sizeof(mask.ruleEgrExtIpv6L2));

        cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
        pattern.ruleEgrExtIpv6L2.common.pclId = 0;
        pattern.ruleEgrExtIpv6L2.common.sourcePort = 5;
        pattern.ruleEgrExtIpv6L2.common.sourceId = 0;
        pattern.ruleEgrExtIpv6L2.common.isTagged = 1;
        pattern.ruleEgrExtIpv6L2.common.vid = 100;
        pattern.ruleEgrExtIpv6L2.common.up = 0;
        pattern.ruleEgrExtIpv6L2.common.isIp = 1;
        pattern.ruleEgrExtIpv6L2.common.egrPacketType = 0;
        pattern.ruleEgrExtIpv6L2.common.egrPktType.toCpu.cpuCode = 1;
        pattern.ruleEgrExtIpv6L2.common.egrPktType.toCpu.srcTrg = 1;
        pattern.ruleEgrExtIpv6L2.common.srcDev = 10;
        pattern.ruleEgrExtIpv6L2.common.isVidx = 0;

        pattern.ruleEgrExtIpv6L2.commonExt.isIpv6 = 1;
        pattern.ruleEgrExtIpv6L2.commonExt.ipProtocol = 1;
        pattern.ruleEgrExtIpv6L2.commonExt.dscp = 2;
        pattern.ruleEgrExtIpv6L2.commonExt.isL4Valid = 0;
        pattern.ruleEgrExtIpv6L2.commonExt.egrTcpUdpPortComparator = 0;

        pattern.ruleEgrExtIpv6L2.dipBits127to120 = 0xAA;
        pattern.ruleEgrExtIpv6L2.sip.u32Ip[0] = 100;
        pattern.ruleEgrExtIpv6L2.sip.u32Ip[1] = 1000;
        pattern.ruleEgrExtIpv6L2.sip.u32Ip[2] = 10000;
        pattern.ruleEgrExtIpv6L2.sip.u32Ip[3] = 100000;
        pattern.ruleEgrExtIpv6L2.macDa = mac2;
        pattern.ruleEgrExtIpv6L2.macSa = mac1;

        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);
        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);
        }
        else
        {
            /* for CH1 result ignored EPCL not supported     */
            /* but EPCL rule formats translated to HW format */
        }

        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /*
                1.15. For all devices except ch1 call cpssDxChPclRuleInvalidate
                with ruleSize [CPSS_PCL_RULE_SIZE_EXT_E] to cleanup after testing.
                Expected: GT_OK.
            */
            ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;

            st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                "cpssDxChPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
        }
    }

    /* Set valid parameters. As soon as family of device can't be determined */
    /* - values valid for any family                                         */
    ruleFormat = PCL_TESTED_RULE_FORMAT;
    ruleIndex = 0;
    cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
    cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclRuleSet(
            dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclRuleSet(
        dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
        &mask, &pattern, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);
*/
UTF_TEST_CASE_MAC(cpssDxChPclRuleActionUpdate)
{
/*
    ITERATE_DEVICES
    1.1. Set rule for testing.  Call cpssDxChPclRuleSet with
        ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
        ruleIndex [PCL_TESTED_RULE_INDEX],
        maskPtr-> ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ...],
        patternPtr-> ruleStdNotIp [ common[pclId=0; sourcePort=5; isTagged=1;
            vid=100; up=0; qosProfile=0; isIp=0; isL2Valid=1; isUdbValid =1],
            isIpv4 = 0; etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
            macSa=AB:CD:EF:00:00:01; udb[10,20,30] ],
        actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
            mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
            matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
            qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
                qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                profilePrecedence=GT_FALSE] ] },
            redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                data[routerLttIndex=0] },
            policer { policerEnable=GT_FALSE, policerId=0 },
            vlan { egressTaggedModify=GT_FALSE,
                modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
            ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
    Expected: GT_OK.
    1.2. For all devices update action with legal parameters.
    Call with ruleSize [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX],
    actionPtr is the same as in 1.1.
    Expected: GT_OK.
    1.3. Call cpssDxChPclRuleActionGet with ruleSize[CPSS_PCL_RULE_SIZE_STD_E],
    ruleIndex [PCL_TESTED_RULE_INDEX], direction [CPSS_PCL_DIRECTION_INGRESS_E],
    non-NULL actionPtr.
    Expected: GT_OK and same values in actionPtr as were written.
    1.4. Check with rule size for other type of rule.
    Call with ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], ruleIndex [PCL_TESTED_RULE_INDEX],
    actionPtr is the same as in 1.1.
    Expected: NOT GT_OK.
    1.5. Check invalid rule size value.  Call with ruleSize [wrong enum values],
    ruleIndex [PCL_TESTED_RULE_INDEX], actionPtr is the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range ruleIndex and other parameters is the same as in 1.2.
    Expected: NON GT_OK.
    1.7. Call with out of range actionPtr->matchCounter.matchCounterIndex [32]
       and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3.
    1.8. For Ch3 and above call with
    out of range actionPtr->matchCounter.matchCounterIndex [16384]
    and other params from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with out of range actionPtr->policer.policerId [policersTableSize]
    and other params from 1.1.
    Expected: NOT GT_OK
    1.11. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                   actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                   out of range actionPtr->redirect.outIf.tunnelPtr[1024]
                   and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3 and above.
    1.12. For Ch3 and above call with actionPtr->redirect.redirectCmd
                [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
           actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
           out of range actionPtr->redirect.outIf.tunnelPtr[8192]
           and other params from 1.1.
    Expected: NOT GT_OK.
    1.13. Call with actionPtr->redirect.redirectCmd
                        [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                   out of range actionPtr->redirect.data.routerLttIndex[8192]
                   and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2/Ch3 and GT_OK for xCat and above.
    1.14. For xCat and above call with actionPtr->redirect.redirectCmd
                        [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                   out of range actionPtr->redirect.data.routerLttIndex[32768]
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.15. For Ch3 and above call with actionPtr->redirect.redirectCmd
                        [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E],
                   out of range actionPtr->redirect.data.vrfId [4096]
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.16. Call with actionPtr->vlan.modifyVlan
                        [CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E],
                   out of range actionPtr->vlan.vlanId [4096]
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.17. For Ch3 and above call with out of range
    actionPtr->sourceId.sourceIdValue [32] and other params from 1.1.
    Expected: NOT GT_OK.
    1.18. Check for NULL pointer. Call with actionPtr [NULL],
    other parameters same as in 1.2.
    Expected: GT_BAD_PTR.
    1.19. Call cpssDxChPclRuleInvalidate with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
    ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
    Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;
    GT_U32          policersTableSize = 0;

    GT_U8                               dev;
    CPSS_PCL_RULE_SIZE_ENT              ruleSize;
    GT_U32                              ruleIndex;
    CPSS_DXCH_PCL_ACTION_STC            action;

    CPSS_PCL_DIRECTION_ENT              direction;
    CPSS_DXCH_PCL_ACTION_STC            retAction;
    GT_BOOL                             failureWas;
    CPSS_PP_FAMILY_TYPE_ENT             devFamily;

    pclActionDefaultSet(&action);
    direction = CPSS_PCL_DIRECTION_INGRESS_E;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* get table sizes for current device */
        st = cpssDxChCfgTableNumEntriesGet(dev,
                    CPSS_DXCH_CFG_TABLE_POLICER_METERS_E, &policersTableSize);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChCfgTableNumEntriesGet: %d", dev);

        /*
            1.1. Set rule for testing.  Call cpssDxChPclRuleSet with
            ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
            ruleIndex [PCL_TESTED_RULE_INDEX],
            maskPtr-> ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ...],
            patternPtr-> ruleStdNotIp [ common[pclId=0; sourcePort=5; isTagged=1;
                vid=100; up=0; qosProfile=0; isIp=0; isL2Valid=1; isUdbValid =1],
                isIpv4 = 0; etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
                macSa=AB:CD:EF:00:00:01; udb[10,20,30] ],
            actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
                matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
                qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
                    qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                    profilePrecedence=GT_FALSE] ] },
                redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                    data[routerLttIndex=0] },
                policer { policerEnable=GT_FALSE, policerId=0 },
                vlan { egressTaggedModify=GT_FALSE,
                    modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                    vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
            Expected: GT_OK.
        */
        st = pclRuleTestingDefaultSet(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPclRuleSet: %d", dev);

        /*
            1.2. For all devices update action with legal parameters.
            Call with ruleSize [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX],
            actionPtr is the same as in 1.1.
            Expected: GT_OK.
        */
        ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
        ruleIndex = PCL_TESTED_RULE_INDEX;

        st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

        /*
            1.3. Call cpssDxChPclRuleActionGet with ruleSize[CPSS_PCL_RULE_SIZE_STD_E],
            ruleIndex [PCL_TESTED_RULE_INDEX], direction [CPSS_PCL_DIRECTION_INGRESS_E],
            non-NULL actionPtr.
            Expected: GT_OK and same values in actionPtr as were written.
        */
        st = cpssDxChPclRuleActionGet(dev, ruleSize, ruleIndex, direction, &retAction);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChPclRuleActionGet: %d, %d, %d, %d",
                                     dev, ruleSize, ruleIndex, direction);
        if (GT_OK == st)
        {
            /* verifying values */
            /*pktCmd*/
            UTF_VERIFY_EQUAL1_STRING_MAC(action.pktCmd, retAction.pktCmd,
                 "cpssDxChPclRuleActionGet: "
                 "get another action.pktCmd than was set: dev = %d", dev);

            if (devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(action.actionStop, retAction.actionStop,
                 "cpssDxChPclRuleActionGet: "
                 "get another action.actionStop than was set: dev = %d", dev);
            }

            /*mirror*/
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action.mirror,
                                            (const GT_VOID*)&retAction.mirror,
                                            sizeof(action.mirror))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                "cpssDxChPclRuleActionGet: "
                "get another action.mirror than was set: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);

            /*matchCounter*/
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action.matchCounter,
                                            (const GT_VOID*)&retAction.matchCounter,
                                            sizeof(action.matchCounter))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                "cpssDxChPclRuleActionGet: "
                "get another action.matchCounter than was set: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);

            /*redirect*/
            if (devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.redirectCmd,
                                             retAction.redirect.redirectCmd,
                     "cpssDxChPclRuleActionGet: "
                     "get another actionPtr->redirect.redirectCmd than was set: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);
                UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.data.outIf.tunnelPtr,
                                             retAction.redirect.data.outIf.tunnelPtr,
                     "cpssDxChPclRuleActionGet: "
                     "get another actionPtr->redirect.data.outIf.tunnelPtr than was set: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);
                UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.data.outIf.tunnelStart,
                                             retAction.redirect.data.outIf.tunnelStart,
                     "cpssDxChPclRuleActionGet: "
                     "get another actionPtr->redirect.data.outIf.tunnelStart than was set: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);
                UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.data.outIf.tunnelType,
                                             retAction.redirect.data.outIf.tunnelType,
                     "cpssDxChPclRuleActionGet: "
                     "get another actionPtr->redirect.data.outIf.tunnelType than was set: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);
                UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.data.outIf.vntL2Echo,
                                             retAction.redirect.data.outIf.vntL2Echo,
                     "cpssDxChPclRuleActionGet: "
                     "get another actionPtr->redirect.data.outIf.vntL2Echo than was set: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);
            }

            /*policer*/
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action.policer,
                                            (const GT_VOID*)&retAction.policer,
                                            sizeof(action.policer))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                "cpssDxChPclRuleActionGet: get another action.policer than was set: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);

            /*vlan*/
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action.vlan,
                                            (const GT_VOID*)&retAction.vlan,
                                            sizeof(action.vlan))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                "cpssDxChPclRuleActionGet: get another action.vlan than was set: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);

            /*ipUcRoute  - not relevant for Ch2 and above*/
            if (CPSS_PP_FAMILY_CHEETAH_E == devFamily)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action.ipUcRoute,
                                                (const GT_VOID*)&retAction.ipUcRoute,
                                                sizeof(action.ipUcRoute))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                    "cpssDxChPclRuleActionGet: get another action.ipUcRoute than was set: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);
            }
        }

        /*
            1.4. Check with rule size for other type of rule.
            Call with ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], ruleIndex [PCL_TESTED_RULE_INDEX],
            actionPtr is the same as in 1.1.
            Expected: NOT GT_OK.
        */
        ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;
        /* maximal index of standard rule, invalid for exteded rule */
        /* RuleActionUpdate uses ruleIndex only for action HW index */
        /* calculation                                              */
        ruleIndex = (PCL_TCAM_RAWS(dev) * 4) - 1;


        st = cpssDxChPclRuleActionUpdate(
            dev, ruleSize, ruleIndex, &action);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

        ruleSize = CPSS_PCL_RULE_SIZE_STD_E; /* restore valid values */
        ruleIndex = PCL_TCAM_RAWS(dev) - 1;

        /*
            1.5. Check invalid rule size value.  Call with ruleSize [wrong enum values],
            ruleIndex [PCL_TESTED_RULE_INDEX], actionPtr is the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclRuleActionUpdate
                            (dev, ruleSize, ruleIndex, &action),
                            ruleSize);

        /*
            1.6. Call with out of range ruleIndex and other parameters is the same as in 1.2.
            Expected: NON GT_OK.
        */
        ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

        st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

        ruleIndex = PCL_TESTED_RULE_INDEX; /* restore valid value */

        /*
            1.7. Call with out of range actionPtr->matchCounter.matchCounterIndex [32]
               and other params from 1.1.
            Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3.
        */
        action.matchCounter.enableMatchCount = GT_TRUE;
        action.matchCounter.matchCounterIndex = 32;

        st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);

        if(devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->matchCounter.matchCounterIndex = %d",
                    dev, ruleSize, ruleIndex, action.matchCounter.matchCounterIndex);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->matchCounter.matchCounterIndex = %d",
                    dev, ruleSize, ruleIndex, action.matchCounter.matchCounterIndex);

            /*
                1.8. For Ch3 and above call with
                out of range actionPtr->matchCounter.matchCounterIndex [16384]
                and other params from 1.1.
                Expected: NOT GT_OK.
            */
            action.matchCounter.matchCounterIndex = 16384;

            st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->matchCounter.matchCounterIndex = %d",
                    dev, ruleSize, ruleIndex, action.matchCounter.matchCounterIndex);
        }

        action.matchCounter.matchCounterIndex = 0; /* restore valid value */

        /*
            1.9. Call with out of range actionPtr->policer.policerId [policersTableSize]
            and other params from 1.1.
            Expected: NOT GT_OK
        */
        action.policer.policerEnable = CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E;
        /* out of HW field range - real policersTableSize not checked */
        action.policer.policerId     = BIT_12;

        st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);

        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->policer.policerId = %d",
                                dev, ruleSize, ruleIndex, action.policer.policerId);

        action.policer.policerId = 0; /* restore valid value */

        /*
            1.11. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                           actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                           out of range actionPtr->redirect.outIf.tunnelPtr[1024]
                           and other params from 1.1.
            Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3 and above.
        */
        action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
        action.redirect.data.outIf.tunnelStart = GT_TRUE;
        action.redirect.data.outIf.tunnelPtr = 1024;

        st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);

        if (devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.outIf.tunnelPtr = %d",
                    dev, ruleSize, ruleIndex, action.redirect.data.outIf.tunnelPtr);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.outIf.tunnelPtr = %d",
                    dev, ruleSize, ruleIndex, action.redirect.data.outIf.tunnelPtr);

            /*
                1.12. For Ch3 and above call with actionPtr->redirect.redirectCmd
                            [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                       actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                       out of range actionPtr->redirect.outIf.tunnelPtr[8192]
                       and other params from 1.1.
                Expected: NOT GT_OK.
            */
            action.redirect.data.outIf.tunnelPtr = 8192;

            st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.outIf.tunnelPtr = %d",
                    dev, ruleSize, ruleIndex, action.redirect.data.outIf.tunnelPtr);
        }

        action.redirect.data.outIf.tunnelPtr = 0; /* restore valid value */

        /*
            1.13. Call with actionPtr->redirect.redirectCmd
                                [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                           out of range actionPtr->redirect.data.routerLttIndex[8192]
                           and other params from 1.1.
            Expected: NOT GT_OK for Ch1/Ch2/Ch3 and GT_OK for xCat and above.
        */
        action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E;
        action.redirect.data.routerLttIndex = 8192;

        st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);

        if ((devFamily <= CPSS_PP_FAMILY_CHEETAH3_E)
            || (devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.routerLttIndex = %d",
                    dev, ruleSize, ruleIndex, action.redirect.data.routerLttIndex);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.routerLttIndex = %d",
                    dev, ruleSize, ruleIndex, action.redirect.data.routerLttIndex);

            /*
                1.14. For xCat and above call with actionPtr->redirect.redirectCmd
                                    [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                               out of range actionPtr->redirect.data.routerLttIndex[32768]
                               and other params from 1.1.
                Expected: NOT GT_OK.
            */
            action.redirect.data.routerLttIndex = 32768;

            st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.routerLttIndex = %d",
                    dev, ruleSize, ruleIndex, action.redirect.data.routerLttIndex);
        }

        action.redirect.data.routerLttIndex = 0; /* restore valid value */

        /*
            1.15. For Ch3 and above call with actionPtr->redirect.redirectCmd
                                [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E],
                           out of range actionPtr->redirect.data.vrfId [4096]
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        if ((devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            && (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E;
            action.redirect.data.vrfId = 4096;

            st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                            "%d, %d, %d, actionPtr->redirect.data.vrfId= %d",
                            dev, ruleSize, ruleIndex, action.redirect.data.vrfId);

            action.redirect.data.vrfId = 0; /* restore valid value */
        }

        if (devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E;
        }

        /*
            1.16. Call with actionPtr->vlan.modifyVlan
                                [CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E],
                           out of range actionPtr->vlan.vlanId [4096]
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        action.vlan.modifyVlan = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;
        action.vlan.vlanId = 4096;

        st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                       "%d, %d, %d, actionPtr->vlan.vlanId = %d",
                            dev, ruleSize, ruleIndex, action.vlan.vlanId);

        action.vlan.vlanId = 100; /* restore valid value */

        /*
            1.17. For Ch3 and above call with out of range
            actionPtr->sourceId.sourceIdValue [32] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        if (devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
        {
            action.sourceId.assignSourceId = GT_TRUE;
            action.sourceId.sourceIdValue = 32;

            st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                            "%d, %d, %d, actionPtr->sourceId.sourceIdValue = %d",
                            dev, ruleSize, ruleIndex, action.sourceId.sourceIdValue);

            action.sourceId.sourceIdValue = 0; /* restore valid value */
        }

        /*
            1.18. Check for NULL pointer. Call with actionPtr [NULL],
            other parameters same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, NULL);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL",
                                     dev, ruleSize, ruleIndex);

        /*
            1.19. Call cpssDxChPclRuleInvalidate with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
            ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
            Expected: GT_OK.
        */
        st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                "cpssDxChPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
    }

    /* Set valid parameters. As soon as family of device can't be determined */
    /* - values valid for any family                                         */
    ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
    ruleIndex = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    IN CPSS_PCL_DIRECTION_ENT              direction,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclRuleActionGet)
{
/*
    ITERATE_DEVICES
    1.1. Set default rule for testing.
    Expected: GT_OK.
    1.2. Check with legal parameters. Call with ruleSize[CPSS_PCL_RULE_SIZE_STD_E],
    ruleIndex [PCL_TESTED_RULE_INDEX], direction [CPSS_PCL_DIRECTION_INGRESS_E],
    non-NULL actionPtr.
    Expected: GT_OK
    1.3. Check with rule size for other type of rule.
    Call with ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], other params same as in 1.2.
    Expected: NOT GT_OK.
    1.4.  Call with ruleSize [wrong enum values], other params same as in 1.2.
    Expected: GT_BAD_PARAM.
    1.5.  Call with wrong ruleIndex, other parameters same as in 1.2.
    Expected: NON GT_OK.
    1.6.  Call with direction [CPSS_PCL_DIRECTION_EGRESS_E],
    other params same as in 1.2.
    Expected: GT_OK.
    1.7.  Call with direction [wrong enum values], other parameters same as in 1.2.
    Expected: GT_BAD_PARAM.
    1.8. Call with actionPtr [NULL], other parameters same as in 1.2.
    Expected: GT_BAD_PTR.
    1.9. Call cpssDxChPclRuleInvalidate with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
    ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
    Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                     dev;
    CPSS_PCL_RULE_SIZE_ENT    ruleSize;
    GT_U32                    ruleIndex;
    CPSS_PCL_DIRECTION_ENT    direction;
    CPSS_DXCH_PCL_ACTION_STC  retAction;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Set default rule for testing.
            Expected: GT_OK.
        */
        st = pclRuleTestingDefaultSet(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPclRuleSet: %d", dev);

        /*
            1.2. Check with legal parameters. Call with ruleSize[CPSS_PCL_RULE_SIZE_STD_E],
            ruleIndex [PCL_TESTED_RULE_INDEX], direction [CPSS_PCL_DIRECTION_INGRESS_E],
            non-NULL actionPtr.
            Expected: GT_OK
        */
        ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
        ruleIndex = PCL_TCAM_RAWS(dev) - 1;
        direction = CPSS_PCL_DIRECTION_INGRESS_E;

        st = cpssDxChPclRuleActionGet(dev, ruleSize, ruleIndex, direction, &retAction);
        if (st != GT_BAD_STATE)
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(
                GT_OK, st, dev, ruleSize, ruleIndex, direction);
        }

        /*
            1.3. Check with rule size for other type of rule.
            Call with ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], other params same as in 1.2.
            Expected: NOT GT_OK.
        */
        ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;
        /* maximal index of standard rule, invalid for exteded rule */
        /* RuleActionGet uses ruleIndex only for action HW index calculation */
        ruleIndex = (PCL_TCAM_RAWS(dev) * 4) - 1;

        st = cpssDxChPclRuleActionGet(
            dev, ruleSize, ruleIndex, direction, &retAction);
        if (st != GT_BAD_STATE)
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleSize);
        }

        ruleSize = CPSS_PCL_RULE_SIZE_STD_E; /* restore valid values */
        ruleIndex = PCL_TCAM_RAWS(dev) - 1;

        /*
            1.4.  Call with ruleSize [wrong enum values], other params same as in 1.2.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclRuleActionGet
                            (dev, ruleSize, ruleIndex, direction, &retAction),
                            ruleSize);

        /*
            1.5.  Call with wrong ruleIndex, other parameters same as in 1.2.
            Expected: NON GT_OK.
        */
        ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

        st = cpssDxChPclRuleActionGet(dev, ruleSize, ruleIndex, direction, &retAction);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

        ruleIndex = PCL_TESTED_RULE_INDEX; /* restore valid value */

        /*
            1.6.  Call with direction [CPSS_PCL_DIRECTION_EGRESS_E],
            other params same as in 1.2.
            Expected: GT_OK.
        */
        direction = CPSS_PCL_DIRECTION_EGRESS_E;

        st = cpssDxChPclRuleActionGet(dev, ruleSize, ruleIndex, direction, &retAction);
        if (st != GT_BAD_STATE)
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(
                GT_OK, st, dev, ruleSize, ruleIndex, direction);
        }

        direction = CPSS_PCL_DIRECTION_INGRESS_E; /* restore */

        /*
            1.7.  Call with direction [wrong enum values], other parameters same as in 1.2.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclRuleActionGet
                            (dev, ruleSize, ruleIndex, direction, &retAction),
                            direction);

        /*
            1.8. Call with actionPtr [NULL], other parameters same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclRuleActionGet(dev, ruleSize, ruleIndex, direction, NULL);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, %d, NULL",
                                     dev, ruleSize, ruleIndex, direction);

        /*
            1.9. Call cpssDxChPclRuleInvalidate with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
            ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
            Expected: GT_OK.
        */
        st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChPclRuleInvalidate: %d, %d, %d",
                                     dev, ruleSize, ruleIndex);
   }

    /* Set valid parameters. As soon as family of device can't be determined */
    /* - values valid for any family                                         */
    ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
    ruleIndex = 0;
    direction = CPSS_PCL_DIRECTION_INGRESS_E;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclRuleActionGet(dev, ruleSize, ruleIndex, direction, &retAction);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclRuleActionGet(dev, ruleSize, ruleIndex, direction, &retAction);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclRuleInvalidate
(
    IN GT_U8                              devNum,
    IN CPSS_DXCH_PCL_RULE_SIZE_ENT        ruleSize,
    IN GT_U32                             ruleIndex
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclRuleInvalidate)
{
/*
    ITERATE_DEVICES
    1.1. Set rule for testing.
    Expected: GT_OK.
    1.2. Check for invalid ruleSize for rule which was written as standard rule.
    Call with invalid ruleSize [CPSS_DXCH_PCL_RULE_SIZE_EXT_E],
    ruleIndex [PCL_TESTED_RULE_INDEX].
    Expected: NOT GT_OK.
    1.3. Invalidate written rule with legal parameters.  Call with
        ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E],
        ruleIndex [PCL_TESTED_RULE_INDEX].
    Expected: GT_OK.
    1.4. Call with ruleSize [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.5. Call with wrong ruleIndex.
    Expected: NON GT_OK.
*/
    GT_STATUS                       st = GT_OK;
    GT_U8                           dev;
    GT_U32                          ruleIndex;
    CPSS_PCL_RULE_SIZE_ENT          ruleSize;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Set rule for testing.
            Expected: GT_OK.
        */
        st = pclRuleTestingDefaultSet(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPclRuleSet: %d", dev);

        /*
            1.2. Check for invalid ruleSize for rule which was written as standard rule.
            Call with invalid ruleSize [CPSS_DXCH_PCL_RULE_SIZE_EXT_E],
            ruleIndex [PCL_TESTED_RULE_INDEX].
            Expected: NOT GT_OK.
        */

        /* The cpssDxChPclRuleValidStatusSet function ckecks the   */
        /* found rule state. The cpssDxChPclRuleInvalidate just    */
        /* sets the Y and Y "valid" bit in the position calculated */
        /* by ruleSize and ruleIndex                               */
        ruleSize      = CPSS_PCL_RULE_SIZE_EXT_E;
        /* index valid for STD but invalid for EXT rule  */
        ruleIndex = (PCL_TCAM_RAWS(dev) * 4) - 1;


        st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

        /*
            1.3. Invalidate written rule with legal parameters. Call with
                ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E],
                ruleIndex [PCL_TESTED_RULE_INDEX].
            Expected: GT_OK.
        */
        ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
        ruleIndex = PCL_TESTED_RULE_INDEX;

        st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

        /*
            1.4. Call with ruleSize [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclRuleInvalidate
                            (dev, ruleSize, ruleIndex),
                            ruleSize);

        /*
            1.5. Call with wrong ruleIndex.
            Expected: NON GT_OK.
        */
        ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
        ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

        st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);
    }

    ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
    ruleIndex = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclRuleValidStatusSet
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN GT_BOOL                            valid
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclRuleValidStatusSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Set rule for testing.
    Expected: GT_OK.
    1.2. Call with  ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                    ruleIndex [PCL_TESTED_RULE_INDEX ],
                    valid [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.3. Call with  ruleSize [CPSS_PCL_RULE_SIZE_EXT_E],
                    ruleIndex [PCL_TESTED_RULE_INDEX],
                    valid [GT_FALSE].
    Try to set status of created rule with another ruleSize.
    Expected: NOT GT_OK.
    1.4. Call with  ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                    ruleIndex [0],
                    valid [GT_FALSE].
    Try to set status of created rule with another ruleIndex.
    Expected: NOT GT_OK.
    1.5. Call with wrong enum values ruleSize, and other parameters from 1.2.
    Expected: NOT GT_OK.
    1.6. Call with out of range ruleIndex, and other parameters from 1.2.
    Expected: NOT GT_OK.
    1.7. Invalidate created rule.  Call cpssDxChPclRuleInvalidate with
        ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E],
        ruleIndex [PCL_TESTED_RULE_INDEX].
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PCL_RULE_SIZE_ENT  ruleSize    = CPSS_PCL_RULE_SIZE_STD_E;
    GT_U32                  ruleIndex   = 0;
    GT_BOOL                 valid       = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Set rule for testing.
            Expected: GT_OK.
        */
        st = pclRuleTestingDefaultSet(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPclRuleSet: %d", dev);

        /*
            1.2. Call with  ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                            ruleIndex [PCL_TESTED_RULE_INDEX],
                            valid [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */
        /* call with valid = GT_TRUE */
        ruleSize  = CPSS_PCL_RULE_SIZE_STD_E;
        ruleIndex = PCL_TESTED_RULE_INDEX;
        valid     = GT_TRUE;

        st = cpssDxChPclRuleValidStatusSet(dev, ruleSize, ruleIndex, valid);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex, valid);

        /* call with valid = GT_FALSE */
        valid = GT_FALSE;

        st = cpssDxChPclRuleValidStatusSet(dev, ruleSize, ruleIndex, valid);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex, valid);

        /*
            1.3. Call with  ruleSize [CPSS_PCL_RULE_SIZE_EXT_E],
                            ruleIndex [PCL_TESTED_RULE_INDEX],
                            valid [GT_FALSE].
            Try to set status of created rule with another ruleSize.
            Expected: NOT GT_OK.
        */
        /* call with ruleSize = CPSS_PCL_RULE_SIZE_EXT_E */
        ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;

        st = cpssDxChPclRuleValidStatusSet(dev, ruleSize, ruleIndex, valid);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleSize = %d", dev, ruleSize);

        ruleSize = CPSS_PCL_RULE_SIZE_STD_E;

        /*
            1.4. Call with  ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                            ruleIndex [0],
                            valid [GT_FALSE].
            Try to set status of created rule with another ruleIndex.
            Expected: NOT GT_OK.
        */
        ruleIndex = 0;

        st = cpssDxChPclRuleValidStatusSet(dev, ruleSize, ruleIndex, valid);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleIndex = %d", dev, ruleIndex);

        ruleIndex = PCL_TESTED_RULE_INDEX;

        /*
            1.5. Call with wrong enum values ruleSize, and other parameters from 1.2.
            Expected: NOT GT_OK.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclRuleValidStatusSet
                            (dev, ruleSize, ruleIndex, valid),
                            ruleSize);

        /*
            1.6. Call with out of range ruleIndex, and other parameters from 1.2.
            Expected: NOT GT_OK.
        */
        ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

        st = cpssDxChPclRuleValidStatusSet(dev, ruleSize, ruleIndex, valid);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleIndex = %d", dev, ruleIndex);

        ruleIndex = PCL_TESTED_RULE_INDEX;

        /*
            1.7. Invalidate created rule.  Call cpssDxChPclRuleInvalidate with
                ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E],
                ruleIndex [PCL_TESTED_RULE_INDEX].
            Expected: GT_OK.
        */
        st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);
    }

    ruleSize  = CPSS_PCL_RULE_SIZE_STD_E;
    ruleIndex = PCL_TESTED_RULE_INDEX;
    valid     = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclRuleValidStatusSet(dev, ruleSize, ruleIndex, valid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclRuleValidStatusSet(dev, ruleSize, ruleIndex, valid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclRuleCopy
(
    IN GT_U8                              devNum,
    IN CPSS_DXCH_PCL_RULE_SIZE_ENT        ruleSize,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclRuleCopy)
{
/*
    ITERATE_DEVICES
    1.1. Set rule for testing.
    Expected: GT_OK.
    1.2.  Call with correct parameters. RuleSize [CPSS_PCL_RULE_SIZE_EXT_E],
            ruleSrcIndex [PCL_TESTED_RULE_INDEX], ruleDstIndex [0].
    Expected: GT_OK.
    1.3. Try to copy with ruleSize which isn't applicable for standard rule.
        Call with ruleSize [CPSS_DXCH_PCL_RULE_SIZE_EXT_E],
        ruleSrcIndex [PCL_TESTED_RULE_INDEX], ruleDstIndex [0].
    Expected: NON GT_OK.
    1.4. Call with ruleSize [wrong enum values],
    ruleSrcIndex [PCL_TESTED_RULE_INDEX], ruleDstIndex [0].
    Expected: GT_BAD_PARAM.
    1.5. Call with wrong source rule index.
    Expected: NON GT_OK.
    1.6. Call with wrong destination rule index.
    Expected: NON GT_OK.
    1.7. Invalidate source rule. Call cpssDxChPclRuleInvalidate with
                ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E],
                ruleIndex [PCL_TESTED_RULE_INDEX].
    Expected: GT_OK.
    1.8. Invalidate source rule. Call cpssDxChPclRuleInvalidate with
                ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E],
                ruleIndex [0].
    Expected: GT_OK.
*/
    GT_STATUS                       st = GT_OK;
    GT_U8                           dev;
    GT_U32                          ruleSrcIndex;
    GT_U32                          ruleDstIndex;
    CPSS_PCL_RULE_SIZE_ENT          ruleSize;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Set rule for testing.
            Expected: GT_OK.
        */
        st = pclRuleTestingDefaultSet(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPclRuleSet: %d", dev);

        /*
            1.2.  Call with correct parameters. RuleSize [CPSS_PCL_RULE_SIZE_EXT_E],
                    ruleSrcIndex [PCL_TESTED_RULE_INDEX], ruleDstIndex [0].
            Expected: GT_OK.
        */
        ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
        ruleSrcIndex = PCL_TESTED_RULE_INDEX;
        ruleDstIndex = 0;

        st = cpssDxChPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleSrcIndex, ruleDstIndex);

        /*
            1.3. Try to copy with ruleSize which isn't applicable for standard rule.
                Call with ruleSize [CPSS_DXCH_PCL_RULE_SIZE_EXT_E],
                ruleSrcIndex [PCL_TESTED_RULE_INDEX], ruleDstIndex [0].
            Expected: NON GT_OK.
        */
        ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;

        st = cpssDxChPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize,
                                        ruleSrcIndex, ruleDstIndex);

        ruleSize = CPSS_PCL_RULE_SIZE_STD_E;

        /*
            1.4. Call with ruleSize [wrong enum values],
            ruleSrcIndex [PCL_TESTED_RULE_INDEX], ruleDstIndex [0].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclRuleCopy
                            (dev, ruleSize, ruleSrcIndex, ruleDstIndex),
                            ruleSize);

        /*
            1.5. Call with wrong source rule index.
            Expected: NON GT_OK.
        */
        ruleSrcIndex = PCL_INVALID_STD_RULE_INDEX(dev);

        st = cpssDxChPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleSrcIndex);

        ruleSrcIndex = PCL_TESTED_RULE_INDEX;

        /*
            1.6. Call with wrong destination rule index.
            Expected: NON GT_OK.
        */
        ruleDstIndex = PCL_INVALID_STD_RULE_INDEX(dev);

        st = cpssDxChPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize,
                                        ruleSrcIndex, ruleDstIndex);

        ruleDstIndex = 0;

        /*
            1.7. Invalidate source rule. Call cpssDxChPclRuleInvalidate with
                        ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E],
                        ruleIndex [PCL_TESTED_RULE_INDEX].
            Expected: GT_OK.
        */
        st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleSrcIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChPclRuleInvalidate: %d, %d, %d",
                           dev, ruleSize, ruleSrcIndex);

        /*
            1.8. Invalidate source rule. Call cpssDxChPclRuleInvalidate with
                        ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E],
                        ruleIndex [0].
            Expected: GT_OK.
        */
        st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleDstIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChPclRuleInvalidate: %d, %d, %d",
                           dev, ruleSize, ruleDstIndex);
    }

    ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
    ruleSrcIndex = PCL_TESTED_RULE_INDEX;
    ruleDstIndex = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclRuleStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclRuleStateGet)
{
/*
    ITERATE_DEVICES
    1.1. Set rule for testing.
    Expected: GT_OK.
    1.2. For all devices check with legal parameters.
    Call with ruleIndex [PCL_TESTED_RULE_INDEX], non-NULL validPtr and ruleSizePtr.
    Expected: GT_OK, validPtr = GT_TRUE, ruleSizePtr = CPSS_PCL_RULE_SIZE_STD_E;
    1.3. Call with wrong ruleIndex, other parameters same as in 1.2.
    Expected: NON GT_OK.
    1.4. Call with validPtr [NULL], other parameters same as in 1.2.
    Expected: GT_BAD_PTR.
    1.5. Call with ruleSizePtr [NULL], other parameters same as in 1.2.
    Expected: GT_BAD_PTR.
    1.6. Call cpssDxChPclRuleInvalidate with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
    ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
    Expected: GT_OK.
    1.7. Get state for invalidated rule. Call with ruleIndex [PCL_TESTED_RULE_INDEX],
    non-NULL validPtr and ruleSizePtr.
    Expected: GT_OK and valid == GT_FALSE.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    GT_U32                              ruleIndex;
    GT_BOOL                             valid;
    CPSS_PCL_RULE_SIZE_ENT              ruleSize;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Set rule for testing.
            Expected: GT_OK.
        */
        st = pclRuleTestingDefaultSet(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPclRuleSet: %d", dev);

        /*
            1.2. For all devices check with legal parameters.
            Call with ruleIndex [PCL_TESTED_RULE_INDEX], non-NULL validPtr and ruleSizePtr.
            Expected: GT_OK, validPtr = GT_TRUE, ruleSizePtr = CPSS_PCL_RULE_SIZE_STD_E;
        */
        ruleIndex = PCL_TESTED_RULE_INDEX;

        st = cpssDxChPclRuleStateGet(dev, ruleIndex, &valid, &ruleSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, valid,
                "get invalid rule: %d, %d", dev, ruleIndex);

        UTF_VERIFY_EQUAL2_STRING_MAC(CPSS_PCL_RULE_SIZE_STD_E, ruleSize,
                "get another rule size than was set: %d, %d", dev, ruleIndex);

        /*
            1.3. Call with wrong ruleIndex, other parameters same as in 1.2.
            Expected: NON GT_OK.
        */
        ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

        st = cpssDxChPclRuleStateGet(dev, ruleIndex, &valid, &ruleSize);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = PCL_TESTED_RULE_INDEX; /* restore valid value */

        /*
            1.4. Call with validPtr [NULL], other parameters same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclRuleStateGet(dev, ruleIndex, NULL, &ruleSize);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, validPtr = NULL",
                                     dev, ruleIndex);

        /*
            1.5. Call with ruleSizePtr [NULL], other parameters same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclRuleStateGet(dev, ruleIndex, &valid, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ruleSizePtr = NULL",
                                     dev, ruleIndex);

        /*
            1.6. Call cpssDxChPclRuleInvalidate with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
            ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
            Expected: GT_OK.
        */
        ruleSize = CPSS_PCL_RULE_SIZE_STD_E;

        st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "cpssDxChPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);

        /*
            1.7. Get state for invalidated rule. Call with ruleIndex [PCL_TESTED_RULE_INDEX],
            non-NULL validPtr and ruleSizePtr.
            Expected: GT_OK and valid == GT_FALSE.
        */
        st = cpssDxChPclRuleStateGet(dev, ruleIndex, &valid, &ruleSize);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChPclRuleStateGet: %d, %d", dev, ruleIndex);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, valid,
                "get invalid rule: %d, %d", dev, ruleIndex);

        UTF_VERIFY_EQUAL2_STRING_MAC(CPSS_PCL_RULE_SIZE_STD_E, ruleSize,
                "get another rule size than was set: %d, %d", dev, ruleIndex);

    }

    /* Set valid parameters. As soon as family of device can't be determined */
    /* - values valid for any family                                         */
    ruleIndex = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclRuleStateGet(dev, ruleIndex, &valid, &ruleSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclRuleStateGet(dev, ruleIndex, &valid, &ruleSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclRuleAnyStateGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclRuleAnyStateGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Set rule for testing.
    Expected: GT_OK.
    1.2. Call with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                   ruleIndex [PCL_TESTED_RULE_INDEX],
                   non-NULL validPtr and non-NULL ruleSizePtr.
    Expected: GT_OK.
    1.3. Call with out of range ruleIndex, and other parameters same as in 1.2.
    Expected: NON GT_OK.
    1.4. Call with validPtr [NULL], and other parameters same as in 1.2.
    Expected: GT_BAD_PTR.
    1.5. Call with ruleSizePtr [NULL], and other parameters same as in 1.2.
    Expected: GT_BAD_PTR.
    1.6. Call cpssDxChPclRuleInvalidate with
            ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
            ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
    Expected: GT_OK.
    1.7. Get state for invalidated rule.
         Call with ruleIndex [PCL_TESTED_RULE_INDEX],
                   non-NULL validPtr
                   and non-NULL ruleSizePtr.
    Expected: GT_OK, validPtr = GT_FALSE.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PCL_RULE_SIZE_ENT  ruleSize  = CPSS_PCL_RULE_SIZE_STD_E;
    GT_U32                  ruleIndex = 0;
    GT_BOOL                 valid     = GT_FALSE;

    CPSS_PP_FAMILY_TYPE_ENT devFamily;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,"prvUtfDeviceFamilyGet: %d", dev);

        /*
            1.1. Set rule for testing.
            Expected: GT_OK.
        */
        st = pclRuleTestingDefaultSet(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPclRuleSet: %d", dev);

        /*
            1.2. Call with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                           ruleIndex [PCL_TESTED_RULE_INDEX],
                           non-NULL validPtr and non-NULL ruleSizePtr.
            Expected: GT_OK.
        */
        ruleSize  = CPSS_PCL_RULE_SIZE_STD_E;
        ruleIndex = PCL_TESTED_RULE_INDEX;

        st = cpssDxChPclRuleAnyStateGet(dev, ruleSize, ruleIndex, &valid, &ruleSize);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

        /*
            1.3. Call with out of range ruleIndex, and other parameters same as in 1.2.
            Expected: NON GT_OK.
        */
        ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

        st = cpssDxChPclRuleAnyStateGet(dev, ruleSize, ruleIndex, &valid, &ruleSize);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = PCL_TESTED_RULE_INDEX; /* restore valid value */

        /*
            1.4. Call with validPtr [NULL], and other parameters same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclRuleAnyStateGet(dev, ruleSize, ruleIndex, NULL, &ruleSize);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, validPtr = NULL",
                                     dev, ruleIndex);

        /*
            1.5. Call with ruleSizePtr [NULL], and other parameters same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclRuleAnyStateGet(dev, ruleSize, ruleIndex, &valid, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ruleSizePtr = NULL",
                                     dev, ruleIndex);

        /*
            1.6. Call cpssDxChPclRuleInvalidate with
                    ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                    ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
            Expected: GT_OK.
        */
        ruleSize = CPSS_PCL_RULE_SIZE_STD_E;

        st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChPclRuleInvalidate: %d, %d, %d",
                                     dev, ruleSize, ruleIndex);

        /*
            1.7. Get state for invalidated rule.
                 Call with ruleIndex [PCL_TESTED_RULE_INDEX],
                           non-NULL validPtr
                           and non-NULL ruleSizePtr.
            Expected: GT_OK, validPtr = GT_FALSE.
        */
        ruleIndex = PCL_TESTED_RULE_INDEX;

        st = cpssDxChPclRuleAnyStateGet(dev, ruleSize, ruleIndex, &valid, &ruleSize);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, valid,
                "get invalid rule: %d, %d", dev, ruleIndex);

        UTF_VERIFY_EQUAL2_STRING_MAC(CPSS_PCL_RULE_SIZE_STD_E, ruleSize,
                "get another rule size than was set: %d, %d", dev, ruleIndex);

    }

    /* Set valid parameters. As soon as family of device can't be determined */
    /* - values valid for any family                                         */
    ruleSize  = CPSS_PCL_RULE_SIZE_STD_E;
    ruleIndex = PCL_TESTED_RULE_INDEX;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclRuleAnyStateGet(dev, ruleSize, ruleIndex, &valid, &ruleSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclRuleAnyStateGet(dev, ruleSize, ruleIndex, &valid, &ruleSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclRuleGet
(
    IN  GT_U8                  devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                 ruleIndex,
    OUT GT_U32                 mask[],
    OUT GT_U32                 pattern[],
    OUT GT_U32                 action[]
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclRuleGet)
{
/*
    ITERATE_DEVICES
    1.1. Set rule for testing.
    Expected: GT_OK.
    1.2. For all devices check with legal parameters.
        Call with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                  ruleIndex [PCL_TESTED_RULE_INDEX],
                  non-NULL mask, pattern and action.
    Expected: GT_OK.
    1.3. Check for invalid ruleSize for rule which was written as standard rule.
    Call with invalid ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], other params same as in 1.2.
    Expected: NON GT_OK.
    1.4. Check for wrong enum values ruleSize and other params same as in 1.2.
    Expected: GT_BAD_PARAM.
    1.5. Call with wrong ruleIndex and other parameters same as in 1.2.
    Expected: NON GT_OK.
    1.6. Call with mask [NULL], other parameters same as in 1.2.
    Expected: GT_BAD_PTR.
    1.7. Call with pattern [NULL], other parameters same as in 1.2.
    Expected: GT_BAD_PTR.
    1.8. Call with action [NULL], other parameters same as in 1.2.
    Expected: GT_BAD_PTR.
    1.9. Call cpssDxChPclRuleInvalidate with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
    ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
    Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    GT_U32                              ruleIndex;
    CPSS_PCL_RULE_SIZE_ENT              ruleSize;
    GT_U32                              mask[12];
    GT_U32                              pattern[12];
    GT_U32                              action[3];

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Set rule for testing.
            Expected: GT_OK.
        */
        st = pclRuleTestingDefaultSet(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPclRuleSet: %d", dev);

        /*
            1.2. For all devices check with legal parameters.
                Call with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                          ruleIndex [PCL_TESTED_RULE_INDEX],
                          non-NULL mask, pattern and action.
            Expected: GT_OK.
        */
        ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
        ruleIndex = PCL_TESTED_RULE_INDEX;

        st = cpssDxChPclRuleGet(dev, ruleSize, ruleIndex, mask, pattern, action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

        /*
            1.3. Check for invalid ruleSize for rule which was written as standard rule.
            Call with invalid ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], other params same as in 1.2.
            Expected: NON GT_OK.
        */
        ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;

        st = cpssDxChPclRuleGet(dev, ruleSize, ruleIndex, mask, pattern, action);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleSize);

        ruleSize = CPSS_PCL_RULE_SIZE_STD_E; /* restore valid value */

        /*
            1.4. Check for wrong enum values ruleSize and other params same as in 1.2.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclRuleGet
                            (dev, ruleSize, ruleIndex, mask, pattern, action),
                            ruleSize);

        /*
            1.5. Call with wrong ruleIndex and other parameters same as in 1.2.
            Expected: NON GT_OK.
        */
        ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

        st = cpssDxChPclRuleGet(dev, ruleSize, ruleIndex, mask, pattern, action);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

        ruleIndex = PCL_TESTED_RULE_INDEX; /* restore valid value */

        /*
            1.6. Call with mask [NULL], other parameters same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclRuleGet(dev, ruleSize, ruleIndex, NULL, pattern, action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mask = NULL", dev);

        /*
            1.7. Call with pattern [NULL], other parameters same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclRuleGet(dev, ruleSize, ruleIndex, mask, NULL, action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pattern = NULL", dev);

        /*
            1.8. Call with action [NULL], other parameters same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclRuleGet(dev, ruleSize, ruleIndex, mask, pattern, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, action = NULL", dev);

        /*
            1.9. Call cpssDxChPclRuleInvalidate with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
            ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
            Expected: GT_OK.
        */
        st = cpssDxChPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "cpssDxChPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
    }

    /* Set valid parameters. As soon as family of device can't be determined */
    /* - values valid for any family                                         */
    ruleIndex = 0;
    ruleSize = CPSS_PCL_RULE_SIZE_STD_E;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclRuleGet(dev, ruleSize, ruleIndex, mask, pattern, action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclRuleGet(dev, ruleSize, ruleIndex, mask, pattern, action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclRuleMatchCounterGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterIndex,
    OUT GT_U32                        *counterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclRuleMatchCounterGet)
{
/*
    ITERATE_DEVICES (DxCh1, DxCh2)
    1.1. Call with counterIndex [0/31], non-NULL counterPtr.
    Expected: GT_OK
    1.2.  Call with wrong counterIndex [32], non-NULL counterPtr.
    Expected: non GT_OK.
    1.3. Call with counterIndex [0], counterPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8    dev;
    GT_U32   counterIndex;
    GT_U32   counter;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with counterIndex [0/31], non-NULL counterPtr.
            Expected: GT_OK
        */
        /*call with counterIndex = 0;*/
        counterIndex = 0;

        st = cpssDxChPclRuleMatchCounterGet(dev, counterIndex, &counter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /*call with counterIndex = 31;*/
        counterIndex = 31;

        st = cpssDxChPclRuleMatchCounterGet(dev, counterIndex, &counter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /*
            1.2.  Call with wrong counterIndex [32], non-NULL counterPtr.
            Expected: non GT_OK.
        */
        counterIndex = 32;

        st = cpssDxChPclRuleMatchCounterGet(dev, counterIndex, &counter);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        counterIndex = 0;

        /*
            1.3. Call with counterIndex [0], counterPtr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChPclRuleMatchCounterGet(dev, counterIndex, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL",
                                     dev, counterIndex);
    }

    counterIndex = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclRuleMatchCounterGet(dev, counterIndex, &counter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclRuleMatchCounterGet(dev, counterIndex, &counter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclRuleMatchCounterSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterIndex,
    IN  GT_U32                        counterValue
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclRuleMatchCounterSet)
{
/*
    ITERATE_DEVICES (DxCh1, DxCh2)
    1.1. Call with counterIndex [0/31], counterValue [0].
    Expected: GT_OK
    1.2. Call cpssDxChPclRuleMatchCounterGet with counterIndex [0/31],
    non-NULL counterPtr.
    Expected: GT_OK and same value as written.
    1.3. Call with wrong counterIndex [32], counterValue[0].
    Expected: NOT GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8    dev;
    GT_U32   counterIndex;
    GT_U32   counter;

    GT_U32  retCounter;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with counterIndex [0/31], counterValue [0].
            Expected: GT_OK
        */
        counter = 0;
        counterIndex = 0;

        st = cpssDxChPclRuleMatchCounterSet(dev, counterIndex, counter);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, counterIndex, counter);

        /*
            1.2. Call cpssDxChPclRuleMatchCounterGet with counterIndex [0/31],
            non-NULL counterPtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxChPclRuleMatchCounterGet(dev, counterIndex, &retCounter);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
            "cpssDxChPclRuleMatchCounterGet: %d, %d", dev, counterIndex);

        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(counter, retCounter,
                "get another counter value than was set: %d, %d", dev, counterIndex);
        }

        /* 1.1. for counterIndex == 31 */
        counterIndex = 31;

        st = cpssDxChPclRuleMatchCounterSet(dev, counterIndex, counter);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, counterIndex, counter);

        /* 1.2. counterIndex == 31 */
        st = cpssDxChPclRuleMatchCounterGet(dev, counterIndex, &retCounter);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
            "cpssDxChPclRuleMatchCounterGet: %d, %d", dev, counterIndex);

        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(counter, retCounter,
                "get another counter value than was set: %d, %d", dev, counterIndex);
        }

        /*
            1.3. Call with wrong counterIndex [32], counterValue[0].
            Expected: NOT GT_OK.
        */
        counterIndex = 32;
        counter = 0;

        st = cpssDxChPclRuleMatchCounterSet(dev, counterIndex, counter);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        counterIndex = 0;
    }

    counterIndex = 0;
    counter = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclRuleMatchCounterSet(dev, counterIndex, counter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclRuleMatchCounterSet(dev, counterIndex, counter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortIngressPolicyEnable
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortIngressPolicyEnable)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS
    1.1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     enable;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            enable = GT_FALSE;

            st = cpssDxChPclPortIngressPolicyEnable(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            enable = GT_TRUE;

            st = cpssDxChPclPortIngressPolicyEnable(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
        }

        /* 1.2. Check that function handles CPU port    */
        port = PRV_CPSS_DXCH_CPU_PORT_NUM_CNS;
        enable = GT_TRUE;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */

        st = cpssDxChPclPortIngressPolicyEnable(dev, port, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxChPclPortIngressPolicyEnable(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPclPortIngressPolicyEnable(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;
    enable = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclPortIngressPolicyEnable(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortIngressPolicyEnable(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortIngressPolicyEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortIngressPolicyEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS
    1.1.1. Call with not null enablePtr.
    Expected: GT_OK.
    1.1.2. Call  with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     enable;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not null enablePtr.
                Expected: GT_OK.
            */
            st = cpssDxChPclPortIngressPolicyEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call  with wrong enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortIngressPolicyEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PTR, st, dev, port, enable);
        }

        /* 1.2. Check that function handles CPU port    */
        port = PRV_CPSS_DXCH_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */

        st = cpssDxChPclPortIngressPolicyEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxChPclPortIngressPolicyEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPclPortIngressPolicyEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclPortIngressPolicyEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortIngressPolicyEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortLookupCfgTabAccessModeSet
(
    IN GT_U8                                          devNum,
    IN GT_U8                                          port,
    IN CPSS_PCL_DIRECTION_ENT                         direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT                     lookupNum,
    IN GT_U32                                         subLookupNum,
    IN CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortLookupCfgTabAccessModeSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS
    1.1.1. Check Ingress direction. For all devices call with
            direction [CPSS_PCL_DIRECTION_INGRESS_E],
            lookupNum [CPSS_PCL_LOOKUP_0_E],
            mode [CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E /
                  CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E].
    Expected: GT_OK
    1.1.2. Check Egress direction (not supported by Ch1).
    Call with direction [CPSS_PCL_DIRECTION_EGRESS_E],
    Expected: GT_OK for all dev except Cheetah.
    1.1.3. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E].
    Expected: GT_OK.
    1.1.4. Check wrong direction enum values.
    Expected: GT_BAD_PARAM.
    1.1.5. Check wrong lookupNum enum values.
    Expected: GT_BAD_PARAM.
    1.1.6. Call with subLookupNum [1],
    Expected: GT_OK for xCat and above and NOT GT_OK for other.
    1.1.7. Call with wrong subLookupNum [2],
    Expected: NOT GT_OK for xCat and above and GT_OK for other.
    1.1.8. Check out-of-range mode enum.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                                           dev;
    GT_U8                                           port;
    CPSS_PCL_DIRECTION_ENT                          direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT                      lookupNum;
    GT_U32                                          subLookupNum = 0;
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT  mode;

    /* used to test supported only by Cheetah2 values */
    CPSS_PP_FAMILY_TYPE_ENT                 devFamily;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Check Ingress direction. For all devices call with
                        direction [CPSS_PCL_DIRECTION_INGRESS_E],
                        lookupNum [CPSS_PCL_LOOKUP_0_E],
                        mode [CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E /
                              CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E].
                Expected: GT_OK
            */
            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_0_E;
            subLookupNum = 0;

            /*call with mode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E;*/
            mode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E;

            st = cpssDxChPclPortLookupCfgTabAccessModeSet(dev, port,
                            direction, lookupNum, subLookupNum, mode);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, direction, lookupNum, mode);

            /*call with mode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E;*/
            mode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E;

            st = cpssDxChPclPortLookupCfgTabAccessModeSet(dev, port,
                            direction, lookupNum, subLookupNum, mode);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, direction, lookupNum, mode);

            /*
                1.1.2. Check Egress direction (not supported by Ch1).
                Call with direction [CPSS_PCL_DIRECTION_EGRESS_E],
                Expected: GT_OK for all dev except Cheetah.
            */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;

            st = cpssDxChPclPortLookupCfgTabAccessModeSet(dev, port,
                            direction, lookupNum, subLookupNum, mode);
            if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port,
                                        direction, lookupNum, mode);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, port,
                                        direction, lookupNum, mode);
            }

            direction = CPSS_PCL_DIRECTION_INGRESS_E;

            /*
                1.1.3. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E].
                Expected: GT_OK.
            */
            lookupNum = CPSS_PCL_LOOKUP_1_E;

            st = cpssDxChPclPortLookupCfgTabAccessModeSet(dev, port,
                            direction, lookupNum, subLookupNum, mode);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port,
                                        direction, lookupNum, mode);

            /*
                1.1.4. Check wrong direction enum values.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortLookupCfgTabAccessModeSet
                                (dev, port, direction, lookupNum, subLookupNum, mode),
                                direction);

            /*
                1.1.5. Check wrong lookupNum enum values.
                Expected: GT_BAD_PARAM.
            */
            if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                /* for CH1 the "lookupNum" ignored */
                UTF_ENUMS_CHECK_MAC(cpssDxChPclPortLookupCfgTabAccessModeSet
                                    (dev, port, direction, lookupNum, subLookupNum, mode),
                                    lookupNum);
            }

            lookupNum = CPSS_PCL_LOOKUP_0_E; /* restore */

            /*
                1.1.6. Call with subLookupNum [1],
                Expected: GT_OK for xCat and above and NOT GT_OK for other.
            */
            subLookupNum = 1;

            st = cpssDxChPclPortLookupCfgTabAccessModeSet(dev, port,
                            direction, lookupNum, subLookupNum, mode);

            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port,
                                                direction, lookupNum, mode);
            }

            subLookupNum = 0;

            /*
                1.1.7. Call with wrong subLookupNum [2],
                Expected: NOT GT_OK for xCat and above and GT_OK for other.
            */
            subLookupNum = 2;

            st = cpssDxChPclPortLookupCfgTabAccessModeSet(dev, port,
                            direction, lookupNum, subLookupNum, mode);

            if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port,
                                                direction, lookupNum, mode);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, port,
                                                direction, lookupNum, mode);
            }

            subLookupNum = 0;

            /*
                1.1.8. Check out-of-range mode enum.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortLookupCfgTabAccessModeSet
                                (dev, port, direction, lookupNum, subLookupNum, mode),
                                mode);
        }

        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_0_E;
        mode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E;

        /* 1.2. Check that function handles CPU port    */
        port = PRV_CPSS_DXCH_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxChPclPortLookupCfgTabAccessModeSet(dev, port,
                        direction, lookupNum, subLookupNum, mode);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, direction, lookupNum, mode);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxChPclPortLookupCfgTabAccessModeSet(dev, port,
                            direction, lookupNum, subLookupNum, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPclPortLookupCfgTabAccessModeSet(dev, port,
                        direction, lookupNum, subLookupNum, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = DXCH_PCL_VALID_PHY_PORT_CNS;
    lookupNum = CPSS_PCL_LOOKUP_0_E;
    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    mode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E;

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclPortLookupCfgTabAccessModeSet(dev, port, direction,
                                        lookupNum, subLookupNum, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortLookupCfgTabAccessModeSet(dev, port, direction,
                                        lookupNum, subLookupNum, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclCfgTblAccessModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   *accModePtr
)
*/

UTF_TEST_CASE_MAC(cpssDxChPclCfgTblAccessModeSet)
{
/*
    ITERATE_DEVICES
    1.1. Check with valid params. Call with accModePtr
            [ipclAccMode = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E,
            ipclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E,
            ipclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E,
            epclAccMode = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E,
            epclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E,
            epclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E ].
    Expected: GT_OK
    1.2. Call cpssDxChPclCfgTblAccessModeGet with non-NULL accModePtr.
    Expected: GT_OK and same values as were written.
    (for DxCh2 and above - all fields; for DxCh - only ipclAccMode)
    1.3. Check with valid params. Call with accModePtr
            [ipclAccMode = CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E,
            ipclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E,
            ipclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E,
            epclAccMode = CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E,
            epclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E,
            epclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E ].
    Expected: GT_OK
    1.2. Call cpssDxChPclCfgTblAccessModeGet with non-NULL accModePtr.
    Expected: GT_OK and same values as were written.
    (for DxCh2 and above - all fields; for DxCh - only ipclAccMode)
    1.4. Call with accModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                                   dev;
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   accMode;

    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   retAccMode;
    GT_BOOL                                 failureWas;
    CPSS_PP_FAMILY_TYPE_ENT                 devFamily;

    cpssOsBzero((GT_VOID*)&accMode, sizeof(accMode));

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /*
            1.1. Check with valid params. Call with accModePtr
                    [ipclAccMode = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E,
                    ipclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E,
                    ipclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E,
                    epclAccMode = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E,
                    epclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E,
                    epclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E ].
            Expected: GT_OK
        */
        accMode.ipclAccMode = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E;
        accMode.ipclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
        accMode.ipclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
        accMode.epclAccMode = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E;
        accMode.epclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
        accMode.epclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;

        st = cpssDxChPclCfgTblAccessModeSet(dev, &accMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPclCfgTblAccessModeGet with non-NULL accModePtr.
            Expected: GT_OK and same values as were written.
            (for DxCh2 and above - all fields; for DxCh - only ipclAccMode)
        */
        st = cpssDxChPclCfgTblAccessModeGet(dev, &retAccMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        if (GT_OK == st)
        {
            /* Verifying values */
            if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&accMode,
                                                (const GT_VOID*)&retAccMode,
                                                sizeof(accMode))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, failureWas,
                "cpssDxChPclCfgTblAccessModeGet: get another accMode than was set: %d", dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(accMode.ipclAccMode, retAccMode.ipclAccMode,
                "cpssDxChBrgCntDropCntrModeGet:"
                "get another accMode.ipclAccMode than was set: %d", dev);
            }
        }

        /*
            1.3. Check with valid params. Call with accModePtr
                    [ipclAccMode = CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E,
                    ipclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E,
                    ipclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E,
                    epclAccMode = CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E,
                    epclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E,
                    epclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E ].
            Expected: GT_OK
        */
        accMode.ipclAccMode = CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E;
        accMode.ipclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E;
        accMode.ipclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E;
        accMode.epclAccMode = CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E;
        accMode.epclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E;
        accMode.epclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E;

        st = cpssDxChPclCfgTblAccessModeSet(dev, &accMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPclCfgTblAccessModeGet with non-NULL accModePtr.
            Expected: GT_OK and same values as were written.
            (for DxCh2 and above - all fields; for DxCh - only ipclAccMode)
        */
        st = cpssDxChPclCfgTblAccessModeGet(dev, &retAccMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        if (GT_OK == st)
        {
            /* Verifying values */
            if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&accMode,
                                                (const GT_VOID*)&retAccMode,
                                                sizeof(accMode))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, failureWas,
                "cpssDxChPclCfgTblAccessModeGet: get another accMode than was set: %d", dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(accMode.ipclAccMode, retAccMode.ipclAccMode,
                "cpssDxChBrgCntDropCntrModeGet:"
                "get another accMode.ipclAccMode than was set: %d", dev);
            }
        }

        /*
            1.4. Call with accModePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclCfgTblAccessModeSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclCfgTblAccessModeSet(dev, &accMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclCfgTblAccessModeSet(dev, &accMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclCfgTblAccessModeGet
(
    IN  GT_U8                                   devNum,
    OUT CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   *accModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclCfgTblAccessModeGet)
{
/*
    ITERATE_DEVICES
    1.1. For all devices call with non-NULL accModePtr.
    Expected: GT_OK
    1.2. Check for NULL pointer support. Call with accModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                                   dev;
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   retAccMode;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. For all devices call with non-NULL accModePtr.
            Expected: GT_OK
        */
        st = cpssDxChPclCfgTblAccessModeGet(dev, &retAccMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Check for NULL pointer support. Call with accModePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclCfgTblAccessModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclCfgTblAccessModeGet(dev, &retAccMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclCfgTblAccessModeGet(dev, &retAccMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclCfgTblSet
(
    IN GT_U8                           devNum,
    IN CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT          direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclCfgTblSet)
{
/*
    ITERATE_DEVICES
       1.1. Check Ingress direction. Call with
       interfaceInfoPtr [type = CPSS_INTERFACE_PORT_E,
                     devPort {devNum=0, portNum=0} ],
       direction [CPSS_PCL_DIRECTION_INGRESS_E],
       lookupNum [CPSS_PCL_LOOKUP_0_E],
       lookupCfgPtr [enableLookup = GT_TRUE,
                 pclId = 1023,
                 groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
                 groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
                 other fields zeroed } ].
    Expected: GT_OK
    1.2. Call cpssDxChPclCfgTblGet with interfaceInfoPtr,
    direction, lookupNum - same as in 1.1, non-NULL lookupCfgPtr.
    Expected: GT_OK and same lookupCfgPtr as was written.
    1.3. Check Egress direction (not supported by Ch1). Call with
    direction [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.1.
    Expected: GT_OK for all devices and non GT_OK for Ch1.
    1.4. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E],
    other params same as in 1.1.
    Expected: GT_OK.

       -- II - INTERFACE_TYPE - VLan (interfaceInfoPtr->type= CPSS_INTERFACE_VID_E) --

    1.5. Check Ingress direction. Call with interfaceInfoPtr [type = CPSS_INTERFACE_VID_E,
    vlanId = [100 / (PRV_CPSS_MAX_NUM_VLANS_CNS - 1) = 4095] ],
    direction [CPSS_PCL_DIRECTION_INGRESS_E],
    lookupNum [CPSS_PCL_LOOKUP_0_E],
    lookupCfgPtr [enableLookup = GT_TRUE,
                  pclId = 1023,
                  groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
                  other fields zeroed } ].
    Expected: GT_OK for all devices except ch1.
    1.6. Call cpssDxChPclCfgTblGet with interfaceInfoPtr,
    direction, lookupNum - same as in 1.5, non-NULL lookupCfgPtr.
    Expected: GT_OK and same lookupCfgPtr as was written.
    1.7. Check Egress direction (not supported by Ch1).
    Call with direction [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.5.
    Expected: GT_OK for all devices except Ch1.
    1.8. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E],
    other params same as in 1.5.
    Expected: GT_OK.
    1.9. Check out-of-range direction enum. Call with direction [wrong enum values],
        other params same as in 1.5.
    Expected: GT_BAD_PARAM.
    1.10. Check out-of-range lookupNum enum. Call with lookupNum [wrong enum values],
    other params same as in 1.5.
    Expected: GT_BAD_PARAM.
    1.11. Call with lookupCfgPtr [NULL], other params same as in 1.5.
    Expected: GT_BAD_PTR.
    1.12. Call with interfaceInfoPtr [NULL], other params same as in 1.5.
    Expected: GT_BAD_PTR.
    1.13. Check out-of-range vlan id. call with interfaceInfoPtr
    [type = CPSS_INTERFACE_VID_E, vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] ],
    other params same as in 1.5.
    Expected: non GT_OK.

       -- III - INTERFACE_TYPE - TRUNK (interfaceInfoPtr->type= CPSS_INTERFACE_TRUNK_E) --
                                     Supported only in Non-Local mode

    1.14. Check trunk interface with valid params. Call with
            interfaceInfoPtr [type = CPSS_INTERFACE_TRUNK_E,
                              trunkId = 0 ],
            direction [CPSS_PCL_DIRECTION_INGRESS_E],
            lookupNum [CPSS_PCL_LOOKUP_0_E],
            lookupCfgPtr [enableLookup = GT_TRUE,
                          pclId = 1023,
                          groupKeyTypes
                            {nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                             ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
                             ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E } ].
    Expected: NOT GT_OK.
    1.15. Check out-of-range trunk id. Call with interfaceInfoPtr
               [type = CPSS_INTERFACE_TRUNK_E,
                trunkId [MAX_TRUNK_ID_CNS = 128] ], other params same as in 1.14.
    Expected: non GT_OK.

       -- IV - INTERFACE_TYPE - Vidx (unsupported) (interfaceInfoPtr->type= CPSS_INTERFACE_VIDX_E) --

    1.16. Check Vidx interface (unsupported) with valid params.
            Call with interfaceInfoPtr [type = CPSS_INTERFACE_VIDX_E, vidx = 0 ],
            other params same as in 1.1.
    Expected: NOT GT_OK.
       -- V - INTERFACE_TYPE - out-of-range --

    1.17. Check out-of-range interface type enum.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                       st = GT_OK;
    GT_U8                           dev;

    CPSS_INTERFACE_INFO_STC         interfaceInfo;
    CPSS_PCL_DIRECTION_ENT          direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfg;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfgEgr;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfgRet;

    CPSS_PP_FAMILY_TYPE_ENT         devFamily;
    GT_BOOL                         failureWas;
    /* access mode */
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   accMode;

    cpssOsBzero((GT_VOID*) &interfaceInfo, sizeof(interfaceInfo));
    cpssOsBzero((GT_VOID*) &lookupCfg, sizeof(lookupCfg));
    cpssOsBzero((GT_VOID*) &lookupCfgRet, sizeof(lookupCfgRet));

    accMode.ipclAccMode     = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E;
    accMode.epclAccMode     = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E;
    accMode.ipclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
    accMode.epclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
    accMode.ipclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
    accMode.epclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        st = cpssDxChPclCfgTblAccessModeSet(
            dev, &accMode);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, st, "cpssDxChPclCfgTblAccessModeSet: %d", dev);

        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);

        /* I - INTERFACE_TYPE - Port (interfaceInfoPtr->type= CPSS_INTERFACE_PORT_E) */

        /*
           1.1. Check Ingress direction. Call with
           interfaceInfoPtr [type = CPSS_INTERFACE_PORT_E,
                             devPort {devNum=0, portNum=0} ],
           direction [CPSS_PCL_DIRECTION_INGRESS_E],
           lookupNum [CPSS_PCL_LOOKUP_0_E],
           lookupCfgPtr [enableLookup = GT_TRUE,
                         pclId = 1023,
                         groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
                         groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
                         other fields zeroed } ].
            Expected: GT_OK
        */
        interfaceInfo.type = CPSS_INTERFACE_PORT_E; /* should be synhronized with 1.2*/
        interfaceInfo.devPort.devNum = dev;
        interfaceInfo.devPort.portNum = 0;
        direction=CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum=CPSS_PCL_LOOKUP_0_E;

        cpssOsBzero((GT_VOID*) &lookupCfg, sizeof(lookupCfg));
        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.pclId = 1023;
        lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
        lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;

        cpssOsBzero((GT_VOID*) &lookupCfgEgr, sizeof(lookupCfgEgr));
        lookupCfgEgr.enableLookup = GT_TRUE;
        lookupCfgEgr.pclId = 1023;
        lookupCfgEgr.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E;
        lookupCfgEgr.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E;
        lookupCfgEgr.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E;

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            lookupCfg.dualLookup = GT_TRUE;
            lookupCfg.pclIdL01 = 1023;
        }

        st = cpssDxChPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
            "dev: %d, interfaceInfo [devNum= %d, portNum= %d], direction=%d, lookupNum=%d",
            dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                        direction, lookupNum);

        /*
            1.2. Call cpssDxChPclCfgTblGet with interfaceInfoPtr,
            direction, lookupNum - same as in 1.1, non-NULL lookupCfgPtr.
            Expected: GT_OK and same lookupCfgPtr as was written.
        */
        cpssOsMemSet((GT_VOID*)&lookupCfgRet, 0, sizeof(CPSS_DXCH_PCL_LOOKUP_CFG_STC));

        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfgRet);

        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
            "cpssDxChPclCfgTblGet: "
            "dev: %d, intInfo [devNum=%d, portNum=%d], direction=%d, lookupNum=%d",
            dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                        direction, lookupNum);

        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&lookupCfg,
                                            (const GT_VOID*)&lookupCfgRet,
                                            sizeof(lookupCfg))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                "get another lookupCfg than was written "
                "[dev = %d, direction=%d, lookupNum=%d]",
                dev, direction, lookupNum);
        }

        /*
            1.3. Check Egress direction (not supported by Ch1). Call with
            direction [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.1.
            Expected: GT_OK for all devices and non GT_OK for Ch1.
        */
        direction = CPSS_PCL_DIRECTION_EGRESS_E;

        st = cpssDxChPclCfgTblSet(
            dev, &interfaceInfo, direction, lookupNum, &lookupCfgEgr);

        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                "dev: %d, interfaceInfo [devNum=%d, portNum=%d], direction=%d, lookupNum=%d",
                dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                            direction, lookupNum);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                    "dev: %d, direction=%d", dev, direction);
        }

        direction = CPSS_PCL_DIRECTION_INGRESS_E;

        /*
            1.4. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E],
            other params same as in 1.1.
            Expected: GT_OK.
        */
        lookupNum = CPSS_PCL_LOOKUP_1_E;

        st = cpssDxChPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
            "dev: %d, interfaceInfo [devNum=%d, portNum=%d], direction=%d, lookupNum=%d",
            dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                        direction, lookupNum);

        lookupNum = CPSS_PCL_LOOKUP_0_E;


        /*
           -- II - INTERFACE_TYPE - VLan (interfaceInfoPtr->type= CPSS_INTERFACE_VID_E) --

            1.5. Check Ingress direction. Call with interfaceInfoPtr [type = CPSS_INTERFACE_VID_E,
            vlanId = [100 / (PRV_CPSS_MAX_NUM_VLANS_CNS - 1) = 4095] ],
            direction [CPSS_PCL_DIRECTION_INGRESS_E],
            lookupNum [CPSS_PCL_LOOKUP_0_E],
            lookupCfgPtr [enableLookup = GT_TRUE,
                          pclId = 1023,
                          groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
                          other fields zeroed } ].
            Expected: GT_OK for all devices except ch1.
        */

        interfaceInfo.type = CPSS_INTERFACE_VID_E;
        interfaceInfo.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS - 1;
        direction=CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum=CPSS_PCL_LOOKUP_0_E;

        cpssOsBzero((GT_VOID*) &lookupCfg, sizeof(lookupCfg));
        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.pclId = 1023;
        lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
        lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;

        st = cpssDxChPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);

        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                        "dev: %d, interfaceInfo [type=%d, vlanId=%d]",
                                dev, interfaceInfo.type, interfaceInfo.vlanId);
        }
        else
        {
            /* CH1 supports VIDs 0-1023 only */
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                        "dev: %d, interfaceInfo [type=%d, vlanId=%d]",
                            dev, interfaceInfo.type, interfaceInfo.vlanId);
        }

        interfaceInfo.vlanId = 100;

        st = cpssDxChPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);

        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
            "dev: %d, interfaceInfo [type=%d, vlanId=%d], direction=%d, lookupNum=%d",
            dev, interfaceInfo.type, interfaceInfo.vlanId, direction, lookupNum);


        /*
            1.6. Call cpssDxChPclCfgTblGet with interfaceInfoPtr,
            direction, lookupNum - same as in 1.5, non-NULL lookupCfgPtr.
            Expected: GT_OK and same lookupCfgPtr as was written.
        */

        cpssOsMemSet((GT_VOID*)&lookupCfgRet, 0, sizeof(CPSS_DXCH_PCL_LOOKUP_CFG_STC));

        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfgRet);

        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
            "cpssDxChPclCfgTblGet:"
            "dev: %d, intInfo [type=%d, vlanId=%d], direction=%d, lookupNum=%d",
            dev, interfaceInfo.type, interfaceInfo.vlanId, direction, lookupNum);

        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&lookupCfg,
                                            (const GT_VOID*)&lookupCfgRet,
                                            sizeof(lookupCfg))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                "get another lookupCfg than has been written [dev = %d, direction=%d, lookupNum=%d]",
                dev, direction, lookupNum);
        }

        /*
            1.7. Check Egress direction (not supported by Ch1).
            Call with direction [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.5.
            Expected: GT_OK for all devices except Ch1.
        */
        direction = CPSS_PCL_DIRECTION_EGRESS_E;

        cpssOsBzero((GT_VOID*) &lookupCfg, sizeof(lookupCfg));
        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.pclId = 1023;
        lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;
        lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;
        lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E;

        st = cpssDxChPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);

        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
              "dev: %d, interfaceInfo [type=%d, vlanId =%d], direction=%d, lookupNum=%d",
                  dev, interfaceInfo.type, interfaceInfo.vlanId, direction, lookupNum);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "dev: %d, direction=%d", dev, direction);
        }

        direction = CPSS_PCL_DIRECTION_INGRESS_E;

        /*
            1.8. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E],
            other params same as in 1.5.
            Expected: GT_OK.
        */
        lookupNum = CPSS_PCL_LOOKUP_1_E;

        cpssOsBzero((GT_VOID*) &lookupCfg, sizeof(lookupCfg));
        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.pclId = 1023;
        lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
        lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;

        st = cpssDxChPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
              "dev: %d, interfaceInfo [type=%d, vlanId =%d], direction=%d, lookupNum=%d",
              dev, interfaceInfo.type, interfaceInfo.vlanId, direction, lookupNum);

        lookupNum = CPSS_PCL_LOOKUP_0_E;

        /*
            1.9. Check out-of-range direction enum. Call with direction [wrong enum values],
                other params same as in 1.5.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclCfgTblSet
                            (dev, &interfaceInfo, direction, lookupNum, &lookupCfg),
                            direction);

        /*
            1.10. Check out-of-range lookupNum enum. Call with lookupNum [wrong enum values],
            other params same as in 1.5.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclCfgTblSet
                            (dev, &interfaceInfo, direction, lookupNum, &lookupCfg),
                            lookupNum);

        /*
            1.11. Call with lookupCfgPtr [NULL], other params same as in 1.5.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "dev: %d, lookupCfgPtr=NULL", dev);

        /*
            1.12. Call with interfaceInfoPtr [NULL], other params same as in 1.5.
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChPclCfgTblSet(dev, NULL, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "dev: %d, interfaceInfoPtr=NULL", dev);

        /*
            1.13. Check out-of-range vlan id. call with interfaceInfoPtr
            [type = CPSS_INTERFACE_VID_E, vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] ],
            other params same as in 1.5.
            Expected: non GT_OK.
        */
        interfaceInfo.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "dev: %d, interfaceInfo.vlanId = %d",
                                         dev, interfaceInfo.vlanId);

        interfaceInfo.vlanId = 100;


        /*
           -- III - INTERFACE_TYPE - TRUNK (interfaceInfoPtr->type= CPSS_INTERFACE_TRUNK_E) --
                                             Supported only in Non-Local mode

            1.14. Check trunk interface with valid params. Call with
                    interfaceInfoPtr [type = CPSS_INTERFACE_TRUNK_E,
                                      trunkId = 0 ],
                    direction [CPSS_PCL_DIRECTION_INGRESS_E],
                    lookupNum [CPSS_PCL_LOOKUP_0_E],
                    lookupCfgPtr [enableLookup = GT_TRUE,
                                  pclId = 1023,
                                  groupKeyTypes
                                    {nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                                     ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
                                     ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E } ].
            Expected: NOT GT_OK.
        */
        interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        interfaceInfo.trunkId = 0;

        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);
        direction=CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum=CPSS_PCL_LOOKUP_0_E;

        lookupCfg.enableLookup = GT_TRUE;
        lookupCfg.pclId = 1023;
        lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
        lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;

        st = cpssDxChPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);

        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                  "dev: %d, interfaceInfo [trunkId=%d], direction=%d, lookupNum=%d",
                                   dev, interfaceInfo.trunkId, direction, lookupNum);

        /*
            1.15. Check out-of-range trunk id. Call with interfaceInfoPtr
                       [type = CPSS_INTERFACE_TRUNK_E,
                        trunkId [MAX_TRUNK_ID_CNS = 128] ], other params same as in 1.14.
            Expected: non GT_OK.
        */
        interfaceInfo.trunkId = MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);

        st = cpssDxChPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
               "dev: %d, interfaceInfo.trunkId = %d", dev, interfaceInfo.trunkId);

        interfaceInfo.trunkId = 0;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);


        /*
           -- IV - INTERFACE_TYPE - Vidx (unsupported) (interfaceInfoPtr->type= CPSS_INTERFACE_VIDX_E) --

            1.16. Check Vidx interface (unsupported) with valid params.
                    Call with interfaceInfoPtr [type = CPSS_INTERFACE_VIDX_E, vidx = 0 ],
                    other params same as in 1.1.
            Expected: NOT GT_OK.
        */
        interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        interfaceInfo.vidx = 0;

        st = cpssDxChPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                "dev: %d, interfaceInfo.type = %d", dev, interfaceInfo.type);

        interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        interfaceInfo.devPort.devNum = dev;
        interfaceInfo.devPort.portNum = 0;


        /*
           -- V - INTERFACE_TYPE - out-of-range --

            1.17. Check out-of-range interface type enum.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclCfgTblSet
                            (dev, &interfaceInfo, direction, lookupNum, &lookupCfg),
                            interfaceInfo.type);
    }

    /* set correct values for all the parameters (except of device number) */
    interfaceInfo.type = CPSS_INTERFACE_VID_E;
    interfaceInfo.vlanId = 100;

    direction=CPSS_PCL_DIRECTION_INGRESS_E;

    lookupNum=CPSS_PCL_LOOKUP_0_E;
    lookupCfg.enableLookup = GT_TRUE;
    lookupCfg.pclId = 1023;
    lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
    lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclCfgTblSet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclCfgTblGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    OUT CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclCfgTblGet)
{
/*
    ITERATE_DEVICES

       I - INTERFACE_TYPE - Port (interfaceInfoPtr->type= CPSS_INTERFACE_PORT_E)

    1.1. Check Ingress direction. Call with
                interfaceInfoPtr [type = CPSS_INTERFACE_PORT_E,
                                  devPort {devNum=0, portNum=0} ],
                direction [CPSS_PCL_DIRECTION_INGRESS_E],
                lookupNum [CPSS_PCL_LOOKUP_0_E], non-NULL lookupCfgPtr.
    Expected: GT_OK
    1.2. Check Egress direction (not supported by Ch1).
    Call with direction [CPSS_PCL_DIRECTION_EGRESS_E],
    other params same as in 1.1.
    Expected: GT_OK for all devices except Ch1.
    1.3. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E],
    other params same as in 1.1.
    Expected: GT_OK.

       -- II - INTERFACE_TYPE - VLan (interfaceInfoPtr->type= CPSS_INTERFACE_VID_E) --

    1.4. Check Ingress direction. Call with
    interfaceInfoPtr [type = CPSS_INTERFACE_VID_E,
                      vlanId = [100  / (PRV_CPSS_MAX_NUM_VLANS_CNS - 1) = 4095] ],
    direction [CPSS_PCL_DIRECTION_INGRESS_E],
    lookupNum [CPSS_PCL_LOOKUP_0_E],
    lookupCfgPtr [enableLookup = GT_TRUE,
                  pclId = 1023,
                  groupKeyTypes
                   {nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                    ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
                    ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E } ].
    Expected: GT_OK for all devices except ch1.
    1.5. Check Egress direction (not supported by Ch1). Call with direction
    [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.4.
    Expected: GT_OK for all devices except ch1.
    1.6. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E],
    other params same as in 1.4.
    Expected: GT_OK.
    1.7. Call with direction [wrong enum values], other params same as in 1.4.
    Expected: GT_BAD_PARAM.
    1.8. Call with lookupNum [wrong enum values], other params same as in 1.4.
    Expected: GT_BAD_PARAM.
    1.9.  Call with lookupCfgPtr [NULL], other params same as in 1.4.
    Expected: GT_BAD_PTR.
    1.10. Call with interfaceInfoPtr [NULL], other params same as in 1.4.
    Expected: GT_BAD_PTR.
    1.11. Check out-of-range vlan id. Call with
           interfaceInfoPtr [type = CPSS_INTERFACE_VID_E,
                             vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] ],
           other params same as in 1.4.
    Expected: non GT_OK.

       -- III - INTERFACE_TYPE - TRUNK (interfaceInfoPtr->type= CPSS_INTERFACE_TRUNK_E) --
                                    Supported only in Non-Local mode

    1.12. Check trunk interface with valid params. Call with
            interfaceInfoPtr [type = CPSS_INTERFACE_TRUNK_E,
                              trunkId = 0 ],
            direction [CPSS_PCL_DIRECTION_INGRESS_E],
            lookupNum [CPSS_PCL_LOOKUP_0_E],
            lookupCfgPtr [enableLookup = GT_TRUE,
                          pclId = 1023,
                          groupKeyTypes {
                              nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                              ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
                              ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E } ].
    Expected: GT_OK.
    1.13. Check out-of-range trunk id. Call with interfaceInfoPtr
    [type = CPSS_INTERFACE_TRUNK_E, trunkId [MAX_TRUNK_ID_CNS = 128] ],
    other params same as in 1.12.
    Expected: NOT GT_OK.

       -- IV - INTERFACE_TYPE - Vidx (unsupported) (interfaceInfoPtr->type= CPSS_INTERFACE_VIDX_E) --

    1.14. Check Vidx interface (unsupported) with valid params.
            Call with interfaceInfoPtr [type = CPSS_INTERFACE_VIDX_E,
                                        vidx = 0 ],
            other params same as in 1.1.
    Expected: non GT_OK.

       -- V - INTERFACE_TYPE - out-of-range --

    1.15. Check out-of-range interface type enum. Call with
            interfaceInfoPtr [type = wrong enum values,
                              devPort {devNum=0, portNum=0},
                              trunkId = 0,
                              vidx = 0,
                              vlanId = 100 ],
            other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                       st = GT_OK;
    GT_U8                           dev;

    CPSS_INTERFACE_INFO_STC         interfaceInfo;
    CPSS_PCL_DIRECTION_ENT          direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfg;

    CPSS_PP_FAMILY_TYPE_ENT         devFamily;
    /* access mode */
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   accMode;

    accMode.ipclAccMode     = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E;
    accMode.epclAccMode     = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E;
    accMode.ipclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
    accMode.epclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
    accMode.ipclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
    accMode.epclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = cpssDxChPclCfgTblAccessModeSet(
            dev, &accMode);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, st, "cpssDxChPclCfgTblAccessModeSet: %d", dev);

        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /*
           I - INTERFACE_TYPE - Port (interfaceInfoPtr->type= CPSS_INTERFACE_PORT_E)

            1.1. Check Ingress direction. Call with
                        interfaceInfoPtr [type = CPSS_INTERFACE_PORT_E,
                                          devPort {devNum=0, portNum=0} ],
                        direction [CPSS_PCL_DIRECTION_INGRESS_E],
                        lookupNum [CPSS_PCL_LOOKUP_0_E], non-NULL lookupCfgPtr.
            Expected: GT_OK
        */
        interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        interfaceInfo.devPort.devNum = dev;
        interfaceInfo.devPort.portNum = 0;

        direction=CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum=CPSS_PCL_LOOKUP_0_E;

        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);

        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
            "dev: %d, interfaceInfo [devNum=%d, portNum=%d], direction=%d, lookupNum=%d",
            dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                      direction, lookupNum);

        /*
            1.2. Check Egress direction (not supported by Ch1).
            Call with direction [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.1.
            Expected: GT_OK for all devices except Ch1.
        */
        direction = CPSS_PCL_DIRECTION_EGRESS_E;
        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);

        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
               "dev: %d, interfaceInfo [devNum=%d, portNum=%d], direction=%d, lookupNum=%d",
               dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                            direction, lookupNum);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                        "Cheetah1 device: %d, direction=%d", dev, direction);
        }

        direction = CPSS_PCL_DIRECTION_INGRESS_E;

        /*
            1.3. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E],
            other params same as in 1.1.
            Expected: GT_OK.
        */
        lookupNum = CPSS_PCL_LOOKUP_1_E;
        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);

        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
           "dev: %d, interfaceInfo [devNum=%d, portNum=%d], direction=%d, lookupNum=%d",
                   dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                                direction, lookupNum);

        lookupNum = CPSS_PCL_LOOKUP_0_E;


        /*
           -- II - INTERFACE_TYPE - VLan (interfaceInfoPtr->type= CPSS_INTERFACE_VID_E) --

            1.4. Check Ingress direction. Call with
            interfaceInfoPtr [type = CPSS_INTERFACE_VID_E,
                              vlanId = [100  / (PRV_CPSS_MAX_NUM_VLANS_CNS - 1) = 4095] ],
            direction [CPSS_PCL_DIRECTION_INGRESS_E],
            lookupNum [CPSS_PCL_LOOKUP_0_E],
            lookupCfgPtr [enableLookup = GT_TRUE,
                          pclId = 1023,
                          groupKeyTypes
                           {nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                            ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
                            ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E } ].
            Expected: GT_OK for all devices except ch1.
        */
        interfaceInfo.type = CPSS_INTERFACE_VID_E;
        interfaceInfo.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS - 1;

        direction=CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum=CPSS_PCL_LOOKUP_0_E;

        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);

        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "dev: %d, interfaceInfo [type=%d, vlanId =%d]",
                                dev, interfaceInfo.type, interfaceInfo.vlanId);
        }
        else
        {
            /* CH1 supports VIDs 0-1023 only */
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                        "dev: %d, interfaceInfo [type=%d, vlanId =%d]",
                            dev, interfaceInfo.type, interfaceInfo.vlanId);
        }

        /*call with interfaceInfo.vlanId = 100;*/
        interfaceInfo.vlanId = 100;

        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);

        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
            "dev: %d, interfaceInfo [type=%d, vlanId =%d], direction=%d, lookupNum=%d",
                dev, interfaceInfo.type, interfaceInfo.vlanId, direction, lookupNum);

        /*
            1.5. Check Egress direction (not supported by Ch1).
            Call with direction [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.4.
            Expected: GT_OK for all devices except ch1.
        */
        direction = CPSS_PCL_DIRECTION_EGRESS_E;
        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);

        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
              "dev: %d, interfaceInfo [type=%d, vlanId =%d], direction=%d, lookupNum=%d",
                  dev, interfaceInfo.type, interfaceInfo.vlanId, direction, lookupNum);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                    "Cheetah device: %d, direction=%d", dev, direction);
        }

        direction = CPSS_PCL_DIRECTION_INGRESS_E;

        /*
            1.6. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E],
            other params same as in 1.4.
            Expected: GT_OK.
        */
        lookupNum = CPSS_PCL_LOOKUP_1_E;

        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
              "dev: %d, interfaceInfo [type=%d, vlanId =%d], direction=%d, lookupNum=%d",
                  dev, interfaceInfo.type, interfaceInfo.vlanId, direction, lookupNum);

        lookupNum = CPSS_PCL_LOOKUP_0_E;

        /*
            1.7. Call with direction [wrong enum values], other params same as in 1.4.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclCfgTblGet
                            (dev, &interfaceInfo, direction, lookupNum, &lookupCfg),
                            direction);

        /*
            1.8. Call with lookupNum [wrong enum values], other params same as in 1.4.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclCfgTblGet
                            (dev, &interfaceInfo, direction, lookupNum, &lookupCfg),
                            lookupNum);

        /*
            1.9.  Call with lookupCfgPtr [NULL], other params same as in 1.4.
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "dev: %d, lookupCfgPtr=NULL", dev);

        /*
            1.10. Call with interfaceInfoPtr [NULL], other params same as in 1.4.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclCfgTblGet(dev, NULL, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "dev: %d, interfaceInfoPtr=NULL", dev);

        /*
            1.11. Check out-of-range vlan id. Call with
                   interfaceInfoPtr [type = CPSS_INTERFACE_VID_E,
                                     vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] ],
                   other params same as in 1.4.
            Expected: non GT_OK.
        */
        interfaceInfo.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "dev: %d, interfaceInfo.vlanId = %d", dev,
                                          interfaceInfo.vlanId);

        interfaceInfo.vlanId = 100;


        /*
           -- III - INTERFACE_TYPE - TRUNK (interfaceInfoPtr->type= CPSS_INTERFACE_TRUNK_E) --
                                            Supported only in Non-Local mode

            1.12. Check trunk interface with valid params. Call with
                    interfaceInfoPtr [type = CPSS_INTERFACE_TRUNK_E,
                                      trunkId = 0 ],
                    direction [CPSS_PCL_DIRECTION_INGRESS_E],
                    lookupNum [CPSS_PCL_LOOKUP_0_E],
                    lookupCfgPtr [enableLookup = GT_TRUE,
                                  pclId = 1023,
                                  groupKeyTypes {
                                      nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                                      ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
                                      ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E } ].
            Expected: GT_OK.
        */
        interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        interfaceInfo.trunkId = 0;

        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);

        direction=CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum=CPSS_PCL_LOOKUP_0_E;

        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);

        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
          "dev: %d, interfaceInfo [trunkId=%d], direction=%d, lookupNum=%d",
                                   dev, interfaceInfo.trunkId, direction, lookupNum);

        /*
            1.13. Check out-of-range trunk id. Call with interfaceInfoPtr
            [type = CPSS_INTERFACE_TRUNK_E, trunkId [MAX_TRUNK_ID_CNS = 128] ],
            other params same as in 1.12.
            Expected: NOT GT_OK.
        */
        interfaceInfo.trunkId = MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);

        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
               "dev: %d, interfaceInfo.trunkId = %d", dev, interfaceInfo.trunkId);

        interfaceInfo.trunkId = 0;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);


        /*
           -- IV - INTERFACE_TYPE - Vidx (unsupported) (interfaceInfoPtr->type= CPSS_INTERFACE_VIDX_E) --

            1.14. Check Vidx interface (unsupported) with valid params.
                    Call with interfaceInfoPtr [type = CPSS_INTERFACE_VIDX_E,
                                                vidx = 0 ],
                    other params same as in 1.1.
            Expected: non GT_OK.
        */
        interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        interfaceInfo.vidx = 0;

        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "dev: %d, interfaceInfo.type = %d",
                                         dev, interfaceInfo.type);

        interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        interfaceInfo.vidx = 0;


        /*
           -- V - INTERFACE_TYPE - out-of-range --

            1.15. Check out-of-range interface type enum. Call with
                    interfaceInfoPtr [type = wrong enum values,
                                      devPort {devNum=0, portNum=0},
                                      trunkId = 0,
                                      vidx = 0,
                                      vlanId = 100 ],
                    other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclCfgTblGet
                            (dev, &interfaceInfo, direction, lookupNum, &lookupCfg),
                            interfaceInfo.type);
    }

    /* set correct values for all the parameters (except of device number) */
    interfaceInfo.type = CPSS_INTERFACE_VID_E;
    interfaceInfo.vlanId = 100;

    direction=CPSS_PCL_DIRECTION_INGRESS_E;
    lookupNum=CPSS_PCL_LOOKUP_0_E;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclCfgTblGet(dev, &interfaceInfo, direction, lookupNum, &lookupCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclIngressPolicyEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclIngressPolicyEnable)
{
/*
    ITERATE_DEVICES
    1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_BOOL                     enable;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* call with enable = GT_FALSE;*/
        enable = GT_FALSE;

        st = cpssDxChPclIngressPolicyEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "dev: %d, %d", dev, enable);

        /* call with enable = GT_TRUE;*/
        enable = GT_TRUE;

        st = cpssDxChPclIngressPolicyEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "dev: %d, %d", dev, enable);
    }

    enable = GT_FALSE;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclIngressPolicyEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclIngressPolicyEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclTcamRuleSizeModeSet
(
    IN GT_U8                       devNum,
    IN GT_BOOL                     mixedRuleSizeMode,
    IN CPSS_PCL_RULE_SIZE_ENT      ruleSize
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclTcamRuleSizeModeSet)
{
/*
    ITERATE_DEVICES (DxCh1)
    1.1. Call with mixedRuleSizeMode [GT_TRUE / GT_FALSE],
            ruleSize [CPSS_PCL_RULE_SIZE_STD_E /
                      CPSS_PCL_RULE_SIZE_EXT_E].
    Expected: GT_OK
    1.2. Check for invalid enum value for ruleSize. Call with
    ruleSize [wrong enum values], mixedRuleSizeMode [GT_FALSE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                   st = GT_OK;

    GT_U8                       dev;
    GT_BOOL                     mixedRuleSizeMode;
    CPSS_PCL_RULE_SIZE_ENT      ruleSize;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mixedRuleSizeMode [GT_TRUE / GT_FALSE],
                    ruleSize [CPSS_PCL_RULE_SIZE_STD_E /
                              CPSS_PCL_RULE_SIZE_EXT_E].
            Expected: GT_OK
        */
        /* call with mixedRuleSizeMode = GT_TRUE; ruleSize = CPSS_PCL_RULE_SIZE_STD_E; */
        mixedRuleSizeMode = GT_TRUE;
        ruleSize = CPSS_PCL_RULE_SIZE_STD_E;

        st = cpssDxChPclTcamRuleSizeModeSet(dev, mixedRuleSizeMode, ruleSize);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mixedRuleSizeMode, ruleSize);

        /* call with mixedRuleSizeMode = GT_TRUE; ruleSize = CPSS_PCL_RULE_SIZE_EXT_E; */
        mixedRuleSizeMode = GT_TRUE;
        ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;

        st = cpssDxChPclTcamRuleSizeModeSet(dev, mixedRuleSizeMode, ruleSize);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mixedRuleSizeMode, ruleSize);

        /* call with mixedRuleSizeMode = GT_FALSE; ruleSize = CPSS_PCL_RULE_SIZE_STD_E; */
        mixedRuleSizeMode = GT_FALSE;
        ruleSize = CPSS_PCL_RULE_SIZE_STD_E;

        st = cpssDxChPclTcamRuleSizeModeSet(dev, mixedRuleSizeMode, ruleSize);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mixedRuleSizeMode, ruleSize);

        /* call with mixedRuleSizeMode = GT_FALSE; ruleSize = CPSS_PCL_RULE_SIZE_EXT_E; */
        mixedRuleSizeMode = GT_FALSE;
        ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;

        st = cpssDxChPclTcamRuleSizeModeSet(dev, mixedRuleSizeMode, ruleSize);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mixedRuleSizeMode, ruleSize);

        /*
            1.2. Check for invalid enum value for ruleSize. Call with
            ruleSize [wrong enum values], mixedRuleSizeMode [GT_FALSE].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclTcamRuleSizeModeSet
                            (dev, mixedRuleSizeMode, ruleSize),
                            ruleSize);
    }

    mixedRuleSizeMode = GT_TRUE;
    ruleSize = CPSS_PCL_RULE_SIZE_STD_E;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclTcamRuleSizeModeSet(dev, mixedRuleSizeMode, ruleSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclTcamRuleSizeModeSet(dev, mixedRuleSizeMode, ruleSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclTwoLookupsCpuCodeResolution
(
    IN GT_U8                       devNum,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT  lookupNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclTwoLookupsCpuCodeResolution)
{
/*
    ITERATE_DEVICES
    1.1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E / CPSS_PCL_LOOKUP_1_E].
    Expected: GT_OK.
    1.2. Check for invalid enum value for lookupNum.
    Call with lookupNum [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                               st = GT_OK;
    GT_U8                                   dev;
    CPSS_PCL_LOOKUP_NUMBER_ENT              lookupNum;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E / CPSS_PCL_LOOKUP_1_E].
            Expected: GT_OK.
        */
        /* call with lookupNum = CPSS_PCL_LOOKUP_0_E; */
        lookupNum = CPSS_PCL_LOOKUP_0_E;

        st = cpssDxChPclTwoLookupsCpuCodeResolution(dev, lookupNum);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "dev: %d, %d", dev, lookupNum);

        /* call with lookupNum = CPSS_PCL_LOOKUP_1_E; */
        lookupNum = CPSS_PCL_LOOKUP_1_E;

        st = cpssDxChPclTwoLookupsCpuCodeResolution(dev, lookupNum);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "dev: %d, %d", dev, lookupNum);

        /*
            1.2. Check for invalid enum value for lookupNum.
            Call with lookupNum [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclTwoLookupsCpuCodeResolution
                            (dev, lookupNum),
                            lookupNum);
    }

    lookupNum = CPSS_PCL_LOOKUP_1_E;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclTwoLookupsCpuCodeResolution(dev, lookupNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclTwoLookupsCpuCodeResolution(dev, lookupNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclInvalidUdbCmdSet
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_UDB_ERROR_CMD_ENT  udbErrorCmd
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclInvalidUdbCmdSet)
{
/*
    ITERATE_DEVICES
    1.1. Call with udbErrorCmd [CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E/
                                CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E].
    Expected: GT_OK
    1.2. Check for invalid enum value for udbErrorCmd.  Call with
    udbErrorCmd [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                               st = GT_OK;
    GT_U8                                   dev;
    CPSS_DXCH_UDB_ERROR_CMD_ENT             udbErrorCmd;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with udbErrorCmd [CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E/
                                        CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E].
            Expected: GT_OK
        */
        /* call with udbErrorCmd = CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E; */
        udbErrorCmd = CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E;

        st = cpssDxChPclInvalidUdbCmdSet(dev, udbErrorCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "dev: %d, %d", dev, udbErrorCmd);

        /* call with udbErrorCmd = CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E; */
        udbErrorCmd = CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E;

        st = cpssDxChPclInvalidUdbCmdSet(dev, udbErrorCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "dev: %d, %d", dev, udbErrorCmd);

        /*
            1.2. Check for invalid enum value for udbErrorCmd.  Call with
            udbErrorCmd [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclInvalidUdbCmdSet
                            (dev, udbErrorCmd),
                            udbErrorCmd);
    }

    udbErrorCmd = CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclInvalidUdbCmdSet(dev, udbErrorCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclInvalidUdbCmdSet(dev, udbErrorCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclIpLengthCheckModeSet
(
    IN GT_U8                                   devNum,
    IN CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclIpLengthCheckModeSet)
{
/*
    ITERATE_DEVICES
    1.1. Call with mode [CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E /
                         CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L3_E]
    Expected: GT_OK
    1.2. Check for invalid enum value for mode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                               st = GT_OK;
    GT_U8                                   dev;
    CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT  mode;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E /
                                 CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L3_E]
            Expected: GT_OK
        */
        /* call with mode = CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E; */
        mode = CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E;

        st = cpssDxChPclIpLengthCheckModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "dev: %d, %d", dev, mode);

        /* call with mode = CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L3_E; */
        mode = CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L3_E;

        st = cpssDxChPclIpLengthCheckModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "dev: %d, %d", dev, mode);

        /*
            1.2. Check for invalid enum value for mode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclIpLengthCheckModeSet
                            (dev, mode),
                            mode);
    }

    mode = CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclIpLengthCheckModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclIpLengthCheckModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclCfgTblEntryGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN  GT_U32                          entryIndex,
    OUT GT_U32                          *pclCfgTblEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclCfgTblEntryGet)
{
/*
    ITERATE_DEVICES
    1.1. Call with direction [CPSS_PCL_DIRECTION_INGRESS_E /
                              CPSS_PCL_DIRECTION_EGRESS_E],
                   lookupNum [CPSS_PCL_LOOKUP_0_E /
                              CPSS_PCL_LOOKUP_1_E],
        entryIndex [0 / tableSize - 1] and non-NULL pclCfgTblEntryPtr.
    Expected: GT_OK
    1.2. Call with direction [CPSS_PCL_DIRECTION_EGRESS_E],
                   lookupNum [CPSS_PCL_LOOKUP_1_E],
        entryIndex [tableSize - 1] and non-NULL pclCfgTblEntryPtr.
    Expected: GT_OK for ch2/ch3 (egress not supported in ch1,
                                 ch2/ch3 lookupNum not relevant)
              and not GT_OK for ch1/xCat and above (wrong lookupNum for egress direction)
    1.2. Call with direction [CPSS_PCL_DIRECTION_EGRESS_E], entryIndex [7]
            and non-NULL pclTblEntryPtr.
    Expected: NON GT_OK for DxCh devices (they don't support egress direction)
            and GT_OK for Cheetah2,3.
    1.3. Check for invalid enum value for direction.
    Expected: GT_BAD_PARAM.
        1.4. Check wrong enum values lookupNum.
        Expected: GT_BAD_PARAM for xCat and above.
    1.5. Call with pclCfgTblEntryPtr [NULL] and other parameters as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                         st = GT_OK;
    GT_U8                             dev;
    CPSS_PCL_DIRECTION_ENT            direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT        lookupNum = CPSS_PCL_RULE_SIZE_STD_E;
    GT_U32                            entryIndex;

    /*!!! will pclCfgTblEntry fit in 4 bytes??? - answer - yes              */
    /*!!! (for now because of stubs we don't know memory size it consumes..)*/
    /*  PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E          - 1 word of data  */
    /*  PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E  - 1 word of data  */
    GT_U32                            pclCfgTblEntry[4];
    CPSS_PP_FAMILY_TYPE_ENT           devFamily;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /*
            1.1. Call with direction [CPSS_PCL_DIRECTION_INGRESS_E /
                                      CPSS_PCL_DIRECTION_EGRESS_E],
                           lookupNum [CPSS_PCL_LOOKUP_0_E /
                                      CPSS_PCL_LOOKUP_1_E],
                entryIndex [0 / tableSize - 1] and non-NULL pclCfgTblEntryPtr.
            Expected: GT_OK
        */
        /* call with direction = CPSS_PCL_DIRECTION_INGRESS_E; */
        direction = CPSS_PCL_DIRECTION_INGRESS_E;

        /* call with lookupNum = CPSS_PCL_LOOKUP_0_E; */
        lookupNum = CPSS_PCL_LOOKUP_0_E;
        entryIndex = 0;

        st = cpssDxChPclCfgTblEntryGet(dev, direction, lookupNum, entryIndex, pclCfgTblEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "dev: %d, %d, %d", dev, direction, entryIndex);

        /* call with lookupNum = CPSS_PCL_LOOKUP_1_E; */
        lookupNum = CPSS_PCL_LOOKUP_1_E;
        entryIndex = PCL_TCAM_RAWS(dev) - 1;

        st = cpssDxChPclCfgTblEntryGet(dev, direction, lookupNum, entryIndex, pclCfgTblEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "dev: %d, %d, %d", dev, direction, entryIndex);

        /* call with direction = CPSS_PCL_DIRECTION_EGRESS_E; */
        direction = CPSS_PCL_DIRECTION_EGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_0_E;
        entryIndex = 0;

        st = cpssDxChPclCfgTblEntryGet(dev, direction, lookupNum, entryIndex, pclCfgTblEntry);
        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(
                GT_OK, st, "dev: %d, %d, %d", dev, direction, entryIndex);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, direction);
        }

        /*
            1.2. Call with direction [CPSS_PCL_DIRECTION_EGRESS_E],
                           lookupNum [CPSS_PCL_LOOKUP_1_E],
                entryIndex [tableSize - 1] and non-NULL pclCfgTblEntryPtr.
            Expected: GT_OK for ch2/ch3 (egress not supported in ch1,
                                         ch2/ch3 lookupNum not relevant)
                      and not GT_OK for ch1/xCat and above (wrong lookupNum for egress direction)
        */
        direction = CPSS_PCL_DIRECTION_EGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_1_E;
        entryIndex = PCL_TCAM_RAWS(dev) - 1;

        st = cpssDxChPclCfgTblEntryGet(dev, direction, lookupNum, entryIndex, pclCfgTblEntry);

        if (devFamily == CPSS_PP_FAMILY_CHEETAH2_E ||
            devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, direction, entryIndex);
        }
        else if (devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E ||
                 devFamily == CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, direction);
        }

        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_0_E;
        entryIndex = 0;

        /*
            1.2. Call with direction [CPSS_PCL_DIRECTION_EGRESS_E], entryIndex [7]
                    and non-NULL pclTblEntryPtr.
            Expected: NON GT_OK for DxCh devices (they don't support egress direction)
                    and GT_OK for Cheetah2,3.
        */
        direction = CPSS_PCL_DIRECTION_EGRESS_E;
        entryIndex = 7;

        st = cpssDxChPclCfgTblEntryGet(dev, direction, lookupNum, entryIndex, pclCfgTblEntry);

        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, direction, entryIndex);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, direction);
        }

        /*
            1.3. Check for invalid enum value for direction.
            Expected: GT_BAD_PARAM.
        */
        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_0_E;
        entryIndex = 0;

        UTF_ENUMS_CHECK_MAC(cpssDxChPclCfgTblEntryGet
                            (dev, direction, lookupNum, entryIndex, pclCfgTblEntry),
                            direction);

        if (devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            /*
                1.4. Check wrong enum values lookupNum.
                Expected: GT_BAD_PARAM for xCat and above.
            */
            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            entryIndex = 0;

            UTF_ENUMS_CHECK_MAC(cpssDxChPclCfgTblEntryGet
                                (dev, direction, lookupNum, entryIndex, pclCfgTblEntry),
                                lookupNum);
        }

        /*
            1.5. Call with pclCfgTblEntryPtr [NULL] and other parameters as in 1.1.
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChPclCfgTblEntryGet( dev, direction, lookupNum, entryIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                        "dev: %d, pclCfgTblEntryPtr = NULL", dev);
    }

    /* set correct values for all the parameters (except of device number) */
    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    entryIndex = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclCfgTblEntryGet(dev, direction, lookupNum,
                                        entryIndex, pclCfgTblEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclCfgTblEntryGet(dev, direction, lookupNum,
                                        entryIndex, pclCfgTblEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxCh2PclEgressPolicyEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxCh2PclEgressPolicyEnable)
{
/*
    ITERATE_DEVICES
    1.1. Call with enable [GT_TRUE/ GT_FALSE].
    Expected: GT_OK
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_BOOL                     enable;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_TRUE/ GT_FALSE].
            Expected: GT_OK
        */
        /* call with enable = GT_TRUE; */
        enable = GT_TRUE;

        st = cpssDxCh2PclEgressPolicyEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* call with enable = GT_FALSE; */
        enable = GT_FALSE;

        st = cpssDxCh2PclEgressPolicyEnable(dev, enable);
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(dev,
                      PRV_CPSS_DXCH_XCAT2_EPCL_GLOBAL_EN_NOT_FUNCTIONAL_WA_E))
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, enable);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
        }
    }

    enable = GT_FALSE;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxCh2PclEgressPolicyEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxCh2PclEgressPolicyEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclEgressForRemoteTunnelStartEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_BOOL                       enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclEgressForRemoteTunnelStartEnableSet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with  enable [GT_TRUE / GT_FALSE]
    Expected: GT_OK.
    1.2 Call cpssDxChPclEgressForRemoteTunnelStartEnableGet
        with non-NULL enableGet.
    Expected: GT_OK and the same enableGet.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with  enable [GT_TRUE / GT_FALSE]
            Expected: GT_OK.
        */
        /* call with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxChPclEgressForRemoteTunnelStartEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2 Call cpssDxChPclEgressForRemoteTunnelStartEnableGet
                with non-NULL enableGet.
            Expected: GT_OK and the same enableGet.
        */
        st = cpssDxChPclEgressForRemoteTunnelStartEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                "cpssDxChPclEgressForRemoteTunnelStartEnableGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                "get another enable than was set: %d", dev);

        /* call with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssDxChPclEgressForRemoteTunnelStartEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2 Call cpssDxChPclEgressForRemoteTunnelStartEnableGet
                with non-NULL enableGet;
            Expected: GT_OK and the same enableGet.
        */
        st = cpssDxChPclEgressForRemoteTunnelStartEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                "cpssDxChPclEgressForRemoteTunnelStartEnableGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                "get another enable than was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclEgressForRemoteTunnelStartEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclEgressForRemoteTunnelStartEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclEgressForRemoteTunnelStartEnableGet
(
    IN  GT_U8                         devNum,
    OUT GT_BOOL                       *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclEgressForRemoteTunnelStartEnableGet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with non-NULL enablePtr
    Expected: GT_OK.
    1.2 Call with enablePtr [NULL];
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     enable    = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL enablePtr
            Expected: GT_OK.
        */
        st = cpssDxChPclEgressForRemoteTunnelStartEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2 Call with enablePtr [NULL];
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclEgressForRemoteTunnelStartEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclEgressForRemoteTunnelStartEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclEgressForRemoteTunnelStartEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxCh3PclTunnelTermForceVlanModeEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_BOOL                       enable
)
*/
UTF_TEST_CASE_MAC(cpssDxCh3PclTunnelTermForceVlanModeEnableSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxCh3PclTunnelTermForceVlanModeEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with  enable [GT_TRUE / GT_FALSE]
            Expected: GT_OK.
        */
        /* call with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxCh3PclTunnelTermForceVlanModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2 Call cpssDxCh3PclTunnelTermForceVlanModeEnableGet with non-NULL enablePtr;
            Expected: GT_OK and the same enable.
        */
        st = cpssDxCh3PclTunnelTermForceVlanModeEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxCh3PclTunnelTermForceVlanModeEnableGet: %d", dev);

        /* Verifying values*/
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /* call with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssDxCh3PclTunnelTermForceVlanModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2 Call cpssDxCh3PclTunnelTermForceVlanModeEnableGet with non-NULL enablePtr;
            Expected: GT_OK and the same enable.
        */
        st = cpssDxCh3PclTunnelTermForceVlanModeEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxCh3PclTunnelTermForceVlanModeEnableGet: %d", dev);

        /* Verifying values*/
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxCh3PclTunnelTermForceVlanModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxCh3PclTunnelTermForceVlanModeEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxCh3PclTunnelTermForceVlanModeEnableGet
(
    IN  GT_U8                         devNum,
    OUT GT_BOOL                       *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxCh3PclTunnelTermForceVlanModeEnableGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL enablePtr
            Expected: GT_OK.
        */
        st = cpssDxCh3PclTunnelTermForceVlanModeEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2 Call with enablePtr [NULL];
            Expected: GT_BAD_PTR.
        */
        st = cpssDxCh3PclTunnelTermForceVlanModeEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxCh3PclTunnelTermForceVlanModeEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxCh3PclTunnelTermForceVlanModeEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxCh2EgressPclPacketTypesSet
(
    IN GT_U8                             devNum,
    IN GT_U8                             port,
    IN CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT pktType,
    IN GT_BOOL                           enable
)
*/
UTF_TEST_CASE_MAC(cpssDxCh2EgressPclPacketTypesSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (dxCh2 and above)
    1.1.1. Call with pktType
                    [CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E/
                    CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E/
                    CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E] and
                    enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK
    1.1.3. Check for invalid enum value for packet type.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                               dev;
    GT_U8                               port;
    CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT   pktType;
    GT_BOOL                             enable;

    GT_U16                              maxMcGroups;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfMaxMcGroupsGet(dev, &maxMcGroups);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMaxMcGroupsGet: %d", dev);

            /*
                1.1.1. Call with pktType
                                [CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E/
                                CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E/
                                CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E] and
                                enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK
            */
            /* call with enable = GT_FALSE; */
            enable = GT_FALSE;

            /* call with pktType = CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E; */
            pktType = CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E;

            st = cpssDxCh2EgressPclPacketTypesSet(dev, port, pktType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, pktType, enable);

            /* call with pktType = CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E; */
            pktType = CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E;

            st = cpssDxCh2EgressPclPacketTypesSet(dev, port, pktType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, pktType, enable);

            /* call with pktType = CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E; */
            pktType = CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E;

            st = cpssDxCh2EgressPclPacketTypesSet(dev, port, pktType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, pktType, enable);

            /* call with enable = GT_TRUE; */
            enable = GT_TRUE;

            /* call with pktType = CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E; */
            pktType = CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E;

            st = cpssDxCh2EgressPclPacketTypesSet(dev, port, pktType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, pktType, enable);

            /* call with pktType = CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E; */
            pktType = CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E;

            st = cpssDxCh2EgressPclPacketTypesSet(dev, port, pktType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, pktType, enable);

            /* call with pktType = CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E; */
            pktType = CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E;

            st = cpssDxCh2EgressPclPacketTypesSet(dev, port, pktType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, pktType, enable);

            /*
                1.1.3. Check for invalid enum value for packet type.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxCh2EgressPclPacketTypesSet
                                (dev, port, pktType, enable),
                                pktType);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */

        pktType = CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E;
        enable = GT_FALSE;

        st = cpssDxCh2EgressPclPacketTypesSet(dev, port, pktType, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
            "dev: %d, %d (CPU port), %d, %d", dev, port, pktType, enable);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            pktType = CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E;
            enable = GT_FALSE;

            st = cpssDxCh2EgressPclPacketTypesSet(dev, port, pktType, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        pktType = CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E;
        enable = GT_FALSE;

        st = cpssDxCh2EgressPclPacketTypesSet(dev, port, pktType, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "dev: %d, %d", dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = DXCH_PCL_VALID_PHY_PORT_CNS;
    pktType = CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E;
    enable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxCh2EgressPclPacketTypesSet(dev, port, pktType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxCh2EgressPclPacketTypesSet(dev, port, pktType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
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
*/
UTF_TEST_CASE_MAC(cpssDxCh2PclTcpUdpPortComparatorSet)
{
/*
    ITERATE_DEVICES
    1.1. Call with direction [CPSS_PCL_DIRECTION_INGRESS_E],
            l4Protocol [CPSS_L4_PROTOCOL_TCP_E], entryIndex [0],
            l4PortType [CPSS_L4_PROTOCOL_PORT_SRC_E],
            compareOperator [CPSS_COMPARE_OPERATOR_LTE], value [25].
    Expected: GT_OK
    1.2. Call with direction [CPSS_PCL_DIRECTION_EGRESS_E],
            l4Protocol [CPSS_L4_PROTOCOL_UDP_E], entryIndex [7],
            l4PortType [CPSS_L4_PROTOCOL_PORT_DST_E],
            compareOperator [CPSS_COMPARE_OPERATOR_NEQ], value [25].
    Expected: GT_OK
    1.3. Check for invalid enum value for direction.
    Expected: GT_BAD_PARAM.
    1.4. Check for invalid enum value for l4Protocol.
    Expected: GT_BAD_PARAM.
    1.5. Check for out of range entryIndex. Call
    function with entryIndex [8] and other parameters as in 1.2.
    Expected: NON GT_OK.
    1.6. Check for invalid enum value for l4Port.
    Expected: GT_BAD_PARAM.
    1.7. Check for invalid enum value for compareOperator.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                         st = GT_OK;
    GT_U8                             dev;
    CPSS_PCL_DIRECTION_ENT            direction;
    CPSS_L4_PROTOCOL_ENT              l4Protocol;
    GT_U8                             entryIndex;
    CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType;
    CPSS_COMPARE_OPERATOR_ENT         compareOperator;
    GT_U16                            value;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with direction [CPSS_PCL_DIRECTION_INGRESS_E],
                    l4Protocol [CPSS_L4_PROTOCOL_TCP_E], entryIndex [0],
                    l4PortType [CPSS_L4_PROTOCOL_PORT_SRC_E],
                    compareOperator [CPSS_COMPARE_OPERATOR_LTE], value [25].
            Expected: GT_OK
        */
        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        l4Protocol = CPSS_L4_PROTOCOL_TCP_E;
        entryIndex = 0;
        l4PortType = CPSS_L4_PROTOCOL_PORT_SRC_E;
        compareOperator = CPSS_COMPARE_OPERATOR_LTE;
        value = 25;

        st = cpssDxCh2PclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                            entryIndex, l4PortType, compareOperator, value);

        UTF_VERIFY_EQUAL7_STRING_MAC(GT_OK, st, "dev: %d, %d, %d, %d, %d, %d, %d",
                        dev, direction, l4Protocol, entryIndex, l4PortType,
                        compareOperator, value);

        /*
            1.2. Call with direction [CPSS_PCL_DIRECTION_EGRESS_E],
                    l4Protocol [CPSS_L4_PROTOCOL_UDP_E], entryIndex [7],
                    l4PortType [CPSS_L4_PROTOCOL_PORT_DST_E],
                    compareOperator [CPSS_COMPARE_OPERATOR_NEQ], value [25].
            Expected: GT_OK
        */
        direction = CPSS_PCL_DIRECTION_EGRESS_E;
        l4Protocol = CPSS_L4_PROTOCOL_UDP_E;
        entryIndex = 7;
        l4PortType = CPSS_L4_PROTOCOL_PORT_DST_E;
        compareOperator = CPSS_COMPARE_OPERATOR_NEQ;

        st = cpssDxCh2PclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                            entryIndex, l4PortType, compareOperator, value);

        UTF_VERIFY_EQUAL7_STRING_MAC(GT_OK, st, "dev: %d, %d, %d, %d, %d, %d, %d",
                        dev, direction, l4Protocol, entryIndex, l4PortType,
                        compareOperator, value);

        /*
            1.3. Check for invalid enum value for direction.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxCh2PclTcpUdpPortComparatorSet
                            (dev, direction, l4Protocol, entryIndex,
                             l4PortType, compareOperator, value),
                            direction);

        /*
            1.4. Check for invalid enum value for l4Protocol.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxCh2PclTcpUdpPortComparatorSet
                            (dev, direction, l4Protocol, entryIndex,
                             l4PortType, compareOperator, value),
                            l4Protocol);

        /*
            1.5. Check for out of range entryIndex. Call
            function with entryIndex [8] and other parameters as in 1.2.
            Expected: NON GT_OK.
        */
        entryIndex = 8;

        st = cpssDxCh2PclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                            entryIndex, l4PortType, compareOperator, value);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                        "dev: %d, entryIndex = %d", dev, entryIndex);

        entryIndex = 7;

        /*
            1.6. Check for invalid enum value for l4Port.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxCh2PclTcpUdpPortComparatorSet
                            (dev, direction, l4Protocol, entryIndex,
                             l4PortType, compareOperator, value),
                            l4PortType);

        /*
            1.7. Check for invalid enum value for compareOperator.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxCh2PclTcpUdpPortComparatorSet
                            (dev, direction, l4Protocol, entryIndex,
                             l4PortType, compareOperator, value),
                            compareOperator);
    }

    /* set correct values for all the parameters (except of device number) */
    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    l4Protocol = CPSS_L4_PROTOCOL_TCP_E;
    entryIndex = 0;
    l4PortType = CPSS_L4_PROTOCOL_PORT_SRC_E;
    compareOperator = CPSS_COMPARE_OPERATOR_LTE;
    value = 25;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxCh2PclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                            entryIndex, l4PortType, compareOperator, value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxCh2PclTcpUdpPortComparatorSet(dev, direction, l4Protocol,
                        entryIndex, l4PortType, compareOperator, value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
(
    IN  GT_U8                                            devNum,
    OUT CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT   *cfgTabAccModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet)
{
/*
    ITERATE_DEVICES (dxCh3 and above)
    1.1. Call with non-NULL cfgTabAccModePtr
    Expected: GT_OK.
    1.2 Call with cfgTabAccModePtr [NULL];
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT   cfgTabAccModePtr;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL cfgTabAccModePtr
            Expected: GT_OK.
        */
        st = cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet(dev, &cfgTabAccModePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2 Call with cfgTabAccModePtr [NULL];
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cfgTabAccModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet(dev, &cfgTabAccModePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet(dev, &cfgTabAccModePtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet
(
    IN  GT_U8                                            devNum,
    IN  CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT   cfgTabAccMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet)
{
/*
    ITERATE_DEVICES
    1.1. Call with cfgTabAccMode [CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E/
                                  CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E]
    Expected: GT_OK.
    1.2. Call with wrong enum values cfgTabAccMode.
    Expected: GT_BAD_PARAM.
    1.3. Call cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
         with non-NULL cfgTabAccMode.
    Expected: GT_OK and the same cfgTabAccMode.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT cfgTabAccMode;
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT cfgTabAccModeGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cfgTabAccMode [CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E/
                                          CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E]
            Expected: GT_OK.
        */

        /* call with cfgTabAccMode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E */
        cfgTabAccMode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E;

        st = cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet(dev, cfgTabAccMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cfgTabAccMode);

        /* call with cfgTabAccMode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E */
        cfgTabAccMode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E;

        st = cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet(dev, cfgTabAccMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cfgTabAccMode);

        /*
            1.2. Call with wrong enum values cfgTabAccMode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet
                            (dev, cfgTabAccMode),
                            cfgTabAccMode);

        /*
            1.3. Call cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
            with non-NULL cfgTabAccMode;
            Expected: GT_OK and the same cfgTabAccMode.
        */
        cfgTabAccMode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E;

        st = cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet(dev, &cfgTabAccModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxCh3PclTunnelTermForceVlanModecfgTabAccModeGet: %d", dev);

        /* Verifying values*/
        UTF_VERIFY_EQUAL1_STRING_MAC(cfgTabAccMode, cfgTabAccModeGet,
                   "get another cfgTabAccMode than was set: %d", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    cfgTabAccMode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E;

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet(dev, cfgTabAccMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet(dev, cfgTabAccMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclOverrideUserDefinedBytesGet
(
    IN  GT_U8                          devNum,
    OUT CPSS_DXCH_PCL_OVERRIDE_UDB_STC *udbOverridePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclOverrideUserDefinedBytesGet)
{
/*
    ITERATE_DEVICES
    1.1. Call with non-NULL udbOverridePtr.
    Expected: GT_OK.
    1.2 Call with udbOverridePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                      st = GT_OK;
    GT_U8                          dev;
    CPSS_DXCH_PCL_OVERRIDE_UDB_STC udbOverridePtr;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL udbOverridePtr
            Expected: GT_OK.
        */
        st = cpssDxChPclOverrideUserDefinedBytesGet(dev, &udbOverridePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2 Call with udbOverridePtr [NULL];
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclOverrideUserDefinedBytesGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, udbOverridePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclOverrideUserDefinedBytesGet(dev, &udbOverridePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclOverrideUserDefinedBytesGet(dev, &udbOverridePtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclOverrideUserDefinedBytesSet
(
    IN  GT_U8                            devNum,
    IN  CPSS_DXCH_PCL_OVERRIDE_UDB_STC   udbOverride
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclOverrideUserDefinedBytesSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with udbOverride GT_TRUE/FALSE and mixed.
    Expected: GT_OK.
    1.2. Call cpssDxChPclOverrideUserDefinedBytesGet
         with non-NULL udbOverride.
    Expected: GT_OK and the same udbOverride.
    1.3. Call with wrong udbOverridePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     isEqual  = GT_FALSE;
    GT_BOOL     vrfIdUse  = GT_FALSE;
    CPSS_PP_FAMILY_TYPE_ENT             devFamily;

    CPSS_DXCH_PCL_OVERRIDE_UDB_STC udbOverride = {0};
    CPSS_DXCH_PCL_OVERRIDE_UDB_STC udbOverrideGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        /*
            1.1. Call with udbOverride GT_TRUE/FALSE and mixed.
            Expected: GT_OK.
        */
        /* Call with udbOverride all GT_FALSE */
        st = cpssDxChPclOverrideUserDefinedBytesSet(dev, &udbOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPclOverrideUserDefinedBytesGet
                 with non-NULL udbOverride.
            Expected: GT_OK and the same udbOverride.
        */
        st = cpssDxChPclOverrideUserDefinedBytesGet(dev, &udbOverrideGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChPclOverrideUserDefinedBytesGet: %d", dev);

        /* Verifying values*/
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &udbOverride,
                                     (GT_VOID*) &udbOverrideGet,
                                     sizeof(udbOverride))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another udbOverride then was set: %d", dev);

        if (devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            vrfIdUse = GT_FALSE;
        }
        else
        {
            vrfIdUse = GT_TRUE;
        }


        /* Call with udbOverride all GT_TRUE*/
        udbOverride.vrfIdLsbEnableStdNotIp   = vrfIdUse;
        udbOverride.vrfIdMsbEnableStdNotIp   = vrfIdUse;
        udbOverride.vrfIdLsbEnableStdIpL2Qos = vrfIdUse;
        udbOverride.vrfIdMsbEnableStdIpL2Qos = vrfIdUse;
        udbOverride.vrfIdLsbEnableStdIpv4L4  = vrfIdUse;
        udbOverride.vrfIdMsbEnableStdIpv4L4  = vrfIdUse;
        udbOverride.vrfIdLsbEnableStdUdb     = vrfIdUse;
        udbOverride.vrfIdMsbEnableStdUdb     = vrfIdUse;
        udbOverride.vrfIdLsbEnableExtNotIpv6 = vrfIdUse;
        udbOverride.vrfIdMsbEnableExtNotIpv6 = vrfIdUse;
        udbOverride.vrfIdLsbEnableExtIpv6L2  = vrfIdUse;
        udbOverride.vrfIdMsbEnableExtIpv6L2  = vrfIdUse;
        udbOverride.vrfIdLsbEnableExtIpv6L4  = vrfIdUse;
        udbOverride.vrfIdMsbEnableExtIpv6L4  = vrfIdUse;
        udbOverride.vrfIdLsbEnableExtUdb     = vrfIdUse;
        udbOverride.vrfIdMsbEnableExtUdb     = vrfIdUse;
        udbOverride.qosProfileEnableStdUdb   = GT_TRUE;
        udbOverride.qosProfileEnableExtUdb   = GT_TRUE;

        st = cpssDxChPclOverrideUserDefinedBytesSet(dev, &udbOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPclOverrideUserDefinedBytesGet
                 with non-NULL udbOverride.
            Expected: GT_OK and the same udbOverride.
        */
        st = cpssDxChPclOverrideUserDefinedBytesGet(dev, &udbOverrideGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChPclOverrideUserDefinedBytesGet: %d", dev);

        /* Verifying values*/
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &udbOverride,
                                     (GT_VOID*) &udbOverrideGet,
                                     sizeof(udbOverride))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another udbOverride then was set: %d", dev);


        if (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            /* Call with udbOverride all mixed*/
            udbOverride.vrfIdLsbEnableStdNotIp   = GT_TRUE;
            udbOverride.vrfIdMsbEnableStdNotIp   = GT_TRUE;
            udbOverride.vrfIdLsbEnableStdIpL2Qos = GT_FALSE;
            udbOverride.vrfIdMsbEnableStdIpL2Qos = GT_FALSE;
            udbOverride.vrfIdLsbEnableStdIpv4L4  = GT_TRUE;
            udbOverride.vrfIdMsbEnableStdIpv4L4  = GT_TRUE;
            udbOverride.vrfIdLsbEnableStdUdb     = GT_FALSE;
            udbOverride.vrfIdMsbEnableStdUdb     = GT_FALSE;
            udbOverride.vrfIdLsbEnableExtNotIpv6 = GT_TRUE;
            udbOverride.vrfIdMsbEnableExtNotIpv6 = GT_TRUE;
            udbOverride.vrfIdLsbEnableExtIpv6L2  = GT_TRUE;
            udbOverride.vrfIdMsbEnableExtIpv6L2  = GT_TRUE;
            udbOverride.vrfIdLsbEnableExtIpv6L4  = GT_TRUE;
            udbOverride.vrfIdMsbEnableExtIpv6L4  = GT_TRUE;
            udbOverride.vrfIdLsbEnableExtUdb     = GT_FALSE;
            udbOverride.vrfIdMsbEnableExtUdb     = GT_FALSE;
            udbOverride.qosProfileEnableStdUdb   = GT_TRUE;
            udbOverride.qosProfileEnableExtUdb   = GT_TRUE;

            st = cpssDxChPclOverrideUserDefinedBytesSet(dev, &udbOverride);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.2. Call cpssDxChPclOverrideUserDefinedBytesGet
                     with non-NULL udbOverride.
                Expected: GT_OK and the same udbOverride.
            */
            st = cpssDxChPclOverrideUserDefinedBytesGet(dev, &udbOverrideGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChPclOverrideUserDefinedBytesGet: %d", dev);

            /* Verifying values*/
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &udbOverride,
                                         (GT_VOID*) &udbOverrideGet,
                                         sizeof(udbOverride))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another udbOverride then was set: %d", dev);
        }

        /*
            1.3. Call with wrong udbOverridePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclOverrideUserDefinedBytesSet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclOverrideUserDefinedBytesSet(dev, &udbOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclOverrideUserDefinedBytesSet(dev, &udbOverride);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_PCL_OFFSET_TYPE_ENT        *offsetTypePtr,
    OUT GT_U8                                *offsetPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclUserDefinedByteGet)
{
/*
    ITERATE_DEVICES
    1.1. Call with ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
         packetType = CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E, udbIndex [0].
    Expected: GT_OK.
    1.2. Call with wrong enum values ruleFormat.
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong enum value spacketType.
    Expected: GT_BAD_PARAM.
    1.4. Call with offsetType = NULL (null pointer).
    Expected: GT_BAD_PTR.
    1.5. Call with offset     = NULL (null pointer).
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                            st = GT_OK;
    GT_U8                                dev;

    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat;
    CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType;
    GT_U32                               udbIndex;
    CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetTypeGet;
    GT_U8                                offsetGet;

    CPSS_PP_FAMILY_TYPE_ENT              devFamily;
    CPSS_PP_DEVICE_TYPE                  devType;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        st = prvUtfDeviceTypeGet(dev, &devType);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceTypeGet: %d", dev);

        /*
            1.1. Call with ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                 packetType = CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E, udbIndex [0].
            Expected: GT_OK.
        */
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        packetType = CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E;
        udbIndex   = 0;

        st = cpssDxChPclUserDefinedByteGet(dev, ruleFormat,
                                           CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E,
                                           udbIndex, &offsetTypeGet, &offsetGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong enum values ruleFormat.
            Expected: GT_BAD_PARAM.
        */

        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChPclUserDefinedByteGet
                                (dev, ruleFormat, packetType,
                                 udbIndex, &offsetTypeGet, &offsetGet),
                                ruleFormat);
        }
        else
        {
            for(enumsIndex = 0; enumsIndex < utfInvalidEnumArrSize; enumsIndex++)
            {
                ruleFormat = utfInvalidEnumArr[enumsIndex];

                st = cpssDxChPclUserDefinedByteGet(dev, ruleFormat, packetType,
                                                   udbIndex, &offsetTypeGet, &offsetGet);
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                            offsetTypeGet, offsetGet);
            }
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        }

        /*
            1.3. Call with wrong enum values packetType.
            Expected: GT_BAD_PARAM.
        */
        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            for(enumsIndex = 0; enumsIndex < utfInvalidEnumArrSize; enumsIndex++)
            {
                packetType = utfInvalidEnumArr[enumsIndex];

                st = cpssDxChPclUserDefinedByteGet(dev, ruleFormat, packetType,
                                                   udbIndex, &offsetTypeGet, &offsetGet);
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
            }
            packetType = CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E;
        }
        else
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChPclUserDefinedByteGet
                                (dev, ruleFormat, packetType, udbIndex,
                                 &offsetTypeGet, &offsetGet),
                                packetType);
        }

        /*
            1.4. Call with offsetType = NULL (null pointer).
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclUserDefinedByteGet(dev, ruleFormat,
                                           CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E,
                                           udbIndex, NULL, &offsetGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.5. Call with offset = NULL (null pointer).
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclUserDefinedByteGet(dev, ruleFormat,
                                           CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E,
                                           udbIndex, &offsetTypeGet, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    udbIndex   = 0;
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclUserDefinedByteGet(dev, ruleFormat,
                                           CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E,
                                           udbIndex, &offsetTypeGet, &offsetGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st  = cpssDxChPclUserDefinedByteGet(dev, ruleFormat,
                                       CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E,
                                       udbIndex, &offsetTypeGet, &offsetGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclInit
(
    IN  GT_U8                            devNum,
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclInit)
{
/*
    ITERATE_DEVICES
    1.1. Call cpssDxChPclInit with correct dev.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssDxChPclInit with correct dev.
            Expected: GT_OK.
        */
        st = cpssDxChPclInit(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclInit(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclInit(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupCfgTblEntryGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN  GT_U32                          entryIndex,
    OUT GT_U32                          *pclCfgTblEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupCfgTblEntryGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(dxChx)
    1.1. Call with portGroupsBmp[CPSS_PORT_GROUP_UNAWARE_MODE_CNS],
        direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum [CPSS_PCL_LOOKUP_0_E],
        entryIndex [0] and non-NULL pclCfgTblEntryPtr.
    Expected: GT_OK.
    1.2. Call with direction [CPSS_PCL_DIRECTION_EGRESS_E],
        lookupNum [CPSS_PCL_LOOKUP_0_E], entryIndex [7] and non-NULL pclTblEntryPtr.
    Expected: GT_OK for all except cheetah1 (not support egress direction).
    1.3. Call with portGroupsBmp[CPSS_PORT_GROUP_UNAWARE_MODE_CNS],
        direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum [CPSS_PCL_LOOKUP_1_E],
        entryIndex [3] and non-NULL pclCfgTblEntryPtr.
    Expected: GT_OK.
    1.4. Call with portGroupsBmp[CPSS_PORT_GROUP_UNAWARE_MODE_CNS],
        direction [CPSS_PCL_DIRECTION_EGRESS_E], lookupNum [CPSS_PCL_LOOKUP_1_E],
        entryIndex [10] and non-NULL pclCfgTblEntryPtr.
    Expected: GT_OK for ch1, ch2, ch3 (lookupNum not relevant)
    and NOT GT_OK for xCat and above.
    1.5. Check for invalid enum value for direction.
    Expected: GT_BAD_PARAM.
    1.6. Check for invalid enum value for lookupNum.
    Expected: GT_BAD_PARAM.
    1.7. Check NULL pointer processing. Call with pclCfgTblEntryPtr [NULL]
        and other parameters as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                         st = GT_OK;
    GT_U8                             dev;
    GT_PORT_GROUPS_BMP                portGroupsBmp = 1;
    CPSS_PCL_DIRECTION_ENT            direction = CPSS_PCL_DIRECTION_INGRESS_E;
    CPSS_PCL_LOOKUP_NUMBER_ENT        lookupNum = CPSS_PCL_LOOKUP_0_E;
    GT_U32                            entryIndex = 0;
    GT_U32                            pclCfgTblEntry[4];

    CPSS_PP_FAMILY_TYPE_ENT           devFamily;

    GT_U32    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1. Call with portGroupsBmp,
                    direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum [CPSS_PCL_LOOKUP_0_E],
                    entryIndex [0] and non-NULL pclCfgTblEntryPtr.
                Expected: GT_OK.
            */
            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_0_E;
            entryIndex = 0;

            st = cpssDxChPclPortGroupCfgTblEntryGet(dev, portGroupsBmp, direction,
                                                lookupNum, entryIndex, pclCfgTblEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, entryIndex: %d, %d, %d", dev, direction, entryIndex);

            /*
                1.2. Call with portGroupsBmp[CPSS_PORT_GROUP_UNAWARE_MODE_CNS],
                    direction [CPSS_PCL_DIRECTION_EGRESS_E], lookupNum [CPSS_PCL_LOOKUP_0_E],
                    entryIndex [7] and non-NULL pclCfgTblEntryPtr.
                Expected: GT_OK for all except cheetah1 (not support egress direction).
            */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;
            entryIndex = 7;

            st = cpssDxChPclPortGroupCfgTblEntryGet(dev, portGroupsBmp, direction,
                                                lookupNum, entryIndex, pclCfgTblEntry);
            if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                        "dev, direction, entryIndex: %d, %d, %d", dev, direction, entryIndex);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                        "dev, direction, entryIndex: %d, %d, %d", dev, direction, entryIndex);
            }

            /*
                1.3. Call with portGroupsBmp[CPSS_PORT_GROUP_UNAWARE_MODE_CNS],
                    direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum [CPSS_PCL_LOOKUP_1_E],
                    entryIndex [3] and non-NULL pclCfgTblEntryPtr.
                Expected: GT_OK.
            */
            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_1_E;
            entryIndex = 3;

            st = cpssDxChPclPortGroupCfgTblEntryGet(dev, portGroupsBmp, direction,
                                                lookupNum, entryIndex, pclCfgTblEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "dev, direction, entryIndex: %d, %d, %d", dev, direction, entryIndex);

            /*
                1.4. Call with portGroupsBmp[CPSS_PORT_GROUP_UNAWARE_MODE_CNS],
                    direction [CPSS_PCL_DIRECTION_EGRESS_E], lookupNum [CPSS_PCL_LOOKUP_1_E],
                    entryIndex [10] and non-NULL pclCfgTblEntryPtr.
                Expected: GT_OK for ch1, ch2, ch3 (lookupNum not relevant)
                and NOT GT_OK for xCat and above.
            */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_1_E;
            entryIndex = 10;

            st = cpssDxChPclPortGroupCfgTblEntryGet(dev, portGroupsBmp, direction,
                                                lookupNum, entryIndex, pclCfgTblEntry);
            if ((devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
                || (devFamily == CPSS_PP_FAMILY_CHEETAH_E))
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                        "dev, direction, entryIndex: %d, %d, %d", dev, direction, entryIndex);
            }
            else
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                        "dev, direction, entryIndex: %d, %d, %d", dev, direction, entryIndex);
            }

            lookupNum = CPSS_PCL_LOOKUP_0_E;

            /*
                1.5. Check for invalid enum value for direction.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupCfgTblEntryGet
                                (dev, portGroupsBmp, direction, lookupNum,
                                 entryIndex, pclCfgTblEntry),
                                direction);

            /*
                1.6. Check wrong enum values lookupNum.
                Expected: GT_BAD_PARAM for xCat and above (not relevant for other).
            */
            if (devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
            {
                UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupCfgTblEntryGet
                                (dev, portGroupsBmp, direction, lookupNum,
                                 entryIndex, pclCfgTblEntry),
                                 lookupNum);
            }

            /*
                1.6. Check wrong pclCfgTblEntryPtr[NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupCfgTblEntryGet(dev, portGroupsBmp, direction,
                                                lookupNum, entryIndex, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                    "device: %d, pclCfgTblEntryPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupCfgTblEntryGet(dev, portGroupsBmp, direction,
                                                lookupNum, entryIndex, pclCfgTblEntry);

            if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev)
                ||
                ((devFamily == CPSS_PP_FAMILY_CHEETAH_E)
                 &&
                 (direction != CPSS_PCL_DIRECTION_INGRESS_E)))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }

        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupCfgTblEntryGet(dev, portGroupsBmp, direction,
                                                lookupNum, entryIndex, pclCfgTblEntry);
        if ((devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            && (direction != CPSS_PCL_DIRECTION_INGRESS_E))
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
        }
    }

    portGroupsBmp = 1;

    /* set correct values for all the parameters (except of device number) */
    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    entryIndex = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. Call function for non active device and valid parameters. */
        /* Expected: GT_BAD_PARAM.                                        */
        st = cpssDxChPclPortGroupCfgTblEntryGet(dev, portGroupsBmp, direction,
                                            lookupNum, entryIndex, pclCfgTblEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupCfgTblEntryGet(dev, portGroupsBmp, direction,
                                        lookupNum, entryIndex, pclCfgTblEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupCfgTblSet
(
    IN GT_U8                           devNum,
    IN GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN CPSS_INTERFACE_INFO_STC         *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT          direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupCfgTblSet)
{
/*
I - INTERFACE_TYPE - Port (interfaceInfoPtr->type= CPSS_INTERFACE_PORT_E)

    ITERATE_DEVICES_PORT_GROUPS(dxChx)
    1.1. Check Ingress direction. For all devices call with
    portGroupsBmp [CPSS_PORT_GROUP_UNAWARE_MODE_CNS],
    interfaceInfoPtr [type = CPSS_INTERFACE_PORT_E, devPort {devNum=0, portNum=0} ],
    direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum [CPSS_PCL_LOOKUP_0_E], lookupCfgPtr
    [enableLookup = GT_TRUE, groupKeyTypes.ipv4Key =
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E , pclId = 1023, groupKeyTypes.ipv6Key
    = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E, other fields zeroed } ].
    Expected: GT_OK.

    1.2. Call cpssDxChPclPortGroupCfgTblGet with interfaceInfoPtr,
    direction, lookupNum - same as in 1.1, non-NULL lookupCfgPtr.
    Expected: GT_OK and same lookupCfgPtr as was written.

    1.3. Check Egress direction (not supported by Cheetah1).
    Call with direction [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.1.
    Expected: GT_OK for all devices except Cheetah.

    1.4. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E], other params same as in 1.1.
    Expected: GT_OK.

II - INTERFACE_TYPE - VLan (interfaceInfoPtr->type= CPSS_INTERFACE_VID_E)

    1.5. Check Ingress direction. For all devices call with interfaceInfoPtr [type =
    CPSS_INTERFACE_VID_E, vlanId = [100  / (PRV_CPSS_MAX_NUM_VLANS_CNS - 1) = 4095]],
    direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum [CPSS_PCL_LOOKUP_0_E],
    lookupCfgPtr [enableLookup = GT_TRUE, pclId = 1023, groupKeyTypes.ipv4Key =
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E, groupKeyTypes.ipv6Key =
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E, other fields zeroed } ].
    Expected: GT_OK for all devices except ch1.

    1.6. Call cpssDxChPclPortGroupCfgTblGet with interfaceInfoPtr,
    direction, lookupNum - same as in 1.5, non-NULL lookupCfgPtr.
    Expected: GT_OK and same lookupCfgPtr as was written.

    1.7. Check Egress direction (not supported by Cheetah1).
    call with direction [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.5.
    Expected: GT_OK for all devices except Cheetah.

    1.8. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E],
    other params same as in 1.5.
    Expected: GT_OK.

    1.9. Check wrong direction enum values. Other params same as in 1.5.
    Expected: GT_BAD_PARAM.

    1.10. Check wrong lookupNum enum values.  Other params same as in 1.5.
    Expected: GT_BAD_PARAM.

    1.11. Check for NULL pointer support. Call with
    lookupCfgPtr [NULL], other params same as in 1.5.
    Expected: GT_BAD_PTR.

    1.12. Check for NULL pointer support. Call with
    interfaceInfoPtr [NULL], other params same as in 1.5.
    Expected: GT_BAD_PTR.

    1.13. Check out-of-range vlan id. Call with interfaceInfoPtr [type =
    CPSS_INTERFACE_VID_E, vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] ],
    other params same as in 1.5.
    Expected: non GT_OK.

III - INTERFACE_TYPE - TRUNK (interfaceInfoPtr->type= CPSS_INTERFACE_TRUNK_E)

    1.14. Check trunk interface with valid params. For all devices call with
    interfaceInfoPtr [type = CPSS_INTERFACE_TRUNK_E, trunkId = 0 ], direction
    [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum [CPSS_PCL_LOOKUP_0_E], lookupCfgPtr
    [enableLookup = GT_TRUE, pclId = 1023, groupKeyTypes {nonIpKey =
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E, ipv4Key =
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E, ipv6Key =
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E } ].
    Expected: NOT GT_OK.

    1.15. Check out-of-range trunk id. Call with interfaceInfoPtr
    [type = CPSS_INTERFACE_TRUNK_E, trunkId [MAX_TRUNK_ID_CNS = 128] ],
    other params same as in 1.14.
    Expected: non GT_OK.

IV - INTERFACE_TYPE - Vidx (unsupported) (interfaceInfoPtr->type= CPSS_INTERFACE_VIDX_E)

    1.16. Check Vidx interface (unsupported) with valid params.
    Call with interfaceInfoPtr [type = CPSS_INTERFACE_VIDX_E, vidx = 0 ], other
    params same as in 1.1.
    Expected: non GT_OK.

V - INTERFACE_TYPE - out-of-range

    1.17. Check out-of-range interface type enum. Call with
    interfaceInfoPtr [type = wrong enum values, devPort {devNum=0, portNum=0}, trunkId =
    0, vidx = 0, vlanId = 100 ], other params same as in 1.1.
    Expected: GT_BAD_PARAM.

VI - additional checks

    1.18. Check Ingress direction. For all devices call with
    portGroupsBmp [CPSS_PORT_GROUP_UNAWARE_MODE_CNS],
    interfaceInfoPtr [type = CPSS_INTERFACE_PORT_E, devPort {devNum=0, portNum=0} ],
    direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum [CPSS_PCL_LOOKUP_1_E], lookupCfgPtr
    [enableLookup = GT_TRUE, groupKeyTypes.ipv4Key =
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E, pclId = 1023, groupKeyTypes.ipv6Key
    = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E, other fields zeroed } ].
    Expected: GT_OK for ch1 and not GT_OK for other (not supported STD IPV6 DIP).

    1.19. Check Ingress direction. For all devices call with
    portGroupsBmp [CPSS_PORT_GROUP_UNAWARE_MODE_CNS],
    interfaceInfoPtr [type = CPSS_INTERFACE_PORT_E, devPort {devNum=0, portNum=0} ],
    direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum [CPSS_PCL_LOOKUP_1_E], lookupCfgPtr
    [enableLookup = GT_TRUE, groupKeyTypes.ipv4Key =
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E, pclId = 1023, groupKeyTypes.ipv6Key
    = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E, other fields zeroed } ].
    Expected: NOT GT_OK for ch1 (different lookupCfg.groupKeyTypes sizes).
*/
    GT_STATUS                       st = GT_OK;
    GT_U8                           dev;

    GT_PORT_GROUPS_BMP              portGroupsBmp = 1;
    CPSS_INTERFACE_INFO_STC         interfaceInfo;
    CPSS_PCL_DIRECTION_ENT          direction = CPSS_PCL_DIRECTION_INGRESS_E;
    CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum = CPSS_PCL_LOOKUP_0_E;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfg;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfgRet;

    CPSS_PP_FAMILY_TYPE_ENT         devFamily;
    GT_BOOL                         failureWas;

    GT_U32                          portGroupId;
    /* access mode */
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   accMode;

    accMode.ipclAccMode     = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E;
    accMode.epclAccMode     = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E;
    accMode.ipclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
    accMode.epclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
    accMode.ipclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
    accMode.epclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;


    cpssOsBzero((GT_VOID*) &interfaceInfo, sizeof(interfaceInfo));
    cpssOsBzero((GT_VOID*) &lookupCfg, sizeof(lookupCfg));
    cpssOsBzero((GT_VOID*) &lookupCfgRet, sizeof(lookupCfgRet));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = cpssDxChPclCfgTblAccessModeSet(
            dev, &accMode);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, st, "cpssDxChPclCfgTblAccessModeSet: %d", dev);

        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);

        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);
            /* I - INTERFACE_TYPE - Port (interfaceInfoPtr->type= CPSS_INTERFACE_PORT_E) */

            /*
               1.1. Check Ingress direction. For all devices call with
               portGroupsBmp [CPSS_PORT_GROUP_UNAWARE_MODE_CNS],
               interfaceInfoPtr [type = CPSS_INTERFACE_PORT_E, devPort {devNum=0,
               portNum=0} ], direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum
               [CPSS_PCL_LOOKUP_0_E], lookupCfgPtr [enableLookup = GT_TRUE,
               pclId = 1023, groupKeyTypes.ipv4Key =
               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
               groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
               other fields zeroed } ].
               Expected: GT_OK.
            */
            interfaceInfo.type = CPSS_INTERFACE_PORT_E; /* should be synhronized with 1.2*/
            interfaceInfo.devPort.devNum = dev;
            interfaceInfo.devPort.portNum = 0;
            if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev) != 0)
            {
                interfaceInfo.devPort.portNum = (GT_U8)(portGroupId *
                     (64 / (PRV_CPSS_PP_MAC(dev)->portGroupsInfo.numOfPortGroups)));
            }

            direction=CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum=CPSS_PCL_LOOKUP_0_E;

            cpssOsBzero((GT_VOID*) &lookupCfg, sizeof(lookupCfg));

            lookupCfg.enableLookup = GT_TRUE;
            lookupCfg.pclId = 1023;
            lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
            lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;

            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                lookupCfg.dualLookup = GT_TRUE;
                lookupCfg.pclIdL01 = 1023;
            }

            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                "device: %d, interfaceInfo [devNum= %d, portNum= %d], direction=%d, lookupNum=%d",
                dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                            direction, lookupNum);

            /*
               1.2. Call cpssDxChPclPortGroupCfgTblGet with interfaceInfoPtr,
               direction, lookupNum - same as in 1.1, non-NULL lookupCfgPtr.
               Expected: GT_OK and same lookupCfgPtr as was written.
            */
            cpssOsMemSet((GT_VOID*)&lookupCfgRet, 0, sizeof(CPSS_DXCH_PCL_LOOKUP_CFG_STC));

            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfgRet);

            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                "cpssDxChPclPortGroupCfgTblGet: "
                "dev: %d, intInfo [devNum=%d, portNum=%d], direction=%d, lookupNum=%d",
                dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                                direction, lookupNum);

            if (GT_OK == st)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&lookupCfg,
                                                (const GT_VOID*)&lookupCfgRet,
                                                sizeof(lookupCfg))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                    "get another lookupCfg than was written [dev = %d, direction=%d, lookupNum=%d]",
                    dev, direction, lookupNum);
            }

            /*
               1.3. Check Egress direction (not supported by Cheetah1).
               Call with direction [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.1.
               Expected: GT_OK for all devices except Cheetah.
            */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;

            cpssOsBzero((GT_VOID*) &lookupCfg, sizeof(lookupCfg));

            lookupCfg.enableLookup = GT_TRUE;
            lookupCfg.pclId = 1023;
            lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E;
            lookupCfg.groupKeyTypes.ipv4Key  = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E;
            lookupCfg.groupKeyTypes.ipv6Key  = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E;

            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            if ((devFamily == CPSS_PP_FAMILY_CHEETAH_E)
                 && (direction != CPSS_PCL_DIRECTION_INGRESS_E))
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                    "Cheetah device: %d, direction=%d", dev, direction);
            }
            else
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                    "Cheetah device: %d, direction=%d", dev, direction);
            }

            direction = CPSS_PCL_DIRECTION_INGRESS_E;

            /*
               1.4. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E],
               other params same as in 1.1.
               Expected: GT_OK.
            */
            cpssOsBzero((GT_VOID*) &lookupCfg, sizeof(lookupCfg));
            lookupCfg.enableLookup = GT_TRUE;
            lookupCfg.pclId = 1023;
            lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
            lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;

            lookupNum = CPSS_PCL_LOOKUP_1_E;

            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);
            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
              "dev: %d, interfaceInfo [devNum=%d, portNum=%d], direction=%d, lookupNum=%d",
              dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                        direction, lookupNum);

            lookupNum = CPSS_PCL_LOOKUP_0_E;

            /* -- II - INTERFACE_TYPE - VLan (interfaceInfoPtr->type= CPSS_INTERFACE_VID_E) --*/

            /*
               1.5. Check Ingress direction. For all devices call with
               interfaceInfoPtr [type = CPSS_INTERFACE_VID_E,
               vlanId = [100 / (PRV_CPSS_MAX_NUM_VLANS_CNS - 1) = 4095] ],
               direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum
               [CPSS_PCL_LOOKUP_0_E], lookupCfgPtr [enableLookup = GT_TRUE, pclId
               = 1023, groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
               other fields zeroed } ].
               Expected: GT_OK for all devices except ch1.
            */
            interfaceInfo.type = CPSS_INTERFACE_VID_E;
            interfaceInfo.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS - 1;

            direction=CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum=CPSS_PCL_LOOKUP_0_E;

            cpssOsBzero((GT_VOID*) &lookupCfg, sizeof(lookupCfg));

            lookupCfg.enableLookup = GT_TRUE;
            lookupCfg.pclId = 1023;
            lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
            lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;

            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                        "device: %d, interfaceInfo [type=%d, vlanId=%d]",
                            dev, interfaceInfo.type, interfaceInfo.vlanId);
            }
            else
            {
                /* CH1 supports VIDs 0-1023 only */
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                        "device: %d, interfaceInfo [type=%d, vlanId=%d]",
                            dev, interfaceInfo.type, interfaceInfo.vlanId);
            }

            interfaceInfo.vlanId = 100;

            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                "device: %d, interfaceInfo [type=%d, vlanId=%d], direction=%d, lookupNum=%d",
                dev, interfaceInfo.type, interfaceInfo.vlanId, direction, lookupNum);

            /*
               1.6. Call cpssDxChPclPortGroupCfgTblGet with interfaceInfoPtr,
               direction, lookupNum - same as in 1.5, non-NULL lookupCfgPtr.
               Expected: GT_OK and same lookupCfgPtr as was written.
            */
            cpssOsMemSet((GT_VOID*)&lookupCfgRet, 0, sizeof(CPSS_DXCH_PCL_LOOKUP_CFG_STC));

            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfgRet);

            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                "cpssDxChPclPortGroupCfgTblGet: "
                "dev: %d, intInfo [type=%d, vlanId=%d], direction=%d, lookupNum=%d",
                dev, interfaceInfo.type, interfaceInfo.vlanId, direction, lookupNum);

            if (GT_OK == st)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&lookupCfg,
                                                (const GT_VOID*)&lookupCfgRet,
                                                sizeof(lookupCfg))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                    "get another lookupCfg than has been written [dev = %d, direction=%d, lookupNum=%d]",
                                        dev, direction, lookupNum);
            }

            /*
               1.7. Check Egress direction (not supported by Cheetah1).
               call with direction [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.5.
               Expected: GT_OK for all devices except Cheetah.
            */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;

            cpssOsBzero((GT_VOID*) &lookupCfg, sizeof(lookupCfg));

            lookupCfg.enableLookup = GT_TRUE;
            lookupCfg.pclId = 1023;
            lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;
            lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;
            lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E;

            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                  "dev: %d, interfaceInfo [type=%d, vlanId =%d], direction=%d, lookupNum=%d",
                        dev, interfaceInfo.type, interfaceInfo.vlanId, direction, lookupNum);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                    "Cheetah device: %d, direction=%d", dev, direction);
            }

            direction = CPSS_PCL_DIRECTION_INGRESS_E;

            /*
               1.8. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E],
               other params same as in 1.5.
               Expected: GT_OK.
            */
            lookupNum = CPSS_PCL_LOOKUP_1_E;

            cpssOsBzero((GT_VOID*) &lookupCfg, sizeof(lookupCfg));

            lookupCfg.enableLookup = GT_TRUE;
            lookupCfg.pclId = 1023;
            lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
            lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;

            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);
            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                  "dev: %d, interfaceInfo [type=%d, vlanId =%d], direction=%d, lookupNum=%d",
                        dev, interfaceInfo.type, interfaceInfo.vlanId, direction, lookupNum);

            lookupNum = CPSS_PCL_LOOKUP_0_E;

            /*
                1.9. Check wrong direction enum values. Other params same as in 1.5.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupCfgTblSet
                                (dev, portGroupsBmp, &interfaceInfo,
                                 direction, lookupNum, &lookupCfg),
                                direction);

            /*
                1.10. Check wrong lookupNum enum values.  Other params same as in 1.5.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupCfgTblSet
                                (dev, portGroupsBmp, &interfaceInfo,
                                 direction, lookupNum, &lookupCfg),
                                lookupNum);

            /*
               1.11. Check for NULL pointer support. Call with lookupCfgPtr [NULL],
               other params same as in 1.5.
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "device: %d, lookupCfgPtr=NULL", dev);

            /*
               1.12. Check for NULL pointer support. Call with interfaceInfoPtr [NULL],
               other params same as in 1.5.
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, NULL, direction,
                                                lookupNum, &lookupCfg);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                        "device: %d, interfaceInfoPtr=NULL", dev);

            /*
               1.13. Check out-of-range vlan id.
               Call with interfaceInfoPtr [type = CPSS_INTERFACE_VID_E, vlanId
               [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] ], other params same as in 1.5.
               Expected: non GT_OK.
            */
            interfaceInfo.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
               "device: %d, interfaceInfo.vlanId = %d", dev, interfaceInfo.vlanId);

            interfaceInfo.vlanId = 100;

            /* -- III - INTERFACE_TYPE - TRUNK (interfaceInfoPtr->type= CPSS_INTERFACE_TRUNK_E) --
              Supported only in Non-Local mode */

            /*
               1.14. Check trunk interface with valid params. For all devices
               call with interfaceInfoPtr [type = CPSS_INTERFACE_TRUNK_E,
               trunkId = 0 ], direction [CPSS_PCL_DIRECTION_INGRESS_E],
               lookupNum [CPSS_PCL_LOOKUP_0_E], lookupCfgPtr [enableLookup =
               GT_TRUE, pclId = 1023, groupKeyTypes {nonIpKey =
               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E, ipv4Key =
               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E, ipv6Key =
               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E } ].
               Expected: NOT GT_OK.
            */
            interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
            interfaceInfo.trunkId = 0;

            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);

            direction=CPSS_PCL_DIRECTION_INGRESS_E;

            lookupNum=CPSS_PCL_LOOKUP_0_E;

            lookupCfg.enableLookup = GT_TRUE;
            lookupCfg.pclId = 1023;
            lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
            lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;

            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
              "device: %d, interfaceInfo [trunkId=%d], direction=%d, lookupNum=%d",
                                       dev, interfaceInfo.trunkId, direction, lookupNum);

            /*
               1.15. Check out-of-range trunk id.
               call with interfaceInfoPtr [type = CPSS_INTERFACE_TRUNK_E, trunkId
               [MAX_TRUNK_ID_CNS = 128] ], other params same as in 1.14.
               Expected: non GT_OK.
            */
            interfaceInfo.trunkId = MAX_TRUNK_ID_CNS;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);

            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
               "device: %d, interfaceInfo.trunkId = %d", dev, interfaceInfo.trunkId);

            interfaceInfo.trunkId = 0;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);

            /* -- IV - INTERFACE_TYPE - Vidx (unsupported) (interfaceInfoPtr->type= CPSS_INTERFACE_VIDX_E) -- */

            /*
               1.16. Check Vidx interface (unsupported) with valid params. For Cheetah/Cheetah2
               devices call with interfaceInfoPtr [type = CPSS_INTERFACE_VIDX_E,
               vidx = 0 ], other params same as in 1.1.
               Expected: non GT_OK.
            */
            interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
            interfaceInfo.vidx = 0;

            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                "device: %d, interfaceInfo.type = %d", dev, interfaceInfo.type);

            interfaceInfo.type = CPSS_INTERFACE_PORT_E;
            interfaceInfo.vidx = 0;

            /* -- V - INTERFACE_TYPE - out-of-range -- */

            /*
                1.17. Check out-of-range interface type enum. Call with
                      interfaceInfoPtr [type = wrong enum values, devPort
                      {devNum=0, portNum=0}, trunkId = 0, vidx = 0, vlanId = 100 ],
                      other params same as in 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupCfgTblSet
                                (dev, portGroupsBmp, &interfaceInfo,
                                 direction, lookupNum, &lookupCfg),
                                interfaceInfo.type);

            /*
                1.18. Check Ingress direction. For all devices call with
                portGroupsBmp ,
                interfaceInfoPtr [type = CPSS_INTERFACE_PORT_E, devPort {devNum=0, portNum=0} ],
                direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum [CPSS_PCL_LOOKUP_1_E], lookupCfgPtr
                [enableLookup = GT_TRUE, groupKeyTypes.ipv4Key =
                CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E, pclId = 1023, groupKeyTypes.ipv6Key
                = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E, other fields zeroed } ].
                Expected: GT_OK for ch1 and not GT_OK for other (not supported STD IPV6 DIP).
            */
            interfaceInfo.type = CPSS_INTERFACE_PORT_E;
            interfaceInfo.devPort.devNum = dev;
            interfaceInfo.devPort.portNum = 0;
            if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev) != 0)
            {
                interfaceInfo.devPort.portNum = (GT_U8)(portGroupId *
                     (64 / (PRV_CPSS_PP_MAC(dev)->portGroupsInfo.numOfPortGroups)));
            }

            direction=CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum=CPSS_PCL_LOOKUP_1_E;

            cpssOsBzero((GT_VOID*) &lookupCfg, sizeof(lookupCfg));

            lookupCfg.enableLookup = GT_TRUE;
            lookupCfg.pclId = 1023;
            lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E;
            lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E;
            lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E;

            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            if (devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            {
                UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                    "dev: %d, interfaceInfo [devNum= %d, portNum= %d], direction=%d, lookupNum=%d",
                    dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                                direction, lookupNum);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL5_STRING_MAC(GT_OK, st,
                    "dev: %d, interfaceInfo [devNum= %d, portNum= %d], direction=%d, lookupNum=%d",
                    dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                                direction, lookupNum);
            }

            /*
                1.19. Check Ingress direction. For all devices call with
                portGroupsBmp [CPSS_PORT_GROUP_UNAWARE_MODE_CNS],
                interfaceInfoPtr [type = CPSS_INTERFACE_PORT_E, devPort {devNum=0, portNum=0} ],
                direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum [CPSS_PCL_LOOKUP_1_E], lookupCfgPtr
                [enableLookup = GT_TRUE, groupKeyTypes.ipv4Key =
                CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E, pclId = 1023, groupKeyTypes.ipv6Key
                = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E, other fields zeroed } ].
                Expected: NOT GT_OK for ch1 (different lookupCfg.groupKeyTypes sizes).
            */

            lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E;
            lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E;
            lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E;

            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            if (devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            {
                UTF_VERIFY_NOT_EQUAL5_STRING_MAC(GT_OK, st,
                    "dev: %d, interfaceInfo [devNum= %d, portNum= %d], direction=%d, lookupNum=%d",
                    dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                                direction, lookupNum);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
        lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                           direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* set correct values for all the parameters (except of device number) */
    interfaceInfo.type = CPSS_INTERFACE_VID_E;
    interfaceInfo.vlanId = 100;

    direction=CPSS_PCL_DIRECTION_INGRESS_E;

    lookupNum=CPSS_PCL_LOOKUP_0_E;

    lookupCfg.enableLookup = GT_TRUE;
    lookupCfg.pclId = 1023;
    lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    lookupCfg.groupKeyTypes.ipv4Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
    lookupCfg.groupKeyTypes.ipv6Key = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                          direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupCfgTblSet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupCfgTblGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    OUT CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupCfgTblGet)
{
/*
I - INTERFACE_TYPE - Port (interfaceInfoPtr->type= CPSS_INTERFACE_PORT_E)

    ITERATE_DEVICES_PORT_GROUPS

    1.1. Check Ingress direction. For all devices call with interfaceInfoPtr [type =
    CPSS_INTERFACE_PORT_E, devPort {devNum=0, portNum=0} ], direction
    [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum [CPSS_PCL_LOOKUP_0_E], non-NULL lookupCfgPtr.
    Expected: GT_OK.

    1.2. Check Egress direction (not supported by cheetah1).
    Call with direction [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.1.
    Expected: GT_OK for all except Cheetah.

    1.3. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E], other params same as in 1.1.
    Expected: GT_OK.

II - INTERFACE_TYPE - VLan (interfaceInfoPtr->type= CPSS_INTERFACE_VID_E)

    1.4. Check Ingress direction. For all devices call with interfaceInfoPtr [type =
    CPSS_INTERFACE_VID_E, vlanId = [100  / (PRV_CPSS_MAX_NUM_VLANS_CNS - 1) = 4095] ],
    direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum [CPSS_PCL_LOOKUP_0_E], non-NULL lookupCfgPtr.
    Expected: GT_OK for all devices except cheetah.

    1.5. Check Egress direction (not supported by Cheetah1). Call with
    direction [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.4.
    Expected: GT_OK for all devices except Cheetah.

    1.6. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E], other params same as in 1.4.
    Expected: GT_OK.

    1.7. Check out-of-range direction enum. call with direction [wrong enum values],
    other params same as in 1.4.
    Expected: GT_BAD_PARAM.

    1.8. Check out-of-range lookupNum enum. call with lookupNum [wrong enum values],
    other params same as in 1.4.
    Expected: GT_BAD_PARAM.

    1.9. Check for NULL pointer support. call with lookupCfgPtr [NULL], other params
    same as in 1.4.
    Expected: GT_BAD_PTR.

    1.10. Check for NULL pointer support. call with interfaceInfoPtr [NULL], other
    params same as in 1.4.
    Expected: GT_BAD_PTR.

    1.11. Check out-of-range vlan id. call with interfaceInfoPtr [type =
    CPSS_INTERFACE_VID_E, vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] ], other params
    same as in 1.4.
    Expected: non GT_OK.

III - INTERFACE_TYPE - TRUNK (interfaceInfoPtr->type= CPSS_INTERFACE_TRUNK_E)

    1.12. Check trunk interface with valid params. For all devices call with
    interfaceInfoPtr [type = CPSS_INTERFACE_TRUNK_E, trunkId = 0 ], direction
    [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum [CPSS_PCL_LOOKUP_0_E], non-NULL lookupCfgPtr.
    Expected: GT_OK for all devices except ch1.

    1.13. Check out-of-range trunk id. call with interfaceInfoPtr [type =
    CPSS_INTERFACE_TRUNK_E, trunkId [MAX_TRUNK_ID_CNS = 128] ], other params same as in 1.12.
    Expected: non GT_OK.

IV - INTERFACE_TYPE - Vidx (unsupported) (interfaceInfoPtr->type= CPSS_INTERFACE_VIDX_E)

    1.14. Check Vidx interface (unsupported) with valid params. Call with
    interfaceInfoPtr [type = CPSS_INTERFACE_VIDX_E, vidx = 0 ], other params same as in 1.1.
    Expected: non GT_OK.

V - INTERFACE_TYPE - out-of-range

    1.15. Check out-of-range interface type enum. call with interfaceInfoPtr [type =
    wrong enum values, devPort {devNum=0, portNum=0}, trunkId = 0, vidx = 0, vlanId =
    100 ], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                       st = GT_OK;
    GT_U8                           dev;

    GT_PORT_GROUPS_BMP              portGroupsBmp = 1;
    CPSS_INTERFACE_INFO_STC         interfaceInfo;
    CPSS_PCL_DIRECTION_ENT          direction = CPSS_PCL_DIRECTION_INGRESS_E;
    CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum = CPSS_PCL_LOOKUP_0_E;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfg;

    CPSS_PP_FAMILY_TYPE_ENT         devFamily;

    GT_U32                          portGroupId;
    /* access mode */
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   accMode;

    accMode.ipclAccMode     = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E;
    accMode.epclAccMode     = CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E;
    accMode.ipclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
    accMode.epclDevPortBase = CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E;
    accMode.ipclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;
    accMode.epclMaxDevPorts = CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = cpssDxChPclCfgTblAccessModeSet(
            dev, &accMode);
        UTF_VERIFY_EQUAL1_STRING_MAC(
            GT_OK, st, "cpssDxChPclCfgTblAccessModeSet: %d", dev);

        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);
            /* I - INTERFACE_TYPE - Port (interfaceInfoPtr->type= CPSS_INTERFACE_PORT_E) */

            /*
               1.1. Check Ingress direction. For all devices call with
               interfaceInfoPtr [type = CPSS_INTERFACE_PORT_E, devPort {devNum=0,
               portNum=0} ], direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum
               [CPSS_PCL_LOOKUP_0_E], non-NULL lookupCfgPtr.
               Expected: GT_OK
           */
            interfaceInfo.type = CPSS_INTERFACE_PORT_E;
            interfaceInfo.devPort.devNum = dev;
            interfaceInfo.devPort.portNum = 0;
            if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev) != 0)
            {
                interfaceInfo.devPort.portNum = (GT_U8)(portGroupId *
                     (64 / (PRV_CPSS_PP_MAC(dev)->portGroupsInfo.numOfPortGroups)));
            }
            direction=CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum=CPSS_PCL_LOOKUP_0_E;

            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                "device: %d, interfaceInfo [devNum=%d, portNum=%d], direction=%d, lookupNum=%d",
                dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                            direction, lookupNum);

            /*
               1.2. Check Egress direction (not supported by cheetah1).
               Call with direction [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.1.
               Expected: GT_OK for all except Cheetah.
            */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;

            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                 "device: %d, interfaceInfo [devNum=%d, portNum=%d], direction=%d, lookupNum=%d",
                 dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                            direction, lookupNum);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                    "Cheetah device: %d, direction=%d", dev, direction);
            }

            direction = CPSS_PCL_DIRECTION_INGRESS_E;

            /*
               1.3. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E],
               other params same as in 1.1.
               Expected: GT_OK.
            */
            lookupNum = CPSS_PCL_LOOKUP_1_E;

            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
              "device: %d, interfaceInfo [devNum=%d, portNum=%d], direction=%d, lookupNum=%d",
              dev, interfaceInfo.devPort.devNum, interfaceInfo.devPort.portNum,
                                                        direction, lookupNum);

            lookupNum = CPSS_PCL_LOOKUP_0_E;

            /* -- II - INTERFACE_TYPE - VLan (interfaceInfoPtr->type= CPSS_INTERFACE_VID_E) --*/

            /*
               1.4. Check Ingress direction. For all devices call with
               interfaceInfoPtr [type = CPSS_INTERFACE_VID_E,
               vlanId = [100  / (PRV_CPSS_MAX_NUM_VLANS_CNS - 1) = 4095] ],
               direction [CPSS_PCL_DIRECTION_INGRESS_E], lookupNum
               [CPSS_PCL_LOOKUP_0_E], lookupCfgPtr [enableLookup = GT_TRUE, pclId
               = 1023, groupKeyTypes {nonIpKey =
               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E, ipv4Key =
               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E, ipv6Key =
               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E } ].
               Expected: GT_OK for all devices except cheetah.
            */

            interfaceInfo.type = CPSS_INTERFACE_VID_E;
            interfaceInfo.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS - 1;

            direction=CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum=CPSS_PCL_LOOKUP_0_E;

            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(
                    GT_OK, st,
                    "device: %d, interfaceInfo [type=%d, vlanId =%d]",
                    dev, interfaceInfo.type, interfaceInfo.vlanId);
            }
            else
            {
                /* CH1 supports VIDs 0-1023 only */
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                    "Cheetah1 device: %d, interfaceInfo [type=%d, vlanId =%d]",
                    dev, interfaceInfo.type, interfaceInfo.vlanId);
            }

            interfaceInfo.vlanId = 100;

            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
            "device: %d, interfaceInfo [type=%d, vlanId =%d], direction=%d, lookupNum=%d",
                      dev, interfaceInfo.type, interfaceInfo.vlanId, direction, lookupNum);

            /*
               1.5. Check Egress direction (not supported by Cheetah1).
               Call with direction [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.4.
               Expected: GT_OK for all devices except Cheetah.
            */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;

            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                  "dev: %d, interfaceInfo [type=%d, vlanId =%d], direction=%d, lookupNum=%d",
                  dev, interfaceInfo.type, interfaceInfo.vlanId, direction, lookupNum);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                    "Cheetah device: %d, direction=%d", dev, direction);
            }

            direction = CPSS_PCL_DIRECTION_INGRESS_E;

            /*
               1.6. Check LOOKUP_1. Call with lookupNum [CPSS_PCL_LOOKUP_1_E],
               other params same as in 1.4.
               Expected: GT_OK.
            */
            lookupNum = CPSS_PCL_LOOKUP_1_E;

            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                                  direction, lookupNum, &lookupCfg);

            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
             "dev: %d, interfaceInfo [type=%d, vlanId =%d], direction=%d, lookupNum=%d",
                dev, interfaceInfo.type, interfaceInfo.vlanId, direction, lookupNum);

            lookupNum = CPSS_PCL_LOOKUP_0_E;

            /*
                1.7. Check wrong direction enum value. Other params same as in 1.4.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupCfgTblGet
                                (dev, portGroupsBmp, &interfaceInfo,
                                direction, lookupNum, &lookupCfg),
                                direction);

            /*
                1.8. Check wrong lookupNum enum value. Other params same as in 1.4.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupCfgTblGet
                                (dev, portGroupsBmp, &interfaceInfo,
                                direction, lookupNum, &lookupCfg),
                                lookupNum);

            /*
               1.9. Check for NULL pointer support. Call with lookupCfgPtr [NULL],
               other params same as in 1.4.
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "device: %d, lookupCfgPtr=NULL", dev);

            /*
               1.10. Check for NULL pointer support. Call with interfaceInfoPtr [NULL],
               other params same as in 1.4.
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, NULL,
                                      direction, lookupNum, &lookupCfg);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                    "device: %d, interfaceInfoPtr=NULL", dev);

            /*
               1.11. Check out-of-range vlan id. Call with interfaceInfoPtr
               [type = CPSS_INTERFACE_VID_E, vlanId
               [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] ], other params same as in 1.4.
               Expected: non GT_OK.
            */
            interfaceInfo.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
               "device: %d, interfaceInfo.vlanId = %d", dev, interfaceInfo.vlanId);

            interfaceInfo.vlanId = 100;

            /* -- III - INTERFACE_TYPE - TRUNK (interfaceInfoPtr->type= CPSS_INTERFACE_TRUNK_E) --
            Supported only in Non-Local mode */

            /*
               1.12. Check trunk interface with valid params. For all devices
               call with interfaceInfoPtr [type = CPSS_INTERFACE_TRUNK_E,
               trunkId = 0 ], direction [CPSS_PCL_DIRECTION_INGRESS_E],
               lookupNum [CPSS_PCL_LOOKUP_0_E], lookupCfgPtr [enableLookup =
               GT_TRUE, pclId = 1023, groupKeyTypes {nonIpKey =
               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E, ipv4Key =
               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E, ipv6Key =
               CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E } ].
               Expected: GT_OK.
            */
            interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
            interfaceInfo.trunkId = 0;

            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);

            direction=CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum=CPSS_PCL_LOOKUP_0_E;

            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
              "device: %d, interfaceInfo [trunkId=%d], direction=%d, lookupNum=%d",
                              dev, interfaceInfo.trunkId, direction, lookupNum);

            /*
               1.13. Check out-of-range trunk id. Call with interfaceInfoPtr
               [type = CPSS_INTERFACE_TRUNK_E, trunkId
               [MAX_TRUNK_ID_CNS = 128] ], other params same as in 1.12.
               Expected: non GT_OK.
            */
            interfaceInfo.trunkId = MAX_TRUNK_ID_CNS;

            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);

            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
               "device: %d, interfaceInfo.trunkId = %d", dev,
                                              interfaceInfo.trunkId);
            interfaceInfo.trunkId = 0;

            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);

            /* -- IV - INTERFACE_TYPE - Vidx (unsupported) (interfaceInfoPtr->type= CPSS_INTERFACE_VIDX_E) -- */

            /*
               1.14. Check Vidx interface (unsupported) with valid params.
               Call with interfaceInfoPtr [type = CPSS_INTERFACE_VIDX_E, vidx = 0 ],
               other params same as in 1.1.
               Expected: non GT_OK.
            */
            interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
            interfaceInfo.vidx = 0;

            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                              direction, lookupNum, &lookupCfg);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                "dev: %d, interfaceInfo.type = %d", dev, interfaceInfo.type);

            interfaceInfo.type = CPSS_INTERFACE_PORT_E;
            interfaceInfo.vidx = 0;

            /* -- V - INTERFACE_TYPE - out-of-range -- */

            /*
                1.15. Check out-of-range interface type enum. Call with interfaceInfoPtr
                [type = wrong enum values, devPort {devNum=0, portNum=0}, trunkId = 0, vidx = 0,
                vlanId = 100 ], other params same as in 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupCfgTblGet
                                (dev, portGroupsBmp, &interfaceInfo,
                                 direction, lookupNum, &lookupCfg),
                                interfaceInfo.type);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        direction=CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum=CPSS_PCL_LOOKUP_0_E;
        interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        interfaceInfo.devPort.devNum = dev;
        interfaceInfo.devPort.portNum = 0;

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                      direction, lookupNum, &lookupCfg);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                           direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* set correct values for all the parameters (except of device number) */
    interfaceInfo.type = CPSS_INTERFACE_VID_E;
    interfaceInfo.vlanId = 100;
    direction=CPSS_PCL_DIRECTION_INGRESS_E;
    lookupNum=CPSS_PCL_LOOKUP_0_E;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                              direction, lookupNum, &lookupCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupCfgTblGet(dev, portGroupsBmp, &interfaceInfo,
                                          direction, lookupNum, &lookupCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesSet
(
    IN  GT_U8                            devNum,
    IN  GT_PORT_GROUPS_BMP               portGroupsBmp,
    IN  CPSS_DXCH_PCL_OVERRIDE_UDB_STC   udbOverride
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupOverrideUserDefinedBytesSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (xCat and above)
    1.1. Call cpssDxChPclPortGroupOverrideUserDefinedBytesSet with
         udbOverride all GT_TRUE/FALSE and mixed.
    Expected: GT_OK.
    1.2. Call cpssDxChPclPortGroupOverrideUserDefinedBytesGet with non-NULL udbOverride.
    Expected: GT_OK and the same udbOverride.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     vrfIdUse  = GT_FALSE;
    CPSS_PP_FAMILY_TYPE_ENT             devFamily;

    GT_PORT_GROUPS_BMP             portGroupsBmp = 1;
    CPSS_DXCH_PCL_OVERRIDE_UDB_STC udbOverride = {0};
    CPSS_DXCH_PCL_OVERRIDE_UDB_STC udbOverrideGet;

    GT_U32    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1. Call with udbOverride GT_TRUE/FALSE and mixed.
                Expected: GT_OK.
            */
            cpssOsBzero((GT_VOID*)&udbOverride, sizeof(udbOverride));
            cpssOsBzero((GT_VOID*)&udbOverrideGet, sizeof(udbOverrideGet));

            /* Call with udbOverride all GT_FALSE */
            st = cpssDxChPclPortGroupOverrideUserDefinedBytesSet(dev,
                                        portGroupsBmp, &udbOverride);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.2. Call cpssDxChPclPortGroupOverrideUserDefinedBytesGet
                     with non-NULL udbOverride.
                Expected: GT_OK and the same udbOverride.
            */
            st = cpssDxChPclPortGroupOverrideUserDefinedBytesGet(dev,
                                      portGroupsBmp, &udbOverrideGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChPclPortGroupOverrideUserDefinedBytesGet: %d", dev);

            /* Verifying values*/
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableStdNotIp,
                                      udbOverrideGet.vrfIdLsbEnableStdNotIp,
              "get another udbOverride.vrfIdLsbEnableStdNotIp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableStdNotIp,
                                      udbOverrideGet.vrfIdMsbEnableStdNotIp,
              "get another udbOverride.vrfIdMsbEnableStdNotIp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableStdIpL2Qos,
                                      udbOverrideGet.vrfIdLsbEnableStdIpL2Qos,
              "get another udbOverride.vrfIdLsbEnableStdIpL2Qos than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableStdIpL2Qos,
                                      udbOverrideGet.vrfIdMsbEnableStdIpL2Qos,
              "get another udbOverride.vrfIdMsbEnableStdIpL2Qos than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableStdIpv4L4,
                                      udbOverrideGet.vrfIdLsbEnableStdIpv4L4,
              "get another udbOverride.vrfIdLsbEnableStdIpv4L4 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableStdIpv4L4,
                                      udbOverrideGet.vrfIdMsbEnableStdIpv4L4,
              "get another udbOverride.vrfIdMsbEnableStdIpv4L4 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableStdUdb,
                                      udbOverrideGet.vrfIdLsbEnableStdUdb,
              "get another udbOverride.vrfIdLsbEnableStdUdb than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableStdUdb,
                                      udbOverrideGet.vrfIdMsbEnableStdUdb,
              "get another udbOverride.vrfIdMsbEnableStdUdb than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableExtNotIpv6,
                                      udbOverrideGet.vrfIdLsbEnableExtNotIpv6,
              "get another udbOverride.vrfIdLsbEnableStdNotIp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableExtNotIpv6,
                                      udbOverrideGet.vrfIdMsbEnableExtNotIpv6,
              "get another udbOverride.vrfIdMsbEnableExtNotIpv6 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableExtIpv6L2,
                                      udbOverrideGet.vrfIdLsbEnableExtIpv6L2,
              "get another udbOverride.vrfIdLsbEnableExtIpv6L2 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableExtIpv6L2,
                                      udbOverrideGet.vrfIdMsbEnableExtIpv6L2,
              "get another udbOverride.vrfIdMsbEnableExtIpv6L2 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableExtIpv6L4,
                                      udbOverrideGet.vrfIdLsbEnableExtIpv6L4,
              "get another udbOverride.vrfIdLsbEnableExtIpv6L4 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableExtIpv6L4,
                                      udbOverrideGet.vrfIdMsbEnableExtIpv6L4,
              "get another udbOverride.vrfIdMsbEnableExtIpv6L4 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableExtUdb,
                                      udbOverrideGet.vrfIdLsbEnableExtUdb,
              "get another udbOverride.vrfIdLsbEnableExtUdb than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableExtUdb,
                                      udbOverrideGet.vrfIdMsbEnableExtUdb,
              "get another udbOverride.vrfIdMsbEnableExtUdb than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.qosProfileEnableStdUdb,
                                      udbOverrideGet.qosProfileEnableStdUdb,
              "get another udbOverride.qosProfileEnableStdUdb than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.qosProfileEnableExtUdb,
                                      udbOverrideGet.qosProfileEnableExtUdb,
              "get another udbOverride.qosProfileEnableExtUdb than was set: %d", dev);

            if (devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            {
                vrfIdUse = GT_FALSE;
            }
            else
            {
                vrfIdUse = GT_TRUE;
            }

            /* Call with udbOverride all GT_TRUE*/
            udbOverride.vrfIdLsbEnableStdNotIp   = vrfIdUse;
            udbOverride.vrfIdMsbEnableStdNotIp   = vrfIdUse;
            udbOverride.vrfIdLsbEnableStdIpL2Qos = vrfIdUse;
            udbOverride.vrfIdMsbEnableStdIpL2Qos = vrfIdUse;
            udbOverride.vrfIdLsbEnableStdIpv4L4  = vrfIdUse;
            udbOverride.vrfIdMsbEnableStdIpv4L4  = vrfIdUse;
            udbOverride.vrfIdLsbEnableStdUdb     = vrfIdUse;
            udbOverride.vrfIdMsbEnableStdUdb     = vrfIdUse;
            udbOverride.vrfIdLsbEnableExtNotIpv6 = vrfIdUse;
            udbOverride.vrfIdMsbEnableExtNotIpv6 = vrfIdUse;
            udbOverride.vrfIdLsbEnableExtIpv6L2  = vrfIdUse;
            udbOverride.vrfIdMsbEnableExtIpv6L2  = vrfIdUse;
            udbOverride.vrfIdLsbEnableExtIpv6L4  = vrfIdUse;
            udbOverride.vrfIdMsbEnableExtIpv6L4  = vrfIdUse;
            udbOverride.vrfIdLsbEnableExtUdb     = vrfIdUse;
            udbOverride.vrfIdMsbEnableExtUdb     = vrfIdUse;
            udbOverride.qosProfileEnableStdUdb   = GT_TRUE;
            udbOverride.qosProfileEnableExtUdb   = GT_TRUE;

            st = cpssDxChPclPortGroupOverrideUserDefinedBytesSet(dev,
                                            portGroupsBmp, &udbOverride);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.2. Call cpssDxChPclPortGroupOverrideUserDefinedBytesGet
                     with non-NULL udbOverride.
                Expected: GT_OK and the same udbOverride.
            */
            st = cpssDxChPclPortGroupOverrideUserDefinedBytesGet(dev,
                                            portGroupsBmp, &udbOverrideGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChPclPortGroupOverrideUserDefinedBytesGet: %d", dev);

            /* Verifying values*/
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableStdNotIp,
                                      udbOverrideGet.vrfIdLsbEnableStdNotIp,
              "get another udbOverride.vrfIdLsbEnableStdNotIp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableStdNotIp,
                                      udbOverrideGet.vrfIdMsbEnableStdNotIp,
              "get another udbOverride.vrfIdMsbEnableStdNotIp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableStdIpL2Qos,
                                      udbOverrideGet.vrfIdLsbEnableStdIpL2Qos,
              "get another udbOverride.vrfIdLsbEnableStdIpL2Qos than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableStdIpL2Qos,
                                      udbOverrideGet.vrfIdMsbEnableStdIpL2Qos,
              "get another udbOverride.vrfIdMsbEnableStdIpL2Qos than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableStdIpv4L4,
                                      udbOverrideGet.vrfIdLsbEnableStdIpv4L4,
              "get another udbOverride.vrfIdLsbEnableStdIpv4L4 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableStdIpv4L4,
                                      udbOverrideGet.vrfIdMsbEnableStdIpv4L4,
              "get another udbOverride.vrfIdMsbEnableStdIpv4L4 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableStdUdb,
                                      udbOverrideGet.vrfIdLsbEnableStdUdb,
              "get another udbOverride.vrfIdLsbEnableStdUdb than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableStdUdb,
                                      udbOverrideGet.vrfIdMsbEnableStdUdb,
              "get another udbOverride.vrfIdMsbEnableStdUdb than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableExtNotIpv6,
                                      udbOverrideGet.vrfIdLsbEnableExtNotIpv6,
              "get another udbOverride.vrfIdLsbEnableStdNotIp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableExtNotIpv6,
                                      udbOverrideGet.vrfIdMsbEnableExtNotIpv6,
              "get another udbOverride.vrfIdMsbEnableExtNotIpv6 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableExtIpv6L2,
                                      udbOverrideGet.vrfIdLsbEnableExtIpv6L2,
              "get another udbOverride.vrfIdLsbEnableExtIpv6L2 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableExtIpv6L2,
                                      udbOverrideGet.vrfIdMsbEnableExtIpv6L2,
              "get another udbOverride.vrfIdMsbEnableExtIpv6L2 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableExtIpv6L4,
                                      udbOverrideGet.vrfIdLsbEnableExtIpv6L4,
              "get another udbOverride.vrfIdLsbEnableExtIpv6L4 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableExtIpv6L4,
                                      udbOverrideGet.vrfIdMsbEnableExtIpv6L4,
              "get another udbOverride.vrfIdMsbEnableExtIpv6L4 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableExtUdb,
                                      udbOverrideGet.vrfIdLsbEnableExtUdb,
              "get another udbOverride.vrfIdLsbEnableExtUdb than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableExtUdb,
                                      udbOverrideGet.vrfIdMsbEnableExtUdb,
              "get another udbOverride.vrfIdMsbEnableExtUdb than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.qosProfileEnableStdUdb,
                                      udbOverrideGet.qosProfileEnableStdUdb,
              "get another udbOverride.qosProfileEnableStdUdb than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.qosProfileEnableExtUdb,
                                      udbOverrideGet.qosProfileEnableExtUdb,
              "get another udbOverride.qosProfileEnableExtUdb than was set: %d", dev);

            if (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
            {
                /* Call with udbOverride all mixed*/
                udbOverride.vrfIdLsbEnableStdNotIp   = GT_TRUE;
                udbOverride.vrfIdMsbEnableStdNotIp   = GT_TRUE;
                udbOverride.vrfIdLsbEnableStdIpL2Qos = GT_FALSE;
                udbOverride.vrfIdMsbEnableStdIpL2Qos = GT_FALSE;
                udbOverride.vrfIdLsbEnableStdIpv4L4  = GT_TRUE;
                udbOverride.vrfIdMsbEnableStdIpv4L4  = GT_TRUE;
                udbOverride.vrfIdLsbEnableStdUdb     = GT_FALSE;
                udbOverride.vrfIdMsbEnableStdUdb     = GT_FALSE;
                udbOverride.vrfIdLsbEnableExtNotIpv6 = GT_TRUE;
                udbOverride.vrfIdMsbEnableExtNotIpv6 = GT_TRUE;
                udbOverride.vrfIdLsbEnableExtIpv6L2  = GT_TRUE;
                udbOverride.vrfIdMsbEnableExtIpv6L2  = GT_TRUE;
                udbOverride.vrfIdLsbEnableExtIpv6L4  = GT_TRUE;
                udbOverride.vrfIdMsbEnableExtIpv6L4  = GT_TRUE;
                udbOverride.vrfIdLsbEnableExtUdb     = GT_FALSE;
                udbOverride.vrfIdMsbEnableExtUdb     = GT_FALSE;
                udbOverride.qosProfileEnableStdUdb   = GT_TRUE;
                udbOverride.qosProfileEnableExtUdb   = GT_TRUE;

                st = cpssDxChPclPortGroupOverrideUserDefinedBytesSet(dev,
                                            portGroupsBmp, &udbOverride);
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

                /*
                    1.2. Call cpssDxChPclPortGroupOverrideUserDefinedBytesGet
                         with non-NULL udbOverride.
                    Expected: GT_OK and the same udbOverride.
                */
                st = cpssDxChPclPortGroupOverrideUserDefinedBytesGet(dev,
                                            portGroupsBmp, &udbOverrideGet);
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                           "cpssDxChPclPortGroupOverrideUserDefinedBytesGet: %d", dev);

                /* Verifying values*/
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableStdNotIp,
                                          udbOverrideGet.vrfIdLsbEnableStdNotIp,
                  "get another udbOverride.vrfIdLsbEnableStdNotIp than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableStdNotIp,
                                          udbOverrideGet.vrfIdMsbEnableStdNotIp,
                  "get another udbOverride.vrfIdMsbEnableStdNotIp than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableStdIpL2Qos,
                                          udbOverrideGet.vrfIdLsbEnableStdIpL2Qos,
                  "get another udbOverride.vrfIdLsbEnableStdIpL2Qos than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableStdIpL2Qos,
                                          udbOverrideGet.vrfIdMsbEnableStdIpL2Qos,
                  "get another udbOverride.vrfIdMsbEnableStdIpL2Qos than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableStdIpv4L4,
                                          udbOverrideGet.vrfIdLsbEnableStdIpv4L4,
                  "get another udbOverride.vrfIdLsbEnableStdIpv4L4 than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableStdIpv4L4,
                                          udbOverrideGet.vrfIdMsbEnableStdIpv4L4,
                  "get another udbOverride.vrfIdMsbEnableStdIpv4L4 than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableStdUdb,
                                          udbOverrideGet.vrfIdLsbEnableStdUdb,
                  "get another udbOverride.vrfIdLsbEnableStdUdb than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableStdUdb,
                                          udbOverrideGet.vrfIdMsbEnableStdUdb,
                  "get another udbOverride.vrfIdMsbEnableStdUdb than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableExtNotIpv6,
                                          udbOverrideGet.vrfIdLsbEnableExtNotIpv6,
                  "get another udbOverride.vrfIdLsbEnableStdNotIp than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableExtNotIpv6,
                                          udbOverrideGet.vrfIdMsbEnableExtNotIpv6,
                  "get another udbOverride.vrfIdMsbEnableExtNotIpv6 than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableExtIpv6L2,
                                          udbOverrideGet.vrfIdLsbEnableExtIpv6L2,
                  "get another udbOverride.vrfIdLsbEnableExtIpv6L2 than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableExtIpv6L2,
                                          udbOverrideGet.vrfIdMsbEnableExtIpv6L2,
                  "get another udbOverride.vrfIdMsbEnableExtIpv6L2 than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableExtIpv6L4,
                                          udbOverrideGet.vrfIdLsbEnableExtIpv6L4,
                  "get another udbOverride.vrfIdLsbEnableExtIpv6L4 than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableExtIpv6L4,
                                          udbOverrideGet.vrfIdMsbEnableExtIpv6L4,
                  "get another udbOverride.vrfIdMsbEnableExtIpv6L4 than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdLsbEnableExtUdb,
                                          udbOverrideGet.vrfIdLsbEnableExtUdb,
                  "get another udbOverride.vrfIdLsbEnableExtUdb than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.vrfIdMsbEnableExtUdb,
                                          udbOverrideGet.vrfIdMsbEnableExtUdb,
                  "get another udbOverride.vrfIdMsbEnableExtUdb than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.qosProfileEnableStdUdb,
                                          udbOverrideGet.qosProfileEnableStdUdb,
                  "get another udbOverride.qosProfileEnableStdUdb than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(udbOverride.qosProfileEnableExtUdb,
                                          udbOverrideGet.qosProfileEnableExtUdb,
                  "get another udbOverride.qosProfileEnableExtUdb than was set: %d", dev);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupOverrideUserDefinedBytesSet(dev, portGroupsBmp,
                                                                &udbOverride);
            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupOverrideUserDefinedBytesSet(dev, portGroupsBmp,
                                                             &udbOverride);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclPortGroupOverrideUserDefinedBytesSet(dev,
                                    portGroupsBmp, &udbOverride);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupOverrideUserDefinedBytesSet(dev,
                                portGroupsBmp, &udbOverride);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesGet
(
    IN  GT_U8                          devNum,
    IN  GT_PORT_GROUPS_BMP             portGroupsBmp,
    OUT CPSS_DXCH_PCL_OVERRIDE_UDB_STC *udbOverridePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupOverrideUserDefinedBytesGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (xCat and above)
    1.1. Call with non-NULL udbOverridePtr.
    Expected: GT_OK.
    1.2 Call with udbOverridePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                      st = GT_OK;
    GT_U8                          dev;
    GT_PORT_GROUPS_BMP             portGroupsBmp = 1;
    CPSS_DXCH_PCL_OVERRIDE_UDB_STC udbOverridePtr;

    GT_U32    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1. Call with non-NULL udbOverridePtr
                Expected: GT_OK.
            */
            st = cpssDxChPclPortGroupOverrideUserDefinedBytesGet(dev,
                                        portGroupsBmp, &udbOverridePtr);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.2 Call with udbOverridePtr [NULL];
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupOverrideUserDefinedBytesGet(dev, portGroupsBmp, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, udbOverridePtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupOverrideUserDefinedBytesGet(dev,
                                        portGroupsBmp, &udbOverridePtr);
            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupOverrideUserDefinedBytesGet(dev,
                                    portGroupsBmp, &udbOverridePtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclPortGroupOverrideUserDefinedBytesGet(dev,
                                        portGroupsBmp, &udbOverridePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupOverrideUserDefinedBytesGet(dev,
                                    portGroupsBmp, &udbOverridePtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    IN CPSS_PCL_DIRECTION_ENT              direction,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupRuleActionGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(dxChx)

    1.1. Set rule for testing. Call cpssDxChPclPortGroupRuleSet with
    ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E], ruleIndex [PCL_TESTED_RULE_INDEX],
    maskPtr-> ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ], patternPtr-> ruleStdNotIp [
    common[pclId=0; sourcePort=5; isTagged=1; vid=100; up=0; qosProfile=0; isIp=0;
    isL2Valid=1; isUdbValid =1], isIpv4 = 0; etherType=0; isArp=0; l2Encap=0;
    macDa=AB:CD:EF:00:00:02; macSa=AB:CD:EF:00:00:01; udb[10,20,30] ], actionPtr [ pktCmd
    = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
    matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
    egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
    ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
    redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] },
    policer { policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
    modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
    precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
    doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
    Expected: GT_OK.

    1.2. For all devices check with legal parameters. Call with
    ruleSize[CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX], direction
    [CPSS_PCL_DIRECTION_INGRESS_E], non-NULL actionPtr.
    Expected: GT_OK for all devices except cheetah1.

    1.3. Check with rule size for other type of rule. Call with ruleSize
    [CPSS_PCL_RULE_SIZE_EXT_E], other params same as in 1.2.
    Expected: NON GT_OK.

    1.4. Check invalid rule size value. Call with ruleSize [wrong enum values], other
    params same as in 1.2.
    Expected: GT_BAD_PARAM.

    1.5. Check out of range ruleIndex. Call with ruleIndex [1024], other parameters same as in 1.2.
    Expected: NON GT_OK.

    1.6. Check invalid direction value. Call with direction
    [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.2.
    Expected: NON GT_OK.

    1.7. Check out-of-range  enum. Call with direction [wrong enum values], other
    parameters same as in 1.2.
    Expected: GT_BAD_PARAM.

    1.8. Check for NULL pointer. Call with actionPtr [NULL], other parameters same as in 1.2.
    Expected: GT_BAD_PTR.

    1.9. Call cpssDxChPclPortGroupRuleInvalidate with ruleSize
    [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
    Expected: GT_OK.

    1.10. Try to get action from invalidated rule. Call with ruleIndex [PCL_TESTED_RULE_INDEX], other
    parameters same as in 1.2.
    Expected: NON GT_OK.
*/
    GT_STATUS  st = GT_OK;

    GT_U8                      dev;
    GT_PORT_GROUPS_BMP         portGroupsBmp = 1;
    CPSS_PCL_RULE_SIZE_ENT     ruleSize = CPSS_PCL_RULE_SIZE_STD_E ;
    GT_U32                     ruleIndex = 0;
    CPSS_PCL_DIRECTION_ENT     direction = CPSS_PCL_DIRECTION_INGRESS_E;
    CPSS_DXCH_PCL_ACTION_STC   retAction;

    GT_U32    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1. For Set rule for testing.  Call cpssDxChPclPortGroupRuleSet with
                ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
                ruleIndex [PCL_TESTED_RULE_INDEX], maskPtr-> ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ],
                patternPtr-> ruleStdNotIp [ common[pclId=0; sourcePort=5; isTagged=1;
                vid=100; up=0; qosProfile=0; isIp=0; isL2Valid=1; isUdbValid =1],
                isIpv4 = 0; etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
                macSa=AB:CD:EF:00:00:01; udb[10,20,30] ],
                actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
                matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
                qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
                qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                profilePrecedence=GT_FALSE] ] },
                redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 },
                vlan { egressTaggedModify=GT_FALSE,
                modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
                Expected: GT_OK.
            */

            st = pclRuleTestingDefaultSet(dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                    "cpssDxChPclPortGroupRuleSet: device: %d", dev);

            /*
                1.2. For all devices check with legal parameters. Call with
                ruleSize[CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX],
                direction [CPSS_PCL_DIRECTION_INGRESS_E], non-NULL actionPtr.
                Expected: GT_OK
            */
            ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
            ruleIndex = PCL_TCAM_RAWS(dev) - 1;
            direction = CPSS_PCL_DIRECTION_INGRESS_E;

            st = cpssDxChPclPortGroupRuleActionGet(dev, portGroupsBmp,
                                        ruleSize, ruleIndex, direction, &retAction);
            if (st != GT_BAD_STATE)
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(
                    GT_OK, st, "device: %d, %d, %d, %d",
                    dev, ruleSize, ruleIndex, direction);
            }

            /*
                1.3. Check with rule size for other type of rule.
                Call with ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], other params same as in 1.2.
                Expected: GT_OK.
            */
            ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;

            st = cpssDxChPclPortGroupRuleActionGet(dev, portGroupsBmp,
                                    ruleSize, ruleIndex, direction, &retAction);
            if (st != GT_BAD_STATE)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleSize);
            }

            ruleSize = CPSS_PCL_RULE_SIZE_STD_E; /* restore valid values */

            /*
                1.4. Check wrong enum rule size values.  Other params same as in 1.2.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupRuleActionGet
                                (dev, portGroupsBmp, ruleSize,
                                 ruleIndex, direction, &retAction),
                                ruleSize);

            /*
               1.5. Check out of range ruleIndex.
               Expected: NON GT_OK.
            */
            ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

            st = cpssDxChPclPortGroupRuleActionGet(dev, portGroupsBmp, ruleSize,
                                                    ruleIndex, direction, &retAction);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

            ruleIndex = PCL_TESTED_RULE_INDEX; /* restore valid value */

            /*
               1.6. Check invalid direction value.  Call with direction
               [CPSS_PCL_DIRECTION_EGRESS_E], other params same as in 1.2.
               Expected: GT_OK.
            */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;

            st = cpssDxChPclPortGroupRuleActionGet(dev, portGroupsBmp, ruleSize,
                                                    ruleIndex, direction, &retAction);
            if (st != GT_BAD_STATE)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(
                    GT_OK, st, dev, ruleSize, ruleIndex, direction);
            }

            direction = CPSS_PCL_DIRECTION_INGRESS_E; /* restore */

            /*
                1.7. Check with wrong enum values direction. Other parameters same as in 1.2.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupRuleActionGet
                                (dev, portGroupsBmp, ruleSize,
                                 ruleIndex, direction, &retAction),
                                direction);

            /*
               1.8. Call with actionPtr [NULL], other parameters same as in 1.2.
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupRuleActionGet(dev, portGroupsBmp, ruleSize,
                                                    ruleIndex, direction, NULL);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, %d, NULL",
                                         dev, ruleSize, ruleIndex, direction);

            /*
               1.9. Call cpssDxChPclPortGroupRuleInvalidate with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
               ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
               Expected: GT_OK.
            */
            st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                "cpssDxChPclPortGroupRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupRuleActionGet(dev, portGroupsBmp,
                                    ruleSize, ruleIndex, direction, &retAction);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                if (st != GT_BAD_STATE)
                {
                    UTF_VERIFY_EQUAL2_PARAM_MAC(
                        GT_OK, st, dev, portGroupsBmp);
                }
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupRuleActionGet(dev, portGroupsBmp, ruleSize,
                                               ruleIndex, direction, &retAction);
        if (st != GT_BAD_STATE)
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(
                GT_OK, st, dev, portGroupsBmp);
        }
    }

    portGroupsBmp = 1;

    /* Set valid parameters. As soon as family of device can't be determined */
    /* - values valid for any family                                         */
    ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
    ruleIndex = 0;
    direction = CPSS_PCL_DIRECTION_INGRESS_E;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclPortGroupRuleActionGet(dev, portGroupsBmp, ruleSize,
                                        ruleIndex, direction, &retAction);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupRuleActionGet(dev, portGroupsBmp, ruleSize,
                                       ruleIndex, direction, &retAction);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupRuleActionUpdate)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(dxChx)
    1.1. set rule for testing. Call cpssDxChPclRuleSet with ruleFormat
    [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E], ruleIndex [PCL_TESTED_RULE_INDEX], maskPtr->
    ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ], patternPtr-> ruleStdNotIp [
    common[pclId=0; sourcePort=5; isTagged=1; vid=100; up=0; qosProfile=0; isIp=0;
    isL2Valid=1; isUdbValid =1], isIpv4 = 0; etherType=0; isArp=0; l2Encap=0;
    macDa=AB:CD:EF:00:00:02; macSa=AB:CD:EF:00:00:01; udb[10,20,30] ], actionPtr [ pktCmd
    = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
    matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
    egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
    ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
    redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] },
    policer { policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
    modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
    precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
    doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
    Expected: GT_OK.

    1.2. For all devices update action with legal parameters. Call with ruleSize
    [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX], actionPtr is the same as in 1.1.
    Expected: GT_OK

    1.3. call cpssDxChPclPortGroupRuleActionGet with ruleSize[CPSS_PCL_RULE_SIZE_STD_E], ruleIndex
    [PCL_TESTED_RULE_INDEX], direction [CPSS_PCL_DIRECTION_INGRESS_E], non-NULL actionPtr.
    Expected: GT_OK and same values in actionPtr  as were written.

    1.4. Check with rule size for other type of rule. Call with ruleSize
    [CPSS_PCL_RULE_SIZE_EXT_E], ruleIndex [PCL_TESTED_RULE_INDEX], actionPtr is the same as in 1.1.
    Expected: NON GT_OK.

    1.5. Check invalid rule size value. Call with ruleSize [wrong enum values], ruleIndex
    [PCL_TESTED_RULE_INDEX], actionPtr is the same as in 1.1.
    Expected: GT_BAD_PARAM.

    1.6. Check out of range ruleIndex. Call with ruleIndex [1024], other parameters is
    the same as in 1.2.
    Expected: NON GT_OK.

    1.7. Call with out of range actionPtr->matchCounter.matchCounterIndex [32]
                   and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3 and above.

    1.8. Call with out of range actionPtr->matchCounter.matchCounterIndex [16384]
                      and other params from 1.1.
    Expected: NOT GT_OK.

    1.9. Call with out of range actionPtr->policer.policerId [256]
                   and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3 and above.

    1.10. Call with out of range actionPtr->policer.policerId [4095]
                   and other params from 1.1.
    Expected: NOT GT_OK.

    1.11. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                   actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                   out of range actionPtr->redirect.outIf.tunnelPtr[1024]
                   and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3 and above.

    1.12. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                   actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                   out of range actionPtr->redirect.outIf.tunnelPtr[8192]
                   and other params from 1.1.
    Expected: NOT GT_OK.

    1.13. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                   out of range actionPtr->redirect.data.routerLttIndex[8192]
                   and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3 and above.

    1.14. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                   out of range actionPtr->redirect.data.routerLttIndex[32768]
                   and other params from 1.1.
    Expected: NOT GT_OK.

    1.15. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E],
                   out of range actionPtr->redirect.data.vrfId [4096]
                   and other params from 1.1.
    Expected: NOT GT_OK.

    1.16. Call with actionPtr->vlan.modifyVlan [CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E],
                   out of range actionPtr->vlan.vlanId [4096]
                   and other params from 1.1.
    Expected: NOT GT_OK.

    1.17. Call with out of range actionPtr->sourceId.sourceIdValue [32]
                   and other params from 1.1.
    Expected: NOT GT_OK.

    1.18. Check for NULL pointer. For call with actionPtr [NULL], other parameters same as in 1.2.
    Expected: GT_BAD_PTR.

    1.19. Call cpssDxChPclPortGroupRuleInvalidate with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
    ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8                      dev;
    GT_PORT_GROUPS_BMP         portGroupsBmp = 1;
    CPSS_PCL_RULE_SIZE_ENT     ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
    GT_U32                     ruleIndex = 0;
    CPSS_DXCH_PCL_ACTION_STC   action;

    CPSS_PCL_DIRECTION_ENT     direction = CPSS_PCL_DIRECTION_INGRESS_E;
    CPSS_DXCH_PCL_ACTION_STC   retAction;
    GT_BOOL                    failureWas;
    CPSS_PP_FAMILY_TYPE_ENT    devFamily;

    GT_U32    portGroupId;

    pclActionDefaultSet(&action);
    direction = CPSS_PCL_DIRECTION_INGRESS_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            /* get device family */
            st = prvUtfDeviceFamilyGet(dev, &devFamily);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

            /* get max rule size */

            /*
                1.1. Set rule for testing.  Call cpssDxChPclRuleSet with
                ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
                ruleIndex [PCL_TESTED_RULE_INDEX], maskPtr-> ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ],
                patternPtr-> ruleStdNotIp [ common[pclId=0; sourcePort=5; isTagged=1;
                vid=100; up=0; qosProfile=0; isIp=0; isL2Valid=1; isUdbValid =1],
                isIpv4 = 0; etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
                macSa=AB:CD:EF:00:00:01; udb[10,20,30] ],
                actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
                matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
                qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
                qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                profilePrecedence=GT_FALSE] ] },
                redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 },
                vlan { egressTaggedModify=GT_FALSE,
                modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
                Expected: GT_OK
            */
            st = pclRuleTestingDefaultSet(dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPclRuleSet: device: %d", dev);

            /*
                1.2. For all devices update action with legal parameters.
                Call with ruleSize [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX],
                actionPtr is the same as in 1.1.
                Expected: GT_OK
            */
            ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
            ruleIndex = PCL_TESTED_RULE_INDEX;

            st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                            ruleSize, ruleIndex, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                        "device: %d, %d, %d", dev, ruleSize, ruleIndex);

            /*
                1.3. call cpssDxChPclPortGroupRuleActionGet
                with ruleSize[CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX],
                direction [CPSS_PCL_DIRECTION_INGRESS_E], non-NULL actionPtr.
                Expected: GT_OK and same values in actionPtr as were written.
            */
            st = cpssDxChPclPortGroupRuleActionGet(dev, portGroupsBmp, ruleSize,
                                                    ruleIndex, direction, &retAction);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChPclPortGroupRuleActionGet: %d, %d, %d, %d",
                                         dev, ruleSize, ruleIndex, direction);
            if (GT_OK == st)
            {
                /* verifying values */

                /*pktCmd*/
                UTF_VERIFY_EQUAL1_STRING_MAC(action.pktCmd, retAction.pktCmd,
                 "cpssDxChPclPortGroupRuleActionGet: "
                 "get another action.pktCmd than was set: dev = %d", dev);

                if (devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
                {
                    UTF_VERIFY_EQUAL1_STRING_MAC(action.actionStop, retAction.actionStop,
                     "cpssDxChPclPortGroupRuleActionGet: "
                     "get another action.actionStop than was set: dev = %d", dev);
                }

                /*mirror*/
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action.mirror,
                    (const GT_VOID*)&retAction.mirror, sizeof(action.mirror))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                    "cpssDxChPclPortGroupRuleActionGet: "
                    "get another action.mirror than was set: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);

                /*matchCounter*/
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action.matchCounter,
                                                (const GT_VOID*)&retAction.matchCounter,
                                                sizeof(action.matchCounter))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                    "cpssDxChPclPortGroupRuleActionGet: "
                    "get another action.matchCounter than was set: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);

                /*redirect*/
                if (devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
                {
                    UTF_VERIFY_EQUAL3_STRING_MAC(
                        action.redirect.redirectCmd, retAction.redirect.redirectCmd,
                        "cpssDxChPclPortGroupRuleActionGet: "
                        "get another actionPtr->redirect.redirectCmd than was set: %d, %d, %d",
                        dev, ruleSize, ruleIndex);
                    if (action.redirect.redirectCmd
                        == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
                    {
                        UTF_VERIFY_EQUAL3_STRING_MAC(
                            action.redirect.data.outIf.tunnelPtr,
                            retAction.redirect.data.outIf.tunnelPtr,
                            "cpssDxChPclPortGroupRuleActionGet: get another "
                            "actionPtr->redirect.data.outIf.tunnelPtr than was set: %d, %d, %d",
                            dev, ruleSize, ruleIndex);
                        UTF_VERIFY_EQUAL3_STRING_MAC(
                            action.redirect.data.outIf.tunnelStart,
                            retAction.redirect.data.outIf.tunnelStart,
                            "cpssDxChPclPortGroupRuleActionGet: get another "
                            "actionPtr->redirect.data.outIf.tunnelStart than was set: %d, %d, %d",
                            dev, ruleSize, ruleIndex);
                        UTF_VERIFY_EQUAL3_STRING_MAC(
                            action.redirect.data.outIf.tunnelType,
                            retAction.redirect.data.outIf.tunnelType,
                            "cpssDxChPclPortGroupRuleActionGet: get another "
                            "actionPtr->redirect.data.outIf.tunnelType than was set: %d, %d, %d",
                            dev, ruleSize, ruleIndex);
                        UTF_VERIFY_EQUAL3_STRING_MAC(
                            action.redirect.data.outIf.vntL2Echo,
                            retAction.redirect.data.outIf.vntL2Echo,
                            "cpssDxChPclPortGroupRuleActionGet: get another "
                            "actionPtr->redirect.data.outIf.vntL2Echo than was set: %d, %d, %d",
                            dev, ruleSize, ruleIndex);
                    }
                }

                /*policer*/
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action.policer,
                                                (const GT_VOID*)&retAction.policer,
                                                sizeof(action.policer))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                     "cpssDxChPclPortGroupRuleActionGet: get another action.policer than was set: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);

                /*vlan*/
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action.vlan,
                                                (const GT_VOID*)&retAction.vlan,
                                                sizeof(action.vlan))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                     "cpssDxChPclPortGroupRuleActionGet: get another action.vlan than was set: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);

                /*ipUcRoute  - not relevant for Ch2 and above*/
                if (CPSS_PP_FAMILY_CHEETAH_E == devFamily)
                {
                    failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action.ipUcRoute,
                                                    (const GT_VOID*)&retAction.ipUcRoute,
                                                    sizeof(action.ipUcRoute))) ? GT_FALSE : GT_TRUE;
                    UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                     "cpssDxChPclPortGroupRuleActionGet: "
                     "get another action.ipUcRoute than was set: dev: %d, %d, %d",
                                                 dev, ruleSize, ruleIndex);
                }
            }

            /*
                1.4. check with rule size for other type of rule.
                Call with ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], ruleIndex [PCL_TESTED_RULE_INDEX],
                actionPtr is the same as in 1.1.
                Expected: GT_OK.
            */
            ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;
            ruleIndex = PCL_TCAM_RAWS(dev) - 1;

            st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                ruleSize, ruleIndex, &action);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

            ruleSize = CPSS_PCL_RULE_SIZE_STD_E; /* restore valid values */

            /*
                1.5.  Call with ruleSize [wrong enum values], ruleIndex [PCL_TESTED_RULE_INDEX],
                actionPtr is the same as in 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupRuleActionUpdate
                                (dev, portGroupsBmp, ruleSize, ruleIndex, &action),
                                ruleSize);

            /*
                1.6. Check out of range ruleIndex.
                Call with ruleIndex [1024], other parameters is the same as in 1.2.
                Expected: NON GT_OK.
            */
            ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

            st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                        ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

            ruleIndex = PCL_TESTED_RULE_INDEX; /* restore valid value */

            /*
                1.7. Call with out of range actionPtr->matchCounter.matchCounterIndex [32]
                   and other params from 1.1.
                Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3 and above.
            */
            action.matchCounter.enableMatchCount = GT_TRUE;
            action.matchCounter.matchCounterIndex = 32;

            st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                        ruleSize, ruleIndex, &action);

            if(devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->matchCounter.matchCounterIndex = %d",
                          dev, ruleSize, ruleIndex, action.matchCounter.matchCounterIndex);
            }
            else
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->matchCounter.matchCounterIndex = %d",
                          dev, ruleSize, ruleIndex, action.matchCounter.matchCounterIndex);

                /*
                    1.8. Call with out of range actionPtr->matchCounter.matchCounterIndex [16384]
                                      and other params from 1.1.
                    Expected: NOT GT_OK.
                */
                action.matchCounter.matchCounterIndex = 16384;

                st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                            ruleSize, ruleIndex, &action);
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->matchCounter.matchCounterIndex = %d",
                         dev, ruleSize, ruleIndex, action.matchCounter.matchCounterIndex);
            }

            action.matchCounter.matchCounterIndex = 0; /* restore valid value */

            /*
                1.9. Call with out of range actionPtr->policer.policerId [256]
                               and other params from 1.1.
                Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3 and above.
            */
            action.policer.policerEnable = GT_TRUE;
            action.policer.policerId = 256;

            st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                        ruleSize, ruleIndex, &action);

            if(devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->policer.policerId = %d",
                          dev, ruleSize, ruleIndex, action.policer.policerId);
            }
            else
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->policer.policerId = %d",
                              dev, ruleSize, ruleIndex, action.policer.policerId);

                /*
                    1.10. For Ch3 and above call with out of range actionPtr->policer.policerId [4096]
                                   and other params from 1.1.
                    Expected: NOT GT_OK.
                */
                action.policer.policerId = 4096;

                st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                            ruleSize, ruleIndex, &action);
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->policer.policerId = %d",
                          dev, ruleSize, ruleIndex, action.policer.policerId);
            }

            action.policer.policerId = 0; /* restore valid value */

            /*
                1.11. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                               actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                               out of range actionPtr->redirect.outIf.tunnelPtr[1024]
                               and other params from 1.1.
                Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3 and above.
            */
            action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
            action.redirect.data.outIf.tunnelStart = GT_TRUE;
            action.redirect.data.outIf.tunnelPtr = 1024;

            st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                        ruleSize, ruleIndex, &action);
            if (devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.outIf.tunnelPtr = %d",
                          dev, ruleSize, ruleIndex, action.redirect.data.outIf.tunnelPtr);
            }
            else
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->redirect.data.outIf.tunnelPtr = %d",
                               dev, ruleSize, ruleIndex, action.redirect.data.outIf.tunnelPtr);

                /*
                    1.12. Call with actionPtr->redirect.redirectCmd
                                   [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                                   actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                                   out of range actionPtr->redirect.outIf.tunnelPtr[8192]
                                   and other params from 1.1.
                    Expected: NOT GT_OK.
                */
                action.redirect.data.outIf.tunnelPtr = 8192;

                st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                            ruleSize, ruleIndex, &action);
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->redirect.data.outIf.tunnelPtr = %d",
                                 dev, ruleSize, ruleIndex, action.redirect.data.outIf.tunnelPtr);
            }

            action.redirect.data.outIf.tunnelPtr = 0; /* restore valid value */

            /*
                1.13. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                               out of range actionPtr->redirect.data.routerLttIndex[8192]
                               and other params from 1.1.
                Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3 and above.
            */
            action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E;
            action.redirect.data.routerLttIndex = 8192;

            st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                        ruleSize, ruleIndex, &action);

            if ((devFamily <= CPSS_PP_FAMILY_CHEETAH3_E)
                || (devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.routerLttIndex = %d",
                          dev, ruleSize, ruleIndex, action.redirect.data.routerLttIndex);
            }
            else
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->redirect.data.routerLttIndex = %d",
                                 dev, ruleSize, ruleIndex, action.redirect.data.routerLttIndex);

                /*
                    1.14. Call with actionPtr->redirect.redirectCmd
                          [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                          out of range actionPtr->redirect.data.routerLttIndex[32768]
                          and other params from 1.1.
                    Expected: NOT GT_OK.
                */
                action.redirect.data.routerLttIndex = 32768;

                st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                            ruleSize, ruleIndex, &action);
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->redirect.data.routerLttIndex = %d",
                               dev, ruleSize, ruleIndex, action.redirect.data.routerLttIndex);
            }

            action.redirect.data.routerLttIndex = 0; /* restore valid value */

            /*
                1.15. Call with actionPtr->redirect.redirectCmd
                [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E],
                out of range actionPtr->redirect.data.vrfId [4096] and other params from 1.1.
                Expected: NOT GT_OK.
            */
            if ((devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
                && (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E;
                action.redirect.data.vrfId = 4096;

                st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                            ruleSize, ruleIndex, &action);
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->redirect.data.vrfId= %d",
                               dev, ruleSize, ruleIndex, action.redirect.data.vrfId);

                action.redirect.data.vrfId = 0; /* restore valid value */
            }

            if (devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            {
                action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E;
            }

            /*
                1.16. Call with actionPtr->vlan.modifyVlan
                    [CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E],
                               out of range actionPtr->vlan.vlanId [4096] and other params from 1.1.
                Expected: NOT GT_OK.
            */
            action.vlan.modifyVlan = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;
            action.vlan.vlanId = 4096;

            st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                        ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->vlan.vlanId = %d",
                            dev, ruleSize, ruleIndex, action.vlan.vlanId);

            action.vlan.vlanId = 100; /* restore valid value */

            /*
                1.17. Call with out of range actionPtr->sourceId.sourceIdValue [32]
                               and other params from 1.1.
                Expected: NOT GT_OK.
            */
            if (devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            {
                action.sourceId.assignSourceId = GT_TRUE;
                action.sourceId.sourceIdValue = 32;

                st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                            ruleSize, ruleIndex, &action);
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->sourceId.sourceIdValue = %d",
                             dev, ruleSize, ruleIndex, action.sourceId.sourceIdValue);

                action.sourceId.sourceIdValue = 0; /* restore valid value */
            }

            /*
                1.18. Check for NULL pointer. For call with actionPtr [NULL], other
                parameters same as in 1.2.
                Expected: GT_BAD_PTR.
            */

            st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                            ruleSize, ruleIndex, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st,
                                    "%d, %d, %d, NULL", dev, ruleSize, ruleIndex);

            /*
                1.19. call cpssDxChPclPortGroupRuleInvalidate with
                ruleSize [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX]
                to cleanup after testing.
                Expected: GT_OK.
            */
            st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                "cpssDxChPclPortGroupRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                    ruleSize, ruleIndex, &action);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp, ruleSize,
                                                  ruleIndex, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* Set valid parameters. As soon as family of device can't be determined */
    /* - values valid for any family                                         */
    ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
    ruleIndex = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                    ruleSize, ruleIndex, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupRuleActionUpdate(dev, portGroupsBmp,
                                                ruleSize, ruleIndex, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupRuleAnyStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupRuleAnyStateGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxChx)

    1.1. Set rule for testing.
    Expected: GT_OK.

    1.2. Call with ruleSize [CPSS_PCL_RULE_SIZE_EXT_E],
                   ruleIndex [PCL_TESTED_RULE_INDEX],
                   non-NULL validPtr
                   and non-NULL ruleSizePtr.
    Expected: GT_OK.

    1.3. Call with out of range ruleIndex [1024], and other parameters same as in 1.2.
    Expected: NON GT_OK.

    1.4. Call with validPtr [NULL], and other parameters same as in 1.2.
    Expected: GT_BAD_PTR.

    1.5. Call with ruleSizePtr [NULL], and other parameters same as in 1.2.
    Expected: GT_BAD_PTR.

    1.6. Call cpssDxChPclPortGroupRuleInvalidate with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                                             ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
    Expected: GT_OK.

    1.7. Get state for invalidated rule.
         Call with ruleIndex [PCL_TESTED_RULE_INDEX], non-NULL validPtr and non-NULL ruleSizePtr.
    Expected: GT_OK, validPtr = GT_FALSE.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;

    CPSS_PCL_RULE_SIZE_ENT  ruleSize  = CPSS_PCL_RULE_SIZE_STD_E;
    GT_U32                  ruleIndex = 0;
    GT_BOOL                 valid     = GT_FALSE;

    CPSS_PP_FAMILY_TYPE_ENT devFamily;

    GT_U32    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = prvUtfDeviceFamilyGet(dev, &devFamily);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,"prvUtfDeviceFamilyGet: %d", dev);
            /*
                1.1. Set rule for testing.
                Expected: GT_OK.
            */
            st = pclRuleTestingDefaultSet(dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPclPortGroupRuleSet: %d", dev);

            /*
                1.2. Call with ruleSize [CPSS_PCL_RULE_SIZE_STD_E /
                                         CPSS_PCL_RULE_SIZE_EXT_E],
                               ruleIndex [PCL_TCAM_RAWS(dev) - 1],
                               non-NULL validPtr
                               and non-NULL ruleSizePtr.
                Expected: GT_OK.
            */
            ruleIndex = PCL_TCAM_RAWS(dev) - 1;

            /*call with ruleSize  = CPSS_PCL_RULE_SIZE_EXT_E;*/
            ruleSize  = CPSS_PCL_RULE_SIZE_EXT_E;

            st = cpssDxChPclPortGroupRuleAnyStateGet(dev, portGroupsBmp,
                                                    ruleSize, ruleIndex, &valid, &ruleSize);
            if ((st == GT_BAD_STATE) && (devFamily >= CPSS_PP_FAMILY_CHEETAH3_E))
            {
                /* Ch3 and above inititial TCAM state */
                st = GT_OK;
            }
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);


            /*call with ruleSize  = CPSS_PCL_RULE_SIZE_STD_E;*/
            ruleSize  = CPSS_PCL_RULE_SIZE_STD_E;

            st = cpssDxChPclPortGroupRuleAnyStateGet(dev, portGroupsBmp,
                                                    ruleSize, ruleIndex, &valid, &ruleSize);

            if ((st == GT_BAD_STATE) && (devFamily >= CPSS_PP_FAMILY_CHEETAH3_E))
            {
                /* Ch3 and above inititial TCAM state */
                st = GT_OK;
            }
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);


            /*
                1.3. Call with out of range ruleIndex [1024], and other parameters same as in 1.2.
                Expected: NON GT_OK.
            */
            ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

            st = cpssDxChPclPortGroupRuleAnyStateGet(dev, portGroupsBmp,
                                                     ruleSize, ruleIndex, &valid, &ruleSize);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

            ruleIndex = PCL_TESTED_RULE_INDEX; /* restore valid value */

            /*
                1.4. Call with validPtr [NULL], and other parameters same as in 1.2.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupRuleAnyStateGet(dev, portGroupsBmp,
                                                     ruleSize, ruleIndex, NULL, &ruleSize);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                            "%d, %d, validPtr = NULL", dev, ruleIndex);

            /*
                1.5. Call with ruleSizePtr [NULL], and other parameters same as in 1.2.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupRuleAnyStateGet(dev, portGroupsBmp,
                                                    ruleSize, ruleIndex, &valid, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ruleSizePtr = NULL",
                                         dev, ruleIndex);

            /*
                1.6. Call cpssDxChPclPortGroupRuleInvalidate with
                    ruleSize [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX]
                    to cleanup after testing.
                Expected: GT_OK.
            */
            ruleSize = CPSS_PCL_RULE_SIZE_STD_E;

            st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "cpssDxChPclPortGroupRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);

            /*
                1.7. Get state for invalidated rule.
                     Call with ruleIndex [PCL_TESTED_RULE_INDEX],
                     non-NULL validPtr and non-NULL ruleSizePtr.
                Expected: GT_OK, validPtr = GT_FALSE.
            */
            ruleIndex = PCL_TESTED_RULE_INDEX;

            st = cpssDxChPclPortGroupRuleAnyStateGet(dev, portGroupsBmp,
                                                    ruleSize, ruleIndex, &valid, &ruleSize);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupRuleAnyStateGet(dev, portGroupsBmp,
                                                ruleSize, ruleIndex, &valid, &ruleSize);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupRuleAnyStateGet(dev, portGroupsBmp, ruleSize,
                                                 ruleIndex, &valid, &ruleSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* Set valid parameters. As soon as family of device can't be determined */
    /* - values valid for any family                                         */
    ruleSize  = CPSS_PCL_RULE_SIZE_STD_E;
    ruleIndex = PCL_TESTED_RULE_INDEX;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclPortGroupRuleAnyStateGet(dev, portGroupsBmp,
                                                ruleSize, ruleIndex, &valid, &ruleSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupRuleAnyStateGet(dev, portGroupsBmp,
                                            ruleSize, ruleIndex, &valid, &ruleSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupRuleCopy
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_SIZE_ENT        ruleSize,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupRuleCopy)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(dxChx)

    1.1. Set rule for testing. Call cpssDxChPclPortGroupRuleSet with ruleFormat
    [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E], ruleIndex [PCL_TESTED_RULE_INDEX], maskPtr->
    ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00], patternPtr-> ruleStdNotIp [ common[pclId=0;
    sourcePort=5; isTagged=1; vid=100; up=0; qosProfile=0; isIp=0; isL2Valid=1;
    isUdbValid =1], isIpv4 = 0; etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
    macSa=AB:CD:EF:00:00:01; udb[10,20,30] ], actionPtr [ pktCmd =
    CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
    matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
    egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
    ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
    redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] },
    policer { policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
    modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
    precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
    doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
    Expected: GT_OK.

    1.2. Try to copy with ruleSize which isn't applicable
    for standard rule. Call with ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], ruleSrcIndex
    [PCL_TESTED_RULE_INDEX], ruleDstIndex [0].
    Expected: NON GT_OK.

    1.3. Try to copy with invalid enum value for ruleSize. Call with ruleSize [wrong enum
    values], ruleSrcIndex [PCL_TESTED_RULE_INDEX], ruleDstIndex [0].
    Expected: GT_BAD_PARAM.

    1.4. Try to copy with out of range source rule index. Call with ruleSize
    [CPSS_PCL_RULE_SIZE_STD_E], ruleSrcIndex [1024], ruleDstIndex [0].
    Expected: NON GT_OK.

    1.5. Try to copy with out of range destination rule index. Call with ruleSize
    [CPSS_PCL_RULE_SIZE_STD_E], ruleSrcIndex [PCL_TESTED_RULE_INDEX], ruleDstIndex [1024].
    Expected: NON GT_OK.

    1.6. For all active devices try to copy with legal parameters. Call with ruleSize
    [CPSS_PCL_RULE_SIZE_STD_E], ruleSrcIndex [PCL_TESTED_RULE_INDEX], ruleDstIndex [0].
    Expected: GT_OK

    1.7. Invalidate source rule. Call cpssDxChPclPortGroupRuleInvalidate with
    ruleSize [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX].
    Expected: GT_OK.

    1.8. Invalidate destination rule. Call cpssDxChPclPortGroupRuleInvalidate with
    ruleSize [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [0].
    Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  ruleSrcIndex = 0;
    GT_U32                  ruleDstIndex = 0;
    CPSS_PCL_RULE_SIZE_ENT  ruleSize = CPSS_PCL_RULE_SIZE_STD_E;

    GT_U32    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1. set rule for testing.
                Call cpssDxChPclPortGroupRuleSet with
                ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
                ruleIndex [PCL_TESTED_RULE_INDEX], maskPtr-> ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00],
                patternPtr-> ruleStdNotIp [ common[pclId=0; sourcePort=5; isTagged=1;
                vid=100; up=0; qosProfile=0; isIp=0; isL2Valid=1; isUdbValid =1],
                isIpv4 = 0; etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
                macSa=AB:CD:EF:00:00:01; udb[10,20,30] ],
                actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
                matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
                qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
                qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                profilePrecedence=GT_FALSE] ] },
                redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 },
                vlan { egressTaggedModify=GT_FALSE,
                modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
                Expected: GT_OK. */

            st = pclRuleTestingDefaultSet(dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                "cpssDxChPclPortGroupRuleSet: device: %d", dev);

            /*
                1.2. Try to copy with ruleSize which isn't applicable for standard rule.
                Call with ruleSize [CPSS_DXCH_PCL_RULE_SIZE_EXT_E],
                ruleSrcIndex [PCL_TESTED_RULE_INDEX], ruleDstIndex [0].
                Expected: NON GT_OK.
            */
            ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;
            ruleSrcIndex = PCL_TESTED_RULE_INDEX;
            ruleDstIndex = 0;

            st = cpssDxChPclPortGroupRuleCopy(dev, portGroupsBmp,
                                            ruleSize, ruleSrcIndex, ruleDstIndex);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize,
                                            ruleSrcIndex, ruleDstIndex);

            /*
                1.3. Try to copy with invalid enum value for ruleSize.
                Call with ruleSize [wrong enum values],
                ruleSrcIndex [PCL_TESTED_RULE_INDEX], ruleDstIndex [0].
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupRuleCopy
                                (dev, portGroupsBmp, ruleSize, ruleSrcIndex, ruleDstIndex),
                                ruleSize);

            /*
                1.4. Try to copy with out of range source rule index. Call with ruleSize
                [CPSS_DXCH_PCL_RULE_SIZE_STD_E], ruleSrcIndex [wrong value], ruleDstIndex [0].
                Expected: NON GT_OK.
            */
            ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
            ruleSrcIndex = PCL_INVALID_STD_RULE_INDEX(dev);
            ruleDstIndex = 0;

            st = cpssDxChPclPortGroupRuleCopy(dev, portGroupsBmp,
                                            ruleSize, ruleSrcIndex, ruleDstIndex);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleSrcIndex);

            ruleSrcIndex = PCL_TESTED_RULE_INDEX;

            /*
                1.5. For Cheetah devices try to copy with out of range
                destination rule index. Call with ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E],
                ruleSrcIndex [PCL_TESTED_RULE_INDEX], ruleDstIndex [wrong value].
                Expected: NON GT_OK.
            */
            ruleDstIndex = PCL_INVALID_STD_RULE_INDEX(dev);

            st = cpssDxChPclPortGroupRuleCopy(dev, portGroupsBmp,
                                            ruleSize, ruleSrcIndex, ruleDstIndex);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize,
                                            ruleSrcIndex, ruleDstIndex);
            ruleDstIndex = 0;

            /*
                1.6. For all active devices try to copy with legal parameters.
                Call with ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E],
                ruleSrcIndex [PCL_TESTED_RULE_INDEX], ruleDstIndex [0].
                Expected: GT_OK
            */
            ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
            ruleSrcIndex = PCL_TESTED_RULE_INDEX;
            ruleDstIndex = 0;

            st = cpssDxChPclPortGroupRuleCopy(dev, portGroupsBmp,
                                            ruleSize, ruleSrcIndex, ruleDstIndex);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                 "device: %d, %d, %d, %d", dev, ruleSize, ruleSrcIndex, ruleDstIndex);

            /*
                1.7. Invalidate source rule. Call cpssDxChPclPortGroupRuleInvalidate
                with ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX].
                Expected: GT_OK.
            */
            st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleSrcIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssDxChPclPortGroupRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleSrcIndex);

            /*
                1.8. Invalidate destination rule. Call cpssDxChPclPortGroupRuleInvalidate
                with ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E], ruleIndex [0].
                Expected: GT_OK.
            */
            st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleDstIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                "cpssDxChPclPortGroupRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleDstIndex);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupRuleCopy(dev, portGroupsBmp,
                                        ruleSize, ruleSrcIndex, ruleDstIndex);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupRuleCopy(dev, portGroupsBmp, ruleSize,
                                          ruleSrcIndex, ruleDstIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
    ruleSrcIndex = PCL_TESTED_RULE_INDEX;
    ruleDstIndex = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclPortGroupRuleCopy(dev, portGroupsBmp,
                                        ruleSize, ruleSrcIndex, ruleDstIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupRuleCopy(dev, portGroupsBmp,
                                        ruleSize, ruleSrcIndex, ruleDstIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupRuleGet
(
    IN  GT_U8                  devNum,
    IN  GT_PORT_GROUPS_BMP     portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                 ruleIndex,
    OUT GT_U32                 mask[],
    OUT GT_U32                 pattern[],
    OUT GT_U32                 action[]
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupRuleGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(dxChx)

    1.1. Set rule for testing. Call cpssDxChPclPortGroupRuleSet with ruleFormat
    [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E], ruleIndex [PCL_TESTED_RULE_INDEX], maskPtr->
    ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ], patternPtr-> ruleStdNotIp [ common[pclId=0;
    sourcePort=5; isTagged=1; vid=100; up=0; qosProfile=0; isIp=0; isL2Valid=1;
    isUdbValid =1], isIpv4 = 0; etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
    macSa=AB:CD:EF:00:00:01; udb[10,20,30] ], actionPtr [ pktCmd =
    CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
    matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
    egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
    ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
    redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] },
    policer { policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
    modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
    precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
    doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
    Expected: GT_OK.

    1.2. Check with legal parameters. Call with ruleSize
    [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX], non-NULL mask, pattern and action.
    Expected: GT_OK

    1.3. Check for invalid ruleSize for rule which was written as standard rule. Call
    with invalid ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], other params same as in 1.2.
    Expected: NON GT_OK.

    1.4. Check for out-of-range enum ruleSize. call with ruleSize [wrong enum values],
    other params same as in 1.2.
    Expected: GT_BAD_PARAM.

    1.5. Check out of range ruleIndex. Call with ruleIndex [1024], other parameters same as in 1.2.
    Expected: NON GT_OK.

    1.6. Check for NULL pointer. For call with mask [NULL], other parameters same as in 1.2.
    Expected: GT_BAD_PTR.

    1.7. Check for NULL pointer. For call with pattern [NULL], other parameters same as in 1.2.
    Expected: GT_BAD_PTR.

    1.8. Check for NULL pointer. For call with action [NULL], other parameters same as in 1.2.
    Expected: GT_BAD_PTR.

    1.9. Call cpssDxChPclPortGroupRuleInvalidate with ruleSize
    [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
    Expected: GT_OK.

    1.10. Try to get invalidated rule. call with all params same as in 1.2. (ruleIndex [PCL_TESTED_RULE_INDEX])
    Expected: non GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    CPSS_PCL_RULE_SIZE_ENT  ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
    GT_U32                  ruleIndex = 0;
    GT_U32                  mask[12];
    GT_U32                  pattern[12];
    GT_U32                  action[3];

    GT_U32    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1. Set rule for testing.  Call cpssDxChPclPortGroupRuleSet with
                ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
                ruleIndex [PCL_TESTED_RULE_INDEX], maskPtr-> ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ],
                patternPtr-> ruleStdNotIp [ common[pclId=0; sourcePort=5; isTagged=1;
                vid=100; up=0; qosProfile=0; isIp=0; isL2Valid=1; isUdbValid =1],
                isIpv4 = 0; etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
                macSa=AB:CD:EF:00:00:01; udb[10,20,30] ],
                actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
                matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
                qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
                qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                profilePrecedence=GT_FALSE] ] },
                redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 },
                vlan { egressTaggedModify=GT_FALSE,
                modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
                Expected: GT_OK.
            */
            st = pclRuleTestingDefaultSet(dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                        "cpssDxChPclPortGroupRuleSet: device: %d", dev);

            /*
                1.2. Check with legal parameters. Call with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                ruleIndex [PCL_TESTED_RULE_INDEX], non-NULL mask, pattern and action.
                Expected: GT_OK
            */
            ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
            ruleIndex = PCL_TESTED_RULE_INDEX;

            st = cpssDxChPclPortGroupRuleGet(dev, portGroupsBmp, ruleSize,
                                            ruleIndex, mask, pattern, action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "device: %d, %d, %d", dev, ruleSize, ruleIndex);

            /*
                1.3. Check for invalid ruleSize for rule which was written as standard rule.
                Call with invalid ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], other params same as in 1.2.
                Expected: NON GT_OK.
            */
            ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;

            st = cpssDxChPclPortGroupRuleGet(dev, portGroupsBmp, ruleSize,
                                            ruleIndex, mask, pattern, action);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleSize);

            ruleSize = CPSS_PCL_RULE_SIZE_STD_E; /* restore valid value */

            /*
                1.4. Check for wrong enum values ruleSize and other params same as in 1.2.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupRuleGet
                                (dev, portGroupsBmp, ruleSize, ruleIndex, mask, pattern, action),
                                ruleSize);

            /*
                1.5. check out of range ruleIndex.
                Call with ruleIndex [wrong value], other parameters same as in 1.2.
                Expected: NON GT_OK.
            */
            ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

            st = cpssDxChPclPortGroupRuleGet(dev, portGroupsBmp, ruleSize,
                                            ruleIndex, mask, pattern, action);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

            ruleIndex = PCL_TESTED_RULE_INDEX; /* restore valid value */

            /*
                1.6. Call with mask [NULL], other parameters same as in 1.2.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupRuleGet(dev, portGroupsBmp, ruleSize,
                                            ruleIndex, NULL, pattern, action);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mask = NULL", dev);

            /*
                1.7. Call with pattern [NULL], other parameters same as in 1.2.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupRuleGet(dev, portGroupsBmp, ruleSize,
                                            ruleIndex, mask, NULL, action);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pattern = NULL", dev);

            /*
                1.8. Call with action [NULL], other parameters same as in 1.2.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupRuleGet(dev, portGroupsBmp, ruleSize,
                                            ruleIndex, mask, pattern, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, action = NULL", dev);

            /*
                1.9. Call cpssDxChPclPortGroupRuleInvalidate with
                        ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                        ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
                Expected: GT_OK.
            */
            st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChPclPortGroupRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupRuleGet(dev, portGroupsBmp, ruleSize,
                                            ruleIndex, mask, pattern, action);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupRuleGet(dev, portGroupsBmp, ruleSize, ruleIndex,
                                             mask, pattern, action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    ruleIndex = 0;
    ruleSize = CPSS_PCL_RULE_SIZE_STD_E;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclPortGroupRuleGet(dev, portGroupsBmp, ruleSize,
                                            ruleIndex, mask, pattern, action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupRuleGet(dev, portGroupsBmp, ruleSize,
                                        ruleIndex, mask, pattern, action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupRuleInvalidate
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_SIZE_ENT        ruleSize,
    IN GT_U32                             ruleIndex
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupRuleInvalidate)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(dxChx)

    1.1. Set rule for testing. Call cpssDxChPclPortGroupRuleSet with ruleFormat
    [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E], ruleIndex [PCL_TESTED_RULE_INDEX], maskPtr->
    ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ], patternPtr-> ruleStdNotIp [ common[pclId=0;
    sourcePort=5; isTagged=1; vid=100; up=0; qosProfile=0; isIp=0; isL2Valid=1;
    isUdbValid =1], isIpv4 = 0; etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
    macSa=AB:CD:EF:00:00:01; udb[10,20,30] ], actionPtr [ pktCmd =
    CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
    matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
    egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
    ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
    redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] },
    policer { policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
    modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
    precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
    doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
    Expected: GT_OK.

    1.2. Check for invalid ruleSize for rule which was written as standard rule. Call
    with invalid ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], ruleIndex [PCL_TESTED_RULE_INDEX].
    Expected: NON GT_OK.

    1.3. Check for invalid enum value for ruleSize.
    Call with ruleSize [wrong enum values], ruleIndex [PCL_TESTED_RULE_INDEX].
    Expected: GT_BAD_PARAM.

    1.4. Check for out of range ruleIndex. Call with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
    ruleIndex [1024].
    Expected: NON GT_OK.

    1.5. Invalidate written rule with legal parameters. Call with ruleSize
    [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX].
    Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    CPSS_PCL_RULE_SIZE_ENT  ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
    GT_U32                  ruleIndex = 0;

    GT_U32    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1. Set rule for testing. Call cpssDxChPclPortGroupRuleSet with
                ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
                ruleIndex [PCL_TESTED_RULE_INDEX], maskPtr-> ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ],
                patternPtr-> ruleStdNotIp [ common[pclId=0; sourcePort=5; isTagged=1;
                vid=100; up=0; qosProfile=0; isIp=0; isL2Valid=1; isUdbValid =1],
                isIpv4 = 0; etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
                macSa=AB:CD:EF:00:00:01; udb[10,20,30] ],
                actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
                matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
                qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
                qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                profilePrecedence=GT_FALSE] ] },
                redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 },
                vlan { egressTaggedModify=GT_FALSE,
                modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
                Expected: GT_OK.
            */

            st = pclRuleTestingDefaultSet(dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                        "cpssDxChPclPortGroupRuleSet: device: %d", dev);

            /*
                1.2. Check for invalid ruleSize for rule
                which was written as standard rule. Call with invalid
                ruleSize [CPSS_DXCH_PCL_RULE_SIZE_EXT_E], ruleIndex [PCL_TESTED_RULE_INDEX].
                Expected: GT_OK.
            */
            ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;
            ruleIndex = PCL_TCAM_RAWS(dev) - 1;

            st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

            /*
                1.3. Invalidate written rule with legal parameters.
                Call with ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX].
                Expected: GT_OK.
            */
            ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
            ruleIndex = PCL_TESTED_RULE_INDEX;

            st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

            /*
                1.4. Check for invalid enum value for
                ruleSize. Call with ruleSize [wrong enum values], ruleIndex [PCL_TESTED_RULE_INDEX].
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupRuleInvalidate
                                (dev, portGroupsBmp, ruleSize, ruleIndex),
                                ruleSize);

            /*
                1.5. Check for out of range ruleIndex.
                Call with ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E], ruleIndex [wrong value].
                Expected: NON GT_OK.
            */
            ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
            ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

            st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
        ruleIndex = 0;

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
    ruleIndex = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupRuleSet
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupRuleSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(dxChx)

    1.1. Check standard rule for all active devices with legal parameters.
    Call with   ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
                ruleIndex [PCL_TESTED_RULE_INDEX],
                maskPtr-> ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ],
                patternPtr-> ruleStdNotIp [ common[pclId=0;
                                                    sourcePort=5;
                                                    isTagged=1;
                                                    vid=100;
                                                    up=0;
                                                    qosProfile=0;
                                                    isIp=0;
                                                    isL2Valid=1;
                                                    isUdbValid =1],
                                            isIpv4 = 0;
                                            etherType=0;
                                            isArp=0;
                                            l2Encap=0;
                                            macDa=AB:CD:EF:00:00:02;
                                            macSa=AB:CD:EF:00:00:01;
                                            udb[10,20,30] ],
                actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                            mirror{cpuCode = 0,
                                   mirrorToRxAnalyzerPort = GT_FALSE},
                            matchCounter { enableMatchCount = GT_FALSE,
                                           matchCounterIndex = 0 },
                            qos { egressPolicy=GT_FALSE,
                                  modifyDscp=GT_FALSE,
                                  modifyUp=GT_FALSE ,
                            qos [ ingress[profileIndex=0,
                                  profileAssignIndex=GT_FALSE,
                                  profilePrecedence=GT_FALSE] ] },
                            redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                            data[routerLttIndex=0] },
                            policer { policerEnable=GT_FALSE, policerId=0 },
                            vlan {  egressTaggedModify=GT_FALSE,
                                    modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                                    nestedVlan=GT_FALSE,
                                    vlanId=100,
                                    precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                            ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
    Expected: GT_OK

    1.2. Check for out of range ruleIndex for standard rule format.
    Call with ruleIndex [1024], other parameters is the same as in 1.1.
    Expected: NON GT_OK.

    1.3. Call with out of range actionPtr->matchCounter.matchCounterIndex [32]
    and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for other.

    1.4. For Ch3 and above call with out of range
    actionPtr->matchCounter.matchCounterIndex [16384] and other params from 1.1.
    Expected: NOT GT_OK.

    1.5. Call with out of range actionPtr->policer.policerId [256] and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for other.

    1.6. For non ch1/ch2 call with out of range actionPtr->policer.policerId [4096]
    and other params from 1.1.
    Expected: NOT GT_OK.

    1.7. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                   actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                   out of range actionPtr->redirect.outIf.tunnelPtr[1024]
                   and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3 and above.

    1.8. For Ch3 and above call with actionPtr->redirect.redirectCmd
                   [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                   actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                   out of range actionPtr->redirect.outIf.tunnelPtr[8192]
                   and other params from 1.1.
    Expected: NOT GT_OK.

    1.9. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                   out of range actionPtr->redirect.data.routerLttIndex[8192]
                   and other params from 1.1.
    Expected: NOT GT_OK for Ch1/Ch2/Ch3 and GT_OK for other.

    1.10. For xCat and above call with actionPtr->redirect.redirectCmd
                   [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                   out of range actionPtr->redirect.data.routerLttIndex[32768]
                   and other params from 1.1.
    Expected: NOT GT_OK.

    1.11. For Ch3  and above call with
          actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E],
          out of range actionPtr->redirect.data.vrfId [4096] and other params from 1.1.
    Expected: NOT GT_OK.

    1.12. Call with actionPtr->vlan.modifyVlan [CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E],
                   out of range actionPtr->vlan.vlanId [4096]
                   and other params from 1.1.
    Expected: NOT GT_OK.

    1.13. For Ch3 and above call with out of range actionPtr->sourceId.sourceIdValue [32]
                   and other params from 1.1.
    Expected: NOT GT_OK.

    1.14. Check for NULL pointers. Call with maskPtr [NULL],
    other parameters is the same as in 1.1.
    Expected: GT_BAD_PTR.

    1.15. Check for NULL pointers. Call with patternPtr [NULL],
    other parameters is the same as in 1.1.
    Expected: GT_BAD_PTR.

    1.16. Check for NULL pointers. Call with actionPtr [NULL],
    other parameters is the same as in 1.1.
    Expected: GT_BAD_PTR.

    1.17. Check for invalid ruleFormat. Call with ruleFormat [wrong enum
    values], other parameters is the same as in 1.1.
    Expected: GT_BAD_PARAM.

    1.18. Call cpssDxChPclPortGroupRuleInvalidate with ruleSize
    [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
    Expected: GT_OK.

    1.19. Check extended rule.
    For all active device ids call with
                ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E],
                ruleIndex [511],
                maskPtr-> ruleExtNotIpv6.common is the same as in 1.1., other fileds are zeroed,
                patternPtr->ruleExtNotIpv6 [ common[the same as in 1.1],
                        commonExt[ isIpv6=0,
                                   ipProtocol=1,
                                   dscp=0,
                                   isL4Valid=1,
                                   l4Byte0=0,
                                   l4Byte1=0,
                                   l4Byte2=0,
                                   l4Byte3=0,
                                   l4Byte13=0,
                                   ipHeaderOk=1 ],
                        sip={ 64.233.167.1},
                        dip={ 213.180.204.1},
                        etherType=0,
                        l2Encap=1,
                        macDa=AB:CD:EF:00:00:02,
                        macSa=AB:CD:EF:00:00:01,
                        ipv4Fragmented=1,
                        udb[6]={10,20,30,40,50,60} ],
                 actionPtr is the same as in 1.1.
    Expected: GT_OK

    1.20. Check for out of range ruleIndex for extended rule format. Call with ruleIndex
    [512], other parameters is the same as 1.8.
    Expected: NON GT_OK.

    1.21. Set standard rule with ruleIndex which is not allowed because of extended rule
    consumes two spaces of standard rule. Call with ruleIndex [511+512], other parameters
    is the same as in 1.1.
    Expected: NON GT_OK.

    1.22. call cpssDxChPclPortGroupRuleInvalidate with ruleSize
    [CPSS_PCL_RULE_SIZE_EXT_E], ruleIndex [511] to cleanup after testing.
    Expected: GT_OK.

    1.23. Check standard EGRESS rule for all active devices with legal parameters.
    Call with   ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E],
                ruleIndex [PCL_TESTED_RULE_INDEX],
                maskPtr-> ruleEgrStdNotIp [0xFF, , 0xFF],
                patternPtr-> ruleEgrStdNotIp [common[pclId=0;
                                                    sourcePort=5;
                                                    isTagged=1;
                                                    vid=100;
                                                    up=0;
                                                    isIp=0;
                                                    isL2Valid =1;
                                                    egrPacketType=0;
                                                    cpuCode=1;
                                                    srcTrg=1;
                                                    srcDev = 10;
                                                    sourceId = 1;
                                                    isVidx=0;
                                                    others -zeroid],
                                              isIpv4 = 0;
                                              etherType=0;
                                              isArp=0;
                                              l2Encap=0;
                                              macDa=AB:CD:EF:00:00:02;
                                              macSa=AB:CD:EF:00:00:01],
                actionPtr - the same as in 1.1.
    Expected: GT_OK for Cheetah2 and above devices.

    1.24. For Cheetah2 and above devices call cpssDxChPclPortGroupRuleInvalidate with ruleSize
    [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E], ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
    Expected: GT_OK.

    1.25. Check extended EGRESS rule for all active devices with legal parameters. Call
    with ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E], ruleIndex [511],
    maskPtr-> ruleEgrStdNotIp [0xFF, , 0xFF], patternPtr-> ruleEgrStdNotIp [common
        [pclId=0; sourcePort=5; isTagged=1; vid=100; up=0; isIp=0; isL2Valid =1;
         egrPacketType=0; cpuCode=1;srcTrg=1; srcDev = 10; sourceId = 1; isVidx=0; others
         -zeroid], commonExt [isIpv6=1; ipProtocol=1; dscp=2; isL4Valid=0;
         egrTcpUdpPortComparator=0; others- theroid] sip[100; 1000; 10000; 100000];
                                              dipBits127to120=0xAA;
                                              macDa=AB:CD:EF:00:00:02;
                                              macSa=AB:CD:EF:00:00:01],
                                            actionPtr - the same as in 1.1.
    Expected: GT_OK for Cheetah2 and above

    1.26. For Cheetah2 and above devices call cpssDxChPclPortGroupRuleInvalidate with
    ruleSize [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E],
    ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
    Expected: GT_OK.
*/
    GT_STATUS  st = GT_OK;
    GT_U32     portGroupId;

    GT_U8                               dev;
    GT_PORT_GROUPS_BMP                  portGroupsBmp = 1;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat =
                                             CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    GT_U32                              ruleIndex = 0;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       mask;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       pattern;
    CPSS_DXCH_PCL_ACTION_STC            action;
    GT_ETHERADDR                        mac1 = {{0xAB, 0xCD, 0xEF, 0x00, 0x00, 0x01}};
    GT_ETHERADDR                        mac2 = {{0xAB, 0xCD, 0xEF, 0x00, 0x00, 0x02}};

    CPSS_PP_FAMILY_TYPE_ENT             devFamily;
    CPSS_PCL_RULE_SIZE_ENT              ruleSize;

    pclActionDefaultSet(&action);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = prvUtfDeviceFamilyGet(dev, &devFamily);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,"prvUtfDeviceFamilyGet: %d", dev);

            /* 1.1. Check standard rule for all active devices with legal
            parameters. Call with
            ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
            ruleIndex [PCL_TESTED_RULE_INDEX], maskPtr-> ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ],
            patternPtr-> ruleStdNotIp [ common[pclId=0; sourcePort=5; isTagged=1;
            vid=100; up=0; qosProfile=0; isIp=0; isL2Valid=1; isUdbValid =1],
            isIpv4 = 0; etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
            macSa=AB:CD:EF:00:00:01; udb[10,20,30] ],
            actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
            mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
            matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
            qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
            qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
            profilePrecedence=GT_FALSE] ] },
            redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
            data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 },
            vlan { egressTaggedModify=GT_FALSE,
            modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
            vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
            ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
            Expected: GT_OK for  devices and GT_BAD_PARAM for others. */
            ruleFormat = PCL_TESTED_RULE_FORMAT;
            ruleIndex = PCL_TESTED_RULE_INDEX;

            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            mask.ruleStdNotIp.common.pclId = 0xFFFF;

            cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
            pattern.ruleStdNotIp.common.pclId = 0;
            pattern.ruleStdNotIp.common.sourcePort = 5;
            pattern.ruleStdNotIp.common.isTagged = 1;
            pattern.ruleStdNotIp.common.vid = 100;
            pattern.ruleStdNotIp.common.up = 0;
            pattern.ruleStdNotIp.common.qosProfile = 0;
            pattern.ruleStdNotIp.common.isIp = 0;
            pattern.ruleStdNotIp.common.isL2Valid = 1;
            pattern.ruleStdNotIp.common.isUdbValid = 1;
            pattern.ruleStdNotIp.isIpv4 = 0;
            pattern.ruleStdNotIp.etherType = 0;
            pattern.ruleStdNotIp.isArp = 0;
            pattern.ruleStdNotIp.l2Encap = 0;
            pattern.ruleStdNotIp.macDa = mac2;
            pattern.ruleStdNotIp.macSa = mac1;
            pattern.ruleStdNotIp.udb[0] = 10;
            pattern.ruleStdNotIp.udb[1] = 20;
            pattern.ruleStdNotIp.udb[2] = 30;

            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "device: %d, %d, %d", dev, ruleFormat, ruleIndex);

            /*
                1.2. Check for out of range ruleIndex for standard rule format.
                Call with ruleIndex [1024], other parameters is the same as in 1.1.
                Expected: NON GT_OK.
            */
            ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

            ruleIndex = PCL_TESTED_RULE_INDEX; /* restore valid value */

            /*
                1.3. Call with out of range actionPtr->matchCounter.matchCounterIndex [32]
                   and other params from 1.1.
                Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for other.
            */
            action.matchCounter.enableMatchCount = GT_TRUE;
            action.matchCounter.matchCounterIndex = 32;

            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);

            if(devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->matchCounter.matchCounterIndex = %d",
                        dev, ruleFormat, ruleIndex, action.matchCounter.matchCounterIndex);
            }
            else
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->matchCounter.matchCounterIndex = %d",
                        dev, ruleFormat, ruleIndex, action.matchCounter.matchCounterIndex);

                /*
                    1.4. For not ch1/ch2 call with out of range
                    actionPtr->matchCounter.matchCounterIndex [16384] and other params from 1.1.
                    Expected: NOT GT_OK.
                */
                action.matchCounter.matchCounterIndex = 16384;

                st = cpssDxChPclPortGroupRuleSet(
                    dev, portGroupsBmp,
                    ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                    &mask, &pattern, &action);

                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->matchCounter.matchCounterIndex = %d",
                        dev, ruleFormat, ruleIndex, action.matchCounter.matchCounterIndex);
            }

            action.matchCounter.matchCounterIndex = 0; /* restore valid value */

            /*
                1.5. Call with out of range actionPtr->policer.policerId [256]
                               and other params from 1.1.
                Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for other.
            */
            action.policer.policerEnable = GT_TRUE;
            action.policer.policerId = 256;

            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);

            if(devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->policer.policerId = %d",
                                 dev, ruleFormat, ruleIndex, action.policer.policerId);
            }
            else
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->policer.policerId = %d",
                                dev, ruleFormat, ruleIndex, action.policer.policerId);

                /*
                    1.6. For non ch1/ch2 call with out of range
                    actionPtr->policer.policerId [4096] and other params from 1.1.
                    Expected: NOT GT_OK.
                */
                action.policer.policerId = 4096;

                st = cpssDxChPclPortGroupRuleSet(
                    dev, portGroupsBmp,
                    ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                    &mask, &pattern, &action);

                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->policer.policerId = %d",
                                dev, ruleFormat, ruleIndex, action.policer.policerId);
            }

            action.policer.policerId = 0; /* restore valid value */

            /*
                1.7. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                               actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                               out of range actionPtr->redirect.outIf.tunnelPtr[1024]
                               and other params from 1.1.
                Expected: NOT GT_OK for Ch1/Ch2 and GT_OK for Ch3 and above.
            */
            action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
            action.redirect.data.outIf.tunnelStart = GT_TRUE;
            action.redirect.data.outIf.tunnelPtr = 1024;

            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern,&action);

            if (devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.outIf.tunnelPtr = %d",
                           dev, ruleFormat, ruleIndex, action.redirect.data.outIf.tunnelPtr);
            }
            else
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.outIf.tunnelPtr = %d",
                          dev, ruleFormat, ruleIndex, action.redirect.data.outIf.tunnelPtr);

                /*
                    1.8. For Ch3 and above call with actionPtr->redirect.redirectCmd
                                    [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E],
                                   actionPtr->redirect.outIf.tunnelStart[GT_TRUE],
                                   out of range actionPtr->redirect.outIf.tunnelPtr[8192]
                                   and other params from 1.1.
                    Expected: NOT GT_OK.
                */
                action.redirect.data.outIf.tunnelPtr = 8192;

                st = cpssDxChPclPortGroupRuleSet(
                    dev, portGroupsBmp,
                    ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                    &mask, &pattern,&action);

                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.outIf.tunnelPtr = %d",
                          dev, ruleFormat, ruleIndex, action.redirect.data.outIf.tunnelPtr);
            }

            action.redirect.data.outIf.tunnelPtr = 0; /* restore valid value */

            /*
                1.9. Call with actionPtr->redirect.redirectCmd [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                               out of range actionPtr->redirect.data.routerLttIndex[8192]
                               and other params from 1.1.
                Expected: NOT GT_OK for Ch1/Ch2/ch3.
            */
            action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E;
            action.redirect.data.routerLttIndex = 8192;
            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);

            if ((devFamily <= CPSS_PP_FAMILY_CHEETAH3_E)
                || (devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.routerLttIndex = %d",
                          dev, ruleFormat, ruleIndex, action.redirect.data.routerLttIndex);
            }
            else
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.routerLttIndex = %d",
                           dev, ruleFormat, ruleIndex, action.redirect.data.routerLttIndex);

                /*
                    1.10. For xCat and above call with actionPtr->redirect.redirectCmd
                    [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E],
                    out of range actionPtr->redirect.data.routerLttIndex[32768]
                    and other params from 1.1.
                    Expected: NOT GT_OK.
                */
                action.redirect.data.routerLttIndex = 32768;

                st = cpssDxChPclPortGroupRuleSet(
                    dev, portGroupsBmp,
                    ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                    &mask, &pattern, &action);

                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->redirect.data.routerLttIndex = %d",
                           dev, ruleFormat, ruleIndex, action.redirect.data.routerLttIndex);
            }

            action.redirect.data.routerLttIndex = 0; /* restore valid value */
            if (devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            {
                action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
                action.redirect.data.outIf.outInterface.type = CPSS_INTERFACE_PORT_E;
                action.redirect.data.outIf.outInterface.devPort.devNum = dev;
                action.redirect.data.outIf.outInterface.devPort.portNum = 0;
                action.redirect.data.outIf.tunnelStart = GT_FALSE;
                action.redirect.data.outIf.vntL2Echo = GT_FALSE;
            }

            /*
                1.11. For Ch3 and above call with actionPtr->redirect.redirectCmd
                [CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E],
                out of range actionPtr->redirect.data.vrfId [4096] and other params from 1.1.
                Expected: NOT GT_OK.
            */
            if ((devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
                && (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E;
                action.redirect.data.vrfId = 4096;

                st = cpssDxChPclPortGroupRuleSet(
                    dev, portGroupsBmp,
                    ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                    &mask, &pattern, &action);

                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->redirect.data.vrfId= %d",
                               dev, ruleFormat, ruleIndex, action.redirect.data.vrfId);

                action.redirect.data.vrfId = 0; /* restore valid value */
            }

            if (devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            {
                action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E;
            }

            /*
                1.12. Call with actionPtr->vlan.modifyVlan [CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E],
                               out of range actionPtr->vlan.vlanId [4096]
                               and other params from 1.1.
                Expected: NOT GT_OK.
            */
            action.vlan.modifyVlan = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;
            action.vlan.vlanId = 4096;

            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);

            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "%d, %d, %d, actionPtr->vlan.vlanId = %d",
                           dev, ruleFormat, ruleIndex, action.vlan.vlanId);

            action.vlan.vlanId = 100; /* restore valid value */

            /*
                1.13. For Ch3 and above call with out of range
                actionPtr->sourceId.sourceIdValue [32] and other params from 1.1.
                Expected: NOT GT_OK.
            */
            if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            {
                action.sourceId.assignSourceId = GT_TRUE;
                action.sourceId.sourceIdValue = 32;

                st = cpssDxChPclPortGroupRuleSet(
                    dev, portGroupsBmp,
                    ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                    &mask, &pattern, &action);

                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "%d, %d, %d, actionPtr->sourceId.sourceIdValue = %d",
                                dev, ruleFormat, ruleIndex, action.sourceId.sourceIdValue);

                action.sourceId.sourceIdValue = 0; /* restore valid value */
            }

            /*
                1.14. Check for NULL pointers.  Call with maskPtr [NULL],
                other parameters is the same as in 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                NULL, &pattern, &action);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st,
                  "%d, %d, %d, maskPtr = NULL", dev, ruleFormat, ruleIndex);

            /*
                1.15. Check for NULL pointers.  Call with patternPtr [NULL],
                other parameters is the same as in 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, NULL, &action);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st,
                     "%d, %d, %d, patternPtr = NULL", dev, ruleFormat, ruleIndex);

            /*
                1.16. Check for NULL pointers. Call with actionPtr [NULL],
                other parameters is the same as in 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, NULL);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st,
                    "%d, %d, %d, actionPtr = NULL", dev, ruleFormat, ruleIndex);

            /*
                1.17. check for invalid ruleFormat.  Call with ruleFormat [wrong enum values],
                other parameters is the same as in 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupRuleSet
                                (dev, portGroupsBmp, ruleFormat,
                                 ruleIndex, 0 /*ruleOptionsBmp*/,
                                 &mask, &pattern, &action),
                                ruleFormat);

            /*
                1.18. For  devices call cpssDxChPclPortGroupRuleInvalidate
                with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
                Expected: GT_OK.
            */
            ruleSize  = CPSS_PCL_RULE_SIZE_STD_E;
            ruleIndex = PCL_TESTED_RULE_INDEX;

            st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChPclPortGroupRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);

            /*
                1.19. Check extended rule. For all active device ids call with
                ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E],
                ruleIndex [511], maskPtr-> ruleExtNotIpv6.common is the same as in 1.1.,
                other fileds are zeroed, patternPtr->ruleExtNotIpv6 [ common[the same as in 1.1],
                commonExt[ isIpv6=0, ipProtocol=1, dscp=0, isL4Valid=1, l4Byte0=0, l4Byte1=0,
                l4Byte2=0, l4Byte3=0, l4Byte13=0, ipHeaderOk=1 ], sip={ 64.233.167.1},
                dip={ 213.180.204.1},  etherType=0, l2Encap=1, macDa=AB:CD:EF:00:00:02,
                macSa=AB:CD:EF:00:00:01, ipv4Fragmented=1, udb[6]={10,20,30,40,50,60} ],
                actionPtr is the same as in 1.1.
                Expected: GT_OK
            */
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
            ruleIndex = 511;

            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            mask.ruleExtNotIpv6.common.pclId = 0xFFFF;

            pattern.ruleExtNotIpv6.common.pclId = 0;
            pattern.ruleExtNotIpv6.common.sourcePort = 5;
            pattern.ruleExtNotIpv6.common.isTagged = 1;
            pattern.ruleExtNotIpv6.common.vid = 100;
            pattern.ruleExtNotIpv6.common.up = 0;
            pattern.ruleExtNotIpv6.common.qosProfile = 0;
            pattern.ruleExtNotIpv6.common.isIp = 0;
            pattern.ruleExtNotIpv6.common.isL2Valid = 1;
            pattern.ruleExtNotIpv6.common.isUdbValid = 1;

            pattern.ruleExtNotIpv6.commonExt.isIpv6 = 0;
            pattern.ruleExtNotIpv6.commonExt.ipProtocol = 1;
            pattern.ruleExtNotIpv6.commonExt.dscp = 0;
            pattern.ruleExtNotIpv6.commonExt.isL4Valid = 1;
            pattern.ruleExtNotIpv6.commonExt.l4Byte0 = 0;
            pattern.ruleExtNotIpv6.commonExt.l4Byte1 = 0;
            pattern.ruleExtNotIpv6.commonExt.l4Byte2 = 0;
            pattern.ruleExtNotIpv6.commonExt.l4Byte3 = 0;
            pattern.ruleExtNotIpv6.commonExt.l4Byte13 = 0;
            pattern.ruleExtNotIpv6.commonExt.ipHeaderOk = 1;

            pattern.ruleExtNotIpv6.sip.arIP[0]= 64;
            pattern.ruleExtNotIpv6.sip.arIP[1]= 233;
            pattern.ruleExtNotIpv6.sip.arIP[2]= 167;
            pattern.ruleExtNotIpv6.sip.arIP[3]= 1;

            pattern.ruleExtNotIpv6.dip.arIP[0]= 213;
            pattern.ruleExtNotIpv6.dip.arIP[1]= 180;
            pattern.ruleExtNotIpv6.dip.arIP[2]= 204;
            pattern.ruleExtNotIpv6.dip.arIP[3]= 1;

            pattern.ruleExtNotIpv6.etherType = 0;
            pattern.ruleExtNotIpv6.l2Encap = 1;
            pattern.ruleExtNotIpv6.macDa = mac2;
            pattern.ruleExtNotIpv6.macSa = mac1;
            pattern.ruleExtNotIpv6.ipv4Fragmented = 1;

            pattern.ruleExtNotIpv6.udb[0] = 10;
            pattern.ruleExtNotIpv6.udb[1] = 20;
            pattern.ruleExtNotIpv6.udb[2] = 30;
            pattern.ruleExtNotIpv6.udb[3] = 40;
            pattern.ruleExtNotIpv6.udb[4] = 50;
            pattern.ruleExtNotIpv6.udb[5] = 60;

            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "device: %d, %d, %d", dev, ruleFormat, ruleIndex);


            /*
                1.20. Check for out of range ruleIndex for extended rule format.
                Call with ruleIndex [wrong value], other parameters is the same as 1.8.
                Expected: NON GT_OK.
            */
            ruleIndex = PCL_INVALID_EXT_RULE_INDEX(dev);

            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);

            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

            ruleIndex = 511; /* restore valid value */

            /*
                1.21. set standard rule with ruleIndex which is not allowed because of extended
                rule consumes two spaces of standard rule. Call with ruleIndex [wrong value],
                other parameters is the same as in 1.1.
                Expected: NON GT_OK.
            */
            ruleFormat = PCL_TESTED_RULE_FORMAT;
            ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            mask.ruleStdNotIp.common.pclId = 0xFFFF;

            /* pattern.ruleStdNotIp was set previously and wasn't cleaned */

            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);

            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

            /* restore valid values */
            ruleIndex = 511;
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;

            /*
                1.22. call cpssDxChPclPortGroupRuleInvalidate
                with ruleSize [CPSS_PCL_RULE_SIZE_EXT_E], ruleIndex [511] to cleanup after testing.
                Expected: GT_OK.
            */
            ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;

            st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                  "cpssDxChPclPortGroupRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);

            /*
                1.23. Check standard EGRESS rule for all active devices with legal parameters.
                Call with ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E],
                ruleIndex [PCL_TESTED_RULE_INDEX],
                maskPtr-> ruleEgrStdNotIp [0xFF, , 0xFF], patternPtr-> ruleEgrStdNotIp
                [ common[pclId=0; sourcePort=5; isTagged=1; vid=100; up=0;
                isIp=0; isL2Valid =1;  egrPacketType=0; cpuCode=1;srcTrg=1;
                srcDev = 10; sourceId = 1; isVidx=0; others -zeroid], isIpv4 = 0;
                etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
                macSa=AB:CD:EF:00:00:01], actionPtr - the same as in 1.1.
                Expected: GT_OK ch2 and above
            */
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E;
            ruleIndex = PCL_TESTED_RULE_INDEX;

            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            cpssOsMemSet((GT_VOID*)&(mask.ruleEgrStdNotIp), 0xFF, sizeof(mask.ruleEgrStdNotIp));

            cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
            pattern.ruleEgrStdNotIp.common.pclId = 0;
            pattern.ruleEgrStdNotIp.common.sourcePort = 5;
            pattern.ruleEgrStdNotIp.common.isTagged = 1;
            pattern.ruleEgrStdNotIp.common.vid = 100;
            pattern.ruleEgrStdNotIp.common.up = 0;
            pattern.ruleEgrStdNotIp.common.isIp = 0;
            pattern.ruleEgrStdNotIp.common.isL2Valid = 1;
            pattern.ruleEgrStdNotIp.common.egrPacketType = 0;
            pattern.ruleEgrStdNotIp.common.egrPktType.toCpu.cpuCode = 1;
            pattern.ruleEgrStdNotIp.common.egrPktType.toCpu.srcTrg = 1;
            pattern.ruleEgrStdNotIp.common.srcDev = 10;
            pattern.ruleEgrStdNotIp.common.sourceId = 1;
            pattern.ruleEgrStdNotIp.common.isVidx = 0;
            pattern.ruleEgrStdNotIp.common.sourceId = 0;

            pattern.ruleEgrStdNotIp.isIpv4 = 0;
            pattern.ruleEgrStdNotIp.etherType = 0;
            pattern.ruleEgrStdNotIp.isArp = 0;
            pattern.ruleEgrStdNotIp.l2Encap = 0;
            pattern.ruleEgrStdNotIp.macDa = mac2;
            pattern.ruleEgrStdNotIp.macSa = mac1;

            if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            {   /* supported on CHEETAH 3 */
                action.qos.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                action.qos.modifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            }

            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);

            if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "device: %d, %d, %d", dev, ruleFormat, ruleIndex);
            }
            else
            {
                /* for CH1 result ignored EPCL not supported     */
                /* but EPCL rule formats translated to HW format */
            }

            if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                /*
                    1.13. For Cheetah2 and above devices call cpssDxChPclPortGroupRuleInvalidate with
                    ruleSize [CPSS_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX]
                    to cleanup after testing.
                    Expected: GT_OK.
                */
                ruleSize = CPSS_PCL_RULE_SIZE_STD_E;

                st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "cpssDxChPclPortGroupRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
            }

            /*
                1.24. Check extended EGRESS rule for all active devices with legal parameters.
                Call with ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E],
                ruleIndex [511], maskPtr-> ruleEgrExtIpv6L2 [0xFF, , 0xFF],
                patternPtr-> ruleEgrExtIpv6L2 [common [pclId=0; sourcePort=5;
                isTagged=1; vid=100; up=0; isIp=1; isL2Valid =1;  egrPacketType=0;
                cpuCode=1;srcTrg=1; srcDev = 10; sourceId = 1; isVidx=0;
                others -zeroid], commonExt [isIpv6=1; ipProtocol=1; dscp=2;
                isL4Valid=0; egrTcpUdpPortComparator=0; others- theroid]
                sip[100; 1000; 10000; 100000]; dipBits127to120=0xAA;
                macDa=AB:CD:EF:00:00:02; macSa=AB:CD:EF:00:00:01],
                actionPtr - the same as in 1.1.
                Expected: GT_OK for Cheetah2 and above
            */
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E;
            ruleIndex = 511;

            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            cpssOsMemSet((GT_VOID*)&(mask.ruleEgrExtIpv6L2), 0xFF, sizeof(mask.ruleEgrExtIpv6L2));

            cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
            pattern.ruleEgrExtIpv6L2.common.pclId = 0;
            pattern.ruleEgrExtIpv6L2.common.sourcePort = 5;
            pattern.ruleEgrExtIpv6L2.common.sourceId = 0;
            pattern.ruleEgrExtIpv6L2.common.isTagged = 1;
            pattern.ruleEgrExtIpv6L2.common.vid = 100;
            pattern.ruleEgrExtIpv6L2.common.up = 0;
            pattern.ruleEgrExtIpv6L2.common.isIp = 1;
            pattern.ruleEgrExtIpv6L2.common.egrPacketType = 0;
            pattern.ruleEgrExtIpv6L2.common.egrPktType.toCpu.cpuCode = 1;
            pattern.ruleEgrExtIpv6L2.common.egrPktType.toCpu.srcTrg = 1;
            pattern.ruleEgrExtIpv6L2.common.srcDev = 10;
            pattern.ruleEgrExtIpv6L2.common.isVidx = 0;

            pattern.ruleEgrExtIpv6L2.commonExt.isIpv6 = 1;
            pattern.ruleEgrExtIpv6L2.commonExt.ipProtocol = 1;
            pattern.ruleEgrExtIpv6L2.commonExt.dscp = 2;
            pattern.ruleEgrExtIpv6L2.commonExt.isL4Valid = 0;
            pattern.ruleEgrExtIpv6L2.commonExt.egrTcpUdpPortComparator = 0;

            pattern.ruleEgrExtIpv6L2.dipBits127to120 = 0xAA;
            pattern.ruleEgrExtIpv6L2.sip.u32Ip[0] = 100;
            pattern.ruleEgrExtIpv6L2.sip.u32Ip[1] = 1000;
            pattern.ruleEgrExtIpv6L2.sip.u32Ip[2] = 10000;
            pattern.ruleEgrExtIpv6L2.sip.u32Ip[3] = 100000;
            pattern.ruleEgrExtIpv6L2.macDa = mac2;
            pattern.ruleEgrExtIpv6L2.macSa = mac1;

            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);

            if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "device: %d, %d, %d", dev, ruleFormat, ruleIndex);
            }
            else
            {
                /* for CH1 result ignored EPCL not supported     */
                /* but EPCL rule formats translated to HW format */
            }

            if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                /*
                    1.15. For Cheetah2 and above devices call cpssDxChPclPortGroupRuleInvalidate
                    with ruleSize [CPSS_PCL_RULE_SIZE_EXT_E],
                    ruleIndex [PCL_TESTED_RULE_INDEX] to cleanup after testing.
                    Expected: GT_OK.
                */
                ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;

                st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                     "cpssDxChPclPortGroupRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupRuleSet(
                dev, portGroupsBmp,
                ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupRuleSet(
            dev, portGroupsBmp, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* Set valid parameters. As soon as family of device can't be determined */
    /* - values valid for any family                                         */
    ruleFormat = PCL_TESTED_RULE_FORMAT;
    ruleIndex = 0;

    cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
    cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclPortGroupRuleSet(
            dev, portGroupsBmp,
            ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupRuleSet(
        dev, portGroupsBmp,
        ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
        &mask, &pattern, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupRuleValidStatusSet
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN GT_BOOL                            valid
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupRuleValidStatusSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxChx)

    1.1. Set rule for testing.
         Call cpssDxChPclPortGroupRuleSet with ruleFormat
         [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E], ruleIndex [PCL_TESTED_RULE_INDEX],
         maskPtr-> ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ], patternPtr-> ruleStdNotIp
         [ common[pclId=0; sourcePort=5; isTagged=1; vid=100; up=0; qosProfile=0;
         isIp=0; isL2Valid=1; isUdbValid =1], isIpv4 = 0; etherType=0; isArp=0;
         l2Encap=0; macDa=AB:CD:EF:00:00:02; macSa=AB:CD:EF:00:00:01; udb[10,20,30]
         ], actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = 0,
         mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount =
         GT_FALSE, matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE,
         modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [ ingress[profileIndex=0,
         profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] }, redirect {
         CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer
         { policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
         modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
         vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
         ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
    Expected: GT_OK.

    1.2. Call with  ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                    ruleIndex [PCL_TESTED_RULE_INDEX ],
                    valid [GT_TRUE / GT_FALSE].
    Expected: GT_OK.

    1.3. Call with  ruleSize [CPSS_PCL_RULE_SIZE_EXT_E],
                    ruleIndex [PCL_TESTED_RULE_INDEX],
                    valid [GT_FALSE].
    Try to set status of created rule with another ruleSize.
    Expected: NOT GT_OK.

    1.4. Call with  ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                    ruleIndex [0],
                    valid [GT_FALSE].
    Try to set status of created rule with another ruleIndex.
    Expected: NOT GT_OK.

    1.5. Call with wrong enum values ruleSize ,
                   and other parameters from 1.2.
    Expected: NOT GT_OK.

    1.6. Call with out of range ruleIndex [1024],
                   and other parameters from 1.2.
    Expected: NOT GT_OK.

    1.7. Invalidete created rule.
         Call cpssDxChPclPortGroupRuleInvalidate with
         ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E],
         ruleIndex [PCL_TESTED_RULE_INDEX].
    Expected: NON GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U32      portGroupId;

    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    CPSS_PCL_RULE_SIZE_ENT  ruleSize    = CPSS_PCL_RULE_SIZE_STD_E;
    GT_U32                  ruleIndex   = 0;
    GT_BOOL                 valid       = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1. Set rule for testing.  Call cpssDxChPclPortGroupRuleSet with
                ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
                ruleIndex [PCL_TESTED_RULE_INDEX], maskPtr-> ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ],
                patternPtr-> ruleStdNotIp [ common[pclId=0; sourcePort=5; isTagged=1;
                vid=100; up=0; qosProfile=0; isIp=0; isL2Valid=1; isUdbValid =1],
                isIpv4 = 0; etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
                macSa=AB:CD:EF:00:00:01; udb[10,20,30] ],
                actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
                matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
                qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
                qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                profilePrecedence=GT_FALSE] ] },
                redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 },
                vlan { egressTaggedModify=GT_FALSE,
                modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
                Expected: GT_OK.
            */
            st = pclRuleTestingDefaultSet(dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                "cpssDxChPclPortGroupRuleSet: device: %d", dev);

            /*
                1.2. Call with  ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                                ruleIndex [PCL_TESTED_RULE_INDEX ],
                                valid [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */
            /* call with valid = GT_TRUE */
            ruleSize  = CPSS_PCL_RULE_SIZE_STD_E;
            ruleIndex = PCL_TESTED_RULE_INDEX;
            valid     = GT_TRUE;

            st = cpssDxChPclPortGroupRuleValidStatusSet(dev, portGroupsBmp,
                                                        ruleSize, ruleIndex, valid);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex, valid);

            /* call with valid = GT_FALSE */
            valid = GT_FALSE;

            st = cpssDxChPclPortGroupRuleValidStatusSet(dev, portGroupsBmp,
                                                        ruleSize, ruleIndex, valid);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex, valid);

            /*
                1.3. Call with  ruleSize [CPSS_PCL_RULE_SIZE_EXT_E],
                                ruleIndex [PCL_TESTED_RULE_INDEX],
                                valid [GT_FALSE].
                Try to set status of created rule with another ruleSize.
                Expected: NOT GT_OK.
            */
            /* call with ruleSize = CPSS_PCL_RULE_SIZE_EXT_E */
            ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;

            st = cpssDxChPclPortGroupRuleValidStatusSet(dev, portGroupsBmp,
                                                        ruleSize, ruleIndex, valid);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleSize = %d", dev, ruleSize);

            ruleSize = CPSS_PCL_RULE_SIZE_STD_E;

            /*
                1.4. Call with  ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                                ruleIndex [0],
                                valid [GT_FALSE].
                Try to set status of created rule with another ruleIndex.
                Expected: NOT GT_OK.
            */
            ruleIndex = 0;

            st = cpssDxChPclPortGroupRuleValidStatusSet(dev, portGroupsBmp,
                                                        ruleSize, ruleIndex, valid);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleIndex = %d", dev, ruleIndex);

            ruleIndex = PCL_TESTED_RULE_INDEX;

            /*
                1.5. Call with wrong enum values ruleSize, and other parameters from 1.2.
                Expected: NOT GT_OK.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupRuleValidStatusSet
                                (dev, portGroupsBmp, ruleSize, ruleIndex, valid),
                                ruleSize);

            /*
                1.6. Call with out of range ruleIndex [wrong value],
                               and other parameters from 1.2.
                Expected: NOT GT_OK.
            */
            ruleIndex = PCL_INVALID_STD_RULE_INDEX(dev);

            st = cpssDxChPclPortGroupRuleValidStatusSet(dev, portGroupsBmp,
                                                        ruleSize, ruleIndex, valid);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleIndex = %d", dev, ruleIndex);

            ruleIndex = PCL_TESTED_RULE_INDEX;

            /*
                1.7. Invalidete created rule.  Call cpssDxChPclPortGroupRuleInvalidate
                with ruleSize [CPSS_DXCH_PCL_RULE_SIZE_STD_E], ruleIndex [PCL_TESTED_RULE_INDEX].
                Expected: NON GT_OK.
            */
            st = cpssDxChPclPortGroupRuleInvalidate(dev, portGroupsBmp, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupRuleValidStatusSet(dev, portGroupsBmp,
                                          ruleSize, ruleIndex, valid);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupRuleValidStatusSet(dev, portGroupsBmp, ruleSize,
                                                    ruleIndex, valid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    ruleSize  = CPSS_PCL_RULE_SIZE_STD_E;
    ruleIndex = PCL_TESTED_RULE_INDEX;
    valid     = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclPortGroupRuleValidStatusSet(dev, portGroupsBmp,
                                           ruleSize, ruleIndex, valid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    st = cpssDxChPclPortGroupRuleValidStatusSet(dev, portGroupsBmp,
                                         ruleSize, ruleIndex, valid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
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
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupTcpUdpPortComparatorSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(dxCh2 and above)

    1.1. Call with direction [CPSS_PCL_DIRECTION_INGRESS_E], l4Protocol
    [CPSS_L4_PROTOCOL_TCP_E], entryIndex [0], l4PortType [CPSS_L4_PROTOCOL_PORT_SRC_E],
    compareOperator [CPSS_COMPARE_OPERATOR_LTE], value [25].
    Expected: GT_OK for devices and NON GT_OK for others.

    1.2. Call with direction [CPSS_PCL_DIRECTION_EGRESS_E], l4Protocol
    [CPSS_L4_PROTOCOL_UDP_E], entryIndex [7], l4PortType [CPSS_L4_PROTOCOL_PORT_DST_E],
    compareOperator [CPSS_COMPARE_OPERATOR_NEQ], value [25].
    Expected: GT_OK for devices and NON GT_OK for others.

    1.3. Check for invalid enum value for direction. call function
    with direction [wrong enum values] and other parameters as in 1.2.
    Expected: GT_BAD_PARAM.

    1.4. Check for invalid enum value for l4Protocol. call function
    with l4Protocol [wrong enum values] and other parameters as in 1.2.
    Expected: GT_BAD_PARAM.

    1.5. Check for out of range entryIndex. call function with
    entryIndex [8] and other parameters as in 1.2.
    Expected: NON GT_OK.

    1.6. Check for invalid enum value for l4Port. call function with
    l4Port [wrong enum values] and other parameters as in 1.2.
    Expected: GT_BAD_PARAM.

    1.7. Check for invalid enum value for compareOperator. call
    function with compareOperator [wrong enum values] and other parameters as in 1.2.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                         st = GT_OK;

    GT_U8                             dev;
    GT_PORT_GROUPS_BMP                portGroupsBmp = 1;
    CPSS_PCL_DIRECTION_ENT            direction = CPSS_PCL_DIRECTION_INGRESS_E;
    CPSS_L4_PROTOCOL_ENT              l4Protocol = CPSS_L4_PROTOCOL_TCP_E;
    GT_U8                             entryIndex = 0;
    CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType = CPSS_L4_PROTOCOL_PORT_SRC_E;
    CPSS_COMPARE_OPERATOR_ENT         compareOperator = CPSS_COMPARE_OPERATOR_LTE;
    GT_U16                            value = 0;

    GT_U32    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1. Call with direction [CPSS_PCL_DIRECTION_INGRESS_E],
                l4Protocol [CPSS_L4_PROTOCOL_TCP_E], entryIndex [0], l4PortType
                [CPSS_L4_PROTOCOL_PORT_SRC_E], compareOperator [CPSS_COMPARE_OPERATOR_LTE],
                value [25].
                Expected: GT_OK
            */
            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            l4Protocol = CPSS_L4_PROTOCOL_TCP_E;
            entryIndex = 0;
            l4PortType = CPSS_L4_PROTOCOL_PORT_SRC_E;
            compareOperator = CPSS_COMPARE_OPERATOR_LTE;
            value = 25;

            st = cpssDxChPclPortGroupTcpUdpPortComparatorSet(dev, portGroupsBmp,
                    direction, l4Protocol, entryIndex, l4PortType, compareOperator, value);

            UTF_VERIFY_EQUAL7_STRING_MAC(GT_OK, st, "device: %d, %d, %d, %d, %d, %d, %d",
               dev, direction, l4Protocol, entryIndex, l4PortType, compareOperator, value);

            /*
                1.2. Call with direction [CPSS_PCL_DIRECTION_EGRESS_E],
                l4Protocol [CPSS_L4_PROTOCOL_UDP_E], entryIndex [7], l4PortType
                [CPSS_L4_PROTOCOL_PORT_DST_E], compareOperator
                [CPSS_COMPARE_OPERATOR_NEQ], value [25].
                Expected: GT_OK
            */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;
            l4Protocol = CPSS_L4_PROTOCOL_UDP_E;
            entryIndex = 7;
            l4PortType = CPSS_L4_PROTOCOL_PORT_DST_E;
            compareOperator = CPSS_COMPARE_OPERATOR_NEQ;

            st = cpssDxChPclPortGroupTcpUdpPortComparatorSet(dev, portGroupsBmp,
                    direction, l4Protocol, entryIndex, l4PortType, compareOperator, value);

            UTF_VERIFY_EQUAL7_STRING_MAC(GT_OK, st, "device: %d, %d, %d, %d, %d, %d, %d",
              dev, direction, l4Protocol, entryIndex, l4PortType, compareOperator, value);

            /*
                1.3. Check for invalid enum value for direction. Call function
                with direction [wrong enum values] and other parameters as in 1.2.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupTcpUdpPortComparatorSet
                                (dev, portGroupsBmp, direction, l4Protocol, entryIndex,
                                 l4PortType, compareOperator, value),
                                direction);

            /*
                1.4. Check for invalid enum value for l4Protocol.
                    Call with l4Protocol [wrong enum values]
                    and other parameters as in 1.2.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupTcpUdpPortComparatorSet
                                (dev, portGroupsBmp, direction, l4Protocol, entryIndex,
                                 l4PortType, compareOperator, value),
                                l4Protocol);

            /*
               1.5. Check for out of range entryIndex. call
               function with entryIndex [8] and other parameters as in 1.2.
               Expected: NON GT_OK.
            */
            entryIndex = 8;

            st = cpssDxChPclPortGroupTcpUdpPortComparatorSet(dev, portGroupsBmp,
                    direction, l4Protocol, entryIndex, l4PortType, compareOperator, value);

            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                            "device: %d, entryIndex = %d", dev, entryIndex);

            entryIndex = 7;

            /*
                1.6. Check for invalid enum value for l4Port. Call
                    function with l4Port [wrong enum values] and other parameters as in 1.2.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupTcpUdpPortComparatorSet
                                (dev, portGroupsBmp, direction, l4Protocol, entryIndex,
                                 l4PortType, compareOperator, value),
                                l4PortType);

            /*
                1.7. Check for invalid enum value for compareOperator.
                    Call with compareOperator [wrong enum values] and other
                    parameters as in 1.2.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupTcpUdpPortComparatorSet
                                (dev, portGroupsBmp, direction, l4Protocol, entryIndex,
                                 l4PortType, compareOperator, value),
                                compareOperator);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupTcpUdpPortComparatorSet(dev, portGroupsBmp,
                direction, l4Protocol, entryIndex, l4PortType, compareOperator, value);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupTcpUdpPortComparatorSet(dev, portGroupsBmp, direction,
                                                         l4Protocol, entryIndex, l4PortType,
                                                         compareOperator, value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* set correct values for all the parameters (except of device number) */
    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    l4Protocol = CPSS_L4_PROTOCOL_TCP_E;
    entryIndex = 0;
    l4PortType = CPSS_L4_PROTOCOL_PORT_SRC_E;
    compareOperator = CPSS_COMPARE_OPERATOR_LTE;
    value = 25;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChPclPortGroupTcpUdpPortComparatorSet(dev, portGroupsBmp,
                direction, l4Protocol, entryIndex, l4PortType, compareOperator, value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupTcpUdpPortComparatorSet(dev, portGroupsBmp,
            direction, l4Protocol, entryIndex, l4PortType, compareOperator, value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
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
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupUserDefinedByteSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(dxChx)

    1.1. Call with ruleFormat
         [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E], udbIndex [0],
         offsetType [CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
    Expected: GT_OK.

    1.2. Call with ruleFormat
         [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E], udbIndex [1],
         offsetType [CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E], offset [5].
    Expected: GT_OK for ch/ch2/ch3 devices, GT_BAD_PARAM for others.

    1.3. Call with ruleFormat
         [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E], udbIndex [2],
         offsetType [CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E], offset [5].
    Expected: GT_OK for ch/ch2/ch3 devices, GT_BAD_PARAM for others.

    1.4. Call with ruleFormat
         [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E],
         udbIndex [5], offsetType [CPSS_DXCH_PCL_OFFSET_L3_E], offset [5].
    Expected: GT_OK ch1/ch2/ch3 dev and NON GT_OK for others.

    1.5. Call with ruleFormat
         [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E], udbIndex [2],
         offsetType [CPSS_DXCH_PCL_OFFSET_L4_E], offset [5].
    Expected: GT_OK

    1.6. Call with ruleFormat
         [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E], udbIndex [0],
         offsetType [CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E], offset [5].
    Expected: GT_OK Cheetah2 and above devices,  NOT GT_OK for others.

    1.7. Check for invalid udbIndex. Call with
         ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E],
         udbIndex [2], offsetType [CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
    Expected: NON GT_OK.

    1.8. Check for invalid udbIndex. Call with
         ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E],
         udbIndex [6], offsetType [CPSS_DXCH_PCL_OFFSET_L4_E], offset [5].
    Expected: NON GT_OK.

    1.9. Check for invalid offsetType. Call with
    ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
    udbIndex [0], offsetType [CPSS_DXCH_PCL_OFFSET_L4_E], offset [5].
    Expected: NON GT_OK.

    1.10. Check for invalid offsetType. Call with
    ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E],
    udbIndex [2], offsetType [CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
    Expected: NON GT_OK.

    1.11. Check out-of-range ruleFormat. Call with ruleFormat [wrong enum values],
          udbIndex [0], offsetType[CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
    Expected: GT_BAD_PARAM for ch, ch2, ch3 and GT_OK for xCat and above.

    1.12. Check out-of-range offsetType. Call with
          offsetType [wrong enum values], other params same as in 1.1.
    Expected: GT_BAD_PARAM.

    1.13. Check for big value for offset. Call with offset [0xFE] and other
          parameters the same as in 1.1.
    Expected: GT_OUT_OF_RANGE.
*/
    GT_STATUS                           st = GT_OK;

    GT_U8                               dev;
    GT_PORT_GROUPS_BMP                  portGroupsBmp = 1;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    CPSS_DXCH_PCL_PACKET_TYPE_ENT       packetType = CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E;
    GT_U32                              udbIndex = 0;
    CPSS_DXCH_PCL_OFFSET_TYPE_ENT       offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;
    GT_U8                               offset = 0;

    CPSS_DXCH_PCL_OFFSET_TYPE_ENT       offsetTypeGet;
    GT_U8                               offsetGet;
    CPSS_PP_FAMILY_TYPE_ENT             devFamily;
    CPSS_PP_DEVICE_TYPE                 devType;

    GT_U32    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = prvUtfDeviceFamilyGet(dev, &devFamily);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

            st = prvUtfDeviceTypeGet(dev, &devType);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceTypeGet: %d", dev);

            /*
                1.1. Call with ruleFormat
                     [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E], udbIndex [0],
                     offsetType [CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
                Expected: GT_OK
            */
            offset     = 5;
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            packetType = CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E;
            udbIndex   = 0;
            offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;

            st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp, ruleFormat,
                                               packetType, udbIndex, offsetType, offset);

            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex, offsetType, offset);

            if(st == GT_OK)
            {
                /* verifying values */
                st = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp, ruleFormat,
                                     packetType, udbIndex, &offsetTypeGet, &offsetGet);

                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex);

                UTF_VERIFY_EQUAL1_STRING_MAC(offsetType, offsetTypeGet,
                         "cpssDxChPclPortGroupUserDefinedByteGet: "
                         "get another offsetType than was set: dev = %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(offset, offsetGet,
                         "cpssDxChPclPortGroupUserDefinedByteGet: "
                         "get another offset than was set: dev = %d", dev);
            }

            /*
                1.2. Call with ruleFormat
                     [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E], udbIndex [1],
                     offsetType [CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E], offset [5].
                Expected: GT_OK for ch/ch2/ch3 devices, GT_BAD_PARAM for others.
            */
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
            udbIndex   = 1;
            offsetType = CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E;
            offset     = 5;

            st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp, ruleFormat,
                                               packetType, udbIndex, offsetType, offset);

            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleFormat, udbIndex,
                                            offsetType, offset);
            }
            else
            {
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                            offsetType, offset);
            }

            if(st == GT_OK)
            {
                /* verifying values */
                st = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp, ruleFormat,
                    packetType, udbIndex, &offsetTypeGet, &offsetGet);

                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex);

                UTF_VERIFY_EQUAL1_STRING_MAC(offsetType, offsetTypeGet,
                         "cpssDxChPclPortGroupUserDefinedByteGet: "
                         "get another offsetType than was set: dev = %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(offset, offsetGet,
                         "cpssDxChPclPortGroupUserDefinedByteGet: "
                         "get another offset than was set: dev = %d", dev);
            }

            /*
                1.3. Call with ruleFormat
                     [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E], udbIndex [2],
                     offsetType [CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E], offset [5].
                Expected: GT_OK for ch/ch2/ch3 devices, GT_BAD_PARAM for others.
            */
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E;
            udbIndex   = 2;
            offsetType = CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E;
            offset     = 5;

            st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp, ruleFormat,
                           packetType, udbIndex, offsetType, offset);

            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleFormat, udbIndex,
                                            offsetType, offset);
            }
            else
            {
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                            offsetType, offset);
            }

            if(st == GT_OK)
            {
                /* verifying values */
                st = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp, ruleFormat,
                     packetType, udbIndex, &offsetTypeGet, &offsetGet);

                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex);

                UTF_VERIFY_EQUAL1_STRING_MAC(offsetType, offsetTypeGet,
                             "cpssDxChPclPortGroupUserDefinedByteGet: "
                             "get another offsetType than was set: dev = %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(offset,     offsetGet,
                             "cpssDxChPclPortGroupUserDefinedByteGet: "
                             "get another offset than was set: dev = %d", dev);
            }

            /*
                1.4. Call with ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
                     udbIndex [2], offsetType [CPSS_DXCH_PCL_OFFSET_L3_E], offset [5]
                     and wrong enum values packetType.
                Expected: GT_OK ch1/ch2/ch3 dev and NON GT_OK for others.
            */
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            udbIndex   = 2;
            offsetType = CPSS_DXCH_PCL_OFFSET_L3_E;
            offset     = 5;

            if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                for(enumsIndex = 0; enumsIndex < utfInvalidEnumArrSize; enumsIndex++)
                {
                    packetType = utfInvalidEnumArr[enumsIndex];

                    st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp,
                            ruleFormat, packetType, udbIndex, offsetType, offset);

                    UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                                offsetType, offset);
                    if (GT_OK == st)
                    {
                        /* verifying values */
                        st = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp, ruleFormat,
                                           packetType, udbIndex, &offsetTypeGet, &offsetGet);
                        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex);

                        UTF_VERIFY_EQUAL1_STRING_MAC(offsetType, offsetTypeGet,
                                         "cpssDxChPclPortGroupUserDefinedByteGet: "
                                         "get another offsetType than was set: dev = %d", dev);
                        UTF_VERIFY_EQUAL1_STRING_MAC(offset, offsetGet,
                                         "cpssDxChPclPortGroupUserDefinedByteGet: "
                                         "get another offset than was set: dev = %d", dev);
                    }

                    packetType = CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E;
                }
            }
            else
            {
                UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupUserDefinedByteSet
                                    (dev, portGroupsBmp, ruleFormat, packetType,
                                     udbIndex, offsetType, offset),
                                    packetType);
            }

            /*
                1.5. Call with ruleFormat
                     [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E], udbIndex [2],
                     offsetType [CPSS_DXCH_PCL_OFFSET_L4_E], offset [5].
                Expected: GT_OK
            */
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
            udbIndex   = 2;
            offsetType = CPSS_DXCH_PCL_OFFSET_L4_E;
            offset     = 5;

            st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp, ruleFormat,
                                               packetType, udbIndex, offsetType, offset);

            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                        offsetType, offset);
            if(st == GT_OK)
            {
                /* verifying values */
                st = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp, ruleFormat,
                   packetType, udbIndex, &offsetTypeGet, &offsetGet);

                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex);

                UTF_VERIFY_EQUAL1_STRING_MAC(offsetType, offsetTypeGet,
                             "cpssDxChPclPortGroupUserDefinedByteGet: "
                             "get another offsetType than was set: dev = %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(offset,     offsetGet,
                             "cpssDxChPclPortGroupUserDefinedByteGet: "
                             "get another offset than was set: dev = %d", dev);
            }
            /*
                1.6. Call with ruleFormat
                     [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E], udbIndex [0],
                     offsetType [CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E], offset [5].
                Expected: GT_OK Cheetah2 and above devices,  NOT GT_OK for others.
            */
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
            udbIndex   = 0;
            offsetType = CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E;
            offset     = 5; /* offset is ignored for TCP_UDP_COMPARATOR */

            st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp, ruleFormat,
                                            packetType, udbIndex, offsetType, offset);

            if(devFamily >= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                            offsetType, offset);

                /* verifying values */
                st = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp, ruleFormat,
                                          packetType, udbIndex, &offsetTypeGet, &offsetGet);

                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex);

                UTF_VERIFY_EQUAL1_STRING_MAC(offsetType, offsetTypeGet,
                                 "cpssDxChPclPortGroupUserDefinedByteGet: "
                                 "get another offsetType than was set: dev = %d", dev);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                                offsetType, offset);
            }

            /*
                1.7. Check for invalid udbIndex. Call with
                     ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E],
                     udbIndex [2], offsetType [CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
                Expected: NON GT_OK.
            */
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
            udbIndex   = 2;
            offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;
            offset     = 5;

            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                udbIndex = 23;
            }

            st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp, ruleFormat,
                packetType, udbIndex, offsetType, offset);

            UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                             offsetType, offset);
            /*
                1.8. Check for invalid udbIndex. Call with
                     ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E],
                     udbIndex [6], offsetType [CPSS_DXCH_PCL_OFFSET_L4_E], offset [5].
                Expected: NON GT_OK.
            */
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;
            udbIndex   = 6;
            offsetType = CPSS_DXCH_PCL_OFFSET_L4_E;
            offset     = 5;

            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                offsetType = CPSS_DXCH_PCL_OFFSET_L3_E;
            }

            st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp, ruleFormat,
               packetType, udbIndex, offsetType, offset);

            UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                            offsetType, offset);
            /*
                1.9. Check for invalid offsetType. Call with
                ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
                udbIndex [0], offsetType [CPSS_DXCH_PCL_OFFSET_L4_E], offset [5].
                Expected: NON GT_OK.
            */
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            udbIndex   = 0;
            offsetType = CPSS_DXCH_PCL_OFFSET_L4_E;
            offset     = 5;

            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                offsetType = CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E;
            }

            st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp, ruleFormat,
               packetType, udbIndex, offsetType, offset);

            UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                            offsetType, offset);
            /*
                1.10. Check for invalid offsetType. Call with
                ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E],
                udbIndex [2], offsetType [CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
                Expected: NON GT_OK.
            */
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E;
            udbIndex   = 2;
            offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;
            offset     = 5;

            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                /* to cause "BAD_PARAM", for XCat the key type ignored */
                offsetType = CPSS_DXCH_PCL_OFFSET_L3_E;
            }

            st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp, ruleFormat,
                 packetType, udbIndex, offsetType, offset);

            UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                             offsetType, offset);
            /*
                1.11. Check out-of-range ruleFormat. Call with ruleFormat [wrong enum values],
                      udbIndex [0], offsetType[CPSS_DXCH_PCL_OFFSET_L2_E], offset [5].
                Expected: GT_BAD_PARAM for ch, ch2, ch3 and GT_OK for xCat and above.
            */
            packetType = CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E;
            udbIndex   = 0;
            offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;
            offset     = 5;

            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                /* to cause "BAD_PARAM", for XCat the key type ignored */
                offsetType = CPSS_DXCH_PCL_OFFSET_L3_E;
            }

            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupUserDefinedByteSet
                                (dev, portGroupsBmp, ruleFormat,
                                 packetType, udbIndex, offsetType, offset),
                                ruleFormat);

            /*
                1.12. Check out-of-range offsetType. Call with
                      offsetType [wrong enum values], other params same as in 1.1.
                Expected: GT_BAD_PARAM.
            */
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            udbIndex   = 0;
            offset     = 5;

            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupUserDefinedByteSet
                                (dev, portGroupsBmp, ruleFormat,
                                 packetType, udbIndex, offsetType, offset),
                                offsetType);

            /*
                1.13. Check for big value for offset. Call with offset [0xFE] and other
                      parameters the same as in 1.1.
                Expected: GT_OUT_OF_RANGE.
            */
            offset     = 0xFE;
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            udbIndex   = 0;
            offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;

            st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp, ruleFormat,
             packetType, udbIndex, offsetType, offset);

            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, ruleFormat, udbIndex,
                                        offsetType, offset);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        offset     = 0;
        ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
        packetType = CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E;
        udbIndex   = 0;
        offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp, ruleFormat,
                                           packetType, udbIndex, offsetType, offset);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp, ruleFormat,
                                           packetType, udbIndex, offsetType, offset);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* set correct values for all the parameters (except of device number) */
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    udbIndex   = 0;
    offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;
    offset     = 5;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /*
            2.1. Call function for non active device and valid parameters.
            Expected: GT_BAD_PARAM.
        */
        st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp, ruleFormat,
                                       packetType, udbIndex, offsetType, offset);

        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupUserDefinedByteSet(dev, portGroupsBmp, ruleFormat,
                                    packetType, udbIndex, offsetType, offset);

    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

    /* check that configuration replicated to all port groups */

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    udbIndex   = 0;
    offsetType = CPSS_DXCH_PCL_OFFSET_L2_E;
    packetType = CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* store configuration to all port groups */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        for (udbIndex = 0; (udbIndex < 23); udbIndex++)
        {
            offset     = (GT_U8)udbIndex;
            st = cpssDxChPclPortGroupUserDefinedByteSet(
                dev, portGroupsBmp, ruleFormat,
                packetType, udbIndex, offsetType, offset);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }


        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            for (udbIndex = 0; (udbIndex < 23); udbIndex++)
            {
                offset     = (GT_U8)udbIndex;
                st = cpssDxChPclPortGroupUserDefinedByteGet(
                    dev, portGroupsBmp, ruleFormat,
                    packetType, udbIndex, &offsetTypeGet, &offsetGet);
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
                UTF_VERIFY_EQUAL0_PARAM_MAC(offsetGet, offset);
                UTF_VERIFY_EQUAL0_PARAM_MAC(offsetTypeGet, offsetType);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
    }
}

/*----------------------------------------------------------------------------*/
/*
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
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupUserDefinedByteGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(dxChx)
    1.1. Call with ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
         packetType = CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E, udbIndex [0].
    Expected: GT_OK.
    1.2. Call with wrong enum values ruleFormat.
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong enum value spacketType.
    Expected: GT_BAD_PARAM.
    1.4. Call with offsetType = NULL (null pointer).
    Expected: GT_BAD_PTR.
    1.5. Call with offset     = NULL (null pointer).
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                           st = GT_OK;

    GT_U8                               dev;
    GT_PORT_GROUPS_BMP                  portGroupsBmp = 1;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    CPSS_DXCH_PCL_PACKET_TYPE_ENT       packetType = CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E;
    GT_U32                              udbIndex = 0;
    CPSS_DXCH_PCL_OFFSET_TYPE_ENT       offsetTypeGet;
    GT_U8                               offsetGet;

    CPSS_PP_FAMILY_TYPE_ENT             devFamily;
    CPSS_PP_DEVICE_TYPE                 devType;

    GT_U32    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        st = prvUtfDeviceTypeGet(dev, &devType);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceTypeGet: %d", dev);

        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1. Call with ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
                     packetType = CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E, udbIndex [0].
                Expected: GT_OK.
            */
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            packetType = CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E;
            udbIndex   = 0;

            st = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp, ruleFormat,
                                  packetType, udbIndex, &offsetTypeGet, &offsetGet);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.2. Call with wrong enum values ruleFormat.
                Expected: GT_BAD_PARAM.
            */

            if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupUserDefinedByteGet
                                    (dev, portGroupsBmp, ruleFormat, packetType,
                                     udbIndex, &offsetTypeGet, &offsetGet),
                                    ruleFormat);
            }
            else
            {
                for(enumsIndex = 0; enumsIndex < utfInvalidEnumArrSize; enumsIndex++)
                {
                    ruleFormat = utfInvalidEnumArr[enumsIndex];

                    st = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp,
                            ruleFormat, packetType, udbIndex, &offsetTypeGet, &offsetGet);

                    UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleFormat, udbIndex,
                                                offsetTypeGet, offsetGet);
                }
                ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
            }

            /*
                1.3. Call with wrong enum values packetType.
                Expected: GT_BAD_PARAM.
            */
            if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                for(enumsIndex = 0; enumsIndex < utfInvalidEnumArrSize; enumsIndex++)
                {
                    packetType = utfInvalidEnumArr[enumsIndex];

                    st = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp,
                            ruleFormat, packetType, udbIndex, &offsetTypeGet, &offsetGet);

                    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
                }
                packetType = CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E;
            }
            else
            {
                UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupUserDefinedByteGet
                                    (dev, portGroupsBmp, ruleFormat, packetType, udbIndex,
                                     &offsetTypeGet, &offsetGet),
                                    packetType);
            }

            /*
                1.4. Call with offsetType = NULL (null pointer).
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp, ruleFormat,
                                               packetType, udbIndex, NULL, &offsetGet);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

            /*
                1.5. Call with offset = NULL (null pointer).
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp, ruleFormat,
                                               packetType, udbIndex, &offsetTypeGet, NULL);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp, ruleFormat,
                              packetType, udbIndex, &offsetTypeGet, &offsetGet);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp, ruleFormat,
                              packetType, udbIndex, &offsetTypeGet, &offsetGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    udbIndex   = 0;
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp, ruleFormat,
                                packetType, udbIndex, &offsetTypeGet, &offsetGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st  = cpssDxChPclPortGroupUserDefinedByteGet(dev, portGroupsBmp, ruleFormat,
                                packetType, udbIndex, &offsetTypeGet, &offsetGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclUdeEtherTypeSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    IN  GT_U32          ethType
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclUdeEtherTypeSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with index [0..5], ethType [0..0xFFFF]
    Expected: GT_OK.
    1.2. Call cpssDxChPclUdeEtherTypeGet with non-NULL pointers
                                         other params same as in 1.1.
    Expected: GT_OK and the same values.
    1.3. Call with out of range index [6], other params same as in 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range ethType [0x10000], other params same as in 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      index      = 0;
    GT_U32      ethType    = 0;
    GT_U32      ethTypeGet = 0;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0..4], ethType [0..0xFFFF]
            Expected: GT_OK.
        */
        for (index = 0; (index < 5); index++)
        {
            ethType = 13107 * index;

            st = cpssDxChPclUdeEtherTypeSet(dev, index, ethType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, ethType);

            /*
                1.2. Call cpssDxChPclUdeEtherTypeGet with non-NULL pointers
                                                     other params same as in 1.1.
                Expected: GT_OK and the same values.
            */
            if (GT_OK == st)
            {
                st = cpssDxChPclUdeEtherTypeGet(dev, index, &ethTypeGet);
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                           "cpssDxChPclUdeEtherTypeGet: %d", dev);

                /* verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(ethType, ethTypeGet,
                           "got another ethType then was set: %d", dev);
            }
        }

        /*
            1.3. Call with out of range index [5], other params same as in 1.1.
            Expected: NOT GT_OK.
        */
        index = 5;

        st = cpssDxChPclUdeEtherTypeSet(dev, index, ethType);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.4. Call with out of range ethType [0x10000], other params same as in 1.1.
            Expected: NOT GT_OK.
        */
        ethType = 0x10000;

        st = cpssDxChPclUdeEtherTypeSet(dev, index, ethType);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);
    }

    index   = 0;
    ethType = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclUdeEtherTypeSet(dev, index, ethType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclUdeEtherTypeSet(dev, index, ethType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclUdeEtherTypeGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    OUT GT_U32          *ethTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclUdeEtherTypeGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with index [0..5], non NULL ethTypePtr.
    Expected: GT_OK.
    1.2. Call with out of range index [6], other params same as in 1.1.
    Expected: NOT GT_OK.
    1.3. Call with ethTypePtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      index   = 0;
    GT_U32      ethType = 0;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0..4], non NULL ethTypePtr.
            Expected: GT_OK.
        */
        for (index = 0; (index < 5); index++)
        {
            ethType = 13107 * index;

            st = cpssDxChPclUdeEtherTypeGet(dev, index, &ethType);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);
        }

        /*
            1.2. Call with out of range index [5], other params same as in 1.1.
            Expected: NOT GT_OK.
        */
        index = 5;

        st = cpssDxChPclUdeEtherTypeGet(dev, index, &ethType);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call with ethTypePtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclUdeEtherTypeGet(dev, index, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, "%d, ethTypePtr = NULL", dev);
    }

    index = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclUdeEtherTypeGet(dev, index, &ethType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclUdeEtherTypeGet(dev, index, &ethType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet
(
    IN  GT_U8                                     devNum,
    IN  GT_PORT_GROUPS_BMP                        portGroupsBmp,
    IN  CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (Lion and above)
    1.1.1. Call with udbOverTrunkHashPtr {[GT_FALSE / GT_TRUE],...},
    Expected: GT_OK.
    1.1.2. Call cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet with non-NULL pointers
                                                   other params same as in 1.1.1.
    Expected: GT_OK and the same values.
    1.1.3. Call with udbOverTrunkHashPtr [NULL]
                     other params same as in 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    GT_BOOL                                   isEqual             = GT_FALSE;
    CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC udbOverTrunkHash    = {0};
    CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC udbOverTrunkHashGet = {0};


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with udbOverTrunkHashPtr {[GT_FALSE / GT_TRUE],...},
                Expected: GT_OK.
            */
            udbOverTrunkHash.trunkHashEnableStdNotIp   = GT_FALSE;
            udbOverTrunkHash.trunkHashEnableStdIpv4L4  = GT_FALSE;
            udbOverTrunkHash.trunkHashEnableStdUdb     = GT_FALSE;
            udbOverTrunkHash.trunkHashEnableExtNotIpv6 = GT_FALSE;
            udbOverTrunkHash.trunkHashEnableExtIpv6L2  = GT_FALSE;
            udbOverTrunkHash.trunkHashEnableExtIpv6L4  = GT_FALSE;
            udbOverTrunkHash.trunkHashEnableExtUdb     = GT_FALSE;

            st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet(dev, portGroupsBmp,
                                                                            &udbOverTrunkHash);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet
                            with non-NULL pointers other params same as in 1.1.1.
                Expected: GT_OK and the same values.
            */
            st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet(dev, portGroupsBmp,
                                                                       &udbOverTrunkHashGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet: %d", dev);

            /* validation values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &udbOverTrunkHash,
                                         (GT_VOID*) &udbOverTrunkHashGet,
                                         sizeof(udbOverTrunkHash))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another udbOverTrunkHash then was set: %d", dev);

            /*
                1.1.1. Call with udbOverTrunkHashPtr {[GT_FALSE / GT_TRUE],...},
                Expected: GT_OK.
            */
            udbOverTrunkHash.trunkHashEnableStdNotIp   = GT_TRUE;
            udbOverTrunkHash.trunkHashEnableStdIpv4L4  = GT_TRUE;
            udbOverTrunkHash.trunkHashEnableStdUdb     = GT_TRUE;
            udbOverTrunkHash.trunkHashEnableExtNotIpv6 = GT_TRUE;
            udbOverTrunkHash.trunkHashEnableExtIpv6L2  = GT_TRUE;
            udbOverTrunkHash.trunkHashEnableExtIpv6L4  = GT_TRUE;
            udbOverTrunkHash.trunkHashEnableExtUdb     = GT_TRUE;

            st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet(dev, portGroupsBmp,
                                                                            &udbOverTrunkHash);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet
                                with non-NULL pointers other params same as in 1.1.1.
                Expected: GT_OK and the same values.
            */
            st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet(dev, portGroupsBmp,
                                                                    &udbOverTrunkHashGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet: %d", dev);

            /* validation values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &udbOverTrunkHash,
                                         (GT_VOID*) &udbOverTrunkHashGet,
                                         sizeof(udbOverTrunkHash))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another udbOverTrunkHash then was set: %d", dev);

            /*
                1.1.1. Call with udbOverTrunkHashPtr {[GT_FALSE / GT_TRUE],...},
                Expected: GT_OK.
            */
            udbOverTrunkHash.trunkHashEnableStdNotIp   = GT_FALSE;
            udbOverTrunkHash.trunkHashEnableStdIpv4L4  = GT_TRUE;
            udbOverTrunkHash.trunkHashEnableStdUdb     = GT_FALSE;
            udbOverTrunkHash.trunkHashEnableExtNotIpv6 = GT_TRUE;
            udbOverTrunkHash.trunkHashEnableExtIpv6L2  = GT_TRUE;
            udbOverTrunkHash.trunkHashEnableExtIpv6L4  = GT_FALSE;
            udbOverTrunkHash.trunkHashEnableExtUdb     = GT_TRUE;

            st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet(dev, portGroupsBmp,
                                                                            &udbOverTrunkHash);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet
                            with non-NULL pointers other params same as in 1.1.1.
                Expected: GT_OK and the same values.
            */
            st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet(dev, portGroupsBmp,
                                                                        &udbOverTrunkHashGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet: %d", dev);

            /* validation values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &udbOverTrunkHash,
                                         (GT_VOID*) &udbOverTrunkHashGet,
                                         sizeof(udbOverTrunkHash))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another udbOverTrunkHash then was set: %d", dev);

            /*
                1.1.3. Call with udbOverTrunkHashPtr [NULL]
                                 other params same as in 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet(dev,
                                                            portGroupsBmp, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                        "%d, udbOverTrunkHashPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev, portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet(dev, portGroupsBmp,
                                                                            &udbOverTrunkHash);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev, portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet(dev, portGroupsBmp,
                                                                        &udbOverTrunkHash);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    cpssOsMemSet((GT_VOID*) &udbOverTrunkHash, 0, sizeof(udbOverTrunkHash));

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet(dev, portGroupsBmp,
                                                                        &udbOverTrunkHash);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet(dev, portGroupsBmp,
                                                                    &udbOverTrunkHash);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet
(
    IN  GT_U8                                     devNum,
    IN  GT_PORT_GROUPS_BMP                        portGroupsBmp,
    OUT CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (Lion and above)
    1.1.1. Call with non NULL udbOverTrunkHashPtr
    Expected: GT_OK.
    1.1.2. Call with udbOverTrunkHashPtr [NULL]
                     other params same as in 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC udbOverTrunkHash = {0};


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with non NULL udbOverTrunkHashPtr
                Expected: GT_OK.
            */
            st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet(dev, portGroupsBmp,
                                                                            &udbOverTrunkHash);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call with udbOverTrunkHashPtr [NULL]
                                 other params same as in 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet(dev,
                                                            portGroupsBmp, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, udbOverTrunkHashPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev, portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet(dev, portGroupsBmp,
                                                                            &udbOverTrunkHash);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev, portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet(dev, portGroupsBmp,
                                                                            &udbOverTrunkHash);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet(dev, portGroupsBmp,
                                                                        &udbOverTrunkHash);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet(dev, portGroupsBmp,
                                                                    &udbOverTrunkHash);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet
(
    IN  GT_U8                                      devNum,
    IN  GT_PORT_GROUPS_BMP                         portGroupsBmp,
    IN  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (Lion and above)
    1.1.1. Call with vidUpMode [CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E /
                                CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E]
    Expected: GT_OK.
    1.1.2. Call cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet with non-NULL pointers
                                                                     other params same as in 1.1.1.
    Expected: GT_OK and the same values.
    1.1.3. Call with out of range vidUpMode [wrong enum values],
                     other params same as in 1.1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT vidUpMode    =
                            CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E;
    CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT vidUpModeGet =
                            CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with vidUpMode [CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E /
                                            CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E]
                Expected: GT_OK.
            */
            vidUpMode = CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E;

            st = cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet(dev, portGroupsBmp, vidUpMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, portGroupsBmp, vidUpMode);

            /*
                1.1.2. Call cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet
                with non-NULL pointers other params same as in 1.1.1.
                Expected: GT_OK and the same values.
            */
            st = cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet(dev,
                                        portGroupsBmp ,&vidUpModeGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet: %d", dev);

            /* validation values */
            UTF_VERIFY_EQUAL1_STRING_MAC(vidUpMode, vidUpModeGet,
                       "got another vidUpMode then was set: %d", dev);

            /*
                1.1.1. Call with vidUpMode [CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E /
                                            CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E]
                Expected: GT_OK.
            */
            vidUpMode = CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E;

            st = cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet(dev,
                                                portGroupsBmp, vidUpMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, portGroupsBmp, vidUpMode);

            /*
                1.1.2. Call cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet
                with non-NULL pointers other params same as in 1.1.1.
                Expected: GT_OK and the same values.
            */
            st = cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet(dev,
                                        portGroupsBmp ,&vidUpModeGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet: %d", dev);

            /* validation values */
            UTF_VERIFY_EQUAL1_STRING_MAC(vidUpMode, vidUpModeGet,
                       "got another vidUpMode then was set: %d", dev);

            /*
                1.1.3. Call with out of range vidUpMode [wrong enum values],
                                 other params same as in 1.1.1.
                Expected: NOT GT_OK.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet
                                (dev, portGroupsBmp, vidUpMode),
                                vidUpMode);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev, portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet(dev,
                                            portGroupsBmp, vidUpMode);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev, portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet(dev, portGroupsBmp, vidUpMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    vidUpMode = CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet(dev, portGroupsBmp, vidUpMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet(dev, portGroupsBmp, vidUpMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet
(
    IN   GT_U8                                      devNum,
    IN   GT_PORT_GROUPS_BMP                         portGroupsBmp,
    OUT  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   *vidUpModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (Lion and above)
    1.1.1. Call with non NULL vidUpModePtr.
    Expected: GT_OK.
    1.1.2. Call with vidUpModePtr [NULL],
                     other params same as in 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT vidUpMode =
                            CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with non NULL vidUpModePtr.
                Expected: GT_OK.
            */
            st = cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet(dev,
                                            portGroupsBmp, &vidUpMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, portGroupsBmp, vidUpMode);

            /*
                1.1.2. Call with vidUpModePtr [NULL],
                                 other params same as in 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet(dev, portGroupsBmp, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vidUpModePtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev, portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet(dev,
                                            portGroupsBmp, &vidUpMode);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev, portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet(dev, portGroupsBmp, &vidUpMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet(dev, portGroupsBmp, &vidUpMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet(dev, portGroupsBmp, &vidUpMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with udbOverTrunkHashPtr {[GT_FALSE / GT_TRUE],...},
    Expected: GT_OK.
    1.2. Call cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet with non-NULL pointers
                                                  other params same as in 1.1.
    Expected: GT_OK and the same values.
    1.3. Call with udbOverTrunkHashPtr [NULL]
                   other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL                                   isEqual             = GT_FALSE;
    CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC udbOverTrunkHash    = {0};
    CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC udbOverTrunkHashGet = {0};


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with udbOverTrunkHashPtr {[GT_FALSE / GT_TRUE],...},
            Expected: GT_OK.
        */
        udbOverTrunkHash.trunkHashEnableStdNotIp   = GT_FALSE;
        udbOverTrunkHash.trunkHashEnableStdIpv4L4  = GT_FALSE;
        udbOverTrunkHash.trunkHashEnableStdUdb     = GT_FALSE;
        udbOverTrunkHash.trunkHashEnableExtNotIpv6 = GT_FALSE;
        udbOverTrunkHash.trunkHashEnableExtIpv6L2  = GT_FALSE;
        udbOverTrunkHash.trunkHashEnableExtIpv6L4  = GT_FALSE;
        udbOverTrunkHash.trunkHashEnableExtUdb     = GT_FALSE;

        st = cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet(dev, &udbOverTrunkHash);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet
            with non-NULL pointers other params same as in 1.1.
            Expected: GT_OK and the same values.
        */
        st = cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet(dev, &udbOverTrunkHashGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet: %d", dev);

        /* validation values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &udbOverTrunkHash,
                                     (GT_VOID*) &udbOverTrunkHashGet,
                                     sizeof(udbOverTrunkHash))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another udbOverTrunkHash then was set: %d", dev);

        /*
            1.1. Call with udbOverTrunkHashPtr {[GT_FALSE / GT_TRUE],...},
            Expected: GT_OK.
        */
        udbOverTrunkHash.trunkHashEnableStdNotIp   = GT_TRUE;
        udbOverTrunkHash.trunkHashEnableStdIpv4L4  = GT_TRUE;
        udbOverTrunkHash.trunkHashEnableStdUdb     = GT_TRUE;
        udbOverTrunkHash.trunkHashEnableExtNotIpv6 = GT_TRUE;
        udbOverTrunkHash.trunkHashEnableExtIpv6L2  = GT_TRUE;
        udbOverTrunkHash.trunkHashEnableExtIpv6L4  = GT_TRUE;
        udbOverTrunkHash.trunkHashEnableExtUdb     = GT_TRUE;

        st = cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet(dev, &udbOverTrunkHash);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet
            with non-NULL pointers other params same as in 1.1.
            Expected: GT_OK and the same values.
        */
        st = cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet(dev, &udbOverTrunkHashGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet: %d", dev);

        /* validation values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &udbOverTrunkHash,
                                     (GT_VOID*) &udbOverTrunkHashGet,
                                     sizeof(udbOverTrunkHash))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another udbOverTrunkHash then was set: %d", dev);

        /*
            1.1.1. Call with udbOverTrunkHashPtr {[GT_FALSE / GT_TRUE],...},
            Expected: GT_OK.
        */
        udbOverTrunkHash.trunkHashEnableStdNotIp   = GT_FALSE;
        udbOverTrunkHash.trunkHashEnableStdIpv4L4  = GT_TRUE;
        udbOverTrunkHash.trunkHashEnableStdUdb     = GT_FALSE;
        udbOverTrunkHash.trunkHashEnableExtNotIpv6 = GT_TRUE;
        udbOverTrunkHash.trunkHashEnableExtIpv6L2  = GT_TRUE;
        udbOverTrunkHash.trunkHashEnableExtIpv6L4  = GT_FALSE;
        udbOverTrunkHash.trunkHashEnableExtUdb     = GT_TRUE;

        st = cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet(dev, &udbOverTrunkHash);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet
            with non-NULL pointers other params same as in 1.1.
            Expected: GT_OK and the same values.
        */
        st = cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet(dev, &udbOverTrunkHashGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet: %d", dev);

        /* validation values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &udbOverTrunkHash,
                                     (GT_VOID*) &udbOverTrunkHashGet,
                                     sizeof(udbOverTrunkHash))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another udbOverTrunkHash then was set: %d", dev);

        /*
            1.3. Call with udbOverTrunkHashPtr [NULL]
                           other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, udbOverTrunkHashPtr = NULL", dev);
    }

    cpssOsMemSet((GT_VOID*) &udbOverTrunkHash, 0, sizeof(udbOverTrunkHash));

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet(dev, &udbOverTrunkHash);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet(dev, &udbOverTrunkHash);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with non NULL udbOverTrunkHashPtr.
    Expected: GT_OK.
    1.2. Call with udbOverTrunkHashPtr [NULL]
                   other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC udbOverTrunkHash = {0};


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non NULL udbOverTrunkHashPtr.
            Expected: GT_OK.
        */
        st = cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet(dev, &udbOverTrunkHash);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with udbOverTrunkHashPtr [NULL]
                           other params same as in 1.1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                "%d, udbOverTrunkHashPtr = NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet(dev, &udbOverTrunkHash);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet(dev, &udbOverTrunkHash);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclEgressKeyFieldsVidUpModeSet
(
    IN  GT_U8                                      devNum,
    IN  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclEgressKeyFieldsVidUpModeSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with vidUpMode [CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E /
                              CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E]
    Expected: GT_OK.
    1.2. Call cpssDxChPclEgressKeyFieldsVidUpModeGet with non-NULL pointers
                                                          other params same as in 1.1.
    Expected: GT_OK and the same values.
    1.3. Call with out of range vidUpMode [wrong enum values],
                     other params same as in 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT vidUpMode    =
                                CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E;
    CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT vidUpModeGet =
                                CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vidUpMode [CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E /
                                      CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E]
            Expected: GT_OK.
        */
        vidUpMode = CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E;

        st = cpssDxChPclEgressKeyFieldsVidUpModeSet(dev, vidUpMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidUpMode);

        /*
            1.2. Call cpssDxChPclEgressKeyFieldsVidUpModeGet
            with non-NULL pointers other params same as in 1.1.
            Expected: GT_OK and the same values.
        */
        st = cpssDxChPclEgressKeyFieldsVidUpModeGet(dev, &vidUpModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChPclEgressKeyFieldsVidUpModeGet: %d", dev);

        /* validation values */
        UTF_VERIFY_EQUAL1_STRING_MAC(vidUpMode, vidUpModeGet,
                   "got another vidUpMode then was set: %d", dev);

        /*
            1.1. Call with vidUpMode [CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E /
                                      CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E]
            Expected: GT_OK.
        */
        vidUpMode = CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E;

        st = cpssDxChPclEgressKeyFieldsVidUpModeSet(dev, vidUpMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidUpMode);

        /*
            1.2. Call cpssDxChPclEgressKeyFieldsVidUpModeGet
                    with non-NULL pointers other params same as in 1.1.
            Expected: GT_OK and the same values.
        */
        st = cpssDxChPclEgressKeyFieldsVidUpModeGet(dev, &vidUpModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChPclEgressKeyFieldsVidUpModeGet: %d", dev);

        /* validation values */
        UTF_VERIFY_EQUAL1_STRING_MAC(vidUpMode, vidUpModeGet,
                   "got another vidUpMode then was set: %d", dev);

        /*
            1.3. Call with out of range vidUpMode [wrong enum values],
                           other params same as in 1.1.
            Expected: NOT GT_OK.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclEgressKeyFieldsVidUpModeSet
                            (dev, vidUpMode),
                            vidUpMode);
    }

    vidUpMode = CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclEgressKeyFieldsVidUpModeSet(dev, vidUpMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclEgressKeyFieldsVidUpModeSet(dev, vidUpMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclEgressKeyFieldsVidUpModeGet
(
    IN   GT_U8                                      devNum,
    OUT  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   *vidUpModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclEgressKeyFieldsVidUpModeGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with non NULL vidUpModePtr.
    Expected: GT_OK.
    1.2. Call with vidUpModePtr [NULL],
                   other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT vidUpMode =
                            CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non NULL vidUpModePtr.
            Expected: GT_OK.
        */
        st = cpssDxChPclEgressKeyFieldsVidUpModeGet(dev, &vidUpMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidUpMode);

        /*
            1.2. Call with vidUpModePtr [NULL],
                           other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclEgressKeyFieldsVidUpModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vidUpModePtr = NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclEgressKeyFieldsVidUpModeGet(dev, &vidUpMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclEgressKeyFieldsVidUpModeGet(dev, &vidUpMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Rule table.
*/
UTF_TEST_CASE_MAC(cpssDxChPclFillRuleTable)
{
/*
    ITERATE_DEVICE (DxChx)
    1.1. Get table Size.
         Call cpssDxChCfgTableNumEntriesGet with table[CPSS_DXCH_CFG_TABLE_PCL_ACTION_E]
                                                 and not NULL numEntriesPtr.
    Expected: GT_OK.
    1.2. Fill all entries in Rule table.
         Call cpssDxChPclRuleSet with ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
                ruleIndex [0 ... numEntries-1], maskPtr-> ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ],
                patternPtr-> ruleStdNotIp [ common[pclId=0; sourcePort=5; isTagged=1;
                vid=100; up=0; qosProfile=0; isIp=0; isL2Valid=1; isUdbValid =1],
                isIpv4 = 0; etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
                macSa=AB:CD:EF:00:00:01; udb[10,20,30] ],
                actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
                matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
                qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
                qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                profilePrecedence=GT_FALSE] ] },
                redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 },
                vlan { egressTaggedModify=GT_FALSE,
                modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssDxChPclRuleSet with ruleIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in Rule table and compare with original.
         Call cpssDxChPclRuleGet with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                                      ruleIndex [0 ... numEntries-1],
                                      non-NULL mask,
                                      pattern and action.
    Expected: GT_OK.
    1.5. Try to read entry with index out of range.
         Call cpssDxChPclRuleGet with ruleSize [CPSS_PCL_RULE_SIZE_STD_E],
                                      ruleIndex [numEntries],

    Expected: NOT GT_OK.
    1.6. Delete all entries in Rule table.
         Call cpssDxChPclRuleInvalidate with ruleSize and ruleIndex from 1.2.
    Expected: GT_OK.
    1.7. Try to delete entry with index out of range.
         Call cpssDxChPclRuleInvalidate with ruleSize and ruleIndex [numEntries].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat =
                                    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       mask;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       pattern;
    CPSS_DXCH_PCL_ACTION_STC            action;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       maskGet;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       patternGet;
    CPSS_DXCH_PCL_ACTION_STC            actionGet;
    GT_ETHERADDR                        mac1 = {{0xAB, 0xCD, 0xEF, 0x00, 0x00, 0x01}};
    GT_ETHERADDR                        mac2 = {{0xAB, 0xCD, 0xEF, 0x00, 0x00, 0x02}};
    CPSS_PP_FAMILY_TYPE_ENT             devFamily = CPSS_PP_FAMILY_CHEETAH_E;
    CPSS_PCL_RULE_SIZE_ENT              ruleSize  = CPSS_PCL_RULE_SIZE_STD_E;


    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));
    cpssOsBzero((GT_VOID*) &maskGet, sizeof(maskGet));
    cpssOsBzero((GT_VOID*) &patternGet, sizeof(patternGet));
    cpssOsBzero((GT_VOID*) &actionGet, sizeof(actionGet));

    /* Fill the entry for FDB table */
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

    cpssOsMemSet((GT_VOID*) &(mask), 0xFF, sizeof(mask));
    mask.ruleStdNotIp.common.pclId = 0x3FF;

    pattern.ruleStdNotIp.common.pclId = 0;
    pattern.ruleStdNotIp.common.sourcePort = 5;
    pattern.ruleStdNotIp.common.isTagged = 1;
    pattern.ruleStdNotIp.common.vid = 100;
    pattern.ruleStdNotIp.common.up = 0;
    pattern.ruleStdNotIp.common.qosProfile = 0;
    pattern.ruleStdNotIp.common.isIp = 0;
    pattern.ruleStdNotIp.common.isL2Valid = 1;
    pattern.ruleStdNotIp.common.isUdbValid = 1;
    pattern.ruleStdNotIp.isIpv4 = 0;
    pattern.ruleStdNotIp.etherType = 0;
    pattern.ruleStdNotIp.isArp = 0;
    pattern.ruleStdNotIp.l2Encap = 0;
    pattern.ruleStdNotIp.macDa = mac2;
    pattern.ruleStdNotIp.macSa = mac1;
    pattern.ruleStdNotIp.udb[0] = 10;
    pattern.ruleStdNotIp.udb[1] = 20;
    pattern.ruleStdNotIp.udb[2] = 30;

    pclActionDefaultSet(&action);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        st = cpssDxChCfgTableNumEntriesGet(dev, CPSS_DXCH_CFG_TABLE_PCL_ACTION_E, &numEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChCfgTableNumEntriesGet: %d", dev);

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,"prvUtfDeviceFamilyGet: %d", dev);

        if (devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
        {   /* 'disable' not supported on CHEETAH 3 */
            action.qos.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            action.qos.modifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        }

        /* 1.2. Fill all entries in Rule table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            pattern.ruleStdNotIp.common.up  = (GT_U8)(iTemp % 7);
            pattern.ruleStdNotIp.common.vid = (GT_U16)(iTemp % 4095);

            st = cpssDxChPclRuleSet(
                dev, ruleFormat, iTemp, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                        "cpssDxChPclRuleSet: %d, %d, %d", dev, ruleFormat, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssDxChPclRuleSet(
            dev, ruleFormat, numEntries, 0 /*ruleOptionsBmp*/,
            &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                         "cpssDxChPclRuleSet: %d, %d, %d", dev, ruleFormat, numEntries);

        /* 1.4. Read all entries in Rule table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            pattern.ruleStdNotIp.common.up  = (GT_U8)(iTemp % 7);
            pattern.ruleStdNotIp.common.vid = (GT_U16)(iTemp % 4095);

            st = cpssDxChPclRuleGet(dev, ruleSize, iTemp,
                    (GT_U32*)&maskGet, (GT_U32*)&patternGet, (GT_U32*)&actionGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "cpssDxChPclRuleGet: %d, %d, %d", dev, ruleSize, iTemp);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssDxChPclRuleGet(dev, ruleSize, numEntries,
                    (GT_U32*)&maskGet, (GT_U32*)&patternGet, (GT_U32*)&actionGet);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                    "cpssDxChPclRuleGet: %d, %d, %d", dev, ruleSize, numEntries);

        /* 1.6. Delete all entries in Rule table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssDxChPclRuleInvalidate(dev, ruleSize, iTemp);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "cpssDxChPclRuleInvalidate: %d, %d, %d", dev, ruleSize, iTemp);
        }

        /* 1.7. Try to delete entry with index out of range. */
        st = cpssDxChPclRuleInvalidate(dev, ruleSize, numEntries);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "cpssDxChPclRuleInvalidate: %d, %d, %d", dev, ruleSize, numEntries);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclL3L4ParsingOverMplsEnableGet
(
    IN  GT_U8    dev,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclL3L4ParsingOverMplsEnableGet)
{
/*
    ITERATE_DEVICES(All DxChXcat and above devices)
    1.1. Call with not null enablePtr.
    Expected: GT_OK.
    1.2. Call api with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8    dev;
    GT_BOOL  enable;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChPclL3L4ParsingOverMplsEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call api with wrong enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclL3L4ParsingOverMplsEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                   "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclL3L4ParsingOverMplsEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclL3L4ParsingOverMplsEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclL3L4ParsingOverMplsEnableSet
(
    IN  GT_U8   dev,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclL3L4ParsingOverMplsEnableSet)
{
/*
    ITERATE_DEVICES(All DxChXcat and above devices)
    1.1. Call with enable[GT_TRUE / GT_FALSE],
    Expected: GT_OK.
    1.2. Call cpssDxChPclL3L4ParsingOverMplsEnableGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_BOOL enable = GT_FALSE;
    GT_BOOL enableGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable[GT_TRUE / GT_FALSE],
            Expected: GT_OK.
        */
        /* call with enable[GT_TRUE] */
        enable = GT_TRUE;

        st = cpssDxChPclL3L4ParsingOverMplsEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPclL3L4ParsingOverMplsEnableGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChPclL3L4ParsingOverMplsEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPclL3L4ParsingOverMplsEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "got another enable then was set: %d", dev);

        /* call with enable[GT_FALSE] */
        enable = GT_FALSE;

        st = cpssDxChPclL3L4ParsingOverMplsEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call get func again, 1.2. */
        st = cpssDxChPclL3L4ParsingOverMplsEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChPclL3L4ParsingOverMplsEnableGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "got another enable then was set: %d", dev);
    }

    /* restore correct values */
    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPclL3L4ParsingOverMplsEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclL3L4ParsingOverMplsEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupLookupCfgPortListEnableGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_BOOL                       *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupLookupCfgPortListEnableGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(xCat2)
    1.1. Call direction [CPSS_PCL_DIRECTION_INGRESS_E /
                         CPSS_PCL_DIRECTION_EGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_0_E /
                         CPSS_PCL_LOOKUP_1_E],
         subLookupNum [0] and non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_0_E],
         subLookupNum [1] and non-NULL enablePtr.
    Expected: GT_OK.
    1.3. Call direction [CPSS_PCL_DIRECTION_EGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_0_E],
              subLookupNum [2] (not relevant)
              and non-NULL enablePtr.
    Expected: GT_OK.
    1.4. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_1_E],
              subLookupNum [3] (not relevant)
              and non-NULL enablePtr.
    Expected: GT_OK.
    1.5. Check for invalid enum value for direction.
    Expected: GT_BAD_PARAM.
    1.6. Check wrong enum values lookupNum.
    Expected: GT_BAD_PARAM.
    1.7. Check wrong enablePtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                    st = GT_OK;
    GT_U8                        dev;
    GT_PORT_GROUPS_BMP           portGroupsBmp = 1;
    CPSS_PCL_DIRECTION_ENT       direction = CPSS_PCL_DIRECTION_INGRESS_E;
    CPSS_PCL_LOOKUP_NUMBER_ENT   lookupNum = CPSS_PCL_LOOKUP_0_E;
    GT_U32                       subLookupNum = 0;
    GT_BOOL                      enable = GT_FALSE;

    GT_U32    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E
                                         | UTF_CH1_DIAMOND_E
                                         | UTF_CH2_E
                                         | UTF_CH3_E
                                         | UTF_XCAT_E
                                         | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1. Call direction [CPSS_PCL_DIRECTION_INGRESS_E /
                                     CPSS_PCL_DIRECTION_EGRESS_E],
                          lookupNum [CPSS_PCL_LOOKUP_0_E /
                                     CPSS_PCL_LOOKUP_1_E],
                     subLookupNum [0] and non-NULL enablePtr.
                Expected: GT_OK.
            */
            /* Call with direction [CPSS_PCL_DIRECTION_INGRESS_E] */
            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_0_E;
            subLookupNum = 0;

            st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, &enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /* Call with direction [CPSS_PCL_DIRECTION_EGRESS_E] */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_1_E;

            st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, &enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /*
                1.2. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
                          lookupNum [CPSS_PCL_LOOKUP_0_E],
                     subLookupNum [1] and non-NULL enablePtr.
                Expected: GT_OK.
            */
            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_0_E;
            subLookupNum = 1;

            st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, &enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /*
                1.3. Call direction [CPSS_PCL_DIRECTION_EGRESS_E],
                          lookupNum [CPSS_PCL_LOOKUP_0_E],
                          subLookupNum [2] (not relevant)
                          and non-NULL enablePtr.
                Expected: GT_OK.
            */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_0_E;
            subLookupNum = 2;

            st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, &enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /*
                1.4. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
                          lookupNum [CPSS_PCL_LOOKUP_1_E],
                          subLookupNum [3] (not relevant)
                          and non-NULL enablePtr.
                Expected: GT_OK.
            */
            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_1_E;
            subLookupNum = 3;

            st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, &enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /*
                1.5. Check for invalid enum value for direction.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupLookupCfgPortListEnableGet
                                (dev, portGroupsBmp, direction, lookupNum,
                                 subLookupNum, &enable),
                                direction);

            /*
                1.6. Check wrong enum values lookupNum.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupLookupCfgPortListEnableGet
                            (dev, portGroupsBmp, direction, lookupNum,
                             subLookupNum, &enable),
                             lookupNum);

            /*
                1.7. Check wrong enablePtr[NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                    "device: %d, enable = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, &enable);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }

        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                    direction, lookupNum, subLookupNum, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* set correct values for all the parameters (except of device number) */
    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    subLookupNum = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E
                                         | UTF_CH1_DIAMOND_E
                                         | UTF_CH2_E
                                         | UTF_CH3_E
                                         | UTF_XCAT_E
                                         | UTF_LION_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. Call function for non active device and valid parameters. */
        /* Expected: GT_BAD_PARAM.                                        */
        st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                    direction, lookupNum, subLookupNum, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                direction, lookupNum, subLookupNum, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclPortGroupLookupCfgPortListEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclPortGroupLookupCfgPortListEnableSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(xCat2)
    1.1. Call direction [CPSS_PCL_DIRECTION_INGRESS_E /
                         CPSS_PCL_DIRECTION_EGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_0_E /
                         CPSS_PCL_LOOKUP_1_E],
         subLookupNum [0] and enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChPclPortGroupLookupCfgPortListEnableGet
         with the same parameters.
    Expected: GT_OK and the same enable as was set.
    1.3. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_0_E],
         subLookupNum [1] and enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.4. Call direction [CPSS_PCL_DIRECTION_EGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_0_E],
              subLookupNum [2] (not relevant)
              and enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.5. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_1_E],
              subLookupNum [3] (not relevant)
              and enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.6. Check for invalid enum value for direction.
    Expected: GT_BAD_PARAM.
    1.7. Check wrong enum values lookupNum.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                    st = GT_OK;
    GT_U8                        dev;
    GT_PORT_GROUPS_BMP           portGroupsBmp = 1;
    CPSS_PCL_DIRECTION_ENT       direction = CPSS_PCL_DIRECTION_INGRESS_E;
    CPSS_PCL_LOOKUP_NUMBER_ENT   lookupNum = CPSS_PCL_LOOKUP_0_E;
    GT_U32                       subLookupNum = 0;
    GT_BOOL                      enable = GT_FALSE;
    GT_BOOL                      enableGet = GT_TRUE;

    GT_U32    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E
                                         | UTF_CH1_DIAMOND_E
                                         | UTF_CH2_E
                                         | UTF_CH3_E
                                         | UTF_XCAT_E
                                         | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1. Call direction [CPSS_PCL_DIRECTION_INGRESS_E /
                                     CPSS_PCL_DIRECTION_EGRESS_E],
                          lookupNum [CPSS_PCL_LOOKUP_0_E /
                                     CPSS_PCL_LOOKUP_1_E],
                     subLookupNum [0] and enable[GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* Call with direction [CPSS_PCL_DIRECTION_INGRESS_E] */
            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_0_E;
            subLookupNum = 0;
            enable = GT_FALSE;

            st = cpssDxChPclPortGroupLookupCfgPortListEnableSet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /*
                1.2. Call cpssDxChPclPortGroupLookupCfgPortListEnableGet
                     with the same parameters.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "cpssDxChPclPortGroupLookupCfgPortListEnableGet: %d, %d, %d",
                                         dev, direction, subLookupNum);
            /* check value */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "cpssDxChPclPortGroupLookupCfgPortListEnableGet: "
                 "get another enable than was set: dev = %d", dev);

            /* Call with direction [CPSS_PCL_DIRECTION_EGRESS_E] */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_1_E;
            enable = GT_TRUE;

            st = cpssDxChPclPortGroupLookupCfgPortListEnableSet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /*
                1.2. Call cpssDxChPclPortGroupLookupCfgPortListEnableGet
                     with the same parameters.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "cpssDxChPclPortGroupLookupCfgPortListEnableGet: %d, %d, %d",
                                         dev, direction, subLookupNum);
            /* check value */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "cpssDxChPclPortGroupLookupCfgPortListEnableGet: "
                 "get another enable than was set: dev = %d", dev);

            /*
                1.3. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
                          lookupNum [CPSS_PCL_LOOKUP_0_E],
                     subLookupNum [1] and enable[GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_0_E;
            subLookupNum = 1;

            st = cpssDxChPclPortGroupLookupCfgPortListEnableSet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /*
                1.2. Call cpssDxChPclPortGroupLookupCfgPortListEnableGet
                     with the same parameters.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "cpssDxChPclPortGroupLookupCfgPortListEnableGet: %d, %d, %d",
                                         dev, direction, subLookupNum);
            /* check value */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "cpssDxChPclPortGroupLookupCfgPortListEnableGet: "
                 "get another enable than was set: dev = %d", dev);

            /*
                1.4. Call direction [CPSS_PCL_DIRECTION_EGRESS_E],
                          lookupNum [CPSS_PCL_LOOKUP_0_E],
                          subLookupNum [2] (not relevant)
                          and enable[GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_0_E;
            subLookupNum = 2;

            st = cpssDxChPclPortGroupLookupCfgPortListEnableSet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /*
                1.2. Call cpssDxChPclPortGroupLookupCfgPortListEnableGet
                     with the same parameters.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "cpssDxChPclPortGroupLookupCfgPortListEnableGet: %d, %d, %d",
                                         dev, direction, subLookupNum);
            /* check value */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "cpssDxChPclPortGroupLookupCfgPortListEnableGet: "
                 "get another enable than was set: dev = %d", dev);

            /*
                1.5. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
                          lookupNum [CPSS_PCL_LOOKUP_1_E],
                          subLookupNum [3] (not relevant)
                          and enable[GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_1_E;
            subLookupNum = 3;

            st = cpssDxChPclPortGroupLookupCfgPortListEnableSet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /*
                1.2. Call cpssDxChPclPortGroupLookupCfgPortListEnableGet
                     with the same parameters.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPclPortGroupLookupCfgPortListEnableGet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "cpssDxChPclPortGroupLookupCfgPortListEnableGet: %d, %d, %d",
                                         dev, direction, subLookupNum);
            /* check value */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "cpssDxChPclPortGroupLookupCfgPortListEnableGet: "
                 "get another enable than was set: dev = %d", dev);

            /*
                1.6. Check for invalid enum value for direction.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupLookupCfgPortListEnableSet
                                (dev, portGroupsBmp, direction, lookupNum,
                                 subLookupNum, enable),
                                direction);

            /*
                1.7. Check wrong enum values lookupNum.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclPortGroupLookupCfgPortListEnableSet
                            (dev, portGroupsBmp, direction, lookupNum,
                             subLookupNum, enable),
                             lookupNum);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChPclPortGroupLookupCfgPortListEnableSet(dev, portGroupsBmp,
                                        direction, lookupNum, subLookupNum, enable);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }

        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChPclPortGroupLookupCfgPortListEnableSet(dev, portGroupsBmp,
                                    direction, lookupNum, subLookupNum, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    /* set correct values for all the parameters (except of device number) */
    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    subLookupNum = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E
                                         | UTF_CH1_DIAMOND_E
                                         | UTF_CH2_E
                                         | UTF_CH3_E
                                         | UTF_XCAT_E
                                         | UTF_LION_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. Call function for non active device and valid parameters. */
        /* Expected: GT_BAD_PARAM.                                        */
        st = cpssDxChPclPortGroupLookupCfgPortListEnableSet(dev, portGroupsBmp,
                                    direction, lookupNum, subLookupNum, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclPortGroupLookupCfgPortListEnableSet(dev, portGroupsBmp,
                                direction, lookupNum, subLookupNum, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclLookupCfgPortListEnableGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_BOOL                       *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclLookupCfgPortListEnableGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(xCat2)
    1.1. Call direction [CPSS_PCL_DIRECTION_INGRESS_E /
                         CPSS_PCL_DIRECTION_EGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_0_E /
                         CPSS_PCL_LOOKUP_1_E],
         subLookupNum [0] and non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_0_E],
         subLookupNum [1] and non-NULL enablePtr.
    Expected: GT_OK.
    1.3. Call direction [CPSS_PCL_DIRECTION_EGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_0_E],
              subLookupNum [2] (not relevant)
              and non-NULL enablePtr.
    Expected: GT_OK.
    1.4. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_1_E],
              subLookupNum [3] (not relevant)
              and non-NULL enablePtr.
    Expected: GT_OK.
    1.5. Check for invalid enum value for direction.
    Expected: GT_BAD_PARAM.
    1.6. Check wrong enum values lookupNum.
    Expected: GT_BAD_PARAM.
    1.7. Check wrong enablePtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                    st = GT_OK;
    GT_U8                        dev;
    CPSS_PCL_DIRECTION_ENT       direction = CPSS_PCL_DIRECTION_INGRESS_E;
    CPSS_PCL_LOOKUP_NUMBER_ENT   lookupNum = CPSS_PCL_LOOKUP_0_E;
    GT_U32                       subLookupNum = 0;
    GT_BOOL                      enable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E
                                         | UTF_CH1_DIAMOND_E
                                         | UTF_CH2_E
                                         | UTF_CH3_E
                                         | UTF_XCAT_E
                                         | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call direction [CPSS_PCL_DIRECTION_INGRESS_E /
                                 CPSS_PCL_DIRECTION_EGRESS_E],
                      lookupNum [CPSS_PCL_LOOKUP_0_E /
                                 CPSS_PCL_LOOKUP_1_E],
                 subLookupNum [0] and non-NULL enablePtr.
            Expected: GT_OK.
        */
        /* Call with direction [CPSS_PCL_DIRECTION_INGRESS_E] */
        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_0_E;
        subLookupNum = 0;

        st = cpssDxChPclLookupCfgPortListEnableGet(dev,
                                    direction, lookupNum, subLookupNum, &enable);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
             "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

        /* Call with direction [CPSS_PCL_DIRECTION_EGRESS_E] */
        direction = CPSS_PCL_DIRECTION_EGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_1_E;

        st = cpssDxChPclLookupCfgPortListEnableGet(dev,
                                    direction, lookupNum, subLookupNum, &enable);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
             "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

        /*
            1.2. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
                      lookupNum [CPSS_PCL_LOOKUP_0_E],
                 subLookupNum [1] and non-NULL enablePtr.
            Expected: GT_OK.
        */
        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_0_E;
        subLookupNum = 1;

        st = cpssDxChPclLookupCfgPortListEnableGet(dev,
                                    direction, lookupNum, subLookupNum, &enable);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
             "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

        /*
            1.3. Call direction [CPSS_PCL_DIRECTION_EGRESS_E],
                      lookupNum [CPSS_PCL_LOOKUP_0_E],
                      subLookupNum [2] (not relevant)
                      and non-NULL enablePtr.
            Expected: GT_OK.
        */
        direction = CPSS_PCL_DIRECTION_EGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_0_E;
        subLookupNum = 2;

        st = cpssDxChPclLookupCfgPortListEnableGet(dev,
                                    direction, lookupNum, subLookupNum, &enable);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
             "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

        /*
            1.4. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
                      lookupNum [CPSS_PCL_LOOKUP_1_E],
                      subLookupNum [3] (not relevant)
                      and non-NULL enablePtr.
            Expected: GT_OK.
        */
        direction = CPSS_PCL_DIRECTION_INGRESS_E;
        lookupNum = CPSS_PCL_LOOKUP_1_E;
        subLookupNum = 3;

        st = cpssDxChPclLookupCfgPortListEnableGet(dev,
                                    direction, lookupNum, subLookupNum, &enable);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
             "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

        /*
            1.5. Check for invalid enum value for direction.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclLookupCfgPortListEnableGet
                            (dev, direction, lookupNum,
                             subLookupNum, &enable),
                            direction);

        /*
            1.6. Check wrong enum values lookupNum.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPclLookupCfgPortListEnableGet
                        (dev, direction, lookupNum,
                         subLookupNum, &enable),
                         lookupNum);

        /*
            1.7. Check wrong enablePtr[NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPclLookupCfgPortListEnableGet(dev,
                                    direction, lookupNum, subLookupNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                "device: %d, enable = NULL", dev);
    }

    /* set correct values for all the parameters (except of device number) */
    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    subLookupNum = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E
                                         | UTF_CH1_DIAMOND_E
                                         | UTF_CH2_E
                                         | UTF_CH3_E
                                         | UTF_XCAT_E
                                         | UTF_LION_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. Call function for non active device and valid parameters. */
        /* Expected: GT_BAD_PARAM.                                        */
        st = cpssDxChPclLookupCfgPortListEnableGet(dev,
                                    direction, lookupNum, subLookupNum, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclLookupCfgPortListEnableGet(dev,
                                direction, lookupNum, subLookupNum, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPclLookupCfgPortListEnableSet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPclLookupCfgPortListEnableSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(xCat2)
    1.1. Call direction [CPSS_PCL_DIRECTION_INGRESS_E /
                         CPSS_PCL_DIRECTION_EGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_0_E /
                         CPSS_PCL_LOOKUP_1_E],
         subLookupNum [0] and enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChPclLookupCfgPortListEnableGet
         with the same parameters.
    Expected: GT_OK and the same enable as was set.
    1.3. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_0_E],
         subLookupNum [1] and enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.4. Call direction [CPSS_PCL_DIRECTION_EGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_0_E],
              subLookupNum [2] (not relevant)
              and enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.5. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
              lookupNum [CPSS_PCL_LOOKUP_1_E],
              subLookupNum [3] (not relevant)
              and enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.6. Check for invalid enum value for direction.
    Expected: GT_BAD_PARAM.
    1.7. Check wrong enum values lookupNum.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                    st = GT_OK;
    GT_U8                        dev;
    CPSS_PCL_DIRECTION_ENT       direction = CPSS_PCL_DIRECTION_INGRESS_E;
    CPSS_PCL_LOOKUP_NUMBER_ENT   lookupNum = CPSS_PCL_LOOKUP_0_E;
    GT_U32                       subLookupNum = 0;
    GT_BOOL                      enable = GT_FALSE;
    GT_BOOL                      enableGet = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E
                                         | UTF_CH1_DIAMOND_E
                                         | UTF_CH2_E
                                         | UTF_CH3_E
                                         | UTF_XCAT_E
                                         | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
            /*
                1.1. Call direction [CPSS_PCL_DIRECTION_INGRESS_E /
                                     CPSS_PCL_DIRECTION_EGRESS_E],
                          lookupNum [CPSS_PCL_LOOKUP_0_E /
                                     CPSS_PCL_LOOKUP_1_E],
                     subLookupNum [0] and enable[GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* Call with direction [CPSS_PCL_DIRECTION_INGRESS_E] */
            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_0_E;
            subLookupNum = 0;
            enable = GT_FALSE;

            st = cpssDxChPclLookupCfgPortListEnableSet(dev,
                                        direction, lookupNum, subLookupNum, enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /*
                1.2. Call cpssDxChPclLookupCfgPortListEnableGet
                     with the same parameters.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPclLookupCfgPortListEnableGet(dev,
                                        direction, lookupNum, subLookupNum, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "cpssDxChPclLookupCfgPortListEnableGet: %d, %d, %d",
                                         dev, direction, subLookupNum);
            /* check value */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "cpssDxChPclLookupCfgPortListEnableGet: "
                 "get another enable than was set: dev = %d", dev);

            /* Call with direction [CPSS_PCL_DIRECTION_EGRESS_E] */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_1_E;
            enable = GT_TRUE;

            st = cpssDxChPclLookupCfgPortListEnableSet(dev,
                                        direction, lookupNum, subLookupNum, enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /*
                1.2. Call cpssDxChPclLookupCfgPortListEnableGet
                     with the same parameters.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPclLookupCfgPortListEnableGet(dev,
                                        direction, lookupNum, subLookupNum, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "cpssDxChPclLookupCfgPortListEnableGet: %d, %d, %d",
                                         dev, direction, subLookupNum);
            /* check value */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "cpssDxChPclLookupCfgPortListEnableGet: "
                 "get another enable than was set: dev = %d", dev);

            /*
                1.3. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
                          lookupNum [CPSS_PCL_LOOKUP_0_E],
                     subLookupNum [1] and enable[GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_0_E;
            subLookupNum = 1;

            st = cpssDxChPclLookupCfgPortListEnableSet(dev,
                                        direction, lookupNum, subLookupNum, enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /*
                1.2. Call cpssDxChPclLookupCfgPortListEnableGet
                     with the same parameters.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPclLookupCfgPortListEnableGet(dev,
                                        direction, lookupNum, subLookupNum, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "cpssDxChPclLookupCfgPortListEnableGet: %d, %d, %d",
                                         dev, direction, subLookupNum);
            /* check value */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "cpssDxChPclLookupCfgPortListEnableGet: "
                 "get another enable than was set: dev = %d", dev);

            /*
                1.4. Call direction [CPSS_PCL_DIRECTION_EGRESS_E],
                          lookupNum [CPSS_PCL_LOOKUP_0_E],
                          subLookupNum [2] (not relevant)
                          and enable[GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            direction = CPSS_PCL_DIRECTION_EGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_0_E;
            subLookupNum = 2;

            st = cpssDxChPclLookupCfgPortListEnableSet(dev,
                                        direction, lookupNum, subLookupNum, enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /*
                1.2. Call cpssDxChPclLookupCfgPortListEnableGet
                     with the same parameters.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPclLookupCfgPortListEnableGet(dev,
                                        direction, lookupNum, subLookupNum, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "cpssDxChPclLookupCfgPortListEnableGet: %d, %d, %d",
                                         dev, direction, subLookupNum);
            /* check value */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "cpssDxChPclLookupCfgPortListEnableGet: "
                 "get another enable than was set: dev = %d", dev);

            /*
                1.5. Call direction [CPSS_PCL_DIRECTION_INGRESS_E],
                          lookupNum [CPSS_PCL_LOOKUP_1_E],
                          subLookupNum [3] (not relevant)
                          and enable[GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            direction = CPSS_PCL_DIRECTION_INGRESS_E;
            lookupNum = CPSS_PCL_LOOKUP_1_E;
            subLookupNum = 3;

            st = cpssDxChPclLookupCfgPortListEnableSet(dev,
                                        direction, lookupNum, subLookupNum, enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "Dev, direction, subLookupNum: %d, %d, %d", dev, direction, subLookupNum);

            /*
                1.2. Call cpssDxChPclLookupCfgPortListEnableGet
                     with the same parameters.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPclLookupCfgPortListEnableGet(dev,
                                        direction, lookupNum, subLookupNum, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                 "cpssDxChPclLookupCfgPortListEnableGet: %d, %d, %d",
                                         dev, direction, subLookupNum);
            /* check value */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                 "cpssDxChPclLookupCfgPortListEnableGet: "
                 "get another enable than was set: dev = %d", dev);

            /*
                1.6. Check for invalid enum value for direction.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclLookupCfgPortListEnableSet
                                (dev, direction, lookupNum,
                                 subLookupNum, enable),
                                direction);

            /*
                1.7. Check wrong enum values lookupNum.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPclLookupCfgPortListEnableSet
                            (dev, direction, lookupNum,
                             subLookupNum, enable),
                             lookupNum);
    }

    /* set correct values for all the parameters (except of device number) */
    direction = CPSS_PCL_DIRECTION_INGRESS_E;
    subLookupNum = 0;

    /*2. Go over all non active devices. */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E
                                         | UTF_CH1_DIAMOND_E
                                         | UTF_CH2_E
                                         | UTF_CH3_E
                                         | UTF_XCAT_E
                                         | UTF_LION_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. Call function for non active device and valid parameters. */
        /* Expected: GT_BAD_PARAM.                                        */
        st = cpssDxChPclLookupCfgPortListEnableSet(dev,
                                    direction, lookupNum, subLookupNum, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                 */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPclLookupCfgPortListEnableSet(dev,
                                direction, lookupNum, subLookupNum, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChPcl suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChPcl)

    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclUserDefinedByteSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclRuleSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclRuleActionUpdate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclRuleActionGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclRuleInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclRuleValidStatusSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclRuleCopy)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclRuleStateGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclRuleAnyStateGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclRuleGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclRuleMatchCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclRuleMatchCounterSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortIngressPolicyEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortIngressPolicyEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortLookupCfgTabAccessModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclCfgTblAccessModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclCfgTblAccessModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclCfgTblSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclCfgTblGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclIngressPolicyEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclTcamRuleSizeModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclTwoLookupsCpuCodeResolution)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclInvalidUdbCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclIpLengthCheckModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclCfgTblEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxCh2PclEgressPolicyEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclEgressForRemoteTunnelStartEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclEgressForRemoteTunnelStartEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxCh3PclTunnelTermForceVlanModeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxCh3PclTunnelTermForceVlanModeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxCh2EgressPclPacketTypesSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxCh2PclTcpUdpPortComparatorSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclOverrideUserDefinedBytesGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclOverrideUserDefinedBytesSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclUserDefinedByteGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclInit)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclEgressKeyFieldsVidUpModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclEgressKeyFieldsVidUpModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclUdeEtherTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclUdeEtherTypeGet)

    /*port group api UT*/
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupCfgTblEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupCfgTblSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupCfgTblGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupOverrideUserDefinedBytesSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupOverrideUserDefinedBytesGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupRuleActionGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupRuleActionUpdate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupRuleAnyStateGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupRuleCopy)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupRuleGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupRuleInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupRuleSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupRuleValidStatusSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupTcpUdpPortComparatorSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupUserDefinedByteSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupUserDefinedByteGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclL3L4ParsingOverMplsEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclL3L4ParsingOverMplsEnableSet)

    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupLookupCfgPortListEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclPortGroupLookupCfgPortListEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclLookupCfgPortListEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclLookupCfgPortListEnableSet)


    /* Tests filling Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPclFillRuleTable)

UTF_SUIT_END_TESTS_MAC(cpssDxChPcl)

/*----------------------------------------------------------------------------*/

/*  Internal function. Is used for filling PCL action structure     */
/*  with default values which are used for most of all tests.       */
static void pclActionDefaultSet
(
    IN CPSS_DXCH_PCL_ACTION_STC     *actionPtr
)
{
/*  actionPtr [ pktCmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
    mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
    matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
    qos { egressPolicy=GT_FALSE, modifyDscp=CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E,
    modifyUp=CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E,
    qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
    profilePrecedence=GT_FALSE] ] },
    redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
    data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 },
    vlan { egressTaggedModify=GT_FALSE,
    modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
    vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
    ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
*/
    cpssOsBzero((GT_VOID*)actionPtr, sizeof(CPSS_DXCH_PCL_ACTION_STC));

    actionPtr->pktCmd       = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
    actionPtr->actionStop   = GT_TRUE;
    actionPtr->egressPolicy = GT_FALSE;

    actionPtr->mirror.cpuCode = CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E;
    actionPtr->mirror.mirrorToRxAnalyzerPort = GT_FALSE;

    actionPtr->matchCounter.enableMatchCount = GT_FALSE;
    actionPtr->matchCounter.matchCounterIndex = 0;

    actionPtr->qos.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
    actionPtr->qos.modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
    actionPtr->qos.qos.ingress.profileIndex = 0;
    actionPtr->qos.qos.ingress.profileAssignIndex = GT_FALSE;
    actionPtr->qos.qos.ingress.profilePrecedence =
                                CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    actionPtr->redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;

    actionPtr->redirect.data.outIf.outInterface.type   = CPSS_INTERFACE_VID_E;
    actionPtr->redirect.data.outIf.outInterface.vlanId = 100;

    actionPtr->redirect.data.outIf.vntL2Echo     = GT_FALSE;
    actionPtr->redirect.data.outIf.tunnelStart   = GT_FALSE;
    actionPtr->redirect.data.outIf.tunnelType    =
                            CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E;

    actionPtr->policer.policerEnable = GT_FALSE;
    actionPtr->policer.policerId     = 0;

    actionPtr->vlan.modifyVlan = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
    actionPtr->vlan.nestedVlan = GT_FALSE;
    actionPtr->vlan.precedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    actionPtr->vlan.vlanId     = 100;

    actionPtr->ipUcRoute.doIpUcRoute       = GT_FALSE;
    actionPtr->ipUcRoute.arpDaIndex        = 0;
    actionPtr->ipUcRoute.decrementTTL      = GT_FALSE;
    actionPtr->ipUcRoute.bypassTTLCheck    = GT_FALSE;
    actionPtr->ipUcRoute.icmpRedirectCheck = GT_FALSE;

    actionPtr->sourceId.assignSourceId = GT_TRUE;
    actionPtr->sourceId.sourceIdValue  = 0;
}

/*----------------------------------------------------------------------------*/

/*  Internal function. Is used to set rule for testing.             */
/*  Set default values for structures and call cpssDxChPclRuleSet.  */
/*  Returns result of  cpssDxChPclRuleSet function.                 */

/*   set rule for testing.
    Call cpssDxChPclRuleSet with
    ruleFormat [CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E],
    ruleIndex [PCL_TESTED_RULE_INDEX], maskPtr-> ruleStdNotIp [0xFF, 0xFF, 0x00, 0x00, ...],
    patternPtr-> ruleStdNotIp [ common[pclId=0; sourcePort=5; isTagged=1;
    vid=100; up=0; qosProfile=0; isIp=0; isL2Valid=1; isUdbValid =1],
    isIpv4 = 0; etherType=0; isArp=0; l2Encap=0; macDa=AB:CD:EF:00:00:02;
    macSa=AB:CD:EF:00:00:01; udb[10,20,30] ],
    actionPtr [ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
    mirror{cpuCode = 0, mirrorToRxAnalyzerPort = GT_FALSE},
    matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
    qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
    qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
    profilePrecedence=GT_FALSE] ] },
    redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
    data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 },
    vlan { egressTaggedModify=GT_FALSE,
    modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
    vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
    ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } ].
    Expected: GT_OK for Cheetah/Cheetah2  devices and GT_BAD_PARAM for others.
*/
static GT_STATUS pclRuleTestingDefaultSet
(
    IN GT_U8     dev
)
{
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    GT_U32                              ruleIndex;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       mask;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       pattern;
    CPSS_DXCH_PCL_ACTION_STC            action;
    GT_ETHERADDR                        mac1 = {{0xAB, 0xCD, 0xEF, 0x00, 0x00, 0x01}};
    GT_ETHERADDR                        mac2 = {{0xAB, 0xCD, 0xEF, 0x00, 0x00, 0x02}};
    CPSS_PP_FAMILY_TYPE_ENT             devFamily;

    /* check if dev active and Dx Cheetah*/
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    ruleFormat = PCL_TESTED_RULE_FORMAT;
    ruleIndex = PCL_TESTED_RULE_INDEX;

    cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
    mask.ruleStdNotIp.common.pclId = 0xFFFF;

    cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
    pattern.ruleStdNotIp.common.pclId = 0;
    pattern.ruleStdNotIp.common.sourcePort = 5;
    pattern.ruleStdNotIp.common.isTagged = 1;
    pattern.ruleStdNotIp.common.vid = 100;
    pattern.ruleStdNotIp.common.up = 0;
    pattern.ruleStdNotIp.common.qosProfile = 0;
    pattern.ruleStdNotIp.common.isIp = 0;
    pattern.ruleStdNotIp.common.isL2Valid = 1;
    pattern.ruleStdNotIp.common.isUdbValid = 1;
    pattern.ruleStdNotIp.isIpv4 = 0;
    pattern.ruleStdNotIp.etherType = 0;
    pattern.ruleStdNotIp.isArp = 0;
    pattern.ruleStdNotIp.l2Encap = 0;
    pattern.ruleStdNotIp.macDa = mac2;
    pattern.ruleStdNotIp.macSa = mac1;
    pattern.ruleStdNotIp.udb[0] = 10;
    pattern.ruleStdNotIp.udb[1] = 20;
    pattern.ruleStdNotIp.udb[2] = 30;

    pclActionDefaultSet(&action);

    prvUtfDeviceFamilyGet(dev, &devFamily);
    if (devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
    {   /* 'disable' not supported on CHEETAH 3 */
        action.qos.modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        action.qos.modifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
    }

    cpssDxChPclRuleSet(
        dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
        &mask, &pattern, &action);

    cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
    ruleIndex = 0;
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
    cpssDxChPclRuleSet(
        dev, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
        &mask, &pattern, &action);

    return GT_OK;
}


