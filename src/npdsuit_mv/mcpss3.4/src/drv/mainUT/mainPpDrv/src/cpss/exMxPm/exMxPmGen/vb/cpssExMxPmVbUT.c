/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmVbUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmVb that provides
*       CPSS ExMxPm Value Blade (Virtual Port) support API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/vb/cpssExMxPmVb.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Invalid enum */
#define VB_INVALID_ENUM_CNS     0x5AAAAAA5

/* Default valid value for port id */
#define VB_VALID_PHY_PORT_CNS   0

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbPortCascadingModeSet
(
    IN    GT_U8                       devNum,
    IN    GT_U8                       physicalPortNum,
    IN    CPSS_DIRECTION_ENT          direction,
    IN    CPSS_CSCD_PORT_TYPE_ENT     mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbPortCascadingModeSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with direction [CPSS_DIRECTION_INGRESS_E / CPSS_DIRECTION_EGRESS_E],
                   mode [CPSS_CSCD_PORT_DSA_MODE_REGULAR_E / CPSS_CSCD_PORT_DSA_MODE_EXTEND_E],
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmVbPortCascadingModeGet with not NULL modePtr and direction from 1.1.
    Expected :GT_OK and the same mode as was set.
    1.1.3. Call with direction [0x5AAAAAA5] (out of range).
    Expected: GT_BAD_PARAM.
    1.1.4. Call with mode [0x5AAAAAA5] (out of range).
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    CPSS_DIRECTION_ENT      direction = CPSS_DIRECTION_INGRESS_E;
    CPSS_CSCD_PORT_TYPE_ENT mode = CPSS_CSCD_PORT_DSA_MODE_REGULAR_E;
    CPSS_CSCD_PORT_TYPE_ENT modeGet = CPSS_CSCD_PORT_DSA_MODE_REGULAR_E;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with direction [CPSS_DIRECTION_INGRESS_E / CPSS_DIRECTION_EGRESS_E],
                               mode [CPSS_CSCD_PORT_DSA_MODE_REGULAR_E / CPSS_CSCD_PORT_DSA_MODE_EXTEND_E],
                Expected: GT_OK.
            */
            /* iterate with direction = CPSS_DIRECTION_INGRESS_E */
            direction = CPSS_DIRECTION_INGRESS_E;
            mode = CPSS_CSCD_PORT_DSA_MODE_REGULAR_E;

            st = cpssExMxPmVbPortCascadingModeSet(dev, port, direction, mode);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, mode);

            /*
                1.1.2. Call cpssExMxPmVbPortCascadingModeGet with not NULL modePtr and direction from 1.1.
                Expected :GT_OK and the same mode as was set.
            */
            st = cpssExMxPmVbPortCascadingModeGet(dev, port, direction, &modeGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmVbPortCascadingModeGet: %d, %d, %d", dev, port, direction);
            UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet, "got another mode than was set: %d, %d", dev, port);

            /* iterate with direction = CPSS_DIRECTION_EGRESS_E */
            direction = CPSS_DIRECTION_EGRESS_E;
            mode = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;

            st = cpssExMxPmVbPortCascadingModeSet(dev, port, direction, mode);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, mode);

            /*
                1.1.2. Call cpssExMxPmVbPortCascadingModeGet with not NULL modePtr and direction from 1.1.
                Expected :GT_OK and the same mode as was set.
            */
            st = cpssExMxPmVbPortCascadingModeGet(dev, port, direction, &modeGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmVbPortCascadingModeGet: %d, %d, %d", dev, port, direction);
            UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet, "got another mode than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with direction [0x5AAAAAA5] (out of range).
                Expected: GT_BAD_PARAM.
            */
            direction = VB_INVALID_ENUM_CNS;

            st = cpssExMxPmVbPortCascadingModeSet(dev, port, direction, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, direction);

            direction = CPSS_DIRECTION_EGRESS_E;

            /*
                1.1.4. Call with mode [0x5AAAAAA5] (out of range).
                Expected: GT_BAD_PARAM.
            */
            mode = VB_INVALID_ENUM_CNS;

            st = cpssExMxPmVbPortCascadingModeSet(dev, port, direction, mode);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, mode = %d", dev, port, mode);
        }

        direction = CPSS_DIRECTION_INGRESS_E;
        mode = CPSS_CSCD_PORT_DSA_MODE_REGULAR_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmVbPortCascadingModeSet(dev, port, direction, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmVbPortCascadingModeSet(dev, port, direction, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbPortCascadingModeSet(dev, port, direction, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    direction = CPSS_DIRECTION_INGRESS_E;
    mode = CPSS_CSCD_PORT_DSA_MODE_REGULAR_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbPortCascadingModeSet(dev, port, direction, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbPortCascadingModeSet(dev, port, direction, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbPortCascadingModeGet
(
    IN    GT_U8                       devNum,
    IN    GT_U8                       physicalPortNum,
    IN    CPSS_DIRECTION_ENT          direction,
    OUT   CPSS_CSCD_PORT_TYPE_ENT     *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbPortCascadingModeGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with direction [CPSS_DIRECTION_INGRESS_E / CPSS_DIRECTION_EGRESS_E],
                     not NULL modePtr.
    Expected: GT_OK.
    1.1.2. Call with direction [0x5AAAAAA5] (out of range).
    Expected: GT_BAD_PARAM.
    1.1.3. Call with mode [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    CPSS_DIRECTION_ENT      direction = CPSS_DIRECTION_INGRESS_E;
    CPSS_CSCD_PORT_TYPE_ENT mode = CPSS_CSCD_PORT_DSA_MODE_REGULAR_E;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with direction [CPSS_DIRECTION_INGRESS_E / CPSS_DIRECTION_EGRESS_E],
                                 not NULL modePtr.
                Expected: GT_OK.
            */
            /* iterate with direction = CPSS_DIRECTION_INGRESS_E */
            direction = CPSS_DIRECTION_INGRESS_E;

            st = cpssExMxPmVbPortCascadingModeGet(dev, port, direction, &mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, direction);

            /* iterate with direction = CPSS_DIRECTION_EGRESS_E */
            direction = CPSS_DIRECTION_EGRESS_E;

            st = cpssExMxPmVbPortCascadingModeGet(dev, port, direction, &mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, direction);

            /*
                1.1.2. Call with direction [0x5AAAAAA5] (out of range).
                Expected: GT_BAD_PARAM.
            */
            direction = VB_INVALID_ENUM_CNS;

            st = cpssExMxPmVbPortCascadingModeGet(dev, port, direction, &mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, direction);

            direction = CPSS_DIRECTION_EGRESS_E;

            /*
                1.1.3. Call with mode [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmVbPortCascadingModeGet(dev, port, direction, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, mode = NULL", dev, port);
        }

        direction = CPSS_DIRECTION_INGRESS_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmVbPortCascadingModeGet(dev, port, direction, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmVbPortCascadingModeGet(dev, port, direction, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbPortCascadingModeGet(dev, port, direction, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    direction = CPSS_DIRECTION_INGRESS_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbPortCascadingModeGet(dev, port, direction, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbPortCascadingModeGet(dev, port, direction, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbIngressPortDsaTagManipulationEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_U32   portNum,
    IN  GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbIngressPortDsaTagManipulationEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmVbIngressPortDsaTagManipulationEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = VB_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmVbIngressPortDsaTagManipulationEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmVbIngressPortDsaTagManipulationEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmVbIngressPortDsaTagManipulationEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmVbIngressPortDsaTagManipulationEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmVbIngressPortDsaTagManipulationEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmVbIngressPortDsaTagManipulationEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmVbIngressPortDsaTagManipulationEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmVbIngressPortDsaTagManipulationEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            if(port == PRV_CPSS_EXMXPM_LBP_PHY_PORT_NUM_CNS)
            {
                /* skip LBP port */
                continue;
            }
            st = cpssExMxPmVbIngressPortDsaTagManipulationEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmVbIngressPortDsaTagManipulationEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbIngressPortDsaTagManipulationEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbIngressPortDsaTagManipulationEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbIngressPortDsaTagManipulationEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbIngressPortDsaTagManipulationEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   portNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbIngressPortDsaTagManipulationEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = VB_VALID_PHY_PORT_CNS;

    GT_BOOL     enable = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmVbIngressPortDsaTagManipulationEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmVbIngressPortDsaTagManipulationEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            if(port == PRV_CPSS_EXMXPM_LBP_PHY_PORT_NUM_CNS)
            {
                /* skip LBP port */
                continue;
            }

            st = cpssExMxPmVbIngressPortDsaTagManipulationEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmVbIngressPortDsaTagManipulationEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbIngressPortDsaTagManipulationEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbIngressPortDsaTagManipulationEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbIngressPortDsaTagManipulationEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbIngressPortEtherTypeSelectSet
(
    IN    GT_U8                 devNum,
    IN    GT_U8                 physicalPortNum,
    IN    CPSS_ETHER_MODE_ENT   mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbIngressPortEtherTypeSelectSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with mode [CPSS_VLAN_ETHERTYPE0_E / CPSS_VLAN_ETHERTYPE1_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmVbIngressPortEtherTypeSelectGet with not NULL modePtr.
    Expected: GT_OK and the same mode as was set.
    1.1.3. Call with mode [0x5AAAAAA5] (out of range).
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    CPSS_ETHER_MODE_ENT mode = CPSS_VLAN_ETHERTYPE0_E;
    CPSS_ETHER_MODE_ENT modeGet = CPSS_VLAN_ETHERTYPE0_E;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with mode [CPSS_VLAN_ETHERTYPE0_E / CPSS_VLAN_ETHERTYPE1_E].
                Expected: GT_OK.
            */
            /* iterate with mode = CPSS_VLAN_ETHERTYPE0_E */
            mode = CPSS_VLAN_ETHERTYPE0_E;

            st = cpssExMxPmVbIngressPortEtherTypeSelectSet(dev, port, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);

            /*
                1.1.2. Call cpssExMxPmVbIngressPortEtherTypeSelectGet with not NULL modePtr.
                Expected: GT_OK and the same mode as was set.
            */
            st = cpssExMxPmVbIngressPortEtherTypeSelectGet(dev, port, &modeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmVbIngressPortEtherTypeSelectGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet, "got another mode than wa sset: %d, %d", dev, port);

            /* iterate with mode = CPSS_VLAN_ETHERTYPE1_E */
            mode = CPSS_VLAN_ETHERTYPE1_E;

            st = cpssExMxPmVbIngressPortEtherTypeSelectSet(dev, port, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);

            /*
                1.1.2. Call cpssExMxPmVbIngressPortEtherTypeSelectGet with not NULL modePtr.
                Expected: GT_OK and the same mode as was set.
            */
            st = cpssExMxPmVbIngressPortEtherTypeSelectGet(dev, port, &modeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmVbIngressPortEtherTypeSelectGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet, "got another mode than wa sset: %d, %d", dev, port);

            /*
                1.1.3. Call with mode [0x5AAAAAA5] (out of range).
                Expected: GT_BAD_PARAM.
            */
            mode = VB_INVALID_ENUM_CNS;

            st = cpssExMxPmVbIngressPortEtherTypeSelectSet(dev, port, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mode);
        }

        mode = CPSS_VLAN_ETHERTYPE0_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            if(port == PRV_CPSS_EXMXPM_LBP_PHY_PORT_NUM_CNS)
            {
                /* skip LBP port */
                continue;
            }

            st = cpssExMxPmVbIngressPortEtherTypeSelectSet(dev, port, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmVbIngressPortEtherTypeSelectSet(dev, port, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbIngressPortEtherTypeSelectSet(dev, port, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    mode = CPSS_VLAN_ETHERTYPE0_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbIngressPortEtherTypeSelectSet(dev, port, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbIngressPortEtherTypeSelectSet(dev, port, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbIngressPortEtherTypeSelectGet
(
    IN    GT_U8                 devNum,
    IN    GT_U8                 physicalPortNum,
    OUT   CPSS_ETHER_MODE_ENT   *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbIngressPortEtherTypeSelectGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with not NULL modePtr.
    Expected: GT_OK,
    1.1.2. Call with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    CPSS_ETHER_MODE_ENT mode;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not NULL modePtr.
                Expected: GT_OK,
            */
            st = cpssExMxPmVbIngressPortEtherTypeSelectGet(dev, port, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with modePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmVbIngressPortEtherTypeSelectGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, mode = NULL", dev, port);
        }


        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            if(port == PRV_CPSS_EXMXPM_LBP_PHY_PORT_NUM_CNS)
            {
                /* skip LBP port */
                continue;
            }

            st = cpssExMxPmVbIngressPortEtherTypeSelectGet(dev, port, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmVbIngressPortEtherTypeSelectGet(dev, port, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbIngressPortEtherTypeSelectGet(dev, port, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbIngressPortEtherTypeSelectGet(dev, port, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbIngressPortEtherTypeSelectGet(dev, port, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbVirtualPortsMapModeSet
(
    IN  GT_U8                                        devNum,
    IN CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_ENT     vbMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbVirtualPortsMapModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with vbMode [CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_PHYSICAL_E /
                         CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_DSA_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmVbVirtualPortsMapModeGet with non-NULL vbModePtr.
    Expected: GT_OK and the same vbMode.
    1.3. Call with out of range vbMode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_ENT vbMode    = CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_PHYSICAL_E;
    CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_ENT vbModeGet = CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_PHYSICAL_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vbMode [CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_PHYSICAL_E /
                                 CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_DSA_E].
            Expected: GT_OK.
        */

        /* Call with vbMode [CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_PHYSICAL_E] */
        vbMode = CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_PHYSICAL_E;

        st = cpssExMxPmVbVirtualPortsMapModeSet(dev, vbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vbMode);

        /*
            1.2. Call cpssExMxPmVbVirtualPortsMapModeGet
            Expected: GT_OK and the same vbModePtr.
        */
        st = cpssExMxPmVbVirtualPortsMapModeGet(dev, &vbModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmVbVirtualPortsMapModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(vbMode, vbModeGet,
                       "get another vbMode than was set: %d", dev);

        /* Call with vbMode [CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_DSA_E] */
        vbMode = CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_DSA_E;

        st = cpssExMxPmVbVirtualPortsMapModeSet(dev, vbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vbMode);

        /*
            1.2. Call cpssExMxPmVbVirtualPortsMapModeGet
            Expected: GT_OK and the same vbModePtr.
        */
        st = cpssExMxPmVbVirtualPortsMapModeGet(dev, &vbModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmVbVirtualPortsMapModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(vbMode, vbModeGet,
                       "get another vbMode than was set: %d", dev);

        /*
            1.3. Call function with out of range vbMode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        vbMode = VB_INVALID_ENUM_CNS;

        st = cpssExMxPmVbVirtualPortsMapModeSet(dev, vbMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vbMode);
    }

    vbMode = CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_PHYSICAL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbVirtualPortsMapModeSet(dev, vbMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbVirtualPortsMapModeSet(dev, vbMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbVirtualPortsMapModeGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_ENT    *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbVirtualPortsMapModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null vbModePtr.
    Expected: GT_OK.
    1.2. Call with vbModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_ENT vbMode = CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_PHYSICAL_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null vbModePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmVbVirtualPortsMapModeGet(dev, &vbMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null vbModePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmVbVirtualPortsMapModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vbModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbVirtualPortsMapModeGet(dev, &vbMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbVirtualPortsMapModeGet(dev, &vbMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbPortValueModeEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_U32   portNum,
    IN  GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbPortValueModeEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmVbPortValueModeEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = VB_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmVbPortValueModeEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmVbPortValueModeEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmVbPortValueModeEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmVbPortValueModeEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmVbPortValueModeEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmVbPortValueModeEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmVbPortValueModeEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmVbPortValueModeEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            if(port == PRV_CPSS_EXMXPM_LBP_PHY_PORT_NUM_CNS)
            {
                /* skip LBP port */
                continue;
            }

            st = cpssExMxPmVbPortValueModeEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmVbPortValueModeEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbPortValueModeEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbPortValueModeEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbPortValueModeEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbPortValueModeEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   portNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbPortValueModeEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = VB_VALID_PHY_PORT_CNS;

    GT_BOOL     enable = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmVbPortValueModeEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmVbPortValueModeEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            if(port == PRV_CPSS_EXMXPM_LBP_PHY_PORT_NUM_CNS)
            {
                /* skip LBP port */
                continue;
            }

            st = cpssExMxPmVbPortValueModeEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmVbPortValueModeEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbPortValueModeEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbPortValueModeEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbPortValueModeEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet
(
    IN    GT_U8         devNum,
    IN    GT_U8         physicalPortNum,
    IN    GT_U8         dsaTagSrcDev,
    IN    GT_U8         dsaTagSrcPort,
    IN    GT_U8         portNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with dsaTagSrcDev [0 / 7],
                     dsaTagSrcPort [0 / 63],
                     portNum [0 / UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev)-1].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet with not NULL pointers and other params from 1.1.
    Expected: GT_OK and portNum, bmPhysicalPortNumPtr as was set.
    1.1.3. Call with portNum [UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev)] (out of range).
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    GT_U8       dsaTagSrcDev = 0;
    GT_U8       dsaTagSrcPort = 0;
    GT_U8       portNum = 0;
    GT_U8       portNumGet = 0;
    GT_U8       bmPhysicalPortNum = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with dsaTagSrcDev [0 / 7],
                                 dsaTagSrcPort [0 / 63],
                                 portNum [0 / 54].
                Expected: GT_OK.
            */
            /* iterate with dsaTagSrcDev = 0 */
            dsaTagSrcDev = 0;
            dsaTagSrcPort = 0;
            portNum = 0;

            st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet(dev, port, dsaTagSrcDev, dsaTagSrcPort, portNum);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, dsaTagSrcDev, dsaTagSrcPort, portNum);

            /*
                1.1.2. Call cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet with not NULL pointers and other params from 1.1.
                Expected: GT_OK and portNum, bmPhysicalPortNumPtr as was set.
            */
            st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet(dev, port, dsaTagSrcDev, dsaTagSrcPort, &portNumGet, &bmPhysicalPortNum);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet: %d, %d, %d, %d" ,dev, port, dsaTagSrcDev, dsaTagSrcPort);

            UTF_VERIFY_EQUAL2_STRING_MAC(portNum, portNumGet, "got another portNumGet than was set: %d, %d" , dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(port, bmPhysicalPortNum, "got another portNumGet than was set: %d, %d" , dev, port);

            /* iterate with dsaTagSrcDev = 7 */
            dsaTagSrcDev = 7;
            dsaTagSrcPort = 63;
            portNum = (GT_U8)(UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev)-1);

            st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet(dev, port, dsaTagSrcDev, dsaTagSrcPort, portNum);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, dsaTagSrcDev, dsaTagSrcPort, portNum);

            /*
                1.1.2. Call cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet with not NULL pointers and other params from 1.1.
                Expected: GT_OK and portNum, bmPhysicalPortNumPtr as was set.
            */
            st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet(dev, port, dsaTagSrcDev, dsaTagSrcPort, &portNumGet, &bmPhysicalPortNum);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet: %d, %d, %d, %d" ,dev, port, dsaTagSrcDev, dsaTagSrcPort);

            UTF_VERIFY_EQUAL2_STRING_MAC(portNum, portNumGet, "got another portNumGet than was set: %d, %d" , dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(port, bmPhysicalPortNum, "got another portNumGet than was set: %d, %d" , dev, port);

            /*
                1.1.3. Call with portNum [55] (out of range).
                Expected: NOT GT_OK.
            */
            portNum = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

            st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet(dev, port, dsaTagSrcDev, dsaTagSrcPort, portNum);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, portNum = %d", dev, port, portNum);
        }

        dsaTagSrcDev = 0;
        dsaTagSrcPort = 0;
        portNum = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet(dev, port, dsaTagSrcDev, dsaTagSrcPort, portNum);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet(dev, port, dsaTagSrcDev, dsaTagSrcPort, portNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet(dev, port, dsaTagSrcDev, dsaTagSrcPort, portNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    dsaTagSrcDev = 0;
    dsaTagSrcPort = 0;
    portNum = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet(dev, port, dsaTagSrcDev, dsaTagSrcPort, portNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet(dev, port, dsaTagSrcDev, dsaTagSrcPort, portNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet
(
    IN    GT_U8         devNum,
    IN    GT_U8         physicalPortNum,
    IN    GT_U8         dsaTagSrcDev,
    IN    GT_U8         dsaTagSrcPort,
    OUT   GT_U8         *portNumPtr,
    OUT   GT_U8         *bmPhysicalPortNumPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with dsaTagSrcDev [0 / 7],
                     dsaTagSrcPort [0 / 63],
                     not NULL portNumPtr and bmPhysicalPortNumPtr.
    Expected: GT_OK.
    1.1.2. Call with portNumPtr [NULL].
    Expected: GT_BAD_PTR.
    1.1.3. Call with bmPhysicalPortNumPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    GT_U8       dsaTagSrcDev = 0;
    GT_U8       dsaTagSrcPort = 0;
    GT_U8       portNum = 0;
    GT_U8       bmPhysicalPortNum = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with dsaTagSrcDev [0 / 7],
                                 dsaTagSrcPort [0 / 63],
                                 not NULL portNumPtr and bmPhysicalPortNumPtr.
                Expected: GT_OK.
            */
            /* iterate with dsaTagSrcDev = 0 */
            dsaTagSrcDev = 0;
            dsaTagSrcPort = 0;

            st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet(dev, port, dsaTagSrcDev, dsaTagSrcPort, &portNum, &bmPhysicalPortNum);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, dsaTagSrcDev, dsaTagSrcPort);

            /* iterate with dsaTagSrcDev = 7 */
            dsaTagSrcDev = 7;
            dsaTagSrcPort = 63;

            st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet(dev, port, dsaTagSrcDev, dsaTagSrcPort, &portNum, &bmPhysicalPortNum);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, dsaTagSrcDev, dsaTagSrcPort);

            /*
                1.1.2. Call with portNumPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet(dev, port, dsaTagSrcDev, dsaTagSrcPort, NULL, &bmPhysicalPortNum);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, portNum = NULL", dev, port);

            /*
                1.1.3. Call with bmPhysicalPortNumPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet(dev, port, dsaTagSrcDev, dsaTagSrcPort, &portNum, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, bmPhysicalPortNum = NULL", dev, port);
        }

        dsaTagSrcDev = 0;
        dsaTagSrcPort = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet(dev, port, dsaTagSrcDev, dsaTagSrcPort, &portNum, &bmPhysicalPortNum);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet(dev, port, dsaTagSrcDev, dsaTagSrcPort, &portNum, &bmPhysicalPortNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet(dev, port, dsaTagSrcDev, dsaTagSrcPort, &portNum, &bmPhysicalPortNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    dsaTagSrcDev = 0;
    dsaTagSrcPort = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet(dev, port, dsaTagSrcDev, dsaTagSrcPort, &portNum, &bmPhysicalPortNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet(dev, port, dsaTagSrcDev, dsaTagSrcPort, &portNum, &bmPhysicalPortNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet
(
    IN    GT_U8         devNum,
    IN    GT_U8         physicalPortNum,
    IN    GT_U8         portNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with portNum [0 / 54].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet with not NULL portNumPtr.
    Expected: GT_OK and the same portNum as was set.
    1.1.3. Call with portNum [55] (out of range).
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    GT_U8 portNum = 0;
    GT_U8 portNumGet = 0;
    GT_U8 bmPhysicalPortNum = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with portNum [0 / 54].
                Expected: GT_OK.
            */
            /* iterate with portNum = 0 */
            portNum = 0;

            st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, port, portNum);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, portNum);

            /*
                1.1.2. Call cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet with not NULL portNumPtr.
                Expected: GT_OK and the same portNum as was set.
            */
            st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet(dev, port, &portNumGet, &bmPhysicalPortNum);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portNum, portNumGet, "got another portNum than wa sset: %d, %d", dev, port);

            /* iterate with portNum = 54 */
            portNum = 54;

            st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, port, portNum);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, portNum);

            /*
                1.1.2. Call cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet with not NULL portNumPtr.
                Expected: GT_OK and the same portNum as was set.
            */
            st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet(dev, port, &portNumGet, &bmPhysicalPortNum);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portNum, portNumGet, "got another portNum than wa sset: %d, %d", dev, port);

            /*
                1.1.3. Call with portNum [64] (out of range).
                Expected: NOT GT_OK.
            */
            portNum = 64;

            st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, port, portNum);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, portNum);
        }

        portNum = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
            /* 1.2.1. Call function for each non-active port */
        st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, port, 64);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = 28;

        st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, port, portNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, port, portNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    portNum = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, port, portNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, port, portNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet
(
    IN    GT_U8         devNum,
    IN    GT_U8         physicalPortNum,
    OUT   GT_U8         *portNumPtr,
    OUT   GT_U8         *bmPhysicalPortNumPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with not NULL portNumPtr.
    Expected: GT_OK,
    1.1.2. Call with portNumPtr [NULL].
    Expected: GT_BAD_PTR.
    1.1.3. Call with bmPhysicalPortNum [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    GT_U8 portNum = 0;
    GT_U8 bmPhysicalPortNum = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not NULL portNumPtr.
                Expected: GT_OK,
            */
            st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet(dev, port, &portNum, &bmPhysicalPortNum);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with portNumPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet(dev, port, NULL, &bmPhysicalPortNum);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, portNum = NULL", dev, port);

            /*
                1.1.3. Call with bmPhysicalPortNum [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet(dev, port, &portNum, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, bmPhysicalPortNum = NULL", dev, port);
        }


        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            if(port == PRV_CPSS_EXMXPM_LBP_PHY_PORT_NUM_CNS)
            {
                /* skip LBP port */
                continue;
            }

            st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet(dev, port, &portNum, &bmPhysicalPortNum);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = 28;

        st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet(dev, port, &portNum, &bmPhysicalPortNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet(dev, port, &portNum, &bmPhysicalPortNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet(dev, port, &portNum, &bmPhysicalPortNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet(dev, port, &portNum, &bmPhysicalPortNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet
(
    IN  GT_U8           devNum,
    IN  GT_U8           portNum,
    IN  GT_U8           dsaTagTrgDev,
    IN  GT_U8           dsaTagTrgPort
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with dsaTagTrgDev [0 / 127], 
                     dsaTagTrgPort [0 / 63].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet with not NULL pointers.
    Expected: GT_OK and the same dsaTagTrgDev, dsaTagTrgPort as was set.
    1.1.3. Call with dsaTagTrgDev [128] (out of range)
    Expected: NOT GT_OK.
    1.1.4. Call with dsaTagTrgPort [64] (out of range)
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    GT_U8       dsaTagTrgDev = 0;
    GT_U8       dsaTagTrgPort = 0;
    GT_U8       dsaTagTrgDevGet = 0;
    GT_U8       dsaTagTrgPortGet = 0;


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
            /*
                1.1.1. Call with dsaTagTrgDev [0 / 127], 
                                 dsaTagTrgPort [0 / 63].
                Expected: GT_OK.
            */
            /* iterate with dsaTagTrgDev = 0 */
            dsaTagTrgDev = 0;
            dsaTagTrgPort = 0;

            st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet(dev, port, dsaTagTrgDev, dsaTagTrgPort);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, dsaTagTrgDev, dsaTagTrgPort);

            /*
                1.1.2. Call cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet with not NULL pointers.
                Expected: GT_OK and the same dsaTagTrgDev, dsaTagTrgPort as was set.
            */
            st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet(dev, port, &dsaTagTrgDevGet, &dsaTagTrgPortGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet: %d, %d", dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(dsaTagTrgDev, dsaTagTrgDevGet, "got another dsaTagTrgDev than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(dsaTagTrgPort, dsaTagTrgPortGet, "got another dsaTagTrgPort than was set: %d, %d", dev, port);

            /* iterate with dsaTagTrgDev = 127 */
            dsaTagTrgDev = 127;
            dsaTagTrgPort = 63;

            st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet(dev, port, dsaTagTrgDev, dsaTagTrgPort);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, dsaTagTrgDev, dsaTagTrgPort);

            /*
                1.1.2. Call cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet with not NULL pointers.
                Expected: GT_OK and the same dsaTagTrgDev, dsaTagTrgPort as was set.
            */
            st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet(dev, port, &dsaTagTrgDevGet, &dsaTagTrgPortGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet: %d, %d", dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(dsaTagTrgDev, dsaTagTrgDevGet, "got another dsaTagTrgDev than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(dsaTagTrgPort, dsaTagTrgPortGet, "got another dsaTagTrgPort than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with dsaTagTrgDev [128] (out of range)
                Expected: NOT GT_OK.
            */
            dsaTagTrgDev = 128;

            st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet(dev, port, dsaTagTrgDev, dsaTagTrgPort);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, dsaTagTrgDev);

            dsaTagTrgDev = 0;

            /*
                1.1.4. Call with dsaTagTrgPort [64] (out of range)
                Expected: NOT GT_OK.
            */
            dsaTagTrgPort = 64;

            st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet(dev, port, dsaTagTrgDev, dsaTagTrgPort);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, dsaTagTrgPort = %d", dev, port, dsaTagTrgPort);
        }

        dsaTagTrgDev = 0;
        dsaTagTrgPort = 0;


        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet(dev, port, dsaTagTrgDev, dsaTagTrgPort);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = 64;

        st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet(dev, port, dsaTagTrgDev, dsaTagTrgPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet(dev, port, dsaTagTrgDev, dsaTagTrgPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    dsaTagTrgDev = 0;
    dsaTagTrgPort = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet(dev, port, dsaTagTrgDev, dsaTagTrgPort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet(dev, port, dsaTagTrgDev, dsaTagTrgPort);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet
(
    IN  GT_U8           devNum,
    IN  GT_U8           portNum,
    OUT GT_U8           *dsaTagTrgDevPtr,
    OUT GT_U8           *dsaTagTrgPortPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with not NULL pointers.
    Expected: GT_OK.
    1.1.2. Call with dsaTagTrgDevPtr [NULL].
    Expected: GT_BAD_PTR.
    1.1.3. Call with dsaTagTrgPortPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    GT_U8           dsaTagTrgDev = 0;
    GT_U8           dsaTagTrgPort = 0;


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
            /*
                1.1.1. Call with not NULL pointers.
                Expected: GT_OK.
            */
            st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet(dev, port, &dsaTagTrgDev, &dsaTagTrgPort);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with dsaTagTrgDevPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet(dev, port, NULL, &dsaTagTrgPort);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, dsaTagTrgDev = NULL", dev, port);

            /*
                1.1.3. Call with dsaTagTrgPortPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet(dev, port, &dsaTagTrgDev, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, dsaTagTrgPort = NULL", dev, port);

        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet(dev, port, &dsaTagTrgDev, &dsaTagTrgPort);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = 64;

        st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet(dev, port, &dsaTagTrgDev, &dsaTagTrgPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet(dev, port, &dsaTagTrgDev, &dsaTagTrgPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet(dev, port, &dsaTagTrgDev, &dsaTagTrgPort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet(dev, port, &dsaTagTrgDev, &dsaTagTrgPort);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet
(
    IN  GT_U8           devNum,
    IN  GT_U8           portNum,
    IN  GT_U32          tc,
    IN  GT_U8           physicalPortNum,
    IN  GT_U32          physicalTc
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with tc [0 / 7], physicalPortNum [0 / 31], physicalTc [0 / 7].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet with not NULL pointers.
    Expected: GT_OK and the same params as was set.
    1.1.3. Call with tc [8] (out of range).
    Expected: NOT GT_OK.
    1.1.4. Call with physicalPortNum [32] (out of range).
    Expected: NOT GT_OK.
    1.1.5. Call with physicalTc [8] (out of range).
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;


    GT_U32      tc = 0;
    GT_U8       physicalPortNum = 0;
    GT_U32      physicalTc = 0;
    GT_U8       physicalPortNumGet = 0;
    GT_U32      physicalTcGet = 0;


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
            /*
                1.1.1. Call with tc [0 / 7], physicalPortNum [0 / 31], physicalTc [0 / 7].
                Expected: GT_OK.
            */
            /* iterate with tc = 0 */
            tc = 0;
            physicalPortNum = 0;
            physicalTc = 0;

            st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, physicalPortNum, physicalTc);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, tc, physicalPortNum, physicalTc);

            /*
                1.1.2. Call cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet with not NULL pointers.
                Expected: GT_OK and the same params as was set.
            */
            st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet(dev, port, tc, &physicalPortNumGet, &physicalTcGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet: %d, %d, %d", dev, port, tc);

            UTF_VERIFY_EQUAL2_STRING_MAC(physicalPortNum, physicalPortNumGet, "got another physicalPortNum than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(physicalTc, physicalTcGet, "got another physicalTc than was set: %d, %d", dev, port);

            /* iterate with tc = 7 */
            tc = 7;
            physicalPortNum = 31;
            physicalTc = 7;

            st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, physicalPortNum, physicalTc);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, tc, physicalPortNum, physicalTc);

            /*
                1.1.2. Call cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet with not NULL pointers.
                Expected: GT_OK and the same params as was set.
            */
            st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet(dev, port, tc, &physicalPortNumGet, &physicalTcGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet: %d, %d, %d", dev, port, tc);

            UTF_VERIFY_EQUAL2_STRING_MAC(physicalPortNum, physicalPortNumGet, "got another physicalPortNum than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(physicalTc, physicalTcGet, "got another physicalTc than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with tc [8] (out of range).
                Expected: NOT GT_OK.
            */
            tc = 8;

            st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, physicalPortNum, physicalTc);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            tc = 0;

            /*
                1.1.4. Call with physicalPortNum [32] (out of range).
                Expected: NOT GT_OK.
            */
            physicalPortNum = 32;

            st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, physicalPortNum, physicalTc);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d physicalPortNum = %d", dev, port, physicalPortNum);

            physicalPortNum = 0;

            /*
                1.1.5. Call with physicalTc [8] (out of range).
                Expected: NOT GT_OK.
            */
            physicalTc = 8;

            st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, physicalPortNum, physicalTc);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d physicalTc = %d", dev, port, physicalTc);
        }

        tc = 0;
        physicalPortNum = 0;
        physicalTc = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, physicalPortNum, physicalTc);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = 64;

        st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, physicalPortNum, physicalTc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, physicalPortNum, physicalTc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    tc = 0;
    physicalPortNum = 0;
    physicalTc = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, physicalPortNum, physicalTc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(dev, port, tc, physicalPortNum, physicalTc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet
(
    IN  GT_U8           devNum,
    IN  GT_U8           portNum,
    IN  GT_U32          tc,
    OUT GT_U8           *physicalPortNumPtr,
    OUT GT_U32          *physicalTcPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with tc [0 / 7], not NULL physicalPortNumPtr, physicalTcPtr.
    Expected: GT_OK.
    1.1.2. Call with tc [8] (out of range).
    Expected: NOT GT_OK.
    1.1.3. Call with physicalPortNum [NULL].
    Expected: GT_BAD_PTR.
    1.1.4. Call with physicalTc [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    GT_U32      tc = 0;
    GT_U8       physicalPortNum = 0;
    GT_U32      physicalTc = 0;


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
            /*
                1.1.1. Call with tc [0 / 7], not NULL physicalPortNumPtr, physicalTcPtr.
                Expected: GT_OK.
            */
            tc = 0;

            st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet(dev, port, tc, &physicalPortNum, &physicalTc);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            tc = 7;

            st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet(dev, port, tc, &physicalPortNum, &physicalTc);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            /*
                1.1.2. Call with tc [8] (out of range).
                Expected: NOT GT_OK.
            */
            tc = 8;

            st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet(dev, port, tc, &physicalPortNum, &physicalTc);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            tc = 0;

            /*
                1.1.3. Call with physicalPortNum [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet(dev, port, tc, NULL, &physicalTc);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, physicalPortNum = NULL", dev, port);

            /*
                1.1.4. Call with physicalTc [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet(dev, port, tc, &physicalPortNum, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, physicalTc = NULL", dev, port);
        }

        tc = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet(dev, port, tc, &physicalPortNum, &physicalTc);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = 64;

        st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet(dev, port, tc, &physicalPortNum, &physicalTc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet(dev, port, tc, &physicalPortNum, &physicalTc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    tc = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet(dev, port, tc, &physicalPortNum, &physicalTc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet(dev, port, tc, &physicalPortNum, &physicalTc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbEgressVirtualPortAggregationModeSet
(
    IN    GT_U8                                         devNum,
    IN    CPSS_EXMXPM_VB_QUEUE_AGGREGATION_MODE_ENT     mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbEgressVirtualPortAggregationModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mode [CPSS_EXMXPM_VB_QUEUE_NON_AGGREGATED_E /
                         CPSS_EXMXPM_VB_QUEUE_AGGREGATED_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmVbEgressVirtualPortAggregateModeGet with non-NULL modePtr.
    Expected: GT_OK and the same mode.
    1.3. Call with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_VB_QUEUE_AGGREGATION_MODE_ENT mode    = CPSS_EXMXPM_VB_QUEUE_NON_AGGREGATED_E;
    CPSS_EXMXPM_VB_QUEUE_AGGREGATION_MODE_ENT modeGet = CPSS_EXMXPM_VB_QUEUE_NON_AGGREGATED_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [CPSS_EXMXPM_VB_QUEUE_NON_AGGREGATED_E /
                                 CPSS_EXMXPM_VB_QUEUE_AGGREGATED_E].
            Expected: GT_OK.
        */

        /* Call with mode [CPSS_EXMXPM_VB_QUEUE_NON_AGGREGATED_E] */
        mode = CPSS_EXMXPM_VB_QUEUE_NON_AGGREGATED_E;

        st = cpssExMxPmVbEgressVirtualPortAggregationModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmVbEgressVirtualPortAggregateModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmVbEgressVirtualPortAggregateModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmVbEgressVirtualPortAggregateModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_VB_QUEUE_AGGREGATED_E] */
        mode = CPSS_EXMXPM_VB_QUEUE_AGGREGATED_E;

        st = cpssExMxPmVbEgressVirtualPortAggregationModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmVbEgressVirtualPortAggregateModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmVbEgressVirtualPortAggregateModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmVbEgressVirtualPortAggregateModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /*
            1.3. Call function with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = VB_INVALID_ENUM_CNS;

        st = cpssExMxPmVbEgressVirtualPortAggregationModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_VB_QUEUE_NON_AGGREGATED_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbEgressVirtualPortAggregationModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbEgressVirtualPortAggregationModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbEgressVirtualPortAggregateModeGet
(
    IN    GT_U8                                         devNum,
    OUT   CPSS_EXMXPM_VB_QUEUE_AGGREGATION_MODE_ENT     *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbEgressVirtualPortAggregateModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null modePtr.
    Expected: GT_OK.
    1.2. Call with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_VB_QUEUE_AGGREGATION_MODE_ENT mode = CPSS_EXMXPM_VB_QUEUE_NON_AGGREGATED_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null modePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmVbEgressVirtualPortAggregateModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmVbEgressVirtualPortAggregateModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbEgressVirtualPortAggregateModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbEgressVirtualPortAggregateModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbServicePortVirtualPortNumSet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbServicePortVirtualPortNumSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1. Call cpssExMxPmVbServicePortVirtualPortNumSet.
    Expected: GT_OK.
    1.2. Call cpssExMxPmVbServicePortVirtualPortNumGet with not NULL portPtr.
    Expected: GT_OK and the same port as was set.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;
    GT_U8       portGet = 0;

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
            /*
                1.1. Call cpssExMxPmVbServicePortVirtualPortNumSet.
                Expected: GT_OK.
            */
            st = cpssExMxPmVbServicePortVirtualPortNumSet(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.2. Call cpssExMxPmVbServicePortVirtualPortNumGet with not NULL portPtr.
                Expected: GT_OK and the same port as was set.
            */
            st = cpssExMxPmVbServicePortVirtualPortNumGet(dev, &portGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmVbServicePortVirtualPortNumGet: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(port, portGet, "got another port than was set: %d", dev);
        }


        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmVbServicePortVirtualPortNumSet(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_OUT_OF_RANGE */
        /* for out of bound value for port number.                         */
        port = 64;

        st = cpssExMxPmVbServicePortVirtualPortNumSet(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbServicePortVirtualPortNumSet(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbServicePortVirtualPortNumSet(dev, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbServicePortVirtualPortNumSet(dev, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbServicePortVirtualPortNumGet
(
    IN    GT_U8     devNum,
    OUT   GT_U8     *portNumPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbServicePortVirtualPortNumGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null portNumPtr.
    Expected: GT_OK.
    1.2. Call with portNumPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       portNum = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL portNumPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmVbServicePortVirtualPortNumGet(dev, &portNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with portNumPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmVbServicePortVirtualPortNumGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d , portNumPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbServicePortVirtualPortNumGet(dev, &portNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbServicePortVirtualPortNumGet(dev, &portNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbLoopbackPortVirtualPortNumSet
(
    IN    GT_U8     devNum,
    IN    GT_U8     portNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbLoopbackPortVirtualPortNumSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1. Call cpssExMxPmVbLoopbackPortVirtualPortNumSet.
    Expected: GT_OK.
    1.2. Call cpssExMxPmVbLoopbackPortVirtualPortNumGet with not NULL portPtr.
    Expected: GT_OK and the same port as was set.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;
    GT_U8       portGet = 0;

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
            /*
                1.1. Call cpssExMxPmVbLoopbackPortVirtualPortNumSet.
                Expected: GT_OK.
            */
            st = cpssExMxPmVbLoopbackPortVirtualPortNumSet(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.2. Call cpssExMxPmVbLoopbackPortVirtualPortNumGet with not NULL portPtr.
                Expected: GT_OK and the same port as was set.
            */
            st = cpssExMxPmVbLoopbackPortVirtualPortNumGet(dev, &portGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmVbLoopbackPortVirtualPortNumGet: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(port, portGet, "got another port than was set: %d", dev);
        }


        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmVbLoopbackPortVirtualPortNumSet(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_OUT_OF_RANGE */
        /* for out of bound value for port number.                         */
        port = 64;

        st = cpssExMxPmVbLoopbackPortVirtualPortNumSet(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbLoopbackPortVirtualPortNumSet(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbLoopbackPortVirtualPortNumSet(dev, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbLoopbackPortVirtualPortNumSet(dev, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbLoopbackPortVirtualPortNumGet
(
    IN    GT_U8     devNum,
    OUT   GT_U8     *portNumPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbLoopbackPortVirtualPortNumGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null portNumPtr.
    Expected: GT_OK.
    1.2. Call with portNumPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       portNum = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL portNumPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmVbLoopbackPortVirtualPortNumGet(dev, &portNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with portNumPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmVbLoopbackPortVirtualPortNumGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d , portNumPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbLoopbackPortVirtualPortNumGet(dev, &portNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbLoopbackPortVirtualPortNumGet(dev, &portNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet
(
    IN    GT_U8                 devNum,
    IN    GT_U8                 physicalPortNum,
    IN    CPSS_PORTS_BMP_STC    *portsBitmapPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with portsBitmap { ports [0, 0 / 0xFFFFFFFF, 0xFFFFFFFF]}.
    Expected: GT_OK.
    1.1.2. Call with not NULL portsBitmapPtr.
    Expected: GT_OK and the same portsBitmapPtr as was set.
    1.1.3. Call with portsBitmapPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = 0;

    CPSS_PORTS_BMP_STC    portsBitmap;
    CPSS_PORTS_BMP_STC    portsBitmapGet;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with portsBitmap { ports [0, 0 / 0xFFFFFFFF, 0xFFFFFFFF]}.
                Expected: GT_OK.
            */
            /* iterate with 0 */
            portsBitmap.ports[0] = 0;
            portsBitmap.ports[1] = 0;

            st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet(dev, port, &portsBitmap);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with not NULL portsBitmapPtr.
                Expected: GT_OK and the same portsBitmapPtr as was set.
            */
            st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet(dev, port, &portsBitmapGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet: %d, %d", dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(portsBitmap.ports[0], portsBitmapGet.ports[0], 
                                         "got another portsBitmap.ports[0] than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portsBitmap.ports[1], portsBitmapGet.ports[1], 
                                         "got another portsBitmap.ports[1] than was set: %d, %d", dev, port);

            /* iterate with 0xFFFFFFFF */
            portsBitmap.ports[0] = 0xFFFFFFFF;
            portsBitmap.ports[1] = 0xFFFFFFFF;

            st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet(dev, port, &portsBitmap);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with not NULL portsBitmapPtr.
                Expected: GT_OK and the same portsBitmapPtr as was set.
            */
            st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet(dev, port, &portsBitmapGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet: %d, %d", dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(portsBitmap.ports[0], portsBitmapGet.ports[0], 
                                         "got another portsBitmap.ports[0] than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portsBitmap.ports[1], portsBitmapGet.ports[1], 
                                         "got another portsBitmap.ports[1] than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with portsBitmapPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet(dev, port, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d , portsBitmapGet = NULL", dev);
        }

        portsBitmap.ports[0] = 0;
        portsBitmap.ports[1] = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if(port == PRV_CPSS_EXMXPM_LBP_PHY_PORT_NUM_CNS)
            {
                /* skip LBP port */
                continue;
            }

            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet(dev, port, &portsBitmap);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet(dev, port, &portsBitmap);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet(dev, port, &portsBitmap);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    portsBitmap.ports[0] = 0;
    portsBitmap.ports[1] = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet(dev, port, &portsBitmap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet(dev, port, &portsBitmap);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet
(
    IN    GT_U8                 devNum,
    IN    GT_U8                 physicalPortNum,
    OUT   CPSS_PORTS_BMP_STC    *portsBitmapPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null portsBitmapPtr.
    Expected: GT_OK.
    1.1.2. Call with portsBitmapPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = VB_VALID_PHY_PORT_CNS;

    CPSS_PORTS_BMP_STC  portsBitmap;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL portsBitmapPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet(dev, port, &portsBitmap);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with portsBitmapPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, portsBitmapPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            if(port == PRV_CPSS_EXMXPM_LBP_PHY_PORT_NUM_CNS)
            {
                /* skip LBP port */
                continue;
            }

            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet(dev, port, &portsBitmap);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet(dev, port, &portsBitmap);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet(dev, port, &portsBitmap);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = VB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet(dev, port, &portsBitmap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet(dev, port, &portsBitmap);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmVbConfigSet
(
    IN GT_U8                                devNum,
    IN GT_U32                               cscdPortsArraySize,
    IN CPSS_EXMXPM_VB_CSCD_PORT_INFO_STC    cscdPortsArray[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmVbConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cscdPortsArraySize [1],
                    cscdPortsArray [0] { cscdIngressPhysicalPortNum [0 / 63],
                                            cscdIngressPortDsaType [CPSS_EXMXPM_NET_DSA_TYPE_REGULAR_E / CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                            virtPortsInfoSize [0 / 1],
                                            virtPortsInfo { portNum [1 / 2],
                                                            dsaTagSrcDev [1 / 2],
                                                            dsaTagSrcPort [1 / 2],
                                                            egressPhysicalPortsNum[0,...,0],
                                                            egressPhysicalTcMap[0,...,0] } }
    Expected: GT_OK.
    1.2. Call with cscdPortsArray[0]->cscdIngressPhysicalPortNum [64] (out of range)
    Expected: NOT GT_OK.
    1.3. Call with cscdPortsArray[0]->cscdIngressPortDsaType [0x5AAAAAA5] (out of range)
    Expected: GT_BAD_PARAM.
    1.4. Call with cscdPortsArray[0]->virtPortsInfo[0]->portNum [55] (out of range)
    Expected: NOT GT_OK.
    1.5. Call with cscdPortsArray[0]->virtPortsInfo[0]->egressPhysicalPortsNum[0] [64] (out of range)
    Expected: NOT GT_OK.
    1.6. Call with cscdPortsArray[0]->virtPortsInfo[0]->egressPhysicalTcMap[0] [8] (out of range)
    Expected: NOT GT_OK.
    1.7. Call with cscdPortsArray [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                            cscdPortsArraySize = 0;
    CPSS_EXMXPM_VB_CSCD_PORT_INFO_STC cscdPortsArray[5];
    CPSS_EXMXPM_VB_VIRT_PORT_INFO_STC virtPortsInfo[5];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cscdPortsArraySize [1],
                            cscdPortsArray [0] { cscdIngressPhysicalPortNum [0 / 20],
                                                    cscdIngressPortDsaType [CPSS_EXMXPM_NET_DSA_TYPE_REGULAR_E / CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                                    virtPortsInfoSize [1 / 2],
                                                    virtPortsInfo { portNum [1 / 2],
                                                                    dsaTagSrcDev [1 / 2],
                                                                    dsaTagSrcPort [1 / 2],
                                                                    egressPhysicalPortsNum[0,...,0],
                                                                    egressPhysicalTcMap[0,...,0] } }
            Expected: GT_OK.
        */
        /* iterate with cscdPortsArray[0].virtPortsInfoSize = 1 */
        cscdPortsArraySize = 1;
        cscdPortsArray[0].cscdIngressPhysicalPortNum = 0;
        cscdPortsArray[0].cscdIngressPortDsaType = CPSS_EXMXPM_NET_DSA_TYPE_REGULAR_E;
        cscdPortsArray[0].virtPortsInfoSize = 1;
        cscdPortsArray[0].virtPortsInfoPtr = virtPortsInfo;

        virtPortsInfo[0].portNum = 1;
        virtPortsInfo[0].dsaTagSrcDev = 1;
        virtPortsInfo[0].dsaTagSrcPort = 1;

        virtPortsInfo[0].egressPhysicalPortsNum[0] = 0;
        virtPortsInfo[0].egressPhysicalPortsNum[1] = 0;
        virtPortsInfo[0].egressPhysicalPortsNum[2] = 0;
        virtPortsInfo[0].egressPhysicalPortsNum[3] = 0;
        virtPortsInfo[0].egressPhysicalPortsNum[4] = 0;
        virtPortsInfo[0].egressPhysicalPortsNum[5] = 0;
        virtPortsInfo[0].egressPhysicalPortsNum[6] = 0;
        virtPortsInfo[0].egressPhysicalPortsNum[7] = 0;

        virtPortsInfo[0].egressPhysicalTcMap[0] = 0;
        virtPortsInfo[0].egressPhysicalTcMap[1] = 0;
        virtPortsInfo[0].egressPhysicalTcMap[2] = 0;
        virtPortsInfo[0].egressPhysicalTcMap[3] = 0;
        virtPortsInfo[0].egressPhysicalTcMap[4] = 0;
        virtPortsInfo[0].egressPhysicalTcMap[5] = 0;
        virtPortsInfo[0].egressPhysicalTcMap[6] = 0;
        virtPortsInfo[0].egressPhysicalTcMap[7] = 0;

        st = cpssExMxPmVbConfigSet(dev, cscdPortsArraySize, cscdPortsArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cscdPortsArraySize);

        /* iterate with cscdPortsArray[0].virtPortsInfoSize = 1 */
        cscdPortsArraySize = 1;
        cscdPortsArray[0].cscdIngressPhysicalPortNum = 20;
        cscdPortsArray[0].cscdIngressPortDsaType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;
        cscdPortsArray[0].virtPortsInfoSize = 2;
        cscdPortsArray[0].virtPortsInfoPtr = virtPortsInfo;

        virtPortsInfo[0].portNum = 2;
        virtPortsInfo[0].dsaTagSrcDev = 2;
        virtPortsInfo[0].dsaTagSrcPort = 2;

        virtPortsInfo[0].egressPhysicalPortsNum[0] = 0;
        virtPortsInfo[0].egressPhysicalPortsNum[1] = 0;
        virtPortsInfo[0].egressPhysicalPortsNum[2] = 0;
        virtPortsInfo[0].egressPhysicalPortsNum[3] = 0;
        virtPortsInfo[0].egressPhysicalPortsNum[4] = 0;
        virtPortsInfo[0].egressPhysicalPortsNum[5] = 0;
        virtPortsInfo[0].egressPhysicalPortsNum[6] = 0;
        virtPortsInfo[0].egressPhysicalPortsNum[7] = 0;

        virtPortsInfo[0].egressPhysicalTcMap[0] = 0;
        virtPortsInfo[0].egressPhysicalTcMap[1] = 0;
        virtPortsInfo[0].egressPhysicalTcMap[2] = 0;
        virtPortsInfo[0].egressPhysicalTcMap[3] = 0;
        virtPortsInfo[0].egressPhysicalTcMap[4] = 0;
        virtPortsInfo[0].egressPhysicalTcMap[5] = 0;
        virtPortsInfo[0].egressPhysicalTcMap[6] = 0;
        virtPortsInfo[0].egressPhysicalTcMap[7] = 0;

        virtPortsInfo[1].portNum = 2;
        virtPortsInfo[1].dsaTagSrcDev = 2;
        virtPortsInfo[1].dsaTagSrcPort = 2;

        virtPortsInfo[1].egressPhysicalPortsNum[0] = 0;
        virtPortsInfo[1].egressPhysicalPortsNum[1] = 0;
        virtPortsInfo[1].egressPhysicalPortsNum[2] = 0;
        virtPortsInfo[1].egressPhysicalPortsNum[3] = 0;
        virtPortsInfo[1].egressPhysicalPortsNum[4] = 0;
        virtPortsInfo[1].egressPhysicalPortsNum[5] = 0;
        virtPortsInfo[1].egressPhysicalPortsNum[6] = 0;
        virtPortsInfo[1].egressPhysicalPortsNum[7] = 0;

        virtPortsInfo[1].egressPhysicalTcMap[0] = 0;
        virtPortsInfo[1].egressPhysicalTcMap[1] = 0;
        virtPortsInfo[1].egressPhysicalTcMap[2] = 0;
        virtPortsInfo[1].egressPhysicalTcMap[3] = 0;
        virtPortsInfo[1].egressPhysicalTcMap[4] = 0;
        virtPortsInfo[1].egressPhysicalTcMap[5] = 0;
        virtPortsInfo[1].egressPhysicalTcMap[6] = 0;
        virtPortsInfo[1].egressPhysicalTcMap[7] = 0;

        st = cpssExMxPmVbConfigSet(dev, cscdPortsArraySize, cscdPortsArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cscdPortsArraySize);

        /*
            1.2. Call with cscdPortsArray[0]->cscdIngressPhysicalPortNum [64] (out of range)
            Expected: NOT GT_OK.
        */
        cscdPortsArray[0].cscdIngressPhysicalPortNum = 64;

        st = cpssExMxPmVbConfigSet(dev, cscdPortsArraySize, cscdPortsArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, cscdPortsArray[0].cscdIngressPhysicalPortNum = %d", 
                                         dev, cscdPortsArraySize, cscdPortsArray[0].cscdIngressPhysicalPortNum);

        cscdPortsArray[0].cscdIngressPhysicalPortNum = 0;

        /*
            1.3. Call with cscdPortsArray[0]->cscdIngressPortDsaType [0x5AAAAAA5] (out of range)
            Expected: GT_BAD_PARAM.
        */
        cscdPortsArray[0].cscdIngressPortDsaType = VB_INVALID_ENUM_CNS;

        st = cpssExMxPmVbConfigSet(dev, cscdPortsArraySize, cscdPortsArray);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, cscdPortsArray[0].cscdIngressPortDsaType = %d", 
                                     dev, cscdPortsArraySize, cscdPortsArray[0].cscdIngressPortDsaType);

        cscdPortsArray[0].cscdIngressPortDsaType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        /*
            1.4. Call with cscdPortsArray[0]->virtPortsInfo[0]->portNum [55] (out of range)
            Expected: NOT GT_OK.
        */
        cscdPortsArray[0].virtPortsInfoPtr[0].portNum = 55;

        st = cpssExMxPmVbConfigSet(dev, cscdPortsArraySize, cscdPortsArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, cscdPortsArray[0].virtPortsInfoPtr[0].portNum = %d", 
                                         dev, cscdPortsArraySize, cscdPortsArray[0].virtPortsInfoPtr[0].portNum);

        cscdPortsArray[0].virtPortsInfoPtr[0].portNum = 0;

        /*
            1.5. Call with cscdPortsArray[0]->virtPortsInfoPtr[0].egressPhysicalPortsNum[0] [64] (out of range)
            Expected: NOT GT_OK.
        */
        cscdPortsArray[0].virtPortsInfoPtr[0].egressPhysicalPortsNum[0] = 64;

        st = cpssExMxPmVbConfigSet(dev, cscdPortsArraySize, cscdPortsArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, cscdPortsArray[0].virtPortsInfoPtr[0].egressPhysicalPortsNum[0] = %d", 
                                         dev, cscdPortsArraySize, cscdPortsArray[0].virtPortsInfoPtr[0].egressPhysicalPortsNum[0]);

        cscdPortsArray[0].virtPortsInfoPtr[0].egressPhysicalPortsNum[0] = 0;

        /*
            1.6. Call with cscdPortsArray[0]->virtPortsInfoPtr[0].egressPhysicalTcMap[0] [8] (out of range)
            Expected: NOT GT_OK.
        */
        cscdPortsArray[0].virtPortsInfoPtr[0].egressPhysicalTcMap[0] = 8;

        st = cpssExMxPmVbConfigSet(dev, cscdPortsArraySize, cscdPortsArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, cscdPortsArray[0].virtPortsInfoPtr[0].egressPhysicalTcMap[0] = %d", 
                                         dev, cscdPortsArraySize, cscdPortsArray[0].virtPortsInfoPtr[0].egressPhysicalTcMap[0]);

        cscdPortsArray[0].virtPortsInfoPtr[0].egressPhysicalTcMap[0] = 0;

        /*
            1.7. Call with cscdPortsArray [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmVbConfigSet(dev, cscdPortsArraySize, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, cscdPortsArray = NULL", dev, cscdPortsArraySize);
    }

    cscdPortsArraySize = 1;
    cscdPortsArray[0].cscdIngressPhysicalPortNum = 0;
    cscdPortsArray[0].cscdIngressPortDsaType = CPSS_EXMXPM_NET_DSA_TYPE_REGULAR_E;
    cscdPortsArray[0].virtPortsInfoSize = 1;
    cscdPortsArray[0].virtPortsInfoPtr = virtPortsInfo;

    virtPortsInfo[0].portNum = 1;
    virtPortsInfo[0].dsaTagSrcDev = 1;
    virtPortsInfo[0].dsaTagSrcPort = 1;

    virtPortsInfo[0].egressPhysicalPortsNum[0] = 0;
    virtPortsInfo[0].egressPhysicalPortsNum[1] = 0;
    virtPortsInfo[0].egressPhysicalPortsNum[2] = 0;
    virtPortsInfo[0].egressPhysicalPortsNum[3] = 0;
    virtPortsInfo[0].egressPhysicalPortsNum[4] = 0;
    virtPortsInfo[0].egressPhysicalPortsNum[5] = 0;
    virtPortsInfo[0].egressPhysicalPortsNum[6] = 0;
    virtPortsInfo[0].egressPhysicalPortsNum[7] = 0;

    virtPortsInfo[0].egressPhysicalTcMap[0] = 0;
    virtPortsInfo[0].egressPhysicalTcMap[1] = 0;
    virtPortsInfo[0].egressPhysicalTcMap[2] = 0;
    virtPortsInfo[0].egressPhysicalTcMap[3] = 0;
    virtPortsInfo[0].egressPhysicalTcMap[4] = 0;
    virtPortsInfo[0].egressPhysicalTcMap[5] = 0;
    virtPortsInfo[0].egressPhysicalTcMap[6] = 0;
    virtPortsInfo[0].egressPhysicalTcMap[7] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmVbConfigSet(dev, cscdPortsArraySize, cscdPortsArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmVbConfigSet(dev, cscdPortsArraySize, cscdPortsArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmVb suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmVb)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbPortCascadingModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbPortCascadingModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbIngressPortDsaTagManipulationEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbIngressPortDsaTagManipulationEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbIngressPortEtherTypeSelectSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbIngressPortEtherTypeSelectGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbVirtualPortsMapModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbVirtualPortsMapModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbPortValueModeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbPortValueModeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbEgressVirtualPortAggregationModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbEgressVirtualPortAggregateModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbServicePortVirtualPortNumSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbServicePortVirtualPortNumGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbLoopbackPortVirtualPortNumSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbLoopbackPortVirtualPortNumGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmVbConfigSet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmVb)

