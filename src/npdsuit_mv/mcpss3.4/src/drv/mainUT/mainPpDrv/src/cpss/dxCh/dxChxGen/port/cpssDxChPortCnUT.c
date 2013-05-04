/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortCnUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChPortCn, that provides
*       CPSS implementation for Port Congestion Notification API.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCn.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPort.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <common/tgfCommon.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* defines */

/* Default valid value for port id */
#define PORT_CN_VALID_PHY_PORT_CNS  0

/* Default invalid value for port id */
#define PORT_CN_INVALID_PHY_PORT_CNS  64

/* Default valid value for dev id */
#define PORT_CN_VALID_DEV_CNS  0

/* Default invalid value for dev id */
#define PORT_CN_INVALID_DEV_CNS  32

/* Invalid etherType value */
#define PORT_CN_INVALID_ETHERTYPE_CNS  0xFFFF+1

/* Invalid timer value */
#define PORT_CN_INVALID_TIMER_CNS      0xFFFF+1

/* Invalid xOnLimit / xOffLimit  value */
#define PORT_CN_INVALID_XONXOFFLIMIT_CNS      0x1FFF+1

/* Invalid threshold  value */
#define PORT_CN_INVALID_THRESHOLD_CNS         0x1FFF+1

/* Invalid portSpeedIndex */
#define PORT_CN_INVALID_PORTSPEEDINDEX_CNS 8

/* Invalid profileIndex */
#define PORT_CN_INVALID_PROFILEINDEX_CNS   8

/* Invalid tcQueue */
#define PORT_CN_INVALID_TCQUEUE_CNS        8

#define IS_PORT_XG_E(devNum,portNum)    \
        (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType==PRV_CPSS_PORT_XG_E)

#define IS_PORT_FE_E(devNum,portNum)    \
        (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType==PRV_CPSS_PORT_FE_E)


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnEtherTypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherType
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnEtherTypeSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with etherType [0 / 0xF0F0 / 0xFFFF].
    Expected: GT_OK.
    1.2. Call cpssDxChPortCnEtherTypeGet with not-NULL etherTypePtr.
    Expected: GT_OK and the same etherType as was set.
    1.3. Call with wrong etherType [PORT_CN_INVALID_ETHERTYPE_CNS].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32   etherType = 0;
    GT_U32   etherTypeGet = 1;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with etherType [0 / 0xF0F0 / 0xFFFF].
            Expected: GT_OK.
        */
        etherType = 0;

        st = cpssDxChPortCnEtherTypeSet(dev, etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnEtherTypeGet with not-NULL etherTypePtr.
            Expected: GT_OK and the same etherType as was set.
        */
        st = cpssDxChPortCnEtherTypeGet(dev, &etherTypeGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortCnEtherTypeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet,
                                     "got another etherType then was set: %d", dev);

        /*
            1.1. Call with etherType [0 / 0xF0F0 / 0xFFFF].
            Expected: GT_OK.
        */
        etherType = 0xF0F0;

        st = cpssDxChPortCnEtherTypeSet(dev, etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnEtherTypeGet with not-NULL etherTypePtr.
            Expected: GT_OK and the same etherType as was set.
        */
        st = cpssDxChPortCnEtherTypeGet(dev, &etherTypeGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortCnEtherTypeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet,
                                     "got another etherType then was set: %d", dev);


        /*
            1.1. Call with etherType [0 / 0xF0F0 / 0xFFFF].
            Expected: GT_OK.
        */
        etherType = 0xFFFF;

        st = cpssDxChPortCnEtherTypeSet(dev, etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnEtherTypeGet with not-NULL etherTypePtr.
            Expected: GT_OK and the same etherType as was set.
        */
        st = cpssDxChPortCnEtherTypeGet(dev, &etherTypeGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortCnEtherTypeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet,
                                     "got another etherType then was set: %d", dev);

        /*
            1.3. Call with wrong etherType [PORT_CN_INVALID_ETHERTYPE_CNS].
            Expected: GT_OUT_OF_RANGE.
        */
        etherType = PORT_CN_INVALID_ETHERTYPE_CNS;

        st = cpssDxChPortCnEtherTypeSet(dev, etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);

        etherType = 0;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnEtherTypeSet(dev, etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnEtherTypeSet(dev, etherType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnEtherTypeGet
(
    IN  GT_U8           devNum,
    OUT GT_U32         *etherTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnEtherTypeGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with not-NULL etherTypePtr
    Expected: GT_OK.
    1.2. Call with wrong  etherTypePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      etherType;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not-NULL etherType.
            Expected: GT_OK.
        */
        st = cpssDxChPortCnEtherTypeGet(dev, &etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with etherType[NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnEtherTypeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, etherType = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnEtherTypeGet(dev, &etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnEtherTypeGet(dev, &etherType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnFcEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnFcEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat,Lion,xCat2)
    1.1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortCnFcEnableGet.
    Expected: GT_OK and the same enable.
*/

    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_CN_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;
    GT_BOOL     state  = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
               1.1.1. Call with state [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_FALSE] */
            enable = GT_FALSE;

            st = cpssDxChPortCnFcEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
               1.1.2. Call cpssDxChPortCnFcEnableGet.
               Expected: GT_OK and the same enable.
            */
            st = cpssDxChPortCnFcEnableGet(dev, port, &state);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "[cpssDxChPortCnFcEnableGet]: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, state,
                            "get another enable value than was set: %d, %d", dev, port);

            /*
               1.1.1. Call with state [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_TRUE] */
            enable = GT_TRUE;

            st = cpssDxChPortCnFcEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
               1.1.2. Call cpssDxChPortCnFcEnableGet.
               Expected: GT_OK and the same enable.
            */
            st = cpssDxChPortCnFcEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                          "[cpssDxChPortCnFcEnableGet]: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, state,
                          "get another enable value than was set: %d, %d", dev, port);
        }

        /*
           1.2. For all active devices go over all non available
           physical ports.
        */
        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* enable == GT_TRUE    */
            st = cpssDxChPortCnFcEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* enable == GT_TRUE  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortCnFcEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                     */
        /* enable == GT_TRUE  */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortCnFcEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, enable);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    enable = GT_TRUE;
    port = PORT_CN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnFcEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, enable == GT_TRUE */

    st = cpssDxChPortCnFcEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnFcEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *statePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnFcEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat,Lion,xCat2)
    1.1.1. Call with non-null statePtr.
    Expected: GT_OK.
    1.1.2. Call with statePtr [NULL].
    Expected: GT_BAD_PTR
*/

    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = PORT_CN_VALID_PHY_PORT_CNS;
    GT_BOOL     state = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null statePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortCnFcEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with statePtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortCnFcEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortCnFcEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortCnFcEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortCnFcEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnFcEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnFcEnableGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnFcTimerSet
(
    IN GT_U8      devNum,
    IN GT_U8      portNum,
    IN GT_U32     portSpeedIndex,
    IN GT_U32     timer
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnFcTimerSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat and above)
    1.1.1. Call with portSpeedIndex [0 / 7], timer[0 / 0xFFFF].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortCnFcTimerGet with the same params.
    Expected: GT_OK and the same enable.
    1.1.3. Call with wrong portSpeedIndex [PORT_CN_INVALID_PORTSPEEDINDEX_CNS]
    Expected: GT_BAD_PARAM.
    1.1.4. Call with wrong timer [PORT_CN_INVALID_TIMER_CNS]
    Expected: GT_OUT_OF_RANGE.
*/

    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_CN_VALID_PHY_PORT_CNS;
    GT_U32      portSpeedIndex = 0;
    GT_U32      timer = 0;
    GT_U32      timerGet = 0;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
               1.1.1. Call with portSpeedIndex [0 / 7], timer[0 / 0xFFFF].
               Expected: GT_OK.
            */

            /*call with portSpeedIndex [0] and timer[0]*/
            portSpeedIndex = 0;
            timer = 0;

            st = cpssDxChPortCnFcTimerSet(dev, port, portSpeedIndex, timer);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, portSpeedIndex, timer);

            /*
               1.1.2. Call cpssDxChPortCnFcTimerGet with the same params.
               Expected: GT_OK and the same timer.
            */
            st = cpssDxChPortCnFcTimerGet(dev, port, portSpeedIndex, &timerGet);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "[cpssDxChPortCnFcTimerGet]: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(timer, timerGet,
                      "get another timer value than was set: %d, %d", dev, port);

            /*
               1.1.1. Call with portSpeedIndex [0 / 7], timer[0 / 0xFFFF].
               Expected: GT_OK.
            */

            /*call with portSpeedIndex [7] and timer[0xFFFF]*/
            portSpeedIndex = 7;
            timer = 0xFFFF;

            st = cpssDxChPortCnFcTimerSet(dev, port, portSpeedIndex, timer);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, portSpeedIndex, timer);

            /*
               1.1.2. Call cpssDxChPortCnFcTimerGet with the same params.
               Expected: GT_OK and the same timer.
            */
            st = cpssDxChPortCnFcTimerGet(dev, port, portSpeedIndex, &timerGet);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "[cpssDxChPortCnFcTimerGet]: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(timer, timerGet,
                      "get another timer value than was set: %d, %d", dev, port);

            /*
               1.1.3. Call with wrong portSpeedIndex [PORT_CN_INVALID_PORTSPEEDINDEX_CNS].
               Expected: GT_BAD_PARAM.
            */
            portSpeedIndex = PORT_CN_INVALID_PORTSPEEDINDEX_CNS;

            st = cpssDxChPortCnFcTimerSet(dev, port, portSpeedIndex, timer);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, portSpeedIndex, timer);

            portSpeedIndex = 7;

            /*
               1.1.4. Call with wrong timer [PORT_CN_INVALID_TIMER_CNS]
               Expected: GT_OUT_OF_RANGE.
            */
            timer = PORT_CN_INVALID_TIMER_CNS;

            st = cpssDxChPortCnFcTimerSet(dev, port, portSpeedIndex, timer);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, port, portSpeedIndex, timer);

            timer = 0xFFFF;
        }

        /*
           1.2. For all active devices go over all non available
           physical ports.
        */

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortCnFcTimerSet(dev, port, portSpeedIndex, timer);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortCnFcTimerSet(dev, port, portSpeedIndex, timer);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortCnFcTimerSet(dev, port, portSpeedIndex, timer);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, portSpeedIndex, timer);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnFcTimerSet(dev, port, portSpeedIndex, timer);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, enable == GT_TRUE */

    st = cpssDxChPortCnFcTimerSet(dev, port, portSpeedIndex, timer);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnFcTimerGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  GT_U32   portSpeedIndex,
    OUT GT_U32   *timerPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnFcTimerGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat and above)
    1.1.1. Call with portSpeedIndex [0 / 7] and  non-null timerPtr.
    Expected: GT_OK.
    1.1.2. Call with wrong  portSpeedIndex [PORT_CN_INVALID_PORTSPEEDINDEX_CNS]
           and  non-null timerPtr.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with wrong timerPtr [NULL].
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = PORT_CN_VALID_PHY_PORT_CNS;
    GT_U32      portSpeedIndex = 0;
    GT_U32      timerGet = 0;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
               1.1.1. Call with portSpeedIndex [0 / 7] and  non-null timerPtr.
               Expected: GT_OK.
            */

            portSpeedIndex = 0;

            st = cpssDxChPortCnFcTimerGet(dev, port, portSpeedIndex, &timerGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
               1.1.1. Call with portSpeedIndex [0 / 7] and  non-null timerPtr.
               Expected: GT_OK.
            */

            portSpeedIndex = 7;

            st = cpssDxChPortCnFcTimerGet(dev, port, portSpeedIndex, &timerGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with wrong  portSpeedIndex [PORT_CN_INVALID_PORTSPEEDINDEX_CNS]
                       and  non-null timerPtr.
                Expected: GT_BAD_PARAM.
            */

            portSpeedIndex = PORT_CN_INVALID_PORTSPEEDINDEX_CNS;

            st = cpssDxChPortCnFcTimerGet(dev, port, portSpeedIndex, &timerGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

            portSpeedIndex = 7;

            /*
                1.1.3. Call with wrong timerPtr [NULL].
                Expected: GT_BAD_PTR
            */
            st = cpssDxChPortCnFcTimerGet(dev, port, portSpeedIndex,  NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortCnFcTimerGet(dev, port, portSpeedIndex, &timerGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortCnFcTimerGet(dev, port, portSpeedIndex, &timerGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortCnFcTimerGet(dev, port, portSpeedIndex, &timerGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnFcTimerGet(dev, port, portSpeedIndex, &timerGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnFcTimerGet(dev, port, portSpeedIndex, &timerGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnFrameQueueSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnFrameQueueSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with tcQueue [0 / 7].
    Expected: GT_OK.
    1.2. Call cpssDxChPortCnFrameQueueGet with not-NULL tcQueuePtr.
    Expected: GT_OK and the same tcQueue as was set.
    1.3. Call with wrong tcQueue [PORT_CN_INVALID_TCQUEUE_CNS].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U8   tcQueue = 0;
    GT_U8   tcQueueGet = 1;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcQueue [0 / 7].
            Expected: GT_OK.
        */
        tcQueue = 0;

        st = cpssDxChPortCnFrameQueueSet(dev, tcQueue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnFrameQueueGet with not-NULL tcQueuePtr.
            Expected: GT_OK and the same tcQueue as was set.
        */
        st = cpssDxChPortCnFrameQueueGet(dev, &tcQueueGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortCnFrameQueueGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(tcQueue, tcQueueGet,
                                     "got another tcQueue then was set: %d", dev);

        /*
            1.1. Call with tcQueue [0 / 7].
            Expected: GT_OK.
        */
        tcQueue = 7;

        st = cpssDxChPortCnFrameQueueSet(dev, tcQueue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnFrameQueueGet with not-NULL tcQueuePtr.
            Expected: GT_OK and the same tcQueue as was set.
        */
        st = cpssDxChPortCnFrameQueueGet(dev, &tcQueueGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortCnFrameQueueGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(tcQueue, tcQueueGet,
                                     "got another tcQueue then was set: %d", dev);

        /*
            1.3. Call with wrong tcQueue [PORT_CN_INVALID_TCQUEUE_CNS].
            Expected: GT_BAD_PARAM.
        */
        tcQueue = PORT_CN_INVALID_TCQUEUE_CNS;

        st = cpssDxChPortCnFrameQueueSet(dev, tcQueue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        tcQueue = 0;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnFrameQueueSet(dev, tcQueue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnFrameQueueSet(dev, tcQueue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnFrameQueueGet
(
    IN  GT_U8           devNum,
    OUT GT_U8          *tcQueuePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnFrameQueueGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with not-NULL tcQueuePtr
    Expected: GT_OK.
    1.2. Call with wrong  tcQueuePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U8      tcQueue = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not-NULL tcQueue.
            Expected: GT_OK.
        */
        st = cpssDxChPortCnFrameQueueGet(dev, &tcQueue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with tcQueue[NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnFrameQueueGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tcQueue = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnFrameQueueGet(dev, &tcQueue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnFrameQueueGet(dev, &tcQueue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnModeEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnModeEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat and above)
    1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChPortCnModeEnableGet.
    Expected: GT_OK and the same enable.
*/

    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_BOOL     enable = GT_FALSE;
    GT_BOOL     state  = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
           1.1. Call with state [GT_FALSE and GT_TRUE].
           Expected: GT_OK.
        */

        /* Call function with enable = GT_FALSE] */
        enable = GT_FALSE;

        st = cpssDxChPortCnModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
           1.2. Call cpssDxChPortCnModeEnableGet.
           Expected: GT_OK and the same enable.
        */
        st = cpssDxChPortCnModeEnableGet(dev, &state);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                        "[cpssDxChPortCnModeEnableGet]: %d, %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, state,
                        "get another enable value than was set: %d, %d", dev);

        /*
           1.1. Call with state [GT_FALSE and GT_TRUE].
           Expected: GT_OK.
        */

        /* Call function with enable = GT_TRUE] */
        enable = GT_TRUE;

        st = cpssDxChPortCnModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
           1.2. Call cpssDxChPortCnModeEnableGet.
           Expected: GT_OK and the same enable.
        */
        st = cpssDxChPortCnModeEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "[cpssDxChPortCnModeEnableGet]: %d, %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, state,
                      "get another enable value than was set: %d, %d", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    enable = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnModeEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnModeEnableGet
(
    IN   GT_U8     devNum,
    OUT  GT_BOOL   *statePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnModeEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat and above)
    1.1. Call with non-null statePtr.
    Expected: GT_OK.
    1.2. Call with statePtr [NULL].
    Expected: GT_BAD_PTR
*/

    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with non-null statePtr.
           Expected: GT_OK.
        */
        st = cpssDxChPortCnModeEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call with statePtr [NULL].
           Expected: GT_BAD_PTR
        */
        st = cpssDxChPortCnModeEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnModeEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnModeEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnPanicPauseThresholdsSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable,
    IN GT_U32   xoffLimit,
    IN GT_U32   xonLimit
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnPanicPauseThresholdsSet)
{
/*
    ITERATE_DEVICES (xCat,xCat2)
    1.1. Call with enable [GT_FALSE and GT_TRUE], xoffLimit [0 / 0x1FFF],
                  xonLimit  [0 / 0x1FFF].
    Expected: GT_OK.
    1.2. Call cpssDxChPortCnPanicPauseThresholdsGet with the same params.
    Expected: GT_OK and the same values.
    1.3. Call with enable [GT_FALSE],
                  xoffLimit [PORT_CN_INVALID_XONXOFFLIMIT_CNS],
                  xonLimit  [0].
    Expected: GT_OK.
    1.4. Call with enable [GT_FALSE], xoffLimit [0],
                  xonLimit  [PORT_CN_INVALID_XONXOFFLIMIT_CNS].
    Expected: GT_OK.
    1.5. Call with enable [GT_TRUE],
                  xoffLimit [PORT_CN_INVALID_XONXOFFLIMIT_CNS],
                  xonLimit  [0].
    Expected: GT_OUT_OF_RANGE.
    1.6. Call with enable [GT_TRUE], xoffLimit [0],
                  xonLimit  [PORT_CN_INVALID_XONXOFFLIMIT_CNS].
    Expected: GT_OUT_OF_RANGE.
*/

    GT_STATUS   st     = GT_OK;

    GT_U8    dev;
    GT_BOOL  enable = GT_FALSE;
    GT_U32   xoffLimit = 0;
    GT_U32   xonLimit = 0;

    GT_BOOL  enableGet = GT_FALSE;
    GT_U32   xoffLimitGet = 0;
    GT_U32   xonLimitGet = 0;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
           1.1. Call with enable [GT_FALSE and GT_TRUE], xoffLimit [0 /0x1FFF],
                          xonLimit  [0 /0x1FFF].
           Expected: GT_OK.
        */

        enable = GT_FALSE;
        xoffLimit = 0;
        xonLimit = 0;

        st = cpssDxChPortCnPanicPauseThresholdsSet(dev, enable, xoffLimit, xonLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* the functionality does not have "enable" bit.
           The "disable" is simulated by set limits to maximal value 0x1FFF. */
        xoffLimit = 0x1FFF;
        xonLimit = 0x1FFF;

        /*
            1.2. Call cpssDxChPortCnPanicPauseThresholdsGet with the same params.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChPortCnPanicPauseThresholdsGet(dev, &enableGet,
                                                   &xoffLimitGet, &xonLimitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                        "[cpssDxChPortCnPanicPauseThresholdsGet]: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                        "get another enable value than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xoffLimit, xoffLimitGet,
                        "get another xoffLimit value than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xonLimit, xonLimitGet,
                        "get another xonLimit value than was set: %d", dev);

        /*
           1.1. Call with enable [GT_FALSE and GT_TRUE],
                          xoffLimit [0 /0x1FFF],
                          xonLimit  [0 /0x1FFF].
           Expected: GT_OK.
        */

        enable = GT_TRUE;
        xoffLimit = 0x1FFF;
        xonLimit = 0x1FFF;

        st = cpssDxChPortCnPanicPauseThresholdsSet(dev, enable, xoffLimit, xonLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* the functionality does not have "enable" bit.
           The "disable" is simulated by set limits to maximal value 0x1FFF.
           So Get function return enable = GT_FALSE */
        enable = GT_FALSE;

        /*
            1.2. Call cpssDxChPortCnPanicPauseThresholdsGet with the same params.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChPortCnPanicPauseThresholdsGet(dev, &enableGet,
                                                   &xoffLimitGet, &xonLimitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                        "[cpssDxChPortCnPanicPauseThresholdsGet]: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                        "get another enable value than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xoffLimit, xoffLimitGet,
                        "get another xoffLimit value than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xonLimit, xonLimitGet,
                        "get another xonLimit value than was set: %d", dev);

        /*
            1.3. Call with enable [GT_FALSE],
                          xoffLimit [PORT_CN_INVALID_XONXOFFLIMIT_CNS],
                          xonLimit  [0].
            Expected: GT_OK.
        */

        enable = GT_FALSE;
        xoffLimit = PORT_CN_INVALID_XONXOFFLIMIT_CNS;
        xonLimit = 0;

        st = cpssDxChPortCnPanicPauseThresholdsSet(dev, enable, xoffLimit, xonLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        xoffLimit = 0;

        /*
            1.4. Call with enable [GT_FALSE],
                          xoffLimit [0],
                          xonLimit  [PORT_CN_INVALID_XONXOFFLIMIT_CNS].
            Expected: GT_OK.
        */

        enable = GT_FALSE;
        xoffLimit = 0;
        xonLimit = PORT_CN_INVALID_XONXOFFLIMIT_CNS;

        st = cpssDxChPortCnPanicPauseThresholdsSet(dev, enable, xoffLimit, xonLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        xonLimit = 0;

        /*
            1.5. Call with enable [GT_TRUE],
                          xoffLimit [PORT_CN_INVALID_XONXOFFLIMIT_CNS],
                          xonLimit  [0].
            Expected: GT_OUT_OF_RANGE.
        */

        enable = GT_TRUE;
        xoffLimit = PORT_CN_INVALID_XONXOFFLIMIT_CNS;
        xonLimit = 0;

        st = cpssDxChPortCnPanicPauseThresholdsSet(dev, enable, xoffLimit, xonLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, enable);

        xoffLimit = 0;

        /*
            1.6. Call with enable [GT_TRUE], xoffLimit [0],
                          xonLimit  [PORT_CN_INVALID_XONXOFFLIMIT_CNS].
            Expected: GT_OUT_OF_RANGE.
        */

        enable = GT_TRUE;
        xoffLimit = 0;
        xonLimit = PORT_CN_INVALID_XONXOFFLIMIT_CNS;

        st = cpssDxChPortCnPanicPauseThresholdsSet(dev, enable, xoffLimit, xonLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, enable);

        xonLimit = 0;
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    enable = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnPanicPauseThresholdsSet(dev, enable, xoffLimit, xonLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnPanicPauseThresholdsSet(dev, enable, xoffLimit, xonLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnPanicPauseThresholdsGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr,
    OUT GT_U32   *xoffLimitPtr,
    OUT GT_U32   *xonLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnPanicPauseThresholdsGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat,xCat2)
    1.1. Call with non-null pointers.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR
    1.3. Call with xoffLimitPtr [NULL].
    Expected: GT_BAD_PTR
    1.4. Call with xonLimitPtr [NULL].
    Expected: GT_BAD_PTR
*/

    GT_STATUS   st    = GT_OK;

    GT_U8    dev;
    GT_BOOL  enableGet;
    GT_U32   xoffLimitGet;
    GT_U32   xonLimitGet;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
           1.1. Call with non-null statePtr.
           Expected: GT_OK.
        */
        st = cpssDxChPortCnPanicPauseThresholdsGet(dev, &enableGet,
                                              &xoffLimitGet, &xonLimitGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
           1.2. Call with enableGet [NULL].
           Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnPanicPauseThresholdsGet(dev, NULL,
                                              &xoffLimitGet, &xonLimitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enable is NULL", dev);

        /*
           1.3. Call with xoffLimitGet [NULL].
           Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnPanicPauseThresholdsGet(dev, &enableGet,
                                              NULL, &xonLimitGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, xoffLimitGet is NULL", dev);

        /*
           1.4. Call with xonLimitGet [NULL].
           Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnPanicPauseThresholdsGet(dev, &enableGet,
                                              &xoffLimitGet, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, xonLimitGet is NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnPanicPauseThresholdsGet(dev, &enableGet, &xoffLimitGet, &xonLimitGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnPanicPauseThresholdsGet(dev, &enableGet, &xoffLimitGet, &xonLimitGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnPauseTriggerEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnPauseTriggerEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat,xCat2)
    1.1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortCnPauseTriggerEnableGet.
    Expected: GT_OK and the same enable.
*/

    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_CN_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;
    GT_BOOL     state  = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
               1.1.1. Call with state [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_FALSE] */
            enable = GT_FALSE;

            st = cpssDxChPortCnPauseTriggerEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
               1.1.2. Call cpssDxChPortCnPauseTriggerEnableGet.
               Expected: GT_OK and the same enable.
            */
            st = cpssDxChPortCnPauseTriggerEnableGet(dev, port, &state);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "[cpssDxChPortCnPauseTriggerEnableGet]: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, state,
                            "get another enable value than was set: %d, %d", dev, port);

            /*
               1.1.1. Call with state [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_TRUE] */
            enable = GT_TRUE;

            st = cpssDxChPortCnPauseTriggerEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
               1.1.2. Call cpssDxChPortCnPauseTriggerEnableGet.
               Expected: GT_OK and the same enable.
            */
            st = cpssDxChPortCnPauseTriggerEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                          "[cpssDxChPortCnPauseTriggerEnableGet]: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, state,
                          "get another enable value than was set: %d, %d", dev, port);
        }

        /*
           1.2. For all active devices go over all non available
           physical ports.
        */
        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* enable == GT_TRUE    */
            st = cpssDxChPortCnPauseTriggerEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* enable == GT_TRUE  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortCnPauseTriggerEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                     */
        /* enable == GT_TRUE  */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortCnPauseTriggerEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, enable);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    enable = GT_TRUE;
    port = PORT_CN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnPauseTriggerEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, enable == GT_TRUE */

    st = cpssDxChPortCnPauseTriggerEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnPauseTriggerEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *statePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnPauseTriggerEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat,xCat2)
    1.1.1. Call with non-null statePtr.
    Expected: GT_OK.
    1.1.2. Call with statePtr [NULL].
    Expected: GT_BAD_PTR
*/

    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = PORT_CN_VALID_PHY_PORT_CNS;
    GT_BOOL     state = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null statePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortCnPauseTriggerEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with statePtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortCnPauseTriggerEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortCnPauseTriggerEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortCnPauseTriggerEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortCnPauseTriggerEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnPauseTriggerEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnPauseTriggerEnableGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnProfileSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   profileIndex
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnProfileSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat,xCat2)
    1.1.1. Call with profileIndex [0 / 7].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortCnProfileGet with the same params.
    Expected: GT_OK and the same profileIndex.
    1.1.3. Call with profileIndex [PORT_CN_INVALID_PROFILEINDEX_CNS].
    Expected: GT_OUT_OF_RANGE.
*/

    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_CN_VALID_PHY_PORT_CNS;
    GT_U32      profileIndex = 0;
    GT_U32      profileIndexGet = 0;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with profileIndex [0 / 7].
                Expected: GT_OK.
            */

            profileIndex = 0;

            st = cpssDxChPortCnProfileSet(dev, port, profileIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileIndex);

            /*
               1.1.2. Call cpssDxChPortCnProfileGet.
               Expected: GT_OK and the same profileIndex.
            */
            st = cpssDxChPortCnProfileGet(dev, port, &profileIndexGet);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "[cpssDxChPortCnProfileGet]: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(profileIndex, profileIndexGet,
                            "get another profileIndex value than was set: %d, %d", dev, port);

            /*
                1.1.1. Call with profileIndex [0 / 7].
                Expected: GT_OK.
            */

            /* Call function with profileIndex = GT_TRUE] */
            profileIndex = 7;

            st = cpssDxChPortCnProfileSet(dev, port, profileIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileIndex);

            /*
               1.1.2. Call cpssDxChPortCnProfileGet.
               Expected: GT_OK and the same profileIndex.
            */
            st = cpssDxChPortCnProfileGet(dev, port, &profileIndexGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                          "[cpssDxChPortCnProfileGet]: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(profileIndex, profileIndexGet,
                          "get another profileIndex value than was set: %d, %d", dev, port);
        }

        /*
           1.2. For all active devices go over all non available
           physical ports.
        */

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* profileIndex == GT_TRUE    */
            st = cpssDxChPortCnProfileSet(dev, port, profileIndex);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortCnProfileSet(dev, port, profileIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortCnProfileSet(dev, port, profileIndex);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, profileIndex);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    profileIndex = GT_TRUE;
    port = PORT_CN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnProfileSet(dev, port, profileIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnProfileSet(dev, port, profileIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnProfileGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U32  *profileIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnProfileGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat,xCat2)
    1.1.1. Call with non-null profileIndexGetPtr.
    Expected: GT_OK.
    1.1.2. Call with profileIndexGetPtr [NULL].
    Expected: GT_BAD_PTR
*/

    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = PORT_CN_VALID_PHY_PORT_CNS;
    GT_U32      profileIndexGet = 0;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null profileIndexGetPtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortCnProfileGet(dev, port, &profileIndexGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with profileIndexGetPtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortCnProfileGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortCnProfileGet(dev, port, &profileIndexGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortCnProfileGet(dev, port, &profileIndexGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortCnProfileGet(dev, port, &profileIndexGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnProfileGet(dev, port, &profileIndexGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnProfileGet(dev, port, &profileIndexGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnProfileQueueThresholdSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U8    tcQueue,
    IN GT_U32   threshold
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnProfileQueueThresholdSet)
{
/*
    ITERATE_DEVICES (xCat,xCat2)
    1.1. Call with profileIndex [0 / 3 / 7], tcQueue [0 / 3 / 7]
         and threshold [0 / 0x1F00 / 0x1FFF].
    Expected: GT_OK.
    1.2. Call cpssDxChPortCnProfileQueueThresholdGet with not-NULL thresholdPtr.
    Expected: GT_OK and the same threshold as was set.
    1.3. Call with wrong profileIndex [PORT_CN_INVALID_PROFILEINDEX_CNS].
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong tcQueue [PORT_CN_INVALID_TCQUEUE_CNS].
    Expected: GT_BAD_PARAM.
    1.5. Call with wrong threshold [PORT_CN_INVALID_THRESHOLD_CNS].
    Expected: GT_OUT_OF_RANGE.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32   profileIndex = 0;
    GT_U8    tcQueue = 0;
    GT_U32   threshold = 0;
    GT_U32   thresholdGet = 1;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileIndex [0 / 3 / 7], tcQueue [0 / 3 / 7]
                 and threshold [0 / 0x1F00 / 0x1FFF].
            Expected: GT_OK.
        */
        /*Call with profileIndex [0], tcQueue [0] threshold [0].*/
        profileIndex = 0;
        tcQueue = 0;
        threshold = 0;

        st = cpssDxChPortCnProfileQueueThresholdSet(dev, profileIndex, tcQueue, threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnProfileQueueThresholdGet with not-NULL thresholdPtr.
            Expected: GT_OK and the same threshold as was set.
        */
        st = cpssDxChPortCnProfileQueueThresholdGet(dev, profileIndex, tcQueue, &thresholdGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                "cpssDxChPortCnProfileQueueThresholdGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(threshold, thresholdGet,
                                "got another threshold then was set: %d", dev);

        /*
            1.1. Call with profileIndex [0 / 3 / 7], tcQueue [0 / 3 / 7]
                 and threshold [0 / 0x1F00 / 0x1FFF].
            Expected: GT_OK.
        */
        /*Call with profileIndex [3], tcQueue [3] threshold [0x1F00].*/
        profileIndex = 3;
        tcQueue = 3;
        threshold = 0x1F00;

        st = cpssDxChPortCnProfileQueueThresholdSet(dev, profileIndex, tcQueue, threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnProfileQueueThresholdGet with not-NULL thresholdPtr.
            Expected: GT_OK and the same threshold as was set.
        */
        st = cpssDxChPortCnProfileQueueThresholdGet(dev, profileIndex, tcQueue, &thresholdGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                "cpssDxChPortCnProfileQueueThresholdGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(threshold, thresholdGet,
                                "got another threshold then was set: %d", dev);

        /*
            1.1. Call with profileIndex [0 / 3 / 7], tcQueue [0 / 3 / 7]
                 and threshold [0 / 0x1F00 / 0x1FFF].
            Expected: GT_OK.
        */
        /*Call with profileIndex [7], tcQueue [7] threshold [0x1FFF].*/
        profileIndex = 7;
        tcQueue = 7;
        threshold = 0x1FFF;

        st = cpssDxChPortCnProfileQueueThresholdSet(dev, profileIndex, tcQueue, threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnProfileQueueThresholdGet with not-NULL thresholdPtr.
            Expected: GT_OK and the same threshold as was set.
        */
        st = cpssDxChPortCnProfileQueueThresholdGet(dev, profileIndex, tcQueue, &thresholdGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                "cpssDxChPortCnProfileQueueThresholdGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(threshold, thresholdGet,
                                "got another threshold then was set: %d", dev);


        /*
            1.3. Call with wrong profileIndex [PORT_CN_INVALID_PROFILEINDEX_CNS].
            Expected: GT_BAD_PARAM.
        */
        profileIndex = PORT_CN_INVALID_PROFILEINDEX_CNS;

        st = cpssDxChPortCnProfileQueueThresholdSet(dev, profileIndex, tcQueue, threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        profileIndex = 7;

        /*
            1.4. Call with wrong tcQueue [PORT_CN_INVALID_TCQUEUE_CNS].
            Expected: GT_BAD_PARAM.
        */
        tcQueue = PORT_CN_INVALID_TCQUEUE_CNS;

        st = cpssDxChPortCnProfileQueueThresholdSet(dev, profileIndex, tcQueue, threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        tcQueue = 7;

        /*
            1.5. Call with wrong threshold [PORT_CN_INVALID_THRESHOLD_CNS].
            Expected: GT_OUT_OF_RANGE.
        */
        threshold = PORT_CN_INVALID_THRESHOLD_CNS;

        st = cpssDxChPortCnProfileQueueThresholdSet(dev, profileIndex, tcQueue, threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);

        threshold = 0x1FFF;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnProfileQueueThresholdSet(dev, profileIndex, tcQueue, threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnProfileQueueThresholdSet(dev, profileIndex, tcQueue, threshold);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnProfileQueueThresholdGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   profileIndex,
    IN  GT_U8    tcQueue,
    OUT GT_U32   *thresholdPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnProfileQueueThresholdGet)
{
/*
    ITERATE_DEVICES (xCat,xCat2)
    1.1. Call with profileIndex [0 / 7], tcQueue [0 / 7] and not null threshold.
    Expected: GT_OK.
    1.2. Call with wrong profileIndex [PORT_CN_INVALID_PROFILEINDEX_CNS].
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong tcQueue [PORT_CN_INVALID_TCQUEUE_CNS].
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong  thresholdPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      profileIndex = 0;
    GT_U8       tcQueue = 0;
    GT_U32      threshold;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileIndex [0 / 7], tcQueue [0 / 7] and not null threshold.
            Expected: GT_OK.
        */
        profileIndex = 0;
        tcQueue = 0;

        st = cpssDxChPortCnProfileQueueThresholdGet(dev, profileIndex, tcQueue, &threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.1. Call with profileIndex [0 / 7], tcQueue [0 / 7] and not null threshold.
            Expected: GT_OK.
        */
        profileIndex = 7;
        tcQueue = 7;

        st = cpssDxChPortCnProfileQueueThresholdGet(dev, profileIndex, tcQueue, &threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong profileIndex [PORT_CN_INVALID_PROFILEINDEX_CNS].
            Expected: GT_BAD_PARAM.
        */
        profileIndex = PORT_CN_INVALID_PROFILEINDEX_CNS;

        st = cpssDxChPortCnProfileQueueThresholdGet(dev, profileIndex, tcQueue, &threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        profileIndex = 7;

        /*
            1.3. Call with wrong tcQueue [PORT_CN_INVALID_TCQUEUE_CNS].
            Expected: GT_BAD_PARAM.
        */
        tcQueue = PORT_CN_INVALID_TCQUEUE_CNS;

        st = cpssDxChPortCnProfileQueueThresholdGet(dev, profileIndex, tcQueue, &threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        tcQueue = 7;

        /*
            1.4. Call with wrong  threshold pointer [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnProfileQueueThresholdGet(dev, profileIndex, tcQueue, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, threshold = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnProfileQueueThresholdGet(dev, profileIndex, tcQueue, &threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnProfileQueueThresholdGet(dev, profileIndex, tcQueue, &threshold);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnProfileThresholdSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U32   threshold
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnProfileThresholdSet)
{
/*
    ITERATE_DEVICES (xCat,xCat2)
    1.1. Call with profileIndex [0 / 3 / 7] and threshold [0 / 0x1F00 / 0x1FFF].
    Expected: GT_OK.
    1.2. Call cpssDxChPortCnProfileThresholdGet with not-NULL thresholdPtr.
    Expected: GT_OK and the same threshold as was set.
    1.3. Call with wrong profileIndex [PORT_CN_INVALID_PROFILEINDEX_CNS].
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong threshold [PORT_CN_INVALID_THRESHOLD_CNS].
    Expected: GT_OUT_OF_RANGE.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32   profileIndex = 0;
    GT_U32   threshold = 0;
    GT_U32   thresholdGet = 1;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileIndex [0 / 3 / 7] and threshold [0 / 0x1F00 / 0x1FFF].
            Expected: GT_OK.
        */
        /*Call with profileIndex [0], threshold [0].*/
        profileIndex = 0;
        threshold = 0;

        st = cpssDxChPortCnProfileThresholdSet(dev, profileIndex, threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnProfileThresholdGet with not-NULL thresholdPtr.
            Expected: GT_OK and the same threshold as was set.
        */
        st = cpssDxChPortCnProfileThresholdGet(dev, profileIndex, &thresholdGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                "cpssDxChPortCnProfileThresholdGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(threshold, thresholdGet,
                                "got another threshold then was set: %d", dev);

        /*
            1.1. Call with profileIndex [0 / 3 / 7] and threshold [0 / 0x1F00 / 0x1FFF].
            Expected: GT_OK.
        */
        /*Call with profileIndex [3] threshold [0x1F00].*/
        profileIndex = 3;
        threshold = 0x1F00;

        st = cpssDxChPortCnProfileThresholdSet(dev, profileIndex, threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnProfileThresholdGet with not-NULL thresholdPtr.
            Expected: GT_OK and the same threshold as was set.
        */
        st = cpssDxChPortCnProfileThresholdGet(dev, profileIndex, &thresholdGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                "cpssDxChPortCnProfileThresholdGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(threshold, thresholdGet,
                                "got another threshold then was set: %d", dev);

        /*
            1.1. Call with profileIndex [0 / 7] and threshold [0 / 0x1FFF].
            Expected: GT_OK.
        */
        /*Call with profileIndex [7], threshold [0x1FFF].*/
        profileIndex = 7;
        threshold = 0x1FFF;

        st = cpssDxChPortCnProfileThresholdSet(dev, profileIndex, threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnProfileThresholdGet with not-NULL thresholdPtr.
            Expected: GT_OK and the same threshold as was set.
        */
        st = cpssDxChPortCnProfileThresholdGet(dev, profileIndex, &thresholdGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                "cpssDxChPortCnProfileThresholdGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(threshold, thresholdGet,
                                "got another threshold then was set: %d", dev);

        /*
            1.3. Call with wrong profileIndex [PORT_CN_INVALID_PROFILEINDEX_CNS].
            Expected: GT_BAD_PARAM.
        */
        profileIndex = PORT_CN_INVALID_PROFILEINDEX_CNS;

        st = cpssDxChPortCnProfileThresholdSet(dev, profileIndex, threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        profileIndex = 7;

        /*
            1.4. Call with wrong threshold [PORT_CN_INVALID_THRESHOLD_CNS].
            Expected: GT_OUT_OF_RANGE.
        */
        threshold = PORT_CN_INVALID_THRESHOLD_CNS;

        st = cpssDxChPortCnProfileThresholdSet(dev, profileIndex, threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);

        threshold = 0x1FFF;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnProfileThresholdSet(dev, profileIndex, threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnProfileThresholdSet(dev, profileIndex, threshold);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnProfileThresholdGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   profileIndex,
    OUT GT_U32   *thresholdPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnProfileThresholdGet)
{
/*
    ITERATE_DEVICES (xCat,xCat2)
    1.1. Call with profileIndex [0 / 7] and not null threshold.
    Expected: GT_OK.
    1.2. Call with wrong profileIndex [PORT_CN_INVALID_PROFILEINDEX_CNS].
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong  thresholdPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      profileIndex = 0;
    GT_U32      threshold;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileIndex [0 / 7] and not null threshold.
            Expected: GT_OK.
        */
        profileIndex = 0;

        st = cpssDxChPortCnProfileThresholdGet(dev, profileIndex, &threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.1. Call with profileIndex [0 / 7] and not null threshold.
            Expected: GT_OK.
        */
        profileIndex = 7;

        st = cpssDxChPortCnProfileThresholdGet(dev, profileIndex, &threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong profileIndex [PORT_CN_INVALID_PROFILEINDEX_CNS].
            Expected: GT_BAD_PARAM.
        */
        profileIndex = PORT_CN_INVALID_PROFILEINDEX_CNS;

        st = cpssDxChPortCnProfileThresholdGet(dev, profileIndex, &threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        profileIndex = 7;

        /*
            1.3. Call with wrong  threshold pointer [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnProfileThresholdGet(dev, profileIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, threshold = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnProfileThresholdGet(dev, profileIndex, &threshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnProfileThresholdGet(dev, profileIndex, &threshold);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnQueueAwareEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnQueueAwareEnableSet)
{
/*
    ITERATE_DEVICES (xCat,xCat2)
    1.1. Call with tcQueue [0 / 7] and enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChPortCnQueueAwareEnableGet with not-NULL enablePtr.
    Expected: GT_OK and the same enable as was set.
    1.3. Call with wrong tcQueue [PORT_CN_INVALID_TCQUEUE_CNS].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U8   tcQueue = 0;
    GT_BOOL  enable = GT_TRUE;
    GT_BOOL  enableGet = 1;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcQueue [0 / 7] and enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /*Call with tcQueue [0], enable [GT_TRUE].*/
        tcQueue = 0;
        enable = GT_TRUE;

        st = cpssDxChPortCnQueueAwareEnableSet(dev, tcQueue, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnQueueAwareEnableGet with not-NULL enablePtr.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssDxChPortCnQueueAwareEnableGet(dev, tcQueue, &enableGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                "cpssDxChPortCnQueueAwareEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                                "got another enable then was set: %d", dev);

        /*
            1.1. Call with tcQueue [0 / 7] and enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /*Call with tcQueue [7] enable [GT_FALSE].*/
        tcQueue = 7;
        enable = GT_FALSE;

        st = cpssDxChPortCnQueueAwareEnableSet(dev, tcQueue, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnQueueAwareEnableGet with not-NULL enablePtr.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssDxChPortCnQueueAwareEnableGet(dev, tcQueue, &enableGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                "cpssDxChPortCnQueueAwareEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                                "got another enable then was set: %d", dev);


        /*
            1.3. Call with wrong tcQueue [PORT_CN_INVALID_TCQUEUE_CNS].
            Expected: GT_BAD_PARAM.
        */
        tcQueue = PORT_CN_INVALID_TCQUEUE_CNS;

        st = cpssDxChPortCnQueueAwareEnableSet(dev, tcQueue, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        tcQueue = 7;

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnQueueAwareEnableSet(dev, tcQueue, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnQueueAwareEnableSet(dev, tcQueue, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnQueueAwareEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   tcQueue,
    OUT GT_U32   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnQueueAwareEnableGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with tcQueue [0 / 7] and not null enable.
    Expected: GT_OK.
    1.2. Call with wrong tcQueue [PORT_CN_INVALID_TCQUEUE_CNS].
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong  enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U8       tcQueue = 0;
    GT_BOOL     enable = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcQueue [0 / 7] and not null enable.
            Expected: GT_OK.
        */
        tcQueue = 0;

        st = cpssDxChPortCnQueueAwareEnableGet(dev, tcQueue, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.1. Call with tcQueue [0 / 7] and not null enable.
            Expected: GT_OK.
        */
        tcQueue = 7;

        st = cpssDxChPortCnQueueAwareEnableGet(dev, tcQueue, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong tcQueue [PORT_CN_INVALID_TCQUEUE_CNS].
            Expected: GT_BAD_PARAM.
        */
        tcQueue = PORT_CN_INVALID_TCQUEUE_CNS;

        st = cpssDxChPortCnQueueAwareEnableGet(dev, tcQueue, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        tcQueue = 7;

        /*
            1.3. Call with wrong  enable pointer [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnQueueAwareEnableGet(dev, tcQueue, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enable = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnQueueAwareEnableGet(dev, tcQueue, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnQueueAwareEnableGet(dev, tcQueue, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnSpeedIndexSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   portSpeedIndex
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnSpeedIndexSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat,xCat2)
    1.1.1. Call with portSpeedIndex [0 / 7].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortCnSpeedIndexGet with the same params.
    Expected: GT_OK and the same portSpeedIndex.
    1.1.3. Call with portSpeedIndex [PORT_CN_INVALID_PORTSPEEDINDEX_CNS].
    Expected: GT_OUT_OF_RANGE.
*/

    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_CN_VALID_PHY_PORT_CNS;
    GT_U32      portSpeedIndex = 0;
    GT_U32      portSpeedIndexGet = 0;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with portSpeedIndex [0 / 7].
                Expected: GT_OK.
            */
            portSpeedIndex = 0;

            st = cpssDxChPortCnSpeedIndexSet(dev, port, portSpeedIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, portSpeedIndex);

            /*
               1.1.2. Call cpssDxChPortCnSpeedIndexGet.
               Expected: GT_OK and the same portSpeedIndex.
            */
            st = cpssDxChPortCnSpeedIndexGet(dev, port, &portSpeedIndexGet);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "[cpssDxChPortCnSpeedIndexGet]: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portSpeedIndex, portSpeedIndexGet,
                      "get another portSpeedIndex value than was set: %d, %d", dev, port);

            /*
                1.1.1. Call with portSpeedIndex [0 / 7].
                Expected: GT_OK.
            */
            /* Call function with portSpeedIndex = GT_TRUE] */
            portSpeedIndex = 7;

            st = cpssDxChPortCnSpeedIndexSet(dev, port, portSpeedIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, portSpeedIndex);

            /*
               1.1.2. Call cpssDxChPortCnSpeedIndexGet.
               Expected: GT_OK and the same portSpeedIndex.
            */
            st = cpssDxChPortCnSpeedIndexGet(dev, port, &portSpeedIndexGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                          "[cpssDxChPortCnSpeedIndexGet]: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(portSpeedIndex, portSpeedIndexGet,
                    "get another portSpeedIndex value than was set: %d, %d", dev, port);
        }

        /*
           1.2. For all active devices go over all non available
           physical ports.
        */

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortCnSpeedIndexSet(dev, port, portSpeedIndex);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortCnSpeedIndexSet(dev, port, portSpeedIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortCnSpeedIndexSet(dev, port, portSpeedIndex);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, portSpeedIndex);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnSpeedIndexSet(dev, port, portSpeedIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnSpeedIndexSet(dev, port, portSpeedIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnSpeedIndexGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_U32   *portSpeedIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnSpeedIndexGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat,xCat2)
    1.1.1. Call with non-null portSpeedIndexGetPtr.
    Expected: GT_OK.
    1.1.2. Call with portSpeedIndexGetPtr [NULL].
    Expected: GT_BAD_PTR
*/

    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = PORT_CN_VALID_PHY_PORT_CNS;
    GT_U32      portSpeedIndexGet = 0;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null portSpeedIndexGetPtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortCnSpeedIndexGet(dev, port, &portSpeedIndexGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with portSpeedIndexGetPtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortCnSpeedIndexGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortCnSpeedIndexGet(dev, port, &portSpeedIndexGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortCnSpeedIndexGet(dev, port, &portSpeedIndexGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortCnSpeedIndexGet(dev, port, &portSpeedIndexGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_LION_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnSpeedIndexGet(dev, port, &portSpeedIndexGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnSpeedIndexGet(dev, port, &portSpeedIndexGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnFbCalcConfigSet
(
    IN GT_U8    devNum,
    IN CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC *fbCalcCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnFbCalcConfigSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with all correct values
                                   fbCalcCfg.wExp[0 / 5];
                                   fbCalcCfg.fbLsb[0 / 0x1A];
                                   fbCalcCfg.deltaEnable[GT_TRUE / GT_FALSE];
                                   fbCalcCfg.fbMin[0 / 0xFFFFA];
                                   fbCalcCfg.fbMax[0 / 0xFFFFA];
    Expected: GT_OK.
    1.2. Call cpssDxChPortCnFbCalcConfigGet with not-NULL fbCalcCfgPtr.
    Expected: GT_OK and the same fbCalcCfgPtr  as was set.
    1.3. Call with wrong wExp (out of range):
                                   fbCalcCfg.wExp[8];
                                   fbCalcCfg.fbLsb[0];
                                   fbCalcCfg.deltaEnable[GT_TRUE];
                                   fbCalcCfg.fbMin[0];
                                   fbCalcCfg.fbMax[0];
    Expected: GT_OUT_OF_RANGE.
    1.4. Call with wrong fbLsb (out of range):
                                   fbCalcCfg.wExp[0];
                                   fbCalcCfg.fbLsb[0x20];
                                   fbCalcCfg.deltaEnable[GT_TRUE];
                                   fbCalcCfg.fbMin[0];
                                   fbCalcCfg.fbMax[0];
    Expected: GT_OUT_OF_RANGE.
    1.5. Call with wrong fbMin (out of range):
                                   fbCalcCfg.wExp[0];
                                   fbCalcCfg.fbLsb[0];
                                   fbCalcCfg.deltaEnable[GT_TRUE];
                                   fbCalcCfg.fbMin[0x100000];
                                   fbCalcCfg.fbMax[0];
    Expected: GT_OUT_OF_RANGE.
    1.6. Call with wrong fbMax (out of range):
                                   fbCalcCfg.wExp[0];
                                   fbCalcCfg.fbLsb[0];
                                   fbCalcCfg.deltaEnable[GT_TRUE];
                                   fbCalcCfg.fbMin[0];
                                   fbCalcCfg.fbMax[0x100000];
    Expected: GT_OUT_OF_RANGE.
    1.7. Call with wrong fbCalcCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC fbCalcCfg;
    CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC fbCalcCfgGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with all correct values
                                           fbCalcCfg.wExp[0];
                                           fbCalcCfg.fbLsb[0];
                                           fbCalcCfg.deltaEnable[GT_TRUE];
                                           fbCalcCfg.fbMin[0];
                                           fbCalcCfg.fbMax[0];
            Expected: GT_OK.
        */
        fbCalcCfg.wExp = 0;
        fbCalcCfg.fbLsb = 0;
        fbCalcCfg.deltaEnable = GT_TRUE;
        fbCalcCfg.fbMin = 0;
        fbCalcCfg.fbMax = 0;

        st = cpssDxChPortCnFbCalcConfigSet(dev, &fbCalcCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnFbCalcConfigGet with not-NULL fbCalcCfgPtr.
            Expected: GT_OK and the same fbCalcCfg values.
        */
        st = cpssDxChPortCnFbCalcConfigGet(dev, &fbCalcCfgGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortCnFbCalcConfigGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(fbCalcCfg.wExp, fbCalcCfgGet.wExp,
                         "got another fbCalcCfg.wExp then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fbCalcCfg.fbLsb, fbCalcCfgGet.fbLsb,
                         "got another fbCalcCfg.fbLsb then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fbCalcCfg.deltaEnable, fbCalcCfgGet.deltaEnable,
                         "got another fbCalcCfg.deltaEnable then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fbCalcCfg.fbMin, fbCalcCfgGet.wExp,
                         "got another fbCalcCfg.fbMin then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fbCalcCfg.fbMax, fbCalcCfgGet.wExp,
                         "got another fbCalcCfg.fbMax then was set: %d", dev);

        /*
            1.1. Call with all correct values
                                           fbCalcCfg.wExp[5];
                                           fbCalcCfg.fbLsb[0x1A];
                                           fbCalcCfg.deltaEnable[GT_FALSE];
                                           fbCalcCfg.fbMin[0xFFFFA];
                                           fbCalcCfg.fbMax[0xFFFFA];
            Expected: GT_OK.
        */
        fbCalcCfg.wExp = 5;
        fbCalcCfg.fbLsb = 0x1A;
        fbCalcCfg.deltaEnable = GT_FALSE;
        fbCalcCfg.fbMin = 0xFFFFA;
        fbCalcCfg.fbMax = 0xFFFFA;

        st = cpssDxChPortCnFbCalcConfigSet(dev, &fbCalcCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnFbCalcConfigGet with not-NULL fbCalcCfgPtr.
            Expected: GT_OK and the same fbCalcCfg values.
        */
        st = cpssDxChPortCnFbCalcConfigGet(dev, &fbCalcCfgGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortCnFbCalcConfigGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(fbCalcCfg.wExp, fbCalcCfgGet.wExp,
                         "got another fbCalcCfg.wExp then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fbCalcCfg.fbLsb, fbCalcCfgGet.fbLsb,
                         "got another fbCalcCfg.fbLsb then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fbCalcCfg.deltaEnable, fbCalcCfgGet.deltaEnable,
                         "got another fbCalcCfg.deltaEnable then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fbCalcCfg.fbMin, fbCalcCfgGet.fbMin,
                         "got another fbCalcCfg.fbMin then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fbCalcCfg.fbMax, fbCalcCfgGet.fbMax,
                         "got another fbCalcCfg.fbMax then was set: %d", dev);

        /*
            1.3. Call with wrong wExp (out of range):
                                           fbCalcCfg.wExp[8];
                                           fbCalcCfg.fbLsb[0];
                                           fbCalcCfg.deltaEnable[GT_TRUE];
                                           fbCalcCfg.fbMin[0];
                                           fbCalcCfg.fbMax[0];
            Expected: GT_OUT_OF_RANGE.
        */
        fbCalcCfg.wExp = 8;
        fbCalcCfg.fbLsb = 0;
        fbCalcCfg.deltaEnable = GT_TRUE;
        fbCalcCfg.fbMin = 0;
        fbCalcCfg.fbMax = 0;

        st = cpssDxChPortCnFbCalcConfigSet(dev, &fbCalcCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);

        /*
            1.4. Call with wrong fbLsb (out of range):
                                               fbCalcCfg.wExp[0];
                                               fbCalcCfg.fbLsb[0x20];
                                               fbCalcCfg.deltaEnable[GT_TRUE];
                                               fbCalcCfg.fbMin[0];
                                               fbCalcCfg.fbMax[0];
            Expected: GT_OUT_OF_RANGE.
        */
        fbCalcCfg.wExp = 0;
        fbCalcCfg.fbLsb = 0x20;
        fbCalcCfg.deltaEnable = GT_TRUE;
        fbCalcCfg.fbMin = 0;
        fbCalcCfg.fbMax = 0;

        st = cpssDxChPortCnFbCalcConfigSet(dev, &fbCalcCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);

        /*
            1.5. Call with wrong fbMin (out of range):
                                           fbCalcCfg.wExp[0];
                                           fbCalcCfg.fbLsb[0];
                                           fbCalcCfg.deltaEnable[GT_TRUE];
                                           fbCalcCfg.fbMin[0x100000];
                                           fbCalcCfg.fbMax[0];
            Expected: GT_OUT_OF_RANGE.
        */
        fbCalcCfg.wExp = 0;
        fbCalcCfg.fbLsb = 0;
        fbCalcCfg.deltaEnable = GT_TRUE;
        fbCalcCfg.fbMin = 0x100000;
        fbCalcCfg.fbMax = 0;

        st = cpssDxChPortCnFbCalcConfigSet(dev, &fbCalcCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);

        /*
            1.6. Call with wrong fbMax (out of range):
                                           fbCalcCfg.wExp[0];
                                           fbCalcCfg.fbLsb[0];
                                           fbCalcCfg.deltaEnable[GT_TRUE];
                                           fbCalcCfg.fbMin[0];
                                           fbCalcCfg.fbMax[0x100000];
            Expected: GT_OUT_OF_RANGE.
        */
        fbCalcCfg.wExp = 0;
        fbCalcCfg.fbLsb = 0;
        fbCalcCfg.deltaEnable = GT_TRUE;
        fbCalcCfg.fbMin = 0;
        fbCalcCfg.fbMax = 0x100000;

        st = cpssDxChPortCnFbCalcConfigSet(dev, &fbCalcCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);

        /*
            1.7. Call with wrong fbCalcCfgPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnFbCalcConfigSet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    fbCalcCfg.wExp = 0;
    fbCalcCfg.fbLsb = 0;
    fbCalcCfg.deltaEnable = GT_TRUE;
    fbCalcCfg.fbMin = 0;
    fbCalcCfg.fbMax = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnFbCalcConfigSet(dev, &fbCalcCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnFbCalcConfigSet(dev, &fbCalcCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnFbCalcConfigGet
(
    IN  GT_U8    devNum,
    OUT CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC *fbCalcCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnFbCalcConfigGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with not null fbCalcCfgPtr.
    Expected: GT_OK.
    1.2. Call with wrong fbCalcCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC fbCalcCfg;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null fbCalcCfgPtr.
            Expected: GT_OK.
        */
        st = cpssDxChPortCnFbCalcConfigGet(dev, &fbCalcCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong fbCalcCfgPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnFbCalcConfigGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnFbCalcConfigGet(dev, &fbCalcCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnFbCalcConfigGet(dev, &fbCalcCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnMessageGenerationConfigSet
(
    IN GT_U8    devNum,
    IN CPSS_PORT_CNM_GENERATION_CONFIG_STC *cnmGenerationCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnMessageGenerationConfigSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with all correct values
                       cnmGenerationCfg.qosProfileId [0 / 0x7F]
                       cnmGenerationCfg.isRouted [0 / 1]
                       cnmGenerationCfg.overrideUp [GT_TRUE / GT_FALSE]
                       cnmGenerationCfg.cnmUp [0 / 10]
                       cnmGenerationCfg.defaultVlanId [0 / 0xFFF]
                       cnmGenerationCfg.scaleFactor [256 / 512]
                       cnmGenerationCfg.version [0 / 0xF]
                       cnmGenerationCfg.cpidMsb [0 / 10]
                       cnmGenerationCfg.cnUntaggedEnable [GT_TRUE / GT_FALSE]
                       cnmGenerationCfg.forceCnTag [GT_TRUE / GT_FALSE]
                       cnmGenerationCfg.flowIdTag [0 / 0xFFFF]
                       cnmGenerationCfg.appendPacket [GT_TRUE / GT_FALSE]

    Expected: GT_OK.
    1.2. Call cpssDxChPortCnMessageGenerationConfigGet with
         not-NULL cnmGenerationCfgPtr.
    Expected: GT_OK and the same identEnaalcCfg as was set.
    1.3. Call with wrong cnmGenerationCfgPtr [NULL].
    Expected: GT_BAD_PTR.
    1.4. Call with wrong cnmGenerationCfg.qosProfileId [0x80]
    Expected: NOT GT_OK.
    1.5. Call with wrong cnmGenerationCfg.isRouted [2]
    Expected: NOT GT_OK.
    1.6. Call with wrong cnmGenerationCfg.defaultVlanId [2]
    Expected: NOT GT_OK.
    1.7. Call with wrong cnmGenerationCfg.scaleFactor [128]
    Expected: NOT GT_OK.
    1.8. Call with wrong cnmGenerationCfg.version [0x10]
    Expected: NOT GT_OK.
    1.9. Call with wrong cnmGenerationCfg.flowIdTag [0x10000]
    Expected: NOT GT_OK.
*/

    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_PORT_CNM_GENERATION_CONFIG_STC cnmGenerationCfg;
    CPSS_PORT_CNM_GENERATION_CONFIG_STC cnmGenerationCfgGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with all correct values
                               cnmGenerationCfg.qosProfileId [0]
                               cnmGenerationCfg.isRouted [0]
                               cnmGenerationCfg.overrideUp [GT_TRUE]
                               cnmGenerationCfg.cnmUp [0]
                               cnmGenerationCfg.defaultVlanId [0]
                               cnmGenerationCfg.scaleFactor [256]
                               cnmGenerationCfg.version [0]
                               cnmGenerationCfg.cpidMsb [0]
                               cnmGenerationCfg.cnUntaggedEnable [GT_TRUE]
                               cnmGenerationCfg.forceCnTag [GT_TRUE]
                               cnmGenerationCfg.flowIdTag [0]
                               cnmGenerationCfg.appendPacket [GT_TRUE]
            Expected: GT_OK.
        */
        cnmGenerationCfg.qosProfileId = 0;
        cnmGenerationCfg.isRouted = 0;
        cnmGenerationCfg.overrideUp = GT_TRUE;
        cnmGenerationCfg.cnmUp = 0;
        cnmGenerationCfg.defaultVlanId = 0;
        cnmGenerationCfg.scaleFactor = 256;
        cnmGenerationCfg.version = 0;
        cnmGenerationCfg.cpidMsb.l[0] = 0;
        cnmGenerationCfg.cpidMsb.l[1] = 0;
        cnmGenerationCfg.cnUntaggedEnable = GT_TRUE;
        cnmGenerationCfg.forceCnTag = GT_TRUE;
        cnmGenerationCfg.flowIdTag = 0;
        cnmGenerationCfg.appendPacket = GT_TRUE;

        st = cpssDxChPortCnMessageGenerationConfigSet(dev, &cnmGenerationCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnMessageGenerationConfigGet with not-NULL etherTypePtr.
            Expected: GT_OK and the same etherType and identEnable as was set.
        */
        st = cpssDxChPortCnMessageGenerationConfigGet(dev, &cnmGenerationCfgGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortCnMessageGenerationConfigGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.qosProfileId, cnmGenerationCfgGet.qosProfileId,
                         "got another cnmGenerationCfg.qosProfileId then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.defaultVlanId,
                                     cnmGenerationCfgGet.defaultVlanId,
                         "got another cnmGenerationCfg.defaultVlanId then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.appendPacket,
                                     cnmGenerationCfgGet.appendPacket,
                         "got another cnmGenerationCfg.appendPacket then was set: %d", dev);

        if (PRV_CPSS_PP_MAC(prvTgfDevNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.isRouted, cnmGenerationCfgGet.isRouted,
                             "got another cnmGenerationCfg.isRouted then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.overrideUp, cnmGenerationCfgGet.overrideUp,
                             "got another cnmGenerationCfg.overrideUp then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.cnmUp, cnmGenerationCfgGet.cnmUp,
                             "got another cnmGenerationCfg.cnmUp then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.scaleFactor, cnmGenerationCfgGet.scaleFactor,
                             "got another cnmGenerationCfg.scaleFactor then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.version, cnmGenerationCfgGet.version,
                             "got another cnmGenerationCfg.version then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.cpidMsb.l[0],
                                         cnmGenerationCfgGet.cpidMsb.l[0],
                             "got another cnmGenerationCfg.cpidMsb then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.cpidMsb.l[1],
                                         cnmGenerationCfgGet.cpidMsb.l[1],
                             "got another cnmGenerationCfg.cpidMsb then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.cnUntaggedEnable,
                                         cnmGenerationCfgGet.cnUntaggedEnable,
                             "got another cnmGenerationCfg.cnUntaggedEnable then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.forceCnTag, cnmGenerationCfgGet.forceCnTag,
                             "got another cnmGenerationCfg.forceCnTag then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.flowIdTag, cnmGenerationCfgGet.flowIdTag,
                             "got another cnmGenerationCfg.flowIdTag then was set: %d", dev);
        }



        /*
            1.1. Call with all correct values
                               cnmGenerationCfg.qosProfileId [0x7F]
                               cnmGenerationCfg.isRouted [1]
                               cnmGenerationCfg.overrideUp [GT_FALSE]
                               cnmGenerationCfg.cnmUp [10]
                               cnmGenerationCfg.defaultVlanId [0xFFF]
                               cnmGenerationCfg.scaleFactor [512]
                               cnmGenerationCfg.version [0xF]
                               cnmGenerationCfg.cpidMsb [10]
                               cnmGenerationCfg.cnUntaggedEnable [GT_FALSE]
                               cnmGenerationCfg.forceCnTag [GT_FALSE]
                               cnmGenerationCfg.flowIdTag [0xFFFF]
                               cnmGenerationCfg.appendPacket [GT_FALSE]
            Expected: GT_OK.
        */
        cnmGenerationCfg.qosProfileId = 0x7F;
        cnmGenerationCfg.isRouted = 1;
        cnmGenerationCfg.overrideUp = GT_FALSE;
        cnmGenerationCfg.cnmUp = 10;
        cnmGenerationCfg.defaultVlanId = 0xFFF;
        cnmGenerationCfg.scaleFactor = 512;
        cnmGenerationCfg.version = 0xF;
        cnmGenerationCfg.cpidMsb.l[0] = 10;
        cnmGenerationCfg.cpidMsb.l[1] = 0;
        cnmGenerationCfg.cnUntaggedEnable = GT_FALSE;
        cnmGenerationCfg.forceCnTag = GT_FALSE;
        cnmGenerationCfg.flowIdTag = 0xFFFF;
        cnmGenerationCfg.appendPacket = GT_FALSE;

        st = cpssDxChPortCnMessageGenerationConfigSet(dev, &cnmGenerationCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnMessageGenerationConfigGet with not-NULL etherTypePtr.
            Expected: GT_OK and the same etherType and identEnable as was set.
        */
        st = cpssDxChPortCnMessageGenerationConfigGet(dev, &cnmGenerationCfgGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortCnMessageGenerationConfigGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.qosProfileId, cnmGenerationCfgGet.qosProfileId,
                         "got another cnmGenerationCfg.qosProfileId then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.defaultVlanId,
                                     cnmGenerationCfgGet.defaultVlanId,
                         "got another cnmGenerationCfg.defaultVlanId then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.appendPacket,
                                     cnmGenerationCfgGet.appendPacket,
                         "got another cnmGenerationCfg.appendPacket then was set: %d", dev);

        if (PRV_CPSS_PP_MAC(prvTgfDevNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.isRouted, cnmGenerationCfgGet.isRouted,
                             "got another cnmGenerationCfg.isRouted then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.overrideUp, cnmGenerationCfgGet.overrideUp,
                             "got another cnmGenerationCfg.overrideUp then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.scaleFactor, cnmGenerationCfgGet.scaleFactor,
                             "got another cnmGenerationCfg.scaleFactor then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.version, cnmGenerationCfgGet.version,
                             "got another cnmGenerationCfg.version then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.cpidMsb.l[0],
                                         cnmGenerationCfgGet.cpidMsb.l[0],
                             "got another cnmGenerationCfg.cpidMsb then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.cpidMsb.l[1],
                                         cnmGenerationCfgGet.cpidMsb.l[1],
                             "got another cnmGenerationCfg.cpidMsb then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.cnUntaggedEnable,
                                         cnmGenerationCfgGet.cnUntaggedEnable,
                             "got another cnmGenerationCfg.cnUntaggedEnable then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.forceCnTag, cnmGenerationCfgGet.forceCnTag,
                             "got another cnmGenerationCfg.forceCnTag then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cnmGenerationCfg.flowIdTag, cnmGenerationCfgGet.flowIdTag,
                             "got another cnmGenerationCfg.flowIdTag then was set: %d", dev);
        }

        /*
            1.3. Call with wrong cnmGenerationCfgPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnMessageGenerationConfigSet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.4. Call with wrong cnmGenerationCfg.qosProfileId [0x80]
            Expected: NOT GT_OK.
        */
        cnmGenerationCfg.qosProfileId = 0x80;

        st = cpssDxChPortCnMessageGenerationConfigSet(dev, &cnmGenerationCfg);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        cnmGenerationCfg.qosProfileId = 0;

        /*
            1.5. Call with wrong cnmGenerationCfg.isRouted [2]
            Expected: NOT GT_OK.
        */
        cnmGenerationCfg.isRouted = 2;

        st = cpssDxChPortCnMessageGenerationConfigSet(dev, &cnmGenerationCfg);
        if (PRV_CPSS_PP_MAC(prvTgfDevNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }

        cnmGenerationCfg.isRouted = 0;

        /*
            1.6. Call with wrong cnmGenerationCfg.defaultVlanId [0x1000]
            Expected: NOT GT_OK.
        */
        cnmGenerationCfg.defaultVlanId = 0x1000;

        st = cpssDxChPortCnMessageGenerationConfigSet(dev, &cnmGenerationCfg);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        cnmGenerationCfg.defaultVlanId = 0;

        /*
            1.7. Call with wrong cnmGenerationCfg.scaleFactor [128]
            Expected: NOT GT_OK.
        */
        cnmGenerationCfg.scaleFactor = 128;
        st = cpssDxChPortCnMessageGenerationConfigSet(dev, &cnmGenerationCfg);
        if (PRV_CPSS_PP_MAC(prvTgfDevNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }

        cnmGenerationCfg.scaleFactor = 256;

        /*
            1.8. Call with wrong cnmGenerationCfg.version [0x10]
            Expected: NOT GT_OK.
        */
        cnmGenerationCfg.version = 0x10;

        st = cpssDxChPortCnMessageGenerationConfigSet(dev, &cnmGenerationCfg);
        if (PRV_CPSS_PP_MAC(prvTgfDevNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }

        cnmGenerationCfg.version = 0;

        /*
            1.9. Call with wrong cnmGenerationCfg.flowIdTag [0x10000]
            Expected: NOT GT_OK.
        */
        cnmGenerationCfg.flowIdTag = 0x10000;

        st = cpssDxChPortCnMessageGenerationConfigSet(dev, &cnmGenerationCfg);
        if (PRV_CPSS_PP_MAC(prvTgfDevNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }

        cnmGenerationCfg.flowIdTag = 0;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnMessageGenerationConfigSet(dev, &cnmGenerationCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnMessageGenerationConfigSet(dev, &cnmGenerationCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnMessageGenerationConfigGet
(
    IN  GT_U8    devNum,
    OUT CPSS_PORT_CNM_GENERATION_CONFIG_STC *cnmGenerationCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnMessageGenerationConfigGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with not null cnmGenerationCfgPtr.
    Expected: GT_OK.
    1.2. Call with wrong cnmGenerationCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_PORT_CNM_GENERATION_CONFIG_STC cnmGenerationCfg;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null cnmGenerationCfgPtr.
            Expected: GT_OK.
        */
        st = cpssDxChPortCnMessageGenerationConfigGet(dev, &cnmGenerationCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong cnmGenerationCfgPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnMessageGenerationConfigGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnMessageGenerationConfigGet(dev, &cnmGenerationCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnMessageGenerationConfigGet(dev, &cnmGenerationCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnPacketLengthSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT packetLength
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnPacketLengthSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with all correct packetLength
                                [CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E /
                                 CPSS_DXCH_PORT_CN_LENGTH_1_5_KB_E /
                                 CPSS_DXCH_PORT_CN_LENGTH_2_KB_E /
                                 CPSS_DXCH_PORT_CN_LENGTH_10_KB_E].
    Expected: GT_OK.
    1.2. Call cpssDxChPortCnPacketLengthGet with not-NULL packetLengthPtr.
    Expected: GT_OK and the same packetLength as was set.
    1.3. Call with wrong enum values packetLength.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT  packetLength = CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E;
    CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT  packetLengthGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with all correct packetLength
                                        [CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E /
                                         CPSS_DXCH_PORT_CN_LENGTH_1_5_KB_E /
                                         CPSS_DXCH_PORT_CN_LENGTH_2_KB_E /
                                         CPSS_DXCH_PORT_CN_LENGTH_10_KB_E].
            Expected: GT_OK.
        */
        packetLength = CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E;

        st = cpssDxChPortCnPacketLengthSet(dev, packetLength);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnPacketLengthGet with not-NULL packetLengthPtr.
            Expected: GT_OK and the same packetLength as was set.
        */
        st = cpssDxChPortCnPacketLengthGet(dev, &packetLengthGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                         "cpssDxChPortCnPacketLengthGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(packetLength, packetLengthGet,
                         "got another packetLength then was set: %d", dev);

        /*
            1.1. Call with all correct packetLength
                                        [CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E /
                                         CPSS_DXCH_PORT_CN_LENGTH_1_5_KB_E /
                                         CPSS_DXCH_PORT_CN_LENGTH_2_KB_E /
                                         CPSS_DXCH_PORT_CN_LENGTH_10_KB_E].
            Expected: GT_OK.
        */
        packetLength = CPSS_DXCH_PORT_CN_LENGTH_1_5_KB_E;

        st = cpssDxChPortCnPacketLengthSet(dev, packetLength);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnPacketLengthGet with not-NULL packetLengthPtr.
            Expected: GT_OK and the same packetLength as was set.
        */
        st = cpssDxChPortCnPacketLengthGet(dev, &packetLengthGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                         "cpssDxChPortCnPacketLengthGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(packetLength, packetLengthGet,
                         "got another packetLength then was set: %d", dev);

        /*
            1.1. Call with all correct packetLength
                                        [CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E /
                                         CPSS_DXCH_PORT_CN_LENGTH_1_5_KB_E /
                                         CPSS_DXCH_PORT_CN_LENGTH_2_KB_E /
                                         CPSS_DXCH_PORT_CN_LENGTH_10_KB_E].
            Expected: GT_OK.
        */
        packetLength = CPSS_DXCH_PORT_CN_LENGTH_2_KB_E;

        st = cpssDxChPortCnPacketLengthSet(dev, packetLength);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnPacketLengthGet with not-NULL packetLengthPtr.
            Expected: GT_OK and the same packetLength as was set.
        */
        st = cpssDxChPortCnPacketLengthGet(dev, &packetLengthGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                         "cpssDxChPortCnPacketLengthGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(packetLength, packetLengthGet,
                         "got another packetLength then was set: %d", dev);

        /*
            1.1. Call with all correct packetLength
                                        [CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E /
                                         CPSS_DXCH_PORT_CN_LENGTH_1_5_KB_E /
                                         CPSS_DXCH_PORT_CN_LENGTH_2_KB_E /
                                         CPSS_DXCH_PORT_CN_LENGTH_10_KB_E].
            Expected: GT_OK.
        */
        packetLength = CPSS_DXCH_PORT_CN_LENGTH_10_KB_E;

        st = cpssDxChPortCnPacketLengthSet(dev, packetLength);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnPacketLengthGet with not-NULL packetLengthPtr.
            Expected: GT_OK and the same packetLength as was set.
        */
        st = cpssDxChPortCnPacketLengthGet(dev, &packetLengthGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                         "cpssDxChPortCnPacketLengthGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(packetLength, packetLengthGet,
                         "got another packetLength then was set: %d", dev);

        /*
            1.3. Call with wrong enum values packetLength.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPortCnPacketLengthSet
                            (dev, packetLength),
                            packetLength);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnPacketLengthSet(dev, packetLength);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnPacketLengthSet(dev, packetLength);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnPacketLengthGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT  *packetLengthPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnPacketLengthGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with not null packetLengthPtr.
    Expected: GT_OK.
    1.2. Call with wrong packetLengthPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT  packetLength;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null packetLengthPtr.
            Expected: GT_OK.
        */
        st = cpssDxChPortCnPacketLengthGet(dev, &packetLength);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong packetLengthPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnPacketLengthGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnPacketLengthGet(dev, &packetLength);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnPacketLengthGet(dev, &packetLength);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnPrioritySpeedLocationSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT prioritySpeedLocation
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnPrioritySpeedLocationSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with all correct prioritySpeedLocation
                                [CPSS_DXCH_PORT_CN_PRIORITY_SPEED_CPID_E /
                                 CPSS_DXCH_PORT_CN_SDU_UP_E /
                                 CPSS_DXCH_PORT_CN_SPEED_E]
    Expected: GT_OK.
    1.2. Call cpssDxChPortCnPrioritySpeedLocationGet
         with not-NULL prioritySpeedLocationPtr.
    Expected: GT_OK and the same prioritySpeedLocation as was set.
    1.3. Call with wrong enum values prioritySpeedLocation.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT  prioritySpeedLocation = 0;
    CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT  prioritySpeedLocationGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with all correct prioritySpeedLocation
                                        [CPSS_DXCH_PORT_CN_PRIORITY_SPEED_CPID_E /
                                         CPSS_DXCH_PORT_CN_SDU_UP_E /
                                         CPSS_DXCH_PORT_CN_SPEED_E]
            Expected: GT_OK.
        */
        prioritySpeedLocation = CPSS_DXCH_PORT_CN_PRIORITY_SPEED_CPID_E;

        st = cpssDxChPortCnPrioritySpeedLocationSet(dev, prioritySpeedLocation);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnPrioritySpeedLocationGet
                 with not-NULL prioritySpeedLocationPtr.
            Expected: GT_OK and the same prioritySpeedLocation as was set.
        */
        st = cpssDxChPortCnPrioritySpeedLocationGet(dev, &prioritySpeedLocationGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                         "cpssDxChPortCnPrioritySpeedLocationGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(prioritySpeedLocation, prioritySpeedLocationGet,
                         "got another prioritySpeedLocation then was set: %d", dev);

        /*
            1.1. Call with all correct prioritySpeedLocation
                                        [CPSS_DXCH_PORT_CN_PRIORITY_SPEED_CPID_E /
                                         CPSS_DXCH_PORT_CN_SDU_UP_E /
                                         CPSS_DXCH_PORT_CN_SPEED_E]
            Expected: GT_OK.
        */
        prioritySpeedLocation = CPSS_DXCH_PORT_CN_SDU_UP_E;

        st = cpssDxChPortCnPrioritySpeedLocationSet(dev, prioritySpeedLocation);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnPrioritySpeedLocationGet
                 with not-NULL prioritySpeedLocationPtr.
            Expected: GT_OK and the same prioritySpeedLocation as was set.
        */
        st = cpssDxChPortCnPrioritySpeedLocationGet(dev, &prioritySpeedLocationGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                         "cpssDxChPortCnPrioritySpeedLocationGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(prioritySpeedLocation, prioritySpeedLocationGet,
                         "got another prioritySpeedLocation then was set: %d", dev);

        /*
            1.1. Call with all correct prioritySpeedLocation
                                        [CPSS_DXCH_PORT_CN_PRIORITY_SPEED_CPID_E /
                                         CPSS_DXCH_PORT_CN_SDU_UP_E /
                                         CPSS_DXCH_PORT_CN_SPEED_E]
            Expected: GT_OK.
        */
        prioritySpeedLocation = CPSS_DXCH_PORT_CN_SPEED_E;

        st = cpssDxChPortCnPrioritySpeedLocationSet(dev, prioritySpeedLocation);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnPrioritySpeedLocationGet
                 with not-NULL prioritySpeedLocationPtr.
            Expected: GT_OK and the same prioritySpeedLocation as was set.
        */
        st = cpssDxChPortCnPrioritySpeedLocationGet(dev, &prioritySpeedLocationGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                         "cpssDxChPortCnPrioritySpeedLocationGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(prioritySpeedLocation, prioritySpeedLocationGet,
                         "got another prioritySpeedLocation then was set: %d", dev);

        /*
            1.3. Call with wrong enum values prioritySpeedLocation.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPortCnPrioritySpeedLocationSet
                            (dev, prioritySpeedLocation),
                            prioritySpeedLocation);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnPrioritySpeedLocationSet(dev, prioritySpeedLocation);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnPrioritySpeedLocationSet(dev, prioritySpeedLocation);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnPrioritySpeedLocationGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT  *prioritySpeedLocationPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnPrioritySpeedLocationGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with not null prioritySpeedLocationPtr.
    Expected: GT_OK.
    1.2. Call with wrong prioritySpeedLocationPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT  prioritySpeedLocation;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null prioritySpeedLocationPtr.
            Expected: GT_OK.
        */
        st = cpssDxChPortCnPrioritySpeedLocationGet(dev, &prioritySpeedLocation);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong prioritySpeedLocationPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnPrioritySpeedLocationGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnPrioritySpeedLocationGet(dev, &prioritySpeedLocation);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnPrioritySpeedLocationGet(dev, &prioritySpeedLocation);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnProfileQueueConfigSet
(
    IN GT_U8                               devNum,
    IN CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN GT_U8                               tcQueue,
    IN CPSS_PORT_CN_PROFILE_CONFIG_STC     *cnProfileCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnProfileQueueConfigSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with all correct parameters
                                profileSet [CPSS_PORT_TX_DROP_PROFILE_1_E /
                                            CPSS_PORT_TX_DROP_PROFILE_4_E],
                                tcQueue [1 / 5],
                                cnProfileCfg.cnAware [GT_TRUE / GT_FALSE],
                                cnProfileCfg.threshold [1 / 10].
    Expected: GT_OK.
    1.2. Call cpssDxChPortCnProfileQueueConfigGet
         with not-NULL cnProfileCfgPtr.
    Expected: GT_OK and the same values as was set.
    1.3. Call with wrong enum values profileSet.
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong tcQueue [PORT_CN_INVALID_TCQUEUE_CNS].
    Expected: NOT GT_OK.
    1.5. Call with wrong cnProfileCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet = 0;
    GT_U8                               tcQueue = 0;
    CPSS_PORT_CN_PROFILE_CONFIG_STC     cnProfileCfg;
    CPSS_PORT_CN_PROFILE_CONFIG_STC     cnProfileCfgGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with all correct parameters
                                        profileSet [CPSS_PORT_TX_DROP_PROFILE_1_E],
                                        tcQueue [1],
                                        cnProfileCfg.threshold [1].
            Expected: GT_OK.
        */
        profileSet = CPSS_PORT_TX_DROP_PROFILE_1_E;
        tcQueue = 1;
        cnProfileCfg.cnAware = GT_TRUE;
        cnProfileCfg.threshold = 1;

        st = cpssDxChPortCnProfileQueueConfigSet(dev, profileSet, tcQueue, &cnProfileCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnProfileQueueConfigGet
                 with not-NULL cnProfileCfgPtr.
            Expected: GT_OK and the same values as was set.
        */
        st = cpssDxChPortCnProfileQueueConfigGet(dev, profileSet, tcQueue, &cnProfileCfgGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "cpssDxChPortCnProfileQueueConfigGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(cnProfileCfg.cnAware,
                                     cnProfileCfgGet.cnAware,
            "got another cnProfileCfg.cnAware then was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(cnProfileCfg.threshold,
                                     cnProfileCfgGet.threshold,
            "got another cnProfileCfg.threshold then was set: %d", dev);

        /*
            1.1. Call with all correct parameters
                                        profileSet [CPSS_PORT_TX_DROP_PROFILE_4_E],
                                        tcQueue [5],
                                        cnProfileCfg.threshold [10].
            Expected: GT_OK.
        */
        profileSet = CPSS_PORT_TX_DROP_PROFILE_4_E;
        tcQueue = 5;
        cnProfileCfg.cnAware = GT_FALSE;
        cnProfileCfg.threshold = 10;

        st = cpssDxChPortCnProfileQueueConfigSet(dev, profileSet, tcQueue, &cnProfileCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortCnProfileQueueConfigGet
                 with not-NULL cnProfileCfgPtr.
            Expected: GT_OK and the same values as was set.
        */
        st = cpssDxChPortCnProfileQueueConfigGet(dev, profileSet, tcQueue, &cnProfileCfgGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "cpssDxChPortCnProfileQueueConfigGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(cnProfileCfg.cnAware,
                                     cnProfileCfgGet.cnAware,
            "got another cnProfileCfg.cnAware then was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(cnProfileCfg.threshold,
                                     cnProfileCfgGet.threshold,
            "got another cnProfileCfg.threshold then was set: %d", dev);

        /*
            1.3. Call with wrong enum values profileSet.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPortCnProfileQueueConfigSet
                            (dev, profileSet, tcQueue, &cnProfileCfg),
                            profileSet);

        /*
            1.4. Call with wrong tcQueue [PORT_CN_INVALID_TCQUEUE_CNS].
            Expected: NOT GT_OK.
        */
        tcQueue = PORT_CN_INVALID_TCQUEUE_CNS;

        st = cpssDxChPortCnProfileQueueConfigSet(dev, profileSet, tcQueue, &cnProfileCfg);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        tcQueue = 5;

        /*
            1.5. Call with wrong cnProfileCfgPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnProfileQueueConfigSet(dev, profileSet, tcQueue, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnProfileQueueConfigSet(dev, profileSet, tcQueue, &cnProfileCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnProfileQueueConfigSet(dev, profileSet, tcQueue, &cnProfileCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnProfileQueueConfigGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN  GT_U8                               tcQueue,
    OUT CPSS_PORT_CN_PROFILE_CONFIG_STC     *cnProfileCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnProfileQueueConfigGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with all correct parameters
                                profileSet [CPSS_PORT_TX_DROP_PROFILE_1_E /
                                            CPSS_PORT_TX_DROP_PROFILE_4_E],
                                tcQueue [1 / 5],
                   and not NULL cnProfileCfgPtr.
    Expected: GT_OK.
    1.2. Call with wrong enum values profileSet.
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong tcQueue [PORT_CN_INVALID_TCQUEUE_CNS].
    Expected: NOT GT_OK.
    1.4. Call with wrong cnProfileCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet = 0;
    GT_U8                               tcQueue = 0;
    CPSS_PORT_CN_PROFILE_CONFIG_STC     cnProfileCfg;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with all correct parameters
                                        profileSet [CPSS_PORT_TX_DROP_PROFILE_1_E]
                                        tcQueue [1],
                           and not NULL cnProfileCfgPtr.
            Expected: GT_OK.
        */
        profileSet = CPSS_PORT_TX_DROP_PROFILE_1_E;
        tcQueue = 1;

        st = cpssDxChPortCnProfileQueueConfigGet(dev, profileSet, tcQueue, &cnProfileCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.1. Call with all correct parameters
                                        profileSet [CPSS_PORT_TX_DROP_PROFILE_4_E],
                                        tcQueue [5],
                           and not NULL cnProfileCfgPtr.
            Expected: GT_OK.
        */
        profileSet = CPSS_PORT_TX_DROP_PROFILE_4_E;
        tcQueue = 5;

        st = cpssDxChPortCnProfileQueueConfigGet(dev, profileSet, tcQueue, &cnProfileCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);


        /*
            1.2. Call with wrong enum values profileSet.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPortCnProfileQueueConfigGet
                            (dev, profileSet, tcQueue, &cnProfileCfg),
                            profileSet);

        /*
            1.4. Call with wrong tcQueue [PORT_CN_INVALID_TCQUEUE_CNS].
            Expected: NOT GT_OK.
        */
        tcQueue = PORT_CN_INVALID_TCQUEUE_CNS;

        st = cpssDxChPortCnProfileQueueConfigGet(dev, profileSet, tcQueue, &cnProfileCfg);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        tcQueue = 5;

        /*
            1.5. Call with wrong cnProfileCfgPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnProfileQueueConfigGet(dev, profileSet, tcQueue, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnProfileQueueConfigGet(dev, profileSet, tcQueue, &cnProfileCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnProfileQueueConfigGet(dev, profileSet, tcQueue, &cnProfileCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnQueueStatusModeEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable,
    IN GT_U8    targetDev,
    IN GT_U8    targetPort
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnQueueStatusModeEnableSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with all correct parameters
                                        enable [GT_TRUE / GT_FALSE],
                                        targetDev [0 - 31],
                                        targetPort [0 - 61].
    Expected: GT_OK.
    1.2. Call cpssDxChPortCnQueueStatusModeEnableGet with not-NULL pointers
    Expected: GT_OK and the same values as was set.
    1.3. Call with enable [GT_FALSE]
         and wrong targetDev [PORT_CN_INVALID_DEV_CNS] (not relevant).
    Expected: GT_OK.
    1.4. Call withenable [GT_FALSE]
         and wrong targetPort [PORT_CN_INVALID_PHY_PORT_CNS] (not relevant).
    Expected: GT_OK.
    1.5. Call with enable [GT_TRUE] and wrong targetDev [PORT_CN_INVALID_DEV_CNS].
    Expected: NOT GT_OK.
    1.6. Call with enable [GT_TRUE] and wrong targetPort [PORT_CN_INVALID_PHY_PORT_CNS].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL  enable = GT_TRUE;
    GT_U8    targetDev = PORT_CN_VALID_DEV_CNS;
    GT_U8    targetPort = PORT_CN_VALID_PHY_PORT_CNS;

    GT_BOOL  enableGet = GT_FALSE;
    GT_U8    targetDevGet = 0;
    GT_U8    targetPortGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);;

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with all correct parameters enable [GT_TRUE / GT_FALSE],
                                                  targetDev [0 - 31], targetPort [0 - 61].
            Expected: GT_OK.
        */
        for(enable = GT_FALSE; enable <= GT_TRUE; enable++)
            for(targetDev = 0; targetDev < PORT_CN_INVALID_DEV_CNS; targetDev++)
                for(targetPort = 0; targetPort < PORT_CN_INVALID_PHY_PORT_CNS; targetPort++)
                {
                    st = cpssDxChPortCnQueueStatusModeEnableSet(dev, enable, targetDev, targetPort);
                    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                     "cpssDxChPortCnQueueStatusModeEnableSet: dev=%d, enable=%d, targetDev=%d, targetPort=%d",
                                                 dev, enable, targetDev, targetPort);

                    if(GT_OK == st)
                    {
                        /*
                            1.2. Call cpssDxChPortCnQueueStatusModeEnableGet
                                 with not-NULL cnProfileCfgPtr.
                            Expected: GT_OK and the same values as was set.
                        */
                        st = cpssDxChPortCnQueueStatusModeEnableGet(dev,
                                            &enableGet, &targetDevGet, &targetPortGet);
                        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                            "cpssDxChPortCnQueueStatusModeEnableGet: %d", dev);

                        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                                        "got another enable then was set: %d", dev);

                        if(GT_TRUE == enable)
                        {
                            UTF_VERIFY_EQUAL1_STRING_MAC(targetDev, targetDevGet,
                                            "got another targetDev then was set: %d", dev);
                            UTF_VERIFY_EQUAL1_STRING_MAC(targetPort, targetPortGet,
                                            "got another targetPort then was set: %d", dev);
                        }
                    }
                }

        /*
            1.3. Call with enable [GT_FALSE]
                 and wrong targetDev [PORT_CN_INVALID_DEV_CNS] (not relevant).
            Expected: GT_OK.
        */
        enable = GT_FALSE;
        targetDev = PORT_CN_INVALID_DEV_CNS;
        targetPort = 0;

        st = cpssDxChPortCnQueueStatusModeEnableSet(dev, enable, targetDev, targetPort);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
         "cpssDxChPortCnQueueStatusModeEnableSet: dev=%d, enable=%d, targetDev=%d, targetPort=%d",
                                     dev, enable, targetDev, targetPort);
        targetDev = 0;

        /*
            1.4. Call with enable [GT_FALSE]
                 and wrong targetPort [PORT_CN_INVALID_PHY_PORT_CNS] (not relevant).
            Expected: GT_OK.
        */
        enable = GT_FALSE;
        targetDev = 0;
        targetPort = PORT_CN_INVALID_PHY_PORT_CNS ;

        st = cpssDxChPortCnQueueStatusModeEnableSet(dev, enable, targetDev, targetPort);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
         "cpssDxChPortCnQueueStatusModeEnableSet: dev=%d, enable=%d, targetDev=%d, targetPort=%d",
                                     dev, enable, targetDev, targetPort);
        targetPort = 0;

        /*
            1.5. Call with enable [GT_TRUE]
                 and wrong targetDev [PORT_CN_INVALID_DEV_CNS].
            Expected: NOT GT_OK.
        */
        enable = GT_TRUE;
        targetDev = PORT_CN_INVALID_DEV_CNS;
        targetPort = 0;

        st = cpssDxChPortCnQueueStatusModeEnableSet(dev, enable, targetDev, targetPort);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
         "cpssDxChPortCnQueueStatusModeEnableSet: dev=%d, enable=%d, targetDev=%d, targetPort=%d",
                                     dev, enable, targetDev, targetPort);
        targetDev = 0;

        /*
            1.6. Call with enable [GT_TRUE]
                 and wrong targetPort [PORT_CN_INVALID_PHY_PORT_CNS].
            Expected: NOT GT_OK.
        */
        enable = GT_TRUE;
        targetDev = 0;
        targetPort = PORT_CN_INVALID_PHY_PORT_CNS ;

        st = cpssDxChPortCnQueueStatusModeEnableSet(dev, enable, targetDev, targetPort);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
         "cpssDxChPortCnQueueStatusModeEnableSet: dev=%d, enable=%d, targetDev=%d, targetPort=%d",
                                     dev, enable, targetDev, targetPort);
        targetPort = 0;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnQueueStatusModeEnableSet(dev, enable, targetDev, targetPort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnQueueStatusModeEnableSet(dev, enable, targetDev, targetPort);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnQueueStatusModeEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr,
    OUT GT_U8    *targetDevPtr,
    OUT GT_U8    *targetPortPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnQueueStatusModeEnableGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with not null pointers.
    Expected: GT_OK.
    1.2. Call with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
    1.2. Call with wrong targetDevPtr [NULL].
    Expected: GT_BAD_PTR.
    1.2. Call with wrong terargetPortPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL  enable;
    GT_U8    targetDev;
    GT_U8    targetPort;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChPortCnQueueStatusModeEnableGet(dev, &enable, &targetDev, &targetPort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnQueueStatusModeEnableGet(dev, NULL, &targetDev, &targetPort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.3. Call with wrong targetDev [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnQueueStatusModeEnableGet(dev, &enable, NULL, &targetPort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.4. Call with wrong targetPort [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnQueueStatusModeEnableGet(dev, &enable, &targetDev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnQueueStatusModeEnableGet(dev, &enable, &targetDev, &targetPort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnQueueStatusModeEnableGet(dev, &enable, &targetDev, &targetPort);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnSampleEntrySet
(
    IN GT_U8    devNum,
    IN GT_U8    entryIndex,
    IN CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnSampleEntrySet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with all correct parameters
                            entryIndex[0 - 7]
                            entry.interval[0 - 0xFFFF]
                            entry.randBitmap[0 - 0xFFFF]
    Expected: GT_OK.
    1.2. Call cpssDxChPortCnSampleEntryGet with not-NULL entryPtr.
    Expected: GT_OK and the same values as was set.
    1.3. Call with wrong entryIndex [8].
    Expected: NOT GT_OK.
    1.4. Call with wrong entry.interval [0x10000].
    Expected: NOT GT_OK.
    1.5. Call with wrong entry.randBitmap [0x10000].
    Expected: NOT GT_OK.
    1.6. Call with wrong entryPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U8                                  entryIndex = 0;
    CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC entry;
    CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC entryGet;
    GT_U32      counter = 0;/* loop counter */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        for(entryIndex = 0; entryIndex < 8; entryIndex++)
        {
            for(entry.interval = 0; entry.interval < 0xFFFF; entry.interval += 100)
            {
                for(entry.randBitmap = 0; entry.randBitmap < 0xFFFF; entry.randBitmap += 100)
                {
                    counter++;
                    /*
                        1.1. Call with all correct parameters
                                                entryIndex[0 - 7]
                                                entry.interval[0 - 0xFFFF]
                                                entry.randBitmap[0 - 0xFFFF]
                        Expected: GT_OK.
                    */
                    st = cpssDxChPortCnSampleEntrySet(dev, entryIndex, &entry);

                    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChPortCnSampleEntrySet: dev=%d, entryIndex=%d, entry.interval=%d, entry.randBitmap=%d",
                               dev, entryIndex, entry.interval, entry.randBitmap);

                    if(GT_OK == st)
                    {
                        /*
                            1.2. Call cpssDxChPortCnSampleEntryGet with not-NULL entryPtr.
                            Expected: GT_OK and the same values as was set.
                        */
                        st = cpssDxChPortCnSampleEntryGet(dev, entryIndex, &entryGet);

                        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                         "cpssDxChPortCnSampleEntryGet: %d", dev);
                        UTF_VERIFY_EQUAL1_STRING_MAC(entry.interval, entryGet.interval,
                                         "got another entry.interval then was set: %d", dev);
                        UTF_VERIFY_EQUAL1_STRING_MAC(entry.randBitmap, entryGet.randBitmap,
                                         "got another entry.randBitmap then was set: %d", dev);
                    }

                    if((counter % 100000) == 0)
                    {
                        utfPrintKeepAlive();
                    }
                }
            }
        }
        /*
            1.3. Call with wrong entryIndex [8].
            Expected: NOT GT_OK.
        */
        entryIndex = 8;
        entry.interval = 0;
        entry.randBitmap = 0;

        st = cpssDxChPortCnSampleEntrySet(dev, entryIndex, &entry);

        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
        "cpssDxChPortCnSampleEntrySet: dev=%d, entryIndex=%d, entry.interval=%d, entry.randBitmap=%d",
                   dev, entryIndex, entry.interval, entry.randBitmap);

        entryIndex = 0;

        /*
            1.4. Call with wrong entry.interval [0x10000].
            Expected: NOT GT_OK.
        */
        entry.interval = 0x10000;

        st = cpssDxChPortCnSampleEntrySet(dev, entryIndex, &entry);

        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
        "cpssDxChPortCnSampleEntrySet: dev=%d, entryIndex=%d, entry.interval=%d, entry.randBitmap=%d",
                   dev, entryIndex, entry.interval, entry.randBitmap);

        entry.interval = 0;

        /*
            1.5. Call with wrong entry.randBitmap [0x10000].
            Expected: NOT GT_OK.
        */
        entry.randBitmap = 0x10000;

        st = cpssDxChPortCnSampleEntrySet(dev, entryIndex, &entry);

        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
        "cpssDxChPortCnSampleEntrySet: dev=%d, entryIndex=%d, entry.randBitmap=%d, entry.randBitmap=%d",
                   dev, entryIndex, entry.randBitmap, entry.randBitmap);

        entry.randBitmap = 0;

        /*
            1.6. Call with wrong entryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnSampleEntrySet(dev, entryIndex, NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnSampleEntrySet(dev, entryIndex, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnSampleEntrySet(dev, entryIndex, &entry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnSampleEntryGet
(
    IN GT_U8    devNum,
    IN GT_U8    entryIndex,
    OUT CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnSampleEntryGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with all correct parameters entryIndex[0 - 7] and not null entryPtr.
    Expected: GT_OK.
    1.2. Call with wrong entryIndex [8].
    Expected: NOT GT_OK.
    1.3. Call with wrong entryPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U8                                  entryIndex = 0;
    CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC entry;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with all correct parameters entryIndex[0 - 7] and not null entryPtr.
            Expected: GT_OK.
        */
        for(entryIndex = 0; entryIndex < 8; entryIndex++)
        {
            st = cpssDxChPortCnSampleEntryGet(dev, entryIndex, &entry);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChPortCnSampleEntryGet: dev=%d, entryIndex=", dev, entryIndex);
        }
        /*
            1.3. Call with wrong entryIndex [8].
            Expected: NOT GT_OK.
        */
        entryIndex = 8;

        st = cpssDxChPortCnSampleEntryGet(dev, entryIndex, &entry);

        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChPortCnSampleEntryGet: dev=%d, entryIndex=%d", dev, entryIndex);
        entryIndex = 0;

        /*
            1.3. Call with wrong entryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortCnSampleEntryGet(dev, entryIndex, NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnSampleEntryGet(dev, entryIndex, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnSampleEntryGet(dev, entryIndex, &entry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnTerminationEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnTerminationEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (Lion and above)
    1.1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortCnTerminationEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_CN_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;
    GT_BOOL     state  = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
               1.1.1. Call with state [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_FALSE] */
            enable = GT_FALSE;

            st = cpssDxChPortCnTerminationEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
               1.1.2. Call cpssDxChPortCnTerminationEnableGet.
               Expected: GT_OK and the same enable.
            */
            st = cpssDxChPortCnTerminationEnableGet(dev, port, &state);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "cpssDxChPortCnTerminationEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, state,
                            "get another enable value than was set: %d, %d", dev, port);

            /*
               1.1.1. Call with state [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_TRUE] */
            enable = GT_TRUE;

            st = cpssDxChPortCnTerminationEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
               1.1.2. Call cpssDxChPortCnTerminationEnableGet.
               Expected: GT_OK and the same enable.
            */
            st = cpssDxChPortCnTerminationEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                          "[cpssDxChPortCnTerminationEnableGet]: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, state,
                          "get another enable value than was set: %d, %d", dev, port);
        }

        /*
           1.2. For all active devices go over all non available
           physical ports.
        */
        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* enable == GT_TRUE    */
            st = cpssDxChPortCnTerminationEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* enable == GT_TRUE  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortCnTerminationEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                     */
        /* enable == GT_TRUE  */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortCnTerminationEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, enable);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    enable = GT_TRUE;
    port = PORT_CN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnTerminationEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, enable == GT_TRUE */

    st = cpssDxChPortCnTerminationEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCnTerminationEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *statePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCnTerminationEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (Lion and above)
    1.1.1. Call with non-null statePtr.
    Expected: GT_OK.
    1.1.2. Call with statePtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = PORT_CN_VALID_PHY_PORT_CNS;
    GT_BOOL     state = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null statePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortCnTerminationEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with statePtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortCnTerminationEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortCnTerminationEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortCnTerminationEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortCnTerminationEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCnTerminationEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCnTerminationEnableGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChPortCn suit
 */

UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChPortCn)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnEtherTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnEtherTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnFcEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnFcEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnFcTimerSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnFcTimerGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnFrameQueueSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnFrameQueueGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnModeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnModeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnPanicPauseThresholdsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnPanicPauseThresholdsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnPauseTriggerEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnPauseTriggerEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnProfileGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnProfileQueueThresholdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnProfileQueueThresholdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnProfileThresholdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnProfileThresholdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnQueueAwareEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnQueueAwareEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnSpeedIndexSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnSpeedIndexGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnFbCalcConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnFbCalcConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnMessageGenerationConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnMessageGenerationConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnPacketLengthSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnPacketLengthGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnPrioritySpeedLocationSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnPrioritySpeedLocationGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnProfileQueueConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnProfileQueueConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnQueueStatusModeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnQueueStatusModeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnSampleEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnSampleEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnTerminationEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCnTerminationEnableGet)
UTF_SUIT_END_TESTS_MAC(cpssDxChPortCn)

