/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxBrgGenUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxBrgGen.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgGen.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* From cpssExMxBrgGen.c code!!!!           */
/* Be careful to keep them synchronized!    */
#define NUMBER_OF_TRAFIC_CLASSES_CNS            8

/* From cpssExMxBrgGen.c code!!!!           */
/* maximum value of broadcast rate limit    */
/* Be careful to keep them synchronized!    */
#define MAX_RATE_LIMIT_VALUE_CNS                0xffff


/* Tests use this value to check invalid enumeration */
#define BRG_GEN_INVALID_ENUM_CNS                0x5AAAAAA5

/* Valid value for physical port                    */
#define BRG_GEN_VALID_PHY_PORT_CNS              0

/* Valid value for virtual port                     */
#define BRG_GEN_VALID_VIRT_PORT_CNS             0

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgGenBrdPortLimitSet
(
    IN GT_U8                    dev,
    IN GT_U8                    port,
    IN CPSS_EXMX_RATE_MODE_ENT  rMode,
    IN GT_U32                   rateValue,
    IN GT_U32                   window
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgGenBrdPortLimitSet)
{
    GT_STATUS               st = GT_OK;

    GT_U8                   dev;
    GT_U8                   port;
    GT_U32                  rateValue;
    GT_U32                  window;

    /* prepare active device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* For all active devices goes over all available physical ports. */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* Call function for each rate calculation mode          */
            /* with correct rate value and time window parameters.          */

            /* check CPSS_RATE_LIMIT_PCKT_BASED_E mode */
            rateValue = 10;
            window = 20;

            st = cpssExMxBrgGenBrdPortLimitSet(dev, port, CPSS_RATE_LIMIT_PCKT_BASED_E, rateValue, window);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, CPSS_RATE_LIMIT_PCKT_BASED_E, rateValue, window);

            /* check CPSS_RATE_LIMIT_BYTE_BASED_E mode */
            st = cpssExMxBrgGenBrdPortLimitSet(dev, port, CPSS_RATE_LIMIT_BYTE_BASED_E, rateValue, window);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, CPSS_RATE_LIMIT_BYTE_BASED_E, rateValue, window);
        }

        /* Call function with invalid rMode [0x5AAAAAA5].   */
        /* Must return GT_BAD_PARAM.                        */
        port = BRG_GEN_VALID_PHY_PORT_CNS;
        rateValue = 5;
        window = 43;

        st = cpssExMxBrgGenBrdPortLimitSet(dev, port, BRG_GEN_INVALID_ENUM_CNS, rateValue, window);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, port, BRG_GEN_INVALID_ENUM_CNS, rateValue, window);

        /* Call function with out of bound value for rate value and */
        /* time window parameters                                        */
        rateValue = 23;
        window = 0xFFFFFFF2;

        st = cpssExMxBrgGenBrdPortLimitSet(dev, port, CPSS_RATE_LIMIT_BYTE_BASED_E, rateValue, window);
        UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, CPSS_RATE_LIMIT_BYTE_BASED_E, rateValue, window);

        rateValue = 0xFFFFFFF0;
        window = 15;

        st = cpssExMxBrgGenBrdPortLimitSet(dev, port, CPSS_RATE_LIMIT_BYTE_BASED_E, rateValue, window);
        UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, CPSS_RATE_LIMIT_BYTE_BASED_E, rateValue, window);


        /* For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                      */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        rateValue = 0;
        window = 0;

        st = cpssExMxBrgGenBrdPortLimitSet(dev, port, CPSS_RATE_LIMIT_BYTE_BASED_E, rateValue, window);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, port, CPSS_RATE_LIMIT_BYTE_BASED_E, rateValue, window);

        /* For active device check that function returns GT_BAD_PARAM */
        /* for non configured physical ports                            */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        rateValue = 0;
        window = 0;

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssExMxBrgGenBrdPortLimitSet(dev, port, CPSS_RATE_LIMIT_BYTE_BASED_E, rateValue, window);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, port, CPSS_RATE_LIMIT_BYTE_BASED_E, rateValue, window);
        }
    }

    /* For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    port = BRG_GEN_VALID_PHY_PORT_CNS;
    rateValue = 0;
    window = 0;

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgGenBrdPortLimitSet(dev, port, CPSS_RATE_LIMIT_BYTE_BASED_E, rateValue, window);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, port, CPSS_RATE_LIMIT_BYTE_BASED_E, rateValue, window);
    }

    /* Call function with out of bound value for device id and correct   */
    /* value for port.                                                      */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    port = BRG_GEN_VALID_PHY_PORT_CNS;
    rateValue = 0;
    window = 0;

    st = cpssExMxBrgGenBrdPortLimitSet(dev, port, CPSS_RATE_LIMIT_BYTE_BASED_E, rateValue, window);
    UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, port, CPSS_RATE_LIMIT_BYTE_BASED_E, rateValue, window);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgGenBcLimitDisable
(
    IN GT_U8        dev,
    IN GT_U8        port
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgGenBcLimitDisable)
{
    GT_STATUS st = GT_OK;

    GT_U8 dev;
    GT_U8 port;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function for each port */
            st = cpssExMxBrgGenBcLimitDisable(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each invalid port */
            st = cpssExMxBrgGenBcLimitDisable(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                      */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxBrgGenBcLimitDisable(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    port = BRG_GEN_VALID_PHY_PORT_CNS;

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgGenBcLimitDisable(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    port = BRG_GEN_VALID_PHY_PORT_CNS;

    st = cpssExMxBrgGenBcLimitDisable(dev, port);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev ,port);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgGenIgmpSnoopEnable
(
    IN GT_U8        dev,
    IN GT_U8        port,
    IN GT_BOOL      status
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgGenIgmpSnoopEnable)
{
    GT_STATUS st = GT_OK;

    GT_U8 dev;
    GT_U8 port;
    GT_BOOL status;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function for each port */
            status = GT_TRUE;
            st = cpssExMxBrgGenIgmpSnoopEnable(dev, port, status);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, status);

            status = GT_FALSE;
            st = cpssExMxBrgGenIgmpSnoopEnable(dev, port, status);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, status);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each invalid port */
            status = GT_TRUE;
            st = cpssExMxBrgGenIgmpSnoopEnable(dev, port, status);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status);

            status = GT_FALSE;
            st = cpssExMxBrgGenIgmpSnoopEnable(dev, port, status);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                      */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        status = GT_TRUE;

        st = cpssExMxBrgGenIgmpSnoopEnable(dev, port, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    status = GT_TRUE;
    port = BRG_GEN_VALID_PHY_PORT_CNS;

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgGenIgmpSnoopEnable(dev, port, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    port = BRG_GEN_VALID_PHY_PORT_CNS;
    status = GT_TRUE;

    st = cpssExMxBrgGenIgmpSnoopEnable(dev, port, status);
    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev ,port, status);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgGenDropIpMcEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  state
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgGenDropIpMcEnable)
{
    GT_STATUS st = GT_OK;

    GT_U8 dev;
    GT_BOOL state;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1.1. Call function for with state GT_TRUE and GT_FALSE */
        state = GT_TRUE;
        st = cpssExMxBrgGenDropIpMcEnable(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        state = GT_FALSE;
        st = cpssExMxBrgGenDropIpMcEnable(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    state = GT_TRUE;

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgGenDropIpMcEnable(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, state);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    state = GT_TRUE;

    st = cpssExMxBrgGenDropIpMcEnable(dev, state);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, state);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgGenDropNonIpMcEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  state
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgGenDropNonIpMcEnable)
{
    GT_STATUS st = GT_OK;

    GT_U8 dev;
    GT_BOOL state;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1.1. Call function for with state GT_TRUE and GT_FALSE */
        state = GT_TRUE;
        st = cpssExMxBrgGenDropNonIpMcEnable(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        state = GT_FALSE;
        st = cpssExMxBrgGenDropNonIpMcEnable(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    state = GT_TRUE;

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgGenDropNonIpMcEnable(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, state);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    state = GT_TRUE;

    st = cpssExMxBrgGenDropNonIpMcEnable(dev, state);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, state);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgGenArpIgmpTrapTcSet
(
    IN GT_U8        dev,
    IN GT_U8        trClass
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgGenArpIgmpTrapTcSet)
{
    GT_STATUS st = GT_OK;

    GT_U8 dev;
    GT_U8 trClass;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all trClass from 0 to NUMBER_OF_TRAFIC_CLASSES_CNS */
        for (trClass = 0; trClass < NUMBER_OF_TRAFIC_CLASSES_CNS; ++trClass)
        {
            /* 1.1.1. Call function for each traffic class value */
            st = cpssExMxBrgGenArpIgmpTrapTcSet(dev, trClass);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trClass);
        }

        /* 1.2. Call function with out of bound value for trClass */
        trClass = NUMBER_OF_TRAFIC_CLASSES_CNS;

        st = cpssExMxBrgGenArpIgmpTrapTcSet(dev, trClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, trClass);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trClass = 0;

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgGenArpIgmpTrapTcSet(dev, trClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, trClass);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    trClass = 0;

    st = cpssExMxBrgGenArpIgmpTrapTcSet(dev, trClass);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev ,trClass);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgGenControlPktTcSet
(
    IN GT_U8        dev,
    IN GT_U8        trClass
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgGenControlPktTcSet)
{
    GT_STATUS st = GT_OK;

    GT_U8 dev;
    GT_U8 trClass;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all trClass (from 0 to NUMBER_OF_TRAFIC_CLASSES_CNS-1). Must return GT_OK. */
        for (trClass = 0; trClass < NUMBER_OF_TRAFIC_CLASSES_CNS; ++trClass)
        {
            st = cpssExMxBrgGenControlPktTcSet(dev, trClass);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trClass);
        }

        /* 1.2. Call function with out of bound value for trClass. Must return GT_BAD_PARAM. */
        trClass = NUMBER_OF_TRAFIC_CLASSES_CNS;

        st = cpssExMxBrgGenControlPktTcSet(dev, trClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, trClass);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trClass = NUMBER_OF_TRAFIC_CLASSES_CNS -1; /* any correct value for trClass */
    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgGenControlPktTcSet(dev, trClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, trClass);
    }

    /* 3. Call function with out of bound value for device id. Must return GT_BAD_PARAM. */

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    trClass = 0; /* any correct value for trClass */

    st = cpssExMxBrgGenControlPktTcSet(dev, trClass);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, trClass);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgGenMcRateLimitEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgGenMcRateLimitEnable)
{
    GT_STATUS st = GT_OK;

    GT_U8 dev;
    GT_BOOL enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with enable = GT_TRUE and GT_FALSE. Must return GT_OK. */
        enable = GT_TRUE;
        st = cpssExMxBrgGenMcRateLimitEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_FALSE;
        st = cpssExMxBrgGenMcRateLimitEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enable = GT_TRUE;

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgGenMcRateLimitEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, enable);
    }

    /* 3. Call function with out of bound value for device id. Must return GT_BAD_PARAM. */

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    enable = GT_FALSE;

    st = cpssExMxBrgGenMcRateLimitEnable(dev, enable);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, enable);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgGenUnkRateLimitPpEnable
(
    IN GT_U8                    dev,
    IN CPSS_EXMX_RATE_MODE_ENT  rMode,
    IN GT_U32                   rValue,
    IN GT_U32                   window
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgGenUnkRateLimitPpEnable)
{
    GT_STATUS               st = GT_OK;

    GT_U8                       dev;
    CPSS_RATE_LIMIT_MODE_ENT    rMode;
    GT_U32                      rValue;
    GT_U32                      window;

    CPSS_PP_FAMILY_TYPE_ENT     devFamily;


    /* prepare active device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet");

        /* 1.1. Call function with rate calculation mode         */
        /* [CPSS_RATE_LIMIT_BYTE_BASED_E], rate value [12] and time    */
        /* window [32]. Expected: GT_OK for Tiger family devices */
        /* and NON GT_OK for others.                             */
        rMode = CPSS_RATE_LIMIT_BYTE_BASED_E;

        rValue = 12;
        window = 32;
        st = cpssExMxBrgGenUnkRateLimitPpEnable(dev, rMode, rValue, window);

        /* only tiger support the "packet based" */
        /*
        if(st != GT_BAD_PARAM)
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st,
                                        dev, rMode, rValue, window);
        }
        */

        /* Check for error code is replaced by check for device family  */
        /* because it suites requirements better                        */

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, 
                                         "Tiger device: %d, %d, %d, %d",
                                         dev, rMode, rValue, window);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, 
                                            "non Tiger device: %d, %d, %d, %d",
                                            dev, rMode, rValue, window);
        }

        /* 1.2. Call function with rate calculation mode  */
        /* [CPSS_RATE_LIMIT_PCKT_BASED_E], rate value [12] and  */
        /* time window [32]. Expected: GT_OK              */
        rMode = CPSS_RATE_LIMIT_PCKT_BASED_E;

        st = cpssExMxBrgGenUnkRateLimitPpEnable(dev, rMode, rValue, window);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st,
                                    dev, rMode, rValue, window);

        /* 1.3. Call function with out of bound value for rate value    */
        /* and time window parameters                                   */
        window = 0xFFFFFFF1; /* There are not max constant for window value:*/
                             /* in sources there are some calculations on   */
                             /* window value performing and then testing on */
                             /* this changed value is performing.           */
                             /* That's why we use explicit value but not    */
                             /* predefined constant for out of range window */
                             /* value.                                      */

        rValue = 11;
        rMode = CPSS_RATE_LIMIT_PCKT_BASED_E; /* this mode suites all devices */

        st = cpssExMxBrgGenUnkRateLimitPpEnable(dev, rMode, rValue, window);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st,
                                        dev, rMode, rValue, window);

        rValue = MAX_RATE_LIMIT_VALUE_CNS + 1; /* there is following testing */
                               /* in sources:                                */
                               /* if (rateValue > MAX_RATE_LIMIT_VALUE_CNS)  */
                               /* return GT_OUT_OF_RANGE;                    */
                               /* that's why we are tesing not for max       */
                               /* constant, but for [max constant]+1         */

        window = 15;
        rMode = CPSS_RATE_LIMIT_PCKT_BASED_E; /* this mode suites all devices */

        st = cpssExMxBrgGenUnkRateLimitPpEnable(dev, rMode, rValue, window);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st,
                                        dev, rMode, rValue, window);

        /* 1.4. Call function with out of range rMode [0x5AAAAAA5].  */
        /* Expected: GT_BAD_PARAM.                                   */

        rMode = BRG_GEN_INVALID_ENUM_CNS;
        rValue = 11;
        /* window == 15 */

        st = cpssExMxBrgGenUnkRateLimitPpEnable(dev, rMode, rValue, window);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st,
                                    dev, rMode, rValue, window);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    rValue = 2; /* any correct value */
    window = 1; /* any correct value */
    rMode = CPSS_RATE_LIMIT_PCKT_BASED_E; /* this mode suites all devices */

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgGenUnkRateLimitPpEnable(dev, rMode, rValue, window);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st,
                                        dev, rMode, rValue, window);
    }

    /* 3. Call function with out of bound value for device id and correct   */
    /* values of other parameters.                                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    rValue = 4; /* any correct value */
    window = 23; /* any correct value */
    rMode = CPSS_RATE_LIMIT_PCKT_BASED_E; /* this mode suites all devices */

    st = cpssExMxBrgGenUnkRateLimitPpEnable(dev, rMode, rValue, window);
    UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st,
                                    dev, rMode, rValue, window);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgGenUnkRateLimitPpDisable
(
    IN GT_U8    dev
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgGenUnkRateLimitPpDisable)
{
    GT_STATUS st = GT_OK;

    GT_U8 dev;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function. Must return GT_OK. */
        st = cpssExMxBrgGenUnkRateLimitPpDisable(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgGenUnkRateLimitPpDisable(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. Must return GT_BAD_PARAM. */

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgGenUnkRateLimitPpDisable(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgGenBcRateLimitForArpBc
(
    IN GT_U8   dev,
    IN GT_BOOL enable
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgGenBcRateLimitForArpBc)
{
    GT_STATUS st = GT_OK;

    GT_U8 dev;
    GT_BOOL enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with enable = GT_TRUE and GT_FALSE. Must return GT_OK. */
        enable = GT_TRUE;
        st = cpssExMxBrgGenBcRateLimitForArpBc(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_FALSE;
        st = cpssExMxBrgGenBcRateLimitForArpBc(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enable = GT_TRUE;

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgGenBcRateLimitForArpBc(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, enable);
    }

    /* 3. Call function with out of bound value for device id. Must return GT_BAD_PARAM. */

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    enable = GT_FALSE;

    st = cpssExMxBrgGenBcRateLimitForArpBc(dev, enable);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, enable);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgGenLocalSwitchingEnable
(
    IN GT_U8        dev,
    IN GT_U8        port,
    IN GT_BOOL      localSwitchEn
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgGenLocalSwitchingEnable)
{
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL localSwitchEn;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function for each port                             */
            /* Check for localSwitchEn = GT_TRUE and localSwitchEn = GT_FALSE.*/
            localSwitchEn = GT_TRUE;
            st = cpssExMxBrgGenLocalSwitchingEnable(dev, port, localSwitchEn);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, localSwitchEn);

            localSwitchEn = GT_FALSE;
            st = cpssExMxBrgGenLocalSwitchingEnable(dev, port, localSwitchEn);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, localSwitchEn);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.             */
            /* Check for localSwitchEn = GT_TRUE and localSwitchEn = GT_FALSE.*/
            localSwitchEn = GT_TRUE;
            st = cpssExMxBrgGenLocalSwitchingEnable(dev, port, localSwitchEn);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, localSwitchEn);

            localSwitchEn = GT_FALSE;
            st = cpssExMxBrgGenLocalSwitchingEnable(dev, port, localSwitchEn);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, localSwitchEn);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                      */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        localSwitchEn = GT_TRUE;

        st = cpssExMxBrgGenLocalSwitchingEnable(dev, port, localSwitchEn);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, localSwitchEn);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM.*/

    localSwitchEn = GT_TRUE;
    port = BRG_GEN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgGenLocalSwitchingEnable(dev, port, localSwitchEn);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, localSwitchEn);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    port = BRG_GEN_VALID_PHY_PORT_CNS;
    localSwitchEn = GT_TRUE;

    st = cpssExMxBrgGenLocalSwitchingEnable(dev, port, localSwitchEn);
    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev ,port, localSwitchEn);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgGenArpTrapEnable
(
    IN GT_U8        dev,
    IN GT_U8        port,
    IN GT_BOOL      status
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgGenArpTrapEnable)
{
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     status;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare virtual port iterator */
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function for each port                             */
            /* Check for status = GT_TRUE and status = GT_FALSE.*/
            status = GT_TRUE;
            st = cpssExMxBrgGenArpTrapEnable(dev, port, status);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, status);

            status = GT_FALSE;
            st = cpssExMxBrgGenArpTrapEnable(dev, port, status);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, status);
        }

        /* prepare virtual port iterator */
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.             */
            /* Check for status = GT_TRUE and status = GT_FALSE.*/
            status = GT_TRUE;
            st = cpssExMxBrgGenArpTrapEnable(dev, port, status);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status);

            status = GT_FALSE;
            st = cpssExMxBrgGenArpTrapEnable(dev, port, status);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                        */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        status = GT_TRUE;

        st = cpssExMxBrgGenArpTrapEnable(dev, port, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM.*/

    status = GT_TRUE;
    port = BRG_GEN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgGenArpTrapEnable(dev, port, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    port = BRG_GEN_VALID_VIRT_PORT_CNS;
    status = GT_TRUE;

    st = cpssExMxBrgGenArpTrapEnable(dev, port, status);
    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev ,port, status);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cppExMxBrgGen suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxBrgGen)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgGenBrdPortLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgGenBcLimitDisable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgGenIgmpSnoopEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgGenDropIpMcEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgGenDropNonIpMcEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgGenArpIgmpTrapTcSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgGenControlPktTcSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgGenMcRateLimitEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgGenUnkRateLimitPpEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgGenUnkRateLimitPpDisable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgGenBcRateLimitForArpBc)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgGenLocalSwitchingEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgGenArpTrapEnable)
UTF_SUIT_END_TESTS_MAC(cpssExMxBrgGen)

