/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmInlifUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmInlif, that provides
*       APIs for setting input logical interfaces entry
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/inlif/cpssExMxPmInlif.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define INLIF_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define INLIF_INVALID_ENUM_CNS    0x5AAAAAA5

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifPortDefaultInlifIdSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  inlifId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifPortDefaultInlifIdSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with inlifId [0 / 0xFFFF].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmInlifPortDefaultInlifIdGet with non-NULL inlifIdPtr.
    Expected: GT_OK and the same inlifId
    1.1.3. Call with out of range inlifId [0x10000].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_U32  inlifId    = 0;
    GT_U32  inlifIdGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with inlifId [0 / 0xFFFF].
                Expected: GT_OK.
            */
            /* iterate with inlifId = 0 */
            inlifId = 0;

            st = cpssExMxPmInlifPortDefaultInlifIdSet(dev, port, inlifId);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, inlifId);

            /*
                1.1.2. Call cpssExMxPmInlifPortDefaultInlifIdGet with non-NULL inlifIdPtr.
                Expected: GT_OK and the same inlifId
            */
            st = cpssExMxPmInlifPortDefaultInlifIdGet(dev, port, &inlifIdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmInlifPortDefaultInlifIdGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(inlifId, inlifIdGet, "got another inlifId than was set: %d, %d", dev, port);

            /* iterate with inlifId = 63 */
            inlifId = 0xFFFF;

            st = cpssExMxPmInlifPortDefaultInlifIdSet(dev, port, inlifId);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, inlifId);

            /*
                1.1.2. Call cpssExMxPmInlifPortDefaultInlifIdGet with non-NULL inlifIdPtr.
                Expected: GT_OK and the same inlifId
            */
            st = cpssExMxPmInlifPortDefaultInlifIdGet(dev, port, &inlifIdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmInlifPortDefaultInlifIdGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(inlifId, inlifIdGet, "got another inlifId than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range inlifId [0x10000].
                Expected: NOT GT_OK.
            */
            inlifId = 0x10000;

            st = cpssExMxPmInlifPortDefaultInlifIdSet(dev, port, inlifId);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, inlifId);
        }

        inlifId = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmInlifPortDefaultInlifIdSet(dev, port, inlifId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmInlifPortDefaultInlifIdSet(dev, port, inlifId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmInlifPortDefaultInlifIdSet(dev, port, inlifId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    inlifId = 0;
    port    = INLIF_VALID_VIRT_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifPortDefaultInlifIdSet(dev, port, inlifId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifPortDefaultInlifIdSet(dev, port, inlifId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifPortDefaultInlifIdGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U32  *inlifIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifPortDefaultInlifIdGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with not NULL inlifIdPtr
    Expected: GT_OK.
    1.1.2. Call with inlifIdPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_U32  inlifId    = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not NULL inlifIdPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmInlifPortDefaultInlifIdGet(dev, port, &inlifId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with inlifIdPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmInlifPortDefaultInlifIdGet(dev, port,  NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, inlifIdPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmInlifPortDefaultInlifIdGet(dev, port, &inlifId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmInlifPortDefaultInlifIdGet(dev, port, &inlifId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmInlifPortDefaultInlifIdGet(dev, port, &inlifId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    port    = INLIF_VALID_VIRT_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifPortDefaultInlifIdGet(dev, port, &inlifId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifPortDefaultInlifIdGet(dev, port, &inlifId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifPortModeSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    IN  CPSS_EXMXPM_INLIF_PORT_MODE_ENT     inlifMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifPortModeSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with inlifMode [CPSS_EXMXPM_INLIF_PORT_MODE_PORT_E /
                                CPSS_EXMXPM_INLIF_PORT_MODE_VLAN_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmInlifPortModeGet with non-NULL inlifModePtr.
    Expected: GT_OK and the same inlifMode
    1.1.3. Call with out of range inlifMode [0x5AAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = INLIF_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_INLIF_PORT_MODE_ENT inlifMode    = CPSS_EXMXPM_INLIF_PORT_MODE_PORT_E;
    CPSS_EXMXPM_INLIF_PORT_MODE_ENT inlifModeGet = CPSS_EXMXPM_INLIF_PORT_MODE_PORT_E;


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
                1.1.1. Call with inlifMode [CPSS_EXMXPM_INLIF_PORT_MODE_PORT_E /
                                            CPSS_EXMXPM_INLIF_PORT_MODE_VLAN_E].
                Expected: GT_OK.
            */

            /* Call with inlifMode [CPSS_EXMXPM_INLIF_PORT_MODE_PORT_E] */
            inlifMode = CPSS_EXMXPM_INLIF_PORT_MODE_PORT_E;

            st = cpssExMxPmInlifPortModeSet(dev, port, inlifMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, inlifMode);

            /*
                1.1.2. Call cpssExMxPmInlifPortModeGet with non-NULL inlifModePtr.
                Expected: GT_OK and the same inlifMode
            */
            st = cpssExMxPmInlifPortModeGet(dev, port, &inlifModeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmInlifPortModeGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(inlifMode, inlifModeGet,
                       "get another inlifMode than was set: %d, %d", dev, port);

            /* Call with inlifMode [CPSS_EXMXPM_INLIF_PORT_MODE_VLAN_E] */
            inlifMode = CPSS_EXMXPM_INLIF_PORT_MODE_VLAN_E;

            st = cpssExMxPmInlifPortModeSet(dev, port, inlifMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, inlifMode);

            /*
                1.1.2. Call cpssExMxPmInlifPortModeGet with non-NULL inlifModePtr.
                Expected: GT_OK and the same inlifMode
            */
            st = cpssExMxPmInlifPortModeGet(dev, port, &inlifModeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmInlifPortModeGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(inlifMode, inlifModeGet,
                       "get another inlifMode than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range inlifMode [0x5AAAAA5].
                Expected: GT_BAD_PARAM.
            */
            inlifMode = INLIF_INVALID_ENUM_CNS;

            st = cpssExMxPmInlifPortModeSet(dev, port, inlifMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, inlifMode);
        }

        inlifMode = CPSS_EXMXPM_INLIF_PORT_MODE_PORT_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmInlifPortModeSet(dev, port, inlifMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmInlifPortModeSet(dev, port, inlifMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmInlifPortModeSet(dev, port, inlifMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    inlifMode = CPSS_EXMXPM_INLIF_PORT_MODE_PORT_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = INLIF_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifPortModeSet(dev, port, inlifMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifPortModeSet(dev, port, inlifMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifPortModeGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    OUT CPSS_EXMXPM_INLIF_PORT_MODE_ENT *inlifModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifPortModeGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-NULL inlifModePtr.
    Expected: GT_OK.
    1.1.2. Call with inlifModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = INLIF_VALID_VIRT_PORT_CNS;

    CPSS_EXMXPM_INLIF_PORT_MODE_ENT inlifMode = CPSS_EXMXPM_INLIF_PORT_MODE_PORT_E;

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
                1.1.1. Call with non-NULL inlifModePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmInlifPortModeGet(dev, port, &inlifMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with inlifModePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmInlifPortModeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, inlifModePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmInlifPortModeGet(dev, port, &inlifMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmInlifPortModeGet(dev, port, &inlifMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmInlifPortModeGet(dev, port, &inlifMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = INLIF_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifPortModeGet(dev, port, &inlifMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifPortModeGet(dev, port, &inlifMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifIntForceVlanModeOnTtSet
(
    IN GT_U8              devNum,
    IN GT_BOOL            enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifIntForceVlanModeOnTtSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmInlifIntForceVlanModeOnTtGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssExMxPmInlifIntForceVlanModeOnTtSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmInlifIntForceVlanModeOnTtGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmInlifIntForceVlanModeOnTtGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmInlifIntForceVlanModeOnTtGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxPmInlifIntForceVlanModeOnTtSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmInlifIntForceVlanModeOnTtGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmInlifIntForceVlanModeOnTtGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmInlifIntForceVlanModeOnTtGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);
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
        st = cpssExMxPmInlifIntForceVlanModeOnTtSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifIntForceVlanModeOnTtSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifIntForceVlanModeOnTtGet
(
    IN  GT_U8              devNum,
    OUT GT_BOOL            *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifIntForceVlanModeOnTtGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmInlifIntForceVlanModeOnTtGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmInlifIntForceVlanModeOnTtGet(dev, NULL);
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
        st = cpssExMxPmInlifIntForceVlanModeOnTtGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifIntForceVlanModeOnTtGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifEntrySet
(
    IN GT_U8                        devNum,
    IN CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType,
    IN GT_U32                       inlifIndex,
    IN CPSS_EXMXPM_INLIF_ENTRY_STC *inlifEntryPtr

)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   inlifIndex [63 / 4095 / 4096],
                   inlifEntryPtr-> {  bridgeEnable [GT_TRUE],
                                      autoLearnEnable [GT_TRUE],
                                      naMessageToCpuEnable [GT_TRUE],
                                      naStormPreventionEnable [GT_TRUE],
                                      unkSaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                      unkDaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                      unkSaNotSecurBreachEnable [GT_TRUE],
                                      untaggedMruIndex[0 / 5 / 7],
                                      unregNonIpMcCommand [CPSS_PACKET_CMD_FORWARD_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                      unregIpMcCommand [CPSS_PACKET_CMD_FORWARD_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                      unregIpv4BcCommand [CPSS_PACKET_CMD_FORWARD_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                      unregNonIpv4BcCommand[CPSS_PACKET_CMD_FORWARD_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                      ipv4UcRouteEnable [GT_TRUE],
                                      ipv4McRouteEnable [GT_TRUE],
                                      ipv6UcRouteEnable [GT_TRUE],
                                      ipv6McRouteEnable [GT_TRUE],
                                      mplsRouteEnable [GT_TRUE],
                                      vrfId [0 / 100 / 4096-1],
                                      ipv4IcmpRedirectEnable [GT_TRUE],
                                      ipv6IcmpRedirectEnable [GT_TRUE],
                                      bridgeRouterInterfaceEnable [GT_TRUE],
                                      ipSecurityProfile [0 / 4 / 8 / 15],
                                      ipv4IgmpToCpuEnable [GT_TRUE],
                                      ipv6IgmpToCpuEnable [GT_TRUE],
                                      udpBcRelayEnable [GT_TRUE],
                                      arpBcToCpuEnable [GT_TRUE],
                                      arpBcToMeEnable [GT_TRUE],
                                      ripv1MirrorEnable [GT_TRUE],
                                      ipv4LinkLocalMcCommandEnable [GT_TRUE],
                                      ipv6LinkLocalMcCommandEnable [GT_TRUE],
                                      ipv6NeighborSolicitationEnable [GT_TRUE],;
                                      mirrorToAnalyzerEnable [GT_TRUE],
                                      mirrorToCpuEnable [GT_TRUE] }
    Expected: GT_OK.
    1.2. Call cpssExMxPmInlifEntryGet with non-NULL inlifEntryPtr and the same inlifIndex as in 1.1.
    Expected: GT_OK and the same inlifEntryPtr.
    1.3. Call with out of range inlifEntryPtr->unregNonIpMcCommand [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range inlifEntryPtr->unregIpMcCommand [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range inlifEntryPtr->unregIpv4BcCommand [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range inlifEntryPtr->unregNonIpv4BcCommand [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.

    1.7. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.10. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.11. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.12. Call with out of range inlifEntryPtr->unkSaUcCommand [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.13. Call with out of range inlifEntryPtr->unkDaUcCommand [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.14. Call with inlifEntryPtr->unkSaUcCommand [ CPSS_PACKET_CMD_FORWARD_E
                                                    CPSS_PACKET_CMD_ROUTE_E
                                                    CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E
                                                    CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E
                                                    CPSS_PACKET_CMD_NONE_E] (not supported),
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.15. Call with inlifEntryPtr->unkDaUcCommand [ CPSS_PACKET_CMD_ROUTE_E
                                                    CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E
                                                    CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E
                                                    CPSS_PACKET_CMD_BRIDGE_E
                                                    CPSS_PACKET_CMD_NONE_E] (not supported),
                   and other parameters from 1.1.
    Expected: NOT GT_OK.

    1.16. Call with inlifEntryPtr->unregNonIpMcCommand [ CPSS_PACKET_CMD_ROUTE_E
                                                    CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E
                                                    CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E
                                                    CPSS_PACKET_CMD_BRIDGE_E
                                                    CPSS_PACKET_CMD_NONE_E] (not supported),
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.17. Call with inlifEntryPtr->unregIpMcCommand [ CPSS_PACKET_CMD_ROUTE_E
                                                    CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E
                                                    CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E
                                                    CPSS_PACKET_CMD_BRIDGE_E
                                                    CPSS_PACKET_CMD_NONE_E] (not supported),
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.18. Call with inlifEntryPtr->unregIpv4BcCommand [ CPSS_PACKET_CMD_ROUTE_E
                                                    CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E
                                                    CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E
                                                    CPSS_PACKET_CMD_BRIDGE_E
                                                    CPSS_PACKET_CMD_NONE_E] (not supported),
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.19. Call with inlifEntryPtr->unregNonIpv4BcCommand [ CPSS_PACKET_CMD_ROUTE_E
                                                    CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E
                                                    CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E
                                                    CPSS_PACKET_CMD_BRIDGE_E
                                                    CPSS_PACKET_CMD_NONE_E] (not supported),
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.20. Call with inlifEntryPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     isEqual = GT_FALSE;
    GT_U32      inlifIndex;
    CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;

    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntry;
    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntryGet;


    cpssOsBzero((GT_VOID*) &inlifEntry, sizeof(inlifEntry));
    cpssOsBzero((GT_VOID*) &inlifEntryGet, sizeof(inlifEntryGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           inlifEntryPtr-> {  bridgeEnable [GT_TRUE],
                                              autoLearnEnable [GT_TRUE],
                                              naMessageToCpuEnable [GT_TRUE],
                                              naStormPreventionEnable [GT_TRUE],
                                              unkSaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                              unkDaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                              unkSaNotSecurBreachEnable [GT_TRUE],
                                              untaggedMruIndex[0 / 5 / 7],
                                              unregNonIpMcCommand [CPSS_PACKET_CMD_FORWARD_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                              unregIpMcCommand [CPSS_PACKET_CMD_FORWARD_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                              unregIpv4BcCommand [CPSS_PACKET_CMD_FORWARD_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                              unregNonIpv4BcCommand[CPSS_PACKET_CMD_FORWARD_E / CPSS_PACKET_CMD_DROP_SOFT_E / CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                              ipv4UcRouteEnable [GT_TRUE],
                                              ipv4McRouteEnable [GT_TRUE],
                                              ipv6UcRouteEnable [GT_TRUE],
                                              ipv6McRouteEnable [GT_TRUE],
                                              mplsRouteEnable [GT_TRUE],
                                              vrfId [0 / 100 / 4096-1],
                                              ipv4IcmpRedirectEnable [GT_TRUE],
                                              ipv6IcmpRedirectEnable [GT_TRUE],
                                              bridgeRouterInterfaceEnable [GT_TRUE],
                                              ipSecurityProfile [0 / 4 / 15],
                                              ipv4IgmpToCpuEnable [GT_TRUE],
                                              ipv6IgmpToCpuEnable [GT_TRUE],
                                              udpBcRelayEnable [GT_TRUE],
                                              arpBcToCpuEnable [GT_TRUE],
                                              arpBcToMeEnable [GT_TRUE],
                                              ripv1MirrorEnable [GT_TRUE],
                                              ipv4LinkLocalMcCommandEnable [GT_TRUE],
                                              ipv6LinkLocalMcCommandEnable [GT_TRUE],
                                              ipv6NeighborSolicitationEnable [GT_TRUE],;
                                              mirrorToAnalyzerEnable [GT_TRUE],
                                              mirrorToCpuEnable [GT_TRUE],
            Expected: GT_OK.
        */
        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        inlifEntry.bridgeEnable = GT_TRUE;
        inlifEntry.autoLearnEnable = GT_TRUE;
        inlifEntry.naMessageToCpuEnable = GT_TRUE;
        inlifEntry.naStormPreventionEnable = GT_TRUE;
        inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_DROP_HARD_E;
        inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_DROP_HARD_E;
        inlifEntry.unkSaNotSecurBreachEnable = GT_TRUE;
        inlifEntry.untaggedMruIndex = 0;
        inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_FORWARD_E;
        inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_FORWARD_E;
        inlifEntry.unregIpv4BcCommand = CPSS_PACKET_CMD_FORWARD_E;
        inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_FORWARD_E;
        inlifEntry.ipv4UcRouteEnable = GT_TRUE;
        inlifEntry.ipv4McRouteEnable = GT_TRUE;
        inlifEntry.ipv6UcRouteEnable = GT_TRUE;
        inlifEntry.ipv6McRouteEnable = GT_TRUE;
        inlifEntry.mplsRouteEnable = GT_TRUE;
        inlifEntry.vrfId = 0;
        inlifEntry.ipv4IcmpRedirectEnable = GT_TRUE;
        inlifEntry.ipv6IcmpRedirectEnable = GT_TRUE;
        inlifEntry.bridgeRouterInterfaceEnable = GT_TRUE;
        inlifEntry.ipSecurityProfile = 0;
        inlifEntry.ipv4IgmpToCpuEnable = GT_TRUE;
        inlifEntry.ipv6IcmpToCpuEnable = GT_TRUE;
        inlifEntry.udpBcRelayEnable = GT_TRUE;
        inlifEntry.arpBcToCpuEnable = GT_TRUE;
        inlifEntry.arpBcToMeEnable = GT_TRUE;
        inlifEntry.ripv1MirrorEnable = GT_TRUE;
        inlifEntry.ipv4LinkLocalMcCommandEnable = GT_TRUE;
        inlifEntry.ipv6LinkLocalMcCommandEnable = GT_TRUE;
        inlifEntry.ipv6NeighborSolicitationEnable = GT_TRUE;;
        inlifEntry.mirrorToAnalyzerEnable = GT_TRUE;
        inlifEntry.mirrorToCpuEnable = GT_TRUE;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, inlifIndex);

        /*
            1.2. Call cpssExMxPmInlifEntryGet with non-NULL inlifEntryPtr and the same inlifIndex as in 1.1.
            Expected: GT_OK and the same inlifEntryPtr.
        */
        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d", dev, inlifIndex);

        if (GT_OK == st)
        {
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &inlifEntry, (GT_VOID*) &inlifEntryGet, sizeof(inlifEntry) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another inlifEntry than was set: %d", dev);
        }

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        inlifEntry.bridgeEnable = GT_FALSE;
        inlifEntry.autoLearnEnable = GT_FALSE;
        inlifEntry.naMessageToCpuEnable = GT_FALSE;
        inlifEntry.naStormPreventionEnable = GT_FALSE;
        inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
        inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
        inlifEntry.unkSaNotSecurBreachEnable = GT_FALSE;
        inlifEntry.untaggedMruIndex = 5;
        inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
        inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
        inlifEntry.unregIpv4BcCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
        inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
        inlifEntry.ipv4UcRouteEnable = GT_FALSE;
        inlifEntry.ipv4McRouteEnable = GT_FALSE;
        inlifEntry.ipv6UcRouteEnable = GT_FALSE;
        inlifEntry.ipv6McRouteEnable = GT_FALSE;
        inlifEntry.mplsRouteEnable = GT_FALSE;
        inlifEntry.vrfId = 100;
        inlifEntry.ipv4IcmpRedirectEnable = GT_FALSE;
        inlifEntry.ipv6IcmpRedirectEnable = GT_FALSE;
        inlifEntry.bridgeRouterInterfaceEnable = GT_FALSE;
        inlifEntry.ipSecurityProfile = 4;
        inlifEntry.ipv4IgmpToCpuEnable = GT_FALSE;
        inlifEntry.ipv6IcmpToCpuEnable = GT_FALSE;
        inlifEntry.udpBcRelayEnable = GT_FALSE;
        inlifEntry.arpBcToCpuEnable = GT_FALSE;
        inlifEntry.arpBcToMeEnable = GT_FALSE;
        inlifEntry.ripv1MirrorEnable = GT_FALSE;
        inlifEntry.ipv4LinkLocalMcCommandEnable = GT_FALSE;
        inlifEntry.ipv6LinkLocalMcCommandEnable = GT_FALSE;
        inlifEntry.ipv6NeighborSolicitationEnable = GT_FALSE;
        inlifEntry.mirrorToAnalyzerEnable = GT_FALSE;
        inlifEntry.mirrorToCpuEnable = GT_FALSE;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, inlifIndex);

        /*
            1.2. Call cpssExMxPmInlifEntryGet with non-NULL inlifEntryPtr and the same inlifIndex as in 1.1.
            Expected: GT_OK and the same inlifEntryPtr.
        */
        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d", dev, inlifIndex);

        if (GT_OK == st)
        {
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &inlifEntry, (GT_VOID*) &inlifEntryGet, sizeof(inlifEntry) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another inlifEntry than was set: %d", dev);
        }

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        inlifEntry.untaggedMruIndex = 7;
        inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        inlifEntry.unregIpv4BcCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        inlifEntry.vrfId = 4096 - 1;
        inlifEntry.ipSecurityProfile = 15;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, inlifIndex);

        /*
            1.2. Call cpssExMxPmInlifEntryGet with non-NULL inlifEntryPtr and the same inlifIndex as in 1.1.
            Expected: GT_OK and the same inlifEntryPtr.
        */
        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d", dev, inlifIndex);

        if (GT_OK == st)
        {
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &inlifEntry, (GT_VOID*) &inlifEntryGet, sizeof(inlifEntry) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another inlifEntry than was set: %d", dev);
        }

        /*
            1.3. Call with out of range inlifEntryPtr->unregNonIpMcCommand [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifEntry.unregNonIpMcCommand = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, inlifIndex = %d, inlifEntryPtr->unregNonIpMcCommand = %d",
                                     dev, inlifIndex, inlifEntry.unregNonIpMcCommand);

        inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_FORWARD_E;

        /*
            1.4. Call with out of range inlifEntryPtr->unregIpMcCommand [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifEntry.unregIpMcCommand = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, inlifIndex = %d, inlifEntryPtr->unregIpMcCommand = %d",
                                     dev, inlifIndex, inlifEntry.unregIpMcCommand);

        inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        /*
            1.5. Call with out of range inlifEntryPtr->unregNonIpMcCommand [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifEntry.unregNonIpMcCommand = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, inlifIndex = %d, inlifEntryPtr->unregNonIpMcCommand = %d",
                                     dev, inlifIndex, inlifEntry.unregNonIpMcCommand);

        inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        /*
            1.6. Call with out of range inlifEntryPtr->unregNonIpv4BcCommand [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifEntry.unregNonIpv4BcCommand = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, inlifIndex = %d, inlifEntryPtr->unregNonIpv4BcCommand = %d",
                                     dev, inlifIndex, inlifEntry.unregNonIpv4BcCommand);

        inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_DROP_HARD_E;

        /*
            1.7. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.8. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.9. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.10. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 0;

        /*
            1.11. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);

        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;

        /*
            1.12. Call with out of range inlifEntryPtr->unkSaUcCommand [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifEntry.unkSaUcCommand = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, inlifIndex = %d, inlifEntryPtr->unkSaUcCommand = %d",
                                     dev, inlifIndex, inlifEntry.unkSaUcCommand);

        inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        /*
            1.13. Call with out of range inlifEntryPtr->unkDaUcCommand [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifEntry.unkDaUcCommand = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, inlifIndex = %d, inlifEntryPtr->unkDaUcCommand = %d",
                                     dev, inlifIndex, inlifEntry.unkDaUcCommand);


        inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        /*
            1.14. Call with inlifEntryPtr->unkSaUcCommand [ CPSS_PACKET_CMD_FORWARD_E
                                                            CPSS_PACKET_CMD_ROUTE_E
                                                            CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E
                                                            CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E
                                                            CPSS_PACKET_CMD_NONE_E] (not supported),
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unkSaUcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unkSaUcCommand);

        inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unkSaUcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unkSaUcCommand);

        inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unkSaUcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unkSaUcCommand);

        inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unkSaUcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unkSaUcCommand);

        inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_NONE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unkSaUcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unkSaUcCommand);

        inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_DROP_HARD_E;

        /*
            1.15. Call with inlifEntryPtr->unkDaUcCommand [ CPSS_PACKET_CMD_ROUTE_E
                                                            CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E
                                                            CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E
                                                            CPSS_PACKET_CMD_BRIDGE_E
                                                            CPSS_PACKET_CMD_NONE_E] (not supported),
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_BRIDGE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unkDaUcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unkDaUcCommand);

        inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unkDaUcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unkDaUcCommand);

        inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unkDaUcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unkDaUcCommand);

        inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unkDaUcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unkDaUcCommand);

        inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_NONE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unkDaUcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unkDaUcCommand);

        inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_DROP_HARD_E;

        /*
            1.16. Call with inlifEntryPtr->unregNonIpMcCommand [ CPSS_PACKET_CMD_ROUTE_E
                                                            CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E
                                                            CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E
                                                            CPSS_PACKET_CMD_BRIDGE_E
                                                            CPSS_PACKET_CMD_NONE_E] (not supported),
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_BRIDGE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregNonIpMcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregNonIpMcCommand);

        inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregNonIpMcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregNonIpMcCommand);

        inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregNonIpMcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregNonIpMcCommand);

        inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregNonIpMcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregNonIpMcCommand);

        inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_NONE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregNonIpMcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregNonIpMcCommand);

        inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_DROP_HARD_E;

        /*
            1.17. Call with inlifEntryPtr->unregIpMcCommand [ CPSS_PACKET_CMD_ROUTE_E
                                                            CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E
                                                            CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E
                                                            CPSS_PACKET_CMD_BRIDGE_E
                                                            CPSS_PACKET_CMD_NONE_E] (not supported),
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_BRIDGE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregIpMcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregIpMcCommand);

        inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregIpMcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregIpMcCommand);

        inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregIpMcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregIpMcCommand);

        inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregIpMcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregIpMcCommand);

        inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_NONE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregIpMcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregIpMcCommand);

        inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_DROP_HARD_E;

        /*
            1.18. Call with inlifEntryPtr->unregIpv4BcCommand [ CPSS_PACKET_CMD_ROUTE_E
                                                            CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E
                                                            CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E
                                                            CPSS_PACKET_CMD_BRIDGE_E
                                                            CPSS_PACKET_CMD_NONE_E] (not supported),
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifEntry.unregIpv4BcCommand = CPSS_PACKET_CMD_BRIDGE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregIpv4BcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregIpv4BcCommand);

        inlifEntry.unregIpv4BcCommand = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregIpv4BcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregIpv4BcCommand);

        inlifEntry.unregIpv4BcCommand = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregIpv4BcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregIpv4BcCommand);

        inlifEntry.unregIpv4BcCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregIpv4BcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregIpv4BcCommand);

        inlifEntry.unregIpv4BcCommand = CPSS_PACKET_CMD_NONE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregIpv4BcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregIpv4BcCommand);

        inlifEntry.unregIpv4BcCommand = CPSS_PACKET_CMD_DROP_HARD_E;

        /*
            1.19. Call with inlifEntryPtr->unregNonIpv4BcCommand [ CPSS_PACKET_CMD_ROUTE_E
                                                            CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E
                                                            CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E
                                                            CPSS_PACKET_CMD_BRIDGE_E
                                                            CPSS_PACKET_CMD_NONE_E] (not supported),
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_BRIDGE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregNonIpv4BcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregNonIpv4BcCommand);

        inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregNonIpv4BcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregNonIpv4BcCommand);

        inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregNonIpv4BcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregNonIpv4BcCommand);

        inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregNonIpv4BcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregNonIpv4BcCommand);

        inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_NONE_E;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, inlifIndex = %d, inlifEntryPtr->unregNonIpv4BcCommand = %d",
                                         dev, inlifIndex, inlifEntry.unregNonIpv4BcCommand);

        inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_DROP_HARD_E;

        /*
            1.20. Call with inlifEntryPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, inlifEntryPtr = NULL", dev);
    }

    inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex = 63;
    inlifEntry.bridgeEnable = GT_TRUE;
    inlifEntry.autoLearnEnable = GT_TRUE;
    inlifEntry.naMessageToCpuEnable = GT_TRUE;
    inlifEntry.naStormPreventionEnable = GT_TRUE;
    inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_DROP_HARD_E;
    inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
    inlifEntry.unkSaNotSecurBreachEnable = GT_TRUE;
    inlifEntry.untaggedMruIndex = 0;
    inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_DROP_HARD_E;
    inlifEntry.unregIpv4BcCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
    inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
    inlifEntry.ipv4UcRouteEnable = GT_TRUE;
    inlifEntry.ipv4McRouteEnable = GT_TRUE;
    inlifEntry.ipv6UcRouteEnable = GT_TRUE;
    inlifEntry.ipv6McRouteEnable = GT_TRUE;
    inlifEntry.mplsRouteEnable = GT_TRUE;
    inlifEntry.vrfId = 0;
    inlifEntry.ipv4IcmpRedirectEnable = GT_TRUE;
    inlifEntry.ipv6IcmpRedirectEnable = GT_TRUE;
    inlifEntry.bridgeRouterInterfaceEnable = GT_TRUE;
    inlifEntry.ipSecurityProfile = 0;
    inlifEntry.ipv4IgmpToCpuEnable = GT_TRUE;
    inlifEntry.ipv6IcmpToCpuEnable = GT_TRUE;
    inlifEntry.udpBcRelayEnable = GT_TRUE;
    inlifEntry.arpBcToCpuEnable = GT_TRUE;
    inlifEntry.arpBcToMeEnable = GT_TRUE;
    inlifEntry.ripv1MirrorEnable = GT_TRUE;
    inlifEntry.ipv4LinkLocalMcCommandEnable = GT_TRUE;
    inlifEntry.ipv6LinkLocalMcCommandEnable = GT_TRUE;
    inlifEntry.ipv6NeighborSolicitationEnable = GT_TRUE;;
    inlifEntry.mirrorToAnalyzerEnable = GT_TRUE;
    inlifEntry.mirrorToCpuEnable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifEntryGet
(
    IN  GT_U8                        devNum,
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType,
    IN  GT_U32                       inlifIndex,
    OUT CPSS_EXMXPM_INLIF_ENTRY_STC *inlifEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifEntryGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   inlifIndex [63 / 4095],
                   and not NULL inlifEntryPtr.
    Expected: GT_OK.
    1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E], inlifIndex[65535] and not NULL inlifEntryPtr.
    Expected: NOT GT_OK.
    1.3. Call with inlifEntryPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      inlifIndex = 0;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntry;


    cpssOsBzero((GT_VOID*) &inlifEntry, sizeof(inlifEntry));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           inlifIndex [63 / 4095],
                           and not NULL inlifEntryPtr.
            Expected: GT_OK.
        */
        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, inlifIndex);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, inlifIndex);

        /*
            1.2. Call with inlifIndex[65535] and not NULL inlifEntryPtr.
            Expected: NOT GT_OK.
        */
        inlifIndex = 65535;

        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, inlifIndex);

        inlifIndex = 0;

        /*
            1.3. Call with inlifEntryPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, inlifEntryPtr = NULL", dev);

        /*
            1.4. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifBridgeEnableSet
(
    IN GT_U8                        devNum,
    IN CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType,
    IN GT_U32                       inlifIndex,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifBridgeEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   inlifIndex [63 / 4095 / 4096],
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      inlifIndex = 0;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    GT_BOOL                     enable     = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with inlifIndex [0] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;
        enable     = GT_FALSE;

        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4095] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4096] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4096;

        /*
            1.6. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex = 63;
    enable     = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifUnkSaNotSecurBreachEnableSet
(
    IN GT_U8                        devNum,
    IN CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType,
    IN GT_U32                       inlifIndex,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifUnkSaNotSecurBreachEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   inlifIndex [63 / 4095 / 4096],
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      inlifIndex = 0;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    GT_BOOL                     enable     = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with inlifIndex [0] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;
        enable     = GT_FALSE;

        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4095] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4096] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4096;

        /*
            1.6. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex = 63;
    enable     = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifNaStormPreventionEnableSet
(
    IN GT_U8                        devNum,
    IN CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType,
    IN GT_U32                       inlifIndex,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifNaStormPreventionEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   inlifIndex [63 / 4095 / 4096],
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      inlifIndex = 0;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    GT_BOOL                     enable     = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with inlifIndex [0] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;
        enable     = GT_FALSE;

        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4095] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4096] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4096;

        /*
            1.6. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex = 63;
    enable     = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifAutoLearnEnableSet
(
    IN GT_U8                        devNum,
    IN CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType,
    IN GT_U32                       inlifIndex,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifAutoLearnEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   inlifIndex [63 / 4095 / 4096],
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with inlifType [0x5AAAAAA5],
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      inlifIndex = 0;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    GT_BOOL                     enable     = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with inlifIndex [0] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;
        enable     = GT_FALSE;

        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4095] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4096] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4096;

        /*
            1.6. Call with inlifType [0x5AAAAAA5],
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex = 63;
    enable     = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifNaToCpuEnableSet
(
    IN GT_U8                        devNum,
    IN CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType,
    IN GT_U32                       inlifIndex,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifNaToCpuEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   inlifIndex [63 / 4095 / 4096],
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with inlifType [0x5AAAAAA5],
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      inlifIndex = 0;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    GT_BOOL                     enable     = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with inlifIndex [0] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;
        enable     = GT_FALSE;

        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4095] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4096] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4096;

        /*
            1.6. Call with inlifType [0x5AAAAAA5],
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex = 63;
    enable     = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifUnkUnregAddrCmdSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT              inlifType,
    IN  GT_U32                                  inlifIndex,
    IN  CPSS_EXMXPM_INLIF_UNK_UNREG_ADDR_ENT    addrType,
    IN  CPSS_PACKET_CMD_ENT                     cmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifUnkUnregAddrCmdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   inlifIndex [63 / 4095 / 4096],
                   addrType [ CPSS_EXMXPM_INLIF_UNK_UC_SA_E /
                              CPSS_EXMXPM_INLIF_UNK_UC_DA_E /
                              CPSS_EXMXPM_INLIF_UNREG_NON_IP_MC_E /
                              CPSS_EXMXPM_INLIF_UNREG_NON_IP4_BC_E /
                              CPSS_EXMXPM_INLIF_UNREG_IP_MC_E /
                              CPSS_EXMXPM_INLIF_UNREG_IP_V4_BC_E ]
                   and cmd [CPSS_PACKET_CMD_FORWARD_E /
                            CPSS_PACKET_CMD_BRIDGE_E /
                            CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                            CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                            CPSS_PACKET_CMD_DROP_HARD_E /
                            CPSS_PACKET_CMD_DROP_SOFT_E].
    Expected: GT_OK.
    1.2. Call with out of range inlifIndex [65536]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with out of range addrType [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with cmd [CPSS_PACKET_CMD_ROUTE_E /
                        CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E /
                        CPSS_PACKET_CMD_BRIDGE_E /
                        CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E /
                        CPSS_PACKET_CMD_NONE_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range cmd [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range inlifType [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_INLIF_TYPE_ENT              inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    GT_U32                                  inlifIndex = 0;
    CPSS_EXMXPM_INLIF_UNK_UNREG_ADDR_ENT    addrType   = CPSS_EXMXPM_INLIF_UNK_UC_SA_E;
    CPSS_PACKET_CMD_ENT                     cmd        = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           addrType [ CPSS_EXMXPM_INLIF_UNK_UC_SA_E /
                                      CPSS_EXMXPM_INLIF_UNK_UC_DA_E /
                                      CPSS_EXMXPM_INLIF_UNREG_NON_IP_MC_E /
                                      CPSS_EXMXPM_INLIF_UNREG_NON_IP4_BC_E /
                                      CPSS_EXMXPM_INLIF_UNREG_IP_MC_E /
                                      CPSS_EXMXPM_INLIF_UNREG_IP_V4_BC_E ]
                           and cmd [CPSS_PACKET_CMD_FORWARD_E /
                                    CPSS_PACKET_CMD_BRIDGE_E /
                                    CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                                    CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                    CPSS_PACKET_CMD_DROP_HARD_E /
                                    CPSS_PACKET_CMD_DROP_SOFT_E].
            Expected: GT_OK.
        */

        /* Call with addrType = CPSS_EXMXPM_INLIF_UNK_UC_SA_E */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;
        addrType   = CPSS_EXMXPM_INLIF_UNK_UC_SA_E;
        cmd        = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with addrType = CPSS_EXMXPM_INLIF_UNK_UC_DA_E; */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;
        addrType   = CPSS_EXMXPM_INLIF_UNK_UC_DA_E;
        cmd        = CPSS_PACKET_CMD_BRIDGE_E;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with addrType = CPSS_EXMXPM_INLIF_UNREG_NON_IP_MC_E */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;
        addrType   = CPSS_EXMXPM_INLIF_UNREG_NON_IP_MC_E;
        cmd        = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with addrType = CPSS_EXMXPM_INLIF_UNREG_NON_IP4_BC_E */
        addrType   = CPSS_EXMXPM_INLIF_UNREG_NON_IP4_BC_E;
        cmd        = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with addrType = CPSS_EXMXPM_INLIF_UNREG_IP_MC_E */
        addrType   = CPSS_EXMXPM_INLIF_UNREG_IP_MC_E;
        cmd        = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with addrType = CPSS_EXMXPM_INLIF_UNREG_IP_V4_BC_E */
        addrType   = CPSS_EXMXPM_INLIF_UNREG_IP_V4_BC_E;
        cmd        = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with out of range inlifIndex [65536]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 65536;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, inlifIndex);

        inlifIndex = 0;

        /*
            1.3. Call with out of range addrType [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        addrType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, addrType = %d", dev, addrType);

        addrType = CPSS_EXMXPM_INLIF_UNREG_NON_IP_MC_E;

        /*
            1.4. Call with cmd [CPSS_PACKET_CMD_ROUTE_E /
                                CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E /
                                CPSS_PACKET_CMD_BRIDGE_E /
                                CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E /
                                CPSS_PACKET_CMD_NONE_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with cmd [CPSS_PACKET_CMD_ROUTE_E]*/
        cmd = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E]*/
        cmd = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_BRIDGE_E]*/
        cmd = CPSS_PACKET_CMD_BRIDGE_E;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E]*/
        cmd = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_NONE_E]*/
        cmd = CPSS_PACKET_CMD_NONE_E;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /*
            1.5. Call with out of range cmd [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        cmd = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cmd = %d", dev, cmd);

        cmd = CPSS_PACKET_CMD_FORWARD_E;

        /*
            1.6. Call with out of range inlifType [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifIndex = 0;
    addrType   = CPSS_EXMXPM_INLIF_UNREG_NON_IP_MC_E;
    cmd        = CPSS_PACKET_CMD_FORWARD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifUnkUnregAddrCmdSet(dev, inlifType,  inlifIndex, addrType, cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifUntaggedMruIndexSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType,
    IN  GT_U32                      inlifIndex,
    IN  GT_U32                      mruIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifUntaggedMruIndexSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with  inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                    inlifIndex [63 / 4095 / 4096],
                    and mruIndex [0 / 7].
    Expected: GT_OK.
    1.2. Call with out of range mruIndex [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range inlifType [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    GT_U32                      inlifIndex = 0;
    GT_U32                      mruIndex   = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           and mruIndex [0 / 7].
            Expected: GT_OK.
        */
        /* iterate with inlifIndex = 63 */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;
        mruIndex   = 0;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, mruIndex);

        /* iterate with inlifIndex = 4095 */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;
        mruIndex   = 7;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, mruIndex);

        /* iterate with inlifIndex = 4096 */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;
        mruIndex   = 7;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, mruIndex);

        /*
            1.2. Call with out of range mruIndex [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        mruIndex = 8;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, mruIndex = %d" ,dev, mruIndex);

        mruIndex = 0;

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.6. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4096;

        /*
            1.7. Call with out of range inlifType [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex = 64;
    mruIndex   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifIpUcRouteEnableSet
(
    IN  GT_U8                          devNum,
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT     inlifType,
    IN  GT_U32                         inlifIndex,
    IN  CPSS_IP_PROTOCOL_STACK_ENT     ipVer,
    IN  GT_BOOL                        enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifIpUcRouteEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   inlifIndex [63 / 4095 / 4096],
                   ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                          CPSS_IP_PROTOCOL_IPV6_E /
                          CPSS_IP_PROTOCOL_IPV4V6_E]
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with out of range ipVer [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range inlifType [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      inlifIndex = 0;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    CPSS_IP_PROTOCOL_STACK_ENT  ipVer      = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                     enable     = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E /
                                  CPSS_IP_PROTOCOL_IPV4V6_E]
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */
        inlifType   = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex  = 63;
        ipVer       = CPSS_IP_PROTOCOL_IPV4_E;
        enable      = GT_FALSE;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, inlifIndex, ipVer, enable);

        inlifType   = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex  = 4095;
        ipVer       = CPSS_IP_PROTOCOL_IPV6_E;
        enable      = GT_TRUE;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, inlifIndex, ipVer, enable);

        inlifType   = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex  = 4096;
        ipVer       = CPSS_IP_PROTOCOL_IPV4V6_E;
        enable      = GT_TRUE;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, inlifIndex, ipVer, enable);

        inlifType   = CPSS_EXMXPM_INLIF_TYPE_PORT_E;

        /*
            1.2. Call with out of range ipVer [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ipVer = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ipVer = %d", dev, ipVer);

        ipVer = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.6. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4096;

        /*
            1.7. Call with out of range inlifType [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType   = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex  = 0;
    ipVer       = CPSS_IP_PROTOCOL_IPV4_E;
    enable      = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifIpMcRouteEnableSet
(
    IN  GT_U8                          devNum,
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT     inlifType,
    IN  GT_U32                         inlifIndex,
    IN  CPSS_IP_PROTOCOL_STACK_ENT     ipVer,
    IN  GT_BOOL                        enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifIpMcRouteEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   inlifIndex [63 / 4095 / 4096],
                   ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                          CPSS_IP_PROTOCOL_IPV6_E /
                          CPSS_IP_PROTOCOL_IPV4V6_E]
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with out of range ipVer [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range inlifType [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      inlifIndex = 0;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    CPSS_IP_PROTOCOL_STACK_ENT  ipVer      = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                     enable     = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E /
                                  CPSS_IP_PROTOCOL_IPV4V6_E]
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */
        inlifType   = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex  = 63;
        ipVer       = CPSS_IP_PROTOCOL_IPV4_E;
        enable      = GT_FALSE;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, inlifIndex, ipVer, enable);

        inlifType   = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex  = 4095;
        ipVer       = CPSS_IP_PROTOCOL_IPV6_E;
        enable      = GT_TRUE;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, inlifIndex, ipVer, enable);

        inlifType   = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex  = 4096;
        ipVer       = CPSS_IP_PROTOCOL_IPV4V6_E;
        enable      = GT_TRUE;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, inlifIndex, ipVer, enable);

        inlifType   = CPSS_EXMXPM_INLIF_TYPE_PORT_E;

        /*
            1.2. Call with out of range ipVer [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ipVer = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ipVer = %d", dev, ipVer);

        ipVer = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.6. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4096;

        /*
            1.7. Call with out of range inlifType [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType   = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex  = 0;
    ipVer       = CPSS_IP_PROTOCOL_IPV4_E;
    enable      = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifMplsRouteEnableSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType,
    IN  GT_U32                      inlifIndex,
    IN  GT_BOOL                     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifMplsRouteEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   inlifIndex [63 / 4095 / 4096],
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range inlifType [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      inlifIndex = 0;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    GT_BOOL                     enable     = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with inlifIndex [0] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;
        enable     = GT_FALSE;

        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4095] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4096] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4096;

        /*
            1.6. Call with out of range inlifType [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex = 63;
    enable     = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifVrfIdSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType,
    IN  GT_U32                      inlifIndex,
    IN  GT_U32                      vrfId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifVrfIdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with  inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                    inlifIndex [63 / 4095 / 4096],
                    and vrfId [0 / 4095].
    Expected: GT_OK.
    1.2. Call with out of range vrfId [4096]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range inlifType [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    GT_U32                      inlifIndex = 0;
    GT_U32                      vrfId      = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           and vrfId [0 / 4095].
            Expected: GT_OK.
        */
        /* iterate with inlifIndex = 63 */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;
        vrfId      = 0;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, vrfId);

        /* iterate with inlifIndex = 4095 */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;
        vrfId      = 7;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, vrfId);

        /* iterate with inlifIndex = 4096 */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;
        vrfId      = 7;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, vrfId);

        /*
            1.2. Call with out of range vrfId [4096]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        vrfId = 4096;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, mruIndex = %d" ,dev, vrfId);

        vrfId = 0;

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.6. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4096;

        /*
            1.7. Call with out of range inlifType [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex = 63;
    vrfId      = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifBridgeRouterInterfaceEnabledSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType,
    IN  GT_U32                      inlifIndex,
    IN  GT_BOOL                     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifBridgeRouterInterfaceEnabledSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   inlifIndex [63 / 4095 / 4096],
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range inlifType [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      inlifIndex = 0;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    GT_BOOL                     enable     = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with inlifIndex [0] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;
        enable     = GT_FALSE;

        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4095] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4096] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4096;

        /*
            1.6. Call with out of range inlifType [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex = 63;
    enable     = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifIpSecurityProfileSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType,
    IN  GT_U32                      inlifIndex,
    IN  GT_U32                      profileIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifIpSecurityProfileSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with  inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                    inlifIndex [63 / 4095 / 4096],
                    and profileIndex [0 / 15].
    Expected: GT_OK.
    1.2. Call with out of range profileIndex [16]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range inlifType [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    GT_U32                      inlifIndex = 0;
    GT_U32                      profileIndex = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           and profileIndex [0 / 15].
            Expected: GT_OK.
        */
        /* iterate with inlifIndex = 63 */
        inlifType    = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex   = 63;
        profileIndex = 0;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, profileIndex);

        /* iterate with inlifIndex = 4095 */
        inlifType    = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex   = 4095;
        profileIndex = 15;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, profileIndex);

        /* iterate with inlifIndex = 4096 */
        inlifType    = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex   = 4096;
        profileIndex = 15;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, profileIndex);

        /*
            1.2. Call with out of range profileIndex [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        profileIndex = 16;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, mruIndex = %d" ,dev, profileIndex);

        profileIndex = 0;

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.6. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4096;

        /*
            1.7. Call with out of range inlifType [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType    = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex   = 63;
    profileIndex = 15;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifControlTrafficEnableSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT          inlifType,
    IN  GT_U32                              inlifIndex,
    IN  CPSS_EXMXPM_INLIF_CTRL_TRAFFIC_ENT  controlTraffic,
    IN  GT_BOOL                             enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifControlTrafficEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   inlifIndex [63 / 4095 / 4096],
                   controlTraffic [ CPSS_EXMXPM_INLIF_CTRL_ARP_BC_E /
                                    CPSS_EXMXPM_INLIF_CTRL_ARP_TO_ME_E /
                                    CPSS_EXMXPM_INLIF_CTRL_IP_V4_IGMP_TO_CPU_E /
                                    CPSS_EXMXPM_INLIF_CTRL_IP_V6_ICMP_TO_CPU_E /
                                    CPSS_EXMXPM_INLIF_CTRL_IP_V4_ICMP_REDIRECT_E /
                                    CPSS_EXMXPM_INLIF_CTRL_IP_V6_ICMP_REDIRECT_E /
                                    CPSS_EXMXPM_INLIF_CTRL_IP_V6_NBR_SOLC_E /
                                    CPSS_EXMXPM_INLIF_CTRL_IP_V4_LL_MC_E /
                                    CPSS_EXMXPM_INLIF_CTRL_IP_V6_LL_MC_E /
                                    CPSS_EXMXPM_INLIF_CTRL_UDP_BC_E /
                                    CPSS_EXMXPM_INLIF_CTRL_RIP_V1_E ],
                   enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call with out of range controlTraffic [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_INLIF_TYPE_ENT          inlifType      = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    GT_U32                              inlifIndex     = 0;
    CPSS_EXMXPM_INLIF_CTRL_TRAFFIC_ENT  controlTraffic = CPSS_EXMXPM_INLIF_CTRL_ARP_TO_ME_E;
    GT_BOOL                             enable         = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           controlTraffic [ CPSS_EXMXPM_INLIF_CTRL_ARP_BC_E /
                                            CPSS_EXMXPM_INLIF_CTRL_ARP_TO_ME_E /
                                            CPSS_EXMXPM_INLIF_CTRL_IP_V4_IGMP_TO_CPU_E /
                                            CPSS_EXMXPM_INLIF_CTRL_IP_V6_ICMP_TO_CPU_E /
                                            CPSS_EXMXPM_INLIF_CTRL_IP_V4_ICMP_REDIRECT_E /
                                            CPSS_EXMXPM_INLIF_CTRL_IP_V6_ICMP_REDIRECT_E /
                                            CPSS_EXMXPM_INLIF_CTRL_IP_V6_NBR_SOLC_E /
                                            CPSS_EXMXPM_INLIF_CTRL_IP_V4_LL_MC_E /
                                            CPSS_EXMXPM_INLIF_CTRL_IP_V6_LL_MC_E /
                                            CPSS_EXMXPM_INLIF_CTRL_UDP_BC_E /
                                            CPSS_EXMXPM_INLIF_CTRL_RIP_V1_E ],
                           enable[GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* iterate with controlTraffic = CPSS_EXMXPM_INLIF_CTRL_ARP_BC_E */
        inlifType      = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex     = 63;
        controlTraffic = CPSS_EXMXPM_INLIF_CTRL_ARP_BC_E;
        enable         = GT_FALSE;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, controlTraffic);

        /* iterate with controlTraffic = CPSS_EXMXPM_INLIF_CTRL_ARP_TO_ME_E */
        inlifType      = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex     = 4095;
        controlTraffic = CPSS_EXMXPM_INLIF_CTRL_ARP_TO_ME_E;
        enable         = GT_TRUE;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, controlTraffic);

        /* iterate with controlTraffic = CPSS_EXMXPM_INLIF_CTRL_IP_V4_IGMP_TO_CPU_E */
        inlifType      = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex     = 4096;
        controlTraffic = CPSS_EXMXPM_INLIF_CTRL_IP_V4_IGMP_TO_CPU_E;
        enable         = GT_TRUE;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, controlTraffic);

        /* iterate with controlTraffic = CPSS_EXMXPM_INLIF_CTRL_IP_V6_ICMP_TO_CPU_E */
        inlifType      = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex     = 4096;
        controlTraffic = CPSS_EXMXPM_INLIF_CTRL_IP_V6_ICMP_TO_CPU_E;
        enable         = GT_TRUE;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, controlTraffic);

        /* iterate with controlTraffic = CPSS_EXMXPM_INLIF_CTRL_IP_V4_ICMP_REDIRECT_E */
        inlifType      = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex     = 4096;
        controlTraffic = CPSS_EXMXPM_INLIF_CTRL_IP_V4_ICMP_REDIRECT_E;
        enable         = GT_TRUE;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, controlTraffic);

        /* iterate with controlTraffic = CPSS_EXMXPM_INLIF_CTRL_IP_V6_ICMP_REDIRECT_E */
        inlifType      = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex     = 4096;
        controlTraffic = CPSS_EXMXPM_INLIF_CTRL_IP_V6_ICMP_REDIRECT_E;
        enable         = GT_TRUE;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, controlTraffic);

        /* iterate with controlTraffic = CPSS_EXMXPM_INLIF_CTRL_IP_V6_NBR_SOLC_E */
        inlifType      = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex     = 4096;
        controlTraffic = CPSS_EXMXPM_INLIF_CTRL_IP_V6_NBR_SOLC_E;
        enable         = GT_TRUE;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, controlTraffic);

        /* iterate with controlTraffic = CPSS_EXMXPM_INLIF_CTRL_IP_V4_LL_MC_E */
        inlifType      = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex     = 4096;
        controlTraffic = CPSS_EXMXPM_INLIF_CTRL_IP_V4_LL_MC_E;
        enable         = GT_TRUE;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, controlTraffic);

        /* iterate with controlTraffic = CPSS_EXMXPM_INLIF_CTRL_IP_V6_LL_MC_E */
        inlifType      = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex     = 4096;
        controlTraffic = CPSS_EXMXPM_INLIF_CTRL_IP_V6_LL_MC_E;
        enable         = GT_TRUE;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, controlTraffic);

        /* iterate with controlTraffic = CPSS_EXMXPM_INLIF_CTRL_UDP_BC_E */
        inlifType      = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex     = 4096;
        controlTraffic = CPSS_EXMXPM_INLIF_CTRL_UDP_BC_E;
        enable         = GT_TRUE;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, controlTraffic);

        /* iterate with controlTraffic = CPSS_EXMXPM_INLIF_CTRL_RIP_V1_E */
        inlifType      = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex     = 4096;
        controlTraffic = CPSS_EXMXPM_INLIF_CTRL_RIP_V1_E;
        enable         = GT_TRUE;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifIndex, controlTraffic);

        /*
            1.2. Call with out of range controlTraffic [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        controlTraffic = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, controlTraffic = %d", dev, controlTraffic);

        controlTraffic = INLIF_INVALID_ENUM_CNS;

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.6. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4096;

        /*
            1.7. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType      = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex     = 0;
    controlTraffic = CPSS_EXMXPM_INLIF_CTRL_ARP_TO_ME_E;
    enable         = GT_FALSE;


    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifControlTrafficEnableSet(dev, inlifType, inlifIndex, controlTraffic, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifMirrorToAnalyzerEnableSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType,
    IN  GT_U32                      inlifIndex,
    IN  GT_BOOL                     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifMirrorToAnalyzerEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   inlifIndex [63 / 4095 / 4096],
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      inlifIndex = 0;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    GT_BOOL                     enable     = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with inlifIndex [0] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;
        enable     = GT_FALSE;

        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4095] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4096] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4099;

        /*
            1.6. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex = 63;
    enable     = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmInlifMirrorToCpuEnableSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType,
    IN  GT_U32                      inlifIndex,
    IN  GT_BOOL                     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifMirrorToCpuEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   inlifIndex [63 / 4095 / 4096],
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                   out of range inlifIndex [64],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                   out of range inlifIndex [4096],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [65536],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                   out of range inlifIndex [4095],
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      inlifIndex = 0;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    GT_BOOL                     enable     = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           inlifIndex [63 / 4095 / 4096],
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with inlifIndex [0] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;
        enable     = GT_FALSE;

        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4095] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with inlifIndex [4096] */
        inlifType  = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;
        enable     = GT_TRUE;

        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                           out of range inlifIndex [64],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 64;

        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.3. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                           out of range inlifIndex [4096],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.4. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [65536],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 65536;

        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        /*
            1.5. Call with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                           out of range inlifIndex [4095],
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        inlifIndex = 4095;

        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, inlifType, inlifIndex);

        inlifIndex = 4096;

        /*
            1.6. Call with inlifType [INLIF_INVALID_ENUM_CNS],
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        inlifType = INLIF_INVALID_ENUM_CNS;

        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, inlifType);
    }

    inlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    inlifIndex = 63;
    enable     = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Inlif PORT table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifFillInlifPortTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in Inlif PORT table.
         Call cpssExMxPmInlifEntrySet with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E],
                                               inlifIndex [0..numEntries - 1],
                                               inlifEntryPtr-> {  bridgeEnable [GT_TRUE],
                                                                  autoLearnEnable [GT_TRUE],
                                                                  naMessageToCpuEnable [GT_TRUE],
                                                                  naStormPreventionEnable [GT_TRUE],
                                                                  unkSaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                                                  unkDaUcCommand[CPSS_PACKET_CMD_DROP_SOFT_E],
                                                                  unkSaNotSecurBreachEnable [GT_TRUE],
                                                                  untaggedMruIndex[0],
                                                                  unregNonIpMcCommand [CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                                                                  unregIpMcCommand [CPSS_PACKET_CMD_DROP_HARD_E],
                                                                  unregIpv4BcCommand [CPSS_PACKET_CMD_DROP_SOFT_E],
                                                                  unregNonIpv4BcCommand[CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                                                  ipv4UcRouteEnable [GT_TRUE],
                                                                  ipv4McRouteEnable [GT_TRUE],
                                                                  ipv6UcRouteEnable [GT_TRUE],
                                                                  ipv6McRouteEnable [GT_TRUE],
                                                                  mplsRouteEnable [GT_TRUE],
                                                                  vrfId [0],
                                                                  ipv4IcmpRedirectEnable [GT_TRUE],
                                                                  ipv6IcmpRedirectEnable [GT_TRUE],
                                                                  bridgeRouterInterfaceEnable [GT_TRUE],
                                                                  ipSecurityProfile [0],
                                                                  ipv4IgmpToCpuEnable [GT_TRUE],
                                                                  ipv6IgmpToCpuEnable [GT_TRUE],
                                                                  udpBcRelayEnable [GT_TRUE],
                                                                  arpBcToCpuEnable [GT_TRUE],
                                                                  arpBcToMeEnable [GT_TRUE],
                                                                  ripv1MirrorEnable [GT_TRUE],
                                                                  ipv4LinkLocalMcCommandEnable [GT_TRUE],
                                                                  ipv6LinkLocalMcCommandEnable [GT_TRUE],
                                                                  ipv6NeighborSolicitationEnable [GT_TRUE],;
                                                                  mirrorToAnalyzerEnable [GT_TRUE],
                                                                  mirrorToCpuEnable [GT_TRUE],
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmInlifEntrySet with inlifIndex [numEntries], and other params from 1.2.
    Expected: NOT GT_OK.
    1.3. Read all entries in Inlif PORT table and compare with original.
         Call cpssExMxPmInlifEntryGet with non-NULL inlifEntryPtr and the same inlifIndex as in 1.1.
    Expected: GT_OK and the same inlifEntryPtr.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmInlifEntryGet with inlifIndex [numEntries],
                                            and other params from 1.2.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;

    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntry;
    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntryGet;


    cpssOsBzero((GT_VOID*) &inlifEntry, sizeof(inlifEntry));
    cpssOsBzero((GT_VOID*) &inlifEntryGet, sizeof(inlifEntryGet));

    /* Fill the entry for Inlif PORT table */
    inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;

    inlifEntry.bridgeEnable = GT_TRUE;
    inlifEntry.autoLearnEnable = GT_TRUE;
    inlifEntry.naMessageToCpuEnable = GT_TRUE;
    inlifEntry.naStormPreventionEnable = GT_TRUE;
    inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_DROP_HARD_E;
    inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
    inlifEntry.unkSaNotSecurBreachEnable = GT_TRUE;
    inlifEntry.untaggedMruIndex = 0;
    inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_DROP_HARD_E;
    inlifEntry.unregIpv4BcCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
    inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
    inlifEntry.ipv4UcRouteEnable = GT_TRUE;
    inlifEntry.ipv4McRouteEnable = GT_TRUE;
    inlifEntry.ipv6UcRouteEnable = GT_TRUE;
    inlifEntry.ipv6McRouteEnable = GT_TRUE;
    inlifEntry.mplsRouteEnable = GT_TRUE;
    inlifEntry.vrfId = 0;
    inlifEntry.ipv4IcmpRedirectEnable = GT_TRUE;
    inlifEntry.ipv6IcmpRedirectEnable = GT_TRUE;
    inlifEntry.bridgeRouterInterfaceEnable = GT_TRUE;
    inlifEntry.ipSecurityProfile = 0;
    inlifEntry.ipv4IgmpToCpuEnable = GT_TRUE;
    inlifEntry.ipv6IcmpToCpuEnable = GT_TRUE;
    inlifEntry.udpBcRelayEnable = GT_TRUE;
    inlifEntry.arpBcToCpuEnable = GT_TRUE;
    inlifEntry.arpBcToMeEnable = GT_TRUE;
    inlifEntry.ripv1MirrorEnable = GT_TRUE;
    inlifEntry.ipv4LinkLocalMcCommandEnable = GT_TRUE;
    inlifEntry.ipv6LinkLocalMcCommandEnable = GT_TRUE;
    inlifEntry.ipv6NeighborSolicitationEnable = GT_TRUE;;
    inlifEntry.mirrorToAnalyzerEnable = GT_TRUE;
    inlifEntry.mirrorToCpuEnable = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);


    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 64;

        /* 1.2. Fill all entries in Inlif PORT table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            inlifEntry.vrfId = iTemp;
            inlifEntry.untaggedMruIndex = iTemp % 7;

            st = cpssExMxPmInlifEntrySet(dev, inlifType, iTemp, &inlifEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntrySet: %d, %d, %d", dev, inlifType, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmInlifEntrySet(dev, inlifType, numEntries, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntrySet: %d, %d, %d", dev, inlifType, iTemp);

        /* 1.4. Read all entries in Inlif PORT table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            inlifEntry.vrfId = iTemp;
            inlifEntry.untaggedMruIndex = iTemp % 7;

            st = cpssExMxPmInlifEntryGet(dev, inlifType, iTemp, &inlifEntryGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d, %d", dev, inlifType, iTemp);

            if (GT_OK == st)
            {
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &inlifEntry, (GT_VOID*) &inlifEntryGet, sizeof(inlifEntry) )) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another inlifEntry than was set: %d", dev);
            }
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmInlifEntryGet(dev, inlifType, numEntries, &inlifEntryGet);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d, %d", dev, inlifType, iTemp);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Inlif Vlan table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifFillInlifVlanTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in Inlif table.
         Call cpssExMxPmInlifEntrySet with inlifType [CPSS_EXMXPM_INLIF_TYPE_VLAN_E],
                                               inlifIndex [0..numEntries - 1],
                                               inlifEntryPtr-> {  bridgeEnable [GT_TRUE],
                                                                  autoLearnEnable [GT_TRUE],
                                                                  naMessageToCpuEnable [GT_TRUE],
                                                                  naStormPreventionEnable [GT_TRUE],
                                                                  unkSaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                                                  unkDaUcCommand[CPSS_PACKET_CMD_DROP_SOFT_E],
                                                                  unkSaNotSecurBreachEnable [GT_TRUE],
                                                                  untaggedMruIndex[0],
                                                                  unregNonIpMcCommand [CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                                                                  unregIpMcCommand [CPSS_PACKET_CMD_DROP_HARD_E],
                                                                  unregIpv4BcCommand [CPSS_PACKET_CMD_DROP_SOFT_E],
                                                                  unregNonIpv4BcCommand[CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                                                  ipv4UcRouteEnable [GT_TRUE],
                                                                  ipv4McRouteEnable [GT_TRUE],
                                                                  ipv6UcRouteEnable [GT_TRUE],
                                                                  ipv6McRouteEnable [GT_TRUE],
                                                                  mplsRouteEnable [GT_TRUE],
                                                                  vrfId [0],
                                                                  ipv4IcmpRedirectEnable [GT_TRUE],
                                                                  ipv6IcmpRedirectEnable [GT_TRUE],
                                                                  bridgeRouterInterfaceEnable [GT_TRUE],
                                                                  ipSecurityProfile [0],
                                                                  ipv4IgmpToCpuEnable [GT_TRUE],
                                                                  ipv6IgmpToCpuEnable [GT_TRUE],
                                                                  udpBcRelayEnable [GT_TRUE],
                                                                  arpBcToCpuEnable [GT_TRUE],
                                                                  arpBcToMeEnable [GT_TRUE],
                                                                  ripv1MirrorEnable [GT_TRUE],
                                                                  ipv4LinkLocalMcCommandEnable [GT_TRUE],
                                                                  ipv6LinkLocalMcCommandEnable [GT_TRUE],
                                                                  ipv6NeighborSolicitationEnable [GT_TRUE],;
                                                                  mirrorToAnalyzerEnable [GT_TRUE],
                                                                  mirrorToCpuEnable [GT_TRUE],
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmInlifEntrySet with inlifIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in Inlif table and compare with original.
         Call cpssExMxPmInlifEntryGet with non-NULL inlifEntryPtr and the same inlifIndex as in 1.2.
    Expected: GT_OK and the same inlifEntryPtr.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmInlifEntryGet with inlifIndex [numEntries].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;

    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntry;
    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntryGet;


    cpssOsBzero((GT_VOID*) &inlifEntry, sizeof(inlifEntry));
    cpssOsBzero((GT_VOID*) &inlifEntryGet, sizeof(inlifEntryGet));

    /* Fill the entry for Inlif Vlan table */
    inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;

    inlifEntry.bridgeEnable = GT_TRUE;
    inlifEntry.autoLearnEnable = GT_TRUE;
    inlifEntry.naMessageToCpuEnable = GT_TRUE;
    inlifEntry.naStormPreventionEnable = GT_TRUE;
    inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_DROP_HARD_E;
    inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
    inlifEntry.unkSaNotSecurBreachEnable = GT_TRUE;
    inlifEntry.untaggedMruIndex = 0;
    inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_DROP_HARD_E;
    inlifEntry.unregIpv4BcCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
    inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
    inlifEntry.ipv4UcRouteEnable = GT_TRUE;
    inlifEntry.ipv4McRouteEnable = GT_TRUE;
    inlifEntry.ipv6UcRouteEnable = GT_TRUE;
    inlifEntry.ipv6McRouteEnable = GT_TRUE;
    inlifEntry.mplsRouteEnable = GT_TRUE;
    inlifEntry.vrfId = 0;
    inlifEntry.ipv4IcmpRedirectEnable = GT_TRUE;
    inlifEntry.ipv6IcmpRedirectEnable = GT_TRUE;
    inlifEntry.bridgeRouterInterfaceEnable = GT_TRUE;
    inlifEntry.ipSecurityProfile = 0;
    inlifEntry.ipv4IgmpToCpuEnable = GT_TRUE;
    inlifEntry.ipv6IcmpToCpuEnable = GT_TRUE;
    inlifEntry.udpBcRelayEnable = GT_TRUE;
    inlifEntry.arpBcToCpuEnable = GT_TRUE;
    inlifEntry.arpBcToMeEnable = GT_TRUE;
    inlifEntry.ripv1MirrorEnable = GT_TRUE;
    inlifEntry.ipv4LinkLocalMcCommandEnable = GT_TRUE;
    inlifEntry.ipv6LinkLocalMcCommandEnable = GT_TRUE;
    inlifEntry.ipv6NeighborSolicitationEnable = GT_TRUE;;
    inlifEntry.mirrorToAnalyzerEnable = GT_TRUE;
    inlifEntry.mirrorToCpuEnable = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = PRV_CPSS_MAX_NUM_VLANS_CNS;

        /* 1.2. Fill all entries in Inlif Vlan table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            inlifEntry.untaggedMruIndex = iTemp % 7;
            inlifEntry.vrfId            = iTemp;

            st = cpssExMxPmInlifEntrySet(dev, inlifType, iTemp, &inlifEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntrySet: %d, %d, %d", dev, inlifType, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmInlifEntrySet(dev, inlifType, numEntries, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntrySet: %d, %d, %d", dev, inlifType, iTemp);

        /* 1.4. Read all entries in Inlif Vlan table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            inlifEntry.untaggedMruIndex = iTemp % 7;
            inlifEntry.vrfId            = iTemp;

            st = cpssExMxPmInlifEntryGet(dev, inlifType, iTemp, &inlifEntryGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d, %d", dev, inlifType, iTemp);

            if (GT_OK == st)
            {
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &inlifEntry, (GT_VOID*) &inlifEntryGet, sizeof(inlifEntry) )) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another inlifEntry than was set: %d", dev);
            }
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmInlifEntryGet(dev, inlifType, numEntries, &inlifEntryGet);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d, %d", dev, inlifType, iTemp);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Inlif EXTERNAL table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifFillInlifExternalTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in Inlif table.
         Call cpssExMxPmInlifEntrySet with inlifType [CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                                               inlifIndex [4096..numEntries - 1],
                                               inlifEntryPtr-> {  bridgeEnable [GT_TRUE],
                                                                  autoLearnEnable [GT_TRUE],
                                                                  naMessageToCpuEnable [GT_TRUE],
                                                                  naStormPreventionEnable [GT_TRUE],
                                                                  unkSaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                                                  unkDaUcCommand[CPSS_PACKET_CMD_DROP_SOFT_E],
                                                                  unkSaNotSecurBreachEnable [GT_TRUE],
                                                                  untaggedMruIndex[0],
                                                                  unregNonIpMcCommand [CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                                                                  unregIpMcCommand [CPSS_PACKET_CMD_DROP_HARD_E],
                                                                  unregIpv4BcCommand [CPSS_PACKET_CMD_DROP_SOFT_E],
                                                                  unregNonIpv4BcCommand[CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                                                  ipv4UcRouteEnable [GT_TRUE],
                                                                  ipv4McRouteEnable [GT_TRUE],
                                                                  ipv6UcRouteEnable [GT_TRUE],
                                                                  ipv6McRouteEnable [GT_TRUE],
                                                                  mplsRouteEnable [GT_TRUE],
                                                                  vrfId [0],
                                                                  ipv4IcmpRedirectEnable [GT_TRUE],
                                                                  ipv6IcmpRedirectEnable [GT_TRUE],
                                                                  bridgeRouterInterfaceEnable [GT_TRUE],
                                                                  ipSecurityProfile [0],
                                                                  ipv4IgmpToCpuEnable [GT_TRUE],
                                                                  ipv6IgmpToCpuEnable [GT_TRUE],
                                                                  udpBcRelayEnable [GT_TRUE],
                                                                  arpBcToCpuEnable [GT_TRUE],
                                                                  arpBcToMeEnable [GT_TRUE],
                                                                  ripv1MirrorEnable [GT_TRUE],
                                                                  ipv4LinkLocalMcCommandEnable [GT_TRUE],
                                                                  ipv6LinkLocalMcCommandEnable [GT_TRUE],
                                                                  ipv6NeighborSolicitationEnable [GT_TRUE],;
                                                                  mirrorToAnalyzerEnable [GT_TRUE],
                                                                  mirrorToCpuEnable [GT_TRUE],
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmInlifEntrySet with inlifIndex [4095 / numEntries],
                                        and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in Inlif table and compare with original.
         Call cpssExMxPmInlifEntryGet with non-NULL inlifEntryPtr and the same inlifIndex as in 1.1.
    Expected: GT_OK and the same inlifEntryPtr.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmInlifEntryGet with inlifIndex [4095 / numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;

    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntry;
    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntryGet;


    cpssOsBzero((GT_VOID*) &inlifEntry, sizeof(inlifEntry));
    cpssOsBzero((GT_VOID*) &inlifEntryGet, sizeof(inlifEntryGet));

    /* Fill the entry for Inlif EXTERNAL table */
    inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;

    inlifEntry.bridgeEnable = GT_TRUE;
    inlifEntry.autoLearnEnable = GT_TRUE;
    inlifEntry.naMessageToCpuEnable = GT_TRUE;
    inlifEntry.naStormPreventionEnable = GT_TRUE;
    inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_DROP_HARD_E;
    inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
    inlifEntry.unkSaNotSecurBreachEnable = GT_TRUE;
    inlifEntry.untaggedMruIndex = 0;
    inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_DROP_HARD_E;
    inlifEntry.unregIpv4BcCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
    inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
    inlifEntry.ipv4UcRouteEnable = GT_TRUE;
    inlifEntry.ipv4McRouteEnable = GT_TRUE;
    inlifEntry.ipv6UcRouteEnable = GT_TRUE;
    inlifEntry.ipv6McRouteEnable = GT_TRUE;
    inlifEntry.mplsRouteEnable = GT_TRUE;
    inlifEntry.vrfId = 0;
    inlifEntry.ipv4IcmpRedirectEnable = GT_TRUE;
    inlifEntry.ipv6IcmpRedirectEnable = GT_TRUE;
    inlifEntry.bridgeRouterInterfaceEnable = GT_TRUE;
    inlifEntry.ipSecurityProfile = 0;
    inlifEntry.ipv4IgmpToCpuEnable = GT_TRUE;
    inlifEntry.ipv6IcmpToCpuEnable = GT_TRUE;
    inlifEntry.udpBcRelayEnable = GT_TRUE;
    inlifEntry.arpBcToCpuEnable = GT_TRUE;
    inlifEntry.arpBcToMeEnable = GT_TRUE;
    inlifEntry.ripv1MirrorEnable = GT_TRUE;
    inlifEntry.ipv4LinkLocalMcCommandEnable = GT_TRUE;
    inlifEntry.ipv6LinkLocalMcCommandEnable = GT_TRUE;
    inlifEntry.ipv6NeighborSolicitationEnable = GT_TRUE;;
    inlifEntry.mirrorToAnalyzerEnable = GT_TRUE;
    inlifEntry.mirrorToCpuEnable = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 65536;

        /* 1.2. Fill all entries in Inlif EXTERNAL table */
        for(iTemp = 4096; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            inlifEntry.untaggedMruIndex = iTemp % 7;
            inlifEntry.vrfId            = iTemp % 4095;

            st = cpssExMxPmInlifEntrySet(dev, inlifType, iTemp, &inlifEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntrySet: %d, %d, %d", dev, inlifType, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        iTemp = 4095;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, iTemp, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntrySet: %d, %d, %d", dev, inlifType, iTemp);

        st = cpssExMxPmInlifEntrySet(dev, inlifType, numEntries, &inlifEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntrySet: %d, %d, %d", dev, inlifType, iTemp);

        /* 1.4. Read all entries in Inlif EXTERNAL table and compare with original */
        for(iTemp = 4096; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            inlifEntry.untaggedMruIndex = iTemp % 7;
            inlifEntry.vrfId            = iTemp % 4095;

            st = cpssExMxPmInlifEntryGet(dev, inlifType, iTemp, &inlifEntryGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d, %d", dev, inlifType, iTemp);

            if (GT_OK == st)
            {
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &inlifEntry, (GT_VOID*) &inlifEntryGet, sizeof(inlifEntry) )) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another inlifEntry than was set: %d", dev);
            }
        }

        /* 1.5. Try to read entry with index out of range. */
        iTemp = 4095;

        st = cpssExMxPmInlifEntryGet(dev, inlifType, iTemp, &inlifEntryGet);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d, %d", dev, inlifType, iTemp);

        st = cpssExMxPmInlifEntryGet(dev, inlifType, numEntries, &inlifEntryGet);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d, %d", dev, inlifType, iTemp);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test for Functionality
*/
UTF_TEST_CASE_MAC(cpssExMxPmInlifFunctionality)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmInlifEntrySet with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                                           inlifIndex [63 / 4095 / 4096],
                                           inlifEntryPtr-> {  bridgeEnable [GT_FALSE],
                                                              autoLearnEnable [GT_FALSE],
                                                              naMessageToCpuEnable [GT_FALSE],
                                                              naStormPreventionEnable [GT_FALSE],
                                                              unkSaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                                              unkDaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                                              unkSaNotSecurBreachEnable [GT_FALSE],
                                                              untaggedMruIndex[0],
                                                              unregNonIpMcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                              unregIpMcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                              unregIpv4BcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                              unregNonIpv4BcCommand[CPSS_PACKET_CMD_FORWARD_E],
                                                              ipv4UcRouteEnable [GT_FALSE],
                                                              ipv4McRouteEnable [GT_FALSE],
                                                              ipv6UcRouteEnable [GT_FALSE],
                                                              ipv6McRouteEnable [GT_FALSE],
                                                              mplsRouteEnable [GT_FALSE],
                                                              vrfId [0],
                                                              ipv4IcmpRedirectEnable [GT_FALSE],
                                                              ipv6IcmpRedirectEnable [GT_FALSE],
                                                              bridgeRouterInterfaceEnable [GT_FALSE],
                                                              ipSecurityProfile [0],
                                                              ipv4IgmpToCpuEnable [GT_FALSE],
                                                              ipv6IgmpToCpuEnable [GT_FALSE],
                                                              udpBcRelayEnable [GT_FALSE],
                                                              arpBcToCpuEnable [GT_FALSE],
                                                              arpBcToMeEnable [GT_FALSE],
                                                              ripv1MirrorEnable [GT_FALSE],
                                                              ipv4LinkLocalMcCommandEnable [GT_FALSE],
                                                              ipv6LinkLocalMcCommandEnable [GT_FALSE],
                                                              ipv6NeighborSolicitationEnable [GT_FALSE];
                                                              mirrorToAnalyzerEnable [GT_FALSE],
                                                              mirrorToCpuEnable [GT_FALSE] }
    Expected: GT_OK.
    1.2. Call cpssExMxPmInlifBridgeEnableSet with inlifType, inlifIndex from 1.1.
                                             and enable [GT_TRUE].
    Expected: GT_OK.
    1.3. Call cpssExMxPmInlifUnkSaNotSecurBreachEnableSet with inlifType, inlifIndex from 1.1.
                                                          and enable [GT_TRUE].
    Expected: GT_OK.
    1.4. Call cpssExMxPmInlifNaStormPreventionEnableSet with inlifType, inlifIndex from 1.1.
                                                        and enable [GT_TRUE].
    Expected: GT_OK.
    1.5. Call cpssExMxPmInlifAutoLearnEnableSet with inlifType, inlifIndex from 1.1.
                                                and enable [GT_TRUE].
    Expected: GT_OK.
    1.6. Call cpssExMxPmInlifNaToCpuEnableSet with inlifType, inlifIndex from 1.1.
                                              and enable [GT_TRUE].
    Expected: GT_OK.
    1.7. Call cpssExMxPmInlifUntaggedMruIndexSet with inlifType, inlifIndex from 1.1.
                                                 and mruIndex [7].
    Expected: GT_OK.
    1.8. Call cpssExMxPmInlifIpUcRouteEnableSet with inlifType, inlifIndex from 1.1.
                                                ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                                                       CPSS_IP_PROTOCOL_IPV6_E],
                                                and enable [GT_TRUE].
    Expected: GT_OK.
    1.9. Call cpssExMxPmInlifIpMcRouteEnableSet with inlifType, inlifIndex from 1.1.
                                                ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                                                       CPSS_IP_PROTOCOL_IPV6_E],
                                                and enable [GT_TRUE].
    Expected: GT_OK.
    1.10. Call cpssExMxPmInlifMplsRouteEnableSet with inlifType, inlifIndex from 1.1.
                                                 and enable [GT_TRUE].
    Expected: GT_OK.
    1.11. Call cpssExMxPmInlifVrfIdSet with inlifType, inlifIndex from 1.1.
                                       and vrfId [100].
    Expected: GT_OK.
    1.12. Call cpssExMxPmInlifBridgeRouterInterfaceEnabledSet with inlifType, inlifIndex from 1.1.
                                                              and enable [GT_TRUE].
    Expected: GT_OK.
    1.13. Call cpssExMxPmInlifIpSecurityProfileSet with inlifType, inlifIndex from 1.1.
                                                   and profileIndex [15].
    Expected: GT_OK.
    1.14. Call cpssExMxPmInlifMirrorToAnalyzerEnableSet with inlifType, inlifIndex from 1.1.
                                                        and enable [GT_TRUE].
    Expected: GT_OK.
    1.15. Call cpssExMxPmInlifMirrorToCpuEnableSet with inlifType, inlifIndex from 1.1.
                                                   and enable [GT_TRUE].
    Expected: GT_OK.
    1.16. Call cpssExMxPmInlifEntryGet with inlifType, inlifIndex from 1.1.
    Expected: GT_OK and inlifEntryPtr-> {   bridgeEnable [GT_TRUE],
                                            autoLearnEnable [GT_TRUE],
                                            naMessageToCpuEnable [GT_TRUE],
                                            naStormPreventionEnable [GT_TRUE],
                                            unkSaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                            unkDaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                            unkSaNotSecurBreachEnable [GT_TRUE],
                                            untaggedMruIndex[7],
                                            unregNonIpMcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                            unregIpMcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                            unregIpv4BcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                            unregNonIpv4BcCommand[CPSS_PACKET_CMD_FORWARD_E],
                                            ipv4UcRouteEnable [GT_TRUE],
                                            ipv4McRouteEnable [GT_TRUE],
                                            ipv6UcRouteEnable [GT_FALSE],
                                            ipv6McRouteEnable [GT_FALSE],
                                            mplsRouteEnable [GT_TRUE],
                                            vrfId [100],
                                            ipv4IcmpRedirectEnable [GT_FALSE],
                                            ipv6IcmpRedirectEnable [GT_FALSE],
                                            bridgeRouterInterfaceEnable [GT_TRUE],
                                            ipSecurityProfile [15],
                                            ipv4IgmpToCpuEnable [GT_FALSE],
                                            ipv6IgmpToCpuEnable [GT_FALSE],
                                            udpBcRelayEnable [GT_FALSE],
                                            arpBcToCpuEnable [GT_FALSE],
                                            arpBcToMeEnable [GT_FALSE],
                                            ripv1MirrorEnable [GT_FALSE],
                                            ipv4LinkLocalMcCommandEnable [GT_FALSE],
                                            ipv6LinkLocalMcCommandEnable [GT_FALSE],
                                            ipv6NeighborSolicitationEnable [GT_FALSE];
                                            mirrorToAnalyzerEnable [GT_TRUE],
                                            mirrorToCpuEnable [GT_TRUE] }

    1.17. Call cpssExMxPmInlifBridgeEnableSet with inlifType, inlifIndex from 1.1.
                                             and enable [GT_FALSE].
    Expected: GT_OK.
    1.18. Call cpssExMxPmInlifUnkSaNotSecurBreachEnableSet with inlifType, inlifIndex from 1.1.
                                                          and enable [GT_FALSE].
    Expected: GT_OK.
    1.19. Call cpssExMxPmInlifNaStormPreventionEnableSet with inlifType, inlifIndex from 1.1.
                                                        and enable [GT_FALSE].
    Expected: GT_OK.
    1.20. Call cpssExMxPmInlifAutoLearnEnableSet with inlifType, inlifIndex from 1.1.
                                                and enable [GT_FALSE].
    Expected: GT_OK.
    1.21. Call cpssExMxPmInlifNaToCpuEnableSet with inlifType, inlifIndex from 1.1.
                                              and enable [GT_FALSE].
    Expected: GT_OK.
    1.22. Call cpssExMxPmInlifUntaggedMruIndexSet with inlifType, inlifIndex from 1.1.
                                                 and mruIndex [5].
    Expected: GT_OK.
    1.23. Call cpssExMxPmInlifIpUcRouteEnableSet with inlifType, inlifIndex from 1.1.
                                                ipVer [CPSS_IP_PROTOCOL_IPV4_E],
                                                and enable [GT_FALSE].
    Expected: GT_OK.
    1.24. Call cpssExMxPmInlifIpMcRouteEnableSet with inlifType, inlifIndex from 1.1.
                                                ipVer [CPSS_IP_PROTOCOL_IPV4_E],
                                                and enable [GT_FALSE].
    Expected: GT_OK.
    1.25. Call cpssExMxPmInlifMplsRouteEnableSet with inlifType, inlifIndex from 1.1.
                                                 and enable [GT_FALSE].
    Expected: GT_OK.
    1.26. Call cpssExMxPmInlifVrfIdSet with inlifType, inlifIndex from 1.1.
                                       and vrfId [4096-1].
    Expected: GT_OK.
    1.27. Call cpssExMxPmInlifBridgeRouterInterfaceEnabledSet with inlifType, inlifIndex from 1.1.
                                                              and enable [GT_FALSE].
    Expected: GT_OK.
    1.28. Call cpssExMxPmInlifIpSecurityProfileSet with inlifType, inlifIndex from 1.1.
                                                   and profileIndex [5]
    Expected: GT_OK.
    1.29. Call cpssExMxPmInlifMirrorToAnalyzerEnableSet with inlifType, inlifIndex from 1.1.
                                                        and enable [GT_FALSE].
    Expected: GT_OK.
    1.30. Call cpssExMxPmInlifMirrorToCpuEnableSet with inlifType, inlifIndex from 1.1.
                                                   and enable [GT_FALSE].
    Expected: GT_OK.
    1.31. Call cpssExMxPmInlifEntryGet with inlifType, inlifIndex from 1.1.
    Expected: GT_OK and inlifEntryPtr-> {   bridgeEnable [GT_FALSE],
                                            autoLearnEnable [GT_FALSE],
                                            naMessageToCpuEnable [GT_FALSE],
                                            naStormPreventionEnable [GT_FALSE],
                                            unkSaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                            unkDaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                            unkSaNotSecurBreachEnable [GT_FALSE],
                                            untaggedMruIndex[5],
                                            unregNonIpMcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                            unregIpMcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                            unregIpv4BcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                            unregNonIpv4BcCommand[CPSS_PACKET_CMD_FORWARD_E],
                                            ipv4UcRouteEnable [GT_FALSE],
                                            ipv4McRouteEnable [GT_FALSE],
                                            ipv6UcRouteEnable [GT_FALSE],
                                            ipv6McRouteEnable [GT_FALSE],
                                            mplsRouteEnable [GT_FALSE],
                                            vrfId [4096-1],
                                            ipv4IcmpRedirectEnable [GT_FALSE],
                                            ipv6IcmpRedirectEnable [GT_FALSE],
                                            bridgeRouterInterfaceEnable [GT_FALSE],
                                            ipSecurityProfile [5],
                                            ipv4IgmpToCpuEnable [GT_FALSE],
                                            ipv6IgmpToCpuEnable [GT_FALSE],
                                            udpBcRelayEnable [GT_FALSE],
                                            arpBcToCpuEnable [GT_FALSE],
                                            arpBcToMeEnable [GT_FALSE],
                                            ripv1MirrorEnable [GT_FALSE],
                                            ipv4LinkLocalMcCommandEnable [GT_FALSE],
                                            ipv6LinkLocalMcCommandEnable [GT_FALSE],
                                            ipv6NeighborSolicitationEnable [GT_FALSE];
                                            mirrorToAnalyzerEnable [GT_FALSE],
                                            mirrorToCpuEnable [GT_FALSE] }
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable       = GT_FALSE;
    GT_U32      inlifIndex   = 0;
    GT_U32      mruIndex     = 0;
    GT_U32      profileIndex = 0;
    GT_U32      vrfId        = 0;
    CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    CPSS_IP_PROTOCOL_STACK_ENT   ipVer     = CPSS_IP_PROTOCOL_IPV4_E;

    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntry;
    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntryGet;

    cpssOsBzero((GT_VOID*) &inlifEntry, sizeof(inlifEntry));
    cpssOsBzero((GT_VOID*) &inlifEntryGet, sizeof(inlifEntryGet));


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxPmInlifEntrySet with inlifType [CPSS_EXMXPM_INLIF_TYPE_PORT_E / CPSS_EXMXPM_INLIF_TYPE_VLAN_E / CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E],
                                                   inlifIndex [63 / 4095 / 4096],
                                                   inlifEntryPtr-> {  bridgeEnable [GT_FALSE],
                                                                      autoLearnEnable [GT_FALSE],
                                                                      naMessageToCpuEnable [GT_FALSE],
                                                                      naStormPreventionEnable [GT_FALSE],
                                                                      unkSaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                                                      unkDaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                                                      unkSaNotSecurBreachEnable [GT_FALSE],
                                                                      untaggedMruIndex[0],
                                                                      unregNonIpMcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                                      unregIpMcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                                      unregIpv4BcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                                      unregNonIpv4BcCommand[CPSS_PACKET_CMD_FORWARD_E],
                                                                      ipv4UcRouteEnable [GT_FALSE],
                                                                      ipv4McRouteEnable [GT_FALSE],
                                                                      ipv6UcRouteEnable [GT_FALSE],
                                                                      ipv6McRouteEnable [GT_FALSE],
                                                                      mplsRouteEnable [GT_FALSE],
                                                                      vrfId [0],
                                                                      ipv4IcmpRedirectEnable [GT_FALSE],
                                                                      ipv6IcmpRedirectEnable [GT_FALSE],
                                                                      bridgeRouterInterfaceEnable [GT_FALSE],
                                                                      ipSecurityProfile [0],
                                                                      ipv4IgmpToCpuEnable [GT_FALSE],
                                                                      ipv6IcmpToCpuEnable [GT_FALSE],
                                                                      udpBcRelayEnable [GT_FALSE],
                                                                      arpBcToCpuEnable [GT_FALSE],
                                                                      arpBcToMeEnable [GT_FALSE],
                                                                      ripv1MirrorEnable [GT_FALSE],
                                                                      ipv4LinkLocalMcCommandEnable [GT_FALSE],
                                                                      ipv6LinkLocalMcCommandEnable [GT_FALSE],
                                                                      ipv6NeighborSolicitationEnable [GT_FALSE];
                                                                      mirrorToAnalyzerEnable [GT_FALSE],
                                                                      mirrorToCpuEnable [GT_FALSE] }
            Expected: GT_OK.
        */
        inlifEntry.bridgeEnable = GT_TRUE;
        inlifEntry.autoLearnEnable = GT_TRUE;
        inlifEntry.naMessageToCpuEnable = GT_TRUE;
        inlifEntry.naStormPreventionEnable = GT_TRUE;
        inlifEntry.unkSaUcCommand = CPSS_PACKET_CMD_DROP_HARD_E;
        inlifEntry.unkDaUcCommand = CPSS_PACKET_CMD_DROP_HARD_E;
        inlifEntry.unkSaNotSecurBreachEnable = GT_TRUE;
        inlifEntry.untaggedMruIndex = 0;
        inlifEntry.unregNonIpMcCommand = CPSS_PACKET_CMD_FORWARD_E;
        inlifEntry.unregIpMcCommand = CPSS_PACKET_CMD_FORWARD_E;
        inlifEntry.unregIpv4BcCommand = CPSS_PACKET_CMD_FORWARD_E;
        inlifEntry.unregNonIpv4BcCommand = CPSS_PACKET_CMD_FORWARD_E;
        inlifEntry.ipv4UcRouteEnable = GT_TRUE;
        inlifEntry.ipv4McRouteEnable = GT_TRUE;
        inlifEntry.ipv6UcRouteEnable = GT_TRUE;
        inlifEntry.ipv6McRouteEnable = GT_TRUE;
        inlifEntry.mplsRouteEnable = GT_TRUE;
        inlifEntry.vrfId = 0;
        inlifEntry.ipv4IcmpRedirectEnable = GT_FALSE;
        inlifEntry.ipv6IcmpRedirectEnable = GT_FALSE;
        inlifEntry.bridgeRouterInterfaceEnable = GT_TRUE;
        inlifEntry.ipSecurityProfile = 0;
        inlifEntry.ipv4IgmpToCpuEnable = GT_FALSE;
        inlifEntry.ipv6IcmpToCpuEnable = GT_FALSE;
        inlifEntry.udpBcRelayEnable = GT_FALSE;
        inlifEntry.arpBcToCpuEnable = GT_FALSE;
        inlifEntry.arpBcToMeEnable = GT_FALSE;
        inlifEntry.ripv1MirrorEnable = GT_FALSE;
        inlifEntry.ipv4LinkLocalMcCommandEnable = GT_FALSE;
        inlifEntry.ipv6LinkLocalMcCommandEnable = GT_FALSE;
        inlifEntry.ipv6NeighborSolicitationEnable = GT_FALSE;;
        inlifEntry.mirrorToAnalyzerEnable = GT_FALSE;
        inlifEntry.mirrorToCpuEnable = GT_FALSE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntrySet: %d, %d, %d", dev, inlifType, inlifIndex);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntrySet: %d, %d, %d", dev, inlifType, inlifIndex);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifEntrySet(dev, inlifType, inlifIndex, &inlifEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntrySet: %d, %d, %d", dev, inlifType, inlifIndex);

        /*
            1.2. Call cpssExMxPmInlifBridgeEnableSet with inlifType, inlifIndex from 1.1.
                                                     and enable [GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifBridgeEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifBridgeEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifBridgeEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.3. Call cpssExMxPmInlifUnkSaNotSecurBreachEnableSet with inlifType, inlifIndex from 1.1.
                                                                  and enable [GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifUnkSaNotSecurBreachEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifUnkSaNotSecurBreachEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifUnkSaNotSecurBreachEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.4. Call cpssExMxPmInlifNaStormPreventionEnableSet with inlifType, inlifIndex from 1.1.
                                                                and enable [GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifNaStormPreventionEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifNaStormPreventionEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifNaStormPreventionEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.5. Call cpssExMxPmInlifAutoLearnEnableSet with inlifType, inlifIndex from 1.1.
                                                        and enable [GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifAutoLearnEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifAutoLearnEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifAutoLearnEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.6. Call cpssExMxPmInlifNaToCpuEnableSet with inlifType, inlifIndex from 1.1.
                                                      and enable [GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifNaToCpuEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifNaToCpuEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifNaToCpuEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.7. Call cpssExMxPmInlifUntaggedMruIndexSet with inlifType, inlifIndex from 1.1.
                                                         and mruIndex [7].
            Expected: GT_OK.
        */
        mruIndex = 7;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifUntaggedMruIndexSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, mruIndex);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifUntaggedMruIndexSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, mruIndex);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifUntaggedMruIndexSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, mruIndex);

        /*
            1.8. Call cpssExMxPmInlifIpUcRouteEnableSet with inlifType, inlifIndex from 1.1.
                                                        ipVer [CPSS_IP_PROTOCOL_IPV4_E],
                                                        and enable [GT_TRUE].
            Expected: GT_OK.
        */
        /* ipVer  = CPSS_IP_PROTOCOL_IPV4_E */
        ipVer  = CPSS_IP_PROTOCOL_IPV4_E;
        enable = GT_TRUE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpUcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpUcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpUcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /* ipVer  = CPSS_IP_PROTOCOL_IPV6_E */
        ipVer  = CPSS_IP_PROTOCOL_IPV6_E;
        enable = GT_TRUE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpUcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpUcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpUcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /*
            1.9. Call cpssExMxPmInlifIpMcRouteEnableSet with inlifType, inlifIndex from 1.1.
                                                        ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                                                               CPSS_IP_PROTOCOL_IPV4_E],
                                                        and enable [GT_TRUE].
            Expected: GT_OK.
        */
        /* ipVer = CPSS_IP_PROTOCOL_IPV4_E */
        ipVer  = CPSS_IP_PROTOCOL_IPV4_E;
        enable = GT_TRUE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpMcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpMcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpMcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /* ipVer = CPSS_IP_PROTOCOL_IPV6_E */
        ipVer  = CPSS_IP_PROTOCOL_IPV6_E;
        enable = GT_TRUE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpMcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpMcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpMcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /*
            1.10. Call cpssExMxPmInlifMplsRouteEnableSet with inlifType, inlifIndex from 1.1.
                                                         and enable [GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMplsRouteEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMplsRouteEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMplsRouteEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.11. Call cpssExMxPmInlifVrfIdSet with inlifType, inlifIndex from 1.1.
                                               and vrfId [100].
            Expected: GT_OK.
        */
        vrfId = 100;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifVrfIdSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, vrfId);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifVrfIdSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, vrfId);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifVrfIdSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, vrfId);

        /*
            1.12. Call cpssExMxPmInlifBridgeRouterInterfaceEnabledSet with inlifType, inlifIndex from 1.1.
                                                                      and enable [GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifBridgeRouterInterfaceEnabledSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifBridgeRouterInterfaceEnabledSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifBridgeRouterInterfaceEnabledSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.13. Call cpssExMxPmInlifIpSecurityProfileSet with inlifType, inlifIndex from 1.1.
                                                           and profileIndex [15].
            Expected: GT_OK.
        */
        profileIndex = 15;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpSecurityProfileSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, profileIndex);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpSecurityProfileSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, profileIndex);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpSecurityProfileSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, profileIndex);

        /*
            1.14. Call cpssExMxPmInlifMirrorToAnalyzerEnableSet with inlifType, inlifIndex from 1.1.
                                                                and enable [GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMirrorToAnalyzerEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMirrorToAnalyzerEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMirrorToAnalyzerEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.15. Call cpssExMxPmInlifMirrorToCpuEnableSet with inlifType, inlifIndex from 1.1.
                                                           and enable [GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMirrorToCpuEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMirrorToCpuEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMirrorToCpuEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.16. Call cpssExMxPmInlifEntryGet with inlifType, inlifIndex from 1.1.
            Expected: GT_OK and inlifEntryPtr-> {   bridgeEnable [GT_TRUE],
                                                    autoLearnEnable [GT_TRUE],
                                                    naMessageToCpuEnable [GT_TRUE],
                                                    naStormPreventionEnable [GT_TRUE],
                                                    unkSaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                                    unkDaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                                    unkSaNotSecurBreachEnable [GT_TRUE],
                                                    untaggedMruIndex[7],
                                                    unregNonIpMcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                    unregIpMcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                    unregIpv4BcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                    unregNonIpv4BcCommand[CPSS_PACKET_CMD_FORWARD_E],
                                                    ipv4UcRouteEnable [GT_TRUE],
                                                    ipv4McRouteEnable [GT_TRUE],
                                                    ipv6UcRouteEnable [GT_FALSE],
                                                    ipv6McRouteEnable [GT_FALSE],
                                                    mplsRouteEnable [GT_TRUE],
                                                    vrfId [100],
                                                    ipv4IcmpRedirectEnable [GT_FALSE],
                                                    ipv6IcmpRedirectEnable [GT_FALSE],
                                                    bridgeRouterInterfaceEnable [GT_TRUE],
                                                    ipSecurityProfile [15],
                                                    ipv4IgmpToCpuEnable [GT_FALSE],
                                                    ipv6IcmpToCpuEnable [GT_FALSE],
                                                    udpBcRelayEnable [GT_FALSE],
                                                    arpBcToCpuEnable [GT_FALSE],
                                                    arpBcToMeEnable [GT_FALSE],
                                                    ripv1MirrorEnable [GT_FALSE],
                                                    ipv4LinkLocalMcCommandEnable [GT_FALSE],
                                                    ipv6LinkLocalMcCommandEnable [GT_FALSE],
                                                    ipv6NeighborSolicitationEnable [GT_FALSE];
                                                    mirrorToAnalyzerEnable [GT_TRUE],
                                                    mirrorToCpuEnable [GT_TRUE] }
        */
        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d", dev, inlifIndex);

        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.bridgeEnable, "got another inlifEntryGet.bridgeEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.autoLearnEnable, "got another inlifEntryGet.autoLearnEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.naMessageToCpuEnable, "got another inlifEntryGet.naMessageToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.naStormPreventionEnable, "got another inlifEntryGet.naStormPreventionEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_DROP_HARD_E, inlifEntryGet.unkSaUcCommand, "got another inlifEntryGet.unkSaUcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_DROP_HARD_E, inlifEntryGet.unkDaUcCommand, "got another inlifEntryGet.unkDaUcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.unkSaNotSecurBreachEnable, "got another inlifEntryGet.unkSaNotSecurBreachEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(7, inlifEntryGet.untaggedMruIndex, "got another inlifEntryGet.untaggedMruIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregNonIpMcCommand, "got another inlifEntryGet.unregNonIpMcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregIpMcCommand, "got another inlifEntryGet.unregIpMcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregIpv4BcCommand, "got another inlifEntryGet.unregIpv4BcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregNonIpv4BcCommand, "got another inlifEntryGet.unregNonIpv4BcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv4UcRouteEnable, "got another inlifEntryGet.ipv4UcRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv4McRouteEnable, "got another inlifEntryGet.ipv4McRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv6UcRouteEnable, "got another inlifEntryGet.ipv6UcRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv6McRouteEnable, "got another inlifEntryGet.ipv6McRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.mplsRouteEnable, "got another inlifEntryGet.mplsRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(100, inlifEntryGet.vrfId, "got another inlifEntryGet.vrfId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4IcmpRedirectEnable, "got another inlifEntryGet.ipv4IcmpRedirectEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6IcmpRedirectEnable, "got another inlifEntryGet.ipv6IcmpRedirectEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.bridgeRouterInterfaceEnable, "got another inlifEntryGet.bridgeRouterInterfaceEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(15, inlifEntryGet.ipSecurityProfile, "got another inlifEntryGet.ipSecurityProfile than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4IgmpToCpuEnable, "got another inlifEntryGet.ipv4IgmpToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6IcmpToCpuEnable, "got another inlifEntryGet.ipv6IcmpToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.udpBcRelayEnable, "got another inlifEntryGet.udpBcRelayEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.arpBcToCpuEnable, "got another inlifEntryGet.arpBcToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.arpBcToMeEnable, "got another inlifEntryGet.arpBcToMeEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ripv1MirrorEnable, "got another inlifEntryGet.ripv1MirrorEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4LinkLocalMcCommandEnable, "got another inlifEntryGet.ipv4LinkLocalMcCommandEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6LinkLocalMcCommandEnable, "got another inlifEntryGet.ipv6LinkLocalMcCommandEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6NeighborSolicitationEnable, "got another inlifEntryGet.ipv6NeighborSolicitationEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.mirrorToAnalyzerEnable, "got another inlifEntryGet.mirrorToAnalyzerEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.mirrorToCpuEnable, "got another inlifEntryGet.mirrorToCpuEnable than was set: %d", dev);
        }

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d", dev, inlifIndex);

        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.bridgeEnable, "got another inlifEntryGet.bridgeEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.autoLearnEnable, "got another inlifEntryGet.autoLearnEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.naMessageToCpuEnable, "got another inlifEntryGet.naMessageToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.naStormPreventionEnable, "got another inlifEntryGet.naStormPreventionEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_DROP_HARD_E, inlifEntryGet.unkSaUcCommand, "got another inlifEntryGet.unkSaUcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_DROP_HARD_E, inlifEntryGet.unkDaUcCommand, "got another inlifEntryGet.unkDaUcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.unkSaNotSecurBreachEnable, "got another inlifEntryGet.unkSaNotSecurBreachEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(7, inlifEntryGet.untaggedMruIndex, "got another inlifEntryGet.untaggedMruIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregNonIpMcCommand, "got another inlifEntryGet.unregNonIpMcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregIpMcCommand, "got another inlifEntryGet.unregIpMcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregIpv4BcCommand, "got another inlifEntryGet.unregIpv4BcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregNonIpv4BcCommand, "got another inlifEntryGet.unregNonIpv4BcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv4UcRouteEnable, "got another inlifEntryGet.ipv4UcRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv4McRouteEnable, "got another inlifEntryGet.ipv4McRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv6UcRouteEnable, "got another inlifEntryGet.ipv6UcRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv6McRouteEnable, "got another inlifEntryGet.ipv6McRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.mplsRouteEnable, "got another inlifEntryGet.mplsRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(100, inlifEntryGet.vrfId, "got another inlifEntryGet.vrfId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4IcmpRedirectEnable, "got another inlifEntryGet.ipv4IcmpRedirectEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6IcmpRedirectEnable, "got another inlifEntryGet.ipv6IcmpRedirectEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.bridgeRouterInterfaceEnable, "got another inlifEntryGet.bridgeRouterInterfaceEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(15, inlifEntryGet.ipSecurityProfile, "got another inlifEntryGet.ipSecurityProfile than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4IgmpToCpuEnable, "got another inlifEntryGet.ipv4IgmpToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6IcmpToCpuEnable, "got another inlifEntryGet.ipv6IcmpToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.udpBcRelayEnable, "got another inlifEntryGet.udpBcRelayEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.arpBcToCpuEnable, "got another inlifEntryGet.arpBcToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.arpBcToMeEnable, "got another inlifEntryGet.arpBcToMeEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ripv1MirrorEnable, "got another inlifEntryGet.ripv1MirrorEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4LinkLocalMcCommandEnable, "got another inlifEntryGet.ipv4LinkLocalMcCommandEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6LinkLocalMcCommandEnable, "got another inlifEntryGet.ipv6LinkLocalMcCommandEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6NeighborSolicitationEnable, "got another inlifEntryGet.ipv6NeighborSolicitationEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.mirrorToAnalyzerEnable, "got another inlifEntryGet.mirrorToAnalyzerEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.mirrorToCpuEnable, "got another inlifEntryGet.mirrorToCpuEnable than was set: %d", dev);
        }

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d", dev, inlifIndex);

        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.bridgeEnable, "got another inlifEntryGet.bridgeEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.autoLearnEnable, "got another inlifEntryGet.autoLearnEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.naMessageToCpuEnable, "got another inlifEntryGet.naMessageToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.naStormPreventionEnable, "got another inlifEntryGet.naStormPreventionEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_DROP_HARD_E, inlifEntryGet.unkSaUcCommand, "got another inlifEntryGet.unkSaUcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_DROP_HARD_E, inlifEntryGet.unkDaUcCommand, "got another inlifEntryGet.unkDaUcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.unkSaNotSecurBreachEnable, "got another inlifEntryGet.unkSaNotSecurBreachEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(7, inlifEntryGet.untaggedMruIndex, "got another inlifEntryGet.untaggedMruIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregNonIpMcCommand, "got another inlifEntryGet.unregNonIpMcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregIpMcCommand, "got another inlifEntryGet.unregIpMcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregIpv4BcCommand, "got another inlifEntryGet.unregIpv4BcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregNonIpv4BcCommand, "got another inlifEntryGet.unregNonIpv4BcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv4UcRouteEnable, "got another inlifEntryGet.ipv4UcRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv4McRouteEnable, "got another inlifEntryGet.ipv4McRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv6UcRouteEnable, "got another inlifEntryGet.ipv6UcRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv6McRouteEnable, "got another inlifEntryGet.ipv6McRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.mplsRouteEnable, "got another inlifEntryGet.mplsRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(100, inlifEntryGet.vrfId, "got another inlifEntryGet.vrfId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4IcmpRedirectEnable, "got another inlifEntryGet.ipv4IcmpRedirectEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6IcmpRedirectEnable, "got another inlifEntryGet.ipv6IcmpRedirectEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.bridgeRouterInterfaceEnable, "got another inlifEntryGet.bridgeRouterInterfaceEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(15, inlifEntryGet.ipSecurityProfile, "got another inlifEntryGet.ipSecurityProfile than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4IgmpToCpuEnable, "got another inlifEntryGet.ipv4IgmpToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6IcmpToCpuEnable, "got another inlifEntryGet.ipv6IcmpToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.udpBcRelayEnable, "got another inlifEntryGet.udpBcRelayEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.arpBcToCpuEnable, "got another inlifEntryGet.arpBcToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.arpBcToMeEnable, "got another inlifEntryGet.arpBcToMeEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ripv1MirrorEnable, "got another inlifEntryGet.ripv1MirrorEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4LinkLocalMcCommandEnable, "got another inlifEntryGet.ipv4LinkLocalMcCommandEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6LinkLocalMcCommandEnable, "got another inlifEntryGet.ipv6LinkLocalMcCommandEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6NeighborSolicitationEnable, "got another inlifEntryGet.ipv6NeighborSolicitationEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.mirrorToAnalyzerEnable, "got another inlifEntryGet.mirrorToAnalyzerEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.mirrorToCpuEnable, "got another inlifEntryGet.mirrorToCpuEnable than was set: %d", dev);
        }

        /*
            1.17. Call cpssExMxPmInlifBridgeEnableSet with inlifType, inlifIndex from 1.1.
                                                     and enable [GT_FALSE].
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifBridgeEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifBridgeEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifBridgeEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifBridgeEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.18. Call cpssExMxPmInlifUnkSaNotSecurBreachEnableSet with inlifType, inlifIndex from 1.1.
                                                                  and enable [GT_FALSE].
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifUnkSaNotSecurBreachEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifUnkSaNotSecurBreachEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifUnkSaNotSecurBreachEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.19. Call cpssExMxPmInlifNaStormPreventionEnableSet with inlifType, inlifIndex from 1.1.
                                                                and enable [GT_FALSE].
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifNaStormPreventionEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifNaStormPreventionEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifNaStormPreventionEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifNaStormPreventionEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.20. Call cpssExMxPmInlifAutoLearnEnableSet with inlifType, inlifIndex from 1.1.
                                                        and enable [GT_FALSE].
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifAutoLearnEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifAutoLearnEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifAutoLearnEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifAutoLearnEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.21. Call cpssExMxPmInlifNaToCpuEnableSet with inlifType, inlifIndex from 1.1.
                                                      and enable [GT_FALSE].
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifNaToCpuEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifNaToCpuEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifNaToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifNaToCpuEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.22. Call cpssExMxPmInlifUntaggedMruIndexSet with inlifType, inlifIndex from 1.1.
                                                         and mruIndex [5].
            Expected: GT_OK.
        */
        mruIndex = 5;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifUntaggedMruIndexSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, mruIndex);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifUntaggedMruIndexSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, mruIndex);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifUntaggedMruIndexSet(dev, inlifType, inlifIndex, mruIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifUntaggedMruIndexSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, mruIndex);

        /*
            1.23. Call cpssExMxPmInlifIpUcRouteEnableSet with inlifType, inlifIndex from 1.1.
                                                        ipVer [CPSS_IP_PROTOCOL_IPV4_E],
                                                        and enable [GT_FALSE].
            Expected: GT_OK.
        */
        ipVer  = CPSS_IP_PROTOCOL_IPV4_E;
        enable = GT_FALSE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpUcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpUcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifIpUcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpUcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /*
            1.24. Call cpssExMxPmInlifIpMcRouteEnableSet with inlifType, inlifIndex from 1.1.
                                                        ipVer [CPSS_IP_PROTOCOL_IPV4_E],
                                                        and enable [GT_FALSE].
            Expected: GT_OK.
        */
        ipVer  = CPSS_IP_PROTOCOL_IPV4_E;
        enable = GT_FALSE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpMcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpMcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifIpMcRouteEnableSet(dev, inlifType, inlifIndex, ipVer, enable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpMcRouteEnableSet: %d, %d, %d, %d, %d", dev, inlifType, inlifIndex, ipVer, enable);

        /*
            1.25. Call cpssExMxPmInlifMplsRouteEnableSet with inlifType, inlifIndex from 1.1.
                                                         and enable [GT_FALSE].
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMplsRouteEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMplsRouteEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifMplsRouteEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMplsRouteEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.26. Call cpssExMxPmInlifVrfIdSet with inlifType, inlifIndex from 1.1.
                                               and vrfId [4096-1].
            Expected: GT_OK.
        */
        vrfId = 4096-1;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifVrfIdSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, vrfId);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifVrfIdSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, vrfId);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifVrfIdSet(dev, inlifType, inlifIndex, vrfId);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifVrfIdSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, vrfId);

        /*
            1.27. Call cpssExMxPmInlifBridgeRouterInterfaceEnabledSet with inlifType, inlifIndex from 1.1.
                                                                      and enable [GT_FALSE].
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifBridgeRouterInterfaceEnabledSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifBridgeRouterInterfaceEnabledSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifBridgeRouterInterfaceEnabledSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.28. Call cpssExMxPmInlifIpSecurityProfileSet with inlifType, inlifIndex from 1.1.
                                                           and profileIndex [5]
            Expected: GT_OK.
        */
        profileIndex = 5;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpSecurityProfileSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, profileIndex);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpSecurityProfileSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, profileIndex);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifIpSecurityProfileSet(dev, inlifType, inlifIndex, profileIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifIpSecurityProfileSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, profileIndex);

        /*
            1.29. Call cpssExMxPmInlifMirrorToAnalyzerEnableSet with inlifType, inlifIndex from 1.1.
                                                                and enable [GT_FALSE].
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMirrorToAnalyzerEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMirrorToAnalyzerEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifMirrorToAnalyzerEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMirrorToAnalyzerEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.30. Call cpssExMxPmInlifMirrorToCpuEnableSet with inlifType, inlifIndex from 1.1.
                                                           and enable [GT_FALSE].
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMirrorToCpuEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMirrorToCpuEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifMirrorToCpuEnableSet(dev, inlifType, inlifIndex, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmInlifMirrorToCpuEnableSet: %d, %d, %d, %d", dev, inlifType, inlifIndex, enable);

        /*
            1.31. Call cpssExMxPmInlifEntryGet with inlifType, inlifIndex from 1.1.
            Expected: GT_OK and inlifEntryPtr-> {   bridgeEnable [GT_FALSE],
                                                    autoLearnEnable [GT_FALSE],
                                                    naMessageToCpuEnable [GT_FALSE],
                                                    naStormPreventionEnable [GT_FALSE],
                                                    unkSaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                                    unkDaUcCommand[CPSS_PACKET_CMD_DROP_HARD_E],
                                                    unkSaNotSecurBreachEnable [GT_FALSE],
                                                    untaggedMruIndex[5],
                                                    unregNonIpMcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                    unregIpMcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                    unregIpv4BcCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                    unregNonIpv4BcCommand[CPSS_PACKET_CMD_FORWARD_E],
                                                    ipv4UcRouteEnable [GT_FALSE],
                                                    ipv4McRouteEnable [GT_FALSE],
                                                    ipv6UcRouteEnable [GT_FALSE],
                                                    ipv6McRouteEnable [GT_FALSE],
                                                    mplsRouteEnable [GT_FALSE],
                                                    vrfId [4096-1],
                                                    ipv4IcmpRedirectEnable [GT_FALSE],
                                                    ipv6IcmpRedirectEnable [GT_FALSE],
                                                    bridgeRouterInterfaceEnable [GT_FALSE],
                                                    ipSecurityProfile [5],
                                                    ipv4IgmpToCpuEnable [GT_FALSE],
                                                    ipv6IcmpToCpuEnable [GT_FALSE],
                                                    udpBcRelayEnable [GT_FALSE],
                                                    arpBcToCpuEnable [GT_FALSE],
                                                    arpBcToMeEnable [GT_FALSE],
                                                    ripv1MirrorEnable [GT_FALSE],
                                                    ipv4LinkLocalMcCommandEnable [GT_FALSE],
                                                    ipv6LinkLocalMcCommandEnable [GT_FALSE],
                                                    ipv6NeighborSolicitationEnable [GT_FALSE];
                                                    mirrorToAnalyzerEnable [GT_FALSE],
                                                    mirrorToCpuEnable [GT_FALSE] }
        */
        /* iterate with CPSS_EXMXPM_INLIF_TYPE_PORT_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
        inlifIndex = 63;

        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d", dev, inlifIndex);

        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.bridgeEnable, "got another inlifEntryGet.bridgeEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.autoLearnEnable, "got another inlifEntryGet.autoLearnEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.naMessageToCpuEnable, "got another inlifEntryGet.naMessageToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.naStormPreventionEnable, "got another inlifEntryGet.naStormPreventionEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_DROP_HARD_E, inlifEntryGet.unkSaUcCommand, "got another inlifEntryGet.unkSaUcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_DROP_HARD_E, inlifEntryGet.unkDaUcCommand, "got another inlifEntryGet.unkDaUcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.unkSaNotSecurBreachEnable, "got another inlifEntryGet.unkSaNotSecurBreachEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(5, inlifEntryGet.untaggedMruIndex, "got another inlifEntryGet.untaggedMruIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregNonIpMcCommand, "got another inlifEntryGet.unregNonIpMcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregIpMcCommand, "got another inlifEntryGet.unregIpMcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregIpv4BcCommand, "got another inlifEntryGet.unregIpv4BcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregNonIpv4BcCommand, "got another inlifEntryGet.unregNonIpv4BcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4UcRouteEnable, "got another inlifEntryGet.ipv4UcRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4McRouteEnable, "got another inlifEntryGet.ipv4McRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv6UcRouteEnable, "got another inlifEntryGet.ipv6UcRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv6McRouteEnable, "got another inlifEntryGet.ipv6McRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.mplsRouteEnable, "got another inlifEntryGet.mplsRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC((4096-1), inlifEntryGet.vrfId, "got another inlifEntryGet.vrfId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4IcmpRedirectEnable, "got another inlifEntryGet.ipv4IcmpRedirectEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6IcmpRedirectEnable, "got another inlifEntryGet.ipv6IcmpRedirectEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.bridgeRouterInterfaceEnable, "got another inlifEntryGet.bridgeRouterInterfaceEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(5, inlifEntryGet.ipSecurityProfile, "got another inlifEntryGet.ipSecurityProfile than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4IgmpToCpuEnable, "got another inlifEntryGet.ipv4IgmpToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6IcmpToCpuEnable, "got another inlifEntryGet.ipv6IcmpToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.udpBcRelayEnable, "got another inlifEntryGet.udpBcRelayEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.arpBcToCpuEnable, "got another inlifEntryGet.arpBcToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.arpBcToMeEnable, "got another inlifEntryGet.arpBcToMeEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ripv1MirrorEnable, "got another inlifEntryGet.ripv1MirrorEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4LinkLocalMcCommandEnable, "got another inlifEntryGet.ipv4LinkLocalMcCommandEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6LinkLocalMcCommandEnable, "got another inlifEntryGet.ipv6LinkLocalMcCommandEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6NeighborSolicitationEnable, "got another inlifEntryGet.ipv6NeighborSolicitationEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.mirrorToAnalyzerEnable, "got another inlifEntryGet.mirrorToAnalyzerEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.mirrorToCpuEnable, "got another inlifEntryGet.mirrorToCpuEnable than was set: %d", dev);
        }

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_VLAN_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
        inlifIndex = 4095;

        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d", dev, inlifIndex);

        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.bridgeEnable, "got another inlifEntryGet.bridgeEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.autoLearnEnable, "got another inlifEntryGet.autoLearnEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.naMessageToCpuEnable, "got another inlifEntryGet.naMessageToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.naStormPreventionEnable, "got another inlifEntryGet.naStormPreventionEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_DROP_HARD_E, inlifEntryGet.unkSaUcCommand, "got another inlifEntryGet.unkSaUcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_DROP_HARD_E, inlifEntryGet.unkDaUcCommand, "got another inlifEntryGet.unkDaUcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.unkSaNotSecurBreachEnable, "got another inlifEntryGet.unkSaNotSecurBreachEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(5, inlifEntryGet.untaggedMruIndex, "got another inlifEntryGet.untaggedMruIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregNonIpMcCommand, "got another inlifEntryGet.unregNonIpMcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregIpMcCommand, "got another inlifEntryGet.unregIpMcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregIpv4BcCommand, "got another inlifEntryGet.unregIpv4BcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregNonIpv4BcCommand, "got another inlifEntryGet.unregNonIpv4BcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4UcRouteEnable, "got another inlifEntryGet.ipv4UcRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4McRouteEnable, "got another inlifEntryGet.ipv4McRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv6UcRouteEnable, "got another inlifEntryGet.ipv6UcRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv6McRouteEnable, "got another inlifEntryGet.ipv6McRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.mplsRouteEnable, "got another inlifEntryGet.mplsRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC((4096-1), inlifEntryGet.vrfId, "got another inlifEntryGet.vrfId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4IcmpRedirectEnable, "got another inlifEntryGet.ipv4IcmpRedirectEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6IcmpRedirectEnable, "got another inlifEntryGet.ipv6IcmpRedirectEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.bridgeRouterInterfaceEnable, "got another inlifEntryGet.bridgeRouterInterfaceEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(5, inlifEntryGet.ipSecurityProfile, "got another inlifEntryGet.ipSecurityProfile than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4IgmpToCpuEnable, "got another inlifEntryGet.ipv4IgmpToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6IcmpToCpuEnable, "got another inlifEntryGet.ipv6IcmpToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.udpBcRelayEnable, "got another inlifEntryGet.udpBcRelayEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.arpBcToCpuEnable, "got another inlifEntryGet.arpBcToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.arpBcToMeEnable, "got another inlifEntryGet.arpBcToMeEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ripv1MirrorEnable, "got another inlifEntryGet.ripv1MirrorEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4LinkLocalMcCommandEnable, "got another inlifEntryGet.ipv4LinkLocalMcCommandEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6LinkLocalMcCommandEnable, "got another inlifEntryGet.ipv6LinkLocalMcCommandEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6NeighborSolicitationEnable, "got another inlifEntryGet.ipv6NeighborSolicitationEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.mirrorToAnalyzerEnable, "got another inlifEntryGet.mirrorToAnalyzerEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.mirrorToCpuEnable, "got another inlifEntryGet.mirrorToCpuEnable than was set: %d", dev);
        }

        /* iterate with CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E */
        inlifType = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
        inlifIndex = 4096;

        st = cpssExMxPmInlifEntryGet(dev, inlifType, inlifIndex, &inlifEntryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmInlifEntryGet: %d, %d", dev, inlifIndex);

        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.bridgeEnable, "got another inlifEntryGet.bridgeEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.autoLearnEnable, "got another inlifEntryGet.autoLearnEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.naMessageToCpuEnable, "got another inlifEntryGet.naMessageToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.naStormPreventionEnable, "got another inlifEntryGet.naStormPreventionEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_DROP_HARD_E, inlifEntryGet.unkSaUcCommand, "got another inlifEntryGet.unkSaUcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_DROP_HARD_E, inlifEntryGet.unkDaUcCommand, "got another inlifEntryGet.unkDaUcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.unkSaNotSecurBreachEnable, "got another inlifEntryGet.unkSaNotSecurBreachEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(5, inlifEntryGet.untaggedMruIndex, "got another inlifEntryGet.untaggedMruIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregNonIpMcCommand, "got another inlifEntryGet.unregNonIpMcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregIpMcCommand, "got another inlifEntryGet.unregIpMcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregIpv4BcCommand, "got another inlifEntryGet.unregIpv4BcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(CPSS_PACKET_CMD_FORWARD_E, inlifEntryGet.unregNonIpv4BcCommand, "got another inlifEntryGet.unregNonIpv4BcCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4UcRouteEnable, "got another inlifEntryGet.ipv4UcRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4McRouteEnable, "got another inlifEntryGet.ipv4McRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv6UcRouteEnable, "got another inlifEntryGet.ipv6UcRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, inlifEntryGet.ipv6McRouteEnable, "got another inlifEntryGet.ipv6McRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.mplsRouteEnable, "got another inlifEntryGet.mplsRouteEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC((4096-1), inlifEntryGet.vrfId, "got another inlifEntryGet.vrfId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4IcmpRedirectEnable, "got another inlifEntryGet.ipv4IcmpRedirectEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6IcmpRedirectEnable, "got another inlifEntryGet.ipv6IcmpRedirectEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.bridgeRouterInterfaceEnable, "got another inlifEntryGet.bridgeRouterInterfaceEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(5, inlifEntryGet.ipSecurityProfile, "got another inlifEntryGet.ipSecurityProfile than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4IgmpToCpuEnable, "got another inlifEntryGet.ipv4IgmpToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6IcmpToCpuEnable, "got another inlifEntryGet.ipv6IcmpToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.udpBcRelayEnable, "got another inlifEntryGet.udpBcRelayEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.arpBcToCpuEnable, "got another inlifEntryGet.arpBcToCpuEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.arpBcToMeEnable, "got another inlifEntryGet.arpBcToMeEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ripv1MirrorEnable, "got another inlifEntryGet.ripv1MirrorEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv4LinkLocalMcCommandEnable, "got another inlifEntryGet.ipv4LinkLocalMcCommandEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6LinkLocalMcCommandEnable, "got another inlifEntryGet.ipv6LinkLocalMcCommandEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.ipv6NeighborSolicitationEnable, "got another inlifEntryGet.ipv6NeighborSolicitationEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.mirrorToAnalyzerEnable, "got another inlifEntryGet.mirrorToAnalyzerEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, inlifEntryGet.mirrorToCpuEnable, "got another inlifEntryGet.mirrorToCpuEnable than was set: %d", dev);
        }
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmInlif suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmInlif)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifPortDefaultInlifIdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifPortDefaultInlifIdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifPortModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifPortModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifIntForceVlanModeOnTtSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifIntForceVlanModeOnTtGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifBridgeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifUnkSaNotSecurBreachEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifNaStormPreventionEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifAutoLearnEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifNaToCpuEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifUnkUnregAddrCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifUntaggedMruIndexSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifIpUcRouteEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifIpMcRouteEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifMplsRouteEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifVrfIdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifBridgeRouterInterfaceEnabledSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifIpSecurityProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifControlTrafficEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifMirrorToAnalyzerEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifMirrorToCpuEnableSet)
    /* Tests for Table */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifFillInlifPortTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifFillInlifVlanTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifFillInlifExternalTable)
    /* Test for Functionality */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmInlifFunctionality)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmInlif)

