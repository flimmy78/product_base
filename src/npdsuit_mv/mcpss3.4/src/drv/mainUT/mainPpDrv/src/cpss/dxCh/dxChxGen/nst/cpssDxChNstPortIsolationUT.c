/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNstPortIsolationUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChNstPortIsolation, that provides
*       Network Shield Technology facility Cheetah CPSS declarations
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
/* includes */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/nst/cpssDxChNst.h>
#include <cpss/dxCh/dxChxGen/nst/cpssDxChNstPortIsolation.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */
#define NST_VALID_PHY_PORT_CNS  0

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNstPortIsolationEnableSet(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChNstPortIsolationEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat and above)
    1.1. Call with enable [GT_FALSE/ GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChNstPortIsolationEnableGet.
    Expected: GT_OK and the same value.
*/
    GT_STATUS   st     = GT_OK;
    GT_U8       dev;
    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_TRUE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with enable = GT_FALSE and GT_TRUE.   */
        /* Expected: GT_OK.                                             */
        enable = GT_FALSE;

        /*call with enable = GT_FALSE */
        st = cpssDxChNstPortIsolationEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*call with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxChNstPortIsolationEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* 1.2. Call cpssDxChNstPortIsolationEnableGet.   */
        /* Expected: GT_OK and the same value.                          */

        enable = GT_FALSE;

        /*call with enable = GT_FALSE */
        st = cpssDxChNstPortIsolationEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*checking value */
        st = cpssDxChNstPortIsolationEnableGet(dev, &enableGet);

        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                                  "got another enable than was set: %d", dev);

        /*call with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxChNstPortIsolationEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*checking value */
        st = cpssDxChNstPortIsolationEnableGet(dev, &enableGet);

        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                                  "got another enable than was set: %d", dev);
        enable = GT_TRUE;
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNstPortIsolationEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* enable == GT_TRUE    */

    st = cpssDxChNstPortIsolationEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNstPortIsolationEnableGet(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChNstPortIsolationEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat and above)
    1.1. Call function for with non null enable pointer.
    Expected: GT_OK.
    1.2. Call function for with null enable pointer [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st     = GT_OK;
    GT_U8       dev;
    GT_BOOL     enable;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with non null enable pointer.   */
        /* Expected: GT_OK.                                             */

        st = cpssDxChNstPortIsolationEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* 1.2. Call function for with null enable pointer [NULL].      */
        /* Expected: GT_BAD_PTR.                                        */
        st = cpssDxChNstPortIsolationEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNstPortIsolationEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNstPortIsolationEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNstPortIsolationPortAdd
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_U8                                          portNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChNstPortIsolationPortAdd)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (xCat and above)
    1.1.1. Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E /
                                  CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]
                             and  srcInterface.type [CPSS_INTERFACE_PORT_E].
    Expected: GT_OK.
    1.1.2. Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E /
                                  CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]
                             and  srcInterface.type [CPSS_INTERFACE_TRUNK_E].
    Expected: GT_OK.
    1.1.3. Call with srcInterface.type [CPSS_INTERFACE_PORT_E] and out of range
                                devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                           and portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS].
    Expected: GT_BAD_PARAM.
    1.1.4. Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E /
                                  CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]
                             and  srcInterface.type [CPSS_INTERFACE_FABRIC_VIDX_E].
    Expected: NOT GT_OK.
    1.1.5. Call with wrong enum values trafficType.
    Expected: GT_BAD_PARAM.
    1.1.6. Call with wrong enum values port.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                                      st       = GT_OK;

    GT_U8                                          dev;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC                        srcInterface;
    GT_U8                                          port;

    PRV_CPSS_PORT_TYPE_ENT                         portType = PRV_CPSS_PORT_NOT_EXISTS_E;

    trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

    cpssOsBzero((GT_VOID*) &srcInterface, sizeof(CPSS_INTERFACE_INFO_STC));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);
            /*
                1.1.1. Call with trafficType
                       [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E /
                        CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]
                        and  srcInterface.type [CPSS_INTERFACE_PORT_E].
                Expected: GT_OK.
            */

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

            srcInterface.type            = CPSS_INTERFACE_PORT_E;
            srcInterface.devPort.devNum  = dev;
            srcInterface.devPort.portNum = port;

            st = cpssDxChNstPortIsolationPortAdd(dev, trafficType, srcInterface, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            st = cpssDxChNstPortIsolationPortDelete(dev, trafficType, srcInterface, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E;

            st = cpssDxChNstPortIsolationPortAdd(dev, trafficType, srcInterface, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            st = cpssDxChNstPortIsolationPortDelete(dev, trafficType, srcInterface, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            /*
                1.1.2. Call with trafficType
                       [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E /
                        CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]
                        and  srcInterface.type [CPSS_INTERFACE_TRUNK_E].
                Expected: GT_OK.
            */

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

            srcInterface.type     = CPSS_INTERFACE_TRUNK_E;
            srcInterface.trunkId  = 0;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);

            st = cpssDxChNstPortIsolationPortAdd(dev, trafficType, srcInterface, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            st = cpssDxChNstPortIsolationPortDelete(dev, trafficType, srcInterface, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E;

            st = cpssDxChNstPortIsolationPortAdd(dev, trafficType, srcInterface, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            st = cpssDxChNstPortIsolationPortDelete(dev, trafficType, srcInterface, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            /*
                1.1.3. Call with srcInterface.type [CPSS_INTERFACE_PORT_E] and out of range
                                            devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                                       and portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS].
                Expected: GT_BAD_PARAM.
            */

            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

            srcInterface.type            = CPSS_INTERFACE_PORT_E;

            /* call with wrong devNum */
            srcInterface.devPort.devNum  = PRV_CPSS_MAX_PP_DEVICES_CNS;

            st = cpssDxChNstPortIsolationPortAdd(dev, trafficType, srcInterface, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, trafficType);

            srcInterface.devPort.devNum  = dev;

            /* call with wrong portNum */
            srcInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssDxChNstPortIsolationPortAdd(dev, trafficType, srcInterface, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, trafficType);

            srcInterface.devPort.portNum = port;

            /*
                1.1.4. Call with trafficType
                           [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E],
                      and  srcInterface.type [CPSS_INTERFACE_FABRIC_VIDX_E].
                Expected: NOT GT_OK.
            */
            trafficType       = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;
            srcInterface.type = CPSS_INTERFACE_FABRIC_VIDX_E;

            st = cpssDxChNstPortIsolationPortAdd(dev, trafficType, srcInterface, port);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            /*
                1.1.5. Call with wrong enum values trafficType
                        and  srcInterface.type [CPSS_INTERFACE_TRUNK_E].
                Expected: GT_BAD_PARAM.
            */
            srcInterface.type     = CPSS_INTERFACE_TRUNK_E;
            srcInterface.trunkId  = 0;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);

            UTF_ENUMS_CHECK_MAC(cpssDxChNstPortIsolationPortAdd
                                (dev, trafficType, srcInterface, port),
                                trafficType);
        }

        /* 1.2. For all active devices go over all non available physical ports. */

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port    */

            st = cpssDxChNstPortIsolationPortAdd(dev, trafficType, srcInterface, port);
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev) ||
               port >= 64)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
            else
            {
                /* the device supports 64 ports regardless to physical ports */
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChNstPortIsolationPortAdd(dev, trafficType, srcInterface, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    trafficType  = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E;
    port         = NST_VALID_PHY_PORT_CNS;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNstPortIsolationPortAdd(dev, trafficType, srcInterface, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNstPortIsolationPortAdd(dev, trafficType, srcInterface, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNstPortIsolationPortDelete
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_U8                                          portNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChNstPortIsolationPortDelete)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (xCat and above)
    1.1.1. Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E /
                                  CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]
                             and  srcInterface.type [CPSS_INTERFACE_PORT_E].
    Expected: GT_OK.
    1.1.2. Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E /
                                  CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]
                             and  srcInterface.type [CPSS_INTERFACE_TRUNK_E].
    Expected: GT_OK.
*/
    GT_STATUS                                      st       = GT_OK;

    GT_U8                                          dev;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC                        srcInterface;
    GT_U8                                          port;

    PRV_CPSS_PORT_TYPE_ENT                         portType = PRV_CPSS_PORT_NOT_EXISTS_E;

    trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

    cpssOsBzero((GT_VOID*) &srcInterface, sizeof(CPSS_INTERFACE_INFO_STC));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);
            /*
                1.1.1. Call with trafficType
                       [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E /
                        CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]
                        and  srcInterface.type [CPSS_INTERFACE_PORT_E].
                Expected: GT_OK.
            */

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

            srcInterface.type            = CPSS_INTERFACE_PORT_E;
            srcInterface.devPort.devNum  = dev;
            srcInterface.devPort.portNum = port;

            st = cpssDxChNstPortIsolationPortDelete(dev, trafficType, srcInterface, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E;

            st = cpssDxChNstPortIsolationPortDelete(dev, trafficType, srcInterface, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            /*
                1.1.2. Call with trafficType
                       [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E /
                        CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]
                        and  srcInterface.type [CPSS_INTERFACE_TRUNK_E].
                Expected: GT_OK.
            */

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

            srcInterface.type     = CPSS_INTERFACE_TRUNK_E;
            srcInterface.trunkId  = 0;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);

            st = cpssDxChNstPortIsolationPortDelete(dev, trafficType, srcInterface, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E;

            st = cpssDxChNstPortIsolationPortDelete(dev, trafficType, srcInterface, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            /*
                1.1.3. Call with wrong enum values trafficType
                Expected: GT_BAD_PARAM.
            */
            srcInterface.type     = CPSS_INTERFACE_TRUNK_E;
            srcInterface.trunkId  = 0;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);

            UTF_ENUMS_CHECK_MAC(cpssDxChNstPortIsolationPortDelete
                                (dev, trafficType, srcInterface, port),
                                trafficType);
        }

        /* 1.2. For all active devices go over all non available physical ports. */

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port    */

            st = cpssDxChNstPortIsolationPortDelete(dev, trafficType, srcInterface, port);
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev) ||
               port >= 64)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
            else
            {
                /* the device supports 64 ports regardless to physical ports */
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChNstPortIsolationPortDelete(dev, trafficType, srcInterface, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    trafficType  = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E;
    port         = NST_VALID_PHY_PORT_CNS;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNstPortIsolationPortDelete(dev, trafficType, srcInterface, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNstPortIsolationPortDelete(dev, trafficType, srcInterface, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNstPortIsolationTableEntrySet
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_BOOL                                        cpuPortMember,
    IN CPSS_PORTS_BMP_STC                             *localPortsMembersPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNstPortIsolationTableEntrySet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (xCat and above)
    1.1.1. Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E /
                                  CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]
          and  srcInterface.type [CPSS_INTERFACE_TRUNK_E / CPSS_INTERFACE_TRUNK_E].
                   cpuPortMember [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChNstPortIsolationTableEntryGet.
    Expected: GT_OK and the same cpuPortMember and localPortsMembers.
    1.1.3. Call with srcInterface.type [CPSS_INTERFACE_PORT_E] and out of range
                                devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                           and portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS].
    Expected: GT_BAD_PARAM.
    1.1.4. Call with wrong enum values trafficType.
    Expected: GT_BAD_PARAM.
    1.1.5. Call with incorrect localPortsMembersPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                                      st       = GT_OK;

    GT_U8                                          dev;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC                        srcInterface;
    GT_BOOL                                        cpuPortMember = GT_FAIL;
    CPSS_PORTS_BMP_STC                             localPortsMembers;

    GT_BOOL                                        cpuPortMemberGet;
    CPSS_PORTS_BMP_STC                             localPortsMembersGet;

    GT_U8                                          port = NST_VALID_PHY_PORT_CNS;

    trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

    cpssOsBzero((GT_VOID*) &srcInterface, sizeof(CPSS_INTERFACE_INFO_STC));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with trafficType
                               [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E /
                                CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E],
                       srcInterface.type [CPSS_INTERFACE_TRUNK_E /
                                          CPSS_INTERFACE_TRUNK_E].
                       and cpuPortMember [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

            srcInterface.type            = CPSS_INTERFACE_PORT_E;
            srcInterface.devPort.devNum  = dev;
            srcInterface.devPort.portNum = port;

            cpuPortMember                = GT_TRUE;
            localPortsMembers.ports[0]   = 0;
            localPortsMembers.ports[1]   = 0;

            st = cpssDxChNstPortIsolationTableEntrySet(dev, trafficType,
                                srcInterface, cpuPortMember, &localPortsMembers);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            if(GT_OK == st)
            {
                /*
                    1.1.2. Call cpssDxChNstPortIsolationTableEntryGet.
                    Expected: GT_OK and the same cpuPortMember and localPortsMembers.
                */

                st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                                    srcInterface, &cpuPortMemberGet, &localPortsMembersGet);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

                UTF_VERIFY_EQUAL2_STRING_MAC(cpuPortMember, cpuPortMemberGet,
                        "get another cpuPortMember than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(localPortsMembers.ports[0],
                                             localPortsMembersGet.ports[0],
                "get another localPortsMembers.ports[0] than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(localPortsMembers.ports[1],
                                             localPortsMembersGet.ports[1],
                "get another localPortsMembers.ports[1] than was set: %d, %d", dev, port);
            }

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E;

            st = cpssDxChNstPortIsolationTableEntrySet(dev, trafficType,
                                srcInterface, cpuPortMember, &localPortsMembers);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            if(GT_OK == st)
            {
                /*
                    1.1.2. Call cpssDxChNstPortIsolationTableEntryGet.
                    Expected: GT_OK and the same cpuPortMember and localPortsMembers.
                */

                st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                                    srcInterface, &cpuPortMemberGet, &localPortsMembersGet);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

                UTF_VERIFY_EQUAL2_STRING_MAC(cpuPortMember, cpuPortMemberGet,
                        "get another cpuPortMember than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(localPortsMembers.ports[0],
                                             localPortsMembersGet.ports[0],
                "get another localPortsMembers.ports[0] than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(localPortsMembers.ports[1],
                                             localPortsMembersGet.ports[1],
                "get another localPortsMembers.ports[1] than was set: %d, %d", dev, port);
            }

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

            srcInterface.type     = CPSS_INTERFACE_TRUNK_E;
            srcInterface.trunkId  = 0;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);

            cpuPortMember                = GT_FALSE;
            localPortsMembers.ports[0]   = 1;
            localPortsMembers.ports[1]   = 0;

            st = cpssDxChNstPortIsolationTableEntrySet(dev, trafficType,
                                srcInterface, cpuPortMember, &localPortsMembers);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            if(GT_OK == st)
            {
                /*
                    1.1.2. Call cpssDxChNstPortIsolationTableEntryGet.
                    Expected: GT_OK and the same cpuPortMember and localPortsMembers.
                */

                st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                                    srcInterface, &cpuPortMemberGet, &localPortsMembersGet);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

                UTF_VERIFY_EQUAL2_STRING_MAC(cpuPortMember, cpuPortMemberGet,
                        "get another cpuPortMember than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(localPortsMembers.ports[0],
                                             localPortsMembersGet.ports[0],
                "get another localPortsMembers.ports[0] than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(localPortsMembers.ports[1],
                                             localPortsMembersGet.ports[1],
                "get another localPortsMembers.ports[1] than was set: %d, %d", dev, port);
            }

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E;

            st = cpssDxChNstPortIsolationTableEntrySet(dev, trafficType,
                                srcInterface, cpuPortMember, &localPortsMembers);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            if(GT_OK == st)
            {
                /*
                    1.1.2. Call cpssDxChNstPortIsolationTableEntryGet.
                    Expected: GT_OK and the same cpuPortMember and localPortsMembers.
                */

                st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                                    srcInterface, &cpuPortMemberGet, &localPortsMembersGet);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

                UTF_VERIFY_EQUAL2_STRING_MAC(cpuPortMember, cpuPortMemberGet,
                        "get another cpuPortMember than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(localPortsMembers.ports[0],
                                             localPortsMembersGet.ports[0],
                "get another localPortsMembers.ports[0] than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(localPortsMembers.ports[1],
                                             localPortsMembersGet.ports[1],
                "get another localPortsMembers.ports[1] than was set: %d, %d", dev, port);
            }

            /*
                1.1.3. Call with srcInterface.type [CPSS_INTERFACE_PORT_E] and out of range
                                            devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                                       and portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS].
                Expected: GT_BAD_PARAM.
            */

            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

            srcInterface.type            = CPSS_INTERFACE_PORT_E;

            /* call with wrong devNum */
            srcInterface.devPort.devNum  = PRV_CPSS_MAX_PP_DEVICES_CNS;

            st = cpssDxChNstPortIsolationTableEntrySet(dev, trafficType,
                                srcInterface, cpuPortMember, &localPortsMembers);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, trafficType);

            srcInterface.devPort.devNum  = dev;

            /* call with wrong portNum */
            srcInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssDxChNstPortIsolationTableEntrySet(dev, trafficType,
                                srcInterface, cpuPortMember, &localPortsMembers);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, trafficType);

            srcInterface.devPort.portNum = port;

            /*
                1.1.4. Call with wrong enum values trafficType
                        and  srcInterface.type [CPSS_INTERFACE_TRUNK_E].
                Expected: GT_BAD_PARAM.
            */
            srcInterface.type     = CPSS_INTERFACE_TRUNK_E;
            srcInterface.trunkId  = 0;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);

            UTF_ENUMS_CHECK_MAC(cpssDxChNstPortIsolationTableEntrySet
                                (dev, trafficType, srcInterface,
                                 cpuPortMember, &localPortsMembers),
                                trafficType);

            /*
                1.1.5. Call with incorrect localPortsMembersPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNstPortIsolationTableEntrySet(dev, trafficType,
                                srcInterface, cpuPortMember, NULL);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PTR, st, dev, port, trafficType);
        }

        /* 1.2. For all active devices go over all non available physical ports. */

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        srcInterface.type            = CPSS_INTERFACE_PORT_E;
        srcInterface.devPort.devNum  = dev;

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port    */
            srcInterface.devPort.portNum = port;

            st = cpssDxChNstPortIsolationTableEntrySet(dev, trafficType,
                                srcInterface, cpuPortMember, &localPortsMembers);
            /* the device supports 64 ports regardless to physical ports */
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
        }

        trafficType  = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E;
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNstPortIsolationTableEntrySet(dev, trafficType,
                            srcInterface, cpuPortMember, &localPortsMembers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNstPortIsolationTableEntrySet(dev, trafficType,
                        srcInterface, cpuPortMember, &localPortsMembers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNstPortIsolationTableEntryGet
(
    IN  GT_U8                                          devNum,
    IN  CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN  CPSS_INTERFACE_INFO_STC                        srcInterface,
    OUT GT_BOOL                                       *cpuPortMemberPtr,
    OUT CPSS_PORTS_BMP_STC                            *localPortsMembersPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNstPortIsolationTableEntryGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (xCat and above)
    1.1.1. Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E /
                                  CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]
          and  srcInterface.type [CPSS_INTERFACE_TRUNK_E / CPSS_INTERFACE_TRUNK_E].
                   cpuPortMember [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChNstPortIsolationTableEntryGet.
    Expected: GT_OK and the same cpuPortMember and localPortsMembers.
    1.1.3. Call with srcInterface.type [CPSS_INTERFACE_PORT_E] and out of range
                                devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                           and portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS].
    Expected: GT_BAD_PARAM.
    1.1.4. Call with wrong enum values trafficType.
    Expected: GT_BAD_PARAM.
    1.1.5. Call with incorrect localPortsMembersPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                                      st       = GT_OK;

    GT_U8                                          dev;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC                        srcInterface;
    GT_BOOL                                        cpuPortMember = GT_FALSE;
    CPSS_PORTS_BMP_STC                             localPortsMembers;

    GT_U8                                          port = NST_VALID_PHY_PORT_CNS;

    trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

    cpssOsBzero((GT_VOID*) &srcInterface, sizeof(CPSS_INTERFACE_INFO_STC));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with trafficType
                               [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E /
                                CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E],
                       srcInterface.type [CPSS_INTERFACE_TRUNK_E /
                                          CPSS_INTERFACE_TRUNK_E].
                Expected: GT_OK.
            */

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

            srcInterface.type            = CPSS_INTERFACE_PORT_E;
            srcInterface.devPort.devNum  = dev;
            srcInterface.devPort.portNum = port;

            st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                                srcInterface, &cpuPortMember, &localPortsMembers);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            if(GT_OK == st)
            {
                /*
                    1.1.2. Call with wrong enum values trafficType
                            and  srcInterface.type [CPSS_INTERFACE_TRUNK_E].
                    Expected: GT_BAD_PARAM.
                */
                srcInterface.type     = CPSS_INTERFACE_TRUNK_E;
                srcInterface.trunkId  = 0;
                CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);

                UTF_ENUMS_CHECK_MAC(cpssDxChNstPortIsolationTableEntryGet
                                    (dev, trafficType, srcInterface,
                                     &cpuPortMember, &localPortsMembers),
                                    trafficType);
            }

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E;

            st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                                srcInterface, &cpuPortMember, &localPortsMembers);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            if(GT_OK == st)
            {
                /*
                    1.1.2. Call with wrong enum values trafficType
                            and  srcInterface.type [CPSS_INTERFACE_TRUNK_E].
                    Expected: GT_BAD_PARAM.
                */
                srcInterface.type     = CPSS_INTERFACE_TRUNK_E;
                srcInterface.trunkId  = 0;
                CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);

                UTF_ENUMS_CHECK_MAC(cpssDxChNstPortIsolationTableEntryGet
                                    (dev, trafficType, srcInterface,
                                     &cpuPortMember, &localPortsMembers),
                                    trafficType);
            }

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

            srcInterface.type     = CPSS_INTERFACE_TRUNK_E;
            srcInterface.trunkId  = 0;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);

            st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                                srcInterface, &cpuPortMember, &localPortsMembers);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            if(GT_OK == st)
            {
                /*
                    1.1.2. Call with wrong enum values trafficType
                            and  srcInterface.type [CPSS_INTERFACE_TRUNK_E].
                    Expected: GT_BAD_PARAM.
                */
                srcInterface.type     = CPSS_INTERFACE_TRUNK_E;
                srcInterface.trunkId  = 0;
                CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);

                UTF_ENUMS_CHECK_MAC(cpssDxChNstPortIsolationTableEntryGet
                                    (dev, trafficType, srcInterface, &cpuPortMember, &localPortsMembers),
                                    trafficType);
            }

            /*Call with trafficType [CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E]*/
            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E;

            st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                                srcInterface, &cpuPortMember, &localPortsMembers);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, trafficType);

            if(GT_OK == st)
            {
                /*
                    1.1.2. Call with wrong enum values trafficType
                            and  srcInterface.type [CPSS_INTERFACE_TRUNK_E].
                    Expected: GT_BAD_PARAM.
                */
                srcInterface.type     = CPSS_INTERFACE_TRUNK_E;
                srcInterface.trunkId  = 0;
                CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);

                UTF_ENUMS_CHECK_MAC(cpssDxChNstPortIsolationTableEntryGet
                                    (dev, trafficType, srcInterface,
                                     &cpuPortMember, &localPortsMembers),
                                    trafficType);
            }

            /*
                1.1.3. Call with srcInterface.type [CPSS_INTERFACE_PORT_E] and out of range
                                            devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                                       and portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS].
                Expected: GT_BAD_PARAM.
            */

            trafficType = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E;

            srcInterface.type            = CPSS_INTERFACE_PORT_E;

            /* call with wrong devNum */
            srcInterface.devPort.devNum  = PRV_CPSS_MAX_PP_DEVICES_CNS;

            st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                                srcInterface, &cpuPortMember, &localPortsMembers);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, trafficType);

            srcInterface.devPort.devNum  = dev;

            /* call with wrong portNum */
            srcInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                                srcInterface, &cpuPortMember, &localPortsMembers);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, trafficType);

            srcInterface.devPort.portNum = port;

            /*
                1.1.4. Call with incorrect cpuPortMember [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                                        srcInterface, NULL, &localPortsMembers);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PTR, st, dev, port, trafficType);

            /*
                1.1.5. Call with incorrect localPortsMembersPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                                             srcInterface, &cpuPortMember, NULL);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PTR, st, dev, port, trafficType);
        }

        /* 1.2. For all active devices go over all non available physical ports. */

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        srcInterface.type            = CPSS_INTERFACE_PORT_E;
        srcInterface.devPort.devNum  = dev;

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port    */
            srcInterface.devPort.portNum = port;

            st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                                srcInterface, &cpuPortMember, &localPortsMembers);
            /* the device supports 64 ports regardless to physical ports */
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
        }

        trafficType  = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E;
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                            srcInterface, &cpuPortMember, &localPortsMembers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNstPortIsolationTableEntryGet(dev, trafficType,
                        srcInterface, &cpuPortMember, &localPortsMembers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChNst suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChNstPortIsolation)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNstPortIsolationEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNstPortIsolationEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNstPortIsolationPortAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNstPortIsolationPortDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNstPortIsolationTableEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNstPortIsolationTableEntryGet)
UTF_SUIT_END_TESTS_MAC(cpssDxChNstPortIsolation)

