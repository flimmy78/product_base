/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxSalPortTxUT.c
*
* DESCRIPTION:
*       Unit tests for Port configuration and control facility.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxSal/port/cpssDxSalPortTx.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Invalid enum */
#define CPSS_UT_INVALID_ENUM_CNS        0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTxQueueEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTxQueueEnableSet)
{
/*
ITERATE_DEVICES (DxSal)
1.1. Call with enable [GT_FALSE and GT_TRUE]. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_BOOL                 enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with enable [GT_FALSE and GT_TRUE]. Expected: GT_OK.*/

        enable = GT_FALSE;

        st = cpssDxSalPortTxQueueEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_TRUE;

        st = cpssDxSalPortTxQueueEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTxQueueEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTxQueueEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTxQueueingEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTxQueueingEnableSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS (DxSal)
1.1.1. Call with enable [GT_FALSE and GT_TRUE] and tcQueue [0 / 1 / 2 / 3]. 
Expected: GT_OK.
1.1.2. Call with enable [GT_FALSE and GT_TRUE] and out of range tcQueue [CPSS_4_TC_RANGE_CNS]=4
Expected: GT_BAD_PARAM.
1.1.3. Call cpssDxSalPortTxFlushQueuesSet to invalidate all changes.
Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U8       tcQueue;
    GT_BOOL     enable;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with enable [GT_FALSE and GT_TRUE] and tcQueue
               [0 / 1 / 2 / 3].  Expected: GT_OK.   */

            enable = GT_FALSE;
            tcQueue = 0;

            st = cpssDxSalPortTxQueueingEnableSet(dev, port, tcQueue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tcQueue, enable);

            enable = GT_TRUE;
            tcQueue = 2;

            st = cpssDxSalPortTxQueueingEnableSet(dev, port, tcQueue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tcQueue, enable);

            enable = GT_FALSE;
            tcQueue = 3;

            st = cpssDxSalPortTxQueueingEnableSet(dev, port, tcQueue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tcQueue, enable);

            /* 1.1.2. Call with enable [GT_FALSE and GT_TRUE] and out of
               range tcQueue [CPSS_4_TC_RANGE_CNS]=4. Expected: GT_BAD_PARAM.*/

            enable = GT_FALSE;
            tcQueue = CPSS_4_TC_RANGE_CNS;

            st = cpssDxSalPortTxQueueingEnableSet(dev, port, tcQueue, enable);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tcQueue = %d",
                                        dev, tcQueue);

            /* 1.1.3. Call cpssDxSalPortTxFlushQueuesSet to invalidate
               all changes. Expected: GT_OK.    */

            st = cpssDxSalPortTxFlushQueuesSet(dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortTxFlushQueuesSet: %d, %d",
                                        dev, port);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;
        enable = GT_FALSE;
        tcQueue = 0;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxSalPortTxQueueingEnableSet(dev, port, tcQueue, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxSalPortTxQueueingEnableSet(dev, port, tcQueue, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTxQueueingEnableSet(dev, port, tcQueue, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;
    enable = GT_FALSE;
    tcQueue = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTxQueueingEnableSet(dev, port, tcQueue, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTxQueueingEnableSet(dev, port, tcQueue, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTxQueueTxEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTxQueueTxEnableSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS (DxSal)
1.1.1. Call with enable [GT_FALSE and GT_TRUE] and tcQueue [0 / 1 / 2 / 3]. 
Expected: GT_OK.
1.1.2. Call with enable [GT_FALSE and GT_TRUE] and out of range tcQueue [CPSS_4_TC_RANGE_CNS]=4
Expected: GT_BAD_PARAM.
1.1.3. Call cpssDxSalPortTxFlushQueuesSet to invalidate all changes.
Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     enable;
    GT_U8       tcQueue;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with enable [GT_FALSE and GT_TRUE] and tcQueue
               [0 / 1 / 2 / 3].  Expected: GT_OK.   */

            enable = GT_FALSE;
            tcQueue = 0;

            st = cpssDxSalPortTxQueueTxEnableSet(dev, port, tcQueue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tcQueue, enable);

            enable = GT_TRUE;
            tcQueue = 2;

            st = cpssDxSalPortTxQueueTxEnableSet(dev, port, tcQueue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tcQueue, enable);

            enable = GT_FALSE;
            tcQueue = 3;

            st = cpssDxSalPortTxQueueTxEnableSet(dev, port, tcQueue, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tcQueue, enable);

            /* 1.1.2. Call with enable [GT_FALSE and GT_TRUE] and out of
               range tcQueue [CPSS_4_TC_RANGE_CNS]=4. Expected: GT_BAD_PARAM.*/

            enable = GT_FALSE;
            tcQueue = CPSS_4_TC_RANGE_CNS;

            st = cpssDxSalPortTxQueueTxEnableSet(dev, port, tcQueue, enable);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tcQueue = %d",
                                        dev, tcQueue);

            /* 1.1.3. Call cpssDxSalPortTxFlushQueuesSet to invalidate
               all changes. Expected: GT_OK.    */

            st = cpssDxSalPortTxFlushQueuesSet(dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalPortTxFlushQueuesSet: %d, %d",
                                        dev, port);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;
        enable = GT_FALSE;
        tcQueue = 0;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxSalPortTxQueueTxEnableSet(dev, port, tcQueue, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxSalPortTxQueueTxEnableSet(dev, port, tcQueue, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTxQueueTxEnableSet(dev, port, tcQueue, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;
    enable = GT_FALSE;
    tcQueue = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTxQueueTxEnableSet(dev, port, tcQueue, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTxQueueTxEnableSet(dev, port, tcQueue, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTxFlushQueuesSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTxFlushQueuesSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS (DxSal)
1.1.1. Call cpssDxSalPortTxQueueingEnableSet with enable [GT_TRUE] and tcQueue [0]. 
Expected: GT_OK.
1.1.2. Call cpssDxSalPortTxQueueTxEnableSet with enable [GT_TRUE] and tcQueue [0]. 
Expected: GT_OK.
1.1.3. Call function to invalidate changes.
Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     enable;
    GT_U8       tcQueue;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call cpssDxSalPortTxQueueingEnableSet with enable [GT_TRUE]
               and tcQueue [0]. Expected: GT_OK.    */

            enable = GT_TRUE;
            tcQueue = 0;

            st = cpssDxSalPortTxQueueingEnableSet(dev, port, tcQueue, enable);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, 
              "cpssDxSalPortTxQueueingEnableSet: %d, %d, %d, %d", dev, port, tcQueue, enable);

            /* 1.1.2. Call cpssDxSalPortTxQueueTxEnableSet with enable
               [GT_TRUE] and tcQueue [0]. Expected: GT_OK.  */

            st = cpssDxSalPortTxQueueTxEnableSet(dev, port, tcQueue, enable);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, 
              "cpssDxSalPortTxQueueTxEnableSet: %d, %d, %d, %d", dev, port, tcQueue, enable);

            /* 1.1.3. Call function to invalidate changes. Expected: GT_OK.*/

            st = cpssDxSalPortTxFlushQueuesSet(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxSalPortTxFlushQueuesSet(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxSalPortTxFlushQueuesSet(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTxFlushQueuesSet(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;
    st = cpssDxSalPortTxFlushQueuesSet(dev, port);

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTxFlushQueuesSet(dev, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTxFlushQueuesSet(dev, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTxQWrrProfileSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    IN  GT_U8     wrrWeight
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTxQWrrProfileSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS (DxSal)
1.1.1. Call with tcQueue [0 / 1 / 2 / 3], wrrWeight [1 / 2 / 4 / 8].
Expected: GT_OK.
1.1.2. Call with tcQueue [1], wrrWeight [10].
Expected: NOT GT_OK.
1.1.3. Call with out of range tcQueue [CPSS_4_TC_RANGE_CNS]=4 and wrrWeight [1].
Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U8       tcQueue;
    GT_U8       wrrWeight;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with tcQueue [0 / 1 / 2 / 3], wrrWeight
               [1 / 2 / 4 / 8]. Expected: GT_OK.    */

            tcQueue = 0;
            wrrWeight = 1;

            st = cpssDxSalPortTxQWrrProfileSet(dev, port, tcQueue, wrrWeight);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tcQueue, wrrWeight);

            tcQueue = 1;
            wrrWeight = 2;

            st = cpssDxSalPortTxQWrrProfileSet(dev, port, tcQueue, wrrWeight);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tcQueue, wrrWeight);

            tcQueue = 2;
            wrrWeight = 4;

            st = cpssDxSalPortTxQWrrProfileSet(dev, port, tcQueue, wrrWeight);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tcQueue, wrrWeight);

            tcQueue = 3;
            wrrWeight = 8;

            st = cpssDxSalPortTxQWrrProfileSet(dev, port, tcQueue, wrrWeight);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tcQueue, wrrWeight);

            /* 1.1.2. Call with tcQueue [1], wrrWeight [10].
               Expected: NOT GT_OK. */

            tcQueue = 1;
            wrrWeight = 10;

            st = cpssDxSalPortTxQWrrProfileSet(dev, port, tcQueue, wrrWeight);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, wrrWeight = %d",
                                             dev, wrrWeight);

            /* 1.1.3. Call with out of range tcQueue [CPSS_4_TC_RANGE_CNS]=4
               and wrrWeight [1]. Expected: GT_BAD_PARAM.   */

            tcQueue = CPSS_4_TC_RANGE_CNS;
            wrrWeight = 1;

            st = cpssDxSalPortTxQWrrProfileSet(dev, port, tcQueue, wrrWeight);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tcQueue = %d",
                                             dev, tcQueue);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;
        tcQueue = 0;
        wrrWeight = 1;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxSalPortTxQWrrProfileSet(dev, port, tcQueue, wrrWeight);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxSalPortTxQWrrProfileSet(dev, port, tcQueue, wrrWeight);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTxQWrrProfileSet(dev, port, tcQueue, wrrWeight);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;
    tcQueue = 0;
    wrrWeight = 1;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTxQWrrProfileSet(dev, port, tcQueue, wrrWeight);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTxQWrrProfileSet(dev, port, tcQueue, wrrWeight);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTxQArbGroupSet
(
    IN  GT_U8                         devNum,
    IN  GT_U8                         portNum,
    IN  GT_U8                         tcQueue,
    IN  CPSS_PORT_TX_Q_ARB_GROUP_ENT  arbGroup
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTxQArbGroupSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS (DxSal)
1.1.1. Call with tcQueue [0 / 1 / 2 / 3] and arbGroup [CPSS_PORT_TX_WRR_ARB_GROUP_0_E / CPSS_PORT_TX_WRR_ARB_GROUP_1_E / CPSS_PORT_TX_SP_ARB_GROUP_E].
Expected: GT_OK.
1.1.2. Call with out of range tcQueue [CPSS_4_TC_RANGE_CNS]=4 and arbGroup [CPSS_PORT_TX_WRR_ARB_GROUP_0_E].
Expected: GT_BAD_PARAM.
1.1.3. Call with tcQueue [0] and out of range arbGroup [0x5AAAAAA5].
Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U8                         tcQueue;
    CPSS_PORT_TX_Q_ARB_GROUP_ENT  arbGroup;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with tcQueue [0 / 1 / 2 / 3] and arbGroup
            [CPSS_PORT_TX_WRR_ARB_GROUP_0_E / CPSS_PORT_TX_WRR_ARB_GROUP_1_E /
            CPSS_PORT_TX_SP_ARB_GROUP_E]. Expected: GT_OK.   */

            tcQueue = 0;
            arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;

            st = cpssDxSalPortTxQArbGroupSet(dev, port, tcQueue, arbGroup);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tcQueue, arbGroup);

            tcQueue = 2;
            arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_1_E;

            st = cpssDxSalPortTxQArbGroupSet(dev, port, tcQueue, arbGroup);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tcQueue, arbGroup);

            tcQueue = 3;
            arbGroup = CPSS_PORT_TX_SP_ARB_GROUP_E;

            st = cpssDxSalPortTxQArbGroupSet(dev, port, tcQueue, arbGroup);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tcQueue, arbGroup);

            /* 1.1.2. Call with out of range tcQueue [CPSS_4_TC_RANGE_CNS]=4
               and arbGroup [CPSS_PORT_TX_WRR_ARB_GROUP_0_E].
               Expected: GT_BAD_PARAM.  */

            tcQueue = CPSS_4_TC_RANGE_CNS;
            arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;

            st = cpssDxSalPortTxQArbGroupSet(dev, port, tcQueue, arbGroup);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tcQueue = %d",
                                         dev, tcQueue);

            /* 1.1.3. Call with tcQueue [0] and out of range arbGroup
               [0x5AAAAAA5]. Expected: GT_BAD_PARAM.    */

            tcQueue = 0;
            arbGroup = CPSS_UT_INVALID_ENUM_CNS;

            st = cpssDxSalPortTxQArbGroupSet(dev, port, tcQueue, arbGroup);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, arbGroup = %d",
                                         dev, arbGroup);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;
        tcQueue = 0;
        arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxSalPortTxQArbGroupSet(dev, port, tcQueue, arbGroup);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxSalPortTxQArbGroupSet(dev, port, tcQueue, arbGroup);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTxQArbGroupSet(dev, port, tcQueue, arbGroup);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;
    tcQueue = 0;
    arbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTxQArbGroupSet(dev, port, tcQueue, arbGroup);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTxQArbGroupSet(dev, port, tcQueue, arbGroup);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTxMaxDescNumberSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U16      number
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTxMaxDescNumberSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS (DxSal)
1.1.1. Call with number [8]. Expected: GT_OK.
1.1.2. Call cpssDxSalPortTxDescNumberGet. Expected: GT_OK and the same number.
1.1.3. Call with out of range number [getMaxNumber(dev)]. Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U16      number;
    GT_U16      numberRet = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with number [8]. Expected: GT_OK. */

            number = 8;

            st = cpssDxSalPortTxMaxDescNumberSet(dev, port, number);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);

            /* 1.1.2. Call cpssDxSalPortTxDescNumberGet. Expected: GT_OK
               and the same number. */

            st = cpssDxSalPortTxDescNumberGet(dev, port, &numberRet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalPortTxDescNumberGet: %d", dev);

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(number, numberRet,
                 "cpssDxSalPortTxDescNumberGet: get another number than was set: dev = %d", dev);
            }

            /* 1.1.3. Call with out of range number [0xFFFF].
               Expected: NOT GT_OK. */

            number = 0xFFFF;

            st = cpssDxSalPortTxMaxDescNumberSet(dev, port, number);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, number = %d", dev, number);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;
        number = 8;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxSalPortTxMaxDescNumberSet(dev, port, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxSalPortTxMaxDescNumberSet(dev, port, number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTxMaxDescNumberSet(dev, port, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;
    number = 8;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTxMaxDescNumberSet(dev, port, number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTxMaxDescNumberSet(dev, port, number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTxDescNumberGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_U16      *numberPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTxDescNumberGet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS (DxSal)
1.1.1. Call with non-null numberPtr. Expected: GT_OK.
1.1.2. Call with null regsAddrPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U16      number = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null numberPtr. Expected: GT_OK.*/

            st = cpssDxSalPortTxDescNumberGet(dev, port, &number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*1.1.2. Call with null regsAddrPtr [NULL]. Expected: GT_BAD_PTR.*/

            st = cpssDxSalPortTxDescNumberGet(dev, port, &number);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, regsAddrPtr = NULL", dev);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxSalPortTxDescNumberGet(dev, port, &number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxSalPortTxDescNumberGet(dev, port, &number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTxDescNumberGet(dev, port, &number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTxDescNumberGet(dev, port, &number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTxDescNumberGet(dev, port, &number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTxBufNumberSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U16      number
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTxBufNumberSet)
{
/*
ITERATE_DEVICES_PHY_PORTS (DxSal)
1.1.1. Call with number [8]. Expected: GT_OK.
1.1.2. Call cpssDxSalPortTxBufNumberGet. Expected: GT_OK and the same number.
1.1.3. Call with out of range number [0xFFFF]. Expected: NOT GT_OK.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U16      number;
    GT_U16      numberRet = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with number [8]. Expected: GT_OK. */

            number = 8;

            st = cpssDxSalPortTxBufNumberSet(dev, port, number);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);

            /* 1.1.2. Call cpssDxSalPortTxBufNumberGet.
               Expected: GT_OK and the same number. */

            st = cpssDxSalPortTxBufNumberGet(dev, port, &numberRet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalPortTxBufNumberGet: %d", dev);

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(number, numberRet,
                 "cpssDxSalPortTxBufNumberGet: get another number than was set: dev = %d", dev);
            }

            /* 1.1.3. Call with out of range number [0xFFFF].
               Expected: NOT GT_OK. */

            number = 0xFFFF;

            st = cpssDxSalPortTxBufNumberSet(dev, port, number);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, number = %d",
                                             dev, number);
        }

        number = 8;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortTxBufNumberSet(dev, port, number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTxBufNumberSet(dev, port, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortTxBufNumberSet(dev, port, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;
    number = 8;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTxBufNumberSet(dev, port, number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTxBufNumberSet(dev, port, number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTxBufNumberGet
(
    IN   GT_U8       devNum,
    IN   GT_U8       portNum,
    OUT  GT_U16      *numPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTxBufNumberGet)
{
/*
ITERATE_DEVICES_PHY_PORTS (DxSal)
1.1.1. Call with non-null numPtr. Expected: GT_OK.
1.1.2. Call with numPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U16      num = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null numPtr. Expected: GT_OK. */

            st = cpssDxSalPortTxBufNumberGet(dev, port, &num);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with numPtr [NULL]. Expected: GT_BAD_PTR. */

            st = cpssDxSalPortTxBufNumberGet(dev, port, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numPtr = NULL", dev);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortTxBufNumberGet(dev, port, &num);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTxBufNumberGet(dev, port, &num);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortTxBufNumberGet(dev, port, &num);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTxBufNumberGet(dev, port, &num);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTxBufNumberGet(dev, port, &num);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTx4TcMaxDescNumberSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       trafClass,
    IN  GT_U16      number
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTx4TcMaxDescNumberSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS (DxSal)
1.1.1. Call with trafClass [0 / 1], number [1/ 4].
Expected: GT_OK.
1.1.2. Call cpssDxSalPortTx4TcDescNumberGet with trafClass [0 / 1].
Expected: GT_OK and the same number.
1.1.3. Call with out of range trafClass [CPSS_4_TC_RANGE_CNS]=4 and number [0]
Expected: GT_BAD_PARAM.
1.1.4. Call with trafClass [3] and out of range number [0xFFFF].
Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U8       trafClass;
    GT_U16      number;
    GT_U16      numberRet = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with trafClass [0 / 1], number [1/ 4].
                Expected: GT_OK. */

            trafClass = 0;
            number = 1;

            st = cpssDxSalPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trafClass, number);

            trafClass = 1;
            number = 4;

            st = cpssDxSalPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trafClass, number);

            /* 1.1.2. Call cpssDxSalPortTx4TcDescNumberGet with trafClass [0 / 1].
               Expected: GT_OK and the same number. */

            st = cpssDxSalPortTx4TcDescNumberGet(dev, port, trafClass, &numberRet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalPortTx4TcDescNumberGet: %d", dev);

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(number, numberRet,
                 "cpssDxSalPortTx4TcDescNumberGet: get another number than was set: dev = %d", dev);
            }

            /* 1.1.3. Call with out of range trafClass [CPSS_4_TC_RANGE_CNS]=4
               and number [0]. Expected: GT_BAD_PARAM.  */

            trafClass = CPSS_4_TC_RANGE_CNS;
            number = 0;

            st = cpssDxSalPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, trafClass = %d",
                                         dev, trafClass);

            /* 1.1.4. Call with trafClass [3] and out of range number [0xFFFF].
               Expected: NOT GT_OK. */

            trafClass = 3;
            number = 0xFFFF;

            st = cpssDxSalPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, number = %d",
                                             dev, number);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;
        trafClass = 0;
        number = 1;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxSalPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxSalPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;
    trafClass = 0;
    number = 1;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTx4TcDescNumberGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       trafClass,
    OUT GT_U16      *numberPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTx4TcDescNumberGet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS (DxSal)
1.1.1. Call with trafClass [0], and non-null numberPtr. Expected: GT_OK.
1.1.2. Call with out of range trafClass [CPSS_4_TC_RANGE_CNS]=4 and non-null numberPtr. Expected: GT_BAD_PARAM.
1.1.3. Call with trafClass [3], and numberPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U8       trafClass;
    GT_U16      number = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with trafClass [0], and non-null numberPtr.
               Expected: GT_OK. */

            trafClass = 0;

            st = cpssDxSalPortTx4TcDescNumberGet(dev, port, trafClass, &number);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafClass);

            /* 1.1.2. Call with out of range trafClass [CPSS_4_TC_RANGE_CNS]=4
               and non-null numberPtr. Expected: GT_BAD_PARAM.  */

            trafClass = CPSS_4_TC_RANGE_CNS;

            st = cpssDxSalPortTx4TcDescNumberGet(dev, port, trafClass, &number);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, trafClass = %d",
                                         dev, trafClass);

            /* 1.1.3. Call with trafClass [3], and numberPtr [NULL].
               Expected: GT_BAD_PTR.    */

            trafClass = 3;

            st = cpssDxSalPortTx4TcDescNumberGet(dev, port, trafClass, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, numberPtr = NULL", dev);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;
        trafClass = 0;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxSalPortTx4TcDescNumberGet(dev, port, trafClass, &number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxSalPortTx4TcDescNumberGet(dev, port, trafClass, &number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTx4TcDescNumberGet(dev, port, trafClass, &number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;
    trafClass = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTx4TcDescNumberGet(dev, port, trafClass, &number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTx4TcDescNumberGet(dev, port, trafClass, &number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTx4TcBufNumberSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       trafClass,
    IN  GT_U16      number
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTx4TcBufNumberSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS (DxSal)
1.1.1. Call with trafClass [0], number [8].
Expected: GT_OK.
1.1.2. Call cpssDxSalPortTx4TcBufNumberGet with trafClass [0].
Expected: GT_OK and the same number.
1.1.3. Call with out of range trafClass [CPSS_4_TC_RANGE_CNS]=4 and number [0]
Expected: GT_BAD_PARAM.
1.1.4. Call with trafClass [3] and out of range number [0xFFFF].
Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U8       trafClass;
    GT_U16      number;
    GT_U16      numberRet = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with trafClass [0], number [8]. Expected: GT_OK.*/

            trafClass = 0;
            number = 8;

            st = cpssDxSalPortTx4TcBufNumberSet(dev, port, trafClass, number);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, trafClass, number);

            /* 1.1.2. Call cpssDxSalPortTx4TcBufNumberGet with trafClass [0].
                        Expected: GT_OK and the same number.*/

            st = cpssDxSalPortTx4TcBufNumberGet(dev, port, trafClass, &numberRet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalPortTx4TcBufNumberGet: %d", dev);

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(number, numberRet,
                 "cpssDxSalPortTx4TcBufNumberGet: get another number than was set: dev = %d", dev);
            }

            /* 1.1.3. Call with out of range trafClass [CPSS_4_TC_RANGE_CNS]=4
               and number [0]. Expected: GT_BAD_PARAM.  */

            trafClass = CPSS_4_TC_RANGE_CNS;
            number = 0;

            st = cpssDxSalPortTx4TcBufNumberSet(dev, port, trafClass, number);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, trafClass = %d",
                                         dev, trafClass);

            /* 1.1.4. Call with trafClass [3] and out of range number [0xFFFF].
                      Expected: NOT GT_OK.  */

            trafClass = 3;
            number = 0xFFFF;

            st = cpssDxSalPortTx4TcBufNumberSet(dev, port, trafClass, number);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, number = %d",
                                         dev, number);
        }

        trafClass = 0;
        number = 8;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortTx4TcBufNumberSet(dev, port, trafClass, number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTx4TcBufNumberSet(dev, port, trafClass, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortTx4TcBufNumberSet(dev, port, trafClass, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;
    trafClass = 0;
    number = 8;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTx4TcBufNumberSet(dev, port, trafClass, number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTx4TcBufNumberSet(dev, port, trafClass, number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTx4TcBufNumberGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       trafClass,
    OUT GT_U16      *numPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTx4TcBufNumberGet)
{
/*
ITERATE_DEVICES_PHY_PORTS (DxSal)
1.1.1. Call with trafClass [0], and non-null numPtr.
Expected: GT_OK.
1.1.2. Call with out of range trafClass [CPSS_4_TC_RANGE_CNS]=4 and non-null numPtr.
Expected: GT_BAD_PARAM.
1.1.3. Call with trafClass [1], and numPtr [NULL]
Expected: GT_BAD_PTR.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U8       trafClass;
    GT_U16      num;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with trafClass [0], and non-null numPtr.
               Expected: GT_OK. */

            trafClass = 0;

            st = cpssDxSalPortTx4TcBufNumberGet(dev, port, trafClass, &num);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafClass);

            /* 1.1.2. Call with out of range trafClass [CPSS_4_TC_RANGE_CNS]=4
               and non-null numPtr. Expected: GT_BAD_PARAM. */

            trafClass = CPSS_4_TC_RANGE_CNS;

            st = cpssDxSalPortTx4TcBufNumberGet(dev, port, trafClass, &num);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, trafClass = %d",
                                         dev, trafClass);

            /* 1.1.3. Call with trafClass [1], and numPtr [NULL].
                Expected: GT_BAD_PTR.   */

            trafClass = 1;

            st = cpssDxSalPortTx4TcBufNumberGet(dev, port, trafClass, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numPtr = NULL",
                                         dev);
        }

        trafClass = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortTx4TcBufNumberGet(dev, port, trafClass, &num);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTx4TcBufNumberGet(dev, port, trafClass, &num);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortTx4TcBufNumberGet(dev, port, trafClass, &num);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;
    trafClass = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTx4TcBufNumberGet(dev, port, trafClass, &num);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTx4TcBufNumberGet(dev, port, trafClass, &num);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTxWatchdogGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr,
    OUT GT_U16      *timeoutPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTxWatchdogGet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS (DxSal)
1.1.1. Call with non-null enablePtr and non-null timeoutPtr.
Expected: GT_OK.
1.1.2. Call with null enablePtr [NULL] and non-null timeoutPtr. 
Expected: GT_BAD_PTR.
1.1.3. Call with non-null enablePtr and null timeoutPtr [NULL]. 
Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     enable = GT_FALSE;
    GT_U16      timeout = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null enablePtr and non-null timeoutPtr.
                Expected: GT_OK.    */

            st = cpssDxSalPortTxWatchdogGet(dev, port, &enable, &timeout);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with null enablePtr [NULL] and non-null timeoutPtr. 
                Expected: GT_BAD_PTR.   */

            st = cpssDxSalPortTxWatchdogGet(dev, port, NULL, &timeout);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, enablePtr = NULL", dev);

            /* 1.1.3. Call with non-null enablePtr and null timeoutPtr [NULL]. 
                Expected: GT_BAD_PTR.   */

            st = cpssDxSalPortTxWatchdogGet(dev, port, &enable, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, timeoutPtr = NULL", dev);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxSalPortTxWatchdogGet(dev, port, &enable, &timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxSalPortTxWatchdogGet(dev, port, &enable, &timeout);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTxWatchdogGet(dev, port, &enable, &timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTxWatchdogGet(dev, port, &enable, &timeout);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTxWatchdogGet(dev, port, &enable, &timeout);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortTxWatchdogEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable,
    IN  GT_U16    timeout
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortTxWatchdogEnableSet)
{
/*
ITERATE_DEVICES_PHY_PORTS (DxSal)
1.1.1. Call with enable [GT_FALSE and GT_TRUE] and timeout [50]. 
Expected: GT_OK.
1.1.2. Call cpssDxSalPortTxWatchdogGet
 with non-null enable and non-null timeoutPtr. 
Expected: GT_OK and the same enable and timeout.
1.1.3. Call with enable [GT_TRUE] and out of range  timeout [4096]. 
Expected: NOT GT_OK.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     enable;
    GT_U16      timeout;
    GT_BOOL     enableRet = GT_FALSE;
    GT_U16      timeoutRet = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with enable [GT_FALSE and GT_TRUE] and timeout [50]. 
                Expected: GT_OK.    */

            enable = GT_FALSE;
            timeout = 50;

            st = cpssDxSalPortTxWatchdogEnableSet(dev, port, enable, timeout);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, enable, timeout);

            /* 1.1.2. Call cpssDxSalPortTxWatchdogGet
                with non-null enable and non-null timeoutPtr. 
                Expected: GT_OK and the same enable and timeout.    */

            st = cpssDxSalPortTxWatchdogGet(dev, port, &enableRet, &timeoutRet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalPortTxWatchdogGet: %d", dev);

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableRet,
                 "cpssDxSalPortTxWatchdogGet: get another enable than was set: dev = %d", dev);
            }

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(timeout, timeoutRet,
                 "cpssDxSalPortTxWatchdogGet: get another timeout than was set: dev = %d", dev);
            }

            /* 1.1.3. Call with enable [GT_TRUE] and out of range  timeout [4096]. 
                Expected: NOT GT_OK.    */

            enable = GT_TRUE;
            timeout = 4096;

            st = cpssDxSalPortTxWatchdogEnableSet(dev, port, enable, timeout);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, timeout = %d", dev, timeout);
        }

        enable = GT_FALSE;
        timeout = 50;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortTxWatchdogEnableSet(dev, port, enable, timeout);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortTxWatchdogEnableSet(dev, port, enable, timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortTxWatchdogEnableSet(dev, port, enable, timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;
    enable = GT_FALSE;
    timeout = 50;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortTxWatchdogEnableSet(dev, port, enable, timeout);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortTxWatchdogEnableSet(dev, port, enable, timeout);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssDxSalBrgEgrFlt suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxSalPortTx)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTxQueueEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTxQueueingEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTxQueueTxEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTxFlushQueuesSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTxQWrrProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTxQArbGroupSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTxMaxDescNumberSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTxDescNumberGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTxBufNumberSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTxBufNumberGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTx4TcMaxDescNumberSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTx4TcDescNumberGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTx4TcBufNumberSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTx4TcBufNumberGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTxWatchdogGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortTxWatchdogEnableSet)
UTF_SUIT_END_TESTS_MAC(cpssDxSalPortTx)
