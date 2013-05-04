/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmTtiUT.c
*
* DESCRIPTION:
*       Unit tests for CPSS EXMXPM TTI (Tunnel termination interface) support.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/tti/cpssExMxPmTtiTypes.h>
#include <cpss/exMxPm/exMxPmGen/tti/cpssExMxPmTti.h>
#include <cpss/generic/tunnel/cpssGenTunnelTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Invalid enum */
#define TTI_INVALID_ENUM_CNS        0x5AAAAAA5

/* Max correct value for ruleIndex in cpssExMxPmTtiRuleSet */
/* Change to correct value!!  */
#define PRV_TTI_MAX_RULE_INDEX      0x1FFF

/* Default valid value for port id */
#define TTI_VALID_VIRT_PORT_CNS     0

/*  Internal function. Is used for filling TTI action structure     */
/*  with default values which are used for most of all tests.       */
static void ttiActionDefaultSet (OUT CPSS_EXMXPM_TTI_ACTION_UNT     *actionPtr);

/*----------------------------------------------------------------------------*/

/*  Internal function. Is used for filling TTI mask & pattern structure     */
/*  with default values which are used for most of all tests.               */

static void ttiRuleMaskPatternDefaultSet
(
    OUT CPSS_EXMXPM_TTI_RULE_UNT     *maskPtr,
    OUT CPSS_EXMXPM_TTI_RULE_UNT     *patternPtr
);


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiPortLookupModeSet
(
    IN  GT_U8                           dev,
    IN  GT_U8                           port,
    IN  CPSS_EXMXPM_TTI_KEY_TYPE_ENT    keyType,
    IN  CPSS_EXMXPM_TTI_LOOKUP_MODE_ENT mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiPortLookupModeSet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with keyType [CPSS_EXMXPM_TTI_KEY_IP_V4_E /
                            CPSS_EXMXPM_TTI_KEY_MPLS_E],
                   mode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E /
                         CPSS_EXMXPM_TTI_LOOKUP_MODE_EXT_TCAM_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmTtiPortLookupModeGet with non-NULL modePtr.
    Expected: GT_OK and same value as written.
    1.1.3. Call with out of range mode [0x5AAAAA5].
    Expected: GT_BAD_PARAM.
    1.1.4. Call with out of range keyType [0x5AAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U8                   port;

    CPSS_EXMXPM_TTI_LOOKUP_MODE_ENT mode    = CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E;
    CPSS_EXMXPM_TTI_LOOKUP_MODE_ENT modeRet = CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT    keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*     1.1.1. Call with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E /
                                           CPSS_EXMXPM_TTI_KEY_MPLS_E],
                                  mode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E /
                                        CPSS_EXMXPM_TTI_LOOKUP_MODE_EXT_TCAM_E].
                   Expected: GT_OK.
            */
            /* iterate with keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E */
            keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
            mode    = CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E;

            st = cpssExMxPmTtiPortLookupModeSet(dev, port, keyType, mode);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, keyType, mode);

            /* 1.1.2. Call cpssExMxPmTtiPortLookupModeGet with non-NULL modePtr.
                Expected: GT_OK and same value as written.  */

            st = cpssExMxPmTtiPortLookupModeGet(dev, port, keyType, &modeRet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmTtiPortLookupModeGet: %d", dev);

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeRet,
                 "cpssExMxPmTtiPortLookupModeGet: get another mode than was set: dev = %d", dev);
            }

            /* iterate with keyType = CPSS_EXMXPM_TTI_KEY_MPLS_E */
            keyType = CPSS_EXMXPM_TTI_KEY_MPLS_E;
            mode    = CPSS_EXMXPM_TTI_LOOKUP_MODE_EXT_TCAM_E;

            st = cpssExMxPmTtiPortLookupModeSet(dev, port, keyType, mode);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, keyType, mode);

            /* 1.1.2. Call cpssExMxPmTtiPortLookupModeGet with non-NULL modePtr.
                Expected: GT_OK and same value as written.  */

            st = cpssExMxPmTtiPortLookupModeGet(dev, port, keyType, &modeRet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmTtiPortLookupModeGet: %d", dev);

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeRet,
                 "cpssExMxPmTtiPortLookupModeGet: get another mode than was set: dev = %d", dev);
            }

            /*
                1.1.3. Call with out of range mode [0x5AAAAA5].
                Expected: GT_BAD_PARAM.
            */
            mode = TTI_INVALID_ENUM_CNS;

            st = cpssExMxPmTtiPortLookupModeSet(dev, port, keyType, mode);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, mode = %d", dev, port, mode);

            mode = CPSS_EXMXPM_TTI_LOOKUP_MODE_EXT_TCAM_E;

            /*
                1.1.4. Call with out of range keyType [0x5AAAAA5].
                Expected: GT_BAD_PARAM.
            */
            keyType = TTI_INVALID_ENUM_CNS;

            st = cpssExMxPmTtiPortLookupModeSet(dev, port, keyType, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, keyType);
        }

        /* set correct values*/
        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
        mode    = CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmTtiPortLookupModeSet(dev, port, keyType, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmTtiPortLookupModeSet(dev, port, keyType, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmTtiPortLookupModeSet(dev, port, keyType, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

    }

    port    = TTI_VALID_VIRT_PORT_CNS;
    keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    mode    = CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiPortLookupModeSet(dev, port, keyType, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiPortLookupModeSet(dev, port, keyType, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiPortLookupModeGet
(
    IN  GT_U8                           dev,
    IN  GT_U8                           port,
    IN  CPSS_EXMXPM_TTI_KEY_TYPE_ENT    keyType,
    OUT CPSS_EXMXPM_TTI_LOOKUP_MODE_ENT *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiPortLookupModeGet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E],
                   not NULL mode.
    Expected: GT_OK.
    1.1.2. Call with out of range keyType [0x5AAAAA5].
    Expected: GT_BAD_PARAM.
    1.1.3. Call with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                           st = GT_OK;
    GT_U8                               dev;
    GT_U8                               port;

    CPSS_EXMXPM_TTI_LOOKUP_MODE_ENT     mode    = CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E],
                                 not NULL mode.
                Expected: GT_OK.
            */
            keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

            st = cpssExMxPmTtiPortLookupModeGet(dev, port, keyType, &mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, keyType);

            /*
                1.1.2. Call with out of range keyType [0x5AAAAA5].
                Expected: GT_BAD_PARAM.
            */
            keyType = TTI_INVALID_ENUM_CNS;

            st = cpssExMxPmTtiPortLookupModeGet(dev, port, keyType, &mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, keyType);

            keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

            /* 1.1.3. Call with hashModePtr [NULL]. Expected: GT_BAD_PTR. */

            st = cpssExMxPmTtiPortLookupModeGet(dev, port, keyType, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ModePtr = NULL", dev, port);
        }

        /* set correct values*/
        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmTtiPortLookupModeGet(dev, port, keyType, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmTtiPortLookupModeGet(dev, port, keyType, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmTtiPortLookupModeGet(dev, port, keyType, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

    }

    port    = TTI_VALID_VIRT_PORT_CNS;
    keyType = TTI_INVALID_ENUM_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiPortLookupModeGet(dev, port, keyType, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiPortLookupModeGet(dev, port, keyType, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiPortLookupConfigSet
(
    IN  GT_U8                               dev,
    IN  GT_U8                               port,
    IN  CPSS_EXMXPM_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_EXMXPM_TTI_LOOKUP_CONFIG_STC   *configPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiPortLookupConfigSet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E / CPSS_EXMXPM_TTI_KEY_ETH_E],
                     configPtr { actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E], pclId [0] }.
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmTtiPortLookupConfigGet
                with non-NULL configPtr, keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E].
    Expected: GT_OK and same values as written.
    1.1.3. Call with out of range keyType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.1.4. Call with out of range configPtr->actionType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.1.5. Call with out of range configPtr->pclId[0x400], other params same as in 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U8                   port;

    CPSS_EXMXPM_TTI_KEY_TYPE_ENT        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    GT_BOOL                             isEqual = GT_FALSE;

    CPSS_EXMXPM_TTI_LOOKUP_CONFIG_STC   config;
    CPSS_EXMXPM_TTI_LOOKUP_CONFIG_STC   configRet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E /
               CPSS_EXMXPM_TTI_KEY_ETH_E], configPtr { actionType
               [CPSS_EXMXPM_TTI_ACTION_STANDARD_E], pclId [0] }. Expected: GT_OK.   */
            cpssOsBzero((GT_VOID*) &config, sizeof(config));

            keyType = CPSS_EXMXPM_TTI_KEY_ETH_E;
            config.actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
            config.pclId = 0;

            st = cpssExMxPmTtiPortLookupConfigSet(dev, port, keyType, &config);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, keyType);

            keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

            st = cpssExMxPmTtiPortLookupConfigSet(dev, port, keyType, &config);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, keyType);

            /* 1.1.2. Call cpssExMxPmTtiPortLookupConfigGet
               with non-NULL configPtr, keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E].
               Expected: GT_OK and same values as written.  */

            cpssOsBzero((GT_VOID*) &configRet, sizeof(configRet));

            st = cpssExMxPmTtiPortLookupConfigGet(dev, port, keyType, &configRet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmTtiPortLookupConfigGet: %d, %d, %d",
                                         dev, port, keyType);

            if (GT_OK == st)
            {
                isEqual = (0 == cpssOsMemCmp((GT_VOID*)&config, (GT_VOID*)&configRet, sizeof (config)))
                      ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                                 "cpssExMxPmTtiPortLookupConfigGet: get another config than was set: %d, %d", dev, keyType);
            }

            /* 1.1.3. Call with out of range keyType [0x5AAAAA5], other params
               same as in 1.1. Expected: GT_BAD_PARAM.  */

            keyType = TTI_INVALID_ENUM_CNS;

            st = cpssExMxPmTtiPortLookupConfigSet(dev, port, keyType, &config);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, keyType);

            keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

            /* 1.1.4. Call with out of range configPtr-> actionType [0x5AAAAA5],
                    other params same as in 1.1. Expected: GT_BAD_PARAM.    */

            config.actionType = TTI_INVALID_ENUM_CNS;

            st = cpssExMxPmTtiPortLookupConfigSet(dev, port, keyType, &config);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, keyType);

            config.actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;

            /*
                1.1.5. Call with out of range configPtr->pclId[0x400], other params same as in 1.1.
                Expected: NOT GT_OK.
            */
            config.pclId = 0x400;

            st = cpssExMxPmTtiPortLookupConfigSet(dev, port, keyType, &config);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, 5d, config.pclId = %d",
                                            dev, port, config.pclId);
        }

        /* set correct values*/
        cpssOsBzero((GT_VOID*) &config, sizeof(config));
        keyType = CPSS_EXMXPM_TTI_KEY_ETH_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmTtiPortLookupConfigSet(dev, port, keyType, &config);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmTtiPortLookupConfigSet(dev, port, keyType, &config);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmTtiPortLookupConfigSet(dev, port, keyType, &config);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

    }

    port    = TTI_VALID_VIRT_PORT_CNS;
    keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiPortLookupConfigSet(dev, port, keyType, &config);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiPortLookupConfigSet(dev, port, keyType, &config);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiPortLookupConfigGet
(
    IN  GT_U8                               dev,
    IN  GT_U8                               port,
    IN  CPSS_EXMXPM_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_EXMXPM_TTI_LOOKUP_CONFIG_STC   *configPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiPortLookupConfigGet)
{
/*
ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
1.1.1. Call with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E / CPSS_EXMXPM_TTI_KEY_ETH_E],
                non-NULL configPtr. Expected: GT_OK.
1.1.2. Call with out of range keyType [0x5AAAAA5], other params same as in 1.1. Expected: GT_BAD_PARAM.
1.1.3. Call with configPtr [NULL], other params same as in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U8                   port;

    CPSS_EXMXPM_TTI_KEY_TYPE_ENT        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

    CPSS_EXMXPM_TTI_LOOKUP_CONFIG_STC   config;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E /
              CPSS_EXMXPM_TTI_KEY_ETH_E], non-NULL configPtr. Expected: GT_OK.  */

            keyType = CPSS_EXMXPM_TTI_KEY_ETH_E;

            st = cpssExMxPmTtiPortLookupConfigGet(dev, port, keyType, &config);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, keyType);

            keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

            st = cpssExMxPmTtiPortLookupConfigGet(dev, port, keyType, &config);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, keyType);

            /* 1.1.2. Call with out of range keyType [0x5AAAAA5], other params same
                    as in 1.1. Expected: GT_BAD_PARAM.  */

            keyType = TTI_INVALID_ENUM_CNS;

            st = cpssExMxPmTtiPortLookupConfigGet(dev, port, keyType, &config);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, keyType);

            keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

            /* 1.1.3. Call with configPtr [NULL], other params same as in 1.1.
                     Expected: GT_BAD_PTR.  */

            st = cpssExMxPmTtiPortLookupConfigGet(dev, port, keyType, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, configPtr = NULL", dev, port);
        }

        /* set correct values*/
        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmTtiPortLookupConfigGet(dev, port, keyType, &config);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmTtiPortLookupConfigGet(dev, port, keyType, &config);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmTtiPortLookupConfigGet(dev, port, keyType, &config);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    port    = TTI_VALID_VIRT_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiPortLookupConfigGet(dev, port, keyType, &config);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiPortLookupConfigGet(dev, port, keyType, &config);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiMacModeSet
(
    IN  GT_U8                               dev,
    IN  CPSS_EXMXPM_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_EXMXPM_TTI_MAC_MODE_ENT        macMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiMacModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E / CPSS_EXMXPM_TTI_KEY_ETH_E],
                   macMode [CPSS_EXMXPM_TTI_MAC_MODE_DA_E / CPSS_EXMXPM_TTI_MAC_MODE_SA_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTtiMacModeGet
              with non-NULL macModePtr, keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E].
    Expected: GT_OK and same value as written.
    1.3. Call with out of range keyType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range macModePtr [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    CPSS_EXMXPM_TTI_KEY_TYPE_ENT        keyType     = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    CPSS_EXMXPM_TTI_MAC_MODE_ENT        macMode     = CPSS_EXMXPM_TTI_MAC_MODE_DA_E;
    CPSS_EXMXPM_TTI_MAC_MODE_ENT        macModeRet  = CPSS_EXMXPM_TTI_MAC_MODE_DA_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E /
           CPSS_EXMXPM_TTI_KEY_ETH_E], macMode [CPSS_EXMXPM_TTI_MAC_MODE_DA_E
           / CPSS_EXMXPM_TTI_MAC_MODE_SA_E]. Expected: GT_OK.   */

        keyType = CPSS_EXMXPM_TTI_KEY_ETH_E;
        macMode = CPSS_EXMXPM_TTI_MAC_MODE_SA_E;

        st = cpssExMxPmTtiMacModeSet(dev, keyType, macMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, keyType, macMode);

        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
        macMode = CPSS_EXMXPM_TTI_MAC_MODE_DA_E;

        st = cpssExMxPmTtiMacModeSet(dev, keyType, macMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, keyType, macMode);

        /* 1.2. Call cpssExMxPmTtiMacModeGet with non-NULL macModePtr,
           keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E]. Expected: GT_OK and
           same value as written.   */

        st = cpssExMxPmTtiMacModeGet(dev, keyType, &macModeRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmTtiMacModeGet: %d", dev);

        if (GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(macMode, macModeRet,
             "cpssExMxPmTtiMacModeGet: get another macMode than was set: dev = %d", dev);
        }

        /* 1.3. Call with out of range keyType [0x5AAAAA5], other params
           same as in 1.1. Expected: GT_BAD_PARAM.  */

        keyType = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiMacModeSet(dev, keyType, macMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, keyType);

        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

        /* 1.4. Call with out of range macModePtr [0x5AAAAA5], other params
           same as in 1.1. Expected: GT_BAD_PARAM.  */

        macMode = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiMacModeSet(dev, keyType, macMode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, macMode = %d", dev, macMode);

        macMode = CPSS_EXMXPM_TTI_MAC_MODE_DA_E;
    }

    keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    macMode = CPSS_EXMXPM_TTI_MAC_MODE_DA_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiMacModeSet(dev, keyType, macMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiMacModeSet(dev, keyType, macMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiMacModeGet
(
    IN  GT_U8                               dev,
    IN  CPSS_EXMXPM_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_EXMXPM_TTI_MAC_MODE_ENT        *macModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiMacModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E / CPSS_EXMXPM_TTI_KEY_ETH_E],
                   non-NULL macModePtr.
    Expected: GT_OK.
    1.2. Call with out of range keyType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with macModePtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    CPSS_EXMXPM_TTI_KEY_TYPE_ENT    keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    CPSS_EXMXPM_TTI_MAC_MODE_ENT    macMode = CPSS_EXMXPM_TTI_MAC_MODE_DA_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E /
           CPSS_EXMXPM_TTI_KEY_ETH_E], non-NULL macModePtr. Expected: GT_OK.*/

        keyType = CPSS_EXMXPM_TTI_KEY_ETH_E;

        st = cpssExMxPmTtiMacModeGet(dev, keyType, &macMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, keyType);

        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

        st = cpssExMxPmTtiMacModeGet(dev, keyType, &macMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, keyType);

        /* 1.2. Call with out of range keyType [0x5AAAAA5], other params
            same as in 1.1. Expected: GT_BAD_PARAM. */

        keyType = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiMacModeGet(dev, keyType, &macMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, keyType);

        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

        /* 1.3. Call with macModePtr [NULL], other params same as in 1.1.
             Expected: GT_BAD_PTR.  */

        st = cpssExMxPmTtiMacModeGet(dev, keyType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macMode = NULL", dev);
    }

    keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiMacModeGet(dev, keyType, &macMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiMacModeGet(dev, keyType, &macMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiIpv4McEnableSet
(
    IN  GT_U8       dev,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiIpv4McEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTtiIpv4McEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    GT_BOOL                 enable    = GT_FALSE;
    GT_BOOL                 enableRet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable [GT_TRUE / GT_FALSE]. Expected: GT_OK.*/

        enable = GT_TRUE;

        st = cpssExMxPmTtiIpv4McEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_FALSE;

        st = cpssExMxPmTtiIpv4McEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* 1.2. Call cpssExMxPmTtiIpv4McEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.    */

        st = cpssExMxPmTtiIpv4McEnableGet(dev, &enableRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmTtiIpv4McEnableGet: %d", dev);

        if (GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableRet,
             "cpssExMxPmTtiIpv4McEnableGet: get another enable than was set: dev = %d", dev);
        }


    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiIpv4McEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiIpv4McEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiIpv4McEnableGet
(
    IN  GT_U8       dev,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiIpv4McEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    GT_BOOL                 enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL enablePtr. Expected: GT_OK.*/

        st = cpssExMxPmTtiIpv4McEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with enablePtr [NULL]. Expected: GT_BAD_PTR.*/

        st = cpssExMxPmTtiIpv4McEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiIpv4McEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiIpv4McEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiIpv4GreEthTypeSet
(
    IN  GT_U8                               dev,
    IN  CPSS_EXMXPM_TTI_IPV4_GRE_TYPE_ENT   greTunnelType,
    IN  GT_U32                              ethType
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiIpv4GreEthTypeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with greTunnelType [CPSS_TUNNEL_GRE0_ETHER_TYPE_E / CPSS_TUNNEL_GRE1_ETHER_TYPE_E],
                   ethType [0 / 0xFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTtiIpv4GreEthTypeGet
              with non-NULL ethTypePtr, greTunnelType [CPSS_TUNNEL_GRE0_ETHER_TYPE_E].
    Expected: GT_OK and same value as written.
    1.3. Call with out of range greTunnelType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with invalid ethType [0x10000], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    CPSS_TUNNEL_GRE_ETHER_TYPE_ENT      greTunnelType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;
    GT_U32                              ethType       = 0;
    GT_U32                              ethTypeRet    = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with greTunnelType [CPSS_TUNNEL_GRE0_ETHER_TYPE_E /
          CPSS_EXMXPM_TTI_IPV4_GRE1_E], ethType [0 / 0xFFFF]. Expected: GT_OK.*/

        greTunnelType = CPSS_TUNNEL_GRE1_ETHER_TYPE_E;
        ethType = 0xFFFF;

        st = cpssExMxPmTtiIpv4GreEthTypeSet(dev, greTunnelType, ethType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, greTunnelType, ethType);

        greTunnelType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;
        ethType = 0;

        st = cpssExMxPmTtiIpv4GreEthTypeSet(dev, greTunnelType, ethType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, greTunnelType, ethType);

        /* 1.2. Call cpssExMxPmTtiIpv4GreEthTypeGet with non-NULL ethTypePtr,
           greTunnelType [CPSS_EXMXPM_TTI_IPV4_GRE0_E].
           Expected: GT_OK and same value as written.   */

        st = cpssExMxPmTtiIpv4GreEthTypeGet(dev, greTunnelType, &ethTypeRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmTtiIpv4GreEthTypeGet: %d", dev);

        if (GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(ethType, ethTypeRet,
             "cpssExMxPmTtiIpv4GreEthTypeGet: get another ethType than was set: dev = %d", dev);
        }

        /* 1.3. Call with out of range greTunnelType [0x5AAAAA5], other params
           same as in 1.1. Expected: GT_BAD_PARAM.  */

        greTunnelType = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiIpv4GreEthTypeSet(dev, greTunnelType, ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, greTunnelType);

        greTunnelType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;

        /* 1.4. Call with invalid ethType [0x10000], other params same as in 1.1.
           Expected: GT_BAD_PARAM.  */

        ethType = 0x10000;

        st = cpssExMxPmTtiIpv4GreEthTypeSet(dev, greTunnelType, ethType);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ethType = %d", dev, ethType);

        ethType = 0;
    }

    greTunnelType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;
    ethType = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiIpv4GreEthTypeSet(dev, greTunnelType, ethType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiIpv4GreEthTypeSet(dev, greTunnelType, ethType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiIpv4GreEthTypeGet
(
    IN  GT_U8                               dev,
    IN  CPSS_TUNNEL_GRE_ETHER_TYPE_ENT      greTunnelType,
    OUT GT_U32                              *ethTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiIpv4GreEthTypeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with greTunnelType [CPSS_TUNNEL_GRE0_ETHER_TYPE_E / CPSS_TUNNEL_GRE1_ETHER_TYPE_E], non-NULL ethTypePtr.
    Expected: GT_OK.
    1.2. Call with out of range greTunnelType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with ethTypePtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    CPSS_TUNNEL_GRE_ETHER_TYPE_ENT      greTunnelType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;
    GT_U32                              ethType       = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with greTunnelType [CPSS_TUNNEL_GRE0_ETHER_TYPE_E /
           CPSS_EXMXPM_TTI_IPV4_GRE1_E], non-NULL ethTypePtr. Expected: GT_OK.*/

        greTunnelType = CPSS_TUNNEL_GRE1_ETHER_TYPE_E;

        st = cpssExMxPmTtiIpv4GreEthTypeGet(dev, greTunnelType, &ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, greTunnelType);

        greTunnelType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;

        st = cpssExMxPmTtiIpv4GreEthTypeGet(dev, greTunnelType, &ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, greTunnelType);

        /* 1.2. Call with out of range greTunnelType [0x5AAAAA5], other
           params same as in 1.1. Expected: GT_BAD_PARAM.   */

        greTunnelType = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiIpv4GreEthTypeGet(dev, greTunnelType, &ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, greTunnelType);

        greTunnelType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;

        /* 1.3. Call with ethTypePtr [NULL], other params same as in 1.1.
           Expected: GT_BAD_PTR.    */

        st = cpssExMxPmTtiIpv4GreEthTypeGet(dev, greTunnelType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ethTypePtr = NULL", dev);
    }

    greTunnelType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiIpv4GreEthTypeGet(dev, greTunnelType, &ethType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiIpv4GreEthTypeGet(dev, greTunnelType, &ethType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiPortMacToMeEnableSet
(
    IN  GT_U8                               dev,
    IN  GT_U8                               port,
    IN  CPSS_EXMXPM_TTI_KEY_TYPE_ENT        keyType,
    IN  GT_BOOL                             enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiPortMacToMeEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E / CPSS_EXMXPM_TTI_KEY_ETH_E],
                     enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmTtiPortMacToMeEnableGet with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E], non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.1.3. Call with out of range keyType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U8                   port;

    CPSS_EXMXPM_TTI_KEY_TYPE_ENT        keyType   = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    GT_BOOL                             enable    = GT_FALSE;
    GT_BOOL                             enableRet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E /
               CPSS_EXMXPM_TTI_KEY_MPLS_E], enable [GT_TRUE / GT_FALSE].
               Expected: GT_OK. */

            keyType = CPSS_EXMXPM_TTI_KEY_MPLS_E;
            enable = GT_FALSE;

            st = cpssExMxPmTtiPortMacToMeEnableSet(dev, port, keyType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, keyType, enable);

            keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
            enable = GT_TRUE;

            st = cpssExMxPmTtiPortMacToMeEnableSet(dev, port, keyType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, keyType, enable);

            /* 1.1.2. Call cpssExMxPmTtiPortMacToMeEnableGet with keyType
               [CPSS_EXMXPM_TTI_KEY_IPV4_E], non-NULL enablePtr.
               Expected: GT_OK and the same enable. */

            st = cpssExMxPmTtiPortMacToMeEnableGet(dev, port, keyType, &enableRet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTtiPortMacToMeEnableGet: %d, %d", dev, port);

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableRet,
                 "cpssExMxPmTtiPortMacToMeEnableGet: get another enable than was set: dev = %d, port = %d", dev, port);
            }

            /* 1.1.3. Call with out of range keyType [0x5AAAAA5], other params same
               as in 1.1.1. Expected: GT_BAD_PARAM.   */

            keyType = TTI_INVALID_ENUM_CNS;

            st = cpssExMxPmTtiPortMacToMeEnableSet(dev, port, keyType, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, keyType);
        }

        /* set correct values*/
        keyType = CPSS_EXMXPM_TTI_KEY_MPLS_E;
        enable = GT_FALSE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmTtiPortMacToMeEnableSet(dev, port, keyType, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmTtiPortMacToMeEnableSet(dev, port, keyType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmTtiPortMacToMeEnableSet(dev, port, keyType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

    }

    port    = TTI_VALID_VIRT_PORT_CNS;
    keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiPortMacToMeEnableSet(dev, port, keyType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiPortMacToMeEnableSet(dev, port, keyType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiPortMacToMeEnableGet
(
    IN  GT_U8                               dev,
    IN  GT_U8                               port,
    IN  CPSS_EXMXPM_TTI_KEY_TYPE_ENT        keyType,
    OUT GT_BOOL                             *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiPortMacToMeEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E / CPSS_EXMXPM_TTI_KEY_ETH_E], non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with out of range keyType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.1.3. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U8                   port;

    CPSS_EXMXPM_TTI_KEY_TYPE_ENT  keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    GT_BOOL                       enable  = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E /
                CPSS_EXMXPM_TTI_KEY_ETH_E], non-NULL enablePtr. Expected: GT_OK.    */

            keyType = CPSS_EXMXPM_TTI_KEY_ETH_E;

            st = cpssExMxPmTtiPortMacToMeEnableGet(dev, port, keyType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, keyType);

            keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

            st = cpssExMxPmTtiPortMacToMeEnableGet(dev, port, keyType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, keyType);

            /* 1.1.2. Call with out of range keyType [0x5AAAAA5], other params same
               as in 1.1. Expected: GT_BAD_PARAM.   */

            keyType = TTI_INVALID_ENUM_CNS;

            st = cpssExMxPmTtiPortMacToMeEnableGet(dev, port, keyType, &enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, keyType = %d", dev, port, keyType);

            keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

            /* 1.1.3. Call function with enablePtr [NULL]. Expected: GT_BAD_PTR.  */

            st = cpssExMxPmTtiPortMacToMeEnableGet(dev, port, keyType, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        /* set correct values*/
        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmTtiPortMacToMeEnableGet(dev, port, keyType, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmTtiPortMacToMeEnableGet(dev, port, keyType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmTtiPortMacToMeEnableGet(dev, port, keyType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    port    = TTI_VALID_VIRT_PORT_CNS;
    keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiPortMacToMeEnableGet(dev, port, keyType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiPortMacToMeEnableGet(dev, port, keyType, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiMacToMeSet
(
    IN  GT_U8                              dev,
    IN  GT_U32                             entryIndex,
    IN  GT_BOOL                            valid,
    IN  CPSS_EXMXPM_TTI_MAC_VLAN_STC       *valuePtr,
    IN  CPSS_EXMXPM_TTI_MAC_VLAN_STC       *maskPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiMacToMeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIndex [0 / 127]
                   valid [GT_TRUE];
                   valuePtr {mac{arEther[0x10, 0x20, 0x30, 0x40, 0x50, 0x60]},
                             vlanId[100 / 4095]},
                   maskPtr  {all set to 0xFF}
    Expected: GT_OK.
    1.2. Call cpssExMxPmTtiMacToMeGet with non-NULL valuePtr and maskPtr.
    Expected: GT_OK and same values as written.
    1.3. Call with entryIndex [256],
                   and other params same as in 1.1.
    Expected: NOT GT_OK.
    1.4. Call function with valuePtr [NULL].
    Expected: GT_BAD_PTR.
    1.5. Call function with maskPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_BOOL         isEqual = GT_FALSE;

    GT_U32                             entryIndex = 0;
    GT_BOOL                            valid      = GT_TRUE;
    GT_BOOL                            validGet   = GT_TRUE;
    CPSS_EXMXPM_TTI_MAC_VLAN_STC       value;
    CPSS_EXMXPM_TTI_MAC_VLAN_STC       mask;
    CPSS_EXMXPM_TTI_MAC_VLAN_STC       valueGet;
    CPSS_EXMXPM_TTI_MAC_VLAN_STC       maskGet;


    cpssOsBzero((GT_VOID*) &value, sizeof(value));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &valueGet, sizeof(valueGet));
    cpssOsBzero((GT_VOID*) &maskGet, sizeof(maskGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryIndex [0 / 127]
                           valid [GT_TRUE];
                           valuePtr {mac{arEther[0x10, 0x20, 0x30, 0x40, 0x50, 0x60]}, vlanId[100 / 4095]},
                           maskPtr  {all set to 0xFF}
            Expected: GT_OK.
        */
        /* iterate with entryIndex = 0 */
        entryIndex = 0;
        valid      = GT_TRUE;
        value.mac.arEther[0] = 0x10;
        value.mac.arEther[1] = 0x20;
        value.mac.arEther[2] = 0x30;
        value.mac.arEther[3] = 0x40;
        value.mac.arEther[4] = 0x50;
        value.mac.arEther[5] = 0x60;
        value.vlanId         = 100;

        mask.mac.arEther[0] = 0xFF;
        mask.mac.arEther[1] = 0xFF;
        mask.mac.arEther[2] = 0xFF;
        mask.mac.arEther[3] = 0xFF;
        mask.mac.arEther[4] = 0xFF;
        mask.mac.arEther[5] = 0xFF;
        mask.vlanId = 0xFFF;

        st = cpssExMxPmTtiMacToMeSet(dev, entryIndex, valid, &value, &mask);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call cpssExMxPmTtiMacToMeGet with non-NULL valuePtr and maskPtr.
            Expected: GT_OK and same values as written.
        */
        st = cpssExMxPmTtiMacToMeGet(dev, entryIndex, &validGet, &valueGet, &maskGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTtiMacToMeGet: %d, %d",
                                    dev, entryIndex);

        UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet, "got another valid than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &value, (GT_VOID*) &valueGet, sizeof(value) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another value than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask, (GT_VOID*) &maskGet, sizeof(mask) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another mask than was set: %d", dev);

        /* iterate with entryIndex = 0 */
        entryIndex   = 127;
        value.vlanId = 4095;

        st = cpssExMxPmTtiMacToMeSet(dev, entryIndex, valid, &value, &mask);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call cpssExMxPmTtiMacToMeGet with non-NULL valuePtr and maskPtr.
            Expected: GT_OK and same values as written.
        */
        st = cpssExMxPmTtiMacToMeGet(dev, entryIndex, &validGet, &valueGet, &maskGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTtiMacToMeGet: %d, %d",
                                    dev, entryIndex);

        UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet, "got another valid than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &value, (GT_VOID*) &valueGet, sizeof(value) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another value than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask, (GT_VOID*) &maskGet, sizeof(mask) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another mask than was set: %d", dev);

        /*
            1.3. Call with entryIndex [256],
                           and other params same as in 1.1.
            Expected: NOT GT_OK.
        */
        entryIndex = 256;

        st = cpssExMxPmTtiMacToMeSet(dev, entryIndex, valid, &value, &mask);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        entryIndex = 127;

        /*
            1.4. Call function with valuePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTtiMacToMeSet(dev, entryIndex, valid, NULL, &mask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, valuePtr = NULL", dev);

        /*
            1.5. Call function with maskPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTtiMacToMeSet(dev, entryIndex, valid, &value, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);
    }

    entryIndex = 0;
    valid      = GT_TRUE;
    value.mac.arEther[0] = 0x10;
    value.mac.arEther[1] = 0x20;
    value.mac.arEther[2] = 0x30;
    value.mac.arEther[3] = 0x40;
    value.mac.arEther[4] = 0x50;
    value.mac.arEther[5] = 0x60;
    value.vlanId         = 100;

    mask.mac.arEther[0] = 0xFF;
    mask.mac.arEther[1] = 0xFF;
    mask.mac.arEther[2] = 0xFF;
    mask.mac.arEther[3] = 0xFF;
    mask.mac.arEther[4] = 0xFF;
    mask.mac.arEther[5] = 0xFF;
    mask.vlanId = 0xFFF;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiMacToMeSet(dev, entryIndex, valid, &value, &mask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiMacToMeSet(dev, entryIndex, valid, &value, &mask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiMacToMeGet
(
    IN  GT_U8                               dev,
    IN  GT_U32                              entryIndex,
    OUT GT_BOOL                            *validPtr,
    OUT CPSS_EXMXPM_TTI_MAC_VLAN_STC       *valuePtr,
    OUT CPSS_EXMXPM_TTI_MAC_VLAN_STC       *maskPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiMacToMeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIndex [0]
                   not NULL validPtr, valuePtr, maskPtr.
    Expected: GT_OK.
    1.2. Call with entryIndex [256],
                   and other params same as in 1.1.
    Expected: NOT GT_OK.
    1.3. Call function with valuePtr [NULL].
    Expected: GT_BAD_PTR.
    1.4. Call function with maskPtr [NULL].
    Expected: GT_BAD_PTR.
    1.5. Call function with validPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    GT_BOOL                            valid = GT_TRUE;
    GT_U32                             entryIndex = 0;
    CPSS_EXMXPM_TTI_MAC_VLAN_STC       value;
    CPSS_EXMXPM_TTI_MAC_VLAN_STC       mask;


    cpssOsBzero((GT_VOID*) &value, sizeof(value));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryIndex [0]
                           not NULL valuePtr, maskPtr.
            Expected: GT_OK.
        */
        entryIndex = 0;

        st = cpssExMxPmTtiMacToMeGet(dev, entryIndex, &valid, &mask, &value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call with entryIndex [256],
                           and other params same as in 1.1.
            Expected: NOT GT_OK.
        */
        entryIndex = 256;

        st = cpssExMxPmTtiMacToMeGet(dev, entryIndex, &valid, &mask, &value);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        entryIndex = 0;

        /*
            1.3. Call function with valuePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTtiMacToMeGet(dev, entryIndex, &valid, NULL, &mask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, valuePtr = NULL", dev);

        /*
            1.4. Call function with maskPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTtiMacToMeGet(dev, entryIndex, &valid, &value, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);

        /*
            1.5. Call function with validPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTtiMacToMeGet(dev, entryIndex, NULL, &value, &mask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);
    }

    entryIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiMacToMeGet(dev, entryIndex, &valid, &mask, &value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiMacToMeGet(dev, entryIndex, &valid, &mask, &value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiPortLookupEnableSet
(
    IN  GT_U8                               dev,
    IN  GT_U8                               port,
    IN  CPSS_EXMXPM_TTI_KEY_TYPE_ENT        keyType,
    IN  GT_BOOL                             enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiPortLookupEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E / CPSS_EXMXPM_TTI_KEY_ETH_E],
                     enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmTtiPortLookupEnableGet with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E], non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.1.3. Call with out of range keyType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8                   dev;
    GT_U8                   port;

    CPSS_EXMXPM_TTI_KEY_TYPE_ENT  keyType   = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    GT_BOOL                       enable    = GT_FALSE;
    GT_BOOL                       enableRet = GT_FALSE;


    port = TTI_VALID_VIRT_PORT_CNS;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E
              / CPSS_EXMXPM_TTI_KEY_ETH_E], enable [GT_TRUE / GT_FALSE].
               Expected: GT_OK. */

            keyType = CPSS_EXMXPM_TTI_KEY_ETH_E;
            enable = GT_FALSE;

            st = cpssExMxPmTtiPortLookupEnableSet(dev, port, keyType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, keyType, enable);

            keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
            enable = GT_TRUE;

            st = cpssExMxPmTtiPortLookupEnableSet(dev, port, keyType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, keyType, enable);

            /* 1.1.2. Call cpssExMxPmTtiPortLookupEnableGet with keyType
               [CPSS_EXMXPM_TTI_KEY_IPV4_E], non-NULL enablePtr.
               Expected: GT_OK and the same enable. */

            st = cpssExMxPmTtiPortLookupEnableGet(dev, port, keyType, &enableRet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTtiPortLookupEnableGet: %d, port = %d", dev, port);

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableRet,
                 "cpssExMxPmTtiPortLookupEnableGet: get another enable than was set: %d", dev);
            }

            /* 1.1.3. Call with out of range keyType [0x5AAAAA5], other
               params same as in 1.1. Expected: GT_BAD_PARAM.   */

            keyType = TTI_INVALID_ENUM_CNS;

            st = cpssExMxPmTtiPortLookupEnableSet(dev, port, keyType, enable);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, keyType = %d", dev, keyType);

            keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
        }

        /* set correct values*/
        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
        enable = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmTtiPortLookupEnableSet(dev, port, keyType, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmTtiPortLookupEnableSet(dev, port, keyType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmTtiPortLookupEnableSet(dev, port, keyType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* set correct values*/
    keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TTI_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiPortLookupEnableSet(dev, port, keyType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiPortLookupEnableSet(dev, port, keyType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiPortLookupEnableGet
(
    IN  GT_U8                               dev,
    IN  GT_U8                               port,
    IN  CPSS_EXMXPM_TTI_KEY_TYPE_ENT        keyType,
    OUT GT_BOOL                             *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiPortLookupEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E / CPSS_EXMXPM_TTI_KEY_ETH_E], non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with out of range keyType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.1.3. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st   = GT_OK;
    GT_U8                   dev;
    GT_U8                   port;

    CPSS_EXMXPM_TTI_KEY_TYPE_ENT  keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    GT_BOOL                       enable  = GT_FALSE;


    port = TTI_VALID_VIRT_PORT_CNS;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E /
              CPSS_EXMXPM_TTI_KEY_ETH_E], non-NULL enablePtr. Expected: GT_OK.*/

            keyType = CPSS_EXMXPM_TTI_KEY_ETH_E;

            st = cpssExMxPmTtiPortLookupEnableGet(dev, port, keyType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, keyType);

            keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

            st = cpssExMxPmTtiPortLookupEnableGet(dev, port, keyType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, keyType);

            /* 1.1.2. Call with out of range keyType [0x5AAAAA5], other
               params same as in 1.1. Expected: GT_BAD_PARAM.   */

            keyType = TTI_INVALID_ENUM_CNS;

            st = cpssExMxPmTtiPortLookupEnableGet(dev, port, keyType, &enable);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, keyType = %d", dev, keyType);

            keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

            /* 1.1.3. Call function with enablePtr [NULL]. Expected: GT_BAD_PTR.*/

            st = cpssExMxPmTtiPortLookupEnableGet(dev, port, keyType, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
        }

        /* set correct values*/
        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmTtiPortLookupEnableGet(dev, port, keyType, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmTtiPortLookupEnableGet(dev, port, keyType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmTtiPortLookupEnableGet(dev, port, keyType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* set correct values*/
    keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TTI_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiPortLookupEnableGet(dev, port, keyType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiPortLookupEnableGet(dev, port, keyType, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiPortIpv4OnlyTunneledEnableSet
(
    IN  GT_U8                               dev,
    IN  GT_U8                               port,
    IN  GT_BOOL                             enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiPortIpv4OnlyTunneledEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS               st   = GT_OK;
    GT_U8                   dev;
    GT_U8                   port;

    GT_BOOL                 enable    = GT_FALSE;
    GT_BOOL                 enableRet = GT_FALSE;


    port = TTI_VALID_VIRT_PORT_CNS;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with enable [GT_TRUE / GT_FALSE].
               Expected: GT_OK. */

            enable = GT_FALSE;

            st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            enable = GT_TRUE;

            st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /* 1.1.2. Call cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet with
               non-NULL enablePtr. Expected: GT_OK and the same enable. */

            st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enableRet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet: %d, port = %d", dev, port);

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableRet,
                 "cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet: get another enable than was set: %d", dev);
            }
        }

        /* set correct values*/
        enable = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* set correct values*/
    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TTI_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet
(
    IN  GT_U8                               dev,
    IN  GT_U8                               port,
    OUT GT_BOOL                             *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call function with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st   = GT_OK;
    GT_U8                   dev;
    GT_U8                   port;
    GT_BOOL                 enable = GT_FALSE;

    port = TTI_VALID_VIRT_PORT_CNS;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with non-NULL enablePtr. Expected: GT_OK.*/

            st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call function with enablePtr [NULL].
               Expected: GT_BAD_PTR.    */

            st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TTI_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiRuleSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMXPM_TCAM_TYPE_ENT               tcamType,
    IN  CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT   *ruleInfoPtr,
    IN  CPSS_EXMXPM_TTI_KEY_TYPE_ENT            keyType,
    IN  CPSS_EXMXPM_TTI_ACTION_TYPE_ENT         actionType,
    IN  GT_BOOL                                 valid,
    IN  CPSS_EXMXPM_TTI_RULE_UNT                *maskPtr,
    IN  CPSS_EXMXPM_TTI_RULE_UNT                *patternPtr,
    IN  CPSS_EXMXPM_TTI_ACTION_UNT              *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiRuleSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E],
                            ruleIndex [ 0 ],
                            actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                            lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                            patternPtr { pclId [0],
                                         srcIsTrunk  [0],
                                         srcPortTrunk [0],
                                         mac [0xAA, 0x0, 0x0, 0x0, 0x0, 0x0],
                                         vid[100],
                                         isTagged [GT_FALSE],
                                         tunneltype [CPSS_EXMXPM_TTI_IPV4_OVER_IPV4_TUNNEL_E],
                                         srcIp [10,10,10,1],
                                         destIp [10,10,10,2],
                                         dsaSrcPort [0],
                                         dsaSrcDevice [0] },
                            maskPtr { ipv4.pclId = 0xFFFFFFFF, other fields are zeroed},
                            actionPtr {set standard to default valuess },
    Expected: GT_OK.
    1.2. Call cpssExMxPmTtiRuleGet with non-NULL pointers, other params same as in 1.1.
    Expected: GT_OK and same values as written.
    1.3. Call with out of range keyType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range ruleIndex [PRV_TTI_MAX_RULE_INDEX +1], other params same as in 1.1.
    Expected: non GT_OK.
    1.5. Call with patternPtr->ipv4.common.pclId [1024] and other params from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with patternPtr->ipv4.common.vid [4096] and other params from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with patternPtr->ipv4.common.srcIsTrunk [2],
              and other params from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with patternPtr->ipv4.common.srcIsTrunk [0],
              patternPtr->ipv4.common.srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
              and other params from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with patternPtr->ipv4.common.srcIsTrunk [1],
              patternPtr->ipv4.common.srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
              and other params from 1.1.
    Expected: GT_OK.
    1.10. Call with patternPtr->ipv4.common.srcIsTrunk [1],
              patternPtr->ipv4.common.srcPortTrunk [256]
              and other params from 1.1.
    Expected: NOT GT_OK.

    1.11. Call function with patternPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.12. Call function with maskPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.13. Call function with actionPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.14. Call with out of range actionPtr->standard.command [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.15. Call with out of range actionPtr->standard.redirectCommand [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.16. Call with out of range actionPtr->standard.userDefinedCpuCode [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.17. Call with out of range actionPtr->standard.vlanPrecedence [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.18. Call with out of range actionPtr->standard.passengerPacketType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.19. Call with out of range actionPtr->standard.mplsCmd [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TCAM_TYPE_ENT       tcamType   = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT    keyType    = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    CPSS_EXMXPM_TTI_ACTION_TYPE_ENT actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
    GT_BOOL                         valid      = GT_FALSE;
    GT_BOOL                         validRet   = GT_FALSE;
    GT_BOOL                         isEqual    = GT_FALSE;

    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT   ruleInfo;
    CPSS_EXMXPM_TTI_RULE_UNT                pattern;
    CPSS_EXMXPM_TTI_RULE_UNT                mask;
    CPSS_EXMXPM_TTI_ACTION_UNT              action;

    CPSS_EXMXPM_TTI_RULE_UNT                patternRet;
    CPSS_EXMXPM_TTI_RULE_UNT                maskRet;
    CPSS_EXMXPM_TTI_ACTION_UNT              actionRet;


    cpssOsBzero((GT_VOID*) &ruleInfo, sizeof(ruleInfo));
    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));
    cpssOsBzero((GT_VOID*) &patternRet, sizeof(patternRet));
    cpssOsBzero((GT_VOID*) &maskRet, sizeof(maskRet));
    cpssOsBzero((GT_VOID*) &actionRet, sizeof(actionRet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E],
                                    ruleIndex [ 0 / PRV_TTI_MAX_RULE_INDEX ],
                                    actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                                    lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                                    patternPtr { pclId [0], srcIsTrunk  [0], srcPortTrunk [0], mac [0xAA, 0x0, 0x0, 0x0, 0x0, 0x0], vid[100], isTagged [GT_FALSE], tunneltype [CPSS_EXMXPM_TTI_IPV4_OVER_IPV4_TUNNEL_E],srcIp [10,10,10,1],destIp [10,10,10,2],dsaSrcPort [0], dsaSrcDevice [0] },
                                    maskPtr { ipv4.pclId = 0xFFFFFFFF, other fields are zeroed},
                                    actionPtr {set standard to default valuess },
            Expected: GT_OK.
        */
        tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

        ruleInfo.internalTcamRuleStartIndex = 0;

        keyType    = CPSS_EXMXPM_TTI_KEY_IPV4_E;
        actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
        valid      = GT_TRUE;

        ttiRuleMaskPatternDefaultSet(&mask, &pattern);
        ttiActionDefaultSet(&action);

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, tcamType, keyType, actionType, valid);

        /* Check max correct value */
        ruleInfo.internalTcamRuleStartIndex = PRV_TTI_MAX_RULE_INDEX;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, tcamType, keyType, actionType, valid);

        ruleInfo.internalTcamRuleStartIndex = 0;;  /* restore default correct value */

        /* 1.2. Call cpssExMxPmTtiRuleGet with non-NULL pointers, other params
           same as in 1.1. Expected: GT_OK and same values as written.  */

        cpssOsBzero((GT_VOID*) &patternRet, sizeof(patternRet));
        cpssOsBzero((GT_VOID*) &maskRet, sizeof(maskRet));
        cpssOsBzero((GT_VOID*) &actionRet, sizeof(actionRet));

        st = cpssExMxPmTtiRuleGet(dev, tcamType, &ruleInfo, keyType, actionType, &validRet, &maskRet, &patternRet, &actionRet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmTtiRuleGet: %d, %d, %d, %d",
                                 dev, tcamType, keyType, actionType);

        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(valid, validRet,
                 "cpssExMxPmTtiRuleGet: get another valid than was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&pattern, (GT_VOID*)&patternRet, sizeof (pattern)))? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                             "cpssExMxPmTtiRuleGet: get another pattern than was set: %d, %d", dev, keyType);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&mask, (GT_VOID*)&maskRet, sizeof (mask)))? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                             "cpssExMxPmTtiRuleGet: get another mask than was set: %d, %d", dev, keyType);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&action, (GT_VOID*)&actionRet, sizeof (action)))? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                             "cpssExMxPmTtiRuleGet: get another action than was set: %d, %d", dev, keyType);
        }

        /* 1.3. Call with out of range keyType [0x5AAAAA5], other params
          same as in 1.1. Expected: GT_BAD_PARAM.   */
        keyType = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, keyType = %d", dev, keyType);

        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

        /* 1.4. Call with out of range ruleIndex [PRV_TTI_MAX_RULE_INDEX +1],
          other params same as in 1.1. Expected: non GT_OK. */

        ruleInfo.internalTcamRuleStartIndex = PRV_TTI_MAX_RULE_INDEX + 1;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleInfo->internalTcamRuleStartIndex = %d", dev, ruleInfo.internalTcamRuleStartIndex);

        ruleInfo.internalTcamRuleStartIndex = 0;

        /*
            1.5. Call with patternPtr->ipv4.common.pclId [1024] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.ipv4.common.pclId = 1024;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pattern.ipv4.common.pclId = %d",
                                         dev, pattern.ipv4.common.pclId);

        pattern.ipv4.common.pclId = 1023;

        /*
            1.6. Call with patternPtr->ipv4.common.vid [4096] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.ipv4.common.vid = 4096;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pattern.ipv4.common.vid = %d",
                                         dev, pattern.ipv4.common.vid);

        pattern.ipv4.common.vid = 100;

        /*
            1.7. Call with patternPtr->ipv4.common.srcIsTrunk [2],
                      and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.ipv4.common.srcIsTrunk = 2;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pattern.ipv4.common.srcIsTrunk = %d",
                                         dev, pattern.ipv4.common.srcIsTrunk);

        pattern.ipv4.common.srcIsTrunk = 1;

        /*
            1.8. Call with patternPtr->ipv4.common.srcIsTrunk [0],
                      patternPtr->ipv4.common.srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                      and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.ipv4.common.srcIsTrunk   = 0;
        pattern.ipv4.common.srcPortTrunk = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pattern.ipv4.common.srcPortTrunk = %d",
                                         dev, pattern.ipv4.common.srcPortTrunk);

        pattern.ipv4.common.srcPortTrunk = 0;

        /*
            1.9. Call with patternPtr->ipv4.common.srcIsTrunk [1],
                      patternPtr->ipv4.common.srcPortTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                      and other params from 1.1.
            Expected: GT_OK.
        */
        pattern.ipv4.common.srcIsTrunk   = 1;
        pattern.ipv4.common.srcPortTrunk = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, pattern.ipv4.common.srcPortTrunk = %d",
                                     dev, pattern.ipv4.common.srcPortTrunk);

        pattern.ipv4.common.srcPortTrunk = 0;

        /*
            1.10. Call with patternPtr->ipv4.common.srcIsTrunk [1],
                      patternPtr->ipv4.common.srcPortTrunk [256]
                      and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pattern.ipv4.common.srcIsTrunk   = 1;
        pattern.ipv4.common.srcPortTrunk = 256;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pattern.ipv4.common.srcPortTrunk = %d",
                                     dev, pattern.ipv4.common.srcPortTrunk);

        pattern.ipv4.common.srcPortTrunk = 0;

        /* 1.11. Call function with ruleInfoPtr [NULL], other params
          same as in 1.1. Expected: GT_BAD_PTR. */
        st = cpssExMxPmTtiRuleSet(dev, tcamType, NULL, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ruleInfoPtr = NULL", dev);

        /* 1.11. Call function with patternPtr [NULL], other params
          same as in 1.1. Expected: GT_BAD_PTR. */
        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, NULL, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, patternPtr = NULL", dev);

        /* 1.12. Call function with maskPtr [NULL], other params same
          as in 1.1. Expected: GT_BAD_PTR.  */
        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, NULL, &pattern, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);

        /* 1.13. Call function with actionPtr [NULL], other params same as
          in 1.1. Expected: GT_BAD_PTR. */
        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);

        /* 1.14. Call with out of range action.command [0x5AAAAA5],
           other params same as in 1.1. Expected: GT_BAD_PARAM. */
        action.standard.command = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, action.standard.command = %d", dev, action.standard.command);

        action.standard.command  = CPSS_PACKET_CMD_ROUTE_E;

        /* 1.15. Call with out of range action.redirectCommand [0x5AAAAA5],
           other params same as in 1.1. Expected: GT_BAD_PARAM. */
        action.standard.redirectCommand = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, action.standard.redirectCommand = %d", dev, action.standard.redirectCommand);

        action.standard.redirectCommand = CPSS_EXMXPM_TTI_NO_REDIRECT_E;

        /* 1.16. Call with out of range action.userDefinedCpuCode [0x5AAAAA5],
           other params same as in 1.1. Expected: GT_BAD_PARAM. */
        action.standard.userDefinedCpuCode = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, action.standard.userDefinedCpuCode = %d", dev, action.standard.userDefinedCpuCode);

        action.standard.userDefinedCpuCode = CPSS_NET_FIRST_USER_DEFINED_E;

        /* 1.17. Call with out of range action.vlanPrecedence
           [0x5AAAAA5], other params same as in 1.1. Expected: GT_BAD_PARAM.*/
        action.standard.vlanPrecedence = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, action.standard.vlanPrecedence = %d", dev, action.standard.vlanPrecedence);

        action.standard.vlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

        /* 1.18. Call with out of range action.passengerPacketType
           [0x5AAAAA5], other params same as in 1.1. Expected: GT_BAD_PARAM.*/

        action.standard.passengerPacketType = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, action.standard.passengerPacketType = %d", dev, action.standard.passengerPacketType);

        action.standard.passengerPacketType = CPSS_EXMXPM_TTI_PASSENGER_IPV4_E;

        /* 1.19. Call with out of range action.mplsCmd [0x5AAAAA5], other
           params same as in 1.1. Expected: GT_BAD_PARAM.   */

        action.standard.mplsCmd = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, action.standard.mplsCmd = %d", dev, action.standard.mplsCmd);
    }

    /* set correct values*/
    tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

    ruleInfo.internalTcamRuleStartIndex = 0;

    keyType    = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
    valid      = GT_TRUE;
    ttiRuleMaskPatternDefaultSet(&mask, &pattern);
    ttiActionDefaultSet(&action);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiRuleGet
(
    IN  GT_U8                                   devNum,
    IN CPSS_EXMXPM_TCAM_TYPE_ENT                tcamType,
    IN CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT    *ruleInfoPtr,
    IN  CPSS_EXMXPM_TTI_KEY_TYPE_ENT            keyType,
    IN  CPSS_EXMXPM_TTI_ACTION_TYPE_ENT         actionType,
    OUT GT_BOOL                                 *validPtr,
    OUT CPSS_EXMXPM_TTI_RULE_UNT                *maskPtr,
    OUT CPSS_EXMXPM_TTI_RULE_UNT                *patternPtr,
    OUT CPSS_EXMXPM_TTI_ACTION_UNT              *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiRuleGet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Set rule for testing.
Call cpssExMxPmTtiRuleSet with  keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E],
                                ruleIndex [ 0 ],
                                actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                                lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                                patternPtr { pclId [0], srcIsTrunk  [0], srcPortTrunk [0], mac [0xAA, 0x0, 0x0, 0x0, 0x0, 0x0], vid[100], isTagged [GT_FALSE], tunneltype [CPSS_EXMXPM_TTI_IPV4_OVER_IPV4_TUNNEL_E],srcIp [10,10,10,1],destIp [10,10,10,2],dsaSrcPort [0], dsaSrcDevice [0] },
                                maskPtr { ipv4.pclId = 0xFFFFFFFF, other fields are zeroed},
                                actionPtr {set standard to default valuess },
Expected: GT_OK.
1.2. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E],
                        ruleIndex [ 0],
                        actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                        lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                        non-NULL patternPtr, maskPtr, actionPtr.
Expected: GT_OK.
1.3. Call with out of range keyType [0x5AAAAA5], other params same as in 1.1.
Expected: GT_BAD_PARAM.
1.4. Call with out of range ruleIndex [PRV_TTI_MAX_RULE_INDEX +1], other params same as in 1.1.
Expected: non GT_OK.

1.5. Call function with patternPtr [NULL], other params same as in 1.1.
Expected: GT_BAD_PTR.
1.6. Call function with maskPtr [NULL], other params same as in 1.1.
Expected: GT_BAD_PTR.
1.7. Call function with actionPtr [NULL], other params same as in 1.1.
Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TCAM_TYPE_ENT       tcamType   = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT    keyType    = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    CPSS_EXMXPM_TTI_ACTION_TYPE_ENT actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
    GT_BOOL                         valid      = GT_FALSE;

    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT   ruleInfo;
    CPSS_EXMXPM_TTI_RULE_UNT                pattern;
    CPSS_EXMXPM_TTI_RULE_UNT                mask;
    CPSS_EXMXPM_TTI_ACTION_UNT              action;


    cpssOsBzero((GT_VOID*) &ruleInfo, sizeof(ruleInfo));
    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E],
                                    ruleIndex [ 0 / PRV_TTI_MAX_RULE_INDEX ],
                                    actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                                    lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                                    patternPtr { pclId [0], srcIsTrunk  [0], srcPortTrunk [0], mac [0xAA, 0x0, 0x0, 0x0, 0x0, 0x0], vid[100], isTagged [GT_FALSE], tunneltype [CPSS_EXMXPM_TTI_IPV4_OVER_IPV4_TUNNEL_E],srcIp [10,10,10,1],destIp [10,10,10,2],dsaSrcPort [0], dsaSrcDevice [0] },
                                    maskPtr { ipv4.pclId = 0xFFFFFFFF, other fields are zeroed},
                                    actionPtr {set standard to default valuess },
            Expected: GT_OK.
        */
        tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

        ruleInfo.internalTcamRuleStartIndex = 0;

        keyType    = CPSS_EXMXPM_TTI_KEY_IPV4_E;
        actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
        valid      = GT_TRUE;

        ttiRuleMaskPatternDefaultSet(&mask, &pattern);
        ttiActionDefaultSet(&action);

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmTtiRuleSet: %d, %d, %d, %d, %d",
                                    dev, tcamType, keyType, actionType, valid);

        /*
            1.2. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E],
                                    ruleIndex [ 0],
                                    actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                                    lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                                    non-NULL patternPtr, maskPtr, actionPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmTtiRuleGet(dev, tcamType, &ruleInfo, keyType, actionType, &valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, tcamType, keyType, actionType);

        /* 1.3. Call with out of range keyType [0x5AAAAA5], other
           params same as in 1.1. Expected: GT_BAD_PARAM.*/
        keyType = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleGet(dev, tcamType, &ruleInfo, keyType, actionType, &valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, keyType = %d", dev, keyType);

        keyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;

        /* 1.4. Call with out of range ruleIndex [PRV_TTI_MAX_RULE_INDEX +1],
           other params same as in 1.1. Expected: non GT_OK.    */
        ruleInfo.internalTcamRuleStartIndex = PRV_TTI_MAX_RULE_INDEX + 1;

        st = cpssExMxPmTtiRuleGet(dev, tcamType, &ruleInfo, keyType, actionType, &valid, &mask, &pattern, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleInfoPtr->internalTcamRuleStartIndex = %d", dev, ruleInfo.internalTcamRuleStartIndex);

        ruleInfo.internalTcamRuleStartIndex = 0;

        /* 1.5. Call function with ruleInfoPtr [NULL], other params
           same as in 1.1. Expected: GT_BAD_PTR.    */
        st = cpssExMxPmTtiRuleGet(dev, tcamType, NULL, keyType, actionType, &valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ruleInfoPtr = NULL", dev);

        /* 1.5. Call function with validPtr [NULL], other params
           same as in 1.1. Expected: GT_BAD_PTR.    */
        st = cpssExMxPmTtiRuleGet(dev, tcamType, &ruleInfo, keyType, actionType, NULL, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);

        /* 1.5. Call function with patternPtr [NULL], other params
           same as in 1.1. Expected: GT_BAD_PTR.    */
        st = cpssExMxPmTtiRuleGet(dev, tcamType, &ruleInfo, keyType, actionType, &valid, &mask, NULL, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, patternPtr = NULL", dev);

        /* 1.6. Call function with maskPtr [NULL], other params same
           as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxPmTtiRuleGet(dev, tcamType, &ruleInfo, keyType, actionType, &valid, NULL, &pattern, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);

        /* 1.7. Call function with actionPtr [NULL], other params
           same as in 1.1. Expected: GT_BAD_PTR.    */

        st = cpssExMxPmTtiRuleGet(dev, tcamType, &ruleInfo, keyType, actionType, &valid, &mask, &pattern, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);
    }

    /* set correct values*/
    tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

    ruleInfo.internalTcamRuleStartIndex = 0;

    keyType    = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiRuleGet(dev, tcamType, &ruleInfo, keyType, actionType, &valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiRuleGet(dev, tcamType, &ruleInfo, keyType, actionType, &valid, &mask, &pattern, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiRuleValidStatusSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMXPM_TCAM_TYPE_ENT               tcamType,
    IN  CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT   *ruleInfoPtr,
    IN  GT_BOOL                                 valid
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiRuleValidStatusSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with tcamType [CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E],
                            ruleInfoPtr->internalTcamRuleStartIndex [ 0 / 255],
                            valid [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call with out of range tcamType [0x5AAAAA5]
                   other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with out of range ruleInfoPtr->internalTcamRuleStartIndex[PRV_TTI_MAX_RULE_INDEX + 1]
                       other params same as in 1.1.
    Expected: non GT_OK.
    1.4. Call function with ruleInfoPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TCAM_TYPE_ENT       tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    GT_BOOL                         valid    = GT_FALSE;
    GT_BOOL                         validRet = GT_FALSE;

    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT   ruleInfo;


    cpssOsBzero((GT_VOID*) &ruleInfo, sizeof(ruleInfo));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with tcamType [CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E],
                                    ruleInfoPtr->internalTcamRuleStartIndex [ 0 / 255],
                                    valid [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

        ruleInfo.internalTcamRuleStartIndex = 0;

        st = cpssExMxPmTtiRuleValidStatusSet(dev, tcamType, &ruleInfo, valid);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamType, valid);

        /* 1.2. Call cpssExMxPmTtiRuleGet with non-NULL pointers, other params
           same as in 1.1. Expected: GT_OK and same values as written.  */
        st = cpssExMxPmTtiRuleValidStatusGet(dev, tcamType, &ruleInfo, &validRet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTtiRuleValidStatusGet: %d, %d, %d",
                                 dev, tcamType);

        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(valid, validRet,
                 "cpssExMxPmTtiRuleGet: get another valid than was set: %d", dev);
        }

        /* 1.4. Call with out of range ruleIndex [PRV_TTI_MAX_RULE_INDEX +1],
          other params same as in 1.1. Expected: non GT_OK. */

        ruleInfo.internalTcamRuleStartIndex = PRV_TTI_MAX_RULE_INDEX + 1;

        st = cpssExMxPmTtiRuleValidStatusSet(dev, tcamType, &ruleInfo, valid);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleInfo->internalTcamRuleStartIndex = %d", dev, ruleInfo.internalTcamRuleStartIndex);

        ruleInfo.internalTcamRuleStartIndex = 0;

        /* 1.5. Call function with ruleInfoPtr [NULL], other params
          same as in 1.1. Expected: GT_BAD_PTR. */
        st = cpssExMxPmTtiRuleValidStatusSet(dev, tcamType, NULL, valid);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ruleInfoPtr = NULL", dev);
    }

    /* set correct values*/
    tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

    ruleInfo.internalTcamRuleStartIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiRuleValidStatusSet(dev, tcamType, &ruleInfo, valid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiRuleValidStatusSet(dev, tcamType, &ruleInfo, valid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiRuleValidStatusGet
(
    IN  GT_U8                                   devNum,
    IN CPSS_EXMXPM_TCAM_TYPE_ENT                tcamType,
    IN CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT    *ruleInfoPtr,
    OUT GT_BOOL                                 *validPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiRuleValidStatusGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E],
                            ruleIndex [ 0 ],
                            actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                            lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                            patternPtr { pclId [0], srcIsTrunk  [0], srcPortTrunk [0], mac [0xAA, 0x0, 0x0, 0x0, 0x0, 0x0], vid[100], isTagged [GT_FALSE], tunneltype [CPSS_EXMXPM_TTI_IPV4_OVER_IPV4_TUNNEL_E],srcIp [10,10,10,1],destIp [10,10,10,2],dsaSrcPort [0], dsaSrcDevice [0] },
                            maskPtr { ipv4.pclId = 0xFFFFFFFF, other fields are zeroed},
                            actionPtr {set standard to default valuess },
    Expected: GT_OK.
    1.2. Call cpssExMxPmTtiRuleGet with non-NULL pointers, other params same as in 1.1.
    Expected: GT_OK and same values as written.
    1.3. Call with out of range keyType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range ruleIndex [PRV_TTI_MAX_RULE_INDEX +1], other params same as in 1.1.
    Expected: non GT_OK.
    1.5. Call function with patternPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call function with maskPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.7. Call function with actionPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.8. Call with out of range actionPtr->standard.command [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with out of range actionPtr->standard.redirectCommand [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.10. Call with out of range actionPtr->standard.userDefinedCpuCode [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.11. Call with out of range actionPtr->standard.vlanPrecedence [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.12. Call with out of range actionPtr->standard.passengerPacketType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.13. Call with out of range actionPtr->standard.mplsCmd [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TCAM_TYPE_ENT       tcamType   = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT    keyType    = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    GT_BOOL                         valid      = GT_FALSE;

    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT   ruleInfo;
    CPSS_EXMXPM_TTI_RULE_UNT                mask;
    CPSS_EXMXPM_TTI_RULE_UNT                pattern;


    cpssOsBzero((GT_VOID*) &ruleInfo, sizeof(ruleInfo));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E],
                                    ruleIndex [ 0 / PRV_TTI_MAX_RULE_INDEX ],
                                    actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                                    lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                                    patternPtr { pclId [0], srcIsTrunk  [0], srcPortTrunk [0], mac [0xAA, 0x0, 0x0, 0x0, 0x0, 0x0], vid[100], isTagged [GT_FALSE], tunneltype [CPSS_EXMXPM_TTI_IPV4_OVER_IPV4_TUNNEL_E],srcIp [10,10,10,1],destIp [10,10,10,2],dsaSrcPort [0], dsaSrcDevice [0] },
                                    maskPtr { ipv4.pclId = 0xFFFFFFFF, other fields are zeroed},
                                    actionPtr {set standard to default valuess },
            Expected: GT_OK.
        */
        tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

        ruleInfo.internalTcamRuleStartIndex = 0;

        keyType    = CPSS_EXMXPM_TTI_KEY_IPV4_E;
        valid      = GT_TRUE;

        ttiRuleMaskPatternDefaultSet(&mask, &pattern);

        st = cpssExMxPmTtiRuleValidStatusSet(dev, tcamType, &ruleInfo, valid);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmTtiRuleValidStatusSet: %d, %d, %d, %d",
                                    dev, tcamType, valid);

        /*
            1.2. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E],
                                    ruleIndex [ 0],
                                    actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                                    lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                                    non-NULL patternPtr, maskPtr, actionPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmTtiRuleValidStatusGet(dev, tcamType, &ruleInfo, &valid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamType);

        /* 1.4. Call with out of range ruleIndex [PRV_TTI_MAX_RULE_INDEX +1],
           other params same as in 1.1. Expected: non GT_OK.    */
        ruleInfo.internalTcamRuleStartIndex = PRV_TTI_MAX_RULE_INDEX + 1;

        st = cpssExMxPmTtiRuleValidStatusGet(dev, tcamType, &ruleInfo, &valid);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleInfoPtr->internalTcamRuleStartIndex = %d", dev, ruleInfo.internalTcamRuleStartIndex);

        ruleInfo.internalTcamRuleStartIndex = 0;

        /* 1.5. Call function with ruleInfoPtr [NULL], other params
           same as in 1.1. Expected: GT_BAD_PTR.    */
        st = cpssExMxPmTtiRuleValidStatusGet(dev, tcamType, NULL, &valid);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ruleInfoPtr = NULL", dev);

        /* 1.5. Call function with validPtr [NULL], other params
           same as in 1.1. Expected: GT_BAD_PTR.    */
        st = cpssExMxPmTtiRuleValidStatusGet(dev, tcamType, &ruleInfo, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);
    }

    /* set correct values*/
    tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    keyType  = CPSS_EXMXPM_TTI_KEY_IPV4_E;

    ruleInfo.internalTcamRuleStartIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiRuleValidStatusGet(dev, tcamType, &ruleInfo, &valid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiRuleValidStatusGet(dev, tcamType, &ruleInfo, &valid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMXPM_TCAM_TYPE_ENT           tcamType,
    IN  CPSS_EXMXPM_TCAM_ACTION_INFO_UNT    *actionInfoPtr,
    IN  CPSS_EXMXPM_TTI_ACTION_TYPE_ENT     actionType,
    IN  CPSS_EXMXPM_TTI_ACTION_UNT          *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiRuleActionUpdate)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Set rule for testing.
    Call cpssExMxPmTtiRuleSet with  keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E],
                                    ruleIndex [ 0 ],
                                    actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                                    lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                                    patternPtr { pclId [0], srcIsTrunk  [0], srcPortTrunk [0], mac [0xAA, 0x0, 0x0, 0x0, 0x0, 0x0], vid[100], isTagged [GT_FALSE], tunneltype [CPSS_EXMXPM_TTI_IPV4_OVER_IPV4_TUNNEL_E],srcIp [10,10,10,1],destIp [10,10,10,2],dsaSrcPort [0], dsaSrcDevice [0] },
                                    maskPtr { ipv4.pclId = 0xFFFFFFFF, other fields are zeroed},
                                    action [set standard to default values]
    Expected: GT_OK.
    1.2. Call function with ruleIndex [0],
                            actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                            lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                            actionPtr { set standard to default values and
                                        standard{   command[CPSS_PACKET_CMD_MIRROR_TO_CPU_E]
                                                    redirectCommand[CPSS_EXMXPM_TTI_REDIRECT_TO_OUTLIF_E]
                                                    userDefinedCpuCode[CPSS_NET_CONTROL_E]}
                                      } (change action for Rule)
    Expected: GT_OK.
    1.3. Call cpssExMxPmTtiRuleGet with non-NULL pointers, other params same as in 1.1.
    Expected: GT_OK and same action as written.
    1.4. Call with out of range ruleIndex [PRV_TTI_MAX_RULE_INDEX +1], other params same as in 1.1.
    Expected: non GT_OK.
    1.5. Call function with actionPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call with out of range actionPtr->standard.command [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with out of range actionPtr->standard.redirectCommand [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with out of range actionPtr->standard.userDefinedCpuCode [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with out of range actionPtr->standard.vlanPrecedence [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.10. Call with out of range actionPtr->standard.passengerPacketType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.11. Call with out of range actionPtr->standard.mplsCmd [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TTI_KEY_TYPE_ENT    keyType    = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    CPSS_EXMXPM_TCAM_TYPE_ENT       tcamType   = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    CPSS_EXMXPM_TTI_ACTION_TYPE_ENT actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
    GT_BOOL                         valid      = GT_FALSE;
    GT_BOOL                         validRet   = GT_FALSE;
    GT_BOOL                         isEqual    = GT_FALSE;

    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT   ruleInfo;
    CPSS_EXMXPM_TCAM_ACTION_INFO_UNT        actionInfo;
    CPSS_EXMXPM_TTI_RULE_UNT                pattern;
    CPSS_EXMXPM_TTI_RULE_UNT                mask;
    CPSS_EXMXPM_TTI_ACTION_UNT              action;

    CPSS_EXMXPM_TTI_RULE_UNT                patternRet;
    CPSS_EXMXPM_TTI_RULE_UNT                maskRet;
    CPSS_EXMXPM_TTI_ACTION_UNT              actionRet;


    cpssOsBzero((GT_VOID*) &patternRet, sizeof(patternRet));
    cpssOsBzero((GT_VOID*) &maskRet, sizeof(maskRet));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));
    cpssOsBzero((GT_VOID*) &actionRet, sizeof(actionRet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with keyType [CPSS_EXMXPM_TTI_KEY_IPV4_E],
                                    ruleIndex [ 0 / PRV_TTI_MAX_RULE_INDEX ],
                                    actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                                    lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                                    patternPtr { pclId [0], srcIsTrunk  [0], srcPortTrunk [0], mac [0xAA, 0x0, 0x0, 0x0, 0x0, 0x0], vid[100], isTagged [GT_FALSE], tunneltype [CPSS_EXMXPM_TTI_IPV4_OVER_IPV4_TUNNEL_E],srcIp [10,10,10,1],destIp [10,10,10,2],dsaSrcPort [0], dsaSrcDevice [0] },
                                    maskPtr { ipv4.pclId = 0xFFFFFFFF, other fields are zeroed},
                                    actionPtr {set standard to default valuess },
            Expected: GT_OK.
        */
        tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

        ruleInfo.internalTcamRuleStartIndex = 0;

        keyType    = CPSS_EXMXPM_TTI_KEY_IPV4_E;
        actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
        valid      = GT_TRUE;

        ttiRuleMaskPatternDefaultSet(&mask, &pattern);
        ttiActionDefaultSet(&action);

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmTtiRuleSet: %d, %d, %d, %d, %d",
                                    dev, tcamType, keyType, actionType, valid);

        /*
            1.2. Call function with ruleIndex [0],
                                    actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                                    lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                                    actionPtr { set standard to default values and
                                                standard{   command[CPSS_PACKET_CMD_MIRROR_TO_CPU_E]
                                                            redirectCommand[CPSS_EXMXPM_TTI_REDIRECT_TO_OUTLIF_E]
                                                            userDefinedCpuCode[CPSS_NET_CONTROL_E]}
                                              } (change action for Rule)
            Expected: GT_OK.
        */
        actionInfo.internalTcamRuleStartIndex = 0;

        action.standard.command            = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        action.standard.redirectCommand    = CPSS_EXMXPM_TTI_REDIRECT_TO_OUTLIF_E;
        action.standard.userDefinedCpuCode = 0; /* can't check user userDefinedCpuCode for command mirror */

        st = cpssExMxPmTtiRuleActionUpdate(dev, tcamType, &actionInfo, actionType, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamType, actionType);

        /*
            1.3. Call cpssExMxPmTtiRuleGet with non-NULL pointers, other params same as in 1.1.
            Expected: GT_OK and same action as written.
        */
        st = cpssExMxPmTtiRuleGet(dev, tcamType, &ruleInfo, keyType, actionType, &validRet, &maskRet, &patternRet, &actionRet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmTtiRuleGet: %d, %d, %d, %d",
                                 dev, tcamType, keyType, actionType);

        if (GT_OK == st)
        {
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &action.standard, (GT_VOID*) &actionRet.standard, sizeof (action.standard)))
                  ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                             "cpssExMxPmTtiRuleGet: get another action than was set: %d", dev);
        }

        /* 1.4. Call with out of range ruleIndex [PRV_TTI_MAX_RULE_INDEX +1],
           other params same as in 1.1. Expected: non GT_OK.    */

        actionInfo.internalTcamRuleStartIndex = PRV_TTI_MAX_RULE_INDEX + 1;

        st = cpssExMxPmTtiRuleActionUpdate(dev, tcamType, &actionInfo, actionType, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleInfo.internalTcamRuleStartIndex = %d", dev, actionInfo.internalTcamRuleStartIndex);

        actionInfo.internalTcamRuleStartIndex = 0;

        /* 1.5. Call function with actionPtr [NULL], other params same as
          in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxPmTtiRuleActionUpdate(dev, tcamType, &actionInfo, actionType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);

        /* 1.5. Call function with actionInfoPtr [NULL], other params same as
          in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxPmTtiRuleActionUpdate(dev, tcamType, NULL, actionType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionInfoPtr = NULL", dev);

        /* 1.6. Call with out of range action.standard.command [0x5AAAAA5],
           other params same as in 1.1. Expected: GT_BAD_PARAM. */

        action.standard.command = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleActionUpdate(dev, tcamType, &actionInfo, actionType, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, action.standard.command = %d", dev, action.standard.command);

        action.standard.command  = CPSS_PACKET_CMD_DROP_SOFT_E;

        /* 1.7. Call with out of range action.standard.redirectCommand [0x5AAAAA5],
           other params same as in 1.1. Expected: GT_BAD_PARAM. */

        action.standard.redirectCommand = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleActionUpdate(dev, tcamType, &actionInfo, actionType, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, action.standard.redirectCommand = %d", dev, action.standard.redirectCommand);

        action.standard.redirectCommand = CPSS_EXMXPM_TTI_NO_REDIRECT_E;

        /* 1.8. Call with out of range action.standard.userDefinedCpuCode [0x5AAAAA5],
           other params same as in 1.1. Expected: GT_BAD_PARAM. */
        action.standard.userDefinedCpuCode = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleActionUpdate(dev, tcamType, &actionInfo, actionType, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, action.standard.userDefinedCpuCode = %d", dev, action.standard.userDefinedCpuCode);

        action.standard.userDefinedCpuCode = CPSS_NET_FIRST_USER_DEFINED_E;

        /* 1.9. Call with out of range action.standard.vlanPrecedence
           [0x5AAAAA5], other params same as in 1.1. Expected: GT_BAD_PARAM.*/

        action.standard.vlanPrecedence = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleActionUpdate(dev, tcamType, &actionInfo, actionType, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, action.standard.vlanPrecedence = %d", dev, action.standard.vlanPrecedence);

        action.standard.vlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

        /* 1.10. Call with out of range action.standard.passengerPacketType
           [0x5AAAAA5], other params same as in 1.1. Expected: GT_BAD_PARAM.*/

        action.standard.passengerPacketType = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleActionUpdate(dev, tcamType, &actionInfo, actionType, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, action.standard.passengerPacketType = %d", dev, action.standard.passengerPacketType);

        action.standard.passengerPacketType = CPSS_EXMXPM_TTI_PASSENGER_IPV4_E;

        /* 1.11. Call with out of range action.standard.mplsCmd [0x5AAAAA5], other
           params same as in 1.1. Expected: GT_BAD_PARAM.   */

        action.standard.mplsCmd = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleActionUpdate(dev, tcamType, &actionInfo, actionType, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, action.standard.mplsCmd = %d", dev, action.standard.mplsCmd);
    }

    /* set correct values*/
    tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

    actionInfo.internalTcamRuleStartIndex = 0;

    keyType    = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
    valid      = GT_TRUE;
    ttiActionDefaultSet(&action);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiRuleActionUpdate(dev, tcamType, &actionInfo, actionType, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiRuleActionUpdate(dev, tcamType, &actionInfo, actionType, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiRuleActionGet
(
    IN  GT_U8                                  devNum,
    IN  CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType,
    IN  CPSS_EXMXPM_TCAM_ACTION_INFO_UNT       *actionInfoPtr,
    IN  CPSS_EXMXPM_TTI_ACTION_TYPE_ENT        actionType,
    OUT CPSS_EXMXPM_TTI_ACTION_UNT             *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiRuleActionGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Set defaul rule for testing.
    Expected: GT_OK.
    1.2. Call function with actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                            lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                            actionPtr { set standard to default values and
                                        standard{   command[CPSS_PACKET_CMD_MIRROR_TO_CPU_E]
                                                    redirectCommand[CPSS_EXMXPM_TTI_REDIRECT_TO_OUTLIF_E]
                                                    userDefinedCpuCode[CPSS_NET_CONTROL_E]}
                                      } (change action for Rule)
    Expected: GT_OK.
    1.3. Call with out of range ruleIndex [PRV_TTI_MAX_RULE_INDEX +1], other params same as in 1.1.
    Expected: non GT_OK.
    1.4. Call function with actionPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with out of range actionPtr->standard.command [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TTI_KEY_TYPE_ENT    keyType    = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    CPSS_EXMXPM_TCAM_TYPE_ENT       tcamType   = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    CPSS_EXMXPM_TTI_ACTION_TYPE_ENT actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
    GT_BOOL                         valid      = GT_FALSE;

    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT   ruleInfo;
    CPSS_EXMXPM_TCAM_ACTION_INFO_UNT        actionInfo;
    CPSS_EXMXPM_TTI_RULE_UNT                pattern;
    CPSS_EXMXPM_TTI_RULE_UNT                mask;
    CPSS_EXMXPM_TTI_ACTION_UNT              action;

    CPSS_EXMXPM_TTI_RULE_UNT                patternRet;
    CPSS_EXMXPM_TTI_RULE_UNT                maskRet;
    CPSS_EXMXPM_TTI_ACTION_UNT              actionRet;


    cpssOsBzero((GT_VOID*) &patternRet, sizeof(patternRet));
    cpssOsBzero((GT_VOID*) &maskRet, sizeof(maskRet));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));
    cpssOsBzero((GT_VOID*) &actionRet, sizeof(actionRet));


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Set defaul rule for testing.
            Expected: GT_OK.
        */
        tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

        ruleInfo.internalTcamRuleStartIndex = 0;

        keyType    = CPSS_EXMXPM_TTI_KEY_IPV4_E;
        actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
        valid      = GT_TRUE;

        ttiRuleMaskPatternDefaultSet(&mask, &pattern);
        ttiActionDefaultSet(&action);

        st = cpssExMxPmTtiRuleSet(dev, tcamType, &ruleInfo, keyType, actionType, valid, &mask, &pattern, &action);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmTtiRuleSet: %d, %d, %d, %d, %d",
                                    dev, tcamType, keyType, actionType, valid);

        /*
            1.2. Call function with ruleIndex [0],
                                    actionType [CPSS_EXMXPM_TTI_ACTION_STANDARD_E],
                                    lookupMode [CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E],
                                    actionPtr { set standard to default values and
                                                standard{   command[CPSS_PACKET_CMD_MIRROR_TO_CPU_E]
                                                            redirectCommand[CPSS_EXMXPM_TTI_REDIRECT_TO_OUTLIF_E]
                                                            userDefinedCpuCode[CPSS_NET_CONTROL_E]}
                                              } (change action for Rule)
            Expected: GT_OK.
        */
        actionInfo.internalTcamRuleStartIndex = 0;

        action.standard.command            = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        action.standard.redirectCommand    = CPSS_EXMXPM_TTI_REDIRECT_TO_OUTLIF_E;
        action.standard.userDefinedCpuCode = 0; /* can't check user userDefinedCpuCode for command mirror */

        st = cpssExMxPmTtiRuleActionGet(dev, tcamType, &actionInfo, actionType, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamType, actionType);

        /* 1.3. Call with out of range ruleIndex [PRV_TTI_MAX_RULE_INDEX +1],
           other params same as in 1.1. Expected: non GT_OK.    */

        actionInfo.internalTcamRuleStartIndex = PRV_TTI_MAX_RULE_INDEX + 1;

        st = cpssExMxPmTtiRuleActionGet(dev, tcamType, &actionInfo, actionType, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ruleInfo.internalTcamRuleStartIndex = %d", dev, actionInfo.internalTcamRuleStartIndex);

        actionInfo.internalTcamRuleStartIndex = 0;

        /* 1.4. Call function with actionPtr [NULL], other params same as
          in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxPmTtiRuleActionGet(dev, tcamType, &actionInfo, actionType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);

        /* 1.5. Call function with actionInfoPtr [NULL], other params same as
          in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxPmTtiRuleActionGet(dev, tcamType, NULL, actionType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionInfoPtr = NULL", dev);
    }

    /* set correct values*/
    tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

    actionInfo.internalTcamRuleStartIndex = 0;

    keyType    = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
    valid      = GT_TRUE;
    ttiActionDefaultSet(&action);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiRuleActionGet(dev, tcamType, &actionInfo, actionType, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiRuleActionGet(dev, tcamType, &actionInfo, actionType, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiExceptionCmdSet
(
    IN  GT_U8                               dev,
    IN  CPSS_EXMXPM_TTI_EXCEPTION_ENT       exceptionType,
    IN  CPSS_PACKET_CMD_ENT                 cmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiExceptionCmdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with exceptionType [CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E / CPSS_EXMXPM_TTI_EXC_IPV4_HDR_ERR_E],
                            cmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E / CPSS_PACKET_CMD_DROP_HARD_E] .
    Expected: GT_OK.
    1.2. Call cpssExMxPmTtiExceptionCmdGet with non-NULL cmdPtr, other params same as in 1.1.
    Expected: GT_OK and same values as written.
    1.3. Call with exceptionType [CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E],
                   cmd [CPSS_PACKET_CMD_FORWARD_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_INVALID_E] (invalid) .
    Expected: GT_BAD_PARAM.
    1.4. Call with exceptionType [0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with cmd [0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    CPSS_EXMXPM_TTI_EXCEPTION_ENT  exceptionType = CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E;

    CPSS_PACKET_CMD_ENT     cmd    = CPSS_PACKET_CMD_FORWARD_E;
    CPSS_PACKET_CMD_ENT     cmdRet = CPSS_PACKET_CMD_FORWARD_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with exceptionType [CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E
          / CPSS_EXMXPM_TTI_EXC_IPV4_HDR_ERR_E], cmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E
           / CPSS_PACKET_CMD_DROP_HARD_E] . Expected: GT_OK.    */

        exceptionType = CPSS_EXMXPM_TTI_EXC_IPV4_HDR_ERR_E;
        cmd = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmTtiExceptionCmdSet(dev, exceptionType, cmd);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, exceptionType, cmd);

        exceptionType = CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E;
        cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmTtiExceptionCmdSet(dev, exceptionType, cmd);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, exceptionType, cmd);

        /* 1.2. Call cpssExMxPmTtiExceptionCmdGet with non-NULL cmdPtr, other
           params same as in 1.1. Expected: GT_OK and same values as written.*/

        st = cpssExMxPmTtiExceptionCmdGet(dev, exceptionType, &cmdRet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTtiExceptionCmdGet: %d, %d", dev, exceptionType);

        if (GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(cmd, cmdRet,
             "cpssExMxPmTtiExceptionCmdGet: get another cmd than was set: %d, %d", dev, exceptionType);
        }

        /* 1.3. Call function with exceptionType [CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E],
           cmd [CPSS_PACKET_CMD_FORWARD_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
           CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_NONE_E /
           CPSS_PACKET_CMD_INVALID_E] (invalid) . Expected: non GT_OK.  */

        exceptionType = CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E;

        cmd = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmTtiExceptionCmdSet(dev, exceptionType, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        cmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmTtiExceptionCmdSet(dev, exceptionType, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        cmd = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssExMxPmTtiExceptionCmdSet(dev, exceptionType, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;    /* restore*/

        /* 1.4. Call with out of range exceptionType [0x5AAAAA5], other
           params same as in 1.1. Expected: GT_BAD_PARAM.   */

        exceptionType = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiExceptionCmdSet(dev, exceptionType, cmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, exceptionType = %d", dev, exceptionType);

        exceptionType = CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E;

        /* 1.5. Call with out of range cmd [0x5AAAAA5], other params
           same as in 1.1. Expected: GT_BAD_PARAM.  */

        cmd = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiExceptionCmdSet(dev, exceptionType, cmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cmd = %d", dev, cmd);

        cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    }

    /* set correct values*/
    exceptionType = CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E;
    cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiExceptionCmdSet(dev, exceptionType, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiExceptionCmdSet(dev, exceptionType, cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiExceptionCmdGet
(
    IN  GT_U8                               dev,
    IN  CPSS_EXMXPM_TTI_EXCEPTION_ENT       exceptionType,
    OUT CPSS_PACKET_CMD_ENT                 *cmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiExceptionCmdGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with exceptionType [CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E / CPSS_EXMXPM_TTI_EXC_IPV4_HDR_ERR_E], non-NULL cmdPtr.
    Expected: GT_OK.
    1.2. Call with out of range exceptionType [0x5AAAAA5], other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call function with cmdPtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    CPSS_EXMXPM_TTI_EXCEPTION_ENT   exceptionType = CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E;
    CPSS_PACKET_CMD_ENT             cmd           = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with exceptionType [CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E
          / CPSS_EXMXPM_TTI_EXC_IPV4_HDR_ERR_E], non-NULL cmdPtr. Expected: GT_OK.*/

        exceptionType = CPSS_EXMXPM_TTI_EXC_IPV4_HDR_ERR_E;

        st = cpssExMxPmTtiExceptionCmdGet(dev, exceptionType, &cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, exceptionType);

        exceptionType = CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E;

        st = cpssExMxPmTtiExceptionCmdGet(dev, exceptionType, &cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, exceptionType);

        /* 1.2. Call with out of range exceptionType [0x5AAAAA5], other
           params same as in 1.1. Expected: GT_BAD_PARAM.   */

        exceptionType = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiExceptionCmdGet(dev, exceptionType, &cmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, exceptionType = %d", dev, exceptionType);

        exceptionType = CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E;

        /* 1.3. Call function with cmdPtr [NULL], other params same
           as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxPmTtiExceptionCmdGet(dev, exceptionType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cmdPtr = NULL", dev);
    }

    /* set correct values*/
    exceptionType = CPSS_EXMXPM_TTI_EXC_MPLS_UNSUPP_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiExceptionCmdGet(dev, exceptionType, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiExceptionCmdGet(dev, exceptionType, &cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiRuleMatchCounterSet
(
    IN  GT_U8                                    dev,
    IN  CPSS_EXMXPM_TTI_MATCH_COUNTER_INDEX_ENT  counterIndex,
    IN  GT_U32                                   value
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiRuleMatchCounterSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with counterIndex [CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_1_E / CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_2_E / CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_3_E],
                   value [0 / 10000 / 0xFFFFFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTtiRuleMatchCounterGet with non-NULL valuePtr, other params same as in 1.1.
    Expected: GT_OK and same values as written.
    1.3. Call with out of range counterIndex [TTI_INVALID_ENUM_CNS], other params same as in 1.1.
    Expected: non GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    CPSS_EXMXPM_TTI_MATCH_COUNTER_INDEX_ENT counterIndex = 0;
    GT_U32                                  value        = 0;
    GT_U32                                  valueRet     = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with counterIndex [CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_1_E / CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_2_E / CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_3_E],
           value [0 / 10000 / 0xFFFFFFFF]. Expected: GT_OK. */

        counterIndex = CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_1_E;
        value = 0;

        st = cpssExMxPmTtiRuleMatchCounterSet(dev, counterIndex, value);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, counterIndex, value);

        counterIndex = CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_2_E;
        value = 10000;

        st = cpssExMxPmTtiRuleMatchCounterSet(dev, counterIndex, value);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, counterIndex, value);

        counterIndex = CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_3_E;
        value = 0xFFFFFFFF;

        st = cpssExMxPmTtiRuleMatchCounterSet(dev, counterIndex, value);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, counterIndex, value);

        counterIndex = 0;
        value = 0;

        /* 1.2. Call cpssExMxPmTtiRuleMatchCounterGet with non-NULL valuePtr,
           other params same as in 1.1. Expected: GT_OK and same values as written. */

        st = cpssExMxPmTtiRuleMatchCounterGet(dev, counterIndex, &valueRet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTtiRuleMatchCounterGet: %d, %d",
                                      dev, counterIndex);

        if (GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(value, valueRet,
             "cpssExMxPmTtiRuleMatchCounterGet: get another value than was set: %d", dev);
        }

        /* 1.3. Call with out of range counterIndex [TTI_INVALID_ENUM_CNS], other params
           same as in 1.1. Expected: non GT_OK. */

        counterIndex = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleMatchCounterSet(dev, counterIndex, value);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, counterIndex = %d", dev, counterIndex);
    }

    /* set correct values*/
    counterIndex = CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_1_E;
    value = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiRuleMatchCounterSet(dev, counterIndex, value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiRuleMatchCounterSet(dev, counterIndex, value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiRuleMatchCounterGet
(
    IN  GT_U8                                    devNum,
    IN  CPSS_EXMXPM_TTI_MATCH_COUNTER_INDEX_ENT  counterIndex,
    OUT GT_U32                                   *valuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiRuleMatchCounterGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with counterIndex [CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_1_E / CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_2_E], non-NULL valuePtr.
    Expected: GT_OK.
    1.2. Call with out of range counterIndex [TTI_INVALID_ENUM_CNS], other params same as in 1.1.
    Expected: non GT_OK.
    1.3. Call function with valuePtr [NULL], other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    CPSS_EXMXPM_TTI_MATCH_COUNTER_INDEX_ENT counterIndex = 0;
    GT_U32                                  value        = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with counterIndex [CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_1_E / CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_2_E],
           non-NULL valuePtr. Expected: GT_OK.  */

        counterIndex = CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_1_E;

        st = cpssExMxPmTtiRuleMatchCounterGet(dev, counterIndex, &value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        counterIndex = CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_2_E;

        st = cpssExMxPmTtiRuleMatchCounterGet(dev, counterIndex, &value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /* 1.2. Call with out of range counterIndex [TTI_INVALID_ENUM_CNS], other params
           same as in 1.1. Expected: non GT_OK. */

        counterIndex = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleMatchCounterGet(dev, counterIndex, &value);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, counterIndex = %d", dev, counterIndex);

        counterIndex = CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_1_E;

        /* 1.3. Call function with valuePtr [NULL], other
           params same as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxPmTtiRuleMatchCounterGet(dev, counterIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, valuePtr = NULL", dev);
    }

    /* set correct values*/
    counterIndex = CPSS_EXMXPM_TTI_MATCH_COUNTER_SET_1_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiRuleMatchCounterGet(dev, counterIndex, &value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiRuleMatchCounterGet(dev, counterIndex, &value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiMimEtherTypeSet
(
    IN  GT_U8       dev,
    IN  GT_U32      ethType
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiMimEtherTypeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ethType [0 / 0xFFFF]
    Expected: GT_OK.
    1.2. Call cpssExMxPmTtiMimEthTypeGet with not NULL ethTypePtr
    Expected: GT_OK and the same ethType as was set.
    1.3. Call with ethType [0x10000] (out of range).
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      ethType    = 0;
    GT_U32      ethTypeGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ethType [0 / 0xFFFF]
            Expected: GT_OK.
        */
        /* iterate with ethType = 0 */
        ethType = 0;

        st = cpssExMxPmTtiMimEtherTypeSet(dev, ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ethType);

        /*
            1.2. Call cpssExMxPmTtiMimEthTypeGet with not NULL ethTypePtr
            Expected: GT_OK and the same ethType as was set.
        */
        st = cpssExMxPmTtiMimEthTypeGet(dev, &ethTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmTtiMimEthTypeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ethType, ethTypeGet, "got another ethType than was set: %d", dev);

        /* iterate with ethType = 0xFFFF */
        ethType = 0xFFFF;

        st = cpssExMxPmTtiMimEtherTypeSet(dev, ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ethType);

        /*
            1.2. Call cpssExMxPmTtiMimEthTypeGet with not NULL ethTypePtr
            Expected: GT_OK and the same ethType as was set.
        */
        st = cpssExMxPmTtiMimEthTypeGet(dev, &ethTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmTtiMimEthTypeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ethType, ethTypeGet, "got another ethType than was set: %d", dev);

        /*
            1.3. Call with ethType [0x10000] (out of range).
            Expected: NOT GT_OK.
        */
        ethType = 0x10000;

        st = cpssExMxPmTtiMimEtherTypeSet(dev, ethType);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ethType);
    }

    ethType = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiMimEtherTypeSet(dev, ethType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiMimEtherTypeSet(dev, ethType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiMimEthTypeGet
(
    IN  GT_U8       dev,
    OUT GT_U32      *ethTypePtr

)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiMimEthTypeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL ethTypePtr.
    Expected: GT_OK.
    1.2. Call with ethTypePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      ethType    = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL ethTypePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmTtiMimEthTypeGet(dev, &ethType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with ethTypePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTtiMimEthTypeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ethTypePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiMimEthTypeGet(dev, &ethType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiMimEthTypeGet(dev, &ethType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiRuleDefaultActionSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_EXMXPM_TTI_ACTION_QOS_STC  *defaultQosParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiRuleDefaultActionSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with defaultQosParamsPtr{
                                        qosParams{
                                                    keepPreviousQoS [GT_FALSE],
                                                    trustUp [GT_FALSE],
                                                    trustDscp [GT_FALSE],
                                                    trustExp [GT_FALSE],
                                                    enableUpToUpRemapping [GT_FALSE],
                                                    enableDscpToDscpRemapping [GT_FALSE],
                                                    enableCfiToDpMapping [GT_FALSE],
                                                    up [7],
                                                    dscp [10],
                                                    tc [7],
                                                    dp [CPSS_DP_GREEN_E],
                                                    modifyDscp [GT_FALSE],
                                                    modifyUp [GT_FALSE],
                                                    modifyExp [GT_FALSE]
                                                    qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E] }
                                         exp [0] }
    Expected: GT_OK.
    1.2. Call with cpssExMxPmTtiRuleDefaultActionGet with not NULL defaultQosParamsPtr.
    Expected: GT_OK and the same params as was set.
    1.3. Call with defaultQosParamsPtr->qosParams.up[8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with defaultQosParamsPtr->qosParams.dscp[64] and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with defaultQosParamsPtr->qosParams.tc[8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with defaultQosParamsPtr->exp[8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with defaultQosParamsPtr->qosParams.qosPrecedence[0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with defaultQosParamsPtr->qosParams.dp[0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with defaultQosParamsPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL                         isEqual = GT_FALSE;
    CPSS_EXMXPM_TTI_ACTION_QOS_STC  defaultQosParams;
    CPSS_EXMXPM_TTI_ACTION_QOS_STC  defaultQosParamsGet;


    cpssOsBzero((GT_VOID*) &defaultQosParams, sizeof(defaultQosParams));
    cpssOsBzero((GT_VOID*) &defaultQosParamsGet, sizeof(defaultQosParamsGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with defaultQosParamsPtr{
                                                qosParams{
                                                            keepPreviousQoS [GT_FALSE],
                                                            trustUp [GT_FALSE],
                                                            trustDscp [GT_FALSE],
                                                            trustExp [GT_FALSE],
                                                            enableUpToUpRemapping [GT_FALSE],
                                                            enableDscpToDscpRemapping [GT_FALSE],
                                                            enableCfiToDpMapping [GT_FALSE],
                                                            up [7],
                                                            dscp [10],
                                                            tc [7],
                                                            dp [CPSS_DP_GREEN_E],
                                                            modifyDscp [GT_FALSE],
                                                            modifyUp [GT_FALSE],
                                                            modifyExp [GT_FALSE]
                                                            qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E] }
                                                 exp [0] }
            Expected: GT_OK.
        */

        defaultQosParams.qosParams.keepPreviousQoS = GT_FALSE;
        defaultQosParams.qosParams.trustUp = GT_FALSE;
        defaultQosParams.qosParams.trustDscp = GT_FALSE;
        defaultQosParams.qosParams.trustExp = GT_FALSE;
        defaultQosParams.qosParams.enableUpToUpRemapping = GT_FALSE;
        defaultQosParams.qosParams.enableDscpToDscpRemapping = GT_FALSE;
        defaultQosParams.qosParams.enableCfiToDpMapping = GT_FALSE;
        defaultQosParams.qosParams.up = 7;
        defaultQosParams.qosParams.dscp = 10;
        defaultQosParams.qosParams.tc = 7;
        defaultQosParams.qosParams.dp = CPSS_DP_GREEN_E;
        defaultQosParams.qosParams.modifyDscp = GT_FALSE;
        defaultQosParams.qosParams.modifyUp = GT_FALSE;
        defaultQosParams.qosParams.modifyExp = GT_FALSE;
        defaultQosParams.qosParams.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        defaultQosParams.exp = 0;

        st = cpssExMxPmTtiRuleDefaultActionSet(dev, &defaultQosParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with cpssExMxPmTtiRuleDefaultActionGet with not NULL defaultQosParamsPtr.
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmTtiRuleDefaultActionGet(dev, &defaultQosParamsGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmTtiRuleDefaultActionGet: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &defaultQosParams, (GT_VOID*) &defaultQosParamsGet, sizeof(defaultQosParams) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another defaultQosParams than was set", dev);

        /*
            1.3. Call with defaultQosParamsPtr->qosParams.up[8] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        defaultQosParams.qosParams.up = 8;

        st = cpssExMxPmTtiRuleDefaultActionSet(dev, &defaultQosParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, defaultQosParams.qosParams.up = %d",
                                        dev, defaultQosParams.qosParams.up);

        defaultQosParams.qosParams.up = 7;

        /*
            1.4. Call with defaultQosParamsPtr->qosParams.dscp[64] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        defaultQosParams.qosParams.dscp = 64;

        st = cpssExMxPmTtiRuleDefaultActionSet(dev, &defaultQosParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, defaultQosParams.qosParams.dscp = %d",
                                        dev, defaultQosParams.qosParams.dscp);

        defaultQosParams.qosParams.dscp = 63;

        /*
            1.5. Call with defaultQosParamsPtr->qosParams.tc[8] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        defaultQosParams.qosParams.tc = 8;

        st = cpssExMxPmTtiRuleDefaultActionSet(dev, &defaultQosParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, defaultQosParams.qosParams.tc = %d",
                                        dev, defaultQosParams.qosParams.tc);

        defaultQosParams.qosParams.tc = 7;

        /*
            1.6. Call with defaultQosParamsPtr->exp[8] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        defaultQosParams.exp = 8;

        st = cpssExMxPmTtiRuleDefaultActionSet(dev, &defaultQosParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, defaultQosParams.exp = %d",
                                        dev, defaultQosParams.exp);

        defaultQosParams.exp = 7;

        /*
            1.7. Call with defaultQosParamsPtr->qosParams.qosPrecedence[0x5AAAAAA5] and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        defaultQosParams.qosParams.qosPrecedence = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleDefaultActionSet(dev, &defaultQosParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, defaultQosParams.qosParams.qosPrecedence = %d",
                                    dev, defaultQosParams.qosParams.qosPrecedence);

        defaultQosParams.qosParams.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;

        /*
            1.8. Call with defaultQosParamsPtr->qosParams.dp[0x5AAAAAA5] and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        defaultQosParams.qosParams.dp = TTI_INVALID_ENUM_CNS;

        st = cpssExMxPmTtiRuleDefaultActionSet(dev, &defaultQosParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, defaultQosParams.qosParams.dp = %d",
                                    dev, defaultQosParams.qosParams.dp);

        defaultQosParams.qosParams.dp = CPSS_DP_GREEN_E;

        /*
            1.9. Call with defaultQosParamsPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTtiRuleDefaultActionSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, defaultQosParamsPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiRuleDefaultActionSet(dev, &defaultQosParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiRuleDefaultActionSet(dev, &defaultQosParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTtiRuleDefaultActionGet
(
    IN  GT_U8                            devNum,
    OUT CPSS_EXMXPM_TTI_ACTION_QOS_STC  *defaultQosParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiRuleDefaultActionGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL defaultQosParamsPtr.
    Expected: GT_OK.
    1.2. Call with defaultQosParamsPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TTI_ACTION_QOS_STC  defaultQosParams;


    cpssOsBzero((GT_VOID*) &defaultQosParams, sizeof(defaultQosParams));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL defaultQosParamsPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmTtiRuleDefaultActionGet(dev, &defaultQosParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with defaultQosParamsPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTtiRuleDefaultActionGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, defaultQosParamsPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTtiRuleDefaultActionGet(dev, &defaultQosParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTtiRuleDefaultActionGet(dev, &defaultQosParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill MacToMe table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmTtiFillMacToMeTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in MacToMe table.
         Call cpssExMxPmTtiMacToMeSet with entryIndex [0..numEntries-1]
                           valid [GT_TRUE];
                           valuePtr {mac{arEther[0x10, 0x20, 0x30, 0x40, 0x50, 0x60]}, vlanId[100]},
                           maskPtr  {all set to 0xFF}
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmTtiMacToMeSet with entryIndex [numEntries]
    Expected: NOT GT_OK.
    1.4. Read all entries in MacToMe table and compare with original.
         Call cpssExMxPmTtiMacToMeGet with non-NULL valuePtr and maskPtr.
    Expected: GT_OK and same values as written.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmTtiMacToMeGet with entryIndex [numEntries].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;
    GT_BOOL     valid      = GT_TRUE;
    GT_BOOL     validGet   = GT_TRUE;

    CPSS_EXMXPM_TTI_MAC_VLAN_STC       value;
    CPSS_EXMXPM_TTI_MAC_VLAN_STC       mask;
    CPSS_EXMXPM_TTI_MAC_VLAN_STC       valueGet;
    CPSS_EXMXPM_TTI_MAC_VLAN_STC       maskGet;


    cpssOsBzero((GT_VOID*) &value, sizeof(value));
    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsBzero((GT_VOID*) &valueGet, sizeof(valueGet));
    cpssOsBzero((GT_VOID*) &maskGet, sizeof(maskGet));

    /* Fill the entry for MacToMe table */
    valid      = GT_TRUE;
    value.mac.arEther[0] = 0x10;
    value.mac.arEther[1] = 0x20;
    value.mac.arEther[2] = 0x30;
    value.mac.arEther[3] = 0x40;
    value.mac.arEther[4] = 0x50;
    value.mac.arEther[5] = 0x60;
    value.vlanId         = 100;

    mask.mac.arEther[0] = 0xFF;
    mask.mac.arEther[1] = 0xFF;
    mask.mac.arEther[2] = 0xFF;
    mask.mac.arEther[3] = 0xFF;
    mask.mac.arEther[4] = 0xFF;
    mask.mac.arEther[5] = 0xFF;
    mask.vlanId = 0xFFF;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 256;

        /* 1.2. Fill all entries in MacToMe table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unical */
            value.vlanId = (GT_U16)(iTemp % 4095);

            st = cpssExMxPmTtiMacToMeSet(dev, iTemp, valid, &value, &mask);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmTtiMacToMeSet: %d, %d, %d", dev, iTemp, valid);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmTtiMacToMeSet(dev, numEntries, valid, &value, &mask);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmTtiMacToMeSet: %d, %d, %d", dev, numEntries, valid);

        /* 1.4. Read all entries in MacToMe table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unical entry before compare */
            value.vlanId = (GT_U16)(iTemp % 4095);

            st = cpssExMxPmTtiMacToMeGet(dev, iTemp, &validGet, &valueGet, &maskGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTtiMacToMeGet: %d, %d", dev, iTemp);

            UTF_VERIFY_EQUAL1_STRING_MAC(valid, validGet, "got another valid than was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &value, (GT_VOID*) &valueGet, sizeof(value) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another value than was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask, (GT_VOID*) &maskGet, sizeof(mask) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another mask than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmTtiMacToMeGet(dev, numEntries, &validGet, &valueGet, &maskGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTtiMacToMeGet: %d, %d", dev, iTemp);
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmTti suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmTti)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiPortLookupModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiPortLookupModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiPortLookupConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiPortLookupConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiMacModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiMacModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiIpv4McEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiIpv4McEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiIpv4GreEthTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiIpv4GreEthTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiPortMacToMeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiPortMacToMeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiMacToMeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiMacToMeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiPortLookupEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiPortLookupEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiPortIpv4OnlyTunneledEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiRuleSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiRuleGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiRuleValidStatusSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiRuleValidStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiRuleActionUpdate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiRuleActionGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiExceptionCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiExceptionCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiRuleMatchCounterSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiRuleMatchCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiMimEtherTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiMimEthTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiRuleDefaultActionSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiRuleDefaultActionGet)
    /* Test for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTtiFillMacToMeTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmTti)


/*----------------------------------------------------------------------------*/

/*  Internal function. Is used for filling TTI action structure     */
/*  with default values which are used for most of all tests.       */
static void ttiActionDefaultSet
(
    OUT CPSS_EXMXPM_TTI_ACTION_UNT     *actionPtr
)
{
/*
    actionPtr->command  = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    actionPtr->redirectCommand = CPSS_EXMXPM_TTI_NO_REDIRECT_E;
    actionPtr->userDefinedCpuCode = CPSS_NET_FIRST_USER_DEFINED_E;
    actionPtr->ipNextHopIndex = 0;
    actionPtr->tunnelTerminateEnable = GT_FALSE;
    actionPtr->passengerPacketType = CPSS_EXMXPM_TTI_PASSENGER_IPV4_E;
    actionPtr->mplsCmd = CPSS_EXMXPM_TTI_MPLS_NOP_COMMAND_E;
    actionPtr->Exp = 5;
    actionPtr->setExp = GT_FALSE;
    actionPtr->enableDecTtl = GT_TRUE;
    actionPtr->mplsLabel = 5;
    actionPtr->ttl = 30;
    actionPtr->copyTtlFromTunnelHeader = GT_FALSE;
    actionPtr->pwId = 0;
    actionPtr->sourceIsPE = GT_FALSE;
    actionPtr->enableSourceLocalFiltering = GT_FALSE;
    actionPtr->floodDitPointer = 0;
    actionPtr->inLifVidCmd = CPSS_EXMXPM_TTI_VLAN_INLIF_DO_NOT_MODIFY_E;
    actionPtr->vlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    actionPtr->nestedVlanEnable = GT_FALSE;
    actionPtr->counterEnable = GT_FALSE;
    actionPtr->meterEnable = GT_FALSE;
    actionPtr->policerIndex = 0;
    actionPtr->mirrorToIngressAnalyzerEnable = GT_FALSE;
    actionPtr->inLifVidId = 5;
    actionPtr->flowId = 0;
    actionPtr->sstIdEnable = GT_FALSE;
    actionPtr->sstId = 5;
*/
    cpssOsBzero((GT_VOID*)actionPtr, sizeof(actionPtr));

    actionPtr->standard.command  = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    actionPtr->standard.redirectCommand = CPSS_EXMXPM_TTI_NO_REDIRECT_E;
    actionPtr->standard.userDefinedCpuCode = CPSS_NET_FIRST_USER_DEFINED_E;
    actionPtr->standard.ipNextHopIndex = 0;
    actionPtr->standard.tunnelTerminateEnable = GT_FALSE;
    actionPtr->standard.passengerPacketType = CPSS_EXMXPM_TTI_PASSENGER_IPV4_E;
    actionPtr->standard.mplsCmd = CPSS_EXMXPM_TTI_MPLS_NOP_COMMAND_E;
    actionPtr->standard.exp = 5;
    actionPtr->standard.setExp = GT_FALSE;
    actionPtr->standard.enableDecTtl = GT_TRUE;
    actionPtr->standard.mplsLabel = 5;
    actionPtr->standard.ttl = 30;
    actionPtr->standard.copyTtlFromTunnelHeader = GT_FALSE;
    actionPtr->standard.pwId = 0;
    actionPtr->standard.sourceIsPE = GT_FALSE;
    actionPtr->standard.enableSourceLocalFiltering = GT_FALSE;
    actionPtr->standard.floodDitPointer = 0;
    actionPtr->standard.inLifVidCmd = CPSS_EXMXPM_TTI_VLAN_INLIF_DO_NOT_MODIFY_E;
    actionPtr->standard.vlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    actionPtr->standard.nestedVlanEnable = GT_FALSE;
    actionPtr->standard.counterEnable = GT_FALSE;
    actionPtr->standard.meterEnable = GT_FALSE;
    actionPtr->standard.policerIndex = 0;
    actionPtr->standard.mirrorToIngressAnalyzerEnable = GT_FALSE;
    actionPtr->standard.inLifVidId = 5;
    actionPtr->standard.flowId = 0;
    actionPtr->standard.sstIdEnable = GT_FALSE;
    actionPtr->standard.sstId = 5;
}

/*----------------------------------------------------------------------------*/

/*  Internal function. Is used for filling TTI mask & pattern structure     */
/*  with default values which are used for most of all tests.               */

static void ttiRuleMaskPatternDefaultSet
(
    OUT CPSS_EXMXPM_TTI_RULE_UNT     *maskPtr,
    OUT CPSS_EXMXPM_TTI_RULE_UNT     *patternPtr
)
{
    GT_ETHERADDR                         mac1 = {{0xAA, 0, 0, 0, 0, 0}};

    /* maskPtr */
    cpssOsBzero((GT_VOID*)maskPtr, sizeof(CPSS_EXMXPM_TTI_RULE_UNT));
    maskPtr->ipv4.common.pclId = 0x3FF;
    maskPtr->ipv4.common.srcIsTrunk = 0x1;
    maskPtr->ipv4.common.srcPortTrunk = 2;
    maskPtr->ipv4.common.mac.arEther[0] = 0xFF;
    maskPtr->ipv4.common.mac.arEther[1] = 0xFF;
    maskPtr->ipv4.common.mac.arEther[2] = 0xFF;
    maskPtr->ipv4.common.mac.arEther[3] = 0xFF;
    maskPtr->ipv4.common.mac.arEther[4] = 0xFF;
    maskPtr->ipv4.common.mac.arEther[5] = 0xFF;
    maskPtr->ipv4.common.vid = 0xFFF;
    maskPtr->ipv4.common.isTagged = 0x1;
    maskPtr->ipv4.common.dsaSrcIsTrunk = 0x1;
    maskPtr->ipv4.common.dsaSrcPortTrunk = 2;
    maskPtr->ipv4.common.dsaSrcDevice = 0x1F;

    maskPtr->ipv4.tunneltype = 0x7;
    maskPtr->ipv4.srcIp.u32Ip = 0xFFFFFFFF;
    maskPtr->ipv4.destIp.u32Ip = 0xFFFFFFFF;

    maskPtr->ipv4.isArp = 0x1;

    /* patternPtr */
    cpssOsBzero((GT_VOID*)patternPtr, sizeof(CPSS_EXMXPM_TTI_RULE_UNT));
    patternPtr->ipv4.common.pclId = 0;
    patternPtr->ipv4.common.srcIsTrunk = 0;
    patternPtr->ipv4.common.srcPortTrunk = 0;
    patternPtr->ipv4.common.mac = mac1;

    patternPtr->ipv4.common.vid = 100;
    patternPtr->ipv4.common.isTagged = GT_FALSE;
    patternPtr->ipv4.common.dsaSrcIsTrunk = GT_FALSE;
    patternPtr->ipv4.common.dsaSrcPortTrunk = 0;
    patternPtr->ipv4.common.dsaSrcDevice = 0;

    patternPtr->ipv4.tunneltype = CPSS_EXMXPM_TTI_IPV4_OVER_IPV4_TUNNEL_E;
    patternPtr->ipv4.srcIp.arIP[0] = 10;  /* [10,10,10,1] */
    patternPtr->ipv4.srcIp.arIP[1] = 10;
    patternPtr->ipv4.srcIp.arIP[2] = 10;
    patternPtr->ipv4.srcIp.arIP[3] = 1;
    patternPtr->ipv4.destIp.arIP[0] = 10;  /* [10,10,10,2] */
    patternPtr->ipv4.destIp.arIP[1] = 10;
    patternPtr->ipv4.destIp.arIP[2] = 10;
    patternPtr->ipv4.destIp.arIP[3] = 2;

    patternPtr->ipv4.isArp = GT_FALSE;
}

