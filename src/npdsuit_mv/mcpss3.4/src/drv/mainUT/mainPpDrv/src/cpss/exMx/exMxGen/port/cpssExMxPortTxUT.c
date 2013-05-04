/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPortTxUT.c
*
* DESCRIPTION:
*       Unit Tests for the Physical Port Tx Traffic Class
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
******************************************************************************/

#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortTx.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/exMx/exMxGen/cos/private/prvCpssExMxCoS.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/* Max burst size is 4K */
#define PORT_TX_MAX_BURST_SIZE_CNS          0x1000

/* Max descriptors number is 4K */
#define PORT_TX_MAX_DESCRIPTORS_NUM_CNS     0x1000

/* Default valid value for port id */
#define PORT_TX_VALID_PHY_PORT_CNS          0

/* Invalid enumeration value used for testing */
#define PORT_TX_INVALID_ENUM_CNS            0x5AAAAAA5

/* Traffic class value which is used as default valid value for testing */
#define PORT_TX_TESTED_TRAFFIC_CLASS_CNS    0

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxQueueEnable
(
    IN  GT_U8     dev,
    IN  GT_BOOL   enable
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxQueueEnable)
{
/*
ITERATE_DEVICES
1.1. Call function with enable [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     enable;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable [GT_TRUE and GT_FALSE].   */
        /* Expected: GT_OK.                                         */
        enable = GT_TRUE;

        st = cpssExMxPortTxQueueEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_FALSE;

        st = cpssExMxPortTxQueueEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    enable = GT_FALSE;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPortTxQueueEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxQueueEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxQEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_U8     queue,
    IN  GT_BOOL   enable
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxQEnable)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call function with queue [0] and enable [GT_TRUE and GT_FALSE]. Expected: GT_OK.
1.1.2. Call function with out of range queue [CPSS_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   queue;
    GT_BOOL enable;

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
            /* 1.1.1. Call function with queue [0] and enable [GT_TRUE and GT_FALSE].   */
            /* Expected: GT_OK.                                                         */
            queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
            enable = GT_TRUE;

            st = cpssExMxPortTxQEnable(dev, port, queue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, queue, enable);

            enable = GT_FALSE;

            st = cpssExMxPortTxQEnable(dev, port, queue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, queue, enable);

            /* 1.1.2. Call function with out of range queue [CPSS_TC_RANGE_CNS].        */
            /* Expected: GT_BAD_PARAM.                                                  */
            queue = CPSS_TC_RANGE_CNS;

            st = cpssExMxPortTxQEnable(dev, port, queue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, queue, enable);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
        enable = GT_TRUE;

        st = cpssExMxPortTxQEnable(dev, port, queue, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st ,dev, port, queue, enable);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTxQEnable(dev, port, queue, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTxQEnable(dev, port, queue, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;
    queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
    enable = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTxQEnable(dev, port, queue, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxQEnable(dev, port, queue, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxQTxEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_U8     queue,
    IN  GT_BOOL   enable
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxQTxEnable)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call function with queue [0] and enable [GT_TRUE and GT_FALSE]. Expected: GT_OK.
1.1.2. Call function with out of range queue [CPSS_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   queue;
    GT_BOOL enable;

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
            /* 1.1.1. Call function with queue [0] and enable [GT_TRUE and GT_FALSE].   */
            /* Expected: GT_OK.                                                         */
            queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
            enable = GT_TRUE;

            st = cpssExMxPortTxQTxEnable(dev, port, queue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, queue, enable);

            enable = GT_FALSE;

            st = cpssExMxPortTxQTxEnable(dev, port, queue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, queue, enable);

            /* 1.1.2. Call function with out of range queue [CPSS_TC_RANGE_CNS].        */
            /* Expected: GT_BAD_PARAM.                                                  */
            queue  = CPSS_TC_RANGE_CNS;

            st = cpssExMxPortTxQTxEnable(dev, port, queue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, queue, enable);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
        enable = GT_TRUE;

        st = cpssExMxPortTxQTxEnable(dev, port, queue, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, queue, enable);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTxQTxEnable(dev, port, queue, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTxQTxEnable(dev, port, queue, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;
    queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
    enable = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTxQTxEnable(dev, port, queue, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxQTxEnable(dev, port, queue, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxFlushQueues
(
    IN  GT_U8     dev,
    IN  GT_U8     port
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxFlushQueues)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call function. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;

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
            /* 1.1.1. Call function.    */
            /* Expected: GT_OK.         */
            st = cpssExMxPortTxFlushQueues(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssExMxPortTxFlushQueues(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTxFlushQueues(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTxFlushQueues(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTxFlushQueues(dev, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxFlushQueues(dev, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxShaperEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_BOOL   enable
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxShaperEnable)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1. 1. Call function with enable [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL enable;

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
            /* 1.1. 1. Call function with enable [GT_TRUE and GT_FALSE].    */
            /* Expected: GT_OK.                                             */
            enable = GT_TRUE;

            st = cpssExMxPortTxShaperEnable(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            enable = GT_FALSE;

            st = cpssExMxPortTxShaperEnable(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        enable = GT_TRUE;

        st = cpssExMxPortTxShaperEnable(dev, port, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTxShaperEnable(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTxShaperEnable(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;
    enable = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTxShaperEnable(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxShaperEnable(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxShaperProfileSet
(
    IN    GT_U8     dev,
    IN    GT_U8     port,
    IN    GT_U16    burstSize,
    INOUT GT_U32    *maxRatePtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxShaperProfileSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call function with non-NULL maxRatePtr [maxRate=100] and burstSize [16]. Expected: GT_OK.
1.1.2. Call with out of range burstSize [0x1000]. Expected: NON GT_OK.
1.1.3. Call function with maxRatePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U16  burstSize;
    GT_U32  maxRate;

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
            /* 1.1.1. Call function with non-NULL maxRatePtr [maxRate=100] and burstSize [16].  */
            /* Expected: GT_OK.                                                                 */
            burstSize = 16;
            maxRate = 100;

            st = cpssExMxPortTxShaperProfileSet(dev, port, burstSize, &maxRate);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, burstSize, maxRate);

            /* 1.1.2. Call with out of range burstSize [0x1000].                                */
            /* Expected: NON GT_OK.                                                             */
            burstSize = PORT_TX_MAX_BURST_SIZE_CNS;
            maxRate = 100;

            st = cpssExMxPortTxShaperProfileSet(dev, port, burstSize, &maxRate);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, burstSize, maxRate);

            /* 1.1.3. Call function with maxRatePtr [NULL].                                     */
            /* Expected: GT_BAD_PTR.                                                            */
            burstSize = 16;

            st = cpssExMxPortTxShaperProfileSet(dev, port, burstSize, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL", dev, port, burstSize);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        burstSize = 16;
        maxRate = 100;

        st = cpssExMxPortTxShaperProfileSet(dev, port, burstSize, &maxRate);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, burstSize, maxRate);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            burstSize = 16;
            maxRate = 100;

            st = cpssExMxPortTxShaperProfileSet(dev, port, burstSize, &maxRate);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        burstSize = 16;
        maxRate = 100;

        st = cpssExMxPortTxShaperProfileSet(dev, port, burstSize, &maxRate);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        burstSize = 16;
        maxRate = 100;

        st = cpssExMxPortTxShaperProfileSet(dev, port, burstSize, &maxRate);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    burstSize = 16;
    maxRate = 100;

    st = cpssExMxPortTxShaperProfileSet(dev, port, burstSize, &maxRate);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxQShaperEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_U8     queue,
    IN  GT_BOOL   enable
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxQShaperEnable)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call function with queue [0] and enable [GT_TRUE and GT_FALSE]. Expected: GT_OK.
1.1.2. Call function with out of range queue [CPSS_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   queue;
    GT_BOOL enable;


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
            /* 1.1.1. Call function with queue [0] and enable [GT_TRUE and GT_FALSE].   */
            /* Expected: GT_OK.                                                         */
            queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
            enable = GT_TRUE;

            st = cpssExMxPortTxQShaperEnable(dev, port, queue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, queue, enable);

            enable = GT_FALSE;

            st = cpssExMxPortTxQShaperEnable(dev, port, queue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, queue, enable);

            /* 1.1.2. Call function with out of range queue [CPSS_TC_RANGE_CNS].        */
            /* Expected: GT_BAD_PARAM.                                                  */
            queue = CPSS_TC_RANGE_CNS;

            st = cpssExMxPortTxQShaperEnable(dev, port, queue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, queue, enable);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
        enable = GT_TRUE;

        st = cpssExMxPortTxQShaperEnable(dev, port, queue, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, queue, enable);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTxQShaperEnable(dev, port, queue, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTxQShaperEnable(dev, port, queue, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;
    queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
    enable = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTxQShaperEnable(dev, port, queue, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxQShaperEnable(dev, port, queue, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxQShaperProfileSet
(
    IN    GT_U8     dev,
    IN    GT_U8     port,
    IN    GT_U8     queue,
    IN    GT_U16    burstSize,
    INOUT GT_U32    *userRatePtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxQShaperProfileSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call function with non-NULL userRatePtr [userRate=100] and burstSize [100], queue [0]. Expected: GT_OK.
1.1.2. Call function with out of range queue [CPSS_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
1.1.3. Call with out of range burstSize [0x1000]. Expected: NON GT_OK.
1.1.4. Call function with maxRatePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   queue;
    GT_U16  burstSize;
    GT_U32  userRate;

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
            /* 1.1.1. Call function with non-NULL userRatePtr [userRate=100] and burstSize [100], queue [0].    */
            /* Expected: GT_OK.                                                                                 */
            queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
            burstSize = 100;
            userRate = 100;

            st = cpssExMxPortTxQShaperProfileSet(dev, port, queue, burstSize, &userRate);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, queue, burstSize, userRate);

            /* 1.1.2. Call function with out of range queue [CPSS_TC_RANGE_CNS].                                */
            /* Expected: GT_BAD_PARAM.                                                                          */
            queue = CPSS_TC_RANGE_CNS;
            userRate = 100;

            st = cpssExMxPortTxQShaperProfileSet(dev, port, queue, burstSize, &userRate);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, port, queue, burstSize, userRate);

            /* 1.1.3. Call with out of range burstSize [0x1000].                                                */
            /* Expected: NON GT_OK.                                                                             */
            queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
            burstSize = PORT_TX_MAX_BURST_SIZE_CNS;
            userRate = 100;

            st = cpssExMxPortTxQShaperProfileSet(dev, port, queue, burstSize, &userRate);
            UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, queue, burstSize, userRate);

            /* 1.1.4. Call function with maxRatePtr [NULL].                                                     */
            /* Expected: GT_BAD_PTR.                                                                            */
            st = cpssExMxPortTxQShaperProfileSet(dev, port, queue, burstSize, NULL);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, %d, NULL",
                                         dev, port, queue, burstSize);

        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
        burstSize = 100;
        userRate = 100;

        st = cpssExMxPortTxQShaperProfileSet(dev, port, queue, burstSize, &userRate);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, queue, burstSize, userRate);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
            burstSize = 100;
            userRate = 100;

            st = cpssExMxPortTxQShaperProfileSet(dev, port, queue, burstSize, &userRate);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
        burstSize = 100;
        userRate = 100;

        st = cpssExMxPortTxQShaperProfileSet(dev, port, queue, burstSize, &userRate);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
        burstSize = 100;
        userRate = 100;

        st = cpssExMxPortTxQShaperProfileSet(dev, port, queue, burstSize, &userRate);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
    burstSize = 100;
    userRate = 100;

    st = cpssExMxPortTxQShaperProfileSet(dev, port, queue, burstSize, &userRate);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxQWrrProfileSet
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_U8     queue,
    IN  GT_U8     wrrWeight
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxQWrrProfileSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call function with wrrWeight [100] and queue [0]. Expected: GT_OK.
1.1.2. Call function with out of range queue [CPSS_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   queue;
    GT_U8   wrrWeight;

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
            /* 1.1.1. Call function with wrrWeight [100] and queue [0].             */
            /* Expected: GT_OK.                                                     */
            queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
            wrrWeight = 100;

            st = cpssExMxPortTxQWrrProfileSet(dev, port, queue, wrrWeight);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, queue, wrrWeight);

            /* 1.1.2. Call function with out of range queue [CPSS_TC_RANGE_CNS].    */
            /* Expected: GT_BAD_PARAM.                                              */
            queue = CPSS_TC_RANGE_CNS;

            st = cpssExMxPortTxQWrrProfileSet(dev, port, queue, wrrWeight);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, queue, wrrWeight);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
        wrrWeight = 100;

        st = cpssExMxPortTxQWrrProfileSet(dev, port, queue, wrrWeight);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, queue, wrrWeight);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTxQWrrProfileSet(dev, port, queue, wrrWeight);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTxQWrrProfileSet(dev, port, queue, wrrWeight);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;
    queue = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;
    wrrWeight = 100;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTxQWrrProfileSet(dev, port, queue, wrrWeight);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxQWrrProfileSet(dev, port, queue, wrrWeight);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxQArbGroupSet
(
    IN  GT_U8                         dev,
    IN  GT_U8                         port,
    IN  GT_U8                         queue,
    IN  CPSS_PORT_TX_Q_ARB_GROUP_ENT  arbGroup
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxQArbGroupSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call with queue [2], arbGroup [CPSS_PORT_TX_WRR_ARB_GROUP_0_E, CPSS_PORT_TX_WRR_ARB_GROUP_1_E, CPSS_PORT_TX_SP_ARB_GROUP_E]. Expected: GT_OK.
1.1.2. Call with out of range queue [CPSS_TC_RANGE_CNS], arbGroup [CPSS_PORT_TX_WRR_ARB_GROUP_0_E]/ Expected: GT_BAD_PARAM.
1.1.3. Call with queue [2], arbGroup [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                           dev;
    GT_U8                           port;
    GT_U8                           queue;
    CPSS_PORT_TX_Q_ARB_GROUP_ENT    arbGroup;


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
            /* 1.1.1. Call with queue [2],                                                                              */
            /* arbGroup [CPSS_PORT_TX_WRR_ARB_GROUP_0_E, CPSS_PORT_TX_WRR_ARB_GROUP_1_E, CPSS_PORT_TX_SP_ARB_GROUP_E].  */
            /* Expected: GT_OK.                                                                                         */
            queue = 2;
            arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;

            st = cpssExMxPortTxQArbGroupSet(dev, port, queue, arbGroup);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, queue, arbGroup);

            arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_1_E;

            st = cpssExMxPortTxQArbGroupSet(dev, port, queue, arbGroup);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, queue, arbGroup);

            arbGroup = CPSS_PORT_TX_SP_ARB_GROUP_E;

            st = cpssExMxPortTxQArbGroupSet(dev, port, queue, arbGroup);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, queue, arbGroup);

            /* 1.1.2. Call with out of range queue [CPSS_TC_RANGE_CNS], arbGroup [CPSS_PORT_TX_WRR_ARB_GROUP_0_E]       */
            /* Expected: GT_BAD_PARAM.                                                                                  */
            queue = CPSS_TC_RANGE_CNS;
            arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;

            st = cpssExMxPortTxQArbGroupSet(dev, port, queue, arbGroup);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, queue, arbGroup);

            /* 1.1.3. Call with queue [2], arbGroup [0x5AAAAAA5].                                                       */
            /* Expected: GT_BAD_PARAM.                                                                                  */
            queue = 2;
            arbGroup = PORT_TX_INVALID_ENUM_CNS;

            st = cpssExMxPortTxQArbGroupSet(dev, port, queue, arbGroup);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, queue, arbGroup);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        queue = 2;
        arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;

        st = cpssExMxPortTxQArbGroupSet(dev, port, queue, arbGroup);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, queue, arbGroup);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTxQArbGroupSet(dev, port, queue, arbGroup);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTxQArbGroupSet(dev, port, queue, arbGroup);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;
    queue = 2;
    arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTxQArbGroupSet(dev, port, queue, arbGroup);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxQArbGroupSet(dev, port, queue, arbGroup);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxDropMode
(
    IN  GT_U8                         dev,
    IN  GT_U8                         port,
    IN  CPSS_PORT_TX_Q_DROP_MODE_ENT  dropMode
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxDropMode)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call with dropMode [CPSS_PORT_TX_GRED_DROP_MODE_E and CPSS_PORT_TX_TAIL_DROP_MODE_E]. Expected: GT_OK.
1.1.2. Call with dropMode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                           dev;
    GT_U8                           port;
    CPSS_PORT_TX_Q_DROP_MODE_ENT    dropMode;

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
            /* 1.1.1. Call with dropMode [CPSS_PORT_TX_GRED_DROP_MODE_E and CPSS_PORT_TX_TAIL_DROP_MODE_E].     */
            /* Expected: GT_OK.                                                                                 */
            dropMode = CPSS_PORT_TX_GRED_DROP_MODE_E;

            st = cpssExMxPortTxDropMode(dev, port, dropMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, dropMode);

            dropMode = CPSS_PORT_TX_TAIL_DROP_MODE_E;

            st = cpssExMxPortTxDropMode(dev, port, dropMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, dropMode);

            /* 1.1.2. Call with dropMode [0x5AAAAAA5].                                                          */
            /* Expected: GT_BAD_PARAM.                                                                          */
            dropMode = PORT_TX_INVALID_ENUM_CNS;

            st = cpssExMxPortTxDropMode(dev, port, dropMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, dropMode);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        dropMode = CPSS_PORT_TX_GRED_DROP_MODE_E;

        st = cpssExMxPortTxDropMode(dev, port, dropMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, dropMode);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTxDropMode(dev, port, dropMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTxDropMode(dev, port, dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;
    dropMode = CPSS_PORT_TX_GRED_DROP_MODE_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTxDropMode(dev, port, dropMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxDropMode(dev, port, dropMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxTcDpProfileSet
(
    IN  GT_U8                                  dev,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT      pfSet,
    IN  GT_U8                                  tc,
    IN  CPSS_PORT_TX_Q_DROP_PROFILE_PARAMS_STC *dropProfileParams
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxTcDpProfileSet)
{
/*
ITERATE_DEVICES
1.1. Call with pfSet [CPSS_PORT_TX_DROP_PROFILE_1/2/3/4_E], tc [2], dropProfileParams [10,10,10,100,100,100,5,5,5,9]. Expected: GT_OK.
1.2. Call with pfSet [0x5AAAAAA5], tc [2], dropProfileParams [10,10,10,100,100,100,5,5,5,9]. Expected: GT_BAD_PARAM.
1.3. Call with pfSet [CPSS_PORT_TX_DROP_PROFILE_1_E], tc [CPSS_TC_RANGE_CNS], dropProfileParams [10,10,10,100,100,100,5,5,5,9]. Expected: GT_BAD_PARAM.
1.4. Call with pfSet [CPSS_PORT_TX_DROP_PROFILE_1_E], tc [2], dropProfileParams [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                                   dev;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT       pfSet;
    GT_U8                                   tc;
    CPSS_PORT_TX_Q_DROP_PROFILE_PARAMS_STC  dropProfileParams;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* fill drop profile with default legal values */
    dropProfileParams.dp0MinThreshold = 10;
    dropProfileParams.dp1MinThreshold = 10;
    dropProfileParams.dp2MinThreshold = 10;
    dropProfileParams.dp0MaxThreshold = 100;
    dropProfileParams.dp1MaxThreshold = 100;
    dropProfileParams.dp2MaxThreshold = 100;
    dropProfileParams.dp0MaxProb = 5;
    dropProfileParams.dp1MaxProb = 5;
    dropProfileParams.dp2MaxProb = 5;
    dropProfileParams.RedQWeightFactor = 9;

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with pfSet [CPSS_PORT_TX_DROP_PROFILE_1/2/3/4_E], tc [2],                          */
        /* dropProfileParams [10,10,10,100,100,100,5,5,5,9].                                            */
        /* Expected: GT_OK.                                                                             */
        pfSet = CPSS_PORT_TX_DROP_PROFILE_1_E;
        tc = 2;

        st = cpssExMxPortTxTcDpProfileSet(dev, pfSet, tc, &dropProfileParams);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, pfSet, tc);

        pfSet = CPSS_PORT_TX_DROP_PROFILE_2_E;

        st = cpssExMxPortTxTcDpProfileSet(dev, pfSet, tc, &dropProfileParams);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, pfSet, tc);

        pfSet = CPSS_PORT_TX_DROP_PROFILE_3_E;

        st = cpssExMxPortTxTcDpProfileSet(dev, pfSet, tc, &dropProfileParams);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, pfSet, tc);

        pfSet = CPSS_PORT_TX_DROP_PROFILE_4_E;

        st = cpssExMxPortTxTcDpProfileSet(dev, pfSet, tc, &dropProfileParams);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, pfSet, tc);

        /* 1.2. Call with pfSet [0x5AAAAAA5], tc [2], dropProfileParams [10,10,10,100,100,100,5,5,5,9]. */
        /* Expected: GT_BAD_PARAM.                                                                      */
        pfSet = PORT_TX_INVALID_ENUM_CNS;

        st = cpssExMxPortTxTcDpProfileSet(dev, pfSet, tc, &dropProfileParams);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, pfSet, tc);

        /* 1.3. Call with pfSet [CPSS_PORT_TX_DROP_PROFILE_1_E], tc [CPSS_TC_RANGE_CNS],                */
        /* dropProfileParams [10,10,10,100,100,100,5,5,5,9].                                            */
        /* Expected: GT_BAD_PARAM.                                                                      */
        pfSet = CPSS_PORT_TX_DROP_PROFILE_1_E;
        tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPortTxTcDpProfileSet(dev, pfSet, tc, &dropProfileParams);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, pfSet, tc);

        /* 1.4. Call with pfSet [CPSS_PORT_TX_DROP_PROFILE_1_E], tc [2], dropProfileParams [NULL].      */
        /* Expected: GT_BAD_PTR.                                                                        */
        pfSet = CPSS_PORT_TX_DROP_PROFILE_1_E;
        tc = 2;

        st = cpssExMxPortTxTcDpProfileSet(dev, pfSet, tc, NULL);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL", dev, pfSet, tc);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    pfSet = CPSS_PORT_TX_DROP_PROFILE_1_E;
    tc = 2;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPortTxTcDpProfileSet(dev, pfSet, tc, &dropProfileParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxTcDpProfileSet(dev, pfSet, tc, &dropProfileParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxBindPortToDpSet
(
    IN  GT_U8                              dev,
    IN  GT_U8                              port,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT  profileSet
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxBindPortToDpSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call with profileSet [CPSS_PORT_TX_DROP_PROFILE_1/2/3/4_E]. Expected: GT_OK.
1.1.2. Call with profileSet [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                               dev;
    GT_U8                               port;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet;

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
            /* 1.1.1. Call with profileSet [CPSS_PORT_TX_DROP_PROFILE_1/2/3/4_E].   */
            /* Expected: GT_OK.                                                     */
            profileSet = CPSS_PORT_TX_DROP_PROFILE_1_E;

            st = cpssExMxPortTxBindPortToDpSet(dev, port, profileSet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileSet);

            profileSet = CPSS_PORT_TX_DROP_PROFILE_2_E;

            st = cpssExMxPortTxBindPortToDpSet(dev, port, profileSet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileSet);

            profileSet = CPSS_PORT_TX_DROP_PROFILE_3_E;

            st = cpssExMxPortTxBindPortToDpSet(dev, port, profileSet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileSet);

            profileSet = CPSS_PORT_TX_DROP_PROFILE_4_E;

            st = cpssExMxPortTxBindPortToDpSet(dev, port, profileSet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileSet);

            /* 1.1.2. Call with profileSet [0x5AAAAAA5].                            */
            /* Expected: GT_BAD_PARAM.                                              */
            profileSet = PORT_TX_INVALID_ENUM_CNS;

            st = cpssExMxPortTxBindPortToDpSet(dev, port, profileSet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, profileSet);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>.*/
        profileSet = CPSS_PORT_TX_DROP_PROFILE_1_E;

        st = cpssExMxPortTxBindPortToDpSet(dev, port, profileSet);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileSet);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTxBindPortToDpSet(dev, port, profileSet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTxBindPortToDpSet(dev, port, profileSet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;
    profileSet = CPSS_PORT_TX_DROP_PROFILE_1_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTxBindPortToDpSet(dev, port, profileSet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxBindPortToDpSet(dev, port, profileSet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxMaxDescNumberSet
(
    IN  GT_U8       dev,
    IN  GT_U8       port,
    IN  GT_U16      number
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxMaxDescNumberSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call with number [350]. Expected: GT_OK
1.1.2. Call cpssExMxPortTxMaxDescNumberGet non-null maxDescNumPtr. Expected: GT_OK and the same maxDescNumPtr [4].
1.1.3. Call with number [0]. Expected: NON GT_OK.
1.1.4. Call with number [4096]. Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U16  number;

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
            GT_U16 numberWritten;

            /* 1.1.1. Call with number [350].                                         */
            /* Expected: GT_OK                                                      */
            number = 350;

            st = cpssExMxPortTxMaxDescNumberSet(dev, port, number);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);

            /* 1.1.2. Call cpssExMxPortTxMaxDescNumberGet non-null maxDescNumPtr.   */
            /* Expected: GT_OK and the same maxDescNumPtr [4].                      */
            st = cpssExMxPortTxMaxDescNumberGet(dev, port, &numberWritten);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPortTxMaxDescNumberGet: %d, %d, %d", dev, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(number, numberWritten, dev, port, number);

            /* 1.1.3. Call with number [0].                                         */
            /* Expected: GT_OK.                                                 */
            number = 0;

            st = cpssExMxPortTxMaxDescNumberSet(dev, port, number);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);

            /* 1.1.4. Call with number [4096].                                        */
            /* Expected: NON GT_OK.                                                 */
            number = PORT_TX_MAX_DESCRIPTORS_NUM_CNS;

            st = cpssExMxPortTxMaxDescNumberSet(dev, port, number);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        number = 4000;

        st = cpssExMxPortTxMaxDescNumberSet(dev, port, number);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTxMaxDescNumberSet(dev, port, number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTxMaxDescNumberSet(dev, port, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;
    number = 4;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTxMaxDescNumberSet(dev, port, number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxMaxDescNumberSet(dev, port, number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxDescNumberGet
(
    IN  GT_U8       dev,
    IN  GT_U8       port,
    OUT GT_U16      *numberPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxDescNumberGet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call with non-null numberPtr. Expected: GT_OK.
1.1.2. Call with numberPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U16  number;

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
            /* 1.1.1. Call with non-null numberPtr.     */
            /* Expected: GT_OK.                         */
            st = cpssExMxPortTxDescNumberGet(dev, port, &number);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);

            /* 1.1.2. Call with numberPtr [NULL].       */
            /* Expected: GT_BAD_PTR.                    */
            st = cpssExMxPortTxDescNumberGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssExMxPortTxDescNumberGet(dev, port, &number);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTxDescNumberGet(dev, port, &number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTxDescNumberGet(dev, port, &number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTxDescNumberGet(dev, port, &number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxDescNumberGet(dev, port, &number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTx4TcMaxDescNumberSet
(
    IN  GT_U8       dev,
    IN  GT_U8       port,
    IN  GT_U8       trafClass,
    IN  GT_U16      number
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTx4TcMaxDescNumberSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call with trafClass [5], number [4000]. Expected: GT_OK.
1.1.2. Call cpssExMxPortTx4TcMaxDescNumberGet with tc [5] and non-null maxDescNumPtr. Expected: GT_OK and the same maxDescNumPtr [4].
1.1.3. Call with out of range trafClass [CPSS_TC_RANGE_CNS], number [4]. Expected: GT_BAD_PARAM.
1.1.4. Call with trafClass [5], number [0]. Expected: GT_OK.
1.1.5. Call with trafClass [5], number [4096]. Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   trafClass;
    GT_U16  number;

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
            GT_U16  numberWritten;

            /* 1.1.1. Call with trafClass [5], number [4000].                                              */
            /*  Expected: GT_OK.                                                                        */
            trafClass = 5;
            number = 4000;

            st = cpssExMxPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trafClass, number);

            /* 1.1.2. Call cpssExMxPortTx4TcMaxDescNumberGet with tc [5] and non-null maxDescNumPtr.    */
            /* Expected: GT_OK and the same maxDescNumPtr [4].                                          */
            trafClass = 5;

            st = cpssExMxPortTx4TcMaxDescNumberGet(dev, port, trafClass, &numberWritten);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPortTx4TcMaxDescNumberGet: %d, %d, %d",
                                         dev, port, trafClass);
            UTF_VERIFY_EQUAL4_PARAM_MAC(number, numberWritten, dev, port, trafClass, number);

            /* 1.1.3. Call with out of range trafClass [CPSS_TC_RANGE_CNS], number [4].                 */
            /* Expected: GT_BAD_PARAM.                                                                  */
            trafClass = CPSS_TC_RANGE_CNS;
            number = 4;

            st = cpssExMxPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, trafClass, number);

            /* 1.1.4. Call with trafClass [5], number [0].                                              */
            /* Expected: NON GT_OK.                                                                     */
            trafClass = 5;
            number = 0;

            st = cpssExMxPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trafClass, number);

            /* 1.1.5. Call with trafClass [5], number [4096].                                             */
            /* Expected: NON GT_OK.                                                                     */
            trafClass = 5;
            number = PORT_TX_MAX_DESCRIPTORS_NUM_CNS;

            st = cpssExMxPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trafClass, number);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        trafClass = 5;
        number = 4;

        st = cpssExMxPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trafClass, number);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;
    trafClass = 5;
    number = 4;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTx4TcDescNumberGet
(
    IN  GT_U8       dev,
    IN  GT_U8       port,
    IN  GT_U8       trafClass,
    OUT GT_U16      *numberPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTx4TcDescNumberGet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call with trafClass [0], non-null numberPtr. Expected: GT_OK.
1.1.2. Call with trafClass [CPSS_TC_RANGE_CNS], non-null numberPtr. Expected: GT_BAD_PARAM.
1.1.3. Call with trafClass [0], numberPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   trafClass;
    GT_U16  number;

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
            /* 1.1.1. Call with trafClass [0], non-null numberPtr.                  */
            /* Expected: GT_OK.                                                     */
            trafClass = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;

            st = cpssExMxPortTx4TcDescNumberGet(dev, port, trafClass, &number);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trafClass, number);

            /* 1.1.2. Call with trafClass [CPSS_TC_RANGE_CNS], non-null numberPtr.  */
            /* Expected: GT_BAD_PARAM.                                              */
            trafClass = CPSS_TC_RANGE_CNS;

            st = cpssExMxPortTx4TcDescNumberGet(dev, port, trafClass, &number);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, trafClass, number);

            /* 1.1.3. Call with trafClass [0], numberPtr [NULL].                    */
            /* Expected: GT_BAD_PTR.                                                */
            trafClass = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;

            st = cpssExMxPortTx4TcDescNumberGet(dev, port, trafClass, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL", dev, port, trafClass);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        trafClass = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;

        st = cpssExMxPortTx4TcDescNumberGet(dev, port, trafClass, &number);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trafClass, number);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTx4TcDescNumberGet(dev, port, trafClass, &number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTx4TcDescNumberGet(dev, port, trafClass, &number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;
    trafClass = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTx4TcDescNumberGet(dev, port, trafClass, &number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTx4TcDescNumberGet(dev, port, trafClass, &number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTx4TcMaxDescNumberGet
(
    IN  GT_U8       dev,
    IN  GT_U8       port,
    IN  GT_U8       tc,
    OUT GT_U16      *maxDescNumPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTx4TcMaxDescNumberGet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call with tc [0], non-null maxDescNumPtr. Expected: GT_OK.
1.1.2. Call with tc [CPSS_TC_RANGE_CNS], non-null maxDescNumPtr. Expected: GT_BAD_PARAM.
1.1.3. Call with tc [0], maxDescNumPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   tc;
    GT_U16  maxDescNum;

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
            /* 1.1.1. Call with tc [0], non-null maxDescNumPtr.                         */
            /* Expected: GT_OK.                                                         */
            tc = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;

            st = cpssExMxPortTx4TcMaxDescNumberGet(dev, port, tc, &maxDescNum);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, maxDescNum);

            /* 1.1.2. Call with tc [CPSS_TC_RANGE_CNS], non-null maxDescNumPtr.         */
            /* Expected: GT_BAD_PARAM.                                                  */
            tc = CPSS_TC_RANGE_CNS;

            st = cpssExMxPortTx4TcMaxDescNumberGet(dev, port, tc, &maxDescNum);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, tc, maxDescNum);

            /* 1.1.3. Call with tc [0], maxDescNumPtr [NULL].                           */
            /* Expected: GT_BAD_PTR.                                                    */
            tc = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;

            st = cpssExMxPortTx4TcMaxDescNumberGet(dev, port, tc, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL", dev, port, tc);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        tc = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;

        st = cpssExMxPortTx4TcMaxDescNumberGet(dev, port, tc, &maxDescNum);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, maxDescNum);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTx4TcMaxDescNumberGet(dev, port, tc, &maxDescNum);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTx4TcMaxDescNumberGet(dev, port, tc, &maxDescNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;
    tc = PORT_TX_TESTED_TRAFFIC_CLASS_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTx4TcMaxDescNumberGet(dev, port, tc, &maxDescNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTx4TcMaxDescNumberGet(dev, port, tc, &maxDescNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxMaxDescNumberGet
(
    IN  GT_U8       dev,
    IN  GT_U8       port,
    OUT GT_U16      *maxDescNumPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxMaxDescNumberGet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call with non-null maxDescNumPtr. Expected: GT_OK.
1.1.2. Call with maxDescNumPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U16  maxDescNum;

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
            /* 1.1.1. Call with non-null maxDescNumPtr.     */
            /* Expected: GT_OK.                             */
            st = cpssExMxPortTxMaxDescNumberGet(dev, port, &maxDescNum);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, maxDescNum);

            /* 1.1.2. Call with maxDescNumPtr [NULL].       */
            /* Expected: GT_BAD_PTR.                        */
            st = cpssExMxPortTxMaxDescNumberGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssExMxPortTxMaxDescNumberGet(dev, port, &maxDescNum);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, maxDescNum);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTxMaxDescNumberGet(dev, port, &maxDescNum);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTxMaxDescNumberGet(dev, port, &maxDescNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTxMaxDescNumberGet(dev, port, &maxDescNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxMaxDescNumberGet(dev, port, &maxDescNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxWatchdogGet
(
    IN  GT_U8       dev,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr,
    OUT GT_U16      *timeoutPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxWatchdogGet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORT
1.1.1. Call with non-null enablePtr, non-null timeoutPtr. Expected: GT_OK.
1.1.2. Call with enablePtr [NULL], non-null timeoutPtr. Expected: GT_BAD_PTR.
1.1.3. Call with non-null enablePtr, timeoutPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     enable;
    GT_U16      timeout;

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
            /* 1.1.1. Call with non-null enablePtr, non-null timeoutPtr.    */
            /* Expected: GT_OK.                                             */
            st = cpssExMxPortTxWatchdogGet(dev, port, &enable, &timeout);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, timeout);

            /* 1.1.2. Call with enablePtr [NULL], non-null timeoutPtr.      */
            /* Expected: GT_BAD_PTR.                                        */
            st = cpssExMxPortTxWatchdogGet(dev, port, NULL, &timeout);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL, %d", dev, port, timeout);

            /* 1.1.3. Call with non-null enablePtr, timeoutPtr [NULL].      */
            /* Expected: GT_BAD_PTR.                                        */
            st = cpssExMxPortTxWatchdogGet(dev, port, &enable, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL", dev, port, enable);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssExMxPortTxWatchdogGet(dev, port, &enable, &timeout);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, timeout);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTxWatchdogGet(dev, port, &enable, &timeout);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTxWatchdogGet(dev, port, &enable, &timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTxWatchdogGet(dev, port, &enable, &timeout);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxWatchdogGet(dev, port, &enable, &timeout);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortTxWatchdogEnable
(
    IN  GT_U8     dev,
    IN  GT_U8     port,
    IN  GT_BOOL   enable,
    IN  GT_U16    timeout
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortTxWatchdogEnable)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call with enable [GT_TRUE and GT_FALSE], timeout [1024]. Expected: GT_OK.
1.1.2. Call cpssExMxPortTxWatchdogGet with non-null enablePtr, non-null timeoutPtr. Expected: GT_OK and the same enable and timeout as written.
1.1.3. Call with enable [GT_TRUE], timeout [0. Expected: NON GT_OK.
1.1.4. Call with enable [GT_TRUE], timeout [4096]. Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     enable;
    GT_U16      timeout;

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
            GT_BOOL     enableWritten;
            GT_U16      timeoutWritten;

            /* 1.1.1. Call with enable [GT_TRUE and GT_FALSE], timeout [1024].                      */
            /* Expected: GT_OK.                                                                     */
            /* 1.1.2. Call cpssExMxPortTxWatchdogGet with non-null enablePtr, non-null timeoutPtr.  */
            /* Expected: GT_OK and the same enable and timeout as written.                          */
            enable = GT_TRUE;
            timeout = 1024;

            st = cpssExMxPortTxWatchdogEnable(dev, port, enable, timeout);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, timeout);

            st = cpssExMxPortTxWatchdogGet(dev, port, &enableWritten, &timeoutWritten);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPortTxWatchdogGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL4_PARAM_MAC(enable, enableWritten, dev, port, enable, timeout);
            UTF_VERIFY_EQUAL4_PARAM_MAC(timeout, timeoutWritten, dev, port, enable, timeout);

            enable = GT_FALSE;
            timeout = 1000;

            st = cpssExMxPortTxWatchdogEnable(dev, port, enable, timeout);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, timeout);

            st = cpssExMxPortTxWatchdogGet(dev, port, &enableWritten, &timeoutWritten);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPortTxWatchdogGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL4_PARAM_MAC(enable, enableWritten, dev, port, enable, timeout);
            /* when feature disabled there is no meaning in parameter timeOut !!!
            UTF_VERIFY_EQUAL4_PARAM_MAC(timeout, timeoutWritten, dev, port, enable, timeout);
            */

            /* 1.1.3. Call with enable [GT_TRUE], timeout [0]                                       */
            /* Expected: NON GT_OK.                                                                 */
            enable = GT_TRUE;
            timeout = 0;

            st = cpssExMxPortTxWatchdogEnable(dev, port, enable, timeout);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, timeout);

            /* 1.1.4. Call with enable [GT_TRUE], timeout [4096].                                   */
            /* Expected: NON GT_OK.                                                                 */
            enable = GT_TRUE;
            timeout = 4096;

            st = cpssExMxPortTxWatchdogEnable(dev, port, enable, timeout);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, timeout);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        enable = GT_TRUE;
        timeout = 1024;

        st = cpssExMxPortTxWatchdogEnable(dev, port, enable, timeout);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, timeout);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortTxWatchdogEnable(dev, port, enable, timeout);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortTxWatchdogEnable(dev, port, enable, timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_TX_VALID_PHY_PORT_CNS;
    enable = GT_TRUE;
    timeout = 1024;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortTxWatchdogEnable(dev, port, enable, timeout);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortTxWatchdogEnable(dev, port, enable, timeout);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxPortTx suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPortTx)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxQueueEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxQEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxQTxEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxFlushQueues)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxShaperEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxShaperProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxQShaperEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxQShaperProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxQWrrProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxQArbGroupSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxDropMode)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxTcDpProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxBindPortToDpSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxMaxDescNumberSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxDescNumberGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTx4TcMaxDescNumberSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTx4TcDescNumberGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTx4TcMaxDescNumberGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxMaxDescNumberGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxWatchdogGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortTxWatchdogEnable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPortTx)


