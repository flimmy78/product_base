/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxTgBrgGenUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxTgBrgGen.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/exMx/exMxTg/bridge/cpssExMxTgBrgGen.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* From cpssExMxTgBrgGen.c code!!!!         */
/* Be careful to keep them synchronized!    */
#define CPSS_EXMX_IPV6_ICMP_MSG_TYPE_FIELD_NUM_CNS 8

/* Invalid enum */
#define TG_BRIDGE_INVALID_ENUM_CNS         0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgBrgGenEgressArpBcMirrorEnable
(
    IN GT_U8        dev,
    IN GT_BOOL      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgBrgGenEgressArpBcMirrorEnable)
{
/*
ITERATE_DEVICES
1.1. Call function with enable [GT_TRUE and GT_FALSE]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
*/

    GT_STATUS   st = GT_OK;

    GT_U8                           dev;
    GT_BOOL                         enable;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Call function with [enable==GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxTgBrgGenEgressArpBcMirrorEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
            "Tiger device: %d, %d", dev, enable);

        /* Call function with [enable==GT_FALSE] */
        enable = GT_FALSE;

        st = cpssExMxTgBrgGenEgressArpBcMirrorEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
            "Tiger device: %d, %d", dev, enable);
    }

    /*2. Go over all non active devices. */
    enable = GT_TRUE;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE)) {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgBrgGenEgressArpBcMirrorEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgBrgGenEgressArpBcMirrorEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgBrgGenIfIgmpSnoopModeSet
(
    IN GT_U8                       dev,
    IN CPSS_IGMP_SNOOP_MODE_ENT    mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgBrgGenIfIgmpSnoopModeSet)
{
/*
ITERATE_DEVICES
1.1. Call function with  mode [CPSS_IGMP_ALL_TRAP_MODE_E, CPSS_IGMP_ROUTER_MIRROR_MODE_E, CPSS_IGMP_FWD_DECISION_MODE_E]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. For Tiger devices call function with mode [CPSS_IGMP_SNOOP_TRAP_MODE_E] (unsupported). Expected: GT_BAD_PARAM.
1.3. For Tiger devices call function with out-of-range mode [0x5AAAAAA5]. Expected: GT_BAD_PARAM
*/

    GT_STATUS   st = GT_OK;

    GT_U8                      dev;
    CPSS_IGMP_SNOOP_MODE_ENT   mode;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*1.1. Call function with  mode [CPSS_IGMP_ALL_TRAP_MODE_E,        */
        /*                               CPSS_IGMP_ROUTER_MIRROR_MODE_E,   */
        /*                               CPSS_IGMP_FWD_DECISION_MODE_E].   */
        /*     Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others. */

        /* Call function with [mode==CPSS_IGMP_ALL_TRAP_MODE_E] */
        mode = CPSS_IGMP_ALL_TRAP_MODE_E;

        st = cpssExMxTgBrgGenIfIgmpSnoopModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d", dev, mode);

        /* Call function with [mode==CPSS_IGMP_ROUTER_MIRROR_MODE_E] */
        mode = CPSS_IGMP_ROUTER_MIRROR_MODE_E;

        st = cpssExMxTgBrgGenIfIgmpSnoopModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d", dev, mode);

        /* Call function with [mode==CPSS_IGMP_FWD_DECISION_MODE_E] */
        mode = CPSS_IGMP_FWD_DECISION_MODE_E;

        st = cpssExMxTgBrgGenIfIgmpSnoopModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d", dev, mode);

        /*1.2. For Tiger devices call function with
        mode [CPSS_IGMP_SNOOP_TRAP_MODE_E] (unsupported).
        Expected: GT_BAD_PARAM. */
        mode = CPSS_IGMP_SNOOP_TRAP_MODE_E;

        st = cpssExMxTgBrgGenIfIgmpSnoopModeSet(dev, mode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "Tiger device: %d, %d", dev, mode);

        /*1.3. For Tiger devices call function with                     */
        /*     out-of-range mode [0x5AAAAAA5]. Expected: GT_BAD_PARAM   */
            mode = TG_BRIDGE_INVALID_ENUM_CNS;

            st = cpssExMxTgBrgGenIfIgmpSnoopModeSet(dev, mode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "Tiger device: %d, %d", dev, mode);
    }

    /*2. Go over all non active devices. */
    mode = CPSS_IGMP_ALL_TRAP_MODE_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgBrgGenIfIgmpSnoopModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgBrgGenIfIgmpSnoopModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgBrgGenIcmpv6MsgTypeSet
(
    IN GT_U8                                    dev,
    IN GT_U8                                    msgType,
    IN CPSS_EXMX_BRIDGE_CNTRL_PACKET_MODE_ENT   packetCmd,
    IN GT_U32                                   index
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgBrgGenIcmpv6MsgTypeSet)
{
/*
ITERATE_DEVICES
1.1. Call function with valid msgType [0], valid packetCmd [CPSS_EXMX_BRIDGE_CNTRL_TRAP_E], valid index [0]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. For Tiger devices call function with valid msgType[0], unsupported (accordingly to function's contract) packetCmd [CPSS_EXMX_BRIDGE_CNTRL_TRANSPARENT_E and CPSS_EXMX_BRIDGE_CNTRL_SOFT_DROP_E], valid index [0]. Expected: non GT_OK.
1.3. Check for invalid enum. For Tiger devices call function with msgType[0], packetCmd [0x5AAAAAA5], index [0]. Expected: GT_BAD_PARAM.
1.4. For Tiger devices call function with valid msgType[0], valid packetCmd [CPSS_EXMX_BRIDGE_CNTRL_TRAP_E], invalid index [CPSS_EXMX_IPV6_ICMP_MSG_TYPE_FIELD_NUM_CNS]. Expected: non GT_OK.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                                   dev;
    GT_U8                                   msgType;
    CPSS_EXMX_BRIDGE_CNTRL_PACKET_MODE_ENT  packetCmd;
    GT_U32                                  index;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*1.1. Call function with valid msgType [0], valid packetCmd          */
        /*     [CPSS_EXMX_BRIDGE_CNTRL_TRAP_E], valid index [0].              */    
        /*     Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others. */
        msgType = 0;
        packetCmd = CPSS_EXMX_BRIDGE_CNTRL_TRAP_E;
        index = 0;

        st = cpssExMxTgBrgGenIcmpv6MsgTypeSet(dev, msgType, packetCmd, index);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d, %d",
                                     dev, msgType, packetCmd, index);

        /*1.2. For Tiger devices call function with valid msgType[0],       */
        /*     unsupported (accordingly to function's contract) packetCmd   */
        /*     [CPSS_EXMX_BRIDGE_CNTRL_TRANSPARENT_E and                    */
        /*      CPSS_EXMX_BRIDGE_CNTRL_SOFT_DROP_E], valid index [0].       */
        /*     Expected: non GT_OK.                                         */
        msgType = 0;
        packetCmd = CPSS_EXMX_BRIDGE_CNTRL_TRANSPARENT_E;
        index = 0;
        st = cpssExMxTgBrgGenIcmpv6MsgTypeSet(dev, msgType, packetCmd, index);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d, %d",
                                        dev, msgType, packetCmd, index);

        packetCmd = CPSS_EXMX_BRIDGE_CNTRL_SOFT_DROP_E;
        st = cpssExMxTgBrgGenIcmpv6MsgTypeSet(dev, msgType, packetCmd, index);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d, %d",
                                        dev, msgType, packetCmd, index);

        /* 1.3. Check for invalid enum. For Tiger devices call function */
        /*      with msgType[0], packetCmd [0x5AAAAAA5], index [0].     */
        /*      Expected: GT_BAD_PARAM.                                 */
        packetCmd = TG_BRIDGE_INVALID_ENUM_CNS;

        st = cpssExMxTgBrgGenIcmpv6MsgTypeSet(dev, msgType, packetCmd, index);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st, "Tiger device: %d, %d, %d, %d",
                                    dev, msgType, packetCmd, index);

        /* 1.4. For Tiger devices call function with valid msgType[0],      */
        /*      valid packetCmd [CPSS_EXMX_BRIDGE_CNTRL_TRAP_E],            */
        /*      invalid index [CPSS_EXMX_IPV6_ICMP_MSG_TYPE_FIELD_NUM_CNS]. */
        /*      Expected: non GT_OK                                         */
        msgType = 0;
        packetCmd = CPSS_EXMX_BRIDGE_CNTRL_TRAP_E;
        index = CPSS_EXMX_IPV6_ICMP_MSG_TYPE_FIELD_NUM_CNS;

        st = cpssExMxTgBrgGenIcmpv6MsgTypeSet(dev, msgType, packetCmd, index);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d, %d",
                                        dev, msgType, packetCmd, index);
    }

    /*2. Go over all non active devices. */
    msgType = 0;
    packetCmd = CPSS_EXMX_BRIDGE_CNTRL_TRAP_E;
    index = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgBrgGenIcmpv6MsgTypeSet(dev, msgType, packetCmd, index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgBrgGenIcmpv6MsgTypeSet(dev, msgType, packetCmd, index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxTgBrgGen suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxTgBrgGen)
UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgBrgGenEgressArpBcMirrorEnable)
UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgBrgGenIfIgmpSnoopModeSet)
UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgBrgGenIcmpv6MsgTypeSet)
UTF_SUIT_END_TESTS_MAC(cpssExMxTgBrgGen)



