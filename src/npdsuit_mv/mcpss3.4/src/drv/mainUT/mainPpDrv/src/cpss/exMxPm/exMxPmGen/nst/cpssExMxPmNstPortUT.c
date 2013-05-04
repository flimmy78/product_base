/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmNstPortUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmNstPort, that provides
*        CPSS EXMXPM Port Network Shield Technology facility API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/nst/cpssExMxPmNstPort.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define NST_PORT_VALID_VIRT_PORT_CNS 0
#define NST_PORT_VALID_PHY_PORT_CNS  0

/* Invalid enum */
#define NST_PORT_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstPortIngressFrwFilterEnableSet
(
    IN GT_U8                            devNum,
    IN GT_U8                            port,
    IN CPSS_NST_INGRESS_FRW_FILTER_ENT  filterType,
    IN GT_BOOL                          enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstPortIngressFrwFilterEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with filterType [CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E /
                                 CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E]
                     and enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK. 
    1.1.2. Call cpssExMxPmNstPortIngressFrwFilterEnableGet with the same filterType and non-NULL enablePtr.
    Expected: GT_OK and the same enable. 
    1.1.3. Call with out of range filterType[0x5AAAAAA5]
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PARAM.  
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = NST_PORT_VALID_VIRT_PORT_CNS;

    CPSS_NST_INGRESS_FRW_FILTER_ENT  filterType = CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E;
    GT_BOOL                          enable     = GT_FALSE;
    GT_BOOL                          enableGet  = GT_FALSE;


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
                1.1.1. Call with filterType [CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E /
                                             CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E /
                                             CPSS_NST_INGRESS_FRW_FILTER_TO_ANALYZER_E]
                                 and enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK. 
            */

            /* Call with filterType [CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E] */
            filterType = CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E;
            enable     = GT_FALSE;

            st = cpssExMxPmNstPortIngressFrwFilterEnableSet(dev, port, filterType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, filterType, enable);

            /*
                1.1.2. Call cpssExMxPmNstPortIngressFrwFilterEnableGet with the same filterType and non-NULL enablePtr.
                Expected: GT_OK and the same enable. 
            */
            st = cpssExMxPmNstPortIngressFrwFilterEnableGet(dev, port, filterType, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstPortIngressFrwFilterEnableGet: %d, %d, %d", dev, port, filterType);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            
            /* Call with filterType [CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E] */
            filterType = CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E;
            enable     = GT_TRUE;

            st = cpssExMxPmNstPortIngressFrwFilterEnableSet(dev, port, filterType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, filterType, enable);

            /*
                1.1.2. Call cpssExMxPmNstPortIngressFrwFilterEnableGet with the same filterType and non-NULL enablePtr.
                Expected: GT_OK and the same enable. 
            */
            st = cpssExMxPmNstPortIngressFrwFilterEnableGet(dev, port, filterType, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstPortIngressFrwFilterEnableGet: %d, %d, %d", dev, port, filterType);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range filterType[0x5AAAAAA5]
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PARAM.  
            */
            filterType = NST_PORT_INVALID_ENUM_CNS;
            
            st = cpssExMxPmNstPortIngressFrwFilterEnableSet(dev, port, filterType, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, filterType);
        }

        filterType = CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E;
        enable     = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmNstPortIngressFrwFilterEnableSet(dev, port, filterType, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmNstPortIngressFrwFilterEnableSet(dev, port, filterType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmNstPortIngressFrwFilterEnableSet(dev, port, filterType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    filterType = CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E;
    enable     = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = NST_PORT_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstPortIngressFrwFilterEnableSet(dev, port, filterType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmNstPortIngressFrwFilterEnableSet(dev, port, filterType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstPortIngressFrwFilterEnableGet
(
    IN  GT_U8                            devNum,
    IN  GT_U8                            port,
    IN  CPSS_NST_INGRESS_FRW_FILTER_ENT  filterType,
    OUT GT_BOOL                          *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstPortIngressFrwFilterEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with filterType[CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E /
                                CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E]
                     and non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with out of range filterType [0x5AAAAAA5]
                     and and other parameters from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with enablePtr[NULL]
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR. 
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = NST_PORT_VALID_VIRT_PORT_CNS;

    CPSS_NST_INGRESS_FRW_FILTER_ENT  filterType = CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E;
    GT_BOOL                          enable     = GT_FALSE;
    

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
                1.1.1. Call with filterType[CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E /
                                            CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E /
                                            CPSS_NST_INGRESS_FRW_FILTER_TO_ANALYZER_E]
                                 and non-null enablePtr.
                Expected: GT_OK.
            */

            /* Call with filterType [CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E] */
            filterType = CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E;
            
            st = cpssExMxPmNstPortIngressFrwFilterEnableGet(dev, port, filterType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, filterType);

            /* Call with filterType [CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E] */
            filterType = CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E;
            
            st = cpssExMxPmNstPortIngressFrwFilterEnableGet(dev, port, filterType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, filterType);

            /*
                1.1.2. Call with out of range filterType[0x5AAAAAA5]
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PARAM.  
            */
            filterType = NST_PORT_INVALID_ENUM_CNS;
            
            st = cpssExMxPmNstPortIngressFrwFilterEnableGet(dev, port, filterType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, filterType);

            /*
                1.1.3. Call with enablePtr[NULL]
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR. 
            */
            st = cpssExMxPmNstPortIngressFrwFilterEnableGet(dev, port, filterType, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        filterType = CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E;
        
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmNstPortIngressFrwFilterEnableGet(dev, port, filterType, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmNstPortIngressFrwFilterEnableGet(dev, port, filterType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmNstPortIngressFrwFilterEnableGet(dev, port, filterType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    filterType = CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E;
    
    /* 2. For not active devices check that function returns non GT_OK.*/
    port = NST_PORT_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstPortIngressFrwFilterEnableGet(dev, port, filterType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmNstPortIngressFrwFilterEnableGet(dev, port, filterType, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstBrgIngressFrwFilterDropCntrGet
(
    IN GT_U8       devNum,
    OUT GT_U32     *ingressCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstBrgIngressFrwFilterDropCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null ingressCntrPtr. 
    Expected: GT_OK.
    1.2. Call with ingressCntrPtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      ingressCntr = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-null ingressCntrPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmNstBrgIngressFrwFilterDropCntrGet(dev, &ingressCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null ingressCntrPtr [NULL]. 
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNstBrgIngressFrwFilterDropCntrGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ingressCntrPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstBrgIngressFrwFilterDropCntrGet(dev, &ingressCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNstBrgIngressFrwFilterDropCntrGet(dev, &ingressCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstPortEgressFrwFilterEnableSet
(
    IN GT_U8                            devNum,
    IN GT_U8                            port,
    IN CPSS_NST_EGRESS_FRW_FILTER_ENT   filterType,
    IN GT_BOOL                          enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstPortEgressFrwFilterEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with filterType [CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E /
                                 CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E /
                                 CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E]
                     and enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK. 
    1.1.2. Call cpssExMxPmNstPortEgressFrwFilterEnableGet with the same filterType and non-NULL enablePtr.
    Expected: GT_OK and the same enable. 
    1.1.3. Call with out of range filterType[0x5AAAAAA5]
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PARAM.  
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = NST_PORT_VALID_VIRT_PORT_CNS;

    CPSS_NST_EGRESS_FRW_FILTER_ENT  filterType = CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E;
    GT_BOOL                         enable     = GT_FALSE;
    GT_BOOL                         enableGet  = GT_FALSE;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtsical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with filterType [CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E /
                                             CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E /
                                             CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E]
                                 and enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */

            /* Call with filterType [CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E] */
            filterType = CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E;
            enable     = GT_FALSE;

            st = cpssExMxPmNstPortEgressFrwFilterEnableSet(dev, port, filterType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, filterType, enable);

            /*
                1.1.2. Call cpssExMxPmNstPortEgressFrwFilterEnableGet with the same filterType and non-NULL enablePtr.
                Expected: GT_OK and the same enable. 
            */
            st = cpssExMxPmNstPortEgressFrwFilterEnableGet(dev, port, filterType, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstPortEgressFrwFilterEnableGet: %d, %d, %d", dev, port, filterType);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            
            /* Call with filterType [CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E] */
            filterType = CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E;
            enable     = GT_TRUE;

            st = cpssExMxPmNstPortEgressFrwFilterEnableSet(dev, port, filterType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, filterType, enable);

            /*
                1.1.2. Call cpssExMxPmNstPortEgressFrwFilterEnableGet with the same filterType and non-NULL enablePtr.
                Expected: GT_OK and the same enable. 
            */
            st = cpssExMxPmNstPortEgressFrwFilterEnableGet(dev, port, filterType, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstPortEgressFrwFilterEnableGet: %d, %d, %d", dev, port, filterType);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with filterType [CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E] */
            filterType = CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E;
            
            st = cpssExMxPmNstPortEgressFrwFilterEnableSet(dev, port, filterType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, filterType, enable);

            /*
                1.1.2. Call cpssExMxPmNstPortEgressFrwFilterEnableGet with the same filterType and non-NULL enablePtr.
                Expected: GT_OK and the same enable. 
            */
            st = cpssExMxPmNstPortEgressFrwFilterEnableGet(dev, port, filterType, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstPortEgressFrwFilterEnableGet: %d, %d, %d", dev, port, filterType);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range filterType[0x5AAAAAA5]
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PARAM.  
            */
            filterType = NST_PORT_INVALID_ENUM_CNS;
            
            st = cpssExMxPmNstPortEgressFrwFilterEnableSet(dev, port, filterType, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, filterType);
        }

        filterType = CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E;
        enable     = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtsical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmNstPortEgressFrwFilterEnableSet(dev, port, filterType, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmNstPortEgressFrwFilterEnableSet(dev, port, filterType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmNstPortEgressFrwFilterEnableSet(dev, port, filterType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    filterType = CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E;
    enable     = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = NST_PORT_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstPortEgressFrwFilterEnableSet(dev, port, filterType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmNstPortEgressFrwFilterEnableSet(dev, port, filterType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstPortEgressFrwFilterEnableGet
(
    IN  GT_U8                            devNum,
    IN  GT_U8                            port,
    IN  CPSS_NST_EGRESS_FRW_FILTER_ENT   filterType,
    OUT GT_BOOL                          *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstPortEgressFrwFilterEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with filterType [CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E /
                                 CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E /
                                 CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E]
                     and non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with out of range filterType [0x5AAAAAA5]
                     and and other parameters from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with enablePtr[NULL]
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR. 
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = NST_PORT_VALID_VIRT_PORT_CNS;

    CPSS_NST_EGRESS_FRW_FILTER_ENT  filterType = CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E;
    GT_BOOL                         enable     = GT_FALSE;
    

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtsical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with filterType [CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E /
                                             CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E /
                                             CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E]
                                 and non-null enablePtr.
                Expected: GT_OK.
            */

            /* Call with filterType [CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E] */
            filterType = CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E;
            
            st = cpssExMxPmNstPortEgressFrwFilterEnableGet(dev, port, filterType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, filterType);

            /* Call with filterType [CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E] */
            filterType = CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E;
            
            st = cpssExMxPmNstPortEgressFrwFilterEnableGet(dev, port, filterType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, filterType);

            /* Call with filterType [CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E] */
            filterType = CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E;
            
            st = cpssExMxPmNstPortEgressFrwFilterEnableGet(dev, port, filterType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, filterType);

            /*
                1.1.2. Call with out of range filterType[0x5AAAAAA5]
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PARAM.  
            */
            filterType = NST_PORT_INVALID_ENUM_CNS;
            
            st = cpssExMxPmNstPortEgressFrwFilterEnableGet(dev, port, filterType, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, filterType);

            /*
                1.1.3. Call with enablePtr[NULL]
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR. 
            */
            st = cpssExMxPmNstPortEgressFrwFilterEnableGet(dev, port, filterType, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        filterType = CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E;
        
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtsical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmNstPortEgressFrwFilterEnableGet(dev, port, filterType, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmNstPortEgressFrwFilterEnableGet(dev, port, filterType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmNstPortEgressFrwFilterEnableGet(dev, port, filterType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    filterType = CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E;
    
    /* 2. For not active devices check that function returns non GT_OK.*/
    port = NST_PORT_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstPortEgressFrwFilterEnableGet(dev, port, filterType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmNstPortEgressFrwFilterEnableGet(dev, port, filterType, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmNstPort suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmNstPort)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstPortIngressFrwFilterEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstPortIngressFrwFilterEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstBrgIngressFrwFilterDropCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstPortEgressFrwFilterEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstPortEgressFrwFilterEnableGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmNstPort)

