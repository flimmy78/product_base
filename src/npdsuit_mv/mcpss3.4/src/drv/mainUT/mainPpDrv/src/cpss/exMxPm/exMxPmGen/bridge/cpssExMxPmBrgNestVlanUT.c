/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmBrgNestVlanUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmBrgNestVlan, that provides
*       CPSS ExMxPm Nested VLANs facility implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgNestVlan.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define BRG_NEST_VLAN_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define BRG_NEST_VLAN_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgNestVlanAccessPortEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgNestVlanAccessPortEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgNestVlanAccessPortEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_NEST_VLAN_VALID_VIRT_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmBrgNestVlanAccessPortEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgNestVlanAccessPortEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgNestVlanAccessPortEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgNestVlanAccessPortEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmBrgNestVlanAccessPortEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgNestVlanAccessPortEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgNestVlanAccessPortEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgNestVlanAccessPortEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgNestVlanAccessPortEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgNestVlanAccessPortEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgNestVlanAccessPortEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_NEST_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgNestVlanAccessPortEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgNestVlanAccessPortEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgNestVlanAccessPortEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgNestVlanAccessPortEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_NEST_VLAN_VALID_VIRT_PORT_CNS;

    GT_BOOL     enable = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgNestVlanAccessPortEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgNestVlanAccessPortEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgNestVlanAccessPortEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgNestVlanAccessPortEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgNestVlanAccessPortEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_NEST_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgNestVlanAccessPortEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgNestVlanAccessPortEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    IN  CPSS_ETHER_MODE_ENT     ingressVlanSel
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with ingressVlanSel [CPSS_VLAN_ETHERTYPE0_E /
                                     CPSS_VLAN_ETHERTYPE1_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet with non-NULL ingressVlanSel.
    Expected: GT_OK and the same ingressVlanSel.
    1.1.3. Call with out of range ingressVlanSel [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_NEST_VLAN_VALID_VIRT_PORT_CNS;

    CPSS_ETHER_MODE_ENT     ingressVlanSel    = CPSS_VLAN_ETHERTYPE0_E;
    CPSS_ETHER_MODE_ENT     ingressVlanSelGet = CPSS_VLAN_ETHERTYPE0_E;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with ingressVlanSel [CPSS_VLAN_ETHERTYPE0_E /
                                                 CPSS_VLAN_ETHERTYPE1_E].
                Expected: GT_OK.
            */

            /* Call with ingressVlanSel [CPSS_VLAN_ETHERTYPE0_E] */
            ingressVlanSel = CPSS_VLAN_ETHERTYPE0_E;

            st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet(dev, port, ingressVlanSel);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ingressVlanSel);

            /*
                1.1.2. Call cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet with non-NULL ingressVlanSel.
                Expected: GT_OK and the same ingressVlanSel.
            */
            st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet(dev, port, &ingressVlanSelGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(ingressVlanSel, ingressVlanSelGet,
                       "get another ingressVlanSel than was set: %d, %d", dev, port);

            /* Call with ingressVlanSel [CPSS_VLAN_ETHERTYPE1_E] */
            ingressVlanSel = CPSS_VLAN_ETHERTYPE1_E;

            st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet(dev, port, ingressVlanSel);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ingressVlanSel);

            /*
                1.1.2. Call cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet with non-NULL ingressVlanSel.
                Expected: GT_OK and the same ingressVlanSel.
            */
            st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet(dev, port, &ingressVlanSelGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(ingressVlanSel, ingressVlanSelGet,
                       "get another ingressVlanSel than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range ingressVlanSel [0x5AAAAAA5].
                Expected: GT_BAD_PARAM.
            */
            ingressVlanSel = BRG_NEST_VLAN_INVALID_ENUM_CNS;

            st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet(dev, port, ingressVlanSel);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, ingressVlanSel);
        }

        ingressVlanSel = CPSS_VLAN_ETHERTYPE0_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet(dev, port, ingressVlanSel);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet(dev, port, ingressVlanSel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet(dev, port, ingressVlanSel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    ingressVlanSel = CPSS_VLAN_ETHERTYPE0_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_NEST_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet(dev, port, ingressVlanSel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet(dev, port, ingressVlanSel);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    OUT CPSS_ETHER_MODE_ENT     *ingressVlanSelPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-NULL ingressVlanSelPtr.
    Expected: GT_OK.
    1.1.2. Call with ingressVlanSelPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_NEST_VLAN_VALID_VIRT_PORT_CNS;

    CPSS_ETHER_MODE_ENT     ingressVlanSel = CPSS_VLAN_ETHERTYPE0_E;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL ingressVlanSelPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet(dev, port, &ingressVlanSel);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with ingressVlanSelPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ingressVlanSelPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet(dev, port, &ingressVlanSel);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet(dev, port, &ingressVlanSel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet(dev, port, &ingressVlanSel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_NEST_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet(dev, port, &ingressVlanSel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet(dev, port, &ingressVlanSel);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    IN  CPSS_ETHER_MODE_ENT     egressVlanSel
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with egressVlanSel [CPSS_VLAN_ETHERTYPE0_E /
                                    CPSS_VLAN_ETHERTYPE1_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet with non-NULL egressVlanSelPtr.
    Expected: GT_OK and the same egressVlanSel.
    1.1.3. Call with out of range egressVlanSel [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_NEST_VLAN_VALID_VIRT_PORT_CNS;

    CPSS_ETHER_MODE_ENT     egressVlanSel    = CPSS_VLAN_ETHERTYPE0_E;
    CPSS_ETHER_MODE_ENT     egressVlanSelGet = CPSS_VLAN_ETHERTYPE0_E;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with egressVlanSel [CPSS_VLAN_ETHERTYPE0_E /
                                                CPSS_VLAN_ETHERTYPE1_E].
                Expected: GT_OK.
            */

            /* Call with egressVlanSel [CPSS_VLAN_ETHERTYPE0_E] */
            egressVlanSel = CPSS_VLAN_ETHERTYPE0_E;

            st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet(dev, port, egressVlanSel);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, egressVlanSel);

            /*
                1.1.2. Call cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet with non-NULL egressVlanSel.
                Expected: GT_OK and the same egressVlanSel.
            */
            st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet(dev, port, &egressVlanSelGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(egressVlanSel, egressVlanSelGet,
                       "get another egressVlanSel than was set: %d, %d", dev, port);

            /* Call with egressVlanSel [CPSS_VLAN_ETHERTYPE1_E] */
            egressVlanSel = CPSS_VLAN_ETHERTYPE1_E;

            st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet(dev, port, egressVlanSel);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, egressVlanSel);

            /*
                1.1.2. Call cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet with non-NULL egressVlanSel.
                Expected: GT_OK and the same egressVlanSel.
            */
            st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet(dev, port, &egressVlanSelGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(egressVlanSel, egressVlanSelGet,
                       "get another egressVlanSel than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range egressVlanSel [0x5AAAAAA5].
                Expected: GT_BAD_PARAM.
            */
            egressVlanSel = BRG_NEST_VLAN_INVALID_ENUM_CNS;

            st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet(dev, port, egressVlanSel);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, egressVlanSel);
        }

        egressVlanSel = CPSS_VLAN_ETHERTYPE0_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet(dev, port, egressVlanSel);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet(dev, port, egressVlanSel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet(dev, port, egressVlanSel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    egressVlanSel = CPSS_VLAN_ETHERTYPE0_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_NEST_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet(dev, port, egressVlanSel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet(dev, port, egressVlanSel);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    OUT CPSS_ETHER_MODE_ENT     *egressVlanSelPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-NULL egressVlanSelPtr.
    Expected: GT_OK.
    1.1.2. Call with egressVlanSelPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_NEST_VLAN_VALID_VIRT_PORT_CNS;

    CPSS_ETHER_MODE_ENT     egressVlanSel = CPSS_VLAN_ETHERTYPE0_E;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL egressVlanSelPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet(dev, port, &egressVlanSel);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with egressVlanSelPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, egressVlanSelPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet(dev, port, &egressVlanSel);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet(dev, port, &egressVlanSel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet(dev, port, &egressVlanSel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_NEST_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet(dev, port, &egressVlanSel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet(dev, port, &egressVlanSel);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmBrgNestVlan suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmBrgNestVlan)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgNestVlanAccessPortEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgNestVlanAccessPortEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmBrgNestVlan)

