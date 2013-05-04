/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* cpssDxChTtiUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChTti, that provides
*       CPSS tunnel termination.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* includes */
#include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <cpssCommon/cpssPresteraDefs.h>

/* defines */

/* Default valid value for port id */
#define TTI_VALID_PHY_PORT_CNS  0

/* Default valid value for port/trunk id */
#define TTI_VALID_PORT_TRUNK_CNS 20

#define BIT16                   0x00010000

/* maximum value for source id field */
#define TTI_SOURCE_ID_MAX_CNS   31

/* maximum value for time to live (TTL) field */
#define TTI_TTL_MAX_CNS         255

/* max value for trunk */
#define TTI_TRUNK_MAX_CNS       127

/* max value for central counter index */
#define TTI_COUNTER_MAX_CNS     16383

/* Private function forward declaration */
static GT_U32 prvGetMaxRuleIndex    (IN GT_U8  devNum);

static void prvSetDefaultIpv4Pattern(IN CPSS_DXCH_TTI_RULE_UNT *patternPtr);

static void prvSetDefaultMplsPattern(IN CPSS_DXCH_TTI_RULE_UNT *patternPtr);

static void prvSetDefaultEthPattern (IN CPSS_DXCH_TTI_RULE_UNT *patternPtr);

static void prvSetDefaultIpv4Mask   (IN CPSS_DXCH_TTI_RULE_UNT *patternPtr);

static void prvSetDefaultMplsMask   (IN CPSS_DXCH_TTI_RULE_UNT *patternPtr);

static void prvSetDefaultEthMask    (IN CPSS_DXCH_TTI_RULE_UNT *patternPtr);

static void prvSetDefaultAction     (IN CPSS_DXCH_TTI_ACTION_UNT *actionPtr, CPSS_DXCH_TTI_ACTION_TYPE_ENT type);


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiMacToMeSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *valuePtr,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *maskPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiMacToMeSet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: DxCh3; xCat; Lion; xCat2)
    1.1. Call function with entryIndex [0 / 7],
                            valuePtr {mac [[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA]],
                                      vlanId [100]},
                            maskPtr {mac [[0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]],
                                      vlanId [0x0FFF]}
    Expected: GT_OK.
    1.2. Call function cpssDxChTtiMacToMeGet with not-NULL valuePtr,
                                                  not-NULL maskPtr
                                                  and other parameter from 1.1.
    Expected: GT_OK and value, mask the same as was set.
    1.3. Call function with out of range entryIndex [8]
                            and other parameter from 1.1.
    Expected: NOT GT_OK.
    1.4. Call function with valuePtr [NULL]
                            and other parameter from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call function with maskPtr [NULL]
                            and other parameter from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      entryIndex = 0;
    CPSS_DXCH_TTI_MAC_VLAN_STC  value;
    CPSS_DXCH_TTI_MAC_VLAN_STC  mask;
    CPSS_DXCH_TTI_MAC_VLAN_STC  valueGet;
    CPSS_DXCH_TTI_MAC_VLAN_STC  maskGet;
    GT_BOOL                     isEqual = GT_FALSE;


    cpssOsBzero((GT_VOID*) &valueGet, sizeof(valueGet));
    cpssOsBzero((GT_VOID*) &maskGet, sizeof(maskGet));
    cpssOsBzero((GT_VOID*) &value, sizeof(value));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with entryIndex [0 / 7],
                                    valuePtr {mac [[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA]],
                                              vlanId [100]},
                                    maskPtr {mac [[0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]],
                                             vlanId [0x0FFF]}
            Expected: GT_OK.
        */
        /* iterate with entryIndex = 0 */
        entryIndex = 0;

        value.mac.arEther[0] = 0xAA;
        value.mac.arEther[1] = 0xAA;
        value.mac.arEther[2] = 0xAA;
        value.mac.arEther[3] = 0xAA;
        value.mac.arEther[4] = 0xAA;
        value.mac.arEther[5] = 0xAA;

        value.vlanId = 100;

        mask.mac.arEther[0] = 0xFF;
        mask.mac.arEther[1] = 0xFF;
        mask.mac.arEther[2] = 0xFF;
        mask.mac.arEther[3] = 0xFF;
        mask.mac.arEther[4] = 0xFF;
        mask.mac.arEther[5] = 0xFF;

        mask.vlanId = 0x0FFF;

        st = cpssDxChTtiMacToMeSet(dev, entryIndex, &value, &mask);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call function cpssDxChTtiMacToMeGet with not-NULL valuePtr,
                                                          not-NULL maskPtr
                                                          and other parameter from 1.1.
            Expected: GT_OK and value, mask the same as was set.
        */
        st = cpssDxChTtiMacToMeGet(dev, entryIndex, &valueGet, &maskGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiMacToMeGet: %d, %d", dev, entryIndex);

        /* validation values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &value,
                                     (GT_VOID*) &valueGet,
                                     sizeof(value))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another value then was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask,
                                     (GT_VOID*) &maskGet,
                                     sizeof(mask))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another mask then was set: %d", dev);

        /* iterate with entryIndex = 7 */
        entryIndex = 7;

        st = cpssDxChTtiMacToMeSet(dev, entryIndex, &value, &mask);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call function cpssDxChTtiMacToMeGet with not-NULL valuePtr,
                                                          not-NULL maskPtr
                                                          and other parameter from 1.1.
            Expected: GT_OK and value, mask the same as was set.
        */
        st = cpssDxChTtiMacToMeGet(dev, entryIndex, &valueGet, &maskGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiMacToMeGet: %d, %d", dev, entryIndex);

        /* validation values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &value,
                                     (GT_VOID*) &valueGet,
                                     sizeof(value))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another value then was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask,
                                     (GT_VOID*) &maskGet,
                                     sizeof(mask))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another mask then was set: %d", dev);

        /*
            1.3. Call function with out of range entryIndex [8]
                                    and other parameter from 1.1.
            Expected: NOT GT_OK.
        */
        entryIndex = 8;

        st = cpssDxChTtiMacToMeSet(dev, entryIndex, &value, &mask);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        entryIndex = 0;

        /*
            1.4. Call function with valuePtr [NULL]
                                    and other parameter from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiMacToMeSet(dev, entryIndex, NULL, &mask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, valuePtr = NULL", dev);

        /*
            1.5. Call function with maskPtr [NULL]
                                    and other parameter from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiMacToMeSet(dev, entryIndex, &value, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);
    }

    entryIndex = 0;

    value.mac.arEther[0] = 0xAA;
    value.mac.arEther[1] = 0xAA;
    value.mac.arEther[2] = 0xAA;
    value.mac.arEther[3] = 0xAA;
    value.mac.arEther[4] = 0xAA;
    value.mac.arEther[5] = 0xAA;

    value.vlanId = 0;

    mask.mac.arEther[0] = 0xFF;
    mask.mac.arEther[1] = 0xFF;
    mask.mac.arEther[2] = 0xFF;
    mask.mac.arEther[3] = 0xFF;
    mask.mac.arEther[4] = 0xFF;
    mask.mac.arEther[5] = 0xFF;

    mask.vlanId = 0xFFF;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiMacToMeSet(dev, entryIndex, &value, &mask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiMacToMeSet(dev, entryIndex, &value, &mask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiMacToMeGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *valuePtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *maskPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiMacToMeGet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: DxCh3; xCat; Lion; xCat2)
    1.1. Call function with entryIndex [0 / 7],
                            not-NULL valuePtr,
                            not-NULL maskPtr.
    Expected: GT_OK.
    1.2. Call function with out of range entryIndex [8]
                            and other parameter from 1.1.
    Expected: NOT GT_OK.
    1.3. Call function with valuePtr [NULL]
                            and other parameter from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call function with maskPtr [NULL]
                            and other parameter from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      entryIndex = 0;
    CPSS_DXCH_TTI_MAC_VLAN_STC  value;
    CPSS_DXCH_TTI_MAC_VLAN_STC  mask;


    cpssOsBzero((GT_VOID*) &value, sizeof(value));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with entryIndex [0 / 7],
                                    not-NULL valuePtr,
                                    not-NULL maskPtr.
            Expected: GT_OK.
        */
        /* iterate with entryIndex = 0 */
        entryIndex = 0;

        st = cpssDxChTtiMacToMeGet(dev, entryIndex, &value, &mask);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /* iterate with entryIndex = 7 */
        entryIndex = 7;

        st = cpssDxChTtiMacToMeGet(dev, entryIndex, &value, &mask);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call function with out of range entryIndex [8]
                                    and other parameter from 1.1.
            Expected: NOT GT_OK.
        */
        entryIndex = 8;

        st = cpssDxChTtiMacToMeGet(dev, entryIndex, &value, &mask);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_HW_ERROR, st, dev, entryIndex);

        entryIndex = 0;

        /*
            1.3. Call function with valuePtr [NULL]
                                    and other parameter from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiMacToMeGet(dev, entryIndex, NULL, &mask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, valuePtr = NULL", dev);

        /*
            1.4. Call function with maskPtr [NULL]
                                    and other parameter from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiMacToMeGet(dev, entryIndex, &value, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);
    }

    entryIndex = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiMacToMeGet(dev, entryIndex, &value, &mask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiMacToMeGet(dev, entryIndex, &value, &mask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortLookupEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_BOOL                             enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortLookupEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1.1. Call function with keyType [CPSS_DXCH_TTI_KEY_IPV4_E /
                                       CPSS_DXCH_TTI_KEY_ETH_E],
                              enable [GT_TRUE / GT_FALSE]
    Expected: GT_OK.
    1.1.2. Call function cpssDxChTtiPortLookupEnableGet with not-NULL enablePtr
                                                        and other parameter from 1.1.
    Expected: GT_OK and enable the same as was set.
    1.1.3. Call function with wrong enum values keyType
                         and other parameter from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = TTI_VALID_PHY_PORT_CNS;

    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;
    GT_BOOL                     enable    = GT_FALSE;
    GT_BOOL                     enableGet = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call function with keyType [CPSS_DXCH_TTI_KEY_IPV4_E /
                                                   CPSS_DXCH_TTI_KEY_ETH_E],
                                          enable [GT_TRUE / GT_FALSE]
                Expected: GT_OK.
            */
            /* iterate with keyType = CPSS_DXCH_TTI_KEY_IPV4_E */
            keyType = CPSS_DXCH_TTI_KEY_IPV4_E;
            enable  = GT_TRUE;

            st = cpssDxChTtiPortLookupEnableSet(dev, port, keyType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, keyType, enable);

            /*
                1.1.2. Call function cpssDxChTtiPortLookupEnableGet with not-NULL enablePtr
                                                                    and other parameter from 1.1.
                Expected: GT_OK and enableGet the same as was set.
            */
            st = cpssDxChTtiPortLookupEnableGet(dev, port, keyType, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssDxChTtiPortLookupEnableGet: %d, %d, %d", dev, port, keyType);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "got another enable then was set: %d, %d", dev, port);

            /* iterate with keyType = CPSS_DXCH_TTI_KEY_ETH_E */
            keyType = CPSS_DXCH_TTI_KEY_IPV4_E;
            enable  = GT_FALSE;

            st = cpssDxChTtiPortLookupEnableSet(dev, port, keyType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, keyType, enable);

            /*
                1.1.2. Call function cpssDxChTtiPortLookupEnableGet with not-NULL enablePtr
                                                                    and other parameter from 1.1.
                Expected: GT_OK and enableGet the same as was set.
            */
            st = cpssDxChTtiPortLookupEnableGet(dev, port, keyType, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssDxChTtiPortLookupEnableGet: %d, %d, %d", dev, port, keyType);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "got another enable then was set: %d, %d", dev, port);

            /*
                1.1.3. Call function with wrong enum values keyType
                                     and other parameter from 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortLookupEnableSet
                                (dev, port, keyType, enable),
                                keyType);
        }

        keyType = CPSS_DXCH_TTI_KEY_IPV4_E;
        enable  = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChTtiPortLookupEnableSet(dev, port, keyType, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiPortLookupEnableSet(dev, port, keyType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChTtiPortLookupEnableSet(dev, port, keyType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

    }

    keyType = CPSS_DXCH_TTI_KEY_IPV4_E;
    enable  = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TTI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortLookupEnableSet(dev, port, keyType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortLookupEnableSet(dev, port, keyType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortLookupEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    OUT GT_BOOL                             *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortLookupEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1.1. Call function with keyType [CPSS_DXCH_TTI_KEY_IPV4_E /
                                       CPSS_DXCH_TTI_KEY_ETH_E],
                              not-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call function with wrong enum values keyType
                              and other parameter from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.3. Call function with enablePtr [NULL]
                              and other parameter from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = TTI_VALID_PHY_PORT_CNS;

    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType = CPSS_DXCH_TTI_KEY_IPV4_E;
    GT_BOOL                     enable  = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call function with keyType [CPSS_DXCH_TTI_KEY_IPV4_E /
                                                   CPSS_DXCH_TTI_KEY_ETH_E],
                                          not-NULL enablePtr.
                Expected: GT_OK.
            */
            /* iterate with keyType = CPSS_DXCH_TTI_KEY_IPV4_E */
            keyType = CPSS_DXCH_TTI_KEY_IPV4_E;

            st = cpssDxChTtiPortLookupEnableGet(dev, port, keyType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, keyType);

            /* iterate with keyType = CPSS_DXCH_TTI_KEY_ETH_E */
            keyType = CPSS_DXCH_TTI_KEY_ETH_E;

            st = cpssDxChTtiPortLookupEnableGet(dev, port, keyType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, keyType);

            /*
                1.1.2. Call function with wrong enum values keyType
                                          and other parameter from 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortLookupEnableGet
                                (dev, port, keyType, &enable),
                                keyType);

            /*
                1.1.3. Call function with enablePtr [NULL]
                                          and other parameter from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortLookupEnableGet(dev, port, keyType, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        keyType = CPSS_DXCH_TTI_KEY_IPV4_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChTtiPortLookupEnableGet(dev, port, keyType, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiPortLookupEnableGet(dev, port, keyType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_Ok        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChTtiPortLookupEnableGet(dev, port, keyType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

    }

    keyType = CPSS_DXCH_TTI_KEY_IPV4_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TTI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortLookupEnableGet(dev, port, keyType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortLookupEnableGet(dev, port, keyType, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortIpv4OnlyTunneledEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  GT_BOOL                             enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortIpv4OnlyTunneledEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1.1. Call function with enable [GT_TRUE / GT_FALSE]
    Expected: GT_OK.
    1.1.2. Call function cpssDxChTtiPortIpv4OnlyTunneledEnableGet
                with not NULL enablePtr.
    Expected: GT_OK and enable the same as was set.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = TTI_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call function with enable [GT_TRUE / GT_FALSE]
                Expected: GT_OK.
            */
            /* iterate with enable = GT_TRUE */
            enable = GT_TRUE;

            st = cpssDxChTtiPortIpv4OnlyTunneledEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call function cpssDxChTtiPortIpv4OnlyTunneledEnableGet
                            with not NULL enablePtr.
                Expected: GT_OK and enable the same as was set.
            */
            st = cpssDxChTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st,
                       "cpssDxChTtiPortIpv4OnlyTunneledEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL3_PARAM_MAC(enable, enableGet,
                       "got another enable then was set: %d, %d", dev, port);

            /* iterate with enable = GT_FALSE */
            enable = GT_FALSE;

            st = cpssDxChTtiPortIpv4OnlyTunneledEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call function cpssDxChTtiPortIpv4OnlyTunneledEnableGet
                            with not NULL enablePtr.
                Expected: GT_OK and enableGet the same as was set.
            */
            st = cpssDxChTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st,
                       "cpssDxChTtiPortIpv4OnlyTunneledEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL3_PARAM_MAC(enable, enableGet,
                       "got another enable then was set: %d, %d", dev, port);
        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChTtiPortIpv4OnlyTunneledEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiPortIpv4OnlyTunneledEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChTtiPortIpv4OnlyTunneledEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TTI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortIpv4OnlyTunneledEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortIpv4OnlyTunneledEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortIpv4OnlyTunneledEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    OUT GT_BOOL                             *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortIpv4OnlyTunneledEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS
    1.1.1. Call function with not NULL enablePtr
    Expected: GT_OK.
    1.1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = TTI_VALID_PHY_PORT_CNS;

    GT_BOOL     enable = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call function with not NULL enablePtr
                Expected: GT_OK.
            */
            st = cpssDxChTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call function with enablePtr [NULL].
                Expected: GT_BAD_PTR
            */
            st = cpssDxChTtiPortIpv4OnlyTunneledEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);


    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TTI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortIpv4OnlyMacToMeEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  GT_BOOL                             enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortIpv4OnlyMacToMeEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1.1. Call function with enable [GT_TRUE / GT_FALSE]
    Expected: GT_OK.
    1.1.2. Call function cpssDxChTtiPortIpv4OnlyMacToMeEnableGet
                         with not NULL enablePtr.
    Expected: GT_OK and enable the same as was set.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = TTI_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call function with enable [GT_TRUE / GT_FALSE]
                Expected: GT_OK.
            */
            /* iterate with enable = GT_FALSE */
            enable = GT_TRUE;

            st = cpssDxChTtiPortIpv4OnlyMacToMeEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call function cpssDxChTtiPortIpv4OnlyMacToMeEnableGet
                                     with not NULL enablePtr.
                Expected: GT_OK and enable the same as was set.
            */
            st = cpssDxChTtiPortIpv4OnlyMacToMeEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChTtiPortIpv4OnlyMacToMeEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "got another enable then was set: %d, %d", dev, port);

            /* iterate with enable = GT_FALSE */
            enable = GT_FALSE;

            st = cpssDxChTtiPortIpv4OnlyMacToMeEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call function cpssDxChTtiPortIpv4OnlyMacToMeEnableGet
                                     with not NULL enablePtr.
                Expected: GT_OK and enableGet the same as was set.
            */
            st = cpssDxChTtiPortIpv4OnlyMacToMeEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChTtiPortIpv4OnlyMacToMeEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "got another enable then was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChTtiPortIpv4OnlyMacToMeEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiPortIpv4OnlyMacToMeEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChTtiPortIpv4OnlyMacToMeEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);


    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TTI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortIpv4OnlyMacToMeEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortIpv4OnlyMacToMeEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortIpv4OnlyMacToMeEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    OUT GT_BOOL                             *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortIpv4OnlyMacToMeEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1.1. Call function with not NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = TTI_VALID_PHY_PORT_CNS;


    GT_BOOL     enable = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call function with not NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssDxChTtiPortIpv4OnlyMacToMeEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call function with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortIpv4OnlyMacToMeEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChTtiPortIpv4OnlyMacToMeEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiPortIpv4OnlyMacToMeEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChTtiPortIpv4OnlyMacToMeEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);


    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TTI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortIpv4OnlyMacToMeEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortIpv4OnlyMacToMeEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiIpv4McEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiIpv4McEnableSet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1. Call function with enable [GT_TRUE / GT_FALSE]
    Expected: GT_OK.
    1.2. Call function cpssDxChTtiIpv4McEnableGet with not NULL enablePtr.
    Expected: GT_OK and enable the same as was set.
*/

    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */
        /* iterate with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxChTtiIpv4McEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function cpssDxChTtiIpv4McEnableGet with not NULL enablePtr.
            Expected: GT_OK and enable the same as was set.
        */
        st = cpssDxChTtiIpv4McEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTtiIpv4McEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "got another enable then was set: %d", dev);

        /* iterate with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssDxChTtiIpv4McEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function cpssDxChTtiIpv4McEnableGet with not NULL enablePtr.
            Expected: GT_OK and enable the same as was set.
        */
        st = cpssDxChTtiIpv4McEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTtiIpv4McEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "got another enable then was set: %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiIpv4McEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiIpv4McEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiIpv4McEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiIpv4McEnableGet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1. Call function with not NULL enablePtr.
    Expected: GT_OK.
    1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR
*/

    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with not NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChTtiIpv4McEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with enablePtr [NULL].
            Expected: GT_BAD_PTR
        */
        st = cpssDxChTtiIpv4McEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiIpv4McEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiIpv4McEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortMplsOnlyMacToMeEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    IN  GT_BOOL                             enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortMplsOnlyMacToMeEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call function cpssDxChTtiPortMplsOnlyMacToMeEnableGet
                         with not NULL enablePtr.
    Expected: GT_OK and enable the same as was set.
*/

    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = TTI_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call function with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */
            /* iterate with enable = GT_TRUE */
            enable = GT_TRUE;

            st = cpssDxChTtiPortMplsOnlyMacToMeEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call function cpssDxChTtiPortMplsOnlyMacToMeEnableGet
                                     with not NULL enablePtr.
                Expected: GT_OK and enable the same as was set.
            */
            st = cpssDxChTtiPortMplsOnlyMacToMeEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChTtiPortMplsOnlyMacToMeEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "got another enable then was set: %d, %d", dev, port);

            /* iterate with enable = GT_FALSE */
            enable = GT_FALSE;

            st = cpssDxChTtiPortMplsOnlyMacToMeEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call function cpssDxChTtiPortMplsOnlyMacToMeEnableGet
                                     with not NULL enablePtr.
                Expected: GT_OK and enable the same as was set.
            */
            st = cpssDxChTtiPortMplsOnlyMacToMeEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChTtiPortMplsOnlyMacToMeEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "got another enable then was set: %d, %d", dev, port);

        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChTtiPortMplsOnlyMacToMeEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiPortMplsOnlyMacToMeEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChTtiPortMplsOnlyMacToMeEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);


    }

    enable = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TTI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortMplsOnlyMacToMeEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortMplsOnlyMacToMeEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortMplsOnlyMacToMeEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               port,
    OUT GT_BOOL                             *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortMplsOnlyMacToMeEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1.1. Call function with not NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = TTI_VALID_PHY_PORT_CNS;

    GT_BOOL     enable = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call function with not NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssDxChTtiPortMplsOnlyMacToMeEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call function with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortMplsOnlyMacToMeEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChTtiPortMplsOnlyMacToMeEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiPortMplsOnlyMacToMeEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChTtiPortMplsOnlyMacToMeEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);


    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TTI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortMplsOnlyMacToMeEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortMplsOnlyMacToMeEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              ruleIndex,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiRuleSet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: DxCh3)
    1.1. Call with ruleIndex [0],
                   keyType [CPSS_DXCH_TTI_KEY_IPV4_E],
                   patternPtr->ipv4{common{pclId[1],
                                           srcIsTrunk[GT_FALSE],
                                           srcPortTrunk[23],
                                           mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                           vid[100],
                                           isTagged[GT_TRUE],
                                           dsaSrcIsTrunk[GT_FALSE],
                                           dsaSrcPortTrunk[0],
                                           dsaSrcDevice[dev] },
                                    tunneltype [IPv4_OVER_IPv4],
                                    srcIp[192.168.0.1],
                                    destIp[192.168.0.2],
                                    isArp[GT_FALSE] },
                   maskPtr->ipv4 {0xFF,...},
                   actionPtr{tunnelTerminate[GT_TRUE],
                             passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
                             copyTtlFromTunnelHeader[GT_FALSE],
                             command [CPSS_PACKET_CMD_FORWARD_E],
                             redirectCommand[CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E],
                             egressInterface{type[CPSS_INTERFACE_TRUNK_E],
                                             trunkId[2]},

                             tunnelStart [GT_TRUE],
                             tunnelStartPtr[0],
                             targetIsTrunk[GT_FALSE],
                             useVidx[GT_FALSE],

                             sourceIdSetEnable[GT_TRUE],
                             sourceId[0],

                             vlanCmd[CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E],
                             vlanId[100],
                             vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                             nestedVlanEnable[GT_FALSE],

                             bindToPolicer[GT_FALSE],

                             qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                             qosTrustMode[CPSS_DXCH_TTI_QOS_UNTRUST_E],
                             qosProfile[0],
                             modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E],
                             modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
                             up[0],

                             mirrorToIngressAnalyzerEnable[GT_FALSE],
                             vntl2Echo[GT_FALSE],
                             bridgeBypass[GT_FALSE],
                             actionStop[GT_FALSE] }
    Expected: GT_OK.
    1.2. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                      other params same as in 1.1.
    Expected: GT_OK and same values as written
    1.3. Call with ruleIndex [1],
           keyType [CPSS_DXCH_TTI_KEY_MPLS_E],
                   patternPtr->mpls{common{pclId[2],
                                           srcIsTrunk[GT_TRUE],
                                           srcPortTrunk[127],
                                           mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                           vid[100],
                                           isTagged[GT_TRUE],
                                           dsaSrcIsTrunk[GT_FALSE],
                                           dsaSrcPortTrunk[0],
                                           dsaSrcDevice[dev]},
                            label0 [0],
                            exp0 [3],
                            label1 [0xFF],
                            exp1[5],
                            label2 [0xFFF],
                            exp2 [7],
                            numOfLabels [3]
                            protocolAboveMPLS [0]},
    Expected: GT_OK.
    1.4. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                      other params same as in 1.2.
    Expected: GT_OK and same values as written
    1.5. Call with ruleIndex [2],
                   keyType [CPSS_DXCH_TTI_KEY_ETH_E],
                   patternPtr->mpls{common{pclId[3],
                                           srcIsTrunk[GT_TRUE],
                                           srcPortTrunk[127],
                                           mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                           vid[100],
                                           isTagged[GT_TRUE],
                                           dsaSrcIsTrunk[GT_FALSE],
                                           dsaSrcPortTrunk[0],
                                           dsaSrcDevice[dev]},
                                    up0 [0],
                                    cfi0 [1],
                                    isVlan1Exists [GT_TRUE],
                                    vid1 [100],
                                    up1 [3],
                                    cfi1 [1],
                                    etherType [0],
                                    macToMe [GT_FALSE]}
    Expected: GT_OK.
    1.6. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                      other params same as in 1.3.
    Expected: GT_OK and same values as written
    1.7. Call with out of range patternPtr->ipv4.common.pclId [3] (for IPV4 must be 1)
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with patternPtr->ipv4.common.srcIsTrunk [GT_FALSE]
                   out of range srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with patternPtr->ipv4.common.srcIsTrunk [GT_TRUE]
                   out of range srcPortTrunk [128],
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.10. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS],
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.11. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                    patternPtr->ipv4.common.dsaSrcIsTrunk [GT_FALSE]
                   out of range dsaSrcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.12. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                    patternPtr->ipv4.common.dsaSrcIsTrunk [GT_TRUE]
                    out of range dsaSrcPortTrunk [128],
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.13. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                    out of range dsaSrcDevice [PRV_CPSS_MAX_PP_DEVICES_CNS],
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.14. Call with wrong enum values patternPtr->ipv4.tunneltype
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.15. Call with out of range patternPtr->mpls.common.pclId [3] (for MPLSmust be 2)
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.16. Call with patternPtr->mpls.common.srcIsTrunk [GT_FALSE]
                   out of range srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.17. Call with patternPtr->mpls.common.srcIsTrunk [GT_TRUE]
                   out of range srcPortTrunk [128],
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.18. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS],
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.19. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                    patternPtr->mpls.common.dsaSrcIsTrunk [GT_FALSE]
                   out of range dsaSrcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.20. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                    patternPtr->mpls.common.dsaSrcIsTrunk [GT_TRUE]
                    out of range dsaSrcPortTrunk [128],
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.21. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                    out of range dsaSrcDevice [PRV_CPSS_MAX_PP_DEVICES_CNS],
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.22. Call with wrong enum values actionPtr->passengerPacketType
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.23. Call with wrong enum values actionPtr->command
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.24. Call with wrong enum values actionPtr->redirectCommand
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.25. Call with wrong enum values actionPtr->vlanCmd
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.26. Call with wrong enum values actionPtr->qosPrecedence
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.27. Call with wrong enum values actionPtr->qosTrustMode
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.28. Call with wrong enum values actionPtr->userDefinedCpuCode
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.29. Call with wrong enum values keyType and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.30. Call with wrong enum values actionType and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.31. Call with out of range ruleIndex [maxRuleIndex] and other params from 1.1.
    Expected: NOT GT_OK.
    1.32. Call function with patternPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.33. Call function with maskPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.34. Call function with actionPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      ruleIndex = 0;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_RULE_UNT      pattern;
    CPSS_DXCH_TTI_RULE_UNT      mask;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    CPSS_DXCH_TTI_RULE_UNT      patternGet;
    CPSS_DXCH_TTI_RULE_UNT      maskGet;
    CPSS_DXCH_TTI_ACTION_UNT    actionGet;
    GT_BOOL                     isEqual = GT_FALSE;


    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));

    cpssOsBzero((GT_VOID*) &patternGet, sizeof(patternGet));
    cpssOsBzero((GT_VOID*) &maskGet, sizeof(maskGet));
    cpssOsBzero((GT_VOID*) &actionGet, sizeof(actionGet));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ruleIndex [0],
                           keyType [CPSS_DXCH_TTI_KEY_IPV4_E],
                           patternPtr->ipv4{common{pclId[1],
                                                   srcIsTrunk[GT_FALSE],
                                                   srcPortTrunk[23],
                                                   mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                                   vid[100],
                                                   isTagged[GT_TRUE],
                                                   dsaSrcIsTrunk[GT_FALSE],
                                                   dsaSrcPortTrunk[0],
                                                   dsaSrcDevice[dev] },
                                            tunneltype [IPv4_OVER_IPv4],
                                            srcIp[192.168.0.1],
                                            destIp[192.168.0.2],
                                            isArp[GT_FALSE] },
                           maskPtr->ipv4 {0xFF,...},
                           actionPtr{tunnelTerminate[GT_TRUE],
                                     passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
                                     copyTtlFromTunnelHeader[GT_FALSE],
                                     command [CPSS_PACKET_CMD_FORWARD_E],
                                     redirectCommand[CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E],
                                     egressInterface{type[CPSS_INTERFACE_TRUNK_E],
                                                     trunkId[2]},

                                     tunnelStart [GT_TRUE],
                                     tunnelStartPtr[0],
                                     targetIsTrunk[GT_FALSE],
                                     useVidx[GT_FALSE],

                                     sourceIdSetEnable[GT_TRUE],
                                     sourceId[0],

                                     vlanCmd[CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E],
                                     vlanId[100],
                                     vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                     nestedVlanEnable[GT_FALSE],

                                     bindToPolicer[GT_FALSE],

                                     qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                     qosTrustMode[CPSS_DXCH_TTI_QOS_UNTRUST_E],
                                     qosProfile[0],
                                     modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E],
                                     modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
                                     up[0],

                                     mirrorToIngressAnalyzerEnable[GT_FALSE],
                                     vntl2Echo[GT_FALSE],
                                     bridgeBypass[GT_FALSE],
                                     actionStop[GT_FALSE] }
            Expected: GT_OK.
        */
        ruleIndex = 0;
        keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

        prvSetDefaultIpv4Pattern(&pattern);
        prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE1_ENT);
        prvSetDefaultIpv4Mask(&mask);

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, keyType);

        /*
            1.2. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                              other params same as in 1.1.
            Expected: GT_OK and same values as written
        */
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &patternGet, &maskGet, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &actionGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssDxChTtiRuleGet: %d, %d, %d", dev, keyType, ruleIndex );

        /* validation values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.ipv4,
                                     (GT_VOID*) &patternGet.ipv4,
                                     sizeof(pattern.ipv4))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another patternPtr->ipv4 then was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.ipv4,
                                     (GT_VOID*) &maskGet.ipv4,
                                     sizeof(mask.ipv4))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another maskPtr->ipv4 then was set: %d", dev);
        /*
            1.3. Call with ruleIndex [1],
                   keyType [CPSS_DXCH_TTI_KEY_MPLS_E],
                           patternPtr->mpls{common{pclId[2],
                                                   srcIsTrunk[GT_TRUE],
                                                   srcPortTrunk[127],
                                                   mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                                   vid[100],
                                                   isTagged[GT_TRUE],
                                                   dsaSrcIsTrunk[GT_FALSE],
                                                   dsaSrcPortTrunk[0],
                                                   dsaSrcDevice[dev]},
                                    label0 [0],
                                    exp0 [3],
                                    label1 [0xFF],
                                    exp1[5],
                                    label2 [0xFFF],
                                    exp2 [7],
                                    numOfLabels [3]
                                    protocolAboveMPLS [0]},
            Expected: GT_OK.
        */
        ruleIndex = 2;
        keyType   = CPSS_DXCH_TTI_KEY_MPLS_E;

        prvSetDefaultMplsPattern(&pattern);
        prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE1_ENT);
        prvSetDefaultMplsMask(&mask);

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, keyType);
        /*
            1.4. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                              other params same as in 1.2.
            Expected: GT_OK and same values as written
        */
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &patternGet, &maskGet, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &actionGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                     "cpssDxChTtiRuleGet: %d, %d, %d", dev, keyType, ruleIndex );
        /* validation values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.mpls,
                                     (GT_VOID*) &patternGet.mpls,
                                     sizeof(pattern.mpls))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another patternPtr->mpls then was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.mpls,
                                     (GT_VOID*) &maskGet.mpls,
                                     sizeof(mask.mpls))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another maskPtr->mpls then was set: %d", dev);
        /*
            1.5. Call with ruleIndex [2],
                           keyType [CPSS_DXCH_TTI_KEY_ETH_E],
                           patternPtr->mpls{common{pclId[3],
                                                   srcIsTrunk[GT_TRUE],
                                                   srcPortTrunk[127],
                                                   mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                                   vid[100],
                                                   isTagged[GT_TRUE],
                                                   dsaSrcIsTrunk[GT_FALSE],
                                                   dsaSrcPortTrunk[0],
                                                   dsaSrcDevice[dev]},
                                            up0 [0],
                                            cfi0 [1],
                                            isVlan1Exists [GT_TRUE],
                                            vid1 [100],
                                            up1 [3],
                                            cfi1 [1],
                                            etherType [0],
                                            macToMe [GT_FALSE]}
            Expected: GT_OK.
        */
        ruleIndex = 2;
        keyType   = CPSS_DXCH_TTI_KEY_ETH_E;

        prvSetDefaultEthPattern(&pattern);
        prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE1_ENT);
        prvSetDefaultEthMask(&mask);

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, keyType);

        /*
            1.6. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                              other params same as in 1.3.
            Expected: GT_OK and same values as written
        */
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &patternGet, &maskGet, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &actionGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                     "cpssDxChTtiRuleGet: %d, %d, %d", dev, keyType, ruleIndex);
        /* validation values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.eth,
                                     (GT_VOID*) &patternGet.eth,
                                     sizeof(pattern.eth))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another patternPtr->eth then was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.eth,
                                     (GT_VOID*) &maskGet.eth,
                                     sizeof(mask.eth))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another maskPtr->eth then was set: %d", dev);

        prvSetDefaultIpv4Pattern(&pattern);

        /*
            1.7. Call with out of range patternPtr->ipv4.common.pclId [3] (for IPV4 must be < BIT_10)
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        keyType = CPSS_DXCH_TTI_KEY_IPV4_E;

        pattern.ipv4.common.pclId = 0x400;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, patternPtr->ipv4.common.pclId = %d",
                                         dev, pattern.ipv4.common.pclId);

        pattern.ipv4.common.pclId = 1;

        /*
            1.8. Call with patternPtr->ipv4.common.srcIsTrunk [GT_FALSE]
                           out of range srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.ipv4.common.srcIsTrunk   = GT_FALSE;
        pattern.ipv4.common.srcPortTrunk = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.srcIsTrunk = %d, ->common.srcPortTrunk = %d",
                                     dev, pattern.ipv4.common.srcIsTrunk, pattern.ipv4.common.srcPortTrunk);

        pattern.ipv4.common.srcPortTrunk = 0;

        /*
            1.9. Call with patternPtr->ipv4.common.srcIsTrunk [GT_TRUE]
                           out of range srcPortTrunk [128],
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.ipv4.common.srcIsTrunk   = GT_TRUE;
        pattern.ipv4.common.srcPortTrunk = 128;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->common.srcIsTrunk = %d, ->common.srcPortTrunk = %d",
                                         dev, pattern.ipv4.common.srcIsTrunk, pattern.ipv4.common.srcPortTrunk);

        pattern.ipv4.common.srcPortTrunk = 2;

        /*
            1.10. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS],
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.ipv4.common.vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.vid = %d",
                                     dev, pattern.ipv4.common.vid);

        pattern.ipv4.common.vid = 100;

        /*
            1.11. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                            patternPtr->ipv4.common.dsaSrcIsTrunk [GT_FALSE]
                           out of range dsaSrcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.ipv4.common.isTagged        = GT_TRUE;
        pattern.ipv4.common.dsaSrcIsTrunk   = GT_FALSE;
        pattern.ipv4.common.dsaSrcPortTrunk = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.dsaSrcIsTrunk = %d, ->common.dsaSrcPortTrunk = %d",
                                     dev, pattern.ipv4.common.dsaSrcIsTrunk, pattern.ipv4.common.dsaSrcPortTrunk);

        pattern.ipv4.common.dsaSrcPortTrunk = 0;

        /*
            1.12. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                            patternPtr->ipv4.common.dsaSrcIsTrunk [GT_TRUE]
                            out of range dsaSrcPortTrunk [128],
                            and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.ipv4.common.dsaSrcIsTrunk   = GT_TRUE;
        pattern.ipv4.common.dsaSrcPortTrunk = 128;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->common.dsaSrcIsTrunk = %d, ->common.dsaSrcPortTrunk = %d",
                                         dev, pattern.ipv4.common.dsaSrcIsTrunk, pattern.ipv4.common.dsaSrcPortTrunk);

        pattern.ipv4.common.dsaSrcPortTrunk = 2;

        /*
            1.13. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                            out of range dsaSrcDevice [PRV_CPSS_MAX_PP_DEVICES_CNS],
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.ipv4.common.dsaSrcDevice = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.dsaSrcDevice = %d",
                                     dev, pattern.ipv4.common.dsaSrcDevice);

        pattern.ipv4.common.dsaSrcDevice = dev;

        /*
            1.14. Call with wrong enum values patternPtr->ipv4.tunneltype
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            pattern.ipv4.tunneltype);

        /*
            1.15. Call with out of range patternPtr->mpls.common.pclId [3] (for MPLSmust be 2)
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        keyType = CPSS_DXCH_TTI_KEY_ETH_E;

        pattern.mpls.common.pclId = 3;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, patternPtr->mpls.common.pclId = %d",
                                         dev, pattern.mpls.common.pclId);

        pattern.mpls.common.pclId = 2;

        /*
            1.16. Call with patternPtr->mpls.common.srcIsTrunk [GT_FALSE]
                           out of range srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.mpls.common.srcIsTrunk   = GT_FALSE;
        pattern.mpls.common.srcPortTrunk = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.srcIsTrunk = %d, ->common.srcPortTrunk = %d",
                                     dev, pattern.mpls.common.srcIsTrunk, pattern.mpls.common.srcPortTrunk);

        pattern.mpls.common.srcPortTrunk = 0;

        /*
            1.17. Call with patternPtr->mpls.common.srcIsTrunk [GT_TRUE]
                           out of range srcPortTrunk [128],
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.mpls.common.srcIsTrunk   = GT_TRUE;
        pattern.mpls.common.srcPortTrunk = 128;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->common.srcIsTrunk = %d, ->common.srcPortTrunk = %d",
                                         dev, pattern.mpls.common.srcIsTrunk, pattern.mpls.common.srcPortTrunk);

        pattern.mpls.common.srcPortTrunk = 2;

        /*
            1.18. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS],
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.mpls.common.vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.vid = %d",
                                     dev, pattern.mpls.common.vid);

        pattern.mpls.common.vid = 100;

        /*
            1.19. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                            patternPtr->mpls.common.dsaSrcIsTrunk [GT_FALSE]
                           out of range dsaSrcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.mpls.common.isTagged        = GT_TRUE;
        pattern.mpls.common.dsaSrcIsTrunk   = GT_FALSE;
        pattern.mpls.common.dsaSrcPortTrunk = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.dsaSrcIsTrunk = %d, ->common.dsaSrcPortTrunk = %d",
                                     dev, pattern.mpls.common.dsaSrcIsTrunk, pattern.mpls.common.dsaSrcPortTrunk);

        pattern.mpls.common.dsaSrcPortTrunk = 0;

        /*
            1.20. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                            patternPtr->mpls.common.dsaSrcIsTrunk [GT_TRUE]
                            out of range dsaSrcPortTrunk [128],
                            and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.mpls.common.dsaSrcIsTrunk   = GT_TRUE;
        pattern.mpls.common.dsaSrcPortTrunk = 128;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->common.dsaSrcIsTrunk = %d, ->common.dsaSrcPortTrunk = %d",
                                         dev, pattern.mpls.common.dsaSrcIsTrunk, pattern.mpls.common.dsaSrcPortTrunk);

        pattern.mpls.common.dsaSrcPortTrunk = 2;

        /*
            1.21. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                            out of range dsaSrcDevice [PRV_CPSS_MAX_PP_DEVICES_CNS],
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.mpls.common.dsaSrcDevice = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.dsaSrcDevice = %d",
                                     dev, pattern.mpls.common.dsaSrcDevice);

        pattern.mpls.common.dsaSrcDevice = dev;

        /*
            1.22. Call with wrong enum values actionPtr->passengerPacketType
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            action.type1.passengerPacketType);

        /*
            1.23. Call with wrong enum values actionPtr->command and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            action.type1.command);

        /*
            1.24. Call with wrong enum values actionPtr->redirectCommand and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            action.type1.redirectCommand);

        /*
            1.25. Call with wrong enum values actionPtr->vlanCmd and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            action.type1.vlanCmd);

        /*
            1.26. Call with wrong enum values actionPtr->qosPrecedence and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            action.type1.qosPrecedence);

        /*
            1.27. Call with wrong enum values actionPtr->qosTrustMode
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            action.type1.qosTrustMode);

        /*
            1.28. Call with wrong enum values actionPtr->userDefinedCpuCode
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            action.type1.userDefinedCpuCode);

        /*
            1.29. Call with wrong enum values keyType and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            keyType);
        /*
            1.30. Call with wrong enum values keyType and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            actionType);

        /*
            1.31. Call with out of range ruleIndex [maxRuleIndex]
                            and other params from 1.1.
            Expected: NOT GT_OK.
        */
        ruleIndex = prvGetMaxRuleIndex(dev);

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, actionType, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleIndex = %d", dev, ruleIndex);

        ruleIndex = 0;

        /*
            1.32. Call function with patternPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, NULL, &mask, actionType, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, patternPtr = NULL", dev);

        /*
            1.33. Call function with maskPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, NULL, actionType, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);

        /*
            1.34. Call function with actionPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, actionType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);
    }

    ruleIndex = 0;
    keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

    prvSetDefaultIpv4Pattern(&pattern);
    prvSetDefaultIpv4Mask(&mask);

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        prvSetDefaultAction(&action,actionType);
        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, actionType, &action);

        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        }
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, actionType, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              ruleIndex,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiRuleSet_type2)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion; xCat2)
    1.1. Call with ruleIndex [0],
                   keyType [CPSS_DXCH_TTI_KEY_IPV4_E],
                   patternPtr->ipv4{common{pclId[1],
                                           srcIsTrunk[GT_FALSE],
                                           srcPortTrunk[23],
                                           mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                           vid[100],
                                           isTagged[GT_TRUE],
                                           dsaSrcIsTrunk[GT_FALSE],
                                           dsaSrcPortTrunk[0],
                                           dsaSrcDevice[dev] },
                                    tunneltype [IPv4_OVER_IPv4],
                                    srcIp[192.168.0.1],
                                    destIp[192.168.0.2],
                                    isArp[GT_FALSE] },
                   maskPtr->ipv4 {0xFF,...},
                   actionPtr{
                        tunnelTerminate        [GT_TRUE]
                        ttPassengerPacketType  [CPSS_DXCH_TTI_PASSENGER_IPV4V6_E]
                        tsPassengerPacketType  [CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E]
                        copyTtlFromTunnelHeader[GT_FALSE]
                        mplsCommand            [CPSS_DXCH_TTI_MPLS_NOP_CMD_E]
                        mplsTtl                [0]
                        enableDecrementTtl     [ GT_FALSE]
                        command                [CPSS_PACKET_CMD_MIRROR_TO_CPU_E]
                        redirectCommand        [CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E]
                        egressInterface.type   [CPSS_INTERFACE_TRUNK_E]
                        egressInterface.trunkId[2]
                        arpPtr                 [0]
                        tunnelStart            [GT_TRUE]
                        tunnelStartPtr         [0]
                        routerLttPtr           [0]
                        vrfId                  [0]
                        sourceIdSetEnable      [GT_TRUE]
                        sourceId               [0]
                        tag0VlanCmd            [CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E]
                        tag0VlanId             [100]
                        tag1VlanCmd            [CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E]
                        tag1VlanId             [100]
                        tag0VlanPrecedence     [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E]
                        nestedVlanEnable       [GT_FALSE]
                        bindToPolicerMeter     [GT_FALSE]
                        bindToPolicer          [GT_FALSE]
                        policerIndex           [0]
                        qosPrecedence          [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E]
                        keepPreviousQoS        [GT_FALSE]
                        trustUp                [0]
                        trustDscp              [0]
                        trustExp               [0]
                        qosProfile             [0]
                        modifyTag0Up           [GT_FALSE]
                        tag1UpCommand          [CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E]
                        modifyDscp             [GT_FALSE]
                        tag0Up                 [0]
                        tag1Up                 [0]
                        remapDSCP              [GT_FALSE]
                        pcl0OverrideConfigIndex[CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E]
                        pcl0_1OverrideConfigIndex[CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E]
                        pcl1OverrideConfigIndex [CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E]
                        iPclConfigIndex        [0]

                        mirrorToIngressAnalyzerEnable[GT_FALSE]
                        userDefinedCpuCode           [CPSS_NET_FIRST_USER_DEFINED_E]
                        bindToCentralCounter         [GT_FALSE]
                        centralCounterIndex          [0]
                        vntl2Echo                    [GT_FALSE]
                        bridgeBypass                 [GT_FALSE]
                        ingressPipeBypass            [GT_FALSE]
                        actionStop                   [GT_FALSE] }
    Expected: GT_OK.
    1.2. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                      other params same as in 1.1.
    Expected: GT_OK and same values as written
    1.3. Call with ruleIndex [1],
           keyType [CPSS_DXCH_TTI_KEY_MPLS_E],
                   patternPtr->mpls{common{pclId[2],
                                           srcIsTrunk[GT_TRUE],
                                           srcPortTrunk[127],
                                           mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                           vid[100],
                                           isTagged[GT_TRUE],
                                           dsaSrcIsTrunk[GT_FALSE],
                                           dsaSrcPortTrunk[0],
                                           dsaSrcDevice[dev]},
                            label0 [0],
                            exp0 [3],
                            label1 [0xFF],
                            exp1[5],
                            label2 [0xFFF],
                            exp2 [7],
                            numOfLabels [3]
                            protocolAboveMPLS [0]},
    Expected: GT_OK.
    1.4. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                      other params same as in 1.2.
    Expected: GT_OK and same values as written
    1.5. Call with ruleIndex [2],
                   keyType [CPSS_DXCH_TTI_KEY_ETH_E],
                   patternPtr->mpls{common{pclId[3],
                                           srcIsTrunk[GT_TRUE],
                                           srcPortTrunk[127],
                                           mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                           vid[100],
                                           isTagged[GT_TRUE],
                                           dsaSrcIsTrunk[GT_FALSE],
                                           dsaSrcPortTrunk[0],
                                           dsaSrcDevice[dev]},
                                    up0 [0],
                                    cfi0 [1],
                                    isVlan1Exists [GT_TRUE],
                                    vid1 [100],
                                    up1 [3],
                                    cfi1 [1],
                                    etherType [0],
                                    macToMe [GT_FALSE]}
    Expected: GT_OK.
    1.6. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                      other params same as in 1.3.
    Expected: GT_OK and same values as written
    1.7. Call with out of range patternPtr->ipv4.common.pclId [3] (for IPV4 must be 1)
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with patternPtr->ipv4.common.srcIsTrunk [GT_FALSE]
                   out of range srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with patternPtr->ipv4.common.srcIsTrunk [GT_TRUE]
                   out of range srcPortTrunk [128],
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.10. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS],
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.11. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                    patternPtr->ipv4.common.dsaSrcIsTrunk [GT_FALSE]
                   out of range dsaSrcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.12. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                    patternPtr->ipv4.common.dsaSrcIsTrunk [GT_TRUE]
                    out of range dsaSrcPortTrunk [128],
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.13. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                    out of range dsaSrcDevice [PRV_CPSS_MAX_PP_DEVICES_CNS],
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.14. Call with wrong enum values patternPtr->ipv4.tunneltype
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.15. Call with out of range patternPtr->mpls.common.pclId [3] (for MPLSmust be 2)
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.16. Call with patternPtr->mpls.common.srcIsTrunk [GT_FALSE]
                   out of range srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.17. Call with patternPtr->mpls.common.srcIsTrunk [GT_TRUE]
                   out of range srcPortTrunk [128],
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.18. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS],
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.19. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                    patternPtr->mpls.common.dsaSrcIsTrunk [GT_FALSE]
                   out of range dsaSrcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.20. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                    patternPtr->mpls.common.dsaSrcIsTrunk [GT_TRUE]
                    out of range dsaSrcPortTrunk [128],
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.21. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                    out of range dsaSrcDevice [PRV_CPSS_MAX_PP_DEVICES_CNS],
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.22. Call with wrong enum values actionPtr->passengerPacketType
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.23. Call with wrong enum values actionPtr->command
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.24. Call with wrong enum values actionPtr->redirectCommand
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.25. Call with wrong enum values actionPtr->tag2VlanCmd
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.26 Call with wrong enum values actionPtr->tag1VlanCmd
                            and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.27. Call with wrong enum values actionPtr->qosPrecedence
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.28. Call with wrong enum values actionPtr->userDefinedCpuCode
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.29. Call with wrong enum values keyType and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.30. Call with wrong enum values actionType and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.31. Call with out of range ruleIndex [maxRuleIndex] and other params from 1.1.
    Expected: NOT GT_OK.
    1.32. Call function with patternPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.33. Call function with maskPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.34. Call function with actionPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      ruleIndex = 0;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_RULE_UNT      pattern;
    CPSS_DXCH_TTI_RULE_UNT      mask;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    CPSS_DXCH_TTI_RULE_UNT      patternGet;
    CPSS_DXCH_TTI_RULE_UNT      maskGet;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT  actionType = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;
    CPSS_DXCH_TTI_ACTION_UNT    actionGet;
    GT_BOOL                     isEqual = GT_FALSE;


    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));

    cpssOsBzero((GT_VOID*) &patternGet, sizeof(patternGet));
    cpssOsBzero((GT_VOID*) &maskGet, sizeof(maskGet));
    cpssOsBzero((GT_VOID*) &actionGet, sizeof(actionGet));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ruleIndex [0],
                           keyType [CPSS_DXCH_TTI_KEY_IPV4_E],
                           patternPtr->ipv4{common{pclId[1],
                                                   srcIsTrunk[GT_FALSE],
                                                   srcPortTrunk[23],
                                                   mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                                   vid[100],
                                                   isTagged[GT_TRUE],
                                                   dsaSrcIsTrunk[GT_FALSE],
                                                   dsaSrcPortTrunk[0],
                                                   dsaSrcDevice[dev] },
                                            tunneltype [IPv4_OVER_IPv4],
                                            srcIp[192.168.0.1],
                                            destIp[192.168.0.2],
                                            isArp[GT_FALSE] },
                           maskPtr->ipv4 {0xFF,...},
                           actionPtr{tunnelTerminate        [GT_TRUE]
                                ttPassengerPacketType  [CPSS_DXCH_TTI_PASSENGER_IPV4V6_E]
                                tsPassengerPacketType  [CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E]
                                copyTtlFromTunnelHeader[GT_FALSE]
                                mplsCommand            [CPSS_DXCH_TTI_MPLS_NOP_CMD_E]
                                mplsTtl                [0]
                                enableDecrementTtl     [ GT_FALSE]
                                command                [CPSS_PACKET_CMD_MIRROR_TO_CPU_E]
                                redirectCommand        [CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E]
                                egressInterface.type   [CPSS_INTERFACE_TRUNK_E]
                                egressInterface.trunkId[2]
                                arpPtr                 [0]
                                tunnelStart            [GT_TRUE]
                                tunnelStartPtr         [0]
                                routerLttPtr           [0]
                                vrfId                  [0]
                                sourceIdSetEnable      [GT_TRUE]
                                sourceId               [0]
                                tag0VlanCmd            [CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E]
                                tag0VlanId             [100]
                                tag1VlanCmd            [CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E]
                                tag1VlanId             [100]
                                tag0VlanPrecedence     [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E]
                                nestedVlanEnable       [GT_FALSE]
                                bindToPolicerMeter     [GT_FALSE]
                                bindToPolicer          [GT_FALSE]
                                policerIndex           [0]
                                qosPrecedence          [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E]
                                keepPreviousQoS        [GT_FALSE]
                                trustUp                [0]
                                trustDscp              [0]
                                trustExp               [0]
                                qosProfile             [0]
                                modifyTag0Up           [GT_FALSE]
                                tag1UpCommand          [CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E]
                                modifyDscp             [GT_FALSE]
                                tag0Up                 [0]
                                tag1Up                 [0]
                                remapDSCP              [GT_FALSE]
                                pcl0OverrideConfigIndex[CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E]
                                pcl0_1OverrideConfigIndex[CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E]
                                pcl1OverrideConfigIndex [CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E]
                                iPclConfigIndex        [0]

                                mirrorToIngressAnalyzerEnable[GT_FALSE]
                                userDefinedCpuCode           [CPSS_NET_FIRST_USER_DEFINED_E]
                                bindToCentralCounter         [GT_FALSE]
                                centralCounterIndex          [0]
                                vntl2Echo                    [GT_FALSE]
                                bridgeBypass                 [GT_FALSE]
                                ingressPipeBypass            [GT_FALSE]
                                actionStop                   [GT_FALSE]}
           Expected: GT_OK.
        */
        ruleIndex = 0;
        keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

        prvSetDefaultIpv4Pattern(&pattern);
        prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE2_ENT);

        prvSetDefaultIpv4Mask(&mask);

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, keyType);

        /*
            1.2. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                              other params same as in 1.1.
            Expected: GT_OK and same values as written
        */
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &patternGet, &maskGet, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &actionGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssDxChTtiRuleGet: %d, %d, %d", dev, keyType, ruleIndex );

        /* validation values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.ipv4,
                                     (GT_VOID*) &patternGet.ipv4,
                                     sizeof(pattern.ipv4))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another patternPtr->ipv4 then was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.ipv4,
                                     (GT_VOID*) &maskGet.ipv4,
                                     sizeof(mask.ipv4))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another maskPtr->ipv4 then was set: %d", dev);
        /*
            1.3. Call with ruleIndex [1],
                   keyType [CPSS_DXCH_TTI_KEY_MPLS_E],
                           patternPtr->mpls{common{pclId[2],
                                                   srcIsTrunk[GT_TRUE],
                                                   srcPortTrunk[127],
                                                   mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                                   vid[100],
                                                   isTagged[GT_TRUE],
                                                   dsaSrcIsTrunk[GT_FALSE],
                                                   dsaSrcPortTrunk[0],
                                                   dsaSrcDevice[dev]},
                                    label0 [0],
                                    exp0 [3],
                                    label1 [0xFF],
                                    exp1[5],
                                    label2 [0xFFF],
                                    exp2 [7],
                                    numOfLabels [3]
                                    protocolAboveMPLS [0]},
            Expected: GT_OK.
        */
        ruleIndex = 2;
        keyType   = CPSS_DXCH_TTI_KEY_MPLS_E;

        prvSetDefaultMplsPattern(&pattern);
        prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE2_ENT);
        prvSetDefaultMplsMask(&mask);

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, keyType);

        /*
            1.4. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                              other params same as in 1.2.
            Expected: GT_OK and same values as written
        */
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &patternGet, &maskGet, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &actionGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                     "cpssDxChTtiRuleGet: %d, %d, %d", dev, keyType, ruleIndex );
        /* validation values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.mpls,
                                     (GT_VOID*) &patternGet.mpls,
                                     sizeof(pattern.mpls))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another patternPtr->mpls then was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.mpls,
                                     (GT_VOID*) &maskGet.mpls,
                                     sizeof(mask.mpls))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another maskPtr->mpls then was set: %d", dev);

        /*
            1.5. Call with ruleIndex [2],
                           keyType [CPSS_DXCH_TTI_KEY_ETH_E],
                           patternPtr->mpls{common{pclId[3],
                                                   srcIsTrunk[GT_TRUE],
                                                   srcPortTrunk[127],
                                                   mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                                   vid[100],
                                                   isTagged[GT_TRUE],
                                                   dsaSrcIsTrunk[GT_FALSE],
                                                   dsaSrcPortTrunk[0],
                                                   dsaSrcDevice[dev]},
                                            up0 [0],
                                            cfi0 [1],
                                            isVlan1Exists [GT_TRUE],
                                            vid1 [100],
                                            up1 [3],
                                            cfi1 [1],
                                            etherType [0],
                                            macToMe [GT_FALSE]}
            Expected: GT_OK.
        */
        ruleIndex = 2;
        keyType   = CPSS_DXCH_TTI_KEY_ETH_E;

        prvSetDefaultEthPattern(&pattern);
        prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE2_ENT);
        prvSetDefaultEthMask(&mask);

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, keyType);

        /*
            1.6. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                              other params same as in 1.3.
            Expected: GT_OK and same values as written
        */
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &patternGet, &maskGet, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &actionGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                     "cpssDxChTtiRuleGet: %d, %d, %d", dev, keyType, ruleIndex);
        /* validation values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.eth,
                                     (GT_VOID*) &patternGet.eth,
                                     sizeof(pattern.eth))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another patternPtr->eth then was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.eth,
                                     (GT_VOID*) &maskGet.eth,
                                     sizeof(mask.eth))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another maskPtr->eth then was set: %d", dev);

        prvSetDefaultIpv4Pattern(&pattern);

        /*
            1.7. Call with out of range patternPtr->ipv4.common.pclId [3] (for IPV4 must < BIT_10)
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        keyType = CPSS_DXCH_TTI_KEY_IPV4_E;

        pattern.ipv4.common.pclId = 0x400;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, patternPtr->ipv4.common.pclId = %d",
                                         dev, pattern.ipv4.common.pclId);
        pattern.ipv4.common.pclId = 1;
        /*
            1.8. Call with patternPtr->ipv4.common.srcIsTrunk [GT_FALSE]
                           out of range srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.ipv4.common.srcIsTrunk   = GT_FALSE;
        pattern.ipv4.common.srcPortTrunk = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.srcIsTrunk = %d, ->common.srcPortTrunk = %d",
                                     dev, pattern.ipv4.common.srcIsTrunk, pattern.ipv4.common.srcPortTrunk);

        pattern.ipv4.common.srcPortTrunk = 0;

        /*
            1.9. Call with patternPtr->ipv4.common.srcIsTrunk [GT_TRUE]
                           out of range srcPortTrunk [128],
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.ipv4.common.srcIsTrunk   = GT_TRUE;
        pattern.ipv4.common.srcPortTrunk = 128;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->common.srcIsTrunk = %d, ->common.srcPortTrunk = %d",
                                         dev, pattern.ipv4.common.srcIsTrunk, pattern.ipv4.common.srcPortTrunk);

        pattern.ipv4.common.srcPortTrunk = 2;

        /*
            1.10. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS],
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.ipv4.common.vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.vid = %d",
                                     dev, pattern.ipv4.common.vid);

        pattern.ipv4.common.vid = 100;

        /*
            1.11. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                            patternPtr->ipv4.common.dsaSrcIsTrunk [GT_FALSE]
                           out of range dsaSrcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.ipv4.common.isTagged        = GT_TRUE;
        pattern.ipv4.common.dsaSrcIsTrunk   = GT_FALSE;
        pattern.ipv4.common.dsaSrcPortTrunk = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.dsaSrcIsTrunk = %d, ->common.dsaSrcPortTrunk = %d",
                                     dev, pattern.ipv4.common.dsaSrcIsTrunk, pattern.ipv4.common.dsaSrcPortTrunk);

        pattern.ipv4.common.dsaSrcPortTrunk = 0;

        /*
            1.12. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                            patternPtr->ipv4.common.dsaSrcIsTrunk [GT_TRUE]
                            out of range dsaSrcPortTrunk [128],
                            and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.ipv4.common.dsaSrcIsTrunk   = GT_TRUE;
        pattern.ipv4.common.dsaSrcPortTrunk = 128;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->common.dsaSrcIsTrunk = %d, ->common.dsaSrcPortTrunk = %d",
                                         dev, pattern.ipv4.common.dsaSrcIsTrunk, pattern.ipv4.common.dsaSrcPortTrunk);

        pattern.ipv4.common.dsaSrcPortTrunk = 2;

        /*
            1.13. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                            out of range dsaSrcDevice [PRV_CPSS_MAX_PP_DEVICES_CNS],
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.ipv4.common.dsaSrcDevice = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.dsaSrcDevice = %d",
                                     dev, pattern.ipv4.common.dsaSrcDevice);

        pattern.ipv4.common.dsaSrcDevice = dev;

        /*
            1.14. Call with wrong enum values patternPtr->ipv4.tunneltype
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            pattern.ipv4.tunneltype);

        /*
            1.15. Call with out of range patternPtr->mpls.common.pclId [3] (for MPLSmust be 2)
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        keyType = CPSS_DXCH_TTI_KEY_ETH_E;

        pattern.mpls.common.pclId = 3;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, patternPtr->mpls.common.pclId = %d",
                                         dev, pattern.mpls.common.pclId);

        pattern.mpls.common.pclId = 2;

        /*
            1.16. Call with patternPtr->mpls.common.srcIsTrunk [GT_FALSE]
                           out of range srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.mpls.common.srcIsTrunk   = GT_FALSE;
        pattern.mpls.common.srcPortTrunk = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.srcIsTrunk = %d, ->common.srcPortTrunk = %d",
                                     dev, pattern.mpls.common.srcIsTrunk, pattern.mpls.common.srcPortTrunk);

        pattern.mpls.common.srcPortTrunk = 0;

        /*
            1.17. Call with patternPtr->mpls.common.srcIsTrunk [GT_TRUE]
                           out of range srcPortTrunk [128],
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.mpls.common.srcIsTrunk   = GT_TRUE;
        pattern.mpls.common.srcPortTrunk = 128;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->common.srcIsTrunk = %d, ->common.srcPortTrunk = %d",
                                         dev, pattern.mpls.common.srcIsTrunk, pattern.mpls.common.srcPortTrunk);

        pattern.mpls.common.srcPortTrunk = 2;

        /*
            1.18. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS],
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.mpls.common.vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.vid = %d",
                                     dev, pattern.mpls.common.vid);

        pattern.mpls.common.vid = 100;

        /*
            1.19. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                            patternPtr->mpls.common.dsaSrcIsTrunk [GT_FALSE]
                           out of range dsaSrcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.mpls.common.isTagged        = GT_TRUE;
        pattern.mpls.common.dsaSrcIsTrunk   = GT_FALSE;
        pattern.mpls.common.dsaSrcPortTrunk = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.dsaSrcIsTrunk = %d, ->common.dsaSrcPortTrunk = %d",
                                     dev, pattern.mpls.common.dsaSrcIsTrunk, pattern.mpls.common.dsaSrcPortTrunk);

        pattern.mpls.common.dsaSrcPortTrunk = 0;

        /*
            1.20. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                            patternPtr->mpls.common.dsaSrcIsTrunk [GT_TRUE]
                            out of range dsaSrcPortTrunk [128],
                            and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.mpls.common.dsaSrcIsTrunk   = GT_TRUE;
        pattern.mpls.common.dsaSrcPortTrunk = 128;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->common.dsaSrcIsTrunk = %d, ->common.dsaSrcPortTrunk = %d",
                                         dev, pattern.mpls.common.dsaSrcIsTrunk, pattern.mpls.common.dsaSrcPortTrunk);

        pattern.mpls.common.dsaSrcPortTrunk = 2;

        /*
            1.21. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                            out of range dsaSrcDevice [PRV_CPSS_MAX_PP_DEVICES_CNS],
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pattern.mpls.common.dsaSrcDevice = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.dsaSrcDevice = %d",
                                     dev, pattern.mpls.common.dsaSrcDevice);

        pattern.mpls.common.dsaSrcDevice = dev;

        /*
            1.22. Call with wrong enum values actionPtr->ttPassengerPacketType
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            action.type2.ttPassengerPacketType);

        /*
            1.23. Call with wrong enum values actionPtr->command
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            action.type2.command);

        /*
            1.24. Call with wrong enum values actionPtr->redirectCommand
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            action.type2.redirectCommand);

        /*
            1.25. Call with wrong enum values actionPtr->tag0VlanCmd
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            action.type2.tag0VlanCmd);

        /*
            1.26 Call with wrong enum values actionPtr->tag1VlanCmd
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            action.type2.tag1VlanCmd);

        /*
            1.27. Call with wrong enum values actionPtr->qosPrecedence
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            action.type2.qosPrecedence);

        /*
            1.28. Call with wrong enum values actionPtr->userDefinedCpuCode
                            and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            action.type2.userDefinedCpuCode);

        /*
            1.29. Call with wrong enum values keyType and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            keyType);

        /*
            1.30. Call with wrong enum values actionType and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleSet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            actionType);

        /*
            1.31. Call with out of range ruleIndex [maxRuleIndex] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        ruleIndex = prvGetMaxRuleIndex(dev);

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleIndex = %d", dev, ruleIndex);

        ruleIndex = 0;

        /*
            1.32. Call function with patternPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, NULL, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, patternPtr = NULL", dev);

        /*
            1.33. Call function with maskPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, NULL, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);

        /*
            1.34. Call function with actionPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);

    }

    ruleIndex = 0;
    keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

    prvSetDefaultIpv4Pattern(&pattern);
    prvSetDefaultIpv4Mask(&mask);

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {

        prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE2_ENT);
        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_SUPPORTED, st, dev);
        else
        {
            if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(dev))
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
            else
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }

    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiRuleGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_U32                              ruleIndex,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiRuleGet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: DxCh3)
    1.1. Set rule for testing.
         Call cpssDxChTtiRuleSet with ruleIndex [0],
                   keyType [CPSS_DXCH_TTI_KEY_IPV4_E],
                   patternPtr->ipv4{common{pclId[1],
                                           srcIsTrunk[GT_FALSE],
                                           srcPortTrunk[23],
                                           mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                           vid[100],
                                           isTagged[GT_TRUE],
                                           dsaSrcIsTrunk[GT_FALSE],
                                           dsaSrcPortTrunk[0],
                                           dsaSrcDevice[dev] },
                                    tunneltype [IPv4_OVER_IPv4],
                                    srcIp[192.168.0.1],
                                    destIp[192.168.0.2],
                                    isArp[GT_FALSE] },
                   maskPtr->ipv4 {0xFF,...},
                   actionPtr{tunnelTerminate[GT_TRUE],
                             passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
                             copyTtlFromTunnelHeader[GT_FALSE],
                             command [CPSS_PACKET_CMD_FORWARD_E],
                             redirectCommand[CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E],
                             egressInterface{type[CPSS_INTERFACE_TRUNK_E],
                                             trunkId[2]},

                             tunnelStart [GT_TRUE],
                             tunnelStartPtr[0],
                             targetIsTrunk[GT_FALSE],
                             useVidx[GT_FALSE],

                             sourceIdSetEnable[GT_TRUE],
                             sourceId[0],

                             vlanCmd[CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E],
                             vlanId[100],
                             vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                             nestedVlanEnable[GT_FALSE],

                             bindToPolicer[GT_FALSE],

                             qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                             qosTrustMode[CPSS_DXCH_TTI_QOS_UNTRUST_E],
                             qosProfile[0],
                             modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E],
                             modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
                             up[0],

                             mirrorToIngressAnalyzerEnable[GT_FALSE],

                             vntl2Echo[GT_FALSE],
                             bridgeBypass[GT_FALSE],
                             actionStop[GT_FALSE] }
    Expected: GT_OK.
    1.2. Call with keyType [CPSS_DXCH_TTI_KEY_IPV4_E /
                            CPSS_DXCH_TTI_KEY_ETH_E],
                   ruleIndex [ 0 / 10],
                   non-NULL pointers.

    Expected: GT_OK and same values as written
    1.3. Call with wrong enum values keyType , other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong enum values keyType , other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range ruleIndex[maxRuleIndex], other params same as in 1.2.
    Expected: non GT_OK.
    1.6. Call with patternPtr [NULL], other params same as in 1.2.
    Expected: GT_BAD_PTR.
    1.7. Call with maskPtr [NULL], other params same as in 1.2.
    Expected: GT_BAD_PTR.
    1.8. Call with actionPtr [NULL], other params same as in 1.2.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      ruleIndex = 0;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_RULE_UNT      pattern;
    CPSS_DXCH_TTI_RULE_UNT      mask;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT actionType;
    CPSS_DXCH_TTI_ACTION_UNT    action;

    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Set rule for testing.
                 Call cpssDxChTtiRuleSet with ruleIndex [0],
                           keyType [CPSS_DXCH_TTI_KEY_IPV4_E],
                           patternPtr->ipv4{common{pclId[1],
                                                   srcIsTrunk[GT_FALSE],
                                                   srcPortTrunk[23],
                                                   mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                                   vid[100],
                                                   isTagged[GT_TRUE],
                                                   dsaSrcIsTrunk[GT_FALSE],
                                                   dsaSrcPortTrunk[0],
                                                   dsaSrcDevice[dev] },
                                            tunneltype [IPv4_OVER_IPv4],
                                            srcIp[192.168.0.1],
                                            destIp[192.168.0.2],
                                            isArp[GT_FALSE] },
                           maskPtr->ipv4 {0xFF,...},
                           actionPtr{tunnelTerminate[GT_TRUE],
                                     passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
                                     copyTtlFromTunnelHeader[GT_FALSE],
                                     command [CPSS_PACKET_CMD_FORWARD_E],
                                     redirectCommand[CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E],
                                     egressInterface{type[CPSS_INTERFACE_TRUNK_E],
                                                     trunkId[2]},

                                     tunnelStart [GT_TRUE],
                                     tunnelStartPtr[0],
                                     targetIsTrunk[GT_FALSE],
                                     useVidx[GT_FALSE],

                                     sourceIdSetEnable[GT_TRUE],
                                     sourceId[0],

                                     vlanCmd[CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E],
                                     vlanId[100],
                                     vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                     nestedVlanEnable[GT_FALSE],

                                     bindToPolicer[GT_FALSE],

                                     qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                     qosTrustMode[CPSS_DXCH_TTI_QOS_UNTRUST_E],
                                     qosProfile[0],
                                     modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E],
                                     modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
                                     up[0],

                                     mirrorToIngressAnalyzerEnable[GT_FALSE],
                                     vntl2Echo[GT_FALSE],
                                     bridgeBypass[GT_FALSE],
                                     actionStop[GT_FALSE] }
            Expected: GT_OK.
        */
        ruleIndex = 0;
        keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

        prvSetDefaultIpv4Pattern(&pattern);
        prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE1_ENT);
        prvSetDefaultIpv4Mask(&mask);

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChTtiRuleSet: %d, %d, %d",
                                         dev, ruleIndex, keyType);
        /*
            1.2. Call with keyType [CPSS_DXCH_TTI_KEY_IPV4_E /
                                    CPSS_DXCH_TTI_KEY_ETH_E],
                           ruleIndex [ 0 / 10],
                           non-NULL pointers.
            Expected: GT_OK and same values as written
        */
        keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;
        ruleIndex = 0;

        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, keyType);

        /*
            1.3. Call with wrong enum values keyType, other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleGet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            keyType);

        /*
            1.4. Call with wrong enum values actionType, other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleGet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            actionType);

        /*
            1.5. Call with out of range ruleIndex [maxRuleIndex], other params same as in 1.2.
            Expected: non GT_OK.
        */
        ruleIndex = prvGetMaxRuleIndex(dev);

        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = 0;

        /*
            1.6. Call with patternPtr [NULL], other params same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, NULL, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, patternPtr = NULL", dev);

        /*
            1.7. Call with maskPtr [NULL], other params same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &pattern, NULL, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);

        /*
            1.8. Call with actionPtr [NULL], other params same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);
    }

    ruleIndex = 0;
    keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        }
   }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiRuleGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_U32                              ruleIndex,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_STC            *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiRuleGet_type2)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion)
    1.1. Set rule for testing.
         Call cpssDxChTtiRuleSet with ruleIndex [0],
                   keyType [CPSS_DXCH_TTI_KEY_IPV4_E],
                   patternPtr->ipv4{common{pclId[1],
                                           srcIsTrunk[GT_FALSE],
                                           srcPortTrunk[23],
                                           mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                           vid[100],
                                           isTagged[GT_TRUE],
                                           dsaSrcIsTrunk[GT_FALSE],
                                           dsaSrcPortTrunk[0],
                                           dsaSrcDevice[dev] },
                                    tunneltype [IPv4_OVER_IPv4],
                                    srcIp[192.168.0.1],
                                    destIp[192.168.0.2],
                                    isArp[GT_FALSE] },
                   maskPtr->ipv4 {0xFF,...},
                   actionPtr{tunnelTerminate        [GT_TRUE]
                            ttPassengerPacketType  [CPSS_DXCH_TTI_PASSENGER_IPV4V6_E]
                            tsPassengerPacketType  [CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E]
                            copyTtlFromTunnelHeader[GT_FALSE]
                            mplsCommand            [CPSS_DXCH_TTI_MPLS_NOP_CMD_E]
                            mplsTtl                [0]
                            enableDecrementTtl     [ GT_FALSE]
                            command                [CPSS_PACKET_CMD_MIRROR_TO_CPU_E]
                            redirectCommand        [CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E]
                            egressInterface.type   [CPSS_INTERFACE_TRUNK_E]
                            egressInterface.trunkId[2]
                            arpPtr                 [0]
                            tunnelStart            [GT_TRUE]
                            tunnelStartPtr         [0]
                            routerLttPtr           [0]
                            vrfId                  [0]
                            sourceIdSetEnable      [GT_TRUE]
                            sourceId               [0]
                            tag0VlanCmd            [CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E]
                            tag0VlanId             [100]
                            tag1VlanCmd            [CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E]
                            tag1VlanId             [100]
                            tag0VlanPrecedence     [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E]
                            nestedVlanEnable       [GT_FALSE]
                            bindToPolicerMeter     [GT_FALSE]
                            bindToPolicer          [GT_FALSE]
                            policerIndex           [0]
                            qosPrecedence          [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E]
                            keepPreviousQoS        [GT_FALSE]
                            trustUp                [0]
                            trustDscp              [0]
                            trustExp               [0]
                            qosProfile             [0]
                            modifyTag0Up           [GT_FALSE]
                            tag1UpCommand          [CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E]
                            modifyDscp             [GT_FALSE]
                            tag0Up                 [0]
                            tag1Up                 [0]
                            remapDSCP              [GT_FALSE]
                            pcl0OverrideConfigIndex[CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E]
                            pcl0_1OverrideConfigIndex[CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E]
                            pcl1OverrideConfigIndex [CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E]
                            iPclConfigIndex        [0]
                            mirrorToIngressAnalyzerEnable[GT_FALSE]
                            userDefinedCpuCode           [CPSS_NET_FIRST_USER_DEFINED_E]
                            bindToCentralCounter         [GT_FALSE]
                            centralCounterIndex          [0]
                            vntl2Echo                    [GT_FALSE]
                            bridgeBypass                 [GT_FALSE]
                            ingressPipeBypass            [GT_FALSE]
                            actionStop                   [GT_FALSE]}
    Expected: GT_OK.
    1.2. Call with keyType [CPSS_DXCH_TTI_KEY_IPV4_E /
                            CPSS_DXCH_TTI_KEY_ETH_E],
                   ruleIndex [ 0 / 10],
                   non-NULL pointers.

    Expected: GT_OK and same values as written
    1.3. Call with wrong enum values keyType , other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong enum values actionType , other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range ruleIndex[maxRuleIndex], other params same as in 1.2.
    Expected: non GT_OK.
    1.6. Call with patternPtr [NULL], other params same as in 1.2.
    Expected: GT_BAD_PTR.
    1.7. Call with maskPtr [NULL], other params same as in 1.2.
    Expected: GT_BAD_PTR.
    1.8. Call with actionPtr [NULL], other params same as in 1.2.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      ruleIndex = 0;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_RULE_UNT      pattern;
    CPSS_DXCH_TTI_RULE_UNT      mask;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT actionType;
    CPSS_DXCH_TTI_ACTION_UNT    action;


    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Set rule for testing.
                 Call cpssDxChTtiRuleSet with ruleIndex [0],
                           keyType [CPSS_DXCH_TTI_KEY_IPV4_E],
                           patternPtr->ipv4{common{pclId[1],
                                                   srcIsTrunk[GT_FALSE],
                                                   srcPortTrunk[23],
                                                   mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                                   vid[100],
                                                   isTagged[GT_TRUE],
                                                   dsaSrcIsTrunk[GT_FALSE],
                                                   dsaSrcPortTrunk[0],
                                                   dsaSrcDevice[dev] },
                                            tunneltype [IPv4_OVER_IPv4],
                                            srcIp[192.168.0.1],
                                            destIp[192.168.0.2],
                                            isArp[GT_FALSE] },
                           maskPtr->ipv4 {0xFF,...},
                           actionPtr{tunnelTerminate        [GT_TRUE]
                                    ttPassengerPacketType  [CPSS_DXCH_TTI_PASSENGER_IPV4V6_E]
                                    tsPassengerPacketType  [CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E]
                                    copyTtlFromTunnelHeader[GT_FALSE]
                                    mplsCommand            [CPSS_DXCH_TTI_MPLS_NOP_CMD_E]
                                    mplsTtl                [0]
                                    enableDecrementTtl     [ GT_FALSE]
                                    command                [CPSS_PACKET_CMD_MIRROR_TO_CPU_E]
                                    redirectCommand        [CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E]
                                    egressInterface.type   [CPSS_INTERFACE_TRUNK_E]
                                    egressInterface.trunkId[2]
                                    arpPtr                 [0]
                                    tunnelStart            [GT_TRUE]
                                    tunnelStartPtr         [0]
                                    routerLttPtr           [0]
                                    vrfId                  [0]
                                    sourceIdSetEnable      [GT_TRUE]
                                    sourceId               [0]
                                    tag0VlanCmd            [CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E]
                                    tag0VlanId             [100]
                                    tag1VlanCmd            [CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E]
                                    tag1VlanId             [100]
                                    tag0VlanPrecedence     [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E]
                                    nestedVlanEnable       [GT_FALSE]
                                    bindToPolicerMeter     [GT_FALSE]
                                    bindToPolicer          [GT_FALSE]
                                    policerIndex           [0]
                                    qosPrecedence          [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E]
                                    keepPreviousQoS        [GT_FALSE]
                                    trustUp                [0]
                                    trustDscp              [0]
                                    trustExp               [0]
                                    qosProfile             [0]
                                    modifyTag0Up           [GT_FALSE]
                                    tag1UpCommand          [CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E]
                                    modifyDscp             [GT_FALSE]
                                    tag0Up                 [0]
                                    tag1Up                 [0]
                                    remapDSCP              [GT_FALSE]
                                    pcl0OverrideConfigIndex[CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E]
                                    pcl0_1OverrideConfigIndex[CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E]
                                    pcl1OverrideConfigIndex [CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E]
                                    iPclConfigIndex        [0]
                                    mirrorToIngressAnalyzerEnable[GT_FALSE]
                                    userDefinedCpuCode           [CPSS_NET_FIRST_USER_DEFINED_E]
                                    bindToCentralCounter         [GT_FALSE]
                                    centralCounterIndex          [0]
                                    vntl2Echo                    [GT_FALSE]
                                    bridgeBypass                 [GT_FALSE]
                                    ingressPipeBypass            [GT_FALSE]
                                    actionStop                   [GT_FALSE]}
            Expected: GT_OK.
        */
        ruleIndex = 0;
        keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

        prvSetDefaultIpv4Pattern(&pattern);
        prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE2_ENT);
        prvSetDefaultIpv4Mask(&mask);

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChTtiRuleSet: %d, %d, %d",
                                         dev, ruleIndex, keyType);
        /*
            1.2. Call with keyType [CPSS_DXCH_TTI_KEY_IPV4_E /
                                    CPSS_DXCH_TTI_KEY_ETH_E],
                           ruleIndex [ 0 / 10],
                           non-NULL pointers.
            Expected: GT_BAD_PARAM for xCat A1 and above else GT_OK and same values as written
        */
        keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;
        ruleIndex = 0;

        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, keyType);

        /*
            1.3. Call with wrong enum values keyType, other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        actionType = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleGet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            keyType);

        /*
            1.4. Call with wrong enum values actionType, other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleGet
                            (dev, ruleIndex, keyType, &pattern, &mask, actionType, &action),
                            actionType);

        /*
            1.5. Call with out of range ruleIndex [maxRuleIndex], other params same as in 1.2.
            Expected: non GT_OK.
        */
        ruleIndex = prvGetMaxRuleIndex(dev);

        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = 0;

        /*
            1.6. Call with patternPtr [NULL], other params same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, NULL, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, patternPtr = NULL", dev);

        /*
            1.7. Call with maskPtr [NULL], other params same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &pattern, NULL, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);

        /*
            1.8. Call with actionPtr [NULL], other params same as in 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);
    }

    ruleIndex = 0;
    keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_SUPPORTED, st, dev);
        }
        else
        {
            if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(dev))
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
            else
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              ruleIndex,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_STC           *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiRuleActionUpdate)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: DxCh3)
    1.1. actionPtr{tunnelTerminate[GT_TRUE],
                   passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
                   copyTtlFromTunnelHeader[GT_FALSE],
                   command [CPSS_PACKET_CMD_FORWARD_E],
                   redirectCommand[CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E],
                   egressInterface{type[CPSS_INTERFACE_TRUNK_E],
                                   trunkId[2]},

                   tunnelStart [GT_TRUE],
                   tunnelStartPtr[0],
                   targetIsTrunk[GT_FALSE],
                   useVidx[GT_FALSE],

                   sourceIdSetEnable[GT_TRUE],
                   sourceId[0],

                   vlanCmd[CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E],
                   vlanId[100],
                   vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                   nestedVlanEnable[GT_FALSE],

                   bindToPolicer[GT_FALSE],

                   qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                   qosTrustMode[CPSS_DXCH_TTI_QOS_UNTRUST_E],
                   qosProfile[0],
                   modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E],
                   modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
                   up[0],

                   mirrorToIngressAnalyzerEnable[GT_FALSE],
                   vntl2Echo[GT_FALSE],
                   bridgeBypass[GT_FALSE],
                   actionStop[GT_FALSE]
              }
    Expected: GT_OK.
    1.2. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                      other params same as in 1.1.
    Expected: GT_OK and same action as written
    1.3. Call with out of range ruleIndex [3k],
                   other params same as in 1.1.
    Expected: non GT_OK.
    1.4. Call with wrong enum values actionType, other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with actionPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      ruleIndex = 0;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_RULE_UNT      pattern;
    CPSS_DXCH_TTI_RULE_UNT      mask;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    CPSS_DXCH_TTI_ACTION_UNT    actionGet;
    GT_BOOL                     isEqual   = GT_FALSE;


    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));
    cpssOsBzero((GT_VOID*) &actionGet, sizeof(actionGet));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            Create default rule with action
            ruleIndex [0],
            keyType [CPSS_DXCH_TTI_KEY_IPV4_E],

            patternPtr { ipv4{common{pclId[1], srcIsTrunk[GT_FALSE], srcPortTrunk[0],
            mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA], vid[0], isTagged[GT_FALSE], dsaSrcIsTrunk[GT_FALSE],
            dsaSrcPortTrunk[0], dsaSrcDevice[0] }, tunneltype [IPv4_OVER_IPv4], srcIp[192.168.0.1],
            destIp[192.168.0.2], isArp[GT_FALSE] }, mpls{common{pclId[1], srcIsTrunk[GT_FALSE],
            srcPortTrunk[0], mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA], vid[0], isTagged[GT_FALSE],
            dsaSrcIsTrunk[GT_FALSE], dsaSrcPortTrunk[0], dsaSrcDevice[0] },
            label0[0], exp0[0], label1[0], exp1[0], label2[0], exp2[0], numOfLabels[1],
            protocolAboveMPLS[IPv4]}, eth{ common{ pclId[1], srcIsTrunk[GT_FALSE], srcPortTrunk[0],
            mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA], vid[0], isTagged[GT_FALSE], dsaSrcIsTrunk[GT_FALSE],
            dsaSrcPortTrunk[0], dsaSrcDevice[0]}, up0[0], cfi0[0], isVlan1Exists[GT_FALSE], vid1[0], up1[0],
            cfi1[0], etherType[0], macToMe[GT_FALSE]}  },

            maskPtr {All fields are zeroed},

            actionPtr{ tunnelTerminate[GT_FALSE], passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
            copyTtlFromTunnelHeader[GT_FALSE], command [CPSS_PACKET_CMD_FORWARD_E],
            redirectCommand[CPSS_DXCH_TTI_NO_REDIRECT_E], egressInterface{type[CPSS_INTERFACE_TRUNK_E],
            trunkId[0], vidx[0], vlanId[0], devPort {0, 0}}, tunnelStart [GT_FALSE], tunnelStartPtr[0],
            routerLookupPtr[0], vrfId[0], targetIsTrunk[GT_FALSE], virtualSrcPort[0], virtualSrcdev[0],
            useVidx[GT_FALSE], sourceIdSetEnable[GT_FALSE], sourceId[0], vlanCmd[CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E],
            vlanId[0], vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
            nestedVlanEnable[GT_FALSE], bindToPolicer[GT_FALSE], policerIndex[0],
            qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
            qosTrustMode[CPSS_DXCH_TTI_QOS_KEEP_PREVIOUS_E], qosProfile[0],
            modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E], modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
            up[0], remapDSCP[GT_FALSE], mirrorToIngressAnalyzerEnable[GT_FALSE],
            userDefinedCpuCode[CPSS_NET_CONTROL_E],
            vntl2Echo[GT_FALSE], bridgeBypass[GT_FALSE], actionStop[GT_FALSE] }

            Expected: GT_OK.
        */
        ruleIndex = 0;
        keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

        prvSetDefaultIpv4Pattern(&pattern);
        prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE1_ENT);
        prvSetDefaultIpv4Mask(&mask);

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChTtiRuleSet: %d, %d, %d",
                                         dev, ruleIndex, keyType);
        /*
            1.1. Call function with rukeIndex[0], actionPtr{ tunnelTerminate[GT_FALSE], passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E], copyTtlFromTunnelHeader[GT_FALSE], command [CPSS_PACKET_CMD_FORWARD_E], redirectCommand[CPSS_DXCH_TTI_NO_REDIRECT_E], egressInterface{type[CPSS_INTERFACE_TRUNK_E], trunkId[0], vidx[0], vlanId[0], devPort {0, 0}}, tunnelStart [GT_FALSE], tunnelStartPtr[0], routerLookupPtr[0], vrfId[0], targetIsTrunk[GT_FALSE], virtualSrcPort[0], virtualSrcdev[0], useVidx[GT_FALSE], sourceIdSetEnable[GT_FALSE], sourceId[0], vlanCmd[CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E], vlanId[0], vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E], nestedVlanEnable[GT_FALSE], bindToPolicer[GT_FALSE], policerIndex[0], qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E], qosTrustMode[CPSS_DXCH_TTI_QOS_KEEP_PREVIOUS_E], qosProfile[0], modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E], modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E], up[0], remapDSCP[GT_FALSE], mirrorToIngressAnalyzerEnable[GT_FALSE], userDefinedCpuCode[CPSS_NET_CONTROL_E], vntl2Echo[GT_FALSE], bridgeBypass[GT_FALSE], actionStop[GT_FALSE] }
            Expected: GT_OK.
        */
        ruleIndex = 0;

        prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE1_ENT);

        st = cpssDxChTtiRuleActionUpdate(dev, ruleIndex, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        /*
            1.2. Call cpssDxChTtiRuleGet with non-NULL pointers, other params same as in 1.1.
            Expected: GT_OK and same action as written
        */
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &actionGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChTtiRuleGet: %d, %d, %d",
                                     dev, keyType, ruleIndex);
        /* validation values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &action,
                                     (GT_VOID*) &actionGet,
                                     sizeof(action))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another actionPtr then was set: %d", dev);

       /*
            1.3. Call with out of range ruleIndex [maxRuleIndex],
                           other params same as in 1.1.
            Expected: non GT_OK.
        */
        ruleIndex = prvGetMaxRuleIndex(dev);

        st = cpssDxChTtiRuleActionUpdate(dev, ruleIndex, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = 0;

       /*
            1.4. Call with wrong enum values actionType, other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleActionUpdate
                            (dev, ruleIndex, actionType, &action),
                            actionType);

        /*
            1.5. Call with actionPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleActionUpdate(dev, ruleIndex, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);
    }

    ruleIndex = 0;
    keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

    prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE1_ENT);

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiRuleActionUpdate(dev, ruleIndex, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        }
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiRuleActionUpdate(dev, ruleIndex, CPSS_DXCH_TTI_ACTION_TYPE1_ENT, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              ruleIndex,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_STC           *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiRuleActionUpdate_type2)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion; xCat2)
    1.1. actionPtr{tunnelTerminate[GT_TRUE],
                   passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
                   copyTtlFromTunnelHeader[GT_FALSE],
                   command [CPSS_PACKET_CMD_FORWARD_E],
                   redirectCommand[CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E],
                   egressInterface{type[CPSS_INTERFACE_TRUNK_E],
                                   trunkId[2]},

                   tunnelStart [GT_TRUE],
                   tunnelStartPtr[0],
                   targetIsTrunk[GT_FALSE],
                   useVidx[GT_FALSE],

                   sourceIdSetEnable[GT_TRUE],
                   sourceId[0],

                   vlanCmd[CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E],
                   vlanId[100],
                   vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                   nestedVlanEnable[GT_FALSE],

                   bindToPolicer[GT_FALSE],

                   qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                   qosTrustMode[CPSS_DXCH_TTI_QOS_UNTRUST_E],
                   qosProfile[0],
                   modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E],
                   modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
                   up[0],

                   mirrorToIngressAnalyzerEnable[GT_FALSE],
                   vntl2Echo[GT_FALSE],
                   bridgeBypass[GT_FALSE],
                   actionStop[GT_FALSE]
              }
    Expected: GT_OK.
    1.2. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                      other params same as in 1.1.
    Expected: GT_OK and same action as written
    1.3. Call with out of range ruleIndex [3k],
                   other params same as in 1.1.
    Expected: non GT_OK.
    1.4. Call with wrong enum values actionType, other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with actionPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      ruleIndex = 0;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT actionType = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_RULE_UNT      pattern;
    CPSS_DXCH_TTI_RULE_UNT      mask;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    CPSS_DXCH_TTI_ACTION_UNT    actionGet;
    GT_BOOL                     isEqual   = GT_FALSE;


    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));
    cpssOsBzero((GT_VOID*) &actionGet, sizeof(actionGet));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            Create default rule with action
            ruleIndex [0],
            keyType [CPSS_DXCH_TTI_KEY_IPV4_E],

            patternPtr { ipv4{common{pclId[1], srcIsTrunk[GT_FALSE], srcPortTrunk[0],
            mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA], vid[0], isTagged[GT_FALSE], dsaSrcIsTrunk[GT_FALSE],
            dsaSrcPortTrunk[0], dsaSrcDevice[0] }, tunneltype [IPv4_OVER_IPv4], srcIp[192.168.0.1],
            destIp[192.168.0.2], isArp[GT_FALSE] }, mpls{common{pclId[1], srcIsTrunk[GT_FALSE],
            srcPortTrunk[0], mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA], vid[0], isTagged[GT_FALSE],
            dsaSrcIsTrunk[GT_FALSE], dsaSrcPortTrunk[0], dsaSrcDevice[0] },
            label0[0], exp0[0], label1[0], exp1[0], label2[0], exp2[0], numOfLabels[1],
            protocolAboveMPLS[IPv4]}, eth{ common{ pclId[1], srcIsTrunk[GT_FALSE], srcPortTrunk[0],
            mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA], vid[0], isTagged[GT_FALSE], dsaSrcIsTrunk[GT_FALSE],
            dsaSrcPortTrunk[0], dsaSrcDevice[0]}, up0[0], cfi0[0], isVlan1Exists[GT_FALSE], vid1[0], up1[0],
            cfi1[0], etherType[0], macToMe[GT_FALSE]}  },

            maskPtr {All fields are zeroed},

            actionPtr{ tunnelTerminate[GT_FALSE], passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
            copyTtlFromTunnelHeader[GT_FALSE], command [CPSS_PACKET_CMD_FORWARD_E],
            redirectCommand[CPSS_DXCH_TTI_NO_REDIRECT_E], egressInterface{type[CPSS_INTERFACE_TRUNK_E],
            trunkId[0], vidx[0], vlanId[0], devPort {0, 0}}, tunnelStart [GT_FALSE], tunnelStartPtr[0],
            routerLookupPtr[0], vrfId[0], targetIsTrunk[GT_FALSE], virtualSrcPort[0], virtualSrcdev[0],
            useVidx[GT_FALSE], sourceIdSetEnable[GT_FALSE], sourceId[0], vlanCmd[CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E],
            vlanId[0], vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
            nestedVlanEnable[GT_FALSE], bindToPolicer[GT_FALSE], policerIndex[0],
            qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
            qosTrustMode[CPSS_DXCH_TTI_QOS_KEEP_PREVIOUS_E], qosProfile[0],
            modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E], modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
            up[0], remapDSCP[GT_FALSE], mirrorToIngressAnalyzerEnable[GT_FALSE],
            userDefinedCpuCode[CPSS_NET_CONTROL_E],
            vntl2Echo[GT_FALSE], bridgeBypass[GT_FALSE], actionStop[GT_FALSE] }

            Expected: GT_BAD_PARAM for xCat A1 and above else GT_OK.
        */
        ruleIndex = 0;
        keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

        prvSetDefaultIpv4Pattern(&pattern);
        prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE2_ENT);
        prvSetDefaultIpv4Mask(&mask);

        st = cpssDxChTtiRuleSet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChTtiRuleSet: %d, %d, %d",
                                         dev, ruleIndex, keyType);
        /*
            1.1. Call function with rukeIndex[0], actionPtr{ tunnelTerminate[GT_FALSE], passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E], copyTtlFromTunnelHeader[GT_FALSE], command [CPSS_PACKET_CMD_FORWARD_E], redirectCommand[CPSS_DXCH_TTI_NO_REDIRECT_E], egressInterface{type[CPSS_INTERFACE_TRUNK_E], trunkId[0], vidx[0], vlanId[0], devPort {0, 0}}, tunnelStart [GT_FALSE], tunnelStartPtr[0], routerLookupPtr[0], vrfId[0], targetIsTrunk[GT_FALSE], virtualSrcPort[0], virtualSrcdev[0], useVidx[GT_FALSE], sourceIdSetEnable[GT_FALSE], sourceId[0], vlanCmd[CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E], vlanId[0], vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E], nestedVlanEnable[GT_FALSE], bindToPolicer[GT_FALSE], policerIndex[0], qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E], qosTrustMode[CPSS_DXCH_TTI_QOS_KEEP_PREVIOUS_E], qosProfile[0], modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E], modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E], up[0], remapDSCP[GT_FALSE], mirrorToIngressAnalyzerEnable[GT_FALSE], userDefinedCpuCode[CPSS_NET_CONTROL_E], vntl2Echo[GT_FALSE], bridgeBypass[GT_FALSE], actionStop[GT_FALSE] }
            Expected: GT_OK.
        */
        ruleIndex = 0;

        prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE2_ENT);

        st = cpssDxChTtiRuleActionUpdate(dev, ruleIndex, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        /*
            1.2. Call cpssDxChTtiRuleGet with non-NULL pointers, other params same as in 1.1.
            Expected: GT_OK and same action as written
        */
        st = cpssDxChTtiRuleGet(dev, ruleIndex, keyType, &pattern, &mask, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &actionGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChTtiRuleGet: %d, %d, %d",
                                     dev, keyType, ruleIndex);
        /* validation values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &action,
                                     (GT_VOID*) &actionGet,
                                     sizeof(action))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another actionPtr then was set: %d", dev);

       /*
            1.3. Call with out of range ruleIndex [maxRuleIndex],
                           other params same as in 1.1.
            Expected: non GT_OK.
        */
        ruleIndex = prvGetMaxRuleIndex(dev);

        st = cpssDxChTtiRuleActionUpdate(dev, ruleIndex, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = 0;

        /*
            1.4. Call with wrong enum values actionType, other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiRuleActionUpdate
                            (dev, ruleIndex, actionType, &action),
                            actionType);

        /*
            1.5. Call with actionPtr [NULL], other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleActionUpdate(dev, ruleIndex, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);

    }

    ruleIndex = 0;
    keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE2_ENT);
        st = cpssDxChTtiRuleActionUpdate(dev, ruleIndex, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
        if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_SUPPORTED, st, dev);
        else
        {
            if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(dev))
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
            else
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiRuleActionUpdate(dev, ruleIndex, CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiMacModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_DXCH_TTI_MAC_MODE_ENT        macMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiMacModeSet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion)
    1.1. Call function with keyType[CPSS_DXCH_TTI_KEY_IPV4_E /
                                    CPSS_DXCH_TTI_KEY_MPLS_E /
                                    CPSS_DXCH_TTI_KEY_ETH_E],
                            macMode [CPSS_DXCH_TTI_MAC_MODE_DA_E /
                                     CPSS_DXCH_TTI_MAC_MODE_SA_E]
    Expected: GT_OK.
    1.2. Call cpssDxChTtiMacModeGet with non-NULL modePtr
                                    and the same keyType as in 1.1.
    Expected: GT_OK and same value as written.
    1.3. Call with wrong enum values keyType and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong enum values macMode and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType    = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_MAC_MODE_ENT  macMode    = CPSS_DXCH_TTI_MAC_MODE_DA_E;
    CPSS_DXCH_TTI_MAC_MODE_ENT  macModeGet = CPSS_DXCH_TTI_MAC_MODE_DA_E;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with keyType[CPSS_DXCH_TTI_KEY_IPV4_E /
                                            CPSS_DXCH_TTI_KEY_MPLS_E],
                                    macMode [CPSS_DXCH_TTI_MAC_MODE_DA_E /
                                             CPSS_DXCH_TTI_MAC_MODE_SA_E]
            Expected: GT_OK.
        */

        /* iterate with keyType = CPSS_DXCH_TTI_KEY_IPV4_E */
        keyType = CPSS_DXCH_TTI_KEY_IPV4_E;
        macMode = CPSS_DXCH_TTI_MAC_MODE_DA_E;

        st = cpssDxChTtiMacModeSet( dev, keyType, macMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, keyType, macMode);

        /*
            1.2. Call cpssDxChTtiMacModeGet with non-NULL modePtr
                                            and the same keyType as in 1.1.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxChTtiMacModeGet(dev, keyType, &macModeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiMacModeGet: %d, %d", dev, keyType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(macMode, macModeGet,
                   "got another macMode then was set: %d", dev);

        /* iterate with keyType = CPSS_DXCH_TTI_KEY_MPLS_E */
        keyType = CPSS_DXCH_TTI_KEY_MPLS_E;
        macMode = CPSS_DXCH_TTI_MAC_MODE_SA_E;

        st = cpssDxChTtiMacModeSet( dev, keyType, macMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, keyType, macMode);

        /*
            1.2. Call cpssDxChTtiMacModeGet with non-NULL modePtr
                                            and the same keyType as in 1.1.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxChTtiMacModeGet(dev, keyType, &macModeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiMacModeGet: %d, %d", dev, keyType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(macMode, macModeGet,
                   "got another macMode then was set: %d", dev);

        /*
            1.3. Call with wrong enum values keyType and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiMacModeSet
                            (dev, keyType, macMode),
                            keyType);

        /*
            1.4. Call with wrong enum values macMode and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiMacModeSet
                            (dev, keyType, macMode),
                            macMode);
    }

    keyType = CPSS_DXCH_TTI_KEY_IPV4_E;
    macMode = CPSS_DXCH_TTI_MAC_MODE_DA_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiMacModeSet( dev, keyType, macMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiMacModeSet( dev, keyType, macMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiMacModeGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *macModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiMacModeGet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion)
    1.1. Call with keyType[CPSS_DXCH_TTI_KEY_IPV4_E /
                           CPSS_DXCH_TTI_KEY_MPLS_E],
                   non-NULL macModePtr.
    Expected: GT_OK.
    1.2. Call with wrong enum values keyType
                   and other params as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with macModePtr [NULL]
                   and other params as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_MAC_MODE_ENT  macMode = CPSS_DXCH_TTI_MAC_MODE_DA_E;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with keyType[CPSS_DXCH_TTI_KEY_IPV4_E /
                                   CPSS_DXCH_TTI_KEY_MPLS_E],
                           non-NULL macModePtr.
            Expected: GT_OK.
        */

        /* iterate with keyType = CPSS_DXCH_TTI_KEY_IPV4_E */
        keyType = CPSS_DXCH_TTI_KEY_IPV4_E;

        st = cpssDxChTtiMacModeGet(dev, keyType, &macMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, keyType);

        /* iterate with keyType = CPSS_DXCH_TTI_KEY_MPLS_E */
        keyType = CPSS_DXCH_TTI_KEY_MPLS_E;

        st = cpssDxChTtiMacModeGet(dev, keyType, &macMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, keyType);

        /*
            1.2. Call with wrong enum values keyType and other params as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiMacModeGet
                            (dev, keyType, &macMode),
                            keyType);

        /*
            1.2. Call with macModePtr [NULL]
                           and other params as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiMacModeGet(dev, keyType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macModePtr = NULL", dev);
    }

    keyType = CPSS_DXCH_TTI_KEY_MPLS_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiMacModeGet(dev, keyType, &macMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiMacModeGet(dev, keyType, &macMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiIpv4GreEthTypeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_IPV4_GRE_TYPE_ENT     greTunnelType,
    IN  GT_U32                              ethType
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiIpv4GreEthTypeSet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion)
    1.1. Call with greTunnelType [CPSS_DXCH_TTI_IPV4_GRE0_E /
                                  CPSS_DXCH_TTI_IPV4_GRE1_E],
                   ethType[0 / 0xFFFF].
    Expected: GT_OK.
    1.2. Call function cpssDxChTtiIpv4GreEthTypeGet with not NULL ethTypePtr
                                                    and other params as in 1.1.
    Expected: GT_OK and same ethType as written.
    1.3. Call with wrong enum values greTunnelType
                   and other params as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_TTI_IPV4_GRE_TYPE_ENT greTunnelType = CPSS_DXCH_TTI_IPV4_GRE0_E;
    GT_U32                          ethType       = 0;
    GT_U32                          ethTypeGet    = 0;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with greTunnelType [CPSS_DXCH_TTI_IPV4_GRE0_E /
                                                   CPSS_DXCH_TTI_IPV4_GRE1_E],
                                    ethType[0 / 0xFFFF].
            Expected: GT_OK.
        */

        /* iterate with greTunnelType = CPSS_DXCH_TTI_IPV4_GRE0_E */
        greTunnelType = CPSS_DXCH_TTI_IPV4_GRE0_E;
        ethType       = 0;

        st = cpssDxChTtiIpv4GreEthTypeSet(dev, greTunnelType, ethType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, greTunnelType, ethType);

        /*
            1.2. Call function cpssDxChTtiIpv4GreEthTypeGet with not NULL ethTypePtr
                                                            and other params as in 1.1.
            Expected: GT_OK and same ethType as written.
        */
        st = cpssDxChTtiIpv4GreEthTypeGet(dev, greTunnelType, &ethTypeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiIpv4GreEthTypeGet: %d, %d", dev, greTunnelType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ethType, ethTypeGet,
                   "got another ethType then was set: %d", dev);

        /* iterate with greTunnelType = CPSS_DXCH_TTI_IPV4_GRE1_E */
        greTunnelType = CPSS_DXCH_TTI_IPV4_GRE1_E;
        ethType       = 0xFFFF;

        st = cpssDxChTtiIpv4GreEthTypeSet(dev, greTunnelType, ethType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, greTunnelType, ethType);

        /*
            1.2. Call function cpssDxChTtiIpv4GreEthTypeGet with not NULL ethTypePtr
                                                            and other params as in 1.1.
            Expected: GT_OK and same ethType as written.
        */
        st = cpssDxChTtiIpv4GreEthTypeGet(dev, greTunnelType, &ethTypeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiIpv4GreEthTypeGet: %d, %d", dev, greTunnelType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ethType, ethTypeGet,
                   "got another ethType then was set: %d", dev);

        /*
            1.3. Call with wrong enum values greTunnelType and other params as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiIpv4GreEthTypeSet
                            (dev, greTunnelType, ethType),
                            greTunnelType);
    }

    greTunnelType = CPSS_DXCH_TTI_IPV4_GRE0_E;
    ethType       = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiIpv4GreEthTypeSet(dev, greTunnelType, ethType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiIpv4GreEthTypeSet(dev, greTunnelType, ethType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiIpv4GreEthTypeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_IPV4_GRE_TYPE_ENT     greTunnelType,
    OUT GT_U32                              *ethTypePtr

)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiIpv4GreEthTypeGet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion)
    1.1. Call with greTunnelType [CPSS_DXCH_TTI_IPV4_GRE0_E /
                                  CPSS_DXCH_TTI_IPV4_GRE1_E]
                   and not NULL ethTypePtr.
    Expected: GT_OK.
    1.2. Call with wrong enum values greTunnelType
                   and other params as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with ethTypePtr [NULL]
                   and other params as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_TTI_IPV4_GRE_TYPE_ENT greTunnelType = CPSS_DXCH_TTI_IPV4_GRE0_E;
    GT_U32                          ethType       = 0;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with greTunnelType [CPSS_DXCH_TTI_IPV4_GRE0_E /
                                          CPSS_DXCH_TTI_IPV4_GRE1_E]
                               and not NULL ethTypePtr.
            Expected: GT_OK.
        */
        /* iterate with greTunnelType = CPSS_DXCH_TTI_IPV4_GRE0_E */
        greTunnelType = CPSS_DXCH_TTI_IPV4_GRE0_E;

        st = cpssDxChTtiIpv4GreEthTypeGet(dev, greTunnelType, &ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, greTunnelType);

        /* iterate with greTunnelType = CPSS_DXCH_TTI_IPV4_GRE1_E */
        greTunnelType = CPSS_DXCH_TTI_IPV4_GRE1_E;

        st = cpssDxChTtiIpv4GreEthTypeGet(dev, greTunnelType, &ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, greTunnelType);

        /*
            1.2. Call with wrong enum values greTunnelType and other params as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiIpv4GreEthTypeGet
                            (dev, greTunnelType, &ethType),
                            greTunnelType);

        /*
            1.3. Call with ethTypePtr [NULL]
                           and other params as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiIpv4GreEthTypeGet(dev, greTunnelType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ethTypePtr = NULL", dev);
    }

    greTunnelType = CPSS_DXCH_TTI_IPV4_GRE0_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiIpv4GreEthTypeGet(dev, greTunnelType, &ethType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiIpv4GreEthTypeGet(dev, greTunnelType, &ethType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiExceptionCmdSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    IN  CPSS_PACKET_CMD_ENT                 command
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiExceptionCmdSet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion)
    1.1. Call with exceptionType [CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E /
                                  CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E],
                   command [CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                            CPSS_PACKET_CMD_DROP_HARD_E].
    Expected: GT_OK.
    1.2. Call function cpssDxChTtiExceptionCmdGet with not NULL commandPtr
                                                  and other params as in 1.1.
    Expected: GT_OK and same command as written.
    1.3. Call with wrong enum values exceptionType
                   and other params as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with command [CPSS_PACKET_CMD_FORWARD_E] (not supported)
                   and other params as in 1.1.
    Expected: NOT GT_OK.
    1.5. Call with wrong enum values command
                   and other params as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_TTI_EXCEPTION_ENT exceptionType = CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E;
    CPSS_PACKET_CMD_ENT         command       = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    CPSS_PACKET_CMD_ENT         commandGet    = CPSS_PACKET_CMD_TRAP_TO_CPU_E;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with exceptionType [CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E /
                                                   CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E],
                                    command [CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                            CPSS_PACKET_CMD_DROP_HARD_E].
            Expected: GT_OK.
        */
        /* iterate with exceptionType = CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E */
        exceptionType = CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E;
        command       = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssDxChTtiExceptionCmdSet(dev, exceptionType, command);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, exceptionType, command);

        /*
            1.2. Call function cpssDxChTtiExceptionCmdGet with not NULL commandPtr
                                                          and other params as in 1.1.
            Expected: GT_OK and same command as written.
        */
        st = cpssDxChTtiExceptionCmdGet(dev, exceptionType, &commandGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiExceptionCmdGet: %d, %d", dev, exceptionType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet,
                   "got another command then was set: %d", dev);

        /* iterate with exceptionType = CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E */
        exceptionType = CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E;
        command       = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssDxChTtiExceptionCmdSet(dev, exceptionType, command);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, exceptionType, command);

        /*
            1.2. Call function cpssDxChTtiExceptionCmdGet with not NULL commandPtr
                                                          and other params as in 1.1.
            Expected: GT_OK and same command as written.
        */
        st = cpssDxChTtiExceptionCmdGet(dev, exceptionType, &commandGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiExceptionCmdGet: %d, %d", dev, exceptionType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet,
                   "got another command then was set: %d", dev);

        /*
            1.3. Call with wrong enum values exceptionType and other params as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiExceptionCmdSet
                            (dev, exceptionType, command),
                            exceptionType);

        /*
            1.4. Call with command [CPSS_PACKET_CMD_FORWARD_E] (not supported)
                           and other params as in 1.1.
            Expected: NOT GT_OK.
        */
        command = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssDxChTtiExceptionCmdSet(dev, exceptionType, command);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, command = %d", dev, command);

        /*
            1.5. Call with wrong enum values command and other params as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiExceptionCmdSet
                            (dev, exceptionType, command),
                            command);
    }

    exceptionType = CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E;
    command       = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiExceptionCmdSet(dev, exceptionType, command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiExceptionCmdSet(dev, exceptionType, command);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiExceptionCmdGet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion)
    1.1. Call function with exceptionType [CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E /
                                           CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E]
                            and not NULL commandPtr.
    Expected: GT_OK.
    1.2. Call with wrong enum values exceptionType
                   and other params as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with commandPtr [NULL]
                   and other params as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_TTI_EXCEPTION_ENT exceptionType = CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E;
    CPSS_PACKET_CMD_ENT         command       = CPSS_PACKET_CMD_TRAP_TO_CPU_E;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with exceptionType [CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E /
                                                   CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E]
                                    and not NULL commandPtr.
            Expected: GT_OK.
        */
        /* iterate with exceptionType = CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E */
        exceptionType = CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E;

        st = cpssDxChTtiExceptionCmdGet(dev, exceptionType, &command);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, exceptionType);

        /* iterate with exceptionType = CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E */
        exceptionType = CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E;

        st = cpssDxChTtiExceptionCmdGet(dev, exceptionType, &command);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, exceptionType);

        /*
            1.2. Call with wrong enum values exceptionType and other params as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiExceptionCmdGet
                            (dev, exceptionType, &command),
                            exceptionType);

        /*
            1.3. Call with commandPtr [NULL]
                            and other params as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiExceptionCmdGet(dev, exceptionType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, commandPtr = NULL", dev);
    }

    exceptionType = CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiExceptionCmdGet(dev, exceptionType, &command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiExceptionCmdGet(dev, exceptionType, &command);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiMimEthTypeSet(
    IN  GT_U8    devNum,
    IN  GT_U32   ethType
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiMimEthTypeSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (APPLICABLE DEVICES: xCat; Lion)
    1.1. Call with ethType [0 / BIT_16 - 1].
    Expected: GT_OK.
    1.2. Call cpssDxChTtiMimEthTypeGet.
    Expected: GT_OK and the same value.
    1.3. Call with ethType [BIT_16].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U32      ethType    = 0;
    GT_U32      ethTypeGet = 0;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with ethType [ 0 / BIT_16 - 1 ].   */
        /* Expected: GT_OK.                                          */
        ethType = 0;

        /*call with ethType = 0 */
        st = cpssDxChTtiMimEthTypeSet(dev, ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ethType);

        /*call with ethType = BIT_16 - 1 */
        ethType = BIT_16 - 1;

        st = cpssDxChTtiMimEthTypeSet(dev, ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ethType);

        /* 1.2. Call cpssDxChTtiMimEthTypeGet.   */
        /* Expected: GT_OK and the same value.   */

        ethType = 0;
        st = cpssDxChTtiMimEthTypeSet(dev, ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ethType);

        /*checking value */
        st = cpssDxChTtiMimEthTypeGet(dev, &ethTypeGet);

        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ethType, ethTypeGet,
                                  "got another ethType than was set: %d", dev);

        /*call with ethType = 0 */
        ethType = 0;

        st = cpssDxChTtiMimEthTypeSet(dev, ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ethType);

        /*checking value */
        st = cpssDxChTtiMimEthTypeGet(dev, &ethTypeGet);

        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ethType, ethTypeGet,
                                "got another ethType than was set: %d", dev);
        ethType = 1;

        /* 1.3. Call function for with ethType [BIT_16].   */
        /* Expected: GT_BAD_PARAM.                         */
        ethType = BIT_16;

        st = cpssDxChTtiMimEthTypeSet(dev, ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, ethType);

        ethType = 1;
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiMimEthTypeSet(dev, ethType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* ethType == GT_TRUE    */

    st = cpssDxChTtiMimEthTypeSet(dev, ethType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiMimEthTypeGet(
    IN  GT_U8    devNum,
    OUT GT_U32  *ethTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiMimEthTypeGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (APPLICABLE DEVICES: xCat; Lion)
    1.1. Call function for with non null ethType pointer.
    Expected: GT_OK.
    1.2. Call function for with null ethType pointer [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st     = GT_OK;
    GT_U8       dev;
    GT_U32      ethType;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with non null ethType pointer.   */
        /* Expected: GT_OK.                                        */

        st = cpssDxChTtiMimEthTypeGet(dev, &ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ethType);

        /* 1.2. Call function for with null ethType pointer [NULL].      */
        /* Expected: GT_BAD_PTR.                                        */
        st = cpssDxChTtiMimEthTypeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiMimEthTypeGet(dev, &ethType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiMimEthTypeGet(dev, &ethType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortMimOnlyMacToMeEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortMimOnlyMacToMeEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (APPLICABLE DEVICES: xCat; Lion)
    1.1.1. Call function with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChTtiPortMimOnlyMacToMeEnableGet function.
    Expected: GT_OK and the same value.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     enable;
    GT_BOOL     enableGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call function with enable [GT_FALSE and GT_TRUE].
                Expected: GT_OK.
            */
            enable = GT_FALSE;
            st = cpssDxChTtiPortMimOnlyMacToMeEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            enable = GT_TRUE;
            st = cpssDxChTtiPortMimOnlyMacToMeEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssDxChTtiPortMimOnlyMacToMeEnableGet function.
                Expected: GT_OK and the same value.
            */
            st = cpssDxChTtiPortMimOnlyMacToMeEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "got another enable then was set: %d", dev);
        }

        /* 1.2. Check that function handles CPU port    */
        port   = PRV_CPSS_DXCH_CPU_PORT_NUM_CNS;
        enable = GT_TRUE;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxChTtiPortMimOnlyMacToMeEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxChTtiPortMimOnlyMacToMeEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiPortMimOnlyMacToMeEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;
    enable = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortMimOnlyMacToMeEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortMimOnlyMacToMeEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortMimOnlyMacToMeEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortMimOnlyMacToMeEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (APPLICABLE DEVICES: xCat; Lion)
    1.1.1. Call function with correct enable pointer.
    Expected: GT_OK.
    1.1.2. Call function with incorrect enable pointer [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     enable;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with correct enable pointer.
            Expected: GT_OK. */

            st = cpssDxChTtiPortMimOnlyMacToMeEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*1.1.2. Call function with incorrect enable pointer [NULL].
            Expected: GT_BAD_PTR. */

            st = cpssDxChTtiPortMimOnlyMacToMeEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PTR, st, dev, port, enable);
        }

        /* 1.2. Check that function handles CPU port    */
        port   = PRV_CPSS_DXCH_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxChTtiPortMimOnlyMacToMeEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxChTtiPortMimOnlyMacToMeEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTtiPortMimOnlyMacToMeEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortMimOnlyMacToMeEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortMimOnlyMacToMeEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  GT_BOOL                             valid
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiRuleValidStatusSet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion; xCat2)
    1.1. Call function with routerTtiTcamRow [0], valid [GT_FALSE / GT_TRUE]
    Expected: GT_OK.
    1.2. Call cpssDxChTtiRuleValidStatusGet with non-NULL validPtr
                           and the same routerTtiTcamRow as in 1.1.
    Expected: GT_OK and same value as written.
    1.3. Call with out of range routerTtiTcamRow [_3K]
                   and other params same as in 1.1.
    Expected: GT_OK for DxCh3 and GT_BAD_PARAM for DxCh, DxCh2.
    1.4. Call with out of range routerTtiTcamRow [_5K]
                   and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      routerTtiTcamRow = 0;
    GT_BOOL     valid            = GT_FALSE;
    GT_BOOL     validGet         = GT_TRUE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with routerTtiTcamRow [0],
                                    valid [GT_FALSE / GT_TRUE]
            Expected: GT_OK.
        */
        routerTtiTcamRow = 0;
        valid = GT_FALSE;

        st = cpssDxChTtiRuleValidStatusSet(dev, routerTtiTcamRow, valid);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, routerTtiTcamRow, valid);

        valid = GT_FALSE;

        st = cpssDxChTtiRuleValidStatusSet(dev, routerTtiTcamRow, valid);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, routerTtiTcamRow, valid);

        /*
            1.2. Call cpssDxChTtiRuleValidStatusGet with non-NULL validPtr
                                            and the same routerTtiTcamRow as in 1.1.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxChTtiRuleValidStatusGet(dev, routerTtiTcamRow, &validGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiRuleValidStatusGet: %d, %d", dev, routerTtiTcamRow);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet,
                   "got another valid then was set: %d", dev);

        /*
            1.3. Call with out of range routerTtiTcamRow [_3K]
                           and other params same as in 1.1.
            Expected: GT_OK for DxCh3 and GT_BAD_PARAM for DxCh, DxCh2.
        */
        routerTtiTcamRow = _3K; /* _3K - out of range */

        st = cpssDxChTtiRuleValidStatusSet(dev, routerTtiTcamRow, valid);
        if ((CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily)||
            (CPSS_PP_FAMILY_CHEETAH2_E == PRV_CPSS_PP_MAC(dev)->devFamily)||
            (CPSS_PP_FAMILY_DXCH_XCAT2_E == PRV_CPSS_PP_MAC(dev)->devFamily))
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, routerTtiTcamRow);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, routerTtiTcamRow);
        }

        /*
            1.4. Call with out of range routerTtiTcamRow [_5K]
                           and other params same as in 1.1.
            Expected: GT_BAD_PARAM
        */
        routerTtiTcamRow = _5K; /* _5K - out of range */

        st = cpssDxChTtiRuleValidStatusSet(dev, routerTtiTcamRow, valid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, routerTtiTcamRow);

        routerTtiTcamRow = 0;
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiRuleValidStatusSet( dev, routerTtiTcamRow, valid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiRuleValidStatusSet( dev, routerTtiTcamRow, valid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiRuleValidStatusGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        routerTtiTcamRow,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *validPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiRuleValidStatusGet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion; xCat2)
    1.1. Call with routerTtiTcamRow [0], non-NULL validPtr.
    Expected: GT_OK.
    1.2. Call with out of range routerTtiTcamRow [_3K]
                   and other params as in 1.1.
    Expected: GT_OK for DxCh3 and GT_BAD_PARAM for DxCh,DxCh2.
    1.3. Call with out of range routerTtiTcamRow [_5K]
                   and other params as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with validPtr [NULL] and other params as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      routerTtiTcamRow = 0;
    GT_BOOL     valid            = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with routerTtiTcamRow [0], non-NULL validPtr.
            Expected: GT_OK.
        */
        routerTtiTcamRow = 0;

        st = cpssDxChTtiRuleValidStatusGet(dev, routerTtiTcamRow, &valid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, routerTtiTcamRow);

        /*
            1.2. Call with out of range routerTtiTcamRow [_3K]
                           and other params same as in 1.1.
            Expected: GT_OK for DxCh3 and GT_BAD_PARAM for DxCh, DxCh2.
        */
        routerTtiTcamRow = _3K; /* _3K - out of range */

        st = cpssDxChTtiRuleValidStatusGet(dev, routerTtiTcamRow, &valid);
        if ((CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily)||
            (CPSS_PP_FAMILY_CHEETAH2_E == PRV_CPSS_PP_MAC(dev)->devFamily)||
            (CPSS_PP_FAMILY_DXCH_XCAT2_E == PRV_CPSS_PP_MAC(dev)->devFamily))
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, routerTtiTcamRow);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, routerTtiTcamRow);
        }

        /*
            1.3. Call with out of range routerTtiTcamRow [_5K]
                           and other params same as in 1.1.
            Expected: GT_BAD_PARAM
        */
        routerTtiTcamRow = _5K; /* _5K - out of range */

        st = cpssDxChTtiRuleValidStatusGet(dev, routerTtiTcamRow, &valid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, routerTtiTcamRow);


        routerTtiTcamRow = 0;

        /*
            1.4. Call with validPtr [NULL] and other params as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTtiRuleValidStatusGet(dev, routerTtiTcamRow, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiRuleValidStatusGet(dev, routerTtiTcamRow, &valid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiRuleValidStatusGet(dev, routerTtiTcamRow, &valid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiMplsEthTypeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet,
    IN  GT_U32                          ethType
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiMplsEthTypeSet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion)
    1.1. Call function with ucMcSet [CPSS_IP_UNICAST_E / CPSS_IP_MULTICAST_E],
    and ethType [ 1 / BIT_16 - 1 ].
    Expected: GT_OK.
    1.2. Call cpssDxChTtiMplsEthTypeGet with non-NULL ethTypePtr
    and the same ucMcSet as in 1.1.
    Expected: GT_OK and same value as written.
    1.3. Call with wrong enum values ucMcSet
    and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range ethType [BIT_16]
    and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet = CPSS_IP_UNICAST_E;
    GT_U32                          ethType = 0;
    GT_U32                          ethTypeGet;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with ucMcSet [CPSS_IP_UNICAST_E / CPSS_IP_MULTICAST_E],
            and ethType [ 1 / BIT_16 - 1 ].
            Expected: GT_OK.
        */

        /* Call with ucMcSet [CPSS_IP_UNICAST_E] and ethType [1] */

        ucMcSet = CPSS_IP_UNICAST_E;
        ethType = 1;

        st = cpssDxChTtiMplsEthTypeSet(dev, ucMcSet, ethType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcSet, ethType);

        /*
            1.2. Call cpssDxChTtiMplsEthTypeGet with non-NULL ethTypePtr
            and the same ucMcSet as in 1.1.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxChTtiMplsEthTypeGet(dev, ucMcSet, &ethTypeGet);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiMplsEthTypeGet: %d, %d", dev, ucMcSet);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ethType, ethTypeGet,
                   "got another ethType then was set: %d", dev);


        /* Call with ucMcSet [CPSS_IP_MULTICAST_E] and ethType [1] */

        ucMcSet = CPSS_IP_MULTICAST_E;

        st = cpssDxChTtiMplsEthTypeSet(dev, ucMcSet, ethType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcSet, ethType);

        /*
            1.2. Call cpssDxChTtiMplsEthTypeGet with non-NULL ethTypePtr
            and the same ucMcSet as in 1.1.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxChTtiMplsEthTypeGet(dev, ucMcSet, &ethTypeGet);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiMplsEthTypeGet: %d, %d", dev, ucMcSet);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ethType, ethTypeGet,
                   "got another ethType then was set: %d", dev);


        /* Call with ucMcSet [CPSS_IP_UNICAST_E] and ethType [BIT_16 - 1] */

        ucMcSet = CPSS_IP_UNICAST_E;
        ethType = BIT_16 - 1;

        st = cpssDxChTtiMplsEthTypeSet(dev, ucMcSet, ethType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcSet, ethType);

        /*
            1.2. Call cpssDxChTtiMplsEthTypeGet with non-NULL ethTypePtr
            and the same ucMcSet as in 1.1.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxChTtiMplsEthTypeGet(dev, ucMcSet, &ethTypeGet);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiMplsEthTypeGet: %d, %d", dev, ucMcSet);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ethType, ethTypeGet,
                   "got another ethType then was set: %d", dev);


        /* Call with ucMcSet [CPSS_IP_MULTICAST_E] and ethType [BIT_16 - 1] */

        ucMcSet = CPSS_IP_MULTICAST_E;

        st = cpssDxChTtiMplsEthTypeSet(dev, ucMcSet, ethType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcSet, ethType);

        /*
            1.2. Call cpssDxChTtiMplsEthTypeGet with non-NULL ethTypePtr
            and the same ucMcSet as in 1.1.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxChTtiMplsEthTypeGet(dev, ucMcSet, &ethTypeGet);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiMplsEthTypeGet: %d, %d", dev, ucMcSet);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ethType, ethTypeGet,
                   "got another ethType then was set: %d", dev);

        /*
            1.3. Call with wrong enum values ucMcSet and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiMplsEthTypeSet
                            (dev, ucMcSet, ethType),
                            ucMcSet);

        /*
            1.4. Call with out of range ethType [BIT_16]
            and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */

        ethType = BIT_16;

        st = cpssDxChTtiMplsEthTypeSet(dev, ucMcSet, ethType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, ucMcSet, ethType);

        ethType = 1;
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiMplsEthTypeSet(dev, ucMcSet, ethType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiMplsEthTypeSet(dev, ucMcSet, ethType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiMplsEthTypeGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet,
    OUT GT_U32                          *ethTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiMplsEthTypeGet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion)
    1.1. Call function with ucMcSet [CPSS_IP_UNICAST_E / CPSS_IP_MULTICAST_E].
    Expected: GT_OK.
    1.3. Call with wrong enum values ucMcSet.
    Expected: GT_BAD_PARAM.
    1.4. Call with bad ethTypePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet = CPSS_IP_UNICAST_E;
    GT_U32                          ethTypeGet;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with ucMcSet [CPSS_IP_UNICAST_E / CPSS_IP_MULTICAST_E].
            Expected: GT_OK.
        */

        ucMcSet = CPSS_IP_UNICAST_E;

        st = cpssDxChTtiMplsEthTypeGet(dev, ucMcSet, &ethTypeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiMplsEthTypeGet: %d, %d", dev, ucMcSet);

        /* Call with ucMcSet [CPSS_IP_MULTICAST_E] */

        ucMcSet = CPSS_IP_MULTICAST_E;

        st = cpssDxChTtiMplsEthTypeGet(dev, ucMcSet, &ethTypeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiMplsEthTypeGet: %d, %d", dev, ucMcSet);

        /*
            1.3. Call with wrong enum values ucMcSet.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiMplsEthTypeGet
                            (dev, ucMcSet, &ethTypeGet),
                            ucMcSet);

        /*
            1.4. Call with bad ethTypePtr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChTtiMplsEthTypeGet(dev, ucMcSet, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ethType = NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiMplsEthTypeGet(dev, ucMcSet, &ethTypeGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiMplsEthTypeGet(dev, ucMcSet, &ethTypeGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPclIdSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType,
    IN  GT_U32                          pclId
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPclIdSet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion; xCat2)
    1.1. Call function with keyType [CPSS_DXCH_TTI_KEY_IPV4_E /
                                     CPSS_DXCH_TTI_KEY_MPLS_E /
                                     CPSS_DXCH_TTI_KEY_ETH_E  /
                                     CPSS_DXCH_TTI_KEY_MIM_E],
                          and pclId [ 1 / BIT_10 - 1 ].
    Expected: GT_OK.
    1.2. Call cpssDxChTtiPclIdGet with non-NULL pclIdPtr
    and the same keyType as in 1.1.
    Expected: GT_OK and same value as written.
    1.3. Call with wrong enum values keyType
    and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range pclId [BIT_10] and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType = 0;
    GT_U32                          pclId = 0;
    GT_U32                          pclIdGet = 1;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with keyType [CPSS_DXCH_TTI_KEY_IPV4_E /
                                             CPSS_DXCH_TTI_KEY_MPLS_E /
                                             CPSS_DXCH_TTI_KEY_ETH_E  /
                                             CPSS_DXCH_TTI_KEY_MIM_E],
                                  and pclId [ 1 / BIT_10 - 1 ].
            Expected: GT_OK.
        */

        /* Call with keyType [CPSS_DXCH_TTI_KEY_IPV4_E] and pclId [1] */

        keyType = CPSS_DXCH_TTI_KEY_IPV4_E;
        pclId = 1;

        st = cpssDxChTtiPclIdSet(dev, keyType, pclId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, keyType, pclId);

        /*
            1.2. Call cpssDxChTtiPclIdGet with non-NULL pclIdPtr
            and the same keyType as in 1.1.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxChTtiPclIdGet(dev, keyType, &pclIdGet);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssDxChTtiPclIdGet: %d, %d", dev, keyType);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(pclId, pclIdGet,
                   "got another pclId then was set: %d", dev);


        /* Call with keyType [CPSS_DXCH_TTI_KEY_MPLS_E] and pclId [1] */

        keyType = CPSS_DXCH_TTI_KEY_MPLS_E;
        pclId = 1;

        st = cpssDxChTtiPclIdSet(dev, keyType, pclId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, keyType, pclId);

        /*
            1.2. Call cpssDxChTtiPclIdGet with non-NULL pclIdPtr
            and the same keyType as in 1.1.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxChTtiPclIdGet(dev, keyType, &pclIdGet);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssDxChTtiPclIdGet: %d, %d", dev, keyType);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(pclId, pclIdGet,
                   "got another pclId then was set: %d", dev);

        /* Call with keyType [CPSS_DXCH_TTI_KEY_ETH_E] and pclId [BIT_10 - 1] */

        keyType = CPSS_DXCH_TTI_KEY_ETH_E;
        pclId = BIT_10 - 1;

        st = cpssDxChTtiPclIdSet(dev, keyType, pclId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, keyType, pclId);

        /*
            1.2. Call cpssDxChTtiPclIdGet with non-NULL pclIdPtr
            and the same keyType as in 1.1.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxChTtiPclIdGet(dev, keyType, &pclIdGet);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssDxChTtiPclIdGet: %d, %d", dev, keyType);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(pclId, pclIdGet,
                   "got another pclId then was set: %d", dev);

        /* Call with keyType [CPSS_DXCH_TTI_KEY_MIM_E] and pclId [BIT_10 - 1] */

        keyType = CPSS_DXCH_TTI_KEY_MIM_E;
        pclId = BIT_10 - 1;

        st = cpssDxChTtiPclIdSet(dev, keyType, pclId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, keyType, pclId);

        /*
            1.2. Call cpssDxChTtiPclIdGet with non-NULL pclIdPtr
            and the same keyType as in 1.1.
            Expected: GT_OK and same value as written.
        */
        st = cpssDxChTtiPclIdGet(dev, keyType, &pclIdGet);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssDxChTtiPclIdGet: %d, %d", dev, keyType);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(pclId, pclIdGet,
                   "got another pclId then was set: %d", dev);

        /*
            1.3. Call with wrong enum values keyType and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiPclIdSet
                            (dev, keyType, pclId),
                            keyType);

        /*
            1.4. Call with out of range pclId [BIT_10]
            and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */

        pclId = BIT_10;

        st = cpssDxChTtiPclIdSet(dev, keyType, pclId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, keyType, pclId);

        pclId = 1;
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPclIdSet(dev, keyType, pclId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPclIdSet(dev, keyType, pclId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPclIdGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType,
    OUT GT_U32                          *pclIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPclIdGet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: xCat; Lion; xCat2)
    1.1. Call function with keyType [CPSS_DXCH_TTI_KEY_IPV4_E /
                                     CPSS_DXCH_TTI_KEY_MPLS_E /
                                     CPSS_DXCH_TTI_KEY_ETH_E  /
                                     CPSS_DXCH_TTI_KEY_MIM_E],
                        and non null pclIdPtr.
    Expected: GT_OK.
    1.2. Call with wrong enum values keyType
    and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong pclIdPtr [NULL] and other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_TTI_KEY_TYPE_ENT keyType = 0;
    GT_U32                     pclId = 1;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. Call function with keyType [CPSS_DXCH_TTI_KEY_IPV4_E /
                                             CPSS_DXCH_TTI_KEY_MPLS_E /
                                             CPSS_DXCH_TTI_KEY_ETH_E  /
                                             CPSS_DXCH_TTI_KEY_MIM_E],
                        and non null pclIdPtr.
            Expected: GT_OK.
        */

        /* Call with keyType [CPSS_DXCH_TTI_KEY_IPV4_E]*/
        keyType = CPSS_DXCH_TTI_KEY_IPV4_E;

        st = cpssDxChTtiPclIdGet(dev, keyType, &pclId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, keyType, pclId);

        /* Call with keyType [CPSS_DXCH_TTI_KEY_MPLS_E]*/
        keyType = CPSS_DXCH_TTI_KEY_MPLS_E;

        st = cpssDxChTtiPclIdGet(dev, keyType, &pclId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, keyType, pclId);

        /* Call with keyType [CPSS_DXCH_TTI_KEY_ETH_E]*/
        keyType = CPSS_DXCH_TTI_KEY_ETH_E;

        st = cpssDxChTtiPclIdGet(dev, keyType, &pclId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, keyType, pclId);

        /* Call with keyType [CPSS_DXCH_TTI_KEY_MIM_E]*/
        keyType = CPSS_DXCH_TTI_KEY_MIM_E;

        st = cpssDxChTtiPclIdGet(dev, keyType, &pclId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, keyType, pclId);


        /*
            1.2. Call with wrong enum values keyType and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTtiPclIdGet
                            (dev, keyType, &pclId),
                            keyType);

        /*
            1.3. Call with wrong pclIdPtr [NULL] and other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChTtiPclIdGet(dev, keyType, NULL);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PTR, st, dev, keyType, pclId);

    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPclIdGet(dev, keyType, &pclId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPclIdGet(dev, keyType, &pclId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill table MacToMe
*/
UTF_TEST_CASE_MAC(cpssDxChTtiFillMacToMeTable)
{
/*
    ITERATE_DEVICE (APPLICABLE DEVICES: DxCh3; xCat; Lion; xCat2)
    1.1. Get table Size.
         Call cpssDxChCfgTableNumEntriesGet with table [CPSS_DXCH_CFG_TABLE_MAC_TO_ME_E]
                                                 and non-NULL numEntriesPtr.
    Expected: GT_OK.
    1.2. Fill all entries in MacToMe table.
         Call cpssDxChTtiMacToMeSet with entryIndex [0... numEntries-1],
                                    valuePtr {mac [[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA]],
                                              vlanId [100]},
                                    maskPtr {mac [[0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]],
                                              vlanId [0x0FFF]}
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssDxChTtiMacToMeSet with entryIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in MacToMe table and compare with original.
         Call function cpssDxChTtiMacToMeGet with not-NULL valuePtr,
                                                  not-NULL maskPtr
                                                  and other parameter from 1.1.
    Expected: GT_OK and value, mask the same as was set.
    1.5. Try to read entry with index out of range.
         Call function cpssDxChTtiMacToMeGet with entryIndex [numEntries].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;
    GT_BOOL     isEqual    = GT_FALSE;

    CPSS_DXCH_TTI_MAC_VLAN_STC  value;
    CPSS_DXCH_TTI_MAC_VLAN_STC  mask;
    CPSS_DXCH_TTI_MAC_VLAN_STC  valueGet;
    CPSS_DXCH_TTI_MAC_VLAN_STC  maskGet;


    cpssOsBzero((GT_VOID*) &valueGet, sizeof(valueGet));
    cpssOsBzero((GT_VOID*) &maskGet, sizeof(maskGet));
    cpssOsBzero((GT_VOID*) &value, sizeof(value));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));

    /* Fill the entry for MacToMe table */
    value.mac.arEther[0] = 0xAA;
    value.mac.arEther[1] = 0xAA;
    value.mac.arEther[2] = 0xAA;
    value.mac.arEther[3] = 0xAA;
    value.mac.arEther[4] = 0xAA;
    value.mac.arEther[5] = 0xAA;

    value.vlanId = 100;

    mask.mac.arEther[0] = 0xFF;
    mask.mac.arEther[1] = 0xFF;
    mask.mac.arEther[2] = 0xFF;
    mask.mac.arEther[3] = 0xFF;
    mask.mac.arEther[4] = 0xFF;
    mask.mac.arEther[5] = 0xFF;

    mask.vlanId = 0x0FFF;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get MacToMe Size */
        st = cpssDxChCfgTableNumEntriesGet(dev, CPSS_DXCH_CFG_TABLE_MAC_TO_ME_E, &numEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChCfgTableNumEntriesGet: %d", dev);

        /* 1.2. Fill all entries in MacToMe table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            value.mac.arEther[2] = (GT_U8)(iTemp % 255);
            value.mac.arEther[3] = (GT_U8)(iTemp % 255);
            value.mac.arEther[4] = (GT_U8)(iTemp % 255);
            value.mac.arEther[5] = (GT_U8)(iTemp % 255);
            value.vlanId         = (GT_U16)(iTemp % 4095);

            st = cpssDxChTtiMacToMeSet(dev, iTemp, &value, &mask);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTtiMacToMeSet: %d, %d", dev, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssDxChTtiMacToMeSet(dev, numEntries, &value, &mask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTtiMacToMeSet: %d, %d", dev, numEntries);

        /* 1.4. Read all entries in MacToMe table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            value.mac.arEther[2] = (GT_U8)(iTemp % 255);
            value.mac.arEther[3] = (GT_U8)(iTemp % 255);
            value.mac.arEther[4] = (GT_U8)(iTemp % 255);
            value.mac.arEther[5] = (GT_U8)(iTemp % 255);
            value.vlanId         = (GT_U16)(iTemp % 4095);

            st = cpssDxChTtiMacToMeGet(dev, iTemp, &valueGet, &maskGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTtiMacToMeGet: %d, %d", dev, iTemp);

            /* validation values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &value,
                                         (GT_VOID*) &valueGet,
                                         sizeof(value))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another value then was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask,
                                         (GT_VOID*) &maskGet,
                                         sizeof(mask))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another mask then was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssDxChTtiMacToMeGet(dev, numEntries, &valueGet, &maskGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTtiMacToMeGet: %d, %d", dev, numEntries);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortGroupMacToMeSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *valuePtr,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *maskPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortGroupMacToMeSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (APPLICABLE DEVICES: DxCh3; xCat; Lion; xCat2)
    1.1.1. Call function with entryIndex [0 / 7],
                            valuePtr {mac [[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA]],
                                      vlanId [100]},
                            maskPtr {mac [[0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]],
                                      vlanId [0x0FFF]}
    Expected: GT_OK.
    1.1.2. Call function cpssDxChIpPortGroupCntGet with not-NULL valuePtr,
                                                   not-NULL maskPtr
                                                   and other parameter from 1.1.1.
    Expected: GT_OK and value, mask the same as was set.
    1.1.3. Call function with out of range entryIndex [8]
                              and other parameter from 1.1.1.
    Expected: NOT GT_OK.
    1.1.4. Call function with valuePtr [NULL]
                              and other parameter from 1.1.1.
    Expected: GT_BAD_PTR.
    1.1.5. Call function with maskPtr [NULL]
                              and other parameter from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    GT_U32                      entryIndex = 0;
    GT_BOOL                     isEqual  = GT_FALSE;
    CPSS_DXCH_TTI_MAC_VLAN_STC  value    = {{{0}}, 0};
    CPSS_DXCH_TTI_MAC_VLAN_STC  mask     = {{{0}}, 0};
    CPSS_DXCH_TTI_MAC_VLAN_STC  valueGet = {{{0}}, 0};
    CPSS_DXCH_TTI_MAC_VLAN_STC  maskGet  = {{{0}}, 0};


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call function with entryIndex [0 / 7],
                                        valuePtr {mac [[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA]],
                                                  vlanId [100]},
                                        maskPtr {mac [[0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]],
                                                 vlanId [0x0FFF]}
                Expected: GT_OK.
            */
            /* iterate with entryIndex = 0 */
            entryIndex = 0;

            cpssOsMemSet(value.mac.arEther, 0xAA, sizeof(value.mac.arEther));
            value.vlanId = 100;

            cpssOsMemSet(mask.mac.arEther, 0xFF, sizeof(mask.mac.arEther));
            mask.vlanId = 0x0FFF;

            st = cpssDxChTtiPortGroupMacToMeSet(dev, portGroupsBmp, entryIndex, &value, &mask);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

            /*
                1.2. Call function cpssDxChTtiPortGroupMacToMeGet with not-NULL valuePtr,
                                                              not-NULL maskPtr
                                                              and other parameter from 1.1.1.
                Expected: GT_OK and value, mask the same as was set.
            */
            st = cpssDxChTtiPortGroupMacToMeGet(dev, portGroupsBmp, entryIndex, &valueGet, &maskGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChTtiPortGroupMacToMeGet: %d, %d", dev, entryIndex);

            /* validation values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &value,
                                         (GT_VOID*) &valueGet,
                                         sizeof(value))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another value then was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask,
                                         (GT_VOID*) &maskGet,
                                         sizeof(mask))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another mask then was set: %d", dev);

            /* iterate with entryIndex = 7 */
            entryIndex = 7;

            st = cpssDxChTtiPortGroupMacToMeSet(dev, portGroupsBmp, entryIndex, &value, &mask);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

            /*
                1.2. Call function cpssDxChTtiPortGroupMacToMeGet with not-NULL valuePtr,
                                                              not-NULL maskPtr
                                                              and other parameter from 1.1.1.
                Expected: GT_OK and value, mask the same as was set.
            */
            st = cpssDxChTtiPortGroupMacToMeGet(dev, portGroupsBmp, entryIndex, &valueGet, &maskGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChTtiPortGroupMacToMeGet: %d, %d", dev, entryIndex);

            /* validation values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &value,
                                         (GT_VOID*) &valueGet,
                                         sizeof(value))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another value then was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask,
                                         (GT_VOID*) &maskGet,
                                         sizeof(mask))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another mask then was set: %d", dev);

            /*
                1.3. Call function with out of range entryIndex [8]
                                        and other parameter from 1.1.1.
                Expected: NOT GT_OK.
            */
            entryIndex = 8;

            st = cpssDxChTtiPortGroupMacToMeSet(dev, portGroupsBmp, entryIndex, &value, &mask);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

            entryIndex = 0;

            /*
                1.4. Call function with valuePtr [NULL]
                                        and other parameter from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortGroupMacToMeSet(dev, portGroupsBmp, entryIndex, NULL, &mask);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, valuePtr = NULL", dev);

            /*
                1.5. Call function with maskPtr [NULL]
                                        and other parameter from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortGroupMacToMeSet(dev, portGroupsBmp, entryIndex, &value, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChTtiPortGroupMacToMeSet(dev, portGroupsBmp, entryIndex, &value, &mask);

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

        st = cpssDxChTtiPortGroupMacToMeSet(dev, portGroupsBmp, entryIndex, &value, &mask);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    entryIndex = 0;

    cpssOsMemSet(value.mac.arEther, 0xAA, sizeof(value.mac.arEther));
    value.vlanId = 100;

    cpssOsMemSet(mask.mac.arEther, 0xFF, sizeof(mask.mac.arEther));
    mask.vlanId = 0x0FFF;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortGroupMacToMeSet(dev, portGroupsBmp, entryIndex, &value, &mask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortGroupMacToMeSet(dev, portGroupsBmp, entryIndex, &value, &mask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortGroupMacToMeGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *valuePtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *maskPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortGroupMacToMeGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (APPLICABLE DEVICES: DxCh3; xCat; Lion; xCat2)
    1.1.1. Call function with entryIndex [0 / 7],
                            not-NULL valuePtr,
                            not-NULL maskPtr.
    Expected: GT_OK.
    1.1.2. Call function with out of range entryIndex [8]
                            and other parameter from 1.1.1.
    Expected: NOT GT_OK.
    1.1.3. Call function with valuePtr [NULL]
                            and other parameter from 1.1.1.
    Expected: GT_BAD_PTR.
    1.1.4. Call function with maskPtr [NULL]
                            and other parameter from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    GT_U32                      entryIndex = 0;
    CPSS_DXCH_TTI_MAC_VLAN_STC  value = {{{0}}, 0};
    CPSS_DXCH_TTI_MAC_VLAN_STC  mask  = {{{0}}, 0};


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call function with entryIndex [0 / 7],
                                        not-NULL valuePtr,
                                        not-NULL maskPtr.
                Expected: GT_OK.
            */
            /* iterate with entryIndex = 0 */
            entryIndex = 0;

            st = cpssDxChTtiPortGroupMacToMeGet(dev, portGroupsBmp, entryIndex, &value, &mask);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

            /* iterate with entryIndex = 7 */
            entryIndex = 7;

            st = cpssDxChTtiPortGroupMacToMeGet(dev, portGroupsBmp, entryIndex, &value, &mask);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

            /*
                1.1.2. Call function with out of range entryIndex [8]
                                        and other parameter from 1.1.1.
                Expected: NOT GT_OK.
            */
            entryIndex = 8;

            st = cpssDxChTtiPortGroupMacToMeGet(dev, portGroupsBmp, entryIndex, &value, &mask);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_HW_ERROR, st, dev, entryIndex);

            entryIndex = 0;

            /*
                1.1.3. Call function with valuePtr [NULL]
                                        and other parameter from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortGroupMacToMeGet(dev, portGroupsBmp, entryIndex, NULL, &mask);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, valuePtr = NULL", dev);

            /*
                1.1.4. Call function with maskPtr [NULL]
                                        and other parameter from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortGroupMacToMeGet(dev, portGroupsBmp, entryIndex, &value, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChTtiPortGroupMacToMeGet(dev, portGroupsBmp, entryIndex, &value, &mask);

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

        st = cpssDxChTtiPortGroupMacToMeGet(dev, portGroupsBmp, entryIndex, &value, &mask);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    entryIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortGroupMacToMeGet(dev, portGroupsBmp, entryIndex, &value, &mask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortGroupMacToMeGet(dev, portGroupsBmp, entryIndex, &value, &mask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortGroupMacModeSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_DXCH_TTI_MAC_MODE_ENT        macMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortGroupMacModeSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1.1. Call function with keyType[CPSS_DXCH_TTI_KEY_IPV4_E /
                                    CPSS_DXCH_TTI_KEY_MPLS_E /
                                    CPSS_DXCH_TTI_KEY_ETH_E],
                            macMode [CPSS_DXCH_TTI_MAC_MODE_DA_E /
                                     CPSS_DXCH_TTI_MAC_MODE_SA_E]
    Expected: GT_OK.
    1.1.2. Call cpssDxChTtiPortGroupMacModeGet with non-NULL modePtr
                                    and the same keyType as in 1.1.1.
    Expected: GT_OK and same value as written.
    1.1.3. Call with wrong enum values keyType
                     and other params same as in 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.4. Call with wrong enum values macMode
                     and other params same as in 1.1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType    = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_MAC_MODE_ENT  macMode    = CPSS_DXCH_TTI_MAC_MODE_DA_E;
    CPSS_DXCH_TTI_MAC_MODE_ENT  macModeGet = CPSS_DXCH_TTI_MAC_MODE_DA_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call function with keyType[CPSS_DXCH_TTI_KEY_IPV4_E /
                                                CPSS_DXCH_TTI_KEY_MPLS_E],
                                        macMode [CPSS_DXCH_TTI_MAC_MODE_DA_E /
                                                 CPSS_DXCH_TTI_MAC_MODE_SA_E]
                Expected: GT_OK.
            */

            /* iterate with keyType = CPSS_DXCH_TTI_KEY_IPV4_E */
            keyType = CPSS_DXCH_TTI_KEY_IPV4_E;
            macMode = CPSS_DXCH_TTI_MAC_MODE_DA_E;

            st = cpssDxChTtiPortGroupMacModeSet(dev, portGroupsBmp, keyType, macMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, keyType, macMode);

            /*
                1.1.2. Call cpssDxChTtiPortGroupMacModeGet with non-NULL modePtr
                                                and the same keyType as in 1.1.1.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxChTtiPortGroupMacModeGet(dev, portGroupsBmp, keyType, &macModeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChTtiPortGroupMacModeGet: %d, %d", dev, keyType);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(macMode, macModeGet,
                       "got another macMode then was set: %d", dev);

            /* iterate with keyType = CPSS_DXCH_TTI_KEY_MPLS_E */
            keyType = CPSS_DXCH_TTI_KEY_MPLS_E;
            macMode = CPSS_DXCH_TTI_MAC_MODE_SA_E;

            st = cpssDxChTtiPortGroupMacModeSet(dev, portGroupsBmp, keyType, macMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, keyType, macMode);

            /*
                1.1.2. Call cpssDxChTtiPortGroupMacModeGet with non-NULL modePtr
                                                and the same keyType as in 1.1.1.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxChTtiPortGroupMacModeGet(dev, portGroupsBmp, keyType, &macModeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChTtiPortGroupMacModeGet: %d, %d", dev, keyType);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(macMode, macModeGet,
                       "got another macMode then was set: %d", dev);

            /*
                1.1.3. Call with wrong enum values keyType
                                 and other params same as in 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupMacModeSet
                                (dev, portGroupsBmp, keyType, macMode),
                                keyType);

            /*
                1.1.4. Call with wrong enum values macMode
                                 and other params same as in 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupMacModeSet
                                (dev, portGroupsBmp, keyType, macMode),
                                macMode);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChTtiPortGroupMacModeSet(dev, portGroupsBmp, keyType, macMode);

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

        st = cpssDxChTtiPortGroupMacModeSet(dev, portGroupsBmp, keyType, macMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    keyType = CPSS_DXCH_TTI_KEY_IPV4_E;
    macMode = CPSS_DXCH_TTI_MAC_MODE_DA_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortGroupMacModeSet(dev, portGroupsBmp, keyType, macMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortGroupMacModeSet(dev, portGroupsBmp, keyType, macMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortGroupMacModeGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *macModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortGroupMacModeGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1.1. Call with keyType[CPSS_DXCH_TTI_KEY_IPV4_E /
                           CPSS_DXCH_TTI_KEY_MPLS_E],
                   non-NULL macModePtr.
    Expected: GT_OK.
    1.1.2. Call with wrong enum values keyType
                     and other params as in 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with macModePtr [NULL]
                     and other params as in 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_MAC_MODE_ENT  macMode = CPSS_DXCH_TTI_MAC_MODE_DA_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with keyType[CPSS_DXCH_TTI_KEY_IPV4_E /
                                       CPSS_DXCH_TTI_KEY_MPLS_E],
                               non-NULL macModePtr.
                Expected: GT_OK.
            */

            /* iterate with keyType = CPSS_DXCH_TTI_KEY_IPV4_E */
            keyType = CPSS_DXCH_TTI_KEY_IPV4_E;

            st = cpssDxChTtiPortGroupMacModeGet(dev, portGroupsBmp, keyType, &macMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, keyType);

            /* iterate with keyType = CPSS_DXCH_TTI_KEY_MPLS_E */
            keyType = CPSS_DXCH_TTI_KEY_MPLS_E;

            st = cpssDxChTtiPortGroupMacModeGet(dev, portGroupsBmp, keyType, &macMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, keyType);

            /*
                1.1.2. Call with wrong enum values keyType and other params as in 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupMacModeGet
                                (dev, portGroupsBmp, keyType, &macMode),
                                keyType);

            /*
                1.1.2. Call with macModePtr [NULL]
                               and other params as in 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortGroupMacModeGet(dev, portGroupsBmp, keyType, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macModePtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChTtiPortGroupMacModeGet(dev, portGroupsBmp, keyType, &macMode);

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

        st = cpssDxChTtiPortGroupMacModeGet(dev, portGroupsBmp, keyType, &macMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    keyType = CPSS_DXCH_TTI_KEY_IPV4_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortGroupMacModeGet(dev, portGroupsBmp, keyType, &macMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortGroupMacModeGet(dev, portGroupsBmp, keyType, &macMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortGroupRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortGroupRuleSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1.1. Call with routerTtiTcamRow [0],
                   keyType [CPSS_DXCH_TTI_KEY_IPV4_E],
                   patternPtr->ipv4{common{pclId[1],
                                           srcIsTrunk[GT_FALSE],
                                           srcPortTrunk[23],
                                           mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                           vid[100],
                                           isTagged[GT_TRUE],
                                           dsaSrcIsTrunk[GT_FALSE],
                                           dsaSrcPortTrunk[0],
                                           dsaSrcDevice[dev] },
                                    tunneltype [IPv4_OVER_IPv4],
                                    srcIp[192.168.0.1],
                                    destIp[192.168.0.2],
                                    isArp[GT_FALSE] },
                   maskPtr->ipv4 {0xFF,...},
                   actionPtr{tunnelTerminate[GT_TRUE],
                             passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
                             copyTtlFromTunnelHeader[GT_FALSE],
                             command [CPSS_PACKET_CMD_FORWARD_E],
                             redirectCommand[CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E],
                             egressInterface{type[CPSS_INTERFACE_TRUNK_E],
                                             trunkId[2]},

                             tunnelStart [GT_TRUE],
                             tunnelStartPtr[0],
                             targetIsTrunk[GT_FALSE],
                             useVidx[GT_FALSE],

                             sourceIdSetEnable[GT_TRUE],
                             sourceId[0],

                             vlanCmd[CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E],
                             vlanId[100],
                             vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                             nestedVlanEnable[GT_FALSE],

                             bindToPolicer[GT_FALSE],

                             qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                             qosTrustMode[CPSS_DXCH_TTI_QOS_UNTRUST_E],
                             qosProfile[0],
                             modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E],
                             modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
                             up[0],

                             mirrorToIngressAnalyzerEnable[GT_FALSE],
                             vntl2Echo[GT_FALSE],
                             bridgeBypass[GT_FALSE],
                             actionStop[GT_FALSE] }
    Expected: GT_OK.
    1.1.2. Call cpssDxChTtiPortGroupRuleGet with non-NULL pointers,
                                                 other params same as in 1.1.1.
    Expected: GT_OK and same values as written
    1.1.3. Call with routerTtiTcamRow [1],
           keyType [CPSS_DXCH_TTI_KEY_MPLS_E],
                   patternPtr->mpls{common{pclId[2],
                                           srcIsTrunk[GT_TRUE],
                                           srcPortTrunk[127],
                                           mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                           vid[100],
                                           isTagged[GT_TRUE],
                                           dsaSrcIsTrunk[GT_FALSE],
                                           dsaSrcPortTrunk[0],
                                           dsaSrcDevice[dev]},
                            label0 [0],
                            exp0 [3],
                            label1 [0xFF],
                            exp1[5],
                            label2 [0xFFF],
                            exp2 [7],
                            numOfLabels [3]
                            protocolAboveMPLS [0]},
    Expected: GT_OK.
    1.1.4. Call cpssDxChTtiPortGroupRuleGet with non-NULL pointers,
                                                 other params same as in 1.1.2.
    Expected: GT_OK and same values as written
    1.1.5. Call with routerTtiTcamRow [2],
                   keyType [CPSS_DXCH_TTI_KEY_ETH_E],
                   patternPtr->mpls{common{pclId[3],
                                           srcIsTrunk[GT_TRUE],
                                           srcPortTrunk[127],
                                           mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                           vid[100],
                                           isTagged[GT_TRUE],
                                           dsaSrcIsTrunk[GT_FALSE],
                                           dsaSrcPortTrunk[0],
                                           dsaSrcDevice[dev]},
                                    up0 [0],
                                    cfi0 [1],
                                    isVlan1Exists [GT_TRUE],
                                    vid1 [100],
                                    up1 [3],
                                    cfi1 [1],
                                    etherType [0],
                                    macToMe [GT_FALSE]}
    Expected: GT_OK.
    1.1.6. Call cpssDxChTtiPortGroupRuleGet with non-NULL pointers,
                                                 other params same as in 1.1.3.
    Expected: GT_OK and same values as written
    1.1.7. Call with out of range patternPtr->ipv4.common.pclId [3] (for IPV4 must be 1)
                     and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.8. Call with patternPtr->ipv4.common.srcIsTrunk [GT_FALSE]
                     out of range srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                     and other params from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.9. Call with patternPtr->ipv4.common.srcIsTrunk [GT_TRUE]
                     out of range srcPortTrunk [128],
                     and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.10. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS],
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.11. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                      patternPtr->ipv4.common.dsaSrcIsTrunk [GT_FALSE]
                      out of range dsaSrcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.12. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                      patternPtr->ipv4.common.dsaSrcIsTrunk [GT_TRUE]
                      out of range dsaSrcPortTrunk [128],
                      and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.13. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                      out of range dsaSrcDevice [PRV_CPSS_MAX_PP_DEVICES_CNS],
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.14. Call with wrong enum values patternPtr->ipv4.tunneltype
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.15. Call with out of range patternPtr->mpls.common.pclId [3] (for MPLSmust be 2)
                      and other params from 1.1.1.
    Expected: GT_OK.
    1.1.16. Call with patternPtr->mpls.common.srcIsTrunk [GT_FALSE]
                      out of range srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.17. Call with patternPtr->mpls.common.srcIsTrunk [GT_TRUE]
                      out of range srcPortTrunk [128],
                      and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.18. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS],
                   and other params from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.19. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                      patternPtr->mpls.common.dsaSrcIsTrunk [GT_FALSE]
                      out of range dsaSrcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.20. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                      patternPtr->mpls.common.dsaSrcIsTrunk [GT_TRUE]
                      out of range dsaSrcPortTrunk [128],
                      and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.21. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                      out of range dsaSrcDevice [PRV_CPSS_MAX_PP_DEVICES_CNS],
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.22. Call with wrong enum values actionPtr->passengerPacketType
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For DxCh3)
    1.1.23. Call with wrong enum values actionPtr->command
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For DxCh3)
    1.1.24. Call with wrong enum values actionPtr->redirectCommand
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For DxCh3)
    1.1.25. Call with wrong enum values actionPtr->vlanCmd
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For DxCh3)
    1.1.26. Call with wrong enum values actionPtr->qosPrecedence
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For DxCh3)
    1.1.27. Call with wrong enum values actionPtr->qosTrustMode
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For DxCh3)
    1.1.28. Call with wrong enum values actionPtr->userDefinedCpuCode
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For DxCh3)
    1.1.29. Call with wrong enum values actionPtr->ttPassengerPacketType
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.30. Call with wrong enum values actionPtr->tsPassengerPacketType
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.31. Call with wrong enum values actionPtr->mplsCommand
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.32. Call with wrong enum values actionPtr->command
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.33. Call with wrong enum values actionPtr->redirectCommand
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.34. Call with wrong enum values actionPtr->tag0VlanCmd
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.35. Call with wrong enum values actionPtr->tag1VlanCmd
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.36. Call with wrong enum values actionPtr->tag0VlanPrecedence
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.37. Call with wrong enum values actionPtr->qosPrecedence
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.38. Call with wrong enum values actionPtr->modifyTag0Up
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.39. Call with wrong enum values actionPtr->tag1UpCommand
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.40. Call with wrong enum values actionPtr->modifyDscp
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.41. Call with wrong enum values actionPtr->pcl0OverrideConfigIndex
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.42. Call with wrong enum values actionPtr->pcl0_1OverrideConfigIndex
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.43. Call with wrong enum values actionPtr->pcl1OverrideConfigIndex
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.44. Call with wrong enum values actionPtr->userDefinedCpuCode
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM. (For xCat and above)
    1.1.45. Call with wrong enum values keyType
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.46. Call with wrong enum values actionType
                      and other params from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.47. Call with out of range routerTtiTcamRow [maxRuleIndex]
                      and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.48. Call with patternPtr [NULL]
                      and other params from 1.1.1.
    Expected: GT_BAD_PTR.
    1.1.49. Call with maskPtr [NULL]
                      and other params from 1.1.1.
    Expected: GT_BAD_PTR.
    1.1.50. Call with actionPtr [NULL]
                      and other params from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    GT_BOOL                         isEqual    = GT_FALSE;
    GT_U32                          tcamRow    = 0;
    CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType    = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;
    CPSS_DXCH_TTI_RULE_UNT          pattern;
    CPSS_DXCH_TTI_RULE_UNT          mask;
    CPSS_DXCH_TTI_ACTION_UNT        action;
    CPSS_DXCH_TTI_RULE_UNT          patternGet;
    CPSS_DXCH_TTI_RULE_UNT          maskGet;
    CPSS_DXCH_TTI_ACTION_UNT        actionGet;


    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));

    cpssOsBzero((GT_VOID*) &patternGet, sizeof(patternGet));
    cpssOsBzero((GT_VOID*) &maskGet, sizeof(maskGet));
    cpssOsBzero((GT_VOID*) &actionGet, sizeof(actionGet));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* set action type */
        actionType = ((CPSS_PP_FAMILY_CHEETAH3_E == PRV_CPSS_PP_MAC(dev)->devFamily) ||
                      (((CPSS_PP_FAMILY_DXCH_XCAT_E == PRV_CPSS_PP_MAC(dev)->devFamily) ||
                       (CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(dev)->devFamily)) &&
                       (0 == PRV_CPSS_PP_MAC(dev)->revision))) ?
                      CPSS_DXCH_TTI_ACTION_TYPE1_ENT : CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with routerTtiTcamRow [0],
                               keyType [CPSS_DXCH_TTI_KEY_IPV4_E],
                               patternPtr->ipv4{common{pclId[1],
                                                       srcIsTrunk[GT_FALSE],
                                                       srcPortTrunk[23],
                                                       mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                                       vid[100],
                                                       isTagged[GT_TRUE],
                                                       dsaSrcIsTrunk[GT_FALSE],
                                                       dsaSrcPortTrunk[0],
                                                       dsaSrcDevice[dev] },
                                                tunneltype [IPv4_OVER_IPv4],
                                                srcIp[192.168.0.1],
                                                destIp[192.168.0.2],
                                                isArp[GT_FALSE] },
                               maskPtr->ipv4 {0xFF,...},
                               actionPtr{tunnelTerminate[GT_TRUE],
                                         passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
                                         copyTtlFromTunnelHeader[GT_FALSE],
                                         command [CPSS_PACKET_CMD_FORWARD_E],
                                         redirectCommand[CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E],
                                         egressInterface{type[CPSS_INTERFACE_TRUNK_E],
                                                         trunkId[2]},

                                         tunnelStart [GT_TRUE],
                                         tunnelStartPtr[0],
                                         targetIsTrunk[GT_FALSE],
                                         useVidx[GT_FALSE],

                                         sourceIdSetEnable[GT_TRUE],
                                         sourceId[0],

                                         vlanCmd[CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E],
                                         vlanId[100],
                                         vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                         nestedVlanEnable[GT_FALSE],

                                         bindToPolicer[GT_FALSE],

                                         qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                         qosTrustMode[CPSS_DXCH_TTI_QOS_UNTRUST_E],
                                         qosProfile[0],
                                         modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E],
                                         modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
                                         up[0],

                                         mirrorToIngressAnalyzerEnable[GT_FALSE],
                                         vntl2Echo[GT_FALSE],
                                         bridgeBypass[GT_FALSE],
                                         actionStop[GT_FALSE] }
                Expected: GT_OK.
            */
            tcamRow = 0;
            keyType = CPSS_DXCH_TTI_KEY_IPV4_E;

            prvSetDefaultIpv4Pattern(&pattern);
            prvSetDefaultAction(&action,actionType);
            prvSetDefaultIpv4Mask(&mask);
            pattern.ipv4.common.srcPortTrunk = (portGroupId << 4) | 0x7;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamRow, keyType);

            /*
                1.1.2. Call cpssDxChTtiPortGroupRuleGet with non-NULL pointers,
                                                             other params same as in 1.1.1.
                Expected: GT_OK and same values as written
            */
            st = cpssDxChTtiPortGroupRuleGet(dev, portGroupsBmp, tcamRow, keyType,
                                             &patternGet, &maskGet, actionType, &actionGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiPortGroupRuleGet: %d, %d, %d", dev, keyType, tcamRow);

            /* validation values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.ipv4,
                                         (GT_VOID*) &patternGet.ipv4,
                                         sizeof(pattern.ipv4))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another patternPtr->ipv4 then was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.ipv4,
                                         (GT_VOID*) &maskGet.ipv4,
                                         sizeof(mask.ipv4))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another maskPtr->ipv4 then was set: %d", dev);
            /*
                1.1.3. Call with routerTtiTcamRow [1],
                       keyType [CPSS_DXCH_TTI_KEY_MPLS_E],
                               patternPtr->mpls{common{pclId[2],
                                                       srcIsTrunk[GT_TRUE],
                                                       srcPortTrunk[127],
                                                       mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                                       vid[100],
                                                       isTagged[GT_TRUE],
                                                       dsaSrcIsTrunk[GT_FALSE],
                                                       dsaSrcPortTrunk[0],
                                                       dsaSrcDevice[dev]},
                                        label0 [0],
                                        exp0 [3],
                                        label1 [0xFF],
                                        exp1[5],
                                        label2 [0xFFF],
                                        exp2 [7],
                                        numOfLabels [3]
                                        protocolAboveMPLS [0]},
                Expected: GT_OK.
            */
            tcamRow = 2;
            keyType = CPSS_DXCH_TTI_KEY_MPLS_E;

            prvSetDefaultMplsPattern(&pattern);
            prvSetDefaultAction(&action,actionType);
            prvSetDefaultMplsMask(&mask);
            pattern.mpls.common.srcPortTrunk = (portGroupId << 4) | 0x7;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamRow, keyType);
            /*
                1.1.4. Call cpssDxChTtiPortGroupRuleGet with non-NULL pointers,
                                                  other params same as in 1.1.2.
                Expected: GT_OK and same values as written
            */
            st = cpssDxChTtiPortGroupRuleGet(dev, portGroupsBmp, tcamRow, keyType,
                                             &patternGet, &maskGet, actionType, &actionGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiPortGroupRuleGet: %d, %d, %d", dev, keyType, tcamRow);

            /* validation values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.mpls,
                                         (GT_VOID*) &patternGet.mpls,
                                         sizeof(pattern.mpls))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another patternPtr->mpls then was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.mpls,
                                         (GT_VOID*) &maskGet.mpls,
                                         sizeof(mask.mpls))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another maskPtr->mpls then was set: %d", dev);
            /*
                1.1.5. Call with routerTtiTcamRow [2],
                               keyType [CPSS_DXCH_TTI_KEY_ETH_E],
                               patternPtr->mpls{common{pclId[3],
                                                       srcIsTrunk[GT_TRUE],
                                                       srcPortTrunk[127],
                                                       mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                                       vid[100],
                                                       isTagged[GT_TRUE],
                                                       dsaSrcIsTrunk[GT_FALSE],
                                                       dsaSrcPortTrunk[0],
                                                       dsaSrcDevice[dev]},
                                                up0 [0],
                                                cfi0 [1],
                                                isVlan1Exists [GT_TRUE],
                                                vid1 [100],
                                                up1 [3],
                                                cfi1 [1],
                                                etherType [0],
                                                macToMe [GT_FALSE]}
                Expected: GT_OK.
            */
            tcamRow = 4;
            keyType = CPSS_DXCH_TTI_KEY_ETH_E;

            prvSetDefaultEthPattern(&pattern);
            prvSetDefaultAction(&action,actionType);
            prvSetDefaultEthMask(&mask);
            pattern.eth.common.srcPortTrunk = (portGroupId << 4) | 0x7;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamRow, keyType);

            /*
                1.1.6. Call cpssDxChTtiPortGroupRuleGet with non-NULL pointers,
                                                             other params same as in 1.1.3.
                Expected: GT_OK and same values as written
            */
            st = cpssDxChTtiPortGroupRuleGet(dev, portGroupsBmp, tcamRow, keyType,
                                             &patternGet, &maskGet, actionType, &actionGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiPortGroupRuleGet: %d, %d, %d", dev, keyType, tcamRow);

            /* validation values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.eth,
                                         (GT_VOID*) &patternGet.eth,
                                         sizeof(pattern.eth))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another patternPtr->eth then was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.eth,
                                         (GT_VOID*) &maskGet.eth,
                                         sizeof(mask.eth))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another maskPtr->eth then was set: %d", dev);

            /*
                1.1.7. Call with out of range patternPtr->ipv4.common.pclId [3] (for IPV4 must be < BIT_10)
                                 and other params from 1.1.1.
                Expected: NOT GT_OK.
            */
            keyType = CPSS_DXCH_TTI_KEY_IPV4_E;

            prvSetDefaultIpv4Pattern(&pattern);
            prvSetDefaultIpv4Mask(&mask);

            pattern.ipv4.common.pclId = 0x400;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, patternPtr->ipv4.common.pclId = %d",
                                             dev, pattern.ipv4.common.pclId);

            pattern.ipv4.common.pclId = 1;

            /*
                1.1.8. Call with patternPtr->ipv4.common.srcIsTrunk [GT_FALSE]
                                 out of range srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                                 and other params from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            pattern.ipv4.common.srcIsTrunk   = GT_FALSE;
            pattern.ipv4.common.srcPortTrunk = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.srcIsTrunk = %d, ->common.srcPortTrunk = %d",
                                         dev, pattern.ipv4.common.srcIsTrunk, pattern.ipv4.common.srcPortTrunk);

            pattern.ipv4.common.srcPortTrunk = TTI_VALID_PORT_TRUNK_CNS;

            /*
                1.1.9. Call with patternPtr->ipv4.common.srcIsTrunk [GT_TRUE]
                                 out of range srcPortTrunk [128],
                                 and other params from 1.1.1.
                Expected: NOT GT_OK.
            */
            pattern.ipv4.common.srcIsTrunk   = GT_TRUE;
            pattern.ipv4.common.srcPortTrunk = 128;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->common.srcIsTrunk = %d, ->common.srcPortTrunk = %d",
                                             dev, pattern.ipv4.common.srcIsTrunk, pattern.ipv4.common.srcPortTrunk);

            pattern.ipv4.common.srcPortTrunk = TTI_VALID_PORT_TRUNK_CNS;

            /*
                1.1.10. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS],
                                  and other params from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            pattern.ipv4.common.vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.vid = %d",
                                         dev, pattern.ipv4.common.vid);

            pattern.ipv4.common.vid = 100;

            /*
                1.1.11. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                                  patternPtr->ipv4.common.dsaSrcIsTrunk [GT_FALSE]
                                  out of range dsaSrcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                                  and other params from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            pattern.ipv4.common.isTagged        = GT_TRUE;
            pattern.ipv4.common.dsaSrcIsTrunk   = GT_FALSE;
            pattern.ipv4.common.dsaSrcPortTrunk = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.dsaSrcIsTrunk = %d, ->common.dsaSrcPortTrunk = %d",
                                         dev, pattern.ipv4.common.dsaSrcIsTrunk, pattern.ipv4.common.dsaSrcPortTrunk);

            pattern.ipv4.common.dsaSrcPortTrunk = TTI_VALID_PORT_TRUNK_CNS;

            /*
                1.1.12. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                                  patternPtr->ipv4.common.dsaSrcIsTrunk [GT_TRUE]
                                  out of range dsaSrcPortTrunk [128],
                                  and other params from 1.1.1.
                Expected: NOT GT_OK.
            */
            pattern.ipv4.common.dsaSrcIsTrunk   = GT_TRUE;
            pattern.ipv4.common.dsaSrcPortTrunk = 128;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->common.dsaSrcIsTrunk = %d, ->common.dsaSrcPortTrunk = %d",
                                             dev, pattern.ipv4.common.dsaSrcIsTrunk, pattern.ipv4.common.dsaSrcPortTrunk);

            pattern.ipv4.common.dsaSrcPortTrunk = TTI_VALID_PORT_TRUNK_CNS;

            /*
                1.1.13. Call with patternPtr->ipv4.common.isTagged [GT_TRUE]
                                  out of range dsaSrcDevice [PRV_CPSS_MAX_PP_DEVICES_CNS],
                                  and other params from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            pattern.ipv4.common.dsaSrcDevice = PRV_CPSS_MAX_PP_DEVICES_CNS;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.dsaSrcDevice = %d",
                                         dev, pattern.ipv4.common.dsaSrcDevice);

            pattern.ipv4.common.dsaSrcDevice = dev;

            /*
                1.1.14. Call with wrong enum values patternPtr->ipv4.tunneltype
                                  and other params from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                (dev, portGroupsBmp, tcamRow, keyType,
                                 &pattern, &mask, actionType, &action),
                                pattern.ipv4.tunneltype);

            /*
                1.1.15. Call with out of range patternPtr->mpls.common.pclId [3] (for MPLSmust be 2)
                                  and other params from 1.1.1.
                Expected: GT_OK.
            */
            keyType = CPSS_DXCH_TTI_KEY_MPLS_E;

            prvSetDefaultMplsPattern(&pattern);
            prvSetDefaultMplsMask(&mask);

            pattern.mpls.common.pclId = 3;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, patternPtr->mpls.common.pclId = %d",
                                         dev, pattern.mpls.common.pclId);

            pattern.mpls.common.pclId = 2;

            /*
                1.1.16. Call with patternPtr->mpls.common.srcIsTrunk [GT_FALSE]
                                  out of range srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                                  and other params from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            pattern.mpls.common.srcIsTrunk   = GT_FALSE;
            pattern.mpls.common.srcPortTrunk = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.srcIsTrunk = %d, ->common.srcPortTrunk = %d",
                                         dev, pattern.mpls.common.srcIsTrunk, pattern.mpls.common.srcPortTrunk);

            pattern.mpls.common.srcPortTrunk = 0;

            /*
                1.1.17. Call with patternPtr->mpls.common.srcIsTrunk [GT_TRUE]
                                  out of range srcPortTrunk [128],
                                  and other params from 1.1.1.
                Expected: NOT GT_OK.
            */
            pattern.mpls.common.srcIsTrunk   = GT_TRUE;
            pattern.mpls.common.srcPortTrunk = 128;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->common.srcIsTrunk = %d, ->common.srcPortTrunk = %d",
                                             dev, pattern.mpls.common.srcIsTrunk, pattern.mpls.common.srcPortTrunk);

            pattern.mpls.common.srcPortTrunk = TTI_VALID_PORT_TRUNK_CNS;

            /*
                1.1.18. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS],
                                  and other params from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            pattern.mpls.common.vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.vid = %d",
                                         dev, pattern.mpls.common.vid);

            pattern.mpls.common.vid = 100;

            /*
                1.1.19. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                                  patternPtr->mpls.common.dsaSrcIsTrunk [GT_FALSE]
                                  out of range dsaSrcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                                  and other params from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            pattern.mpls.common.isTagged        = GT_TRUE;
            pattern.mpls.common.dsaSrcIsTrunk   = GT_FALSE;
            pattern.mpls.common.dsaSrcPortTrunk = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.dsaSrcIsTrunk = %d, ->common.dsaSrcPortTrunk = %d",
                                         dev, pattern.mpls.common.dsaSrcIsTrunk, pattern.mpls.common.dsaSrcPortTrunk);

            pattern.mpls.common.dsaSrcPortTrunk = TTI_VALID_PORT_TRUNK_CNS;

            /*
                1.1.20. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                                  patternPtr->mpls.common.dsaSrcIsTrunk [GT_TRUE]
                                  out of range dsaSrcPortTrunk [128],
                                  and other params from 1.1.1.
                Expected: NOT GT_OK.
            */
            pattern.mpls.common.dsaSrcIsTrunk   = GT_TRUE;
            pattern.mpls.common.dsaSrcPortTrunk = 128;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->common.dsaSrcIsTrunk = %d, ->common.dsaSrcPortTrunk = %d",
                                             dev, pattern.mpls.common.dsaSrcIsTrunk, pattern.mpls.common.dsaSrcPortTrunk);

            pattern.mpls.common.dsaSrcPortTrunk = TTI_VALID_PORT_TRUNK_CNS;

            /*
                1.1.21. Call with patternPtr->mpls.common.isTagged [GT_TRUE]
                                  out of range dsaSrcDevice [PRV_CPSS_MAX_PP_DEVICES_CNS],
                                  and other params from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            pattern.mpls.common.dsaSrcDevice = PRV_CPSS_MAX_PP_DEVICES_CNS;

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->common.dsaSrcDevice = %d",
                                         dev, pattern.mpls.common.dsaSrcDevice);

            pattern.mpls.common.dsaSrcDevice = dev;

            if (CPSS_DXCH_TTI_ACTION_TYPE1_ENT == actionType)
            {
                /*
                    1.1.22. Call with wrong enum values actionPtr->passengerPacketType
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For DxCh3)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type1.passengerPacketType);

                /*
                    1.1.23. Call with wrong enum values actionPtr->command
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For DxCh3)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type1.command);

                /*
                    1.1.24. Call with wrong enum values actionPtr->redirectCommand
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For DxCh3)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type1.redirectCommand);

                /*
                    1.1.25. Call with wrong enum values actionPtr->vlanCmd
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For DxCh3)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type1.vlanCmd);

                /*
                    1.1.26. Call with wrong enum values actionPtr->qosPrecedence
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For DxCh3)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type1.qosPrecedence);

                /*
                    1.1.27. Call with wrong enum values actionPtr->qosTrustMode
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For DxCh3)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type1.qosTrustMode);

                /*
                    1.1.28. Call with wrong enum values actionPtr->userDefinedCpuCode
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For DxCh3)
                */
                action.type1.command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type1.userDefinedCpuCode);

                action.type1.command = CPSS_PACKET_CMD_FORWARD_E;
            }
            else
            {
                /*
                    1.1.29. Call with wrong enum values actionPtr->ttPassengerPacketType
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.ttPassengerPacketType);

                /*
                    1.1.30. Call with wrong enum values actionPtr->tsPassengerPacketType
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.tsPassengerPacketType);

                /*
                    1.1.31. Call with wrong enum values actionPtr->mplsCommand
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.mplsCommand);

                /*
                    1.1.32. Call with wrong enum values actionPtr->command
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.command);

                /*
                    1.1.33. Call with wrong enum values actionPtr->redirectCommand
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.redirectCommand);

                /*
                    1.1.34. Call with wrong enum values actionPtr->tag0VlanCmd
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.tag0VlanCmd);

                /*
                    1.1.35. Call with wrong enum values actionPtr->tag1VlanCmd
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.tag1VlanCmd);

                /*
                    1.1.36. Call with wrong enum values actionPtr->tag0VlanPrecedence
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.tag0VlanPrecedence);

                /*
                    1.1.37. Call with wrong enum values actionPtr->qosPrecedence
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.qosPrecedence);

                /*
                    1.1.38. Call with wrong enum values actionPtr->modifyTag0Up
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.modifyTag0Up);

                /*
                    1.1.39. Call with wrong enum values actionPtr->tag1UpCommand
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.tag1UpCommand);

                /*
                    1.1.40. Call with wrong enum values actionPtr->modifyDscp
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.modifyDscp);

                /*
                    1.1.41. Call with wrong enum values actionPtr->pcl0OverrideConfigIndex
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                action.type2.redirectCommand = CPSS_DXCH_TTI_NO_REDIRECT_E;

                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.pcl0OverrideConfigIndex);

                /*
                    1.1.42. Call with wrong enum values actionPtr->pcl0_1OverrideConfigIndex
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.pcl0_1OverrideConfigIndex);

                /*
                    1.1.43. Call with wrong enum values actionPtr->pcl1OverrideConfigIndex
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.pcl1OverrideConfigIndex);

                action.type2.redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E;

                /*
                    1.1.44. Call with wrong enum values actionPtr->userDefinedCpuCode
                                      and other params from 1.1.1.
                    Expected: GT_BAD_PARAM. (For xCat and above)
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                    (dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action),
                                    action.type2.userDefinedCpuCode);
            }

            /*
                1.1.45. Call with wrong enum values keyType
                                  and other params from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                (dev, portGroupsBmp, tcamRow, keyType,
                                 &pattern, &mask, actionType, &action),
                                keyType);
            /*
                1.1.46. Call with wrong enum values keyType
                                  and other params from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleSet
                                (dev, portGroupsBmp, tcamRow, keyType,
                                 &pattern, &mask, actionType, &action),
                                actionType);

            /*
                1.1.47. Call with out of range tcamRow [maxRuleIndex]
                                  and other params from 1.1.1.
                Expected: NOT GT_OK.
            */
            tcamRow = prvGetMaxRuleIndex(dev);

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tcamRow = %d", dev, tcamRow);

            tcamRow = 0;

            /*
                1.1.48. Call with patternPtr [NULL]
                                  and other params from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             NULL, &mask, actionType, &action);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, patternPtr = NULL", dev);

            /*
                1.1.49. Call with maskPtr [NULL]
                                  and other params from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, NULL, actionType, &action);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);

            /*
                1.1.50. Call with actionPtr [NULL]
                                  and other params from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);

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

        st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                         &pattern, &mask, actionType, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    tcamRow = 0;
    keyType = CPSS_DXCH_TTI_KEY_IPV4_E;

    prvSetDefaultIpv4Pattern(&pattern);
    prvSetDefaultAction(&action, actionType);
    prvSetDefaultIpv4Mask(&mask);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                         &pattern, &mask, actionType, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortGroupRuleGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortGroupRuleGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1.1. Set rule for testing.
         Call cpssDxChTtiPortGroupRuleSet with routerTtiTcamRow [0],
                   keyType [CPSS_DXCH_TTI_KEY_IPV4_E],
                   patternPtr->ipv4{common{pclId[1],
                                           srcIsTrunk[GT_FALSE],
                                           srcPortTrunk[23],
                                           mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                           vid[100],
                                           isTagged[GT_TRUE],
                                           dsaSrcIsTrunk[GT_FALSE],
                                           dsaSrcPortTrunk[0],
                                           dsaSrcDevice[dev] },
                                    tunneltype [IPv4_OVER_IPv4],
                                    srcIp[192.168.0.1],
                                    destIp[192.168.0.2],
                                    isArp[GT_FALSE] },
                   maskPtr->ipv4 {0xFF,...},
                   actionPtr{tunnelTerminate[GT_TRUE],
                             passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
                             copyTtlFromTunnelHeader[GT_FALSE],
                             command [CPSS_PACKET_CMD_FORWARD_E],
                             redirectCommand[CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E],
                             egressInterface{type[CPSS_INTERFACE_TRUNK_E],
                                             trunkId[2]},

                             tunnelStart [GT_TRUE],
                             tunnelStartPtr[0],
                             targetIsTrunk[GT_FALSE],
                             useVidx[GT_FALSE],

                             sourceIdSetEnable[GT_TRUE],
                             sourceId[0],

                             vlanCmd[CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E],
                             vlanId[100],
                             vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                             nestedVlanEnable[GT_FALSE],

                             bindToPolicer[GT_FALSE],

                             qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                             qosTrustMode[CPSS_DXCH_TTI_QOS_UNTRUST_E],
                             qosProfile[0],
                             modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E],
                             modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
                             up[0],

                             mirrorToIngressAnalyzerEnable[GT_FALSE],

                             vntl2Echo[GT_FALSE],
                             bridgeBypass[GT_FALSE],
                             actionStop[GT_FALSE] }
    Expected: GT_OK.
    1.1.2. Call with keyType [CPSS_DXCH_TTI_KEY_IPV4_E /
                              CPSS_DXCH_TTI_KEY_ETH_E],
                     routerTtiTcamRow [ 0 / 10],
                     non-NULL pointers.
    Expected: GT_OK and same values as written
    1.1.3. Call with wrong enum values keyType,
                     other params same as in 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.4. Call with wrong enum values keyType,
                     other params same as in 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.5. Call with out of range routerTtiTcamRow[maxRuleIndex],
                     other params same as in 1.1.2.
    Expected: non GT_OK.
    1.1.6. Call with patternPtr [NULL],
                     other params same as in 1.1.2.
    Expected: GT_BAD_PTR.
    1.1.7. Call with maskPtr [NULL],
                     other params same as in 1.1.2.
    Expected: GT_BAD_PTR.
    1.1.8. Call with actionPtr [NULL],
                     other params same as in 1.1.2.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    GT_U32                          tcamRow    = 0;
    CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType    = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;
    CPSS_DXCH_TTI_RULE_UNT          pattern;
    CPSS_DXCH_TTI_RULE_UNT          mask;
    CPSS_DXCH_TTI_ACTION_UNT        action;


    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* set action type */
        actionType = ((CPSS_PP_FAMILY_CHEETAH3_E == PRV_CPSS_PP_MAC(dev)->devFamily) ||
                      (((CPSS_PP_FAMILY_DXCH_XCAT_E == PRV_CPSS_PP_MAC(dev)->devFamily) ||
                       (CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(dev)->devFamily)) &&
                       (0 == PRV_CPSS_PP_MAC(dev)->revision))) ?
                      CPSS_DXCH_TTI_ACTION_TYPE1_ENT : CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Set rule for testing.
                       Call cpssDxChTtiPortGroupRuleSet with routerTtiTcamRow [0],
                               keyType [CPSS_DXCH_TTI_KEY_IPV4_E],
                               patternPtr->ipv4{common{pclId[1],
                                                       srcIsTrunk[GT_FALSE],
                                                       srcPortTrunk[23],
                                                       mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                                       vid[100],
                                                       isTagged[GT_TRUE],
                                                       dsaSrcIsTrunk[GT_FALSE],
                                                       dsaSrcPortTrunk[0],
                                                       dsaSrcDevice[dev] },
                                                tunneltype [IPv4_OVER_IPv4],
                                                srcIp[192.168.0.1],
                                                destIp[192.168.0.2],
                                                isArp[GT_FALSE] },
                               maskPtr->ipv4 {0xFF,...},
                               actionPtr{tunnelTerminate[GT_TRUE],
                                         passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
                                         copyTtlFromTunnelHeader[GT_FALSE],
                                         command [CPSS_PACKET_CMD_FORWARD_E],
                                         redirectCommand[CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E],
                                         egressInterface{type[CPSS_INTERFACE_TRUNK_E],
                                                         trunkId[2]},

                                         tunnelStart [GT_TRUE],
                                         tunnelStartPtr[0],
                                         targetIsTrunk[GT_FALSE],
                                         useVidx[GT_FALSE],

                                         sourceIdSetEnable[GT_TRUE],
                                         sourceId[0],

                                         vlanCmd[CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E],
                                         vlanId[100],
                                         vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                         nestedVlanEnable[GT_FALSE],

                                         bindToPolicer[GT_FALSE],

                                         qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                         qosTrustMode[CPSS_DXCH_TTI_QOS_UNTRUST_E],
                                         qosProfile[0],
                                         modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E],
                                         modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
                                         up[0],

                                         mirrorToIngressAnalyzerEnable[GT_FALSE],
                                         vntl2Echo[GT_FALSE],
                                         bridgeBypass[GT_FALSE],
                                         actionStop[GT_FALSE] }
                Expected: GT_OK.
            */
            tcamRow = 0;
            keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

            prvSetDefaultIpv4Pattern(&pattern);
            prvSetDefaultAction(&action, actionType);
            prvSetDefaultIpv4Mask(&mask);

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChTtiPortGroupRuleSet: %d, %d, %d",
                                             dev, tcamRow, keyType);
            /*
                1.1.2. Call with keyType [CPSS_DXCH_TTI_KEY_IPV4_E /
                                          CPSS_DXCH_TTI_KEY_ETH_E],
                                 routerTtiTcamRow [0 / 10],
                                 non-NULL pointers.
                Expected: GT_OK.
            */
            keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;
            tcamRow = 0;

            st = cpssDxChTtiPortGroupRuleGet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamRow, keyType);

            /*
                1.1.3. Call with wrong enum values keyType,
                                 other params same as in 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleGet
                                (dev, portGroupsBmp, tcamRow, keyType,
                                 &pattern, &mask, actionType, &action),
                                keyType);

            /*
                1.1.4. Call with wrong enum values actionType,
                                 other params same as in 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleGet
                                (dev, portGroupsBmp, tcamRow, keyType,
                                 &pattern, &mask, actionType, &action),
                                actionType);

            /*
                1.1.5. Call with out of range routerTtiTcamRow [maxRuleIndex],
                                 other params same as in 1.1.2.
                Expected: non GT_OK.
            */
            tcamRow = prvGetMaxRuleIndex(dev);

            st = cpssDxChTtiPortGroupRuleGet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamRow);

            tcamRow = 0;

            /*
                1.1.6. Call with patternPtr [NULL],
                                 other params same as in 1.1.2.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortGroupRuleGet(dev, portGroupsBmp, tcamRow, keyType,
                                             NULL, &mask, actionType, &action);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, patternPtr = NULL", dev);

            /*
                1.1.7. Call with maskPtr [NULL],
                                 other params same as in 1.1.2.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortGroupRuleGet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, NULL, actionType, &action);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);

            /*
                1.1.8. Call with actionPtr [NULL],
                                 other params same as in 1.1.2.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortGroupRuleGet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChTtiPortGroupRuleGet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);

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

        st = cpssDxChTtiPortGroupRuleGet(dev, portGroupsBmp, tcamRow, keyType,
                                         &pattern, &mask, actionType, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    tcamRow = 0;
    keyType = CPSS_DXCH_TTI_KEY_IPV4_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortGroupRuleGet(dev, portGroupsBmp, tcamRow, keyType,
                                         &pattern, &mask, actionType, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortGroupRuleGet(dev, portGroupsBmp, tcamRow, keyType,
                                     &pattern, &mask, actionType, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortGroupRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortGroupRuleActionUpdate)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1.1. Call with actionPtr{tunnelTerminate[GT_TRUE],
                   passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
                   copyTtlFromTunnelHeader[GT_FALSE],
                   command [CPSS_PACKET_CMD_FORWARD_E],
                   redirectCommand[CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E],
                   egressInterface{type[CPSS_INTERFACE_TRUNK_E],
                                   trunkId[2]},

                   tunnelStart [GT_TRUE],
                   tunnelStartPtr[0],
                   targetIsTrunk[GT_FALSE],
                   useVidx[GT_FALSE],

                   sourceIdSetEnable[GT_TRUE],
                   sourceId[0],

                   vlanCmd[CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E],
                   vlanId[100],
                   vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                   nestedVlanEnable[GT_FALSE],

                   bindToPolicer[GT_FALSE],

                   qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                   qosTrustMode[CPSS_DXCH_TTI_QOS_UNTRUST_E],
                   qosProfile[0],
                   modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E],
                   modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
                   up[0],

                   mirrorToIngressAnalyzerEnable[GT_FALSE],
                   vntl2Echo[GT_FALSE],
                   bridgeBypass[GT_FALSE],
                   actionStop[GT_FALSE]
              }
    Expected: GT_OK.
    1.1.2. Call cpssDxChTtiPortGroupRuleGet with non-NULL pointers,
                                                 other params same as in 1.1.1.
    Expected: GT_OK and same action as written
    1.1.3. Call with out of range routerTtiTcamRow [3k],
                     other params same as in 1.1.1.
    Expected: non GT_OK.
    1.1.4. Call with wrong enum values actionType,
                     other params same as in 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.5. Call with actionPtr [NULL],
                     other params same as in 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    GT_BOOL                         isEqual    = GT_FALSE;
    GT_U32                          tcamRow    = 0;
    CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType    = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;
    CPSS_DXCH_TTI_RULE_UNT          pattern;
    CPSS_DXCH_TTI_RULE_UNT          mask;
    CPSS_DXCH_TTI_ACTION_UNT        action;
    CPSS_DXCH_TTI_ACTION_UNT        actionGet;


    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));
    cpssOsBzero((GT_VOID*) &actionGet, sizeof(actionGet));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* set action type */
        actionType = ((CPSS_PP_FAMILY_CHEETAH3_E == PRV_CPSS_PP_MAC(dev)->devFamily) ||
                      (((CPSS_PP_FAMILY_DXCH_XCAT_E == PRV_CPSS_PP_MAC(dev)->devFamily) ||
                       (CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(dev)->devFamily)) &&
                       (0 == PRV_CPSS_PP_MAC(dev)->revision))) ?
                      CPSS_DXCH_TTI_ACTION_TYPE1_ENT : CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                Create default rule
            */
            tcamRow = 0;
            keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

            prvSetDefaultIpv4Pattern(&pattern);
            prvSetDefaultAction(&action, actionType);
            prvSetDefaultIpv4Mask(&mask);

            st = cpssDxChTtiPortGroupRuleSet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChTtiPortGroupRuleSet: %d, %d, %d",
                                         dev, tcamRow, keyType);
            /*
                1.1.1. Call with actionPtr{tunnelTerminate[GT_TRUE],
                               passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
                               copyTtlFromTunnelHeader[GT_FALSE],
                               command [CPSS_PACKET_CMD_FORWARD_E],
                               redirectCommand[CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E],
                               egressInterface{type[CPSS_INTERFACE_TRUNK_E],
                                               trunkId[2]},

                               tunnelStart [GT_TRUE],
                               tunnelStartPtr[0],
                               targetIsTrunk[GT_FALSE],
                               useVidx[GT_FALSE],

                               sourceIdSetEnable[GT_TRUE],
                               sourceId[0],

                               vlanCmd[CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E],
                               vlanId[100],
                               vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                               nestedVlanEnable[GT_FALSE],

                               bindToPolicer[GT_FALSE],

                               qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                               qosTrustMode[CPSS_DXCH_TTI_QOS_UNTRUST_E],
                               qosProfile[0],
                               modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E],
                               modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
                               up[0],

                               mirrorToIngressAnalyzerEnable[GT_FALSE],
                               vntl2Echo[GT_FALSE],
                               bridgeBypass[GT_FALSE],
                               actionStop[GT_FALSE]
                          }
                Expected: GT_OK.
            */
            tcamRow = 0;

            prvSetDefaultAction(&action, actionType);

            st = cpssDxChTtiPortGroupRuleActionUpdate(dev, portGroupsBmp, tcamRow,
                                                      actionType, &action);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamRow);

            /*
                1.1.2. Call cpssDxChTtiPortGroupRuleGet with non-NULL pointers,
                                                             other params same as in 1.1.
                Expected: GT_OK and same action as written
            */
            st = cpssDxChTtiPortGroupRuleGet(dev, portGroupsBmp, tcamRow, keyType,
                                             &pattern, &mask, actionType, &actionGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChTtiPortGroupRuleGet: %d, %d, %d",
                                         dev, keyType, tcamRow);
            /* validation values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &action,
                                         (GT_VOID*) &actionGet,
                                         sizeof(action))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another actionPtr then was set: %d", dev);

           /*
                1.3. Call with out of range routerTtiTcamRow [maxRuleIndex],
                               other params same as in 1.1.
                Expected: non GT_OK.
            */
            tcamRow = prvGetMaxRuleIndex(dev);

            st = cpssDxChTtiPortGroupRuleActionUpdate(dev, portGroupsBmp, tcamRow,
                                                      actionType, &action);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamRow);

            tcamRow = 0;

           /*
                1.4. Call with wrong enum values actionType, other params same as in 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChTtiPortGroupRuleActionUpdate
                                (dev, portGroupsBmp, tcamRow, actionType, &action),
                                actionType);

            /*
                1.5. Call with actionPtr [NULL], other params same as in 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortGroupRuleActionUpdate(dev, portGroupsBmp, tcamRow,
                                                      actionType, NULL);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChTtiPortGroupRuleActionUpdate(dev, portGroupsBmp, tcamRow,
                                                      actionType, &action);

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

        st = cpssDxChTtiPortGroupRuleActionUpdate(dev, portGroupsBmp, tcamRow,
                                                  actionType, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    tcamRow = 0;
    prvSetDefaultAction(&action, actionType);;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortGroupRuleActionUpdate(dev, portGroupsBmp, tcamRow,
                                                  actionType, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortGroupRuleActionUpdate(dev, portGroupsBmp, tcamRow,
                                              actionType, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortGroupRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    IN  GT_BOOL                             valid
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortGroupRuleValidStatusSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (APPLICABLE DEVICES: DxCh3; xCat; Lion; xCat2)
    1.1.1. Call with routerTtiTcamRow [0 / 10],
                     valid [GT_FALSE / GT_TRUE]
    Expected: GT_OK.
    1.1.2. Call cpssDxChTtiPortGroupRuleValidStatusGet with non-NULL validPtr
                                                            and the same routerTtiTcamRow as in 1.1.1.
    Expected: GT_OK and same value as written.
    1.1.3. Call with out of range routerTtiTcamRow [max row]
                     and other params same as in 1.1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    GT_U32                  tcamRow  = 0;
    GT_BOOL                 valid    = GT_FALSE;
    GT_BOOL                 validGet = GT_FALSE;

    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_RULE_UNT      pattern;
    CPSS_DXCH_TTI_RULE_UNT      mask;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    CPSS_DXCH_TTI_RULE_UNT      patternGet;
    CPSS_DXCH_TTI_RULE_UNT      maskGet;
    CPSS_DXCH_TTI_ACTION_UNT    actionGet;
    GT_BOOL                     isEqual = GT_FALSE;


    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));

    cpssOsBzero((GT_VOID*) &patternGet, sizeof(patternGet));
    cpssOsBzero((GT_VOID*) &maskGet, sizeof(maskGet));
    cpssOsBzero((GT_VOID*) &actionGet, sizeof(actionGet));


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                    1.1. Call with ruleIndex [0],
                                   keyType [CPSS_DXCH_TTI_KEY_IPV4_E],
                                   patternPtr->ipv4{common{pclId[1],
                                                           srcIsTrunk[GT_FALSE],
                                                           srcPortTrunk[23],
                                                           mac[0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA],
                                                           vid[100],
                                                           isTagged[GT_TRUE],
                                                           dsaSrcIsTrunk[GT_FALSE],
                                                           dsaSrcPortTrunk[0],
                                                           dsaSrcDevice[dev] },
                                                    tunneltype [IPv4_OVER_IPv4],
                                                    srcIp[192.168.0.1],
                                                    destIp[192.168.0.2],
                                                    isArp[GT_FALSE] },
                                   maskPtr->ipv4 {0xFF,...},
                                   actionPtr{tunnelTerminate[GT_TRUE],
                                             passengerPacketType[CPSS_DXCH_TTI_PASSENGER_IPV4_E],
                                             copyTtlFromTunnelHeader[GT_FALSE],
                                             command [CPSS_PACKET_CMD_FORWARD_E],
                                             redirectCommand[CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E],
                                             egressInterface{type[CPSS_INTERFACE_TRUNK_E],
                                                             trunkId[2]},

                                             tunnelStart [GT_TRUE],
                                             tunnelStartPtr[0],
                                             targetIsTrunk[GT_FALSE],
                                             useVidx[GT_FALSE],

                                             sourceIdSetEnable[GT_TRUE],
                                             sourceId[0],

                                             vlanCmd[CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E],
                                             vlanId[100],
                                             vlanPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                             nestedVlanEnable[GT_FALSE],

                                             bindToPolicer[GT_FALSE],

                                             qosPrecedence[CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                             qosTrustMode[CPSS_DXCH_TTI_QOS_UNTRUST_E],
                                             qosProfile[0],
                                             modifyUpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E],
                                             modifyDscpEnable[CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E],
                                             up[0],

                                             mirrorToIngressAnalyzerEnable[GT_FALSE],
                                             vntl2Echo[GT_FALSE],
                                             bridgeBypass[GT_FALSE],
                                             actionStop[GT_FALSE] }
                    Expected: GT_OK.
                */
            keyType   = CPSS_DXCH_TTI_KEY_IPV4_E;

            prvSetDefaultIpv4Pattern(&pattern);
            prvSetDefaultIpv4Mask(&mask);
            if( PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE2_ENT);
                actionType = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;
            }
            else
            {
                prvSetDefaultAction(&action,CPSS_DXCH_TTI_ACTION_TYPE1_ENT);
                actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;
            }

            st = cpssDxChTtiRuleSet(dev, tcamRow, keyType, &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamRow, keyType);

            /*
                1.2. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                                  other params same as in 1.1.
                Expected: GT_OK and same values as written
            */
            st = cpssDxChTtiRuleGet(dev, tcamRow, keyType, &patternGet, &maskGet, actionType, &actionGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiRuleGet: %d, %d, %d", dev, keyType, tcamRow );

            /* validation values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.ipv4,
                                         (GT_VOID*) &patternGet.ipv4,
                                         sizeof(pattern.ipv4))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another patternPtr->ipv4 then was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.ipv4,
                                         (GT_VOID*) &maskGet.ipv4,
                                         sizeof(mask.ipv4))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another maskPtr->ipv4 then was set: %d", dev);


            /*
                1.1.1. Call with routerTtiTcamRow [0 / 10],
                                 valid [GT_FALSE / GT_TRUE]
                Expected: GT_OK.
            */
            tcamRow = 0;
            valid   = GT_FALSE;

            st = cpssDxChTtiPortGroupRuleValidStatusSet(dev, portGroupsBmp, tcamRow, valid);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamRow, valid);

            /*
                1.1.2. Call cpssDxChTtiPortGroupRuleValidStatusGet with non-NULL validPtr
                                       and the same routerTtiTcamRow as in 1.1.1.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxChTtiPortGroupRuleValidStatusGet(dev, portGroupsBmp, tcamRow, &validGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChTtiPortGroupRuleValidStatusGet: %d", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet,
                       "got another valid then was set: %d", dev);

            /*
                1.1.1. Call with routerTtiTcamRow [0 / 10],
                                 valid [GT_FALSE / GT_TRUE]
                Expected: GT_OK.
            */
            tcamRow = 10;

            st = cpssDxChTtiRuleSet(dev, tcamRow, keyType, &pattern, &mask, actionType, &action);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamRow, keyType);

            /*
                1.2. Call cpssDxChTtiRuleGet with non-NULL pointers,
                                                  other params same as in 1.1.
                Expected: GT_OK and same values as written
            */
            st = cpssDxChTtiRuleGet(dev, tcamRow, keyType, &patternGet, &maskGet, actionType, &actionGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChTtiRuleGet: %d, %d, %d", dev, keyType, tcamRow );

            /* validation values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.ipv4,
                                         (GT_VOID*) &patternGet.ipv4,
                                         sizeof(pattern.ipv4))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another patternPtr->ipv4 then was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask.ipv4,
                                         (GT_VOID*) &maskGet.ipv4,
                                         sizeof(mask.ipv4))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got another maskPtr->ipv4 then was set: %d", dev);

            valid   = GT_FALSE;
            st = cpssDxChTtiPortGroupRuleValidStatusSet(dev, portGroupsBmp, tcamRow, valid);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamRow, valid);


             /*
                1.1.2. Call cpssDxChTtiPortGroupRuleValidStatusGet with non-NULL validPtr
                                       and the same routerTtiTcamRow as in 1.1.1.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxChTtiPortGroupRuleValidStatusGet(dev, portGroupsBmp, tcamRow, &validGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChTtiPortGroupRuleValidStatusGet: %d", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet,
                       "got another valid then was set: %d", dev);


            valid   = GT_TRUE;
            st = cpssDxChTtiPortGroupRuleValidStatusSet(dev, portGroupsBmp, tcamRow, valid);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamRow, valid);

           /*
                1.1.2. Call cpssDxChTtiPortGroupRuleValidStatusGet with non-NULL validPtr
                                       and the same routerTtiTcamRow as in 1.1.1.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxChTtiPortGroupRuleValidStatusGet(dev, portGroupsBmp, tcamRow, &validGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChTtiPortGroupRuleValidStatusGet: %d", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet,
                       "got another valid then was set: %d", dev);

            /*
                1.1.3. Call with out of range routerTtiTcamRow [max row]
                                 and other params same as in 1.1.1.
                Expected: NOT GT_OK.
            */
            tcamRow = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.routerAndTunnelTermTcam;

            st = cpssDxChTtiRuleSet(dev, tcamRow, keyType, &pattern, &mask, actionType, &action);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamRow);

            st = cpssDxChTtiRuleGet(dev, tcamRow, keyType, &patternGet, &maskGet, actionType, &actionGet);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamRow);

            st = cpssDxChTtiPortGroupRuleValidStatusSet(dev, portGroupsBmp, tcamRow, valid);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamRow);

            tcamRow = 0;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChTtiPortGroupRuleValidStatusSet(dev, portGroupsBmp, tcamRow, valid);

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

        st = cpssDxChTtiPortGroupRuleValidStatusSet(dev, portGroupsBmp, tcamRow, valid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    tcamRow = 0;
    valid   = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortGroupRuleValidStatusSet(dev, portGroupsBmp, tcamRow, valid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortGroupRuleValidStatusSet(dev, portGroupsBmp, tcamRow, valid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTtiPortGroupRuleValidStatusGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerTtiTcamRow,
    OUT GT_BOOL                             *validPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTtiPortGroupRuleValidStatusGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (APPLICABLE DEVICES: DxCh3; xCat; Lion; xCat2)
    1.1.1. Call with routerTtiTcamRow [0 / 10],
                     non-NULL validPtr.
    Expected: GT_OK.
    1.1.2. Call with out of range routerTtiTcamRow [max row]
                     and other params as in 1.1.1.
    Expected: NOT GT_OK.
    1.1.3. Call with validPtr [NULL]
                     and other params as in 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    GT_U32                  tcamRow = 0;
    GT_BOOL                 valid   = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with routerTtiTcamRow [0 / 10],
                                 non-NULL validPtr.
                Expected: GT_OK.
            */

            /* call with routerTtiTcamRow = 0 */
            tcamRow = 0;

            st = cpssDxChTtiPortGroupRuleValidStatusGet(dev, portGroupsBmp, tcamRow, &valid);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamRow);

            /* call with routerTtiTcamRow = 10 */
            tcamRow = 10;

            st = cpssDxChTtiPortGroupRuleValidStatusGet(dev, portGroupsBmp, tcamRow, &valid);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamRow);

            /*
                1.1.2. Call with out of range routerTtiTcamRow [max row]
                                 and other params as in 1.1.1.
                Expected: NOT GT_OK.
            */
            tcamRow = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.routerAndTunnelTermTcam;

            st = cpssDxChTtiPortGroupRuleValidStatusGet(dev, portGroupsBmp, tcamRow, &valid);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamRow);

            tcamRow = 0;

            /*
                1.1.3. Call with validPtr [NULL]
                                 and other params as in 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTtiPortGroupRuleValidStatusGet(dev, portGroupsBmp, tcamRow, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChTtiPortGroupRuleValidStatusGet(dev, portGroupsBmp, tcamRow, &valid);

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

        st = cpssDxChTtiPortGroupRuleValidStatusGet(dev, portGroupsBmp, tcamRow, &valid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;

    tcamRow = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTtiPortGroupRuleValidStatusGet(dev, portGroupsBmp, tcamRow, &valid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTtiPortGroupRuleValidStatusGet(dev, portGroupsBmp, tcamRow, &valid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChTti suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChTti)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiMacToMeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiMacToMeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortLookupEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortLookupEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortIpv4OnlyTunneledEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortIpv4OnlyTunneledEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortIpv4OnlyMacToMeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortIpv4OnlyMacToMeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiIpv4McEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiIpv4McEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortMplsOnlyMacToMeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortMplsOnlyMacToMeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiRuleSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiRuleSet_type2)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiRuleGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiRuleGet_type2)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiRuleActionUpdate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiRuleActionUpdate_type2)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiMacModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiMacModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiIpv4GreEthTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiIpv4GreEthTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiExceptionCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiExceptionCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiMimEthTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiMimEthTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortMimOnlyMacToMeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortMimOnlyMacToMeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiRuleValidStatusSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiRuleValidStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiMplsEthTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiMplsEthTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPclIdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPclIdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortGroupMacToMeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortGroupMacToMeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortGroupMacModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortGroupMacModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortGroupRuleSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortGroupRuleGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortGroupRuleActionUpdate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortGroupRuleValidStatusSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiPortGroupRuleValidStatusGet)

    /* Test for Table */
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTtiFillMacToMeTable)
UTF_SUIT_END_TESTS_MAC(cpssDxChTti)


/*******************************************************************************
* prvGetMaxRuleIndex
*
* DESCRIPTION:
*       This routine returns maximum value of rule index.
*
* INPUTS:
*       dev - device id
*
* OUTPUTS:
*       none
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_U32 prvGetMaxRuleIndex(IN  GT_U8  devNum)
{
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;
    return fineTuningPtr->tableSize.routerAndTunnelTermTcam;
}

/*******************************************************************************
* prvSetDefaultIpv4Pattern
*
* DESCRIPTION:
*       This routine set default Ipv4 pattert.
*
* INPUTS:
*       patternPtr - (pointer to) pattern union
*
* OUTPUTS:
*       none
*
* COMMENTS:
*       None.
*******************************************************************************/
static void prvSetDefaultIpv4Pattern(IN CPSS_DXCH_TTI_RULE_UNT *patternPtr)
{
    patternPtr->ipv4.common.pclId        = 1;
    patternPtr->ipv4.common.srcIsTrunk   = GT_FALSE;
    patternPtr->ipv4.common.srcPortTrunk = TTI_VALID_PORT_TRUNK_CNS;

    patternPtr->ipv4.common.mac.arEther[0] = 0xAA;
    patternPtr->ipv4.common.mac.arEther[1] = 0xAA;
    patternPtr->ipv4.common.mac.arEther[2] = 0xAA;
    patternPtr->ipv4.common.mac.arEther[3] = 0xAA;
    patternPtr->ipv4.common.mac.arEther[4] = 0xAA;
    patternPtr->ipv4.common.mac.arEther[5] = 0xAA;

    patternPtr->ipv4.common.vid             = 0xFFF;
    patternPtr->ipv4.common.isTagged        = GT_TRUE;
    patternPtr->ipv4.common.dsaSrcIsTrunk   = GT_FALSE;
    patternPtr->ipv4.common.dsaSrcPortTrunk = TTI_VALID_PORT_TRUNK_CNS;
    patternPtr->ipv4.common.dsaSrcDevice    = 0;

    patternPtr->ipv4.tunneltype = 0;

    patternPtr->ipv4.srcIp.arIP[0] = 192;
    patternPtr->ipv4.srcIp.arIP[1] = 168;
    patternPtr->ipv4.srcIp.arIP[2] = 0;
    patternPtr->ipv4.srcIp.arIP[3] = 1;

    patternPtr->ipv4.destIp.arIP[0] = 192;
    patternPtr->ipv4.destIp.arIP[1] = 168;
    patternPtr->ipv4.destIp.arIP[2] = 0;
    patternPtr->ipv4.destIp.arIP[3] = 2;

    patternPtr->ipv4.isArp = GT_FALSE;
}

/*******************************************************************************
* prvSetDefaultMplsPattern
*
* DESCRIPTION:
*       This routine set default Mpls pattert.
*
* INPUTS:
*       patternPtr - (pointer to) pattern union
*
* OUTPUTS:
*       none
*
* COMMENTS:
*       None.
*******************************************************************************/
static void prvSetDefaultMplsPattern(IN CPSS_DXCH_TTI_RULE_UNT *patternPtr)
{
    patternPtr->mpls.common.pclId        = 2;
    patternPtr->mpls.common.srcIsTrunk   = GT_TRUE;
    patternPtr->mpls.common.srcPortTrunk = TTI_VALID_PORT_TRUNK_CNS;

    patternPtr->mpls.common.mac.arEther[0] = 0xAA;
    patternPtr->mpls.common.mac.arEther[1] = 0xAA;
    patternPtr->mpls.common.mac.arEther[2] = 0xAA;
    patternPtr->mpls.common.mac.arEther[3] = 0xAA;
    patternPtr->mpls.common.mac.arEther[4] = 0xAA;
    patternPtr->mpls.common.mac.arEther[5] = 0xAA;

    patternPtr->mpls.common.vid             = 0xFFF;
    patternPtr->mpls.common.isTagged        = GT_FALSE;
    patternPtr->mpls.common.dsaSrcIsTrunk   = GT_FALSE;
    patternPtr->mpls.common.dsaSrcPortTrunk = TTI_VALID_PORT_TRUNK_CNS;
    patternPtr->mpls.common.dsaSrcDevice    = 0;

    patternPtr->mpls.label0 = 0;
    patternPtr->mpls.exp0   = 3;
    patternPtr->mpls.label1 = 0xFF;
    patternPtr->mpls.exp1   = 5;
    patternPtr->mpls.label2 = 0xFFF;
    patternPtr->mpls.exp2   = 7;

    patternPtr->mpls.numOfLabels = 3;
    patternPtr->mpls.protocolAboveMPLS = 0;
}

/*******************************************************************************
* prvSetDefaultEthPattern
*
* DESCRIPTION:
*       This routine set default Ethernet pattert.
*
* INPUTS:
*       patternPtr - (pointer to) pattern union
*
* OUTPUTS:
*       none
*
* COMMENTS:
*       None.
*******************************************************************************/
static void prvSetDefaultEthPattern(IN  CPSS_DXCH_TTI_RULE_UNT *patternPtr)
{
    patternPtr->eth.common.pclId        = 3;
    patternPtr->eth.common.srcIsTrunk   = GT_TRUE;
    patternPtr->eth.common.srcPortTrunk = TTI_VALID_PORT_TRUNK_CNS;

    patternPtr->eth.common.mac.arEther[0] = 0xAA;
    patternPtr->eth.common.mac.arEther[1] = 0xAA;
    patternPtr->eth.common.mac.arEther[2] = 0xAA;
    patternPtr->eth.common.mac.arEther[3] = 0xAA;
    patternPtr->eth.common.mac.arEther[4] = 0xAA;
    patternPtr->eth.common.mac.arEther[5] = 0xAA;

    patternPtr->eth.common.vid             = 0xFFF;
    patternPtr->eth.common.isTagged        = GT_TRUE;
    patternPtr->eth.common.dsaSrcIsTrunk   = GT_FALSE;
    patternPtr->eth.common.dsaSrcPortTrunk = TTI_VALID_PORT_TRUNK_CNS;
    patternPtr->eth.common.dsaSrcDevice    = 0;

    patternPtr->eth.up0           = 0;
    patternPtr->eth.cfi0          = 1;
    patternPtr->eth.isVlan1Exists = GT_TRUE;
    patternPtr->eth.vid1          = 0xFFF;
    patternPtr->eth.up1           = 3;
    patternPtr->eth.cfi1          = 1;
    patternPtr->eth.etherType     = 0;
    patternPtr->eth.macToMe       = GT_FALSE;
}
/*******************************************************************************
* prvSetDefaultIpv4Mask
*
* DESCRIPTION:
*       This routine set default Ipv4 mask.
*
* INPUTS:
*       patternPtr - (pointer to) pattern union
*
* OUTPUTS:
*       none
*
* COMMENTS:
*       None.
*******************************************************************************/
static void prvSetDefaultIpv4Mask(IN CPSS_DXCH_TTI_RULE_UNT *patternPtr)
{
    patternPtr->ipv4.common.pclId        = 0x3FF;
    patternPtr->ipv4.common.srcIsTrunk   = 0x1;
    patternPtr->ipv4.common.srcPortTrunk = 0xFF;

    patternPtr->ipv4.common.mac.arEther[0] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[1] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[2] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[3] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[4] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[5] = 0xFF;

    patternPtr->ipv4.common.vid             = 0xFFF;
    patternPtr->ipv4.common.isTagged        = 0x1;
    patternPtr->ipv4.common.dsaSrcIsTrunk   = 0x1;
    patternPtr->ipv4.common.dsaSrcPortTrunk = 0x3F;
    patternPtr->ipv4.common.dsaSrcDevice    = 0x3F;

    patternPtr->ipv4.tunneltype = 0x7;

    patternPtr->ipv4.srcIp.arIP[0] = 0xFF;
    patternPtr->ipv4.srcIp.arIP[1] = 0xFF;
    patternPtr->ipv4.srcIp.arIP[2] = 0xFF;
    patternPtr->ipv4.srcIp.arIP[3] = 0xFF;

    patternPtr->ipv4.destIp.arIP[0] = 0xFF;
    patternPtr->ipv4.destIp.arIP[1] = 0xFF;
    patternPtr->ipv4.destIp.arIP[2] = 0xFF;
    patternPtr->ipv4.destIp.arIP[3] = 0xFF;

    patternPtr->ipv4.isArp = 0x1;
}

/*******************************************************************************
* prvSetDefaultMplsMask
*
* DESCRIPTION:
*       This routine set default Mpls mask.
*
* INPUTS:
*       patternPtr - (pointer to) pattern union
*
* OUTPUTS:
*       none
*
* COMMENTS:
*       None.
*******************************************************************************/
static void prvSetDefaultMplsMask(IN CPSS_DXCH_TTI_RULE_UNT *patternPtr)
{
    patternPtr->ipv4.common.pclId        = 0x3FF;
    patternPtr->ipv4.common.srcIsTrunk   = 0x1;
    patternPtr->ipv4.common.srcPortTrunk = 0xFF;

    patternPtr->ipv4.common.mac.arEther[0] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[1] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[2] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[3] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[4] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[5] = 0xFF;

    patternPtr->ipv4.common.vid             = 0xFFF;
    patternPtr->ipv4.common.isTagged        = 0x1;
    patternPtr->ipv4.common.dsaSrcIsTrunk   = 0x1;
    patternPtr->ipv4.common.dsaSrcPortTrunk = 0x3F;
    patternPtr->ipv4.common.dsaSrcDevice    = 0x3F;

    patternPtr->mpls.label0 = 0xFFFFF;
    patternPtr->mpls.exp0   = 0x7;
    patternPtr->mpls.label1 = 0xFFFFF;
    patternPtr->mpls.exp1   = 0x7;
    patternPtr->mpls.label2 = 0xFFFFF;
    patternPtr->mpls.exp2   = 0x7;

    patternPtr->mpls.numOfLabels = 0x3;
    patternPtr->mpls.protocolAboveMPLS = 0x3;
}

/*******************************************************************************
* prvSetDefaultEthMask
*
* DESCRIPTION:
*       This routine set default Ethernet mask.
*
* INPUTS:
*       patternPtr - (pointer to) pattern union
*
* OUTPUTS:
*       none
*
* COMMENTS:
*       None.
*******************************************************************************/
static void prvSetDefaultEthMask(IN  CPSS_DXCH_TTI_RULE_UNT *patternPtr)
{
    patternPtr->ipv4.common.pclId        = 0x3FF;
    patternPtr->ipv4.common.srcIsTrunk   = 0x1;
    patternPtr->ipv4.common.srcPortTrunk = 0xFF;

    patternPtr->ipv4.common.mac.arEther[0] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[1] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[2] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[3] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[4] = 0xFF;
    patternPtr->ipv4.common.mac.arEther[5] = 0xFF;

    patternPtr->ipv4.common.vid             = 0xFFF;
    patternPtr->ipv4.common.isTagged        = 0x1;
    patternPtr->ipv4.common.dsaSrcIsTrunk   = 0x1;
    patternPtr->ipv4.common.dsaSrcPortTrunk = 0x3F;
    patternPtr->ipv4.common.dsaSrcDevice    = 0x3F;

    patternPtr->eth.up0           = 0x7;
    patternPtr->eth.cfi0          = 0x1;
    patternPtr->eth.isVlan1Exists = 0x1;
    patternPtr->eth.vid1          = 0xFFF;
    patternPtr->eth.up1           = 0x7;
    patternPtr->eth.cfi1          = 0x1;
    patternPtr->eth.etherType     = 0xFFFF;
    patternPtr->eth.macToMe       = 0x1;
}

/*******************************************************************************
* prvSetDefaultAction
*
* DESCRIPTION:
*       This routine set default TTI TCAM rule action.
*
* INPUTS:
*       patternPtr - (pointer to) action
*
* OUTPUTS:
*       none
*
* COMMENTS:
*       None.
*******************************************************************************/
static void prvSetDefaultAction(IN  CPSS_DXCH_TTI_ACTION_UNT *actionPtr, CPSS_DXCH_TTI_ACTION_TYPE_ENT type)
{
    switch(type)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:
        actionPtr->type1.tunnelTerminate        = GT_TRUE;
        actionPtr->type1.passengerPacketType    = CPSS_DXCH_TTI_PASSENGER_IPV4_E;
        actionPtr->type1.copyTtlFromTunnelHeader= GT_FALSE;
        actionPtr->type1.command                = CPSS_PACKET_CMD_FORWARD_E;
        actionPtr->type1.redirectCommand        = CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E;

        actionPtr->type1.egressInterface.type   = CPSS_INTERFACE_TRUNK_E;
        actionPtr->type1.egressInterface.trunkId= 2;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type1.egressInterface.trunkId);

        actionPtr->type1.tunnelStart            = GT_TRUE;
        actionPtr->type1.tunnelStartPtr         = 0;

        actionPtr->type1.useVidx                = GT_FALSE;

        actionPtr->type1.sourceIdSetEnable      = GT_TRUE;
        actionPtr->type1.sourceId               = 0;

        actionPtr->type1.vlanCmd                = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;
        actionPtr->type1.vlanId                 = 100;

        actionPtr->type1.vlanPrecedence         = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        actionPtr->type1.nestedVlanEnable       = GT_FALSE;

        actionPtr->type1.bindToPolicer          = GT_FALSE;

        actionPtr->type1.qosPrecedence          = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        actionPtr->type1.qosTrustMode           = CPSS_DXCH_TTI_QOS_UNTRUST_E;
        actionPtr->type1.qosProfile             = 0;

        actionPtr->type1.modifyUpEnable         = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E;
        actionPtr->type1.modifyDscpEnable       = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E;
        actionPtr->type1.up                     = 0;

        actionPtr->type1.mirrorToIngressAnalyzerEnable = GT_FALSE;

        actionPtr->type1.vntl2Echo              = GT_FALSE;
        actionPtr->type1.bridgeBypass           = GT_FALSE;
        actionPtr->type1.actionStop             = GT_FALSE;
        break;
    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:
        actionPtr->type2.tunnelTerminate        = GT_TRUE;
        actionPtr->type2.ttPassengerPacketType  = CPSS_DXCH_TTI_PASSENGER_IPV4V6_E;
        actionPtr->type2.tsPassengerPacketType  = CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E;

        actionPtr->type2.copyTtlFromTunnelHeader= GT_FALSE;

        actionPtr->type2.mplsCommand            = CPSS_DXCH_TTI_MPLS_NOP_CMD_E;
        actionPtr->type2.mplsTtl                = 0;
        actionPtr->type2.enableDecrementTtl     = GT_FALSE;

        actionPtr->type2. command               = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        actionPtr->type2.redirectCommand        = CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E;
        actionPtr->type2.egressInterface.type   = CPSS_INTERFACE_TRUNK_E;
        actionPtr->type2.egressInterface.trunkId= TTI_TRUNK_MAX_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type2.egressInterface.trunkId);
        actionPtr->type2.arpPtr                 = 0;
        actionPtr->type2.tunnelStart            = GT_TRUE;
        actionPtr->type2.tunnelStartPtr         = 0xFF;
        actionPtr->type2.routerLttPtr           = 0;
        actionPtr->type2.vrfId                  = 0;

        actionPtr->type2.sourceIdSetEnable      = GT_TRUE;
        actionPtr->type2.sourceId               = TTI_SOURCE_ID_MAX_CNS;

        actionPtr->type2.tag0VlanCmd            = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;
        actionPtr->type2.tag0VlanId             = 0xFFF;
        actionPtr->type2.tag1VlanCmd            = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;
        actionPtr->type2.tag1VlanId             = 0xFFF;
        actionPtr->type2.tag0VlanPrecedence     = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        actionPtr->type2.nestedVlanEnable       = GT_FALSE;

        actionPtr->type2.bindToPolicerMeter     = GT_FALSE;
        actionPtr->type2.bindToPolicer          = GT_FALSE;
        actionPtr->type2.policerIndex           = 0;

        actionPtr->type2.qosPrecedence          = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        actionPtr->type2.keepPreviousQoS        = GT_FALSE;
        actionPtr->type2.trustUp                = GT_TRUE;
        actionPtr->type2.trustDscp              = GT_TRUE;
        actionPtr->type2.trustExp               = GT_TRUE;
        actionPtr->type2.qosProfile             = 0;
        actionPtr->type2.modifyTag0Up           = GT_FALSE;
        actionPtr->type2.tag1UpCommand          = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E;
        actionPtr->type2.modifyDscp             = GT_FALSE;
        actionPtr->type2.tag0Up                 = CPSS_USER_PRIORITY_RANGE_CNS - 1;
        actionPtr->type2.tag1Up                 = CPSS_USER_PRIORITY_RANGE_CNS - 1;
        actionPtr->type2.remapDSCP              = GT_FALSE;

        actionPtr->type2.pcl0OverrideConfigIndex        = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
        actionPtr->type2.pcl0_1OverrideConfigIndex      = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
        actionPtr->type2.pcl1OverrideConfigIndex        = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
        actionPtr->type2.iPclConfigIndex                = 0;

        actionPtr->type2.mirrorToIngressAnalyzerEnable  = GT_FALSE;
        actionPtr->type2.userDefinedCpuCode             = CPSS_NET_LAST_USER_DEFINED_E;
        actionPtr->type2.bindToCentralCounter           = GT_TRUE;
        actionPtr->type2.centralCounterIndex            = TTI_COUNTER_MAX_CNS;
        actionPtr->type2.vntl2Echo                      = GT_FALSE;
        actionPtr->type2.bridgeBypass                   = GT_FALSE;
        actionPtr->type2.ingressPipeBypass              = GT_FALSE;
        actionPtr->type2.actionStop                     = GT_FALSE;

        break;
    default:
        break;
    }
}
