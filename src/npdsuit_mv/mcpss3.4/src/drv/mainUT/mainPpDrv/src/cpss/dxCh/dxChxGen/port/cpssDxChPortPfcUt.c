/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortPfcUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChPortPfc, that provides
*       CPSS implementation for Priority Flow Control functionality.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortPfc.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPort.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* defines */

/* Default valid value for port id */
#define PORT_PFC_VALID_PHY_PORT_CNS  0

/* Invalid profileIndex */
#define PORT_PFC_INVALID_PROFILEINDEX_CNS   8

/* Invalid tcQueue */
#define PORT_PFC_INVALID_TCQUEUE_CNS        8

/* Invalid xoffThreshold */
#define PORT_PFC_INVALID_XOFFTHRESHOLD_CNS  0x7FF+1

/* Invalid dropThreshold */
#define PORT_PFC_INVALID_DROPTHRESHOLD_CNS  0x7FF+1

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcCountingModeSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  pfcCountMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcCountingModeSet)
{
/*
    ITERATE_DEVICES (Lion)
    1.1. Call withall correct pfcCountMode [CPSS_DXCH_PORT_PFC_COUNT_BUFFERS_MODE_E /
                                            CPSS_DXCH_PORT_PFC_COUNT_PACKETS_E].
    Expected: GT_OK.
    1.2. Call cpssDxChPortPfcCountingModeGet with not-NULL pfcCountModePtr.
    Expected: GT_OK and the same pfcCountMode as was set.
    1.3. Call with wrong enum values pfcCountMode.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  pfcCountMode = CPSS_DXCH_PORT_PFC_COUNT_BUFFERS_MODE_E;
    CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  pfcCountModeGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call withall correct pfcCountMode [CPSS_DXCH_PORT_PFC_COUNT_BUFFERS_MODE_E /
                                                    CPSS_DXCH_PORT_PFC_COUNT_PACKETS_E].
            Expected: GT_OK.
        */
        pfcCountMode = CPSS_DXCH_PORT_PFC_COUNT_BUFFERS_MODE_E;

        st = cpssDxChPortPfcCountingModeSet(dev, pfcCountMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortPfcCountingModeGet with not-NULL pfcCountModePtr.
            Expected: GT_OK and the same pfcCountMode as was set.
        */
        st = cpssDxChPortPfcCountingModeGet(dev, &pfcCountModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortPfcCountingModeGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(pfcCountMode, pfcCountModeGet,
                         "got another pfcCountMode then was set: %d", dev);

        /*
            1.1. Call withall correct pfcCountMode [CPSS_DXCH_PORT_PFC_COUNT_BUFFERS_MODE_E /
                                                    CPSS_DXCH_PORT_PFC_COUNT_PACKETS_E].
            Expected: GT_OK.
        */
        pfcCountMode = CPSS_DXCH_PORT_PFC_COUNT_PACKETS_E;

        st = cpssDxChPortPfcCountingModeSet(dev, pfcCountMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortPfcCountingModeGet with not-NULL pfcCountModePtr.
            Expected: GT_OK and the same pfcCountMode as was set.
        */
        st = cpssDxChPortPfcCountingModeGet(dev, &pfcCountModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortPfcCountingModeGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(pfcCountMode, pfcCountModeGet,
                         "got another pfcCountMode then was set: %d", dev);

        /*
            1.3. Call with wrong enum values pfcCountMode.
            Expected: NOT GT_OK.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPortPfcCountingModeSet
                            (dev, pfcCountMode),
                            pfcCountMode);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcCountingModeSet(dev, pfcCountMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcCountingModeSet(dev, pfcCountMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcCountingModeGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  *pfcCountModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcCountingModeGet)
{
/*
    ITERATE_DEVICES (Lion)
    1.1. Call with not null pfcCountModePtr.
    Expected: GT_OK.
    1.2. Call with wrong pfcCountModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  pfcCountMode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null pfcCountModePtr.
            Expected: GT_OK.
        */
        st = cpssDxChPortPfcCountingModeGet(dev, &pfcCountMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong pfcCountModePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortPfcCountingModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcCountingModeGet(dev, &pfcCountMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcCountingModeGet(dev, &pfcCountMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcEnableSet)
{
/*
    ITERATE_DEVICES (Lion)
    1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChPortPfcEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    CPSS_DXCH_PORT_PFC_ENABLE_ENT enable;
    CPSS_DXCH_PORT_PFC_ENABLE_ENT state;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        for (enable = CPSS_DXCH_PORT_PFC_DISABLE_ALL_E;
              enable <=CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_AND_RESPONSE_E;
              enable++)
        {
             /*
               1.1. Call with state.
               Expected: GT_OK.
            */

            st = cpssDxChPortPfcEnableSet(dev, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

            /*
               1.2. Call cpssDxChPortPfcEnableGet.
               Expected: GT_OK and the same enable options.
            */
            st = cpssDxChPortPfcEnableGet(dev, &state);

            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                            "[cpssDxChPortPfcEnableGet]: %d, %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, state,
                            "get another enable value than was set: %d, %d", dev);
        }
        /*
           1.3. Call with wrong enable option.
           Expected: GT_BAD_PARAM.
        */

        enable = CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_AND_RESPONSE_E + 1;
        st = cpssDxChPortPfcEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, enable);
    }


    /* 2. For not active devices check that function returns non GT_OK.*/

    enable = CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_AND_RESPONSE_E;

    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcEnableGet
(
    IN   GT_U8     devNum,
    OUT  GT_BOOL   *statePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (Lion)
    1.1. Call with non-null statePtr.
    Expected: GT_OK.
    1.2. Call with statePtr [NULL].
    Expected: GT_BAD_PTR
*/

    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    CPSS_DXCH_PORT_PFC_ENABLE_ENT state;

    /* prepare iterator for go over all active devices */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with non-null statePtr.
           Expected: GT_OK.
        */
        st = cpssDxChPortPfcEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call with statePtr [NULL].
           Expected: GT_BAD_PTR
        */
        st = cpssDxChPortPfcEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcGlobalDropEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcGlobalDropEnableSet)
{
/*
    ITERATE_DEVICES (Lion)
    1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChPortPfcGlobalDropEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_BOOL     enable = GT_FALSE;
    GT_BOOL     state  = GT_FALSE;

    /* prepare iterator for go over all active devices */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
           1.1. Call with state [GT_FALSE and GT_TRUE].
           Expected: GT_OK.
        */

        /* Call function with enable = GT_FALSE] */
        enable = GT_FALSE;

        st = cpssDxChPortPfcGlobalDropEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
           1.2. Call cpssDxChPortPfcGlobalDropEnableGet.
           Expected: GT_OK and the same enable.
        */
        st = cpssDxChPortPfcGlobalDropEnableGet(dev, &state);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                        "[cpssDxChPortPfcGlobalDropEnableGet]: %d, %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, state,
                        "get another enable value than was set: %d, %d", dev);

        /*
           1.1. Call with state [GT_FALSE and GT_TRUE].
           Expected: GT_OK.
        */

        /* Call function with enable = GT_TRUE] */
        enable = GT_TRUE;

        st = cpssDxChPortPfcGlobalDropEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
           1.2. Call cpssDxChPortPfcGlobalDropEnableGet.
           Expected: GT_OK and the same enable.
        */
        st = cpssDxChPortPfcGlobalDropEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "[cpssDxChPortPfcGlobalDropEnableGet]: %d, %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, state,
                      "get another enable value than was set: %d, %d", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    enable = GT_TRUE;

    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcGlobalDropEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcGlobalDropEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcGlobalDropEnableGet
(
    IN   GT_U8     devNum,
    OUT  GT_BOOL   *statePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcGlobalDropEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (Lion)
    1.1. Call with non-null statePtr.
    Expected: GT_OK.
    1.2. Call with statePtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare iterator for go over all active devices */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with non-null statePtr.
           Expected: GT_OK.
        */
        st = cpssDxChPortPfcGlobalDropEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call with statePtr [NULL].
           Expected: GT_BAD_PTR
        */
        st = cpssDxChPortPfcGlobalDropEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcGlobalDropEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcGlobalDropEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcGlobalQueueConfigSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue
    IN GT_U32   xoffThreshold,
    IN GT_U32   dropThreshold
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcGlobalQueueConfigSet)
{
/*
    ITERATE_DEVICES (Lion)
    1.1. Call with tcQueue [0 - 7],
                   xoffThreshold [0 - 0x7FF],
                   dropThreshold [0 - 0x7FF].
    Expected: GT_OK.
    1.2. Call cpssDxChPortPfcGlobalQueueConfigGet with not-NULL pointers.
    Expected: GT_OK and the same values as was set.
    1.3. Call with wrong tcQueue [PORT_PFC_INVALID_TCQUEUE_CNS].
    Expected: NOT GT_OK.
    1.4. Call with wrong xoffThreshold [PORT_PFC_INVALID_XOFFTHRESHOLD_CNS].
    Expected: NOT GT_OK.
    1.5. Call with wrong dropThreshold [PORT_PFC_INVALID_DROPTHRESHOLD_CNS].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U8   tcQueue = 0;
    GT_U32  xoffThreshold = 0;
    GT_U32  xoffThresholdGet = 1;
    GT_U32  dropThreshold = 0;
    GT_U32  dropThresholdGet = 1;

    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcQueue [0 - 7], xoffThreshold [0 - 0x7FF], dropThreshold [0 - 0x7FF].
            Expected: GT_OK.
        */
        for(tcQueue = 0; tcQueue < PORT_PFC_INVALID_TCQUEUE_CNS; tcQueue++)
            for(xoffThreshold = 0;
                xoffThreshold < PORT_PFC_INVALID_XOFFTHRESHOLD_CNS;
                xoffThreshold += 300)
                for(dropThreshold = 0;
                    dropThreshold < PORT_PFC_INVALID_DROPTHRESHOLD_CNS;
                    dropThreshold += 500)
                {
                    st = cpssDxChPortPfcGlobalQueueConfigSet(dev, tcQueue,
                                                             xoffThreshold, dropThreshold);
                    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

                    if(GT_OK == st)
                    {
                        /*
                            1.2. Call cpssDxChPortPfcGlobalQueueConfigGet with not-NULL tcQueuePtr.
                            Expected: GT_OK and the same tcQueue as was set.
                        */
                        st = cpssDxChPortPfcGlobalQueueConfigGet(dev, tcQueue,
                                                                 &xoffThresholdGet, &dropThresholdGet);
                        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                    "cpssDxChPortPfcGlobalQueueConfigGet: %d", dev);
                        UTF_VERIFY_EQUAL1_STRING_MAC(xoffThreshold, xoffThresholdGet,
                                                     "got another xoffThreshold then was set: %d", dev);
                        UTF_VERIFY_EQUAL1_STRING_MAC(dropThreshold, dropThresholdGet,
                                                     "got another dropThreshold then was set: %d", dev);
                    }
                }

        /*
            1.3. Call with wrong tcQueue [PORT_PFC_INVALID_TCQUEUE_CNS].
            Expected: NOT GT_OK.
        */
        tcQueue = PORT_PFC_INVALID_TCQUEUE_CNS;

        st = cpssDxChPortPfcGlobalQueueConfigSet(dev, tcQueue, xoffThreshold, dropThreshold);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        tcQueue = 0;

        /*
            1.4. Call with wrong xoffThreshold [PORT_PFC_INVALID_XOFFTHRESHOLD_CNS].
            Expected: NOT GT_OK.
        */
        xoffThreshold = PORT_PFC_INVALID_XOFFTHRESHOLD_CNS;

        st = cpssDxChPortPfcGlobalQueueConfigSet(dev, tcQueue, xoffThreshold, dropThreshold);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        xoffThreshold = 0;

        /*
            1.5. Call with wrong dropThreshold [PORT_PFC_INVALID_DROPTHRESHOLD_CNS].
            Expected: NOT GT_OK.
        */
        dropThreshold = PORT_PFC_INVALID_DROPTHRESHOLD_CNS;

        st = cpssDxChPortPfcGlobalQueueConfigSet(dev, tcQueue, xoffThreshold, dropThreshold);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        dropThreshold = 0;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcGlobalQueueConfigSet(dev, tcQueue, xoffThreshold, dropThreshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcGlobalQueueConfigSet(dev, tcQueue, xoffThreshold, dropThreshold);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcGlobalQueueConfigGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    tcQueue,
    OUT GT_U32   *xoffThresholdPtr,
    OUT GT_U32   *dropThresholdPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcGlobalQueueConfigGet)
{
/*
    ITERATE_DEVICES (Lion)
    1.1. Call with tcQueue [0 - 7] and  not-NULL pointers.
    Expected: GT_OK.
    1.2. Call with wrong tcQueue [PORT_PFC_INVALID_TCQUEUE_CNS].
    Expected: NOT GT_OK.
    1.3. Call with wrong  xoffThresholdPtr [NULL].
    Expected: GT_BAD_PTR.
    1.4. Call with wrong  dropThresholdPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U8      tcQueue = 0;
    GT_U32  xoffThreshold = 0;
    GT_U32  dropThreshold = 0;

    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not-NULL tcQueue.
            Expected: GT_OK.
        */
        for(tcQueue = 0; tcQueue < PORT_PFC_INVALID_TCQUEUE_CNS; tcQueue++)
        {
            st = cpssDxChPortPfcGlobalQueueConfigGet(dev, tcQueue, &xoffThreshold, &dropThreshold);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }

        /*
            1.2. Call with wrong tcQueue [PORT_PFC_INVALID_TCQUEUE_CNS].
            Expected: NOT GT_OK.
        */
        tcQueue = PORT_PFC_INVALID_TCQUEUE_CNS;

        st = cpssDxChPortPfcGlobalQueueConfigGet(dev, tcQueue, &xoffThreshold, &dropThreshold);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, tcQueue = NULL", dev);

        tcQueue = 0;

        /*
            1.3. Call with wrong xoffThresholdPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortPfcGlobalQueueConfigGet(dev, tcQueue, NULL, &dropThreshold);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, xoffThresholdPtr = NULL", dev);

        /*
            1.4. Call with wrong dropThresholdPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortPfcGlobalQueueConfigGet(dev, tcQueue, &xoffThreshold, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropThresholdPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcGlobalQueueConfigGet(dev, tcQueue, &xoffThreshold, &dropThreshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcGlobalQueueConfigGet(dev, tcQueue, &xoffThreshold, &dropThreshold);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcProfileIndexSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   profileIndex
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcProfileIndexSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (Lion)
    1.1.1. Call with profileIndex [0 / 7].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortPfcProfileIndexGet with the same params.
    Expected: GT_OK and the same profileIndex.
    1.1.3. Call with profileIndex [PORT_PFC_INVALID_PROFILEINDEX_CNS].
    Expected: NOT GT_OK.
*/

    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_PFC_VALID_PHY_PORT_CNS;
    GT_U32      profileIndex = 0;
    GT_U32      profileIndexGet = 0;

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
                1.1.1. Call with profileIndex [0 / 7].
                Expected: GT_OK.
            */

            for(profileIndex = 0;
                profileIndex < PORT_PFC_INVALID_PROFILEINDEX_CNS;
                profileIndex++)
            {
                st = cpssDxChPortPfcProfileIndexSet(dev, port, profileIndex);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileIndex);

                if(GT_OK == st)
                {
                    /*
                       1.1.2. Call cpssDxChPortPfcProfileIndexGet.
                       Expected: GT_OK and the same profileIndex.
                    */
                    st = cpssDxChPortPfcProfileIndexGet(dev, port, &profileIndexGet);

                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                    "[cpssDxChPortPfcProfileIndexGet]: %d, %d", dev, port);
                    UTF_VERIFY_EQUAL2_STRING_MAC(profileIndex, profileIndexGet,
                                    "get another profileIndex value than was set: %d, %d", dev, port);
                }
            }

            /*
                1.1.3. Call with profileIndex [PORT_PFC_INVALID_PROFILEINDEX_CNS].
                Expected: NOT GT_OK.
            */

            profileIndex = PORT_PFC_INVALID_PROFILEINDEX_CNS;

            st = cpssDxChPortPfcProfileIndexSet(dev, port, profileIndex);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileIndex);

            profileIndex = 0;
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
            st = cpssDxChPortPfcProfileIndexSet(dev, port, profileIndex);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortPfcProfileIndexSet(dev, port, profileIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortPfcProfileIndexSet(dev, port, profileIndex);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, profileIndex);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    profileIndex = GT_TRUE;
    port = PORT_PFC_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcProfileIndexSet(dev, port, profileIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcProfileIndexSet(dev, port, profileIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcProfileIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U32  *profileIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcProfileIndexGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (Lion)
    1.1.1. Call with non-null profileIndexGetPtr.
    Expected: GT_OK.
    1.1.2. Call with profileIndexGetPtr [NULL].
    Expected: GT_BAD_PTR
*/

    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = PORT_PFC_VALID_PHY_PORT_CNS;
    GT_U32      profileIndexGet = 0;

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
            /* 1.1.1. Call with non-null profileIndexGetPtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortPfcProfileIndexGet(dev, port, &profileIndexGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with profileIndexGetPtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortPfcProfileIndexGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortPfcProfileIndexGet(dev, port, &profileIndexGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortPfcProfileIndexGet(dev, port, &profileIndexGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortPfcProfileIndexGet(dev, port, &profileIndexGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_PFC_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcProfileIndexGet(dev, port, &profileIndexGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcProfileIndexGet(dev, port, &profileIndexGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcProfileQueueConfigSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U8    tcQueue,
    IN CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC     *pfcProfileCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcProfileQueueConfigSet)
{
/*
    ITERATE_DEVICES (Lion)
    1.1. Call with profileIndex [0 / 3 / 7], tcQueue [0 / 3 / 7],
                        pfcProfileCfg.xonThreshold = 10,
                        pfcProfileCfg.xoffThreshold = 10.
    Expected: GT_OK.
    1.2. Call cpssDxChPortPfcProfileQueueConfigGet with not-NULL pfcProfileCfgPtr.
    Expected: GT_OK and the same pfcProfileCfg as was set.
    1.3. Call with wrong profileIndex [PORT_PFC_INVALID_PROFILEINDEX_CNS].
    Expected: NOT GT_OK.
    1.4. Call with wrong tcQueue [PORT_PFC_INVALID_TCQUEUE_CNS].
    Expected: NOT GT_OK.
    1.5. Call with wrong pfcProfileCfg [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32   profileIndex = 0;
    GT_U8    tcQueue = 0;
    CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC   pfcProfileCfg;
    CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC   pfcProfileCfgGet;

    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileIndex [0 / 3 / 7], tcQueue [0 / 3 / 7]
                 and pfcProfileCfg [0 / 0x1F00 / 0x1FFF].
            Expected: GT_OK.
        */
        /*Call with profileIndex [0], tcQueue [0] pfcProfileCfg [0].*/
        profileIndex = 0;
        tcQueue = 0;
        pfcProfileCfg.xonThreshold = 10;
        pfcProfileCfg.xoffThreshold = 10;

        st = cpssDxChPortPfcProfileQueueConfigSet(dev, profileIndex, tcQueue, &pfcProfileCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortPfcProfileQueueConfigGet with not-NULL pfcProfileCfgPtr.
            Expected: GT_OK and the same pfcProfileCfg as was set.
        */
        st = cpssDxChPortPfcProfileQueueConfigGet(dev, profileIndex, tcQueue, &pfcProfileCfgGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                "cpssDxChPortPfcProfileQueueConfigGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pfcProfileCfg.xonThreshold,
                                     pfcProfileCfgGet.xonThreshold,
               "got another pfcProfileCfg.xonThreshold then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pfcProfileCfg.xoffThreshold,
                                     pfcProfileCfgGet.xoffThreshold,
               "got another pfcProfileCfg.xonThreshold then was set: %d", dev);

        /*
            1.1. Call with profileIndex [0 / 3 / 7], tcQueue [0 / 3 / 7]
                 and pfcProfileCfg [0 / 0x1F00 / 0x1FFF].
            Expected: GT_OK.
        */
        /*Call with profileIndex [3], tcQueue [3] pfcProfileCfg [0x1F00].*/
        profileIndex = 3;
        tcQueue = 3;

        st = cpssDxChPortPfcProfileQueueConfigSet(dev, profileIndex, tcQueue, &pfcProfileCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortPfcProfileQueueConfigGet with not-NULL pfcProfileCfgPtr.
            Expected: GT_OK and the same pfcProfileCfg as was set.
        */
        st = cpssDxChPortPfcProfileQueueConfigGet(dev, profileIndex, tcQueue, &pfcProfileCfgGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                "cpssDxChPortPfcProfileQueueConfigGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pfcProfileCfg.xonThreshold,
                                     pfcProfileCfgGet.xonThreshold,
               "got another pfcProfileCfg.xonThreshold then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pfcProfileCfg.xoffThreshold,
                                     pfcProfileCfgGet.xoffThreshold,
               "got another pfcProfileCfg.xonThreshold then was set: %d", dev);

        /*
            1.1. Call with profileIndex [0 / 3 / 7], tcQueue [0 / 3 / 7]
                 and pfcProfileCfg [0 / 0x1F00 / 0x1FFF].
            Expected: GT_OK.
        */
        /*Call with profileIndex [7], tcQueue [7] pfcProfileCfg [0x1FFF].*/
        profileIndex = 7;
        tcQueue = 7;

        st = cpssDxChPortPfcProfileQueueConfigSet(dev, profileIndex, tcQueue, &pfcProfileCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortPfcProfileQueueConfigGet with not-NULL pfcProfileCfgPtr.
            Expected: GT_OK and the same pfcProfileCfg as was set.
        */
        st = cpssDxChPortPfcProfileQueueConfigGet(dev, profileIndex, tcQueue, &pfcProfileCfgGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                "cpssDxChPortPfcProfileQueueConfigGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pfcProfileCfg.xonThreshold,
                                     pfcProfileCfgGet.xonThreshold,
               "got another pfcProfileCfg.xonThreshold then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pfcProfileCfg.xoffThreshold,
                                     pfcProfileCfgGet.xoffThreshold,
               "got another pfcProfileCfg.xonThreshold then was set: %d", dev);

        /*
            1.3. Call with wrong profileIndex [PORT_PFC_INVALID_PROFILEINDEX_CNS].
            Expected: NOT GT_OK.
        */
        profileIndex = PORT_PFC_INVALID_PROFILEINDEX_CNS;

        st = cpssDxChPortPfcProfileQueueConfigSet(dev, profileIndex, tcQueue, &pfcProfileCfg);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        profileIndex = 7;

        /*
            1.4. Call with wrong tcQueue [PORT_PFC_INVALID_TCQUEUE_CNS].
            Expected: NOT GT_OK.
        */
        tcQueue = PORT_PFC_INVALID_TCQUEUE_CNS;

        st = cpssDxChPortPfcProfileQueueConfigSet(dev, profileIndex, tcQueue, &pfcProfileCfg);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        tcQueue = 7;

        /*
            1.5. Call with wrong pfcProfileCfg [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortPfcProfileQueueConfigSet(dev, profileIndex, tcQueue, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcProfileQueueConfigSet(dev, profileIndex, tcQueue, &pfcProfileCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcProfileQueueConfigSet(dev, profileIndex, tcQueue, &pfcProfileCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcProfileQueueConfigGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   profileIndex,
    IN  GT_U8    tcQueue,
    OUT CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC     *pfcProfileCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcProfileQueueConfigGet)
{
/*
    ITERATE_DEVICES (Lion)
    1.1. Call with profileIndex [0 / 7], tcQueue [0 / 7] and not null pfcProfileCfg.
    Expected: GT_OK.
    1.2. Call with wrong profileIndex [PORT_PFC_INVALID_PROFILEINDEX_CNS].
    Expected: NOT GT_OK.
    1.3. Call with wrong tcQueue [PORT_PFC_INVALID_TCQUEUE_CNS].
    Expected: NOT GT_OK.
    1.4. Call with wrong  pfcProfileCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      profileIndex = 0;
    GT_U8       tcQueue = 0;
    CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC pfcProfileCfg;

    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileIndex [0 / 7], tcQueue [0 / 7] and not null pfcProfileCfg.
            Expected: GT_OK.
        */
        profileIndex = 0;
        tcQueue = 0;

        st = cpssDxChPortPfcProfileQueueConfigGet(dev, profileIndex, tcQueue, &pfcProfileCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.1. Call with profileIndex [0 / 7], tcQueue [0 / 7] and not null pfcProfileCfg.
            Expected: GT_OK.
        */
        profileIndex = 7;
        tcQueue = 7;

        st = cpssDxChPortPfcProfileQueueConfigGet(dev, profileIndex, tcQueue, &pfcProfileCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong profileIndex [PORT_PFC_INVALID_PROFILEINDEX_CNS].
            Expected: NOT GT_OK.
        */
        profileIndex = PORT_PFC_INVALID_PROFILEINDEX_CNS;

        st = cpssDxChPortPfcProfileQueueConfigGet(dev, profileIndex, tcQueue, &pfcProfileCfg);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        profileIndex = 7;

        /*
            1.3. Call with wrong tcQueue [PORT_PFC_INVALID_TCQUEUE_CNS].
            Expected: NOT GT_OK.
        */
        tcQueue = PORT_PFC_INVALID_TCQUEUE_CNS;

        st = cpssDxChPortPfcProfileQueueConfigGet(dev, profileIndex, tcQueue, &pfcProfileCfg);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        tcQueue = 7;

        /*
            1.4. Call with wrong  pfcProfileCfg pointer [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortPfcProfileQueueConfigGet(dev, profileIndex, tcQueue, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pfcProfileCfg = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcProfileQueueConfigGet(dev, profileIndex, tcQueue, &pfcProfileCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcProfileQueueConfigGet(dev, profileIndex, tcQueue, &pfcProfileCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcTimerMapEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_BOOL                                 enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcTimerMapEnableSet)
{
/*
    ITERATE_DEVICES (Lion)
    1.1. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_1_E /
                               CPSS_PORT_TX_SCHEDULER_PROFILE_4_E],
                   enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChPortPfcTimerMapEnableGet with the same profileSet.
    Expected: GT_OK and the same enable.
    1.3. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_5_E /
                               CPSS_PORT_TX_SCHEDULER_PROFILE_8_E],
                   and other valid parameters from 1.1.
    Expected: NOT GT_OK for DxCh and GT_OK for others.
    1.4. Call with wrong enum values profileSet
                   and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet;
    GT_BOOL                                 enable;
    GT_BOOL                                 enableGet;


    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_1_E /
                                       CPSS_PORT_TX_SCHEDULER_PROFILE_4_E],
                           enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;
        enable     = GT_FALSE;

        st = cpssDxChPortPfcTimerMapEnableSet(dev, profileSet, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, enable);

        /*
            1.2. Call cpssDxChPortPfcTimerMapEnableGet with the same profileSet.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChPortPfcTimerMapEnableGet(dev, profileSet, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortPfcTimerMapEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                   "get another value than was set: %d, %d", dev, enableGet);

        /*
            1.1. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_1_E /
                                       CPSS_PORT_TX_SCHEDULER_PROFILE_4_E],
                           enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_4_E;
        enable     = GT_TRUE;

        st = cpssDxChPortPfcTimerMapEnableSet(dev, profileSet, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, enable);

        /*
            1.2. Call cpssDxChPortPfcTimerMapEnableGet with the same profileSet.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChPortPfcTimerMapEnableGet(dev, profileSet, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortPfcTimerMapEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                   "get another value than was set: %d, %d", dev, enableGet);

        /*
            1.3. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_5_E /
                                       CPSS_PORT_TX_SCHEDULER_PROFILE_8_E],
                           and other valid parameters from 1.1.
            Expected: NOT GT_OK for DxCh and GT_OK for others.
        */
        /* call with  profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_5_E */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_5_E;

        st = cpssDxChPortPfcTimerMapEnableSet(dev, profileSet, enable);
        if ((PRV_CPSS_PP_MAC(dev)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, enable);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, enable);
        }

        /* call with  profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_8_E */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_8_E;

        st = cpssDxChPortPfcTimerMapEnableSet(dev, profileSet, enable);
        if ((PRV_CPSS_PP_MAC(dev)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, enable);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, enable);
        }

        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;

        /*
            1.4. Call with wrong enum values profileSet
                           and other valid parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPortPfcTimerMapEnableSet
                            (dev, profileSet, enable),
                            profileSet);
    }

    profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;
    enable     = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcTimerMapEnableSet(dev, profileSet, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcTimerMapEnableSet(dev, profileSet, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcTimerMapEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    OUT GT_BOOL                                *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcTimerMapEnableGet)
{
/*
    ITERATE_DEVICES (Lion)
    1.1. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_1_E /
                               CPSS_PORT_TX_SCHEDULER_PROFILE_4_E],
                   non NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_5_E /
                               CPSS_PORT_TX_SCHEDULER_PROFILE_8_E],
                   and other valid parameters from 1.1.
    Expected: NOT GT_OK for DxCh and GT_OK for others.
    1.3. Call with wrong enum values profileSet
                   and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with NULL enablePtr
                   and other valid parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet;
    GT_BOOL                                 enable;


    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_1_E /
                                       CPSS_PORT_TX_SCHEDULER_PROFILE_4_E],
                           non NULL enablePtr.
            Expected: GT_OK.
        */
        /* call with  profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;

        st = cpssDxChPortPfcTimerMapEnableGet(dev, profileSet, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileSet);

        /* call with  profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_4_E */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_4_E;

        st = cpssDxChPortPfcTimerMapEnableGet(dev, profileSet, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileSet);

        /*
            1.2. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_5_E /
                                       CPSS_PORT_TX_SCHEDULER_PROFILE_8_E],
                           and other valid parameters from 1.1.
            Expected: NOT GT_OK for DxCh and GT_OK for others.
        */
        /* call with  profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_5_E */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_5_E;

        st = cpssDxChPortPfcTimerMapEnableGet(dev, profileSet, &enable);
        if ((PRV_CPSS_PP_MAC(dev)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileSet);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileSet);
        }

        /* call with  profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_8_E */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_8_E;

        st = cpssDxChPortPfcTimerMapEnableGet(dev, profileSet, &enable);
        if ((PRV_CPSS_PP_MAC(dev)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileSet);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileSet);
        }

        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;

        /*
            1.3. Call with wrong enum values profileSet
                           and other valid parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPortPfcTimerMapEnableGet
                            (dev, profileSet, &enable),
                            profileSet);

        /*
            1.4. Call with NULL enablePtr
                           and other valid parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortPfcTimerMapEnableGet(dev, profileSet, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcTimerMapEnableGet(dev, profileSet, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcTimerMapEnableGet(dev, profileSet, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcTimerToQueueMapSet
(
    IN  GT_U8     devNum,
    IN  GT_U32    pfcTimer,
    IN  GT_U32    tcQueue
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcTimerToQueueMapSet)
{
/*
    ITERATE_DEVICES (Lion)
    1.1. Call with pfcTimer [0 / 5 / 7],
                   tcQueue [0 / 5 / 7].
    Expected: GT_OK.
    1.2. Call cpssDxChPortPfcTimerToQueueMapGet with the same pfcTimer.
    Expected: GT_OK and the same tcQueue.
    1.3. Call with out of range pfcTimer [8]
                   and other valid parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range tcQueue [CPSS_TC_RANGE_CNS]
                   and other valid parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U32      pfcTimer   = 0;
    GT_U32      tcQueue    = 0;
    GT_U32      tcQueueGet = 0;


    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with pfcTimer [0 / 5 / 7],
                           tcQueue [0 / 5 / 7].
            Expected: GT_OK.
        */
        pfcTimer = 0;
        tcQueue  = 0;

        st = cpssDxChPortPfcTimerToQueueMapSet(dev, pfcTimer, tcQueue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, pfcTimer, tcQueue);

        /*
            1.2. Call cpssDxChPortPfcTimerToQueueMapGet with the same pfcTimer.
            Expected: GT_OK and the same tcQueue.
        */
        st = cpssDxChPortPfcTimerToQueueMapGet(dev, pfcTimer, &tcQueueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortPfcTimerToQueueMapGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(tcQueue, tcQueueGet,
                   "get another value than was set: %d, %d", dev, tcQueueGet);

        /*
            1.1. Call with pfcTimer [0 / 5 / 7],
                           tcQueue [0 / 5 / 7].
            Expected: GT_OK.
        */
        pfcTimer = 5;
        tcQueue  = 5;

        st = cpssDxChPortPfcTimerToQueueMapSet(dev, pfcTimer, tcQueue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, pfcTimer, tcQueue);

        /*
            1.2. Call cpssDxChPortPfcTimerToQueueMapGet with the same pfcTimer.
            Expected: GT_OK and the same tcQueue.
        */
        st = cpssDxChPortPfcTimerToQueueMapGet(dev, pfcTimer, &tcQueueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortPfcTimerToQueueMapGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(tcQueue, tcQueueGet,
                   "get another value than was set: %d, %d", dev, tcQueueGet);

        /*
            1.1. Call with pfcTimer [0 / 5 / 7],
                           tcQueue [0 / 5 / 7].
            Expected: GT_OK.
        */
        pfcTimer = 7;
        tcQueue  = 7;

        st = cpssDxChPortPfcTimerToQueueMapSet(dev, pfcTimer, tcQueue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, pfcTimer, tcQueue);

        /*
            1.2. Call cpssDxChPortPfcTimerToQueueMapGet with the same pfcTimer.
            Expected: GT_OK and the same tcQueue.
        */
        st = cpssDxChPortPfcTimerToQueueMapGet(dev, pfcTimer, &tcQueueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortPfcTimerToQueueMapGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(tcQueue, tcQueueGet,
                   "get another value than was set: %d, %d", dev, tcQueueGet);

        /*
            1.3. Call with out of range pfcTimer [8]
                           and other valid parameters from 1.1.
            Expected: NOT GT_OK.
        */
        pfcTimer = 8;

        st = cpssDxChPortPfcTimerToQueueMapSet(dev, pfcTimer, tcQueue);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, pfcTimer);

        pfcTimer = 0;

        /*
            1.4. Call with out of range tcQueue [CPSS_TC_RANGE_CNS]
                           and other valid parameters from 1.1.
            Expected: NOT GT_OK.
        */
        tcQueue = CPSS_TC_RANGE_CNS;

        st = cpssDxChPortPfcTimerToQueueMapSet(dev, pfcTimer, tcQueue);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcQueue);
    }

    pfcTimer = 0;
    tcQueue  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcTimerToQueueMapSet(dev, pfcTimer, tcQueue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcTimerToQueueMapSet(dev, pfcTimer, tcQueue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcTimerToQueueMapGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    pfcTimer,
    OUT GT_U32    *tcQueuePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcTimerToQueueMapGet)
{
/*
    ITERATE_DEVICES (Lion)
    1.1. Call with pfcTimer[0 / 5 / 7],
                   non NULL tcQueuePtr.
    Expected: GT_OK.
    1.2. Call with out of range pfcTimer [8]
                   and other valid parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with NULL tcQueuePtr and other valid parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U32      pfcTimer = 0;
    GT_U32      tcQueue  = 0;


    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with pfcTimer[0 / 5 / 7],
                           non NULL tcQueuePtr.
            Expected: GT_OK.
        */

        /* call with pfcTimer = 0 */
        pfcTimer = 0;

        st = cpssDxChPortPfcTimerToQueueMapGet(dev, pfcTimer, &tcQueue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, pfcTimer);

        /* call with pfcTimer = 5 */
        pfcTimer = 5;

        st = cpssDxChPortPfcTimerToQueueMapGet(dev, pfcTimer, &tcQueue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, pfcTimer);

        /* call with pfcTimer = 7 */
        pfcTimer = 7;

        st = cpssDxChPortPfcTimerToQueueMapGet(dev, pfcTimer, &tcQueue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, pfcTimer);

        /*
            1.2. Call with out of range pfcTimer [8]
                           and other valid parameters from 1.1.
            Expected: NOT GT_OK.
        */
        pfcTimer = 8;

        st = cpssDxChPortPfcTimerToQueueMapGet(dev, pfcTimer, &tcQueue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, pfcTimer);

        pfcTimer = 0;

        /*
            1.3. Call with NULL tcQueuePtr and other valid parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortPfcTimerToQueueMapGet(dev, pfcTimer, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tcQueuePtr = NULL", dev);
    }

    pfcTimer = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcTimerToQueueMapGet(dev, pfcTimer, &tcQueue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcTimerToQueueMapGet(dev, pfcTimer, &tcQueue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcShaperToPortRateRatioSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_U8                                   tcQueue,
    IN  GT_U32                                  shaperToPortRateRatio
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcShaperToPortRateRatioSet)
{
/*
    ITERATE_DEVICES (Lion)
    1.1. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_1_E /
                               CPSS_PORT_TX_SCHEDULER_PROFILE_3_E /
                               CPSS_PORT_TX_SCHEDULER_PROFILE_4_E],
                   tcQueue [0 / 5 / 7],
                   shaperToPortRateRatio [0 / 50 / 100].
    Expected: GT_OK.
    1.2. Call cpssDxChPortPfcShaperToPortRateRatioGet with the same profileSet.
    Expected: GT_OK and the same shaperToPortRateRatio.
    1.3. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_5_E /
                               CPSS_PORT_TX_SCHEDULER_PROFILE_8_E],
                   and other valid parameters from 1.1.
    Expected: NOT GT_OK for DxCh and GT_OK for others.
    1.4. Call with wrong enum values profileSet
                   and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range tcQueue [CPSS_TC_RANGE_CNS],
                   and other valid parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range shaperToPortRateRatio [101],
                   and other valid parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet;
    GT_U8                                   tcQueue;
    GT_U32                                  ratio;
    GT_U32                                  ratioGet;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_1_E ],
                           tcQueue [0],
                           shaperToPortRateRatio [0 .. 100].
            Expected: GT_OK.
        */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;
        tcQueue    = 0;

        for (ratio = 0; ratio <= 100; ratio++)
        {
            st = cpssDxChPortPfcShaperToPortRateRatioSet(dev, profileSet, tcQueue, ratio);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, profileSet, tcQueue, ratio);

            /*
                1.2. Call cpssDxChPortPfcShaperToPortRateRatioGet with the same profileSet.
                Expected: GT_OK and the same shaperToPortRateRatio.
            */
            st = cpssDxChPortPfcShaperToPortRateRatioGet(dev, profileSet, tcQueue, &ratioGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortPfcShaperToPortRateRatioGet: %d", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(ratio, ratioGet,
                       "get another value than was set: %d, %d", dev, ratioGet);
        }


        /*
            1.1. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_1_E /
                                       CPSS_PORT_TX_SCHEDULER_PROFILE_3_E /
                                       CPSS_PORT_TX_SCHEDULER_PROFILE_4_E],
                           tcQueue [0 / 5 / 7],
                           shaperToPortRateRatio [0 / 50 / 100].
            Expected: GT_OK.
        */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_3_E;
        tcQueue    = 5;
        ratio      = 50;

        st = cpssDxChPortPfcShaperToPortRateRatioSet(dev, profileSet, tcQueue, ratio);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, profileSet, tcQueue, ratio);

        /*
            1.2. Call cpssDxChPortPfcShaperToPortRateRatioGet with the same profileSet.
            Expected: GT_OK and the same shaperToPortRateRatio.
        */
        st = cpssDxChPortPfcShaperToPortRateRatioGet(dev, profileSet, tcQueue, &ratioGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortPfcShaperToPortRateRatioGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(ratio, ratioGet,
                   "get another value than was set: %d, %d", dev, ratioGet);

        /*
            1.1. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_1_E /
                                       CPSS_PORT_TX_SCHEDULER_PROFILE_3_E /
                                       CPSS_PORT_TX_SCHEDULER_PROFILE_4_E],
                           tcQueue [0 / 5 / 7],
                           shaperToPortRateRatio [0 / 50 / 100].
            Expected: GT_OK.
        */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_4_E;
        tcQueue    = 7;
        ratio      = 100;

        st = cpssDxChPortPfcShaperToPortRateRatioSet(dev, profileSet, tcQueue, ratio);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, profileSet, tcQueue, ratio);

        /*
            1.2. Call cpssDxChPortPfcShaperToPortRateRatioGet with the same profileSet.
            Expected: GT_OK and the same shaperToPortRateRatio.
        */
        st = cpssDxChPortPfcShaperToPortRateRatioGet(dev, profileSet, tcQueue, &ratioGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortPfcShaperToPortRateRatioGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(ratio, ratioGet,
                   "get another value than was set: %d, %d", dev, ratioGet);

        /*
            1.3. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_5_E /
                                       CPSS_PORT_TX_SCHEDULER_PROFILE_8_E],
                           and other valid parameters from 1.1.
            Expected: NOT GT_OK for DxCh and GT_OK for others.
        */
        /* call with  profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_5_E */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_5_E;

        st = cpssDxChPortPfcShaperToPortRateRatioSet(dev, profileSet, tcQueue, ratio);
        if ((PRV_CPSS_PP_MAC(dev)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, profileSet, tcQueue, ratio);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, profileSet, tcQueue, ratio);
        }

        /* call with  profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_8_E */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_8_E;

        st = cpssDxChPortPfcShaperToPortRateRatioSet(dev, profileSet, tcQueue, ratio);
        if ((PRV_CPSS_PP_MAC(dev)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, profileSet, tcQueue, ratio);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, profileSet, tcQueue, ratio);
        }

        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;

        /*
            1.4. Call with wrong enum values profileSet
                           and other valid parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPortPfcShaperToPortRateRatioSet
                            (dev, profileSet, tcQueue, ratio),
                            profileSet);

        /*
            1.5. Call with out of range tcQueue [CPSS_TC_RANGE_CNS],
                           and other valid parameters from 1.1.
            Expected: NOT GT_OK.
        */
        tcQueue = CPSS_TC_RANGE_CNS;

        st = cpssDxChPortPfcShaperToPortRateRatioSet(dev, profileSet, tcQueue, ratio);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcQueue);

        tcQueue = 0;

        /*
            1.6. Call with out of range shaperToPortRateRatio [101],
                           and other valid parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ratio = 101;

        st = cpssDxChPortPfcShaperToPortRateRatioSet(dev, profileSet, tcQueue, ratio);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ratio);
    }

    profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;
    tcQueue    = 0;
    ratio      = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcShaperToPortRateRatioSet(dev, profileSet, tcQueue, ratio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcShaperToPortRateRatioSet(dev, profileSet, tcQueue, ratio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcShaperToPortRateRatioGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_U8                                   tcQueue,
    OUT GT_U32                                 *shaperToPortRateRatioPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcShaperToPortRateRatioGet)
{
/*
    ITERATE_DEVICES (Lion)
    1.1. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_1_E /
                               CPSS_PORT_TX_SCHEDULER_PROFILE_3_E /
                               CPSS_PORT_TX_SCHEDULER_PROFILE_4_E],
                   tcQueue [0 / 5 / 7],
                   not NULL shaperToPortRateRatioPtr.
    Expected: GT_OK.
    1.2. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_5_E /
                               CPSS_PORT_TX_SCHEDULER_PROFILE_8_E],
                   and other valid parameters from 1.1.
    Expected: NOT GT_OK for DxCh and GT_OK for others.
    1.3. Call with wrong enum values profileSet
                   and other valid parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range tcQueue [CPSS_TC_RANGE_CNS],
                   and other valid parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with NULL shaperToPortRateRatioPtr,
                   and other valid parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet;
    GT_U8                                   tcQueue;
    GT_U32                                  ratio;


    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_1_E /
                                       CPSS_PORT_TX_SCHEDULER_PROFILE_3_E /
                                       CPSS_PORT_TX_SCHEDULER_PROFILE_4_E],
                           tcQueue [0 / 5 / 7],
                           not NULL shaperToPortRateRatioPtr.
            Expected: GT_OK.
        */
        /* call with  profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;
        tcQueue    = 0;

        st = cpssDxChPortPfcShaperToPortRateRatioGet(dev, profileSet, tcQueue, &ratio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, tcQueue);

        /* call with  profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_3_E */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_3_E;
        tcQueue    = 5;

        st = cpssDxChPortPfcShaperToPortRateRatioGet(dev, profileSet, tcQueue, &ratio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, tcQueue);

        /* call with  profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_4_E */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_4_E;
        tcQueue    = 7;

        st = cpssDxChPortPfcShaperToPortRateRatioGet(dev, profileSet, tcQueue, &ratio);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, tcQueue);

        /*
            1.2. Call with profileSet [CPSS_PORT_TX_SCHEDULER_PROFILE_5_E /
                                       CPSS_PORT_TX_SCHEDULER_PROFILE_8_E],
                           and other valid parameters from 1.1.
            Expected: NOT GT_OK for DxCh and GT_OK for others.
        */
        /* call with  profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_5_E */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_5_E;

        st = cpssDxChPortPfcShaperToPortRateRatioGet(dev, profileSet, tcQueue, &ratio);
        if ((PRV_CPSS_PP_MAC(dev)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, tcQueue);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, tcQueue);
        }

        /* call with  profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_8_E */
        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_8_E;

        st = cpssDxChPortPfcShaperToPortRateRatioGet(dev, profileSet, tcQueue, &ratio);
        if ((PRV_CPSS_PP_MAC(dev)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, tcQueue);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileSet, tcQueue);
        }

        profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;

        /*
            1.3. Call with wrong enum values profileSet
                           and other valid parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPortPfcShaperToPortRateRatioGet
                            (dev, profileSet, tcQueue, &ratio),
                            profileSet);

        /*
            1.4. Call with out of range tcQueue [CPSS_TC_RANGE_CNS],
                           and other valid parameters from 1.1.
            Expected: NOT GT_OK.
        */
        tcQueue = CPSS_TC_RANGE_CNS;

        st = cpssDxChPortPfcShaperToPortRateRatioGet(dev, profileSet, tcQueue, &ratio);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcQueue);

        tcQueue = 0;

        /*
            1.5. Call with NULL shaperToPortRateRatioPtr,
                           and other valid parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortPfcShaperToPortRateRatioGet(dev, profileSet, tcQueue, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, shaperToPortRateRatioPtr = NULL", dev);
    }

    profileSet = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;
    tcQueue    = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcShaperToPortRateRatioGet(dev, profileSet, tcQueue, &ratio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcShaperToPortRateRatioGet(dev, profileSet, tcQueue, &ratio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcForwardEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcForwardEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS(xCat2)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_PFC_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;

    /* Prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
               1.1.1. Call with non-null enablePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortPfcForwardEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
               1.1.2. Call with enablePtr [NULL].
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortPfcForwardEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /*
           1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortPfcForwardEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortPfcForwardEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortPfcForwardEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_PFC_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcForwardEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcForwardEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPfcForwardEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPfcForwardEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS(xCat2)
    1.1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortPfcForwardEnableGet.
    Expected: GT_OK and the same enable value as was set.
*/
    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port      = PORT_PFC_VALID_PHY_PORT_CNS;
    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_TRUE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* Call function with enable [GT_FALSE] */
            enable = GT_FALSE;
            st = cpssDxChPortPfcForwardEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call cpssDxChPortForward802_3xEnableGet.
                Expected: GT_OK and the same enable value as was set.
            */
            st = cpssDxChPortPfcForwardEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                    "cpssDxChPortPfcForwardEnableGet: %d", dev);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                    "get another enable than was set: %d, %d", dev, port);

            /* Call function with enable [GT_TRUE] */
            enable = GT_TRUE;
            st = cpssDxChPortPfcForwardEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call cpssDxChPortForward802_3xEnableGet.
                Expected: GT_OK and the same enable value as was set.
            */
            st = cpssDxChPortPfcForwardEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                    "cpssDxChPortPfcForwardEnableGet: %d", dev);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                    "get another enable than was set: %d, %d", dev, port);
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
            st = cpssDxChPortPfcForwardEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortPfcForwardEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortPfcForwardEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, enable);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    enable = GT_TRUE;
    port = PORT_PFC_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPfcForwardEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPfcForwardEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChPortPfc suit
 */

UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChPortPfc)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcCountingModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcCountingModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcGlobalDropEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcGlobalDropEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcGlobalQueueConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcGlobalQueueConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcProfileIndexSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcProfileIndexGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcProfileQueueConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcProfileQueueConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcTimerMapEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcTimerMapEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcTimerToQueueMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcTimerToQueueMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcShaperToPortRateRatioSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcShaperToPortRateRatioGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcForwardEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPfcForwardEnableSet)
UTF_SUIT_END_TESTS_MAC(cpssDxChPortPfc)

