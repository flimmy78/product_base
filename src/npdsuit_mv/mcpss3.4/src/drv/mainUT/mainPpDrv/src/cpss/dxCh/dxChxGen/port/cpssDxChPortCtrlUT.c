/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortCtrlUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChPortCtrl, that provides
*       CPSS implementation for Port configuration and control facility.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */

#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* defines */

/* Default valid value for port id */
#define PORT_CTRL_VALID_PHY_PORT_CNS  0

/* Default valid value for ipg */
#define PORT_IPG_VALID_CNS  8

#define IS_PORT_XG_E(devNum,portNum)    \
        (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType>=PRV_CPSS_PORT_XG_E)

#define IS_PORT_GE_E(devNum,portNum)    \
        (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType==PRV_CPSS_PORT_GE_E)

#define IS_PORT_FE_E(devNum,portNum)    \
        (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType==PRV_CPSS_PORT_FE_E)

#define IS_FLEX_PORT(_dev, _port) \
        (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(_dev, _port))

/* TRUE if XG port supports XAUI interface */
#define IS_XAUI_CAPABLE_XG_PORT(_dev, _port) \
        ((PRV_CPSS_DXCH_XG_PORT_TYPE_MAC(_dev,_port) == PRV_CPSS_XG_PORT_XG_ONLY_E) || \
         (PRV_CPSS_DXCH_XG_PORT_TYPE_MAC(_dev,_port) == PRV_CPSS_XG_PORT_XG_HX_QX_E) || \
         (PRV_CPSS_DXCH_XG_PORT_TYPE_MAC(_dev,_port) == PRV_CPSS_XG_PORT_XLG_SGMII_E))

/* TRUE if Flex port supports SGMII interface,
   Lion A0 does not support it */
#define IS_SGMII_CAPABLE_FLEX_PORT(_dev, _port) \
        (!((PRV_CPSS_PP_MAC(_dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) && \
           (PRV_CPSS_PP_MAC(_dev)->revision < 2)))

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortMacSaLsbSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U8    macSaLsb
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortMacSaLsbSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with macSaLsb [0 / 250].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortMacSaLsbGet with non-NULL macSaLsb.
    Expected: GT_OK and macSaLsb the same as just written.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port        = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_U8       macSaLsb    = 0;
    GT_U8       macSaLsbGet = 0;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with macSaLsb [0 / 250].
               Expected: GT_OK.
            */
            /* iterate with macSaLsb = 0 */
            macSaLsb = 0;

            st = cpssDxChPortMacSaLsbSet(dev, port, macSaLsb);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, macSaLsb);

            /*
                1.1.2. Call cpssDxChPortMacSaLsbGet with non-NULL macSaLsb.
                Expected: GT_OK and macSaLsb the same as just written.
            */
            st = cpssDxChPortMacSaLsbGet(dev, port, &macSaLsbGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortMacSaLsbGet: %d, %d", dev, port);

            /* verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(macSaLsb, macSaLsbGet,
                                         "got another macSaLsb as was written: %d, %d", dev, port);

            /* iterate with macSaLsb = 250 */
            macSaLsb = 250;

            st = cpssDxChPortMacSaLsbSet(dev, port, macSaLsb);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, macSaLsb);

            /*
                1.1.2. Call cpssDxChPortMacSaLsbGet with non-NULL macSaLsb.
                Expected: GT_OK and macSaLsb the same as just written.
            */
            st = cpssDxChPortMacSaLsbGet(dev, port, &macSaLsbGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortMacSaLsbGet: %d, %d", dev, port);

            /* verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(macSaLsb, macSaLsbGet,
                                         "got another macSaLsb as was written: %d, %d", dev, port);
        }

        /* 1.2. For all active devices go over all non available physical
           ports.
        */
        macSaLsb = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortMacSaLsbSet(dev, port, macSaLsb);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* macSaLsb == 0 */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortMacSaLsbSet(dev, port, macSaLsb);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    macSaLsb = 0;
    port     = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortMacSaLsbSet(dev, port, macSaLsb);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, macSaLsb == 0 */

    st = cpssDxChPortMacSaLsbSet(dev, port, macSaLsb);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortMacSaLsbGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_U8    *macSaLsb
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortMacSaLsbGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with non-NULL macSaLsb.
    Expected: GT_OK.
    1.1.2. Call with macSaLsb [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port        = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_U8       macSaLsb    = 0;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL macSaLsb.
                Expected: GT_OK.
            */
            st = cpssDxChPortMacSaLsbGet(dev, port, &macSaLsb);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with macSaLsb [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortMacSaLsbGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, macSaLsbPtr = NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available physical
           ports.
        */

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortMacSaLsbGet(dev, port, &macSaLsb);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortMacSaLsbGet(dev, port, &macSaLsb);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortMacSaLsbGet(dev, port, &macSaLsb);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortMacSaLsbGet(dev, port, &macSaLsb);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxChx)
    1.1.1. Call with state [GT_FALSE and GT_TRUE]. Expected: GT_OK.
    1.1.2. Call cpssDxChPortEnableGet.
    Expected: GT_OK and the same enable.
*/

    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;
    GT_BOOL     state  = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with state [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_FALSE] */
            enable = GT_FALSE;

            st = cpssDxChPortEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /* 1.1.2. Call cpssDxChPortEnableGet.
               Expected: GT_OK and the same enable.
            */
            st = cpssDxChPortEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "[cpssDxChPortEnableGet]: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, state,
                                         "get another enable value than was set: %d, %d", dev, port);

            /* 1.1.1. Call with state [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_TRUE] */
            enable = GT_TRUE;

            st = cpssDxChPortEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /* 1.1.2. Call cpssDxChPortEnableGet.
               Expected: GT_OK and the same enable.
            */
            st = cpssDxChPortEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "[cpssDxChPortEnableGet]: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, state,
                                         "get another enable value than was set: %d, %d", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* enable == GT_TRUE    */
            st = cpssDxChPortEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* enable == GT_TRUE  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        /* enable == GT_TRUE  */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    enable = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, enable == GT_TRUE */

    st = cpssDxChPortEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *statePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortEnableGet)
{
    /*
    ITERATE_DEVICES_PHY_PORTS (DxCh)
    1.1.1. Call with non-null statePtr.
    Expected: GT_OK.
    1.1.2. Call with statePtr [NULL].
    Expected: GT_BAD_PTR
    */

    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     state = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

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
            st = cpssDxChPortEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with statePtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortEnableGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortDuplexModeSet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    IN  CPSS_PORT_DUPLEX_ENT  dMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortDuplexModeSet)
{
    /*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh)
    1.1.1. Call with dMode [CPSS_PORT_FULL_DUPLEX_E].
    Expected: GT_OK.
    1.1.2. Call with dMode [CPSS_PORT_HALF_DUPLEX_E].
    Expected: GT_OK for non 10Gbps ports and GT_NOT_SUPPORTED for 10Gbps ports.
    1.1.3. Call with wrong enum values dMode
    Expected: GT_BAD_PARAM
    */

    GT_STATUS            st   = GT_OK;

    GT_U8                dev;
    GT_U8                port = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_PORT_DUPLEX_ENT mode = CPSS_PORT_FULL_DUPLEX_E;

    PRV_CPSS_PORT_TYPE_ENT  portType;

    GT_BOOL phyMacUsed;
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d",
                                         dev, port);

            /* Get PHY MAC object pnt */
            portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(dev,port);
        
            /* check if MACPHY callback should run */
            if (portMacObjPtr != NULL)
                phyMacUsed = GT_TRUE;
            else
                phyMacUsed = GT_FALSE;

            /* 1.1.1. Call with dMode [CPSS_PORT_FULL_DUPLEX_E].
               Expected: GT_OK.
            */

            /* Call with mode [CPSS_PORT_FULL_DUPLEX_E] */
            mode = CPSS_PORT_FULL_DUPLEX_E;

            st = cpssDxChPortDuplexModeSet(dev, port, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);

            /* 1.1.2. Call with dMode [CPSS_PORT_HALF_DUPLEX_E].
               Expected: GT_OK for non 10Gpbs ports and GT_NOT_SUPPORTED otherwise.
            */

            /* Call with mode [CPSS_PORT_HALF_DUPLEX_E] */
            mode = CPSS_PORT_HALF_DUPLEX_E;

            st = cpssDxChPortDuplexModeSet(dev, port, mode);
            if((PRV_CPSS_PORT_XG_E != portType) && (GT_FALSE == phyMacUsed))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, mode);
            }

            /*
               1.1.3. Call with wrong enum values dMode
               Expected: GT_BAD_PARAM
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPortDuplexModeSet
                                (dev, port, mode),
                                mode);
        }

        mode = CPSS_PORT_FULL_DUPLEX_E;

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* dMode [CPSS_PORT_FULL_DUPLEX_E]               */
            st = cpssDxChPortDuplexModeSet(dev, port, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* dMode == CPSS_PORT_FULL_DUPLEX_E */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortDuplexModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        /* dMode == CPSS_PORT_FULL_DUPLEX_E */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortDuplexModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    mode = CPSS_PORT_FULL_DUPLEX_E;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortDuplexModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, dMode == CPSS_PORT_FULL_DUPLEX_E */

    st = cpssDxChPortDuplexModeSet(dev, port, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortDuplexModeGet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_PORT_DUPLEX_ENT  *dModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortDuplexModeGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh)
    1.1.1. Call with non-null dModePtr.
    Expected: GT_OK.
    1.1.2. Call with dModePtr [NULL].
    Expected: GT_BAD_PTR.
*/

    GT_STATUS            st   = GT_OK;

    GT_U8                dev;
    GT_U8                port = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_PORT_DUPLEX_ENT mode;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null dModePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortDuplexModeGet(dev, port, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with dModePtr [NULL].
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortDuplexModeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortDuplexModeGet(dev, port, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortDuplexModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortDuplexModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortDuplexModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortDuplexModeGet(dev, port, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortSpeedSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh)
    1.1.1. Call with speed [CPSS_PORT_SPEED_1000_E].
    Expected: GT_OK for 10Gbps ports and GT_NOT_SUPPORTED for non-10Gbps ports.
    1.1.2. Call with speed [CPSS_PORT_SPEED_10_E, CPSS_PORT_SPEED_100_E,CPSS_PORT_SPEED_1000_E,].
    Expected: GT_OK for non -10Gbps ports and GT_NOT_SUPPORTED for 10Gbps ports.
    1.1.3. Call with wrong enum values speed.
    Expected: GT_BAD_PARAM.
*/

    GT_STATUS              st       = GT_OK;

    GT_U8                  dev;
    GT_U8                  port     = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_PORT_SPEED_ENT    speed    = CPSS_PORT_SPEED_10_E;
    PRV_CPSS_PORT_TYPE_ENT portType = PRV_CPSS_PORT_NOT_EXISTS_E;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;

    GT_BOOL phyMacUsed;
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d",
                                         dev, port);

            st = cpssDxChPortInterfaceModeGet(dev, port, &ifMode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortInterfaceModeGet: %d, %d",
                                         dev, port);

            if(CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E == ifMode)
            {
                continue;
            }

            /* Get PHY MAC object pnt */
            portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(dev,port);
        
            /* check if MACPHY callback should run */
            if (portMacObjPtr != NULL)
                phyMacUsed = GT_TRUE;
            else
                phyMacUsed = GT_FALSE;

            /* 1.1.1. Call with speed [CPSS_PORT_SPEED_10000_E].
               Expected: GT_OK for XG ports with XAUI (XGMII) for even ports or HX interface.
            */
            speed = CPSS_PORT_SPEED_10000_E;

            st = cpssDxChPortSpeedSet(dev, port, speed);
            if((PRV_CPSS_PORT_XG_E != portType) ||
               ((ifMode != CPSS_PORT_INTERFACE_MODE_XGMII_E) &&
                (ifMode != CPSS_PORT_INTERFACE_MODE_HX_E) &&
                (ifMode != CPSS_PORT_INTERFACE_MODE_RXAUI_E)))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC((phyMacUsed) ? GT_BAD_PARAM : GT_NOT_SUPPORTED, st, dev, port, speed);
            }
            else if(ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, speed);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }

            /* 1.1.2. Call with speed [CPSS_PORT_SPEED_10_E /
                                       CPSS_PORT_SPEED_100_E /
                                       CPSS_PORT_SPEED_1000_E].
               Expected: GT_OK for non-10Gbps ports and NON GT_OK for 10Gbps ports.
            */

            /* speed = CPSS_PORT_SPEED_10_E */
            speed = CPSS_PORT_SPEED_10_E;

            st = cpssDxChPortSpeedSet(dev, port, speed);
            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, speed);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }

            /* speed = CPSS_PORT_SPEED_100_E */
            speed = CPSS_PORT_SPEED_100_E;

            st = cpssDxChPortSpeedSet(dev, port, speed);
            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, speed);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }

            /* speed = CPSS_PORT_SPEED_1000_E */
            speed = CPSS_PORT_SPEED_1000_E;

            st = cpssDxChPortSpeedSet(dev, port, speed);
            if((PRV_CPSS_PORT_XG_E == portType) || (PRV_CPSS_PORT_FE_E  == portType))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, speed);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
            }

            if(PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
            {
                /* speed = CPSS_PORT_SPEED_16000_E */
                speed = CPSS_PORT_SPEED_16000_E;

                st = cpssDxChPortSpeedSet(dev, port, speed);
                if(PRV_CPSS_PORT_XG_E == portType)
                {
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
                }
                else
                {
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, speed);
                }
            }

            /*
               1.1.3. Call with wrong enum values speed
               Expected: GT_BAD_PARAM
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPortSpeedSet
                                (dev, port, speed),
                                speed);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        speed = CPSS_PORT_SPEED_1000_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* speed = CPSS_PORT_SPEED_1000_E                */
            st = cpssDxChPortSpeedSet(dev, port, speed);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* speed == CPSS_PORT_SPEED_1000_E */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortSpeedSet(dev, port, speed);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        /* speed == CPSS_PORT_SPEED_1000_E */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortSpeedSet(dev, port, speed);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, speed);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    speed = CPSS_PORT_SPEED_1000_E;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortSpeedSet(dev, port, speed);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, speed == CPSS_PORT_SPEED_1000_E */

    st = cpssDxChPortSpeedSet(dev, port, speed);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortSpeedGet)
{
    /*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh)
    1.1.1. Call with non-null speedPtr.
    Expected: GT_OK.
    1.1.2. Call with speedPtr [NULL].
    Expected: GT_BAD_PTR.
    */

    GT_STATUS           st    = GT_OK;

    GT_U8               dev;
    GT_U8               port  = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_PORT_SPEED_ENT speed;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null speedPtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortSpeedGet(dev, port, &speed);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with speedPtr [NULL].
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortSpeedGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortSpeedGet(dev, port, &speed);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortSpeedGet(dev, port, &speed);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortSpeedGet(dev, port, &speed);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortSpeedGet(dev, port, &speed);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortSpeedGet(dev, port, &speed);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortDuplexAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortDuplexAutoNegEnableSet)
{
    /*
        ITERATE_DEVICES_PHY_PORTS (DxCh)
        1.1.1. Call with state [GT_FALSE].
        Expected: GT_OK.
        1.1.2. Call with state [GT_TRUE].
        Expected: GT_OK for non 10Gbps ports and GT_NOT_SUPPORTED otherwise.
        1.1.3. Call cpssDxChPortDuplexAutoNegEnableGet with not null state.
        Expected: GT_OK and the same value.
    */

    GT_U8                  dev;
    GT_STATUS              st       = GT_OK;
    GT_U8                  port     = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL                state    = GT_FALSE;
    GT_BOOL                stateGet = GT_TRUE;
    PRV_CPSS_PORT_TYPE_ENT portType = PRV_CPSS_PORT_NOT_EXISTS_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with state [GT_FALSE].
               Expected: GT_OK.
            */
            state = GT_FALSE;

            st = cpssDxChPortDuplexAutoNegEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* 1.1.2. Call with state [GT_TRUE].
               Expected: GT_OK for non 10Gbps ports and GT_NOT_SUPPORTED otherwise.
            */
            state = GT_TRUE;

            st = prvUtfPortTypeGet(dev, port, &portType);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d",
                                         dev, port);

            st = cpssDxChPortDuplexAutoNegEnableSet(dev, port, state);

            if(PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, state);
            }

            /*
                1.1.3. Call cpssDxChPortDuplexAutoNegEnableGet with not null state.
                Expected: GT_OK and the same value.
            */
            /*Call with state [GT_FALSE] */
            state = GT_FALSE;

            st = cpssDxChPortDuplexAutoNegEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            st = cpssDxChPortDuplexAutoNegEnableGet(dev, port, &stateGet);

            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                     "got another state then was set: %d, %d", dev, port);

            /*Call with state [GT_TRUE] */
            state = GT_TRUE;

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d",
                                         dev, port);

            st = cpssDxChPortDuplexAutoNegEnableSet(dev, port, state);

            if(PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
                st = cpssDxChPortDuplexAutoNegEnableGet(dev, port, &stateGet);

                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
                UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                         "got another state then was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, state);
            }
        }

        state = GT_TRUE;

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* state = GT_TRUE  */
            st = cpssDxChPortDuplexAutoNegEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* enable == GT_TRUE */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortDuplexAutoNegEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        /* enable == GT_TRUE */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortDuplexAutoNegEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    state = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortDuplexAutoNegEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, state == GT_TRUE */

    st = cpssDxChPortDuplexAutoNegEnableSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortFlowCntrlAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state,
    IN  GT_BOOL   pauseAdvertise
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortFlowCntrlAutoNegEnableSet)
{
    /*
    ITERATE_DEVICES_PHY_PORTS (DxCh)
    1.1.1. Call with state [GT_FALSE] and pauseAdvertise [GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call with state [GT_TRUE] and pauseAdvertise [GT_FALSE and GT_TRUE].
    Expected: GT_OK for non 10Gbps ports and GT_NOT_SUPPORTED otherwise.
    */

    GT_STATUS              st       = GT_OK;

    GT_U8                  dev;
    GT_U8                  port     = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL                state    = GT_FALSE;
    GT_BOOL                pause    = GT_FALSE;
    GT_BOOL                stateGet = GT_FALSE;
    GT_BOOL                pauseGet = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT portType = PRV_CPSS_PORT_NOT_EXISTS_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with state [GT_FALSE], pauseAdvertise [GT_TRUE].
               Expected: GT_OK and the same values.
            */
            state = GT_FALSE;
            pause = GT_FALSE;

            st = cpssDxChPortFlowCntrlAutoNegEnableSet(dev, port, state, pause);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, state, pause);

            /* checking values*/
            st = cpssDxChPortFlowCntrlAutoNegEnableGet(dev, port, &stateGet, &pauseGet);

            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                     "got another state then was set: %d, %d", dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(pause, pauseGet,
                     "got another pause then was set: %d, %d", dev, port);

            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d",
                                         dev, port);

            /*
               1.1.2. Call with state [GT_TRUE]
               pauseAdvertise [GT_FALSE and GT_TRUE].
               Expected: GT_OK for non 10Gbps ports and GT_NOT_SUPPORTED otherwise..
            */

            /* Call function with pauseAdvertise = GT_FALSE] */
            state = GT_TRUE;
            pause = GT_FALSE;

            st = cpssDxChPortFlowCntrlAutoNegEnableSet(dev, port, state, pause);
            if(PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, state, pause);

                /* checking values*/
                st = cpssDxChPortFlowCntrlAutoNegEnableGet(dev, port, &stateGet, &pauseGet);

                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                         "got another state then was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(pause, pauseGet,
                         "got another pause then was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, state, pause);
            }

            /* Call function with pauseAdvertise = GT_TRUE] */
            pause = GT_TRUE;

            st = cpssDxChPortFlowCntrlAutoNegEnableSet(dev, port, state, pause);
            if(PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, state, pause);

                /* checking values*/
                st = cpssDxChPortFlowCntrlAutoNegEnableGet(dev, port, &stateGet, &pauseGet);

                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                         "got another state then was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(pause, pauseGet,
                         "got another pause then was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, state, pause);
            }
        }

        state = GT_TRUE;
        pause = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortFlowCntrlAutoNegEnableSet(dev, port, state, pause);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortFlowCntrlAutoNegEnableSet(dev, port, state, pause);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortFlowCntrlAutoNegEnableSet(dev, port, state, pause);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    state = GT_TRUE;
    pause = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortFlowCntrlAutoNegEnableSet(dev, port, state, pause);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, state == GT_TRUE, pause == GT_TRUE */

    st = cpssDxChPortFlowCntrlAutoNegEnableSet(dev, port, state, pause);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortSpeedAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortSpeedAutoNegEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with state [GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call with state [GT_TRUE ].
    Expected: GT_OK for non 10Gbps ports and GT_NOT_SUPPORTED otherwise.
    1.2. Call with port [CPSS_CPU_PORT_NUM_CNS = 63] and valid other parameters.
    Expected: GT_BAD_PARAM.
*/

    GT_STATUS              st       = GT_OK;

    GT_U8                  dev;
    GT_U8                  port     = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL                state    = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT portType = PRV_CPSS_PORT_NOT_EXISTS_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with state [GT_FALSE].
                Expected: GT_OK.
            */
            state = GT_FALSE;

            st = cpssDxChPortSpeedAutoNegEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d",
                                         dev, port);

            /*
                1.1.2. Call with state [GT_TRUE ].
                Expected: GT_OK for non 10Gbps ports and GT_NOT_SUPPORTED otherwise.
            */
            state = GT_TRUE;

            st = cpssDxChPortSpeedAutoNegEnableSet(dev, port, state);
            if(PRV_CPSS_PORT_XG_E != portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, state);
            }
        }

        state = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* state == GT_TRUE     */
            st = cpssDxChPortSpeedAutoNegEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* state == GT_TRUE */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortSpeedAutoNegEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        /* state == GT_TRUE */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortSpeedAutoNegEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    state = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortSpeedAutoNegEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, state == GT_TRUE */

    st = cpssDxChPortSpeedAutoNegEnableSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortFlowControlEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  CPSS_PORT_FLOW_CONTROL_ENT  state
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortFlowControlEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxChx)
    1.1.1. Call with state [CPSS_PORT_FLOW_CONTROL_DISABLE_E/
                            CPSS_PORT_FLOW_CONTROL_RX_TX_E  /
                            CPSS_PORT_FLOW_CONTROL_RX_ONLY_E/
                            CPSS_PORT_FLOW_CONTROL_TX_ONLY_E]
    Expected: GT_OK for XG ports and non GT_OK when not XG ports with
              last two values.
    1.1.2. Call cpssDxChPortFlowControlEnableGet with the same params.
    Expected: GT_OK and the same value.
    1.1.3. Call with wrong enum value state.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U8     port   = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_FLOW_CONTROL_ENT state  = CPSS_PORT_FLOW_CONTROL_DISABLE_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with state [CPSS_PORT_FLOW_CONTROL_DISABLE_E/
                                        CPSS_PORT_FLOW_CONTROL_RX_TX_E  /
                                        CPSS_PORT_FLOW_CONTROL_RX_ONLY_E/
                                        CPSS_PORT_FLOW_CONTROL_TX_ONLY_E]
                Expected: GT_OK for XG ports and non GT_OK when not XG ports with
                          last two values.
            */

            /* Call function with state = CPSS_PORT_FLOW_CONTROL_DISABLE_E */
            state = CPSS_PORT_FLOW_CONTROL_DISABLE_E;

            st = cpssDxChPortFlowControlEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssDxChPortFlowControlEnableGet returns actual
                Flow control status but not previouse settings of
                cpssDxChPortFlowControlEnableSet.
                The status depends on a lot of other configurations like
                flow control autonegatiation, type of PHY,
                PHY - MAC autonegatiation and so on.
                The status always disabled in simulation.
            */

            /* Call function with state = CPSS_PORT_FLOW_CONTROL_DISABLE_E */
            state = CPSS_PORT_FLOW_CONTROL_RX_TX_E;

            st = cpssDxChPortFlowControlEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* Call function with state = CPSS_PORT_FLOW_CONTROL_RX_ONLY_E */
            state = CPSS_PORT_FLOW_CONTROL_RX_ONLY_E;

            st = cpssDxChPortFlowControlEnableSet(dev, port, state);

            if (IS_PORT_XG_E(dev, port))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            }


            /* Call function with state = CPSS_PORT_FLOW_CONTROL_TX_ONLY_E */
            state = CPSS_PORT_FLOW_CONTROL_TX_ONLY_E;

            st = cpssDxChPortFlowControlEnableSet(dev, port, state);

            if (IS_PORT_XG_E(dev, port))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            }

            state = CPSS_PORT_FLOW_CONTROL_DISABLE_E;

            /*
                1.1.3. Call with wrong enum value state.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPortFlowControlEnableSet
                                (dev, port, state),
                                state);
        }

        state = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /*
           1.2. For all active devices go over all non available physical
           ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* state == GT_TRUE */
            st = cpssDxChPortFlowControlEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* state == GT_TRUE */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortFlowControlEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        /* state == GT_TRUE */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortFlowControlEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    state = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortFlowControlEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, state == GT_TRUE */

    st = cpssDxChPortFlowControlEnableSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortFlowControlEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT CPSS_PORT_FLOW_CONTROL_ENT  *statePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortFlowControlEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxChx)
    1.1.1. Call with non-null statePtr.
    Expected: GT_OK.
    1.1.2. Call with statePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS  st    = GT_OK;

    GT_U8      dev;
    GT_U8      port  = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_FLOW_CONTROL_ENT  state;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
               1.1.1. Call with non-null statePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortFlowControlEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
               1.1.2. Call with statePtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortEnableGet(dev, port, NULL);
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
            st = cpssDxChPortFlowControlEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortFlowControlEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortFlowControlEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortFlowControlEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortFlowControlEnableGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPeriodicFcEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPeriodicFcEnableSet)
{
    /*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE]. Expected: GT_OK.
    */

    GT_STATUS   st        = GT_OK;

    GT_U8       dev;
    GT_U8       port      = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with enable [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_FALSE */
            enable = GT_FALSE;

            st = cpssDxChPortPeriodicFcEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            st = cpssDxChPortPeriodicFcEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                    "cpssDxChPortPeriodicFcEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                    "get another enable value than was set: %d, %d", dev, port);

            /* Call function with enable = GT_TRUE */
            enable = GT_TRUE;

            st = cpssDxChPortPeriodicFcEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            st = cpssDxChPortPeriodicFcEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                    "cpssDxChPortPeriodicFcEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                    "get another enable value than was set: %d, %d", dev, port);

        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* enable == GT_TRUE    */
            st = cpssDxChPortPeriodicFcEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* enable == GT_TRUE */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortPeriodicFcEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        /* enable == GT_TRUE */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortPeriodicFcEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    enable = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPeriodicFcEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, enable == GT_TRUE */

    st = cpssDxChPortPeriodicFcEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortBackPressureEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortBackPressureEnableSet)
{
    /*
        ITERATE_DEVICES_PHY_CPU_PORTS (DxCh)
        1.1.1. Call with state [GT_FALSE and GT_TRUE].
        Expected: GT_OK for non-10Gbps ports and GT_NOT_SUPPORTED otherwise.
        1.1.2. Call cpssDxChPortBackPressureEnableGet with not null state.
        Expected: GT_OK for non-10Gbps ports and the same value.
                  GT_NOT_SUPPORTED otherwise.
    */

    GT_STATUS              st       = GT_OK;
    GT_U8                  dev;
    GT_U8                  port     = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL                state    = GT_FALSE;
    GT_BOOL                stateGet = GT_TRUE;
    PRV_CPSS_PORT_TYPE_ENT portType = PRV_CPSS_PORT_NOT_EXISTS_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with state [GT_FALSE and GT_TRUE].
                Expected: GT_OK for non-10Gbps ports and GT_NOT_SUPPORTED otherwise.
            */
            state = GT_FALSE;

            st = cpssDxChPortBackPressureEnableSet(dev, port, state);

            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, state);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            }

            /*
                1.1.1. Call with state [GT_FALSE and GT_TRUE].
                Expected: GT_OK for non-10Gbps ports and GT_NOT_SUPPORTED otherwise.
            */
            state = GT_TRUE;

            st = cpssDxChPortBackPressureEnableSet(dev, port, state);

            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, state);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            }
            /*
                1.1.2. Call cpssDxChPortBackPressureEnableGet with not null state.
                Expected: GT_OK for non-10Gbps ports and the same value.
                          GT_NOT_SUPPORTED otherwise.
            */
            state = GT_FALSE;
            st = cpssDxChPortBackPressureEnableSet(dev, port, state);

            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, state);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

                /*checking value*/
                st = cpssDxChPortBackPressureEnableGet(dev, port, &stateGet);

                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
                UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                 "cpssDxChPortBackPressureEnableGet: got another state then was set: %d, %d",
                                             dev, port);
            }
        }

        state = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /*
            1.2. For all active devices go over all non available
            physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* state = GT_TRUE  */
            st = cpssDxChPortBackPressureEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        /* state == GT_TRUE */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortBackPressureEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */

        /* state == GT_TRUE */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortBackPressureEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    state = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortBackPressureEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, state == GT_TRUE */

    st = cpssDxChPortBackPressureEnableSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortLinkStatusGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *isLinkUpPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortLinkStatusGet)
{
    /*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh)
    1.1.1. Call with non-null isLinkUpPtr.
    Expected: GT_OK.
    1.1.2. Call with isLinkUpPtr [NULL].
    Expected: GT_BAD_PTR.
    */

    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     state = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null isLinkUpPtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortLinkStatusGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with isLinkUpPtr [NULL].
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortLinkStatusGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortLinkStatusGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortLinkStatusGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortLinkStatusGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortLinkStatusGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortLinkStatusGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortInterfaceModeSet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortInterfaceModeSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh)
    1.1.1. Call with ifMode[CPSS_PORT_INTERFACE_MODE_SGMII_E /
                            CPSS_PORT_INTERFACE_MODE_1000BASE_X_E].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortInterfaceModeGet with not-NULL ifModePtr.
    Expected: GT_OK and the same ifMode as was set.
    1.1.3. Call with ifMode [CPSS_PORT_INTERFACE_MODE_MII_E] (only for CPU port).
    Expected: NOT GT_OK.
    1.1.4. Call with wrong enum values ifMode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_INTERFACE_MODE_ENT ifMode    = CPSS_PORT_INTERFACE_MODE_NA_E;
    CPSS_PORT_INTERFACE_MODE_ENT ifModeGet = CPSS_PORT_INTERFACE_MODE_NA_E;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* the Interface for XG port cannot be changed */

            /* need add proper support for xCat Flex port */
            CPSS_TBD_BOOKMARK;
            if (!IS_PORT_XG_E(dev,port) &&
                /* tmp patch to exclude Flex ports*/ (port < 24))
            {
                /*
                    1.1.1. Call with ifMode[CPSS_PORT_INTERFACE_MODE_SGMII_E /
                                            CPSS_PORT_INTERFACE_MODE_1000BASE_X_E].
                    Expected: GT_OK.
                */
                /* iterate with ifMode = CPSS_PORT_INTERFACE_MODE_SGMII_E */
                ifMode = CPSS_PORT_INTERFACE_MODE_SGMII_E;

                st = cpssDxChPortInterfaceModeSet(dev, port, ifMode);
                if (IS_PORT_FE_E(dev,port))
                {
                    UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ifMode);
                }
                else
                {
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ifMode);

                    /*
                        1.1.2. Call cpssDxChPortInterfaceModeGet with not-NULL ifModePtr.
                        Expected: GT_OK and the same ifMode as was set.
                    */
                    st = cpssDxChPortInterfaceModeGet(dev, port, &ifModeGet);
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "cpssDxChPortInterfaceModeGet: %d, %d", dev, port);

                    /* verifying values */
                    UTF_VERIFY_EQUAL2_STRING_MAC(ifMode, ifModeGet,
                         "got another ifMode then was set: %d, %d", dev, port);
                }

                /* iterate with ifMode = CPSS_PORT_INTERFACE_MODE_1000BASE_X_E */
                ifMode = CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;

                st = cpssDxChPortInterfaceModeSet(dev, port, ifMode);
                if (IS_PORT_FE_E(dev,port))
                {
                    UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ifMode);
                }
                else
                {
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ifMode);

                    /*
                        1.1.2. Call cpssDxChPortInterfaceModeGet with not-NULL ifModePtr.
                        Expected: GT_OK and the same ifMode as was set.
                    */
                    st = cpssDxChPortInterfaceModeGet(dev, port, &ifModeGet);
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "cpssDxChPortInterfaceModeGet: %d, %d", dev, port);

                    /* verifying values */
                    UTF_VERIFY_EQUAL2_STRING_MAC(ifMode, ifModeGet,
                         "got another ifMode then was set: %d, %d", dev, port);
                }
                /*
                    1.1.3. Call with ifMode [CPSS_PORT_INTERFACE_MODE_MII_E]
                           (only for CPU port).
                    Expected: NOT GT_OK.
                */
                ifMode = CPSS_PORT_INTERFACE_MODE_MII_E;

                st = cpssDxChPortInterfaceModeSet(dev, port, ifMode);
                if (!IS_PORT_FE_E(dev,port))
                {
                    UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ifMode);
                }
                else
                {
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ifMode);

                    /*
                        1.1.2. Call cpssDxChPortInterfaceModeGet with not-NULL ifModePtr.
                        Expected: GT_OK and the same ifMode as was set.
                    */
                    st = cpssDxChPortInterfaceModeGet(dev, port, &ifModeGet);
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "cpssDxChPortInterfaceModeGet: %d, %d", dev, port);

                    /* verifying values */
                    UTF_VERIFY_EQUAL2_STRING_MAC(ifMode, ifModeGet,
                         "got another ifMode then was set: %d, %d", dev, port);
                }

                /*
                    1.1.4. Call with wrong enum values ifMode.
                    Expected: GT_BAD_PARAM.
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChPortInterfaceModeSet
                                    (dev, port, ifMode),
                                    ifMode);
            }
            else
            {
                /* the Interface for NOT Flex XG port cannot be changed */
                /* ifMode = CPSS_PORT_INTERFACE_MODE_SGMII_E   */
                /* Expected: GT_BAD_PARAM                      */
                ifMode = CPSS_PORT_INTERFACE_MODE_SGMII_E;

                st = cpssDxChPortInterfaceModeSet(dev, port, ifMode);
                if (IS_FLEX_PORT(dev, port))
                {
                    if (IS_SGMII_CAPABLE_FLEX_PORT(dev, port))
                    {
                        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ifMode);
                    }
                    else
                    {
                        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, ifMode);
                }
                }
                else
                {
                    if(IS_PORT_GE_E(dev, port))
                        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ifMode);
                    else
                        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, ifMode);
                }

                /* ifMode = CPSS_PORT_INTERFACE_MODE_XGMII_E   */
                /* Expected: GT_OK for XAUI capable ports     */
                ifMode = CPSS_PORT_INTERFACE_MODE_XGMII_E;

                st = cpssDxChPortInterfaceModeSet(dev, port, ifMode);

                if (IS_XAUI_CAPABLE_XG_PORT(dev, port))
                {
                    if ((PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) &&
                        (port%2 != 0))
                    {
                        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, ifMode);
                    }
                    else
                    {
                        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, ifMode);
                    }
                }
                else
                {
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, ifMode);
                }
            }
        }

        ifMode = CPSS_PORT_INTERFACE_MODE_GMII_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortInterfaceModeSet(dev, port, ifMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortInterfaceModeSet(dev, port, ifMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortInterfaceModeSet(dev, port, ifMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    ifMode = CPSS_PORT_INTERFACE_MODE_SGMII_E;

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortInterfaceModeSet(dev, port, ifMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortInterfaceModeSet(dev, port, ifMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortInterfaceModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortInterfaceModeGet)
{
/*
    ITERATE_DEVICES_PHY_OR_CPU_PORTS (DxCh)
    1.1.1. Call with non-null ifModePtr.
    Expected: GT_OK.
    1.1.2. Call with ifModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                    st   = GT_OK;

    GT_U8                        dev;
    GT_U8                        port = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_PORT_INTERFACE_MODE_ENT mode = CPSS_PORT_INTERFACE_MODE_NA_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null ifModePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortInterfaceModeGet(dev, port, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with ifModePtr [NULL].
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortInterfaceModeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortInterfaceModeGet(dev, port, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortInterfaceModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortInterfaceModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortInterfaceModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortInterfaceModeGet(dev, port, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortForceLinkPassEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortForceLinkPassEnableSet)
{
    /*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh)
    1.1.1. Call with state [GT_FALSE and GT_TRUE]. Expected: GT_OK.
    */

    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     state  = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with state [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_FALSE] */
            state = GT_FALSE;

            st = cpssDxChPortForceLinkPassEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* Call function with enable = GT_TRUE] */
            state = GT_TRUE;

            st = cpssDxChPortForceLinkPassEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
        }

        state = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* state == GT_TRUE     */
            st = cpssDxChPortForceLinkPassEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* state == GT_TRUE */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortForceLinkPassEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        /* state == GT_TRUE */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortForceLinkPassEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    state = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortForceLinkPassEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, state == GT_TRUE */

    st = cpssDxChPortForceLinkPassEnableSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortForceLinkDownEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortForceLinkDownEnableSet)
{
    /*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh)
    1.1.1. Call with state [GT_FALSE and GT_TRUE]. Expected: GT_OK.
    */

    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     state  = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with state [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_FALSE] */
            state = GT_FALSE;

            st = cpssDxChPortForceLinkDownEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* Call function with enable = GT_TRUE] */
            state = GT_TRUE;

            st = cpssDxChPortForceLinkDownEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
        }

        state = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* state == GT_TRUE */
            st = cpssDxChPortForceLinkDownEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* state == GT_TRUE */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortForceLinkDownEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        /* state == GT_TRUE */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortForceLinkDownEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    state = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortForceLinkDownEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, state == GT_TRUE */

    st = cpssDxChPortForceLinkDownEnableSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortMruSet
(
    IN  GT_U8  devNum,
    IN  GT_U8  portNum,
    IN  GT_U32 mruSize
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortMruSet)
{
    /*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh)
    1.1.1. Call with mruSize [0, 16382].
    Expected: GT_OK and the same value.
    1.1.2. Check that odd values are unsupported. Call with mruSize [1, 16383].
    Expected: NOT GT_OK.
    1.1.3. Call with mruSize [16384] (out of range).
    Expected: NOT GT_OK.
    1.2. Call with port [CPSS_CPU_PORT_NUM_CNS = 63] and valid other parameters.
    Expected: GT_OK.
    */

    GT_STATUS st      = GT_OK;

    GT_U8     dev;
    GT_U8     port    = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_U32    size    = 0;
    GT_U32    sizeGet = 1;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with mruSize [0, 16382].
               Expected: GT_OK and the same value.
            */

            /* mruSize = 0 */
            size = 0;

            st = cpssDxChPortMruSet(dev, port, size);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, size);

            /*checking value*/
            st = cpssDxChPortMruGet(dev, port, &sizeGet);

            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, size);
            UTF_VERIFY_EQUAL2_STRING_MAC(size, sizeGet,
                     "got another size then was set: %d, %d", dev, port);

            /* mruSize = 16382 */
            size = 16382;

            st = cpssDxChPortMruSet(dev, port, size);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, size);

            /*checking value*/
            st = cpssDxChPortMruGet(dev, port, &sizeGet);

            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, size);
            UTF_VERIFY_EQUAL2_STRING_MAC(size, sizeGet,
                     "got another size then was set: %d, %d", dev, port);

            /* 1.1.2. Check that odd values are unsupported.
                      Call with mruSize [1, 16383].
               Expected: NOT GT_OK.
            */

            /* mruSize = 1 */
            size = 1;

            st = cpssDxChPortMruSet(dev, port, size);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, size);

            /* mruSize = 16383 */
            size = 16383;

            st = cpssDxChPortMruSet(dev, port, size);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, size);

            /* 1.1.3. Call with mruSize [16384] (out of range).
               Expected: NOT GT_OK.
            */
            size = 16384;

            st = cpssDxChPortMruSet(dev, port, size);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, size);
        }

        size = 500;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortMruSet(dev, port, size);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortMruSet(dev, port, size);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortMruSet(dev, port, size);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, size);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    size = 500;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortMruSet(dev, port, size);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, size == 500 */

    st = cpssDxChPortMruSet(dev, port, size);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCrcCheckEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCrcCheckEnableSet)
{
    /*
        ITERATE_DEVICES_PHY_CPU_PORTS (DxCh)
        1.1.1. Call with state [GT_FALSE and GT_TRUE].
        Expected: GT_OK.
        1.1.2. Call cpssDxChPortCrcCheckEnableGet with not null state.
        Expected: GT_OK and the same value.
    */
    GT_U8       dev;
    GT_STATUS   st       = GT_OK;
    GT_U8       port     = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_TRUE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with state [GT_FALSE and GT_TRUE].
               Expected: GT_OK and the same value.
            */

            /* Call function with enable = GT_FALSE] */
            state = GT_FALSE;

            st = cpssDxChPortCrcCheckEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* Call function with enable = GT_TRUE] */
            state = GT_TRUE;

            st = cpssDxChPortCrcCheckEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssDxChPortCrcCheckEnableGet with not null state.
                Expected: GT_OK and the same value.
            */
            state = GT_FALSE;

            st = cpssDxChPortCrcCheckEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*checking value*/
            st = cpssDxChPortCrcCheckEnableGet(dev, port, &stateGet);

            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                     "got another state then was set: %d, %d", dev, port);
        }

        state = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* state == GT_TRUE */
            st = cpssDxChPortCrcCheckEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* state == GT_TRUE */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortCrcCheckEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        /* state == GT_TRUE */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortCrcCheckEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    state = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCrcCheckEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, state == GT_TRUE */

    st = cpssDxChPortCrcCheckEnableSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortXGmiiModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_XGMII_MODE_ENT    mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortXGmiiModeSet)
{
    /*
    ITERATE_DEVICES_PHY_PORTS (DxCh)
    1.1.1. Call with mode
    [CPSS_PORT_XGMII_LAN_E, CPSS_PORT_XGMII_WAN_E].
    Expected: GT_OK for 10Gbps ports and GT_NOT_SUPPORTED for others.
    1.1.2. For 10Gbps port call with mode [3].
    Expected: GT_BAD_PARAM.
    */

    GT_STATUS                st       = GT_OK;
    GT_U8                    dev;
    GT_U8                    port     = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_PORT_XGMII_MODE_ENT mode     = CPSS_PORT_XGMII_LAN_E;
    PRV_CPSS_PORT_TYPE_ENT   portType = PRV_CPSS_PORT_NOT_EXISTS_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with mode [CPSS_PORT_XGMII_LAN_E /
                                      CPSS_PORT_XGMII_WAN_E].
               Expected: GT_OK for 10Gbps ports and GT_NOT_SUPPORTED for others.
            */

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);

            /* mode = CPSS_PORT_XGMII_LAN_E */
            mode = CPSS_PORT_XGMII_LAN_E;

            st = cpssDxChPortXGmiiModeSet(dev, port, mode);
            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, mode);
            }

            /* mode = CPSS_PORT_XGMII_WAN_E */
            mode = CPSS_PORT_XGMII_WAN_E;

            st = cpssDxChPortXGmiiModeSet(dev, port, mode);
            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, mode);
            }

            /*
               1.1.2. For 10Gbps port call with wrong enum values mode.
               Expected: GT_BAD_PARAM.
            */
            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_ENUMS_CHECK_MAC(cpssDxChPortXGmiiModeSet
                                    (dev, port, mode),
                                    mode);
            }
        }

        mode = CPSS_PORT_XGMII_LAN_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* mode = CPSS_PORT_XGMII_LAN_E                 */
            st = cpssDxChPortXGmiiModeSet(dev, port, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* mode = CPSS_PORT_XGMII_LAN_E */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortXGmiiModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        /* mode = CPSS_PORT_XGMII_LAN_E */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortXGmiiModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    mode = CPSS_PORT_XGMII_LAN_E;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortXGmiiModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, mode = CPSS_PORT_XGMII_LAN_E               */

    st = cpssDxChPortXGmiiModeSet(dev, port, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortIpgSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  ipg
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortIpgSet)
{
    /*
    ITERATE_DEVICES_PHY_PORTS (DxCh)
    Expected: GT_NOT_SUPPORTED XG ports GT_OK for others.
    */

    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port     = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_U32      ipg      = PORT_IPG_VALID_CNS;
    GT_U32      ipgGet;
    CPSS_PP_FAMILY_TYPE_ENT                 devFamily;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        ipg      = PORT_IPG_VALID_CNS;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1.
               GT_NOT_SUPPORTED XG ports GT_OK for others.
            */
            st = cpssDxChPortIpgSet(dev, port, ipg);
            if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) == PRV_CPSS_PORT_XG_E)
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortIpgSet(dev, port, ipg);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3 For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortIpgSet(dev, port, ipg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* 1.4 check for valid and invalid IPG values */
        port = PORT_CTRL_VALID_PHY_PORT_CNS;
        /* skip XG ports */
        if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) == PRV_CPSS_PORT_XG_E)
            continue;

        ipg  = PORT_IPG_VALID_CNS;

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* ipg  = PORT_IPG_VALID_CNS(8), valid for all DxCh devices */
        st = cpssDxChPortIpgSet(dev, port, ipg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        st = cpssDxChPortIpgGet(dev, port, &ipgGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                     "cpssDxChPortIpgGet: %d, %d", dev, port);
        UTF_VERIFY_EQUAL2_STRING_MAC(ipg, ipgGet,
                                     "get another enable value than was set: %d, %d", dev, port);

        /* ipg  = 15, valid for all DxCh devices */
        ipg = 15;

        st = cpssDxChPortIpgSet(dev, port, ipg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        st = cpssDxChPortIpgGet(dev, port, &ipgGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                     "cpssDxChPortIpgGet: %d, %d", dev, port);
        UTF_VERIFY_EQUAL2_STRING_MAC(ipg, ipgGet,
                                     "get another enable value than was set: %d, %d", dev, port);

        /* ipg  = 16, valid for Ch3 and above */
        ipg = 16;

        st = cpssDxChPortIpgSet(dev, port, ipg);
        if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            st = cpssDxChPortIpgGet(dev, port, &ipgGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                     "cpssDxChPortIpgGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(ipg, ipgGet,
                                     "get another enable value than was set: %d, %d", dev, port);
        }
        else /* CPSS_PP_FAMILY_CHEETAH_E  or CPSS_PP_FAMILY_CHEETAH2_E */
        {
           UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, port, ipg);
        }

        /* ipg  = 511, valid for Ch3 and above */
        ipg = 511;

        st = cpssDxChPortIpgSet(dev, port, ipg);
        if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            st = cpssDxChPortIpgGet(dev, port, &ipgGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                     "cpssDxChPortIpgGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(ipg, ipgGet,
                                     "get another enable value than was set: %d, %d", dev, port);
        }
        else /* CPSS_PP_FAMILY_CHEETAH_E  or CPSS_PP_FAMILY_CHEETAH2_E */
        {
           UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, port, ipg);
        }

        /* ipg  = 512, invalid for all DxCh devices */
        ipg = 512;

        st = cpssDxChPortIpgSet(dev, port, ipg);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, port, ipg);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    ipg  = PORT_IPG_VALID_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortIpgSet(dev, port, ipg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0 */

    st = cpssDxChPortIpgSet(dev, port, ipg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortIpgGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT  GT_U32  *ipgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortIpgGet)
{
    /*
    ITERATE_DEVICES_PHY_PORTS (DxCh)
    Expected: GT_NOT_SUPPORTED XG ports GT_OK for others.
    */

    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port     = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_U32      ipg;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1.
               GT_NOT_SUPPORTED XG ports GT_OK for others.
            */
            st = cpssDxChPortIpgGet(dev, port, &ipg);
            if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) == PRV_CPSS_PORT_XG_E)
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                /* 1.1.2. Call with speedPtr [NULL].
                Expected: GT_BAD_PTR.
                */
                st = cpssDxChPortIpgGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortIpgGet(dev, port, &ipg);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3 For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortIpgGet(dev, port, &ipg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortIpgGet(dev, port, &ipg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0 */

    st = cpssDxChPortIpgGet(dev, port, &ipg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortExtraIpgSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       number
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortExtraIpgSet)
{
    /*
    ITERATE_DEVICES_PHY_PORTS (DxCh)
    1.1.1. Call with number [2].
    Expected: GT_OK for 10-Gbps ports and GT_NOT_SUPPORTED for others.
    */

    GT_STATUS   st        = GT_OK;
    GT_U8       dev;
    GT_U8       port      = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_U8       number    = 2;
    GT_U8       numberGet = 1;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with number [2].
               Expected: GT_OK for 10-Gbps ports and GT_NOT_SUPPORTED for others.
            */
            st = cpssDxChPortExtraIpgSet(dev, port, number);

            if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) != PRV_CPSS_PORT_XG_E)
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, number);
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);

                st = cpssDxChPortExtraIpgGet(dev, port, &numberGet);

                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);
                UTF_VERIFY_EQUAL2_STRING_MAC(number, numberGet,
                         "got another number then was set: %d, %d", dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* number == 2 */

            st = cpssDxChPortExtraIpgSet(dev, port, number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* number == 2 */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortExtraIpgSet(dev, port, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        /* number == 2 */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortExtraIpgSet(dev, port, number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* number == 2 */
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortExtraIpgSet(dev, port, number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, number == 2 */

    st = cpssDxChPortExtraIpgSet(dev, port, number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortXgmiiLocalFaultGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *isLocalFaultPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortXgmiiLocalFaultGet)
{
    /*
    ITERATE_DEVICES_PHY_PORTS (DxCh)
    1.1.1. Call with non-null isLocalFaultPtr.
    Expected: GT_OK  for 10-Gbps ports and GT_NOT_SUPPORTED for others.
    1.1.2. Call with isLocalFaultPtr [NULL].
    Expected: GT_BAD_PTR.
    */

    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     state = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null isLocalFaultPtr.
               Expected: GT_OK  for 10-Gbps ports and GT_NOT_SUPPORTED for others.
            */
            st = cpssDxChPortXgmiiLocalFaultGet(dev, port, &state);
            if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) != PRV_CPSS_PORT_XG_E)
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            else
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with isLocalFaultPtr [NULL].
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortXgmiiLocalFaultGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortXgmiiLocalFaultGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortXgmiiLocalFaultGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortXgmiiLocalFaultGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortXgmiiLocalFaultGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortXgmiiLocalFaultGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortXgmiiRemoteFaultGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *isRemoteFaultPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortXgmiiRemoteFaultGet)
{
    /*
    ITERATE_DEVICES_PHY_PORTS (DxCh)
    1.1.1. Call with non-null isRemoteFaultPtr.
    Expected: GT_OK  for 10-Gbps ports and GT_NOT_SUPPORTED for others.
    1.1.2. Call with isRemoteFaultPtr [NULL].
    Expected: GT_BAD_PTR.
    */

    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     state = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null isRemoteFaultPtr.
               Expected: GT_OK for 10-Gbps ports and GT_NOT_SUPPORTED for others.
            */
            st = cpssDxChPortXgmiiRemoteFaultGet(dev, port, &state);
            if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) != PRV_CPSS_PORT_XG_E)
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            else
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with isRemoteFaultPtr [NULL].
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortXgmiiRemoteFaultGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortXgmiiRemoteFaultGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortXgmiiRemoteFaultGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortXgmiiRemoteFaultGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortXgmiiRemoteFaultGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortXgmiiRemoteFaultGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortMacStatusGet
(
    IN  GT_U8                      devNum,
    IN  GT_U8                      portNum,
    OUT CPSS_PORT_MAC_STATUS_STC   *portMacStatusPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortMacStatusGet)
{
    /*
    ITERATE_DEVICES_PHY_PORTS (DxCh)
    1.1.1. Call with non-null portMacStatusPtr.
    Expected: GT_OK.
    1.1.2. Call with portMacStatusPtr [NULL].
    Expected: GT_BAD_PTR.
    */

    GT_STATUS                st     = GT_OK;

    GT_U8                    dev;
    GT_U8                    port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_PORT_MAC_STATUS_STC status;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null portMacStatusPtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortMacStatusGet(dev, port, &status);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with portMacStatusPtr [NULL].
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortMacStatusGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortMacStatusGet(dev, port, &status);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortMacStatusGet(dev, port, &status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortMacStatusGet(dev, port, &status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortMacStatusGet(dev, port, &status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortMacStatusGet(dev, port, &status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortInternalLoopbackEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortInternalLoopbackEnableSet)
{
    /*
    ITERATE_DEVICES_PHY_PORTS (DxCh)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortInternalLoopbackEnableGet.
    Expected: GT_OK and enablePtr the same as was written.
    */

    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;
    GT_BOOL     state  = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with state [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_FALSE] */
            enable = GT_FALSE;

            st = cpssDxChPortInternalLoopbackEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /* 1.1.2. Call cpssDxChPortInternalLoopbackEnableGet.
               Expected: GT_OK and enablePtr the same as was written.
            */
            st = cpssDxChPortInternalLoopbackEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "cpssDxChPortInternalLoopbackEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, state,
                                         "get another enable value than was set: %d, %d", dev, port);

            /* 1.1.1. Call with state [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_TRUE] */
            enable = GT_TRUE;

            st = cpssDxChPortInternalLoopbackEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /* 1.1.2. Call cpssDxChPortInternalLoopbackEnableGet.
               Expected: GT_OK and enablePtr the same as was written.
            */
            st = cpssDxChPortInternalLoopbackEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "cpssDxChPortInternalLoopbackEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, state,
                                         "get another enable value than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* enable == GT_TRUE */
            st = cpssDxChPortInternalLoopbackEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* enable == GT_TRUE */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortInternalLoopbackEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        /* enable == GT_TRUE */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortInternalLoopbackEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    enable = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortInternalLoopbackEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, enable == GT_TRUE */

    st = cpssDxChPortInternalLoopbackEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortInternalLoopbackEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortInternalLoopbackEnableGet)
{
    /*
    ITERATE_DEVICES_PHY_PORTS (DxCh)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
    */

    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null enablePtr.
                Expected: GT_OK.
            */
            st = cpssDxChPortInternalLoopbackEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with enablePtr [NULL].
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortInternalLoopbackEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortInternalLoopbackEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortInternalLoopbackEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortInternalLoopbackEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortInternalLoopbackEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortInternalLoopbackEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortInbandAutoNegEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortInbandAutoNegEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK for non-10Gbps ports and GT_NOT_SUPPORTED for 10Gbps.
    1. 2. Call for CPU port [CPSS_CPU_PORT_NUM_CNS] with enable [GT_TRUE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS              st        = GT_OK;

    GT_U8                  dev;
    GT_U8                  port      = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL                enable    = GT_FALSE;
    GT_BOOL                enableGet = GT_TRUE;
    PRV_CPSS_PORT_TYPE_ENT portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);

            /* 1.1.1. Call with enable [GT_FALSE and GT_TRUE].
               Expected: GT_OK for non-10Gbps ports and GT_NOT_SUPPORTED for 10Gbps.
            */

            /* enable = GT_FALSE */
            enable = GT_FALSE;

            st = cpssDxChPortInbandAutoNegEnableSet(dev, port, enable);

            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                st = cpssDxChPortInbandAutoNegEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                        "cpssDxChPortInternalLoopbackEnableGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                        "get another enable value than was set: %d, %d", dev, port);
            }

            /* enable = GT_TRUE */
            enable = GT_TRUE;

            st = cpssDxChPortInbandAutoNegEnableSet(dev, port, enable);

            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                st = cpssDxChPortInbandAutoNegEnableGet(dev, port, &enableGet);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                        "cpssDxChPortInternalLoopbackEnableGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                        "get another enable value than was set: %d, %d", dev, port);
            }
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* state = GT_TRUE */
            st = cpssDxChPortInbandAutoNegEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* enable == GT_TRUE */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortInbandAutoNegEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        /* enable == GT_TRUE */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortInbandAutoNegEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    enable = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortInbandAutoNegEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, state == GT_TRUE */

    st = cpssDxChPortInbandAutoNegEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortAttributesOnPortGet
(
    IN    GT_U8                     devNum,
    IN    GT_U8                     portNum,
    OUT   CPSS_PORT_ATTRIBUTES_STC  *portAttributSetArrayPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortAttributesOnPortGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxChx)
    1.1.1. Call with non-null portAttributSetArrayPtr.
    Expected: GT_OK.
    1.1.2. Call with portAttributSetArrayPtr [NULL].
    Expected: GT_BAD_PTR.
*/

    GT_STATUS                st  = GT_OK;

    GT_U8                    dev;
    GT_U8                    port = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_PORT_ATTRIBUTES_STC attributes;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode = CPSS_PORT_INTERFACE_MODE_SGMII_E;
    CPSS_PP_FAMILY_TYPE_ENT     devFamily;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            if((CPSS_PP_FAMILY_DXCH_XCAT_E == devFamily) || 
                (CPSS_PP_FAMILY_DXCH_XCAT2_E == devFamily))
            {/* cpss appDemo doesn't initialize stacking ports of xcat/2 and
              * attribitesGet returns NOT_INITIALIZED, so let initialize the ports here
              * to avoid failures
              */
                if(IS_FLEX_PORT(dev,port))
                {
                    st = cpssDxChPortInterfaceModeSet(dev,port,CPSS_PORT_INTERFACE_MODE_SGMII_E);
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                        "cpssDxChPortInterfaceModeSet %d, %d, CPSS_PORT_INTERFACE_MODE_SGMII_E", dev, port);

                    st = cpssDxChPortSpeedSet(dev, port, CPSS_PORT_SPEED_1000_E);
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                        "cpssDxChPortSpeedSet %d, %d, CPSS_PORT_SPEED_1000_E", dev, port);

                    st = cpssDxChPortSerdesPowerStatusSet(dev, port, CPSS_PORT_DIRECTION_BOTH_E, 0x1, GT_TRUE);
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                        "cpssDxChPortSerdesPowerStatusSet %d, %d, CPSS_PORT_DIRECTION_BOTH_E, 0x1, GT_TRUE", 
                        dev, port);
                }
            }

            /*
               1.1.1. Call with non-null portAttributSetArrayPtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortInterfaceModeGet(dev,port,&ifMode);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
            st = cpssDxChPortAttributesOnPortGet(dev, port, &attributes);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
               1.1.2. Call with portAttributSetArrayPtr [NULL].
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortAttributesOnPortGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortAttributesOnPortGet(dev, port, &attributes);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortAttributesOnPortGet(dev, port, &attributes);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.3. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortAttributesOnPortGet(dev, port, &attributes);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortAttributesOnPortGet(dev, port, &attributes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortAttributesOnPortGet(dev, port, &attributes);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPreambleLengthSet
(
    IN GT_U8                    devNum,
    IN GT_U8                    portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPreambleLengthSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxChx)
    1.1.1. Call with direction [CPSS_PORT_DIRECTION_TX_E] and length [4, 8]
    Expected: GT_OK.
    1.1.2. Call with direction [CPSS_PORT_DIRECTION_RX_E /
                                CPSS_PORT_DIRECTION_BOTH_E] and length [4]
    Expected: GT_OK for 10Gbps ports and GT_NOT_SUPPORTED  for others.
    1.1.3. Call cpssDxChPortPreambleLengthGet with the same parameters
    as in 1.1.1 and 1.1.2.
    Expected: GT_OK and the same value.
    1.1.4. Call with direction [4] and length [4, 8]
    Expected: GT_BAD_PARAM.
    1.1.5. Call with direction [CPSS_PORT_DIRECTION_TX_E] and length [1, 2]
    Expected: NON GT_OK.
*/

    GT_STATUS               st       = GT_OK;

    GT_U8                   dev;
    GT_U8                   port      = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_PORT_DIRECTION_ENT direction = CPSS_PORT_DIRECTION_RX_E;
    GT_U32                  length    = 0;
    GT_U32                  lengthGet = 1;
    PRV_CPSS_PORT_TYPE_ENT  portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with direction [CPSS_PORT_DIRECTION_TX_E], length [4, 8]
               Expected: GT_OK.
            */

            /* Call with length = 4 */
            length = 4;
            direction = CPSS_PORT_DIRECTION_TX_E;

            st = cpssDxChPortPreambleLengthSet(dev, port, direction, length);

            if(PRV_CPSS_PORT_GE_E == portType || PRV_CPSS_PORT_FE_E == portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);

                /*
                    1.1.3. Call cpssDxChPortPreambleLengthGet with the same parameters
                    as in 1.1.1 and 1.1.2.
                    Expected: GT_OK and the same value.
                */
                st = cpssDxChPortPreambleLengthGet(dev, port, direction, &lengthGet);

                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);

                UTF_VERIFY_EQUAL2_STRING_MAC(length, lengthGet,
                                 "cpssDxChPortPreambleLengthGet: %d, %d", dev, port);
            }

            /* Call with length = 8 */
            length = 8;

            st = cpssDxChPortPreambleLengthSet(dev, port, direction, length);

            if(PRV_CPSS_PORT_GE_E == portType || PRV_CPSS_PORT_FE_E == portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);

                /*
                    1.1.3. Call cpssDxChPortPreambleLengthGet with the same parameters
                    as in 1.1.1 and 1.1.2.
                    Expected: GT_OK and the same value.
                */
                st = cpssDxChPortPreambleLengthGet(dev, port, direction, &lengthGet);

                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);

                UTF_VERIFY_EQUAL2_STRING_MAC(length, lengthGet,
                                 "cpssDxChPortPreambleLengthGet: %d, %d", dev, port);
            }

            /*
                1.1.2. Call with direction [CPSS_PORT_DIRECTION_RX_E /
                                           CPSS_PORT_DIRECTION_BOTH_E], length [4]
               Expected: GT_OK for 10Gbps ports and GT_NOT_SUPPORTED  for others.
            */

            /* Call with direction = CPSS_PORT_DIRECTION_RX_E */
            direction = CPSS_PORT_DIRECTION_RX_E;
            length = 4;

            st = cpssDxChPortPreambleLengthSet(dev, port, direction, length);

            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);

                /*
                    1.1.3. Call cpssDxChPortPreambleLengthGet with the same parameters
                    as in 1.1.1 and 1.1.2.
                    Expected: GT_OK and the same value.
                */
                st = cpssDxChPortPreambleLengthGet(dev, port, direction, &lengthGet);

                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);

                UTF_VERIFY_EQUAL2_STRING_MAC(length, lengthGet,
                                 "cpssDxChPortPreambleLengthGet: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, direction, length);
            }

            /* Call with direction = CPSS_PORT_DIRECTION_BOTH_E */
            direction = CPSS_PORT_DIRECTION_BOTH_E;

            st = cpssDxChPortPreambleLengthSet(dev, port, direction, length);

            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);
                /*we don't call get for direction = CPSS_PORT_DIRECTION_BOTH_E */
            }
            else
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, direction, length);
            }

            /*
                1.1.3. Call with wrong enum values direction , length [4]
               Expected: GT_BAD_PARAM.
            */
            /* length == 4; */
            UTF_ENUMS_CHECK_MAC(cpssDxChPortPreambleLengthSet
                                (dev, port, direction, length),
                                direction);

            /*
               1.1.4. Call with direction [CPSS_PORT_DIRECTION_TX_E], length [1, 2]
               Expected: NON GT_OK.
            */

            /* Call with length = 1 */
            length = 1;
            direction = CPSS_PORT_DIRECTION_TX_E;

            st = cpssDxChPortPreambleLengthSet(dev, port, direction, length);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);

            /* Call with length = 2 */
            length = 2;
            direction = CPSS_PORT_DIRECTION_TX_E;

            st = cpssDxChPortPreambleLengthSet(dev, port, direction, length);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        length = 4;
        direction = CPSS_PORT_DIRECTION_TX_E;

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            /* direction == CPSS_PORT_DIRECTION_TX_E */

            st = cpssDxChPortPreambleLengthSet(dev, port, direction, length);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortPreambleLengthSet(dev, port, direction, length);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortPreambleLengthSet(dev, port, direction, length);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    length = 4;
    direction = CPSS_PORT_DIRECTION_TX_E;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPreambleLengthSet(dev, port, direction, length);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, direction == CPSS_PORT_DIRECTION_TX_E, length == 4 */

    st = cpssDxChPortPreambleLengthSet(dev, port, direction, length);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortMacSaBaseSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *macPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortMacSaBaseSet)
{
/*
    ITERATE_DEVICES (DxCh)
    1.1. Call with mac {arEther[10, 20, 30, 40, 50, 60]}.
    Expected: GT_OK.
    1.2. Call cpssDxChPortMacSaBaseGet with not-NULL macPtr.
    Expected: GT_OK and the same mac as was set.
    1.3. Call with mac[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_ETHERADDR    mac;
    GT_ETHERADDR    macGet;
    GT_BOOL         isEqual = GT_FALSE;


    cpssOsBzero((GT_VOID*) &mac, sizeof(mac));
    cpssOsBzero((GT_VOID*) &macGet, sizeof(macGet));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mac {arEther[10, 20, 30, 40, 50, 0]}.
            Expected: GT_OK.
        */
        mac.arEther[0] = 10;
        mac.arEther[1] = 20;
        mac.arEther[2] = 30;
        mac.arEther[3] = 40;
        mac.arEther[4] = 50;
        mac.arEther[5] = 0;

        st = cpssDxChPortMacSaBaseSet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChPortMacSaBaseGet with not-NULL macPtr.
            Expected: GT_OK and the same mac as was set.
        */
        st = cpssDxChPortMacSaBaseGet(dev, &macGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChPortMacSaBaseGet: %d", dev);

        /* verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mac, (GT_VOID*) &macGet, sizeof(mac))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another mac then was set: %d", dev);

        /*
            1.3. Call with mac[NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortMacSaBaseSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mac = NULL" ,dev);
    }

    mac.arEther[0] = 10;
    mac.arEther[1] = 20;
    mac.arEther[2] = 30;
    mac.arEther[3] = 40;
    mac.arEther[4] = 50;
    mac.arEther[5] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortMacSaBaseSet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortMacSaBaseSet(dev, &mac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortMacSaBaseGet
(
    IN  GT_U8           devNum,
    OUT GT_ETHERADDR    *macPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortMacSaBaseGet)
{
/*
    ITERATE_DEVICES (DxCh)
    1.1. Call with not-NULL mac
    Expected: GT_OK.
    1.2. Call with mac[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_ETHERADDR    mac;


    cpssOsBzero((GT_VOID*) &mac, sizeof(mac));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not-NULL mac
            Expected: GT_OK.
        */
        st = cpssDxChPortMacSaBaseGet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with mac[NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortMacSaBaseGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mac = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortMacSaBaseGet(dev, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortMacSaBaseGet(dev, &mac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortSerdesComboModeSet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    IN  CPSS_DXCH_PORT_SERDES_MODE_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortSerdesComboModeSet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxCh3, 20..23)
    1.1.1. Call with mode[CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E /
                          CPSS_DXCH_PORT_SERDES_MODE_COMBO_E].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortSerdesComboModeGet with not-NULL modePtr.
    Expected: GT_OK and the same mode as was set.
    1.1.3. Call with mode[wrong enum values].
    Expected: GT_BAD_PARAM.
*/

    GT_STATUS            st   = GT_OK;
    GT_U8                dev;
    GT_U8                port = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_DXCH_PORT_SERDES_MODE_ENT  mode    = CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E;
    CPSS_DXCH_PORT_SERDES_MODE_ENT  modeGet = CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            if((port>19) && (port<24))
            {
                /*
                    1.1.1. Call with mode[CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E /
                                          CPSS_DXCH_PORT_SERDES_MODE_COMBO_E].
                    Expected: GT_OK.
                */
                /* iterate with mode = CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E */
                mode = CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E;

                st = cpssDxChPortSerdesComboModeSet(dev, port, mode);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);

                /*
                    1.1.2. Call cpssDxChPortSerdesComboModeGet with not-NULL modePtr.
                    Expected: GT_OK and the same mode as was set.
                */
                st = cpssDxChPortSerdesComboModeGet(dev, port, &modeGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortSerdesComboModeGet: %d, %d" ,dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet, "got another mode as was set: %d, %d" ,dev, port);

                /* iterate with mode = CPSS_DXCH_PORT_SERDES_MODE_COMBO_E */
                mode = CPSS_DXCH_PORT_SERDES_MODE_COMBO_E;

                st = cpssDxChPortSerdesComboModeSet(dev, port, mode);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);

                /*
                    1.1.2. Call cpssDxChPortSerdesComboModeGet with not-NULL modePtr.
                    Expected: GT_OK and the same mode as was set.
                */
                st = cpssDxChPortSerdesComboModeGet(dev, port, &modeGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortSerdesComboModeGet: %d, %d" ,dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet, "got another mode as was set: %d, %d" ,dev, port);

                /*
                    1.1.3. Call with mode[wrong enum values].
                    Expected: GT_BAD_PARAM.
                */
                UTF_ENUMS_CHECK_MAC(cpssDxChPortSerdesComboModeSet
                                    (dev, port, mode),
                                    mode);
            }
            else
            {
                /* wrong port. Expected: GT_BAD_PARAM */
                mode = CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E;

                st = cpssDxChPortSerdesComboModeSet(dev, port, mode);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mode);
            }
        }

        mode = CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E;

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortSerdesComboModeSet(dev, port, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortSerdesComboModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortSerdesComboModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mode);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    mode = CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortSerdesComboModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, dMode == CPSS_PORT_FULL_DUPLEX_E */

    st = cpssDxChPortSerdesComboModeSet(dev, port, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortSerdesComboModeGet
(
    IN  GT_U8                             devNum,
    IN  GT_U8                             portNum,
    OUT CPSS_DXCH_PORT_SERDES_MODE_ENT    *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortSerdesComboModeGet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxCh3, 20..23)
    1.1.1. Call with not-NULL modePTr.
    Expected: GT_OK.
    1.1.2. Call with modePtr[NULL].
    Expected: GT_BAD_PTR.
*/

    GT_STATUS            st   = GT_OK;
    GT_U8                dev;
    GT_U8                port = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_DXCH_PORT_SERDES_MODE_ENT  mode    = CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            if((port>19) && (port<24))
            {
                /*
                    1.1.1. Call with not-NULL modePTr.
                    Expected: GT_OK.
                */
                st = cpssDxChPortSerdesComboModeGet(dev, port, &mode);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call with modePtr[NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssDxChPortSerdesComboModeGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, mode = NULL", dev, port);
            }
            else
            {
                /* wrong port. Expected: GT_BAD_PARAM */
                st = cpssDxChPortSerdesComboModeGet(dev, port, &mode);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortSerdesComboModeGet(dev, port, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortSerdesComboModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortSerdesComboModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mode);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortSerdesComboModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, dMode == CPSS_PORT_FULL_DUPLEX_E */

    st = cpssDxChPortSerdesComboModeGet(dev, port, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortSerdesConfigSet
(
    IN GT_U8  devNum,
    IN GT_U8  portNum,
    IN CPSS_DXCH_PORT_SERDES_CONFIG_STC  *serdesCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortSerdesConfigSet)
{
/*
    ITERATE_DEVICE_PHY_PORT (DxCh3 and above)
    1.1.1. Call with serdesCfg { txAmp                 [0 / 10   / 31],
                                 txEmphEn              [GT_FALSE / GT_TRUE],
                                 txEmphAmp             [0 / 10   / 15],
                                 txAmpAdj              [0 / 1000 / 100000],
                                 txEmphLevelAdjEnable  [GT_FALSE / GT_TRUE],
                                 ffeSignalSwingControl [0 / 1000 / 100000],
                                 ffeResistorSelect     [0 / 1000 / 100000],
                                 ffeCapacitorSelect    [0 / 1000 / 100000]}.
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortSerdesConfigGet with not NULL serdesCfgPtr.
    Expected: GT_OK and the same serdesCfgPtr as was set.
    1.1.3. Call with serdesCfg->txAmp [32] out of range, and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.4. Call with serdesCfg->txEmphAmp [16] out of range, and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.5. Call with serdesCfg [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    CPSS_PP_FAMILY_TYPE_ENT devFamily;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;

    CPSS_DXCH_PORT_SERDES_CONFIG_STC  serdesCfg;
    CPSS_DXCH_PORT_SERDES_CONFIG_STC  serdesCfgGet;

    cpssOsBzero((GT_VOID*) &serdesCfg, sizeof(serdesCfg));
    cpssOsBzero((GT_VOID*) &serdesCfgGet, sizeof(serdesCfgGet));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = cpssDxChPortInterfaceModeGet(dev, port, &ifMode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                    "cpssDxChPortInterfaceModeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with serdesCfg { txAmp                 [0 / 10   / 31],
                                             txEmphEn              [GT_FALSE / GT_TRUE],
                                             txEmphAmp             [0 / 10   / 15],
                                             txAmpAdj              [0 / 3 / 3],
                                             txEmphLevelAdjEnable  [GT_FALSE / GT_TRUE],
                                             ffeSignalSwingControl [0 / 3 / 3],
                                             ffeResistorSelect     [0 / 3 / 7],
                                             ffeCapacitorSelect    [0 / 3 / 15]}.
                Expected: GT_OK.
            */
            /* call with first group of the values */
            serdesCfg.txAmp = 0;
            serdesCfg.txEmphEn = GT_FALSE;
            serdesCfg.txEmphAmp = 0;
            serdesCfg.txAmpAdj = 0;
            serdesCfg.ffeSignalSwingControl = 0;
            serdesCfg.ffeResistorSelect = 0;
            serdesCfg.ffeCapacitorSelect = 0;

            st = cpssDxChPortSerdesConfigSet(dev, port, &serdesCfg);
            if( (IS_PORT_FE_E(dev,port)) || (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E) )
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call cpssDxChPortSerdesConfigGet with not NULL serdesCfgPtr.
                    Expected: GT_OK and the same serdesCfgPtr as was set.
                */
                st = cpssDxChPortSerdesConfigGet(dev, port, &serdesCfgGet);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                    "cpssDxChPortSerdesConfigGet: %d, %d", dev, port);

                if (GT_OK == st)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.txAmp, serdesCfgGet.txAmp,
                        "get another serdesCfg.txAmp than was set: %d, %d", dev, port);
                    UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.txEmphAmp,
                                                 serdesCfgGet.txEmphAmp,
                        "get another serdesCfg.txEmphAmp than was set: %d, %d", dev, port);

                    /* check fields relevant only for ch3 */
                    if (CPSS_PP_FAMILY_CHEETAH3_E == devFamily)
                    {
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.txEmphEn, serdesCfgGet.txEmphEn,
                             "get another serdesCfg.txEmphEn than was set: %d, %d", dev, port);
                    }
                    /* check fields relevant only for xCat and above */
                    else
                    {
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.txAmpAdj, serdesCfgGet.txAmpAdj,
                            "get another serdesCfg.txAmpAdj than was set: %d, %d", dev, port);
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.ffeSignalSwingControl,
                                                     serdesCfgGet.ffeSignalSwingControl,
                            "get another serdesCfg.ffeSignalSwingControl than was set: %d, %d",
                                                     dev, port);
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.ffeResistorSelect,
                                                     serdesCfgGet.ffeResistorSelect,
                            "get another serdesCfg.ffeResistorSelect than was set: %d, %d",
                                                     dev, port);
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.ffeCapacitorSelect,
                                                     serdesCfgGet.ffeCapacitorSelect,
                            "get another serdesCfg.ffeCapacitorSelect than was set: %d, %d",
                                                     dev, port);
                    }
                }
            }

            /* call with second group of the values */
            serdesCfg.txAmp = 10;
            serdesCfg.txEmphEn = GT_TRUE;
            serdesCfg.txEmphAmp = 10;
            serdesCfg.txAmpAdj = 3;
            serdesCfg.ffeSignalSwingControl = 3;
            serdesCfg.ffeResistorSelect = 3;
            serdesCfg.ffeCapacitorSelect = 3;

            st = cpssDxChPortSerdesConfigSet(dev, port, &serdesCfg);

            if( (IS_PORT_FE_E(dev,port)) || (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E) )
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call cpssDxChPortSerdesConfigGet with not NULL serdesCfgPtr.
                    Expected: GT_OK and the same serdesCfgPtr as was set.
                */

                st = cpssDxChPortSerdesConfigGet(dev, port, &serdesCfgGet);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                    "cpssDxChPortSerdesConfigGet: %d, %d", dev, port);

                if (GT_OK == st)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.txAmp, serdesCfgGet.txAmp,
                        "get another serdesCfg.txAmp than was set: %d, %d", dev, port);
                    UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.txEmphAmp,
                                                 serdesCfgGet.txEmphAmp,
                        "get another serdesCfg.txEmphAmp than was set: %d, %d", dev, port);

                    /* check fields relevant only for ch3 */
                    if (CPSS_PP_FAMILY_CHEETAH3_E == devFamily)
                    {
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.txEmphEn, serdesCfgGet.txEmphEn,
                             "get another serdesCfg.txEmphEn than was set: %d, %d", dev, port);
                    }
                    /* check fields relevant only for xCat and above */
                    else
                    {
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.txAmpAdj, serdesCfgGet.txAmpAdj,
                            "get another serdesCfg.txAmpAdj than was set: %d, %d", dev, port);
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.ffeSignalSwingControl,
                                                     serdesCfgGet.ffeSignalSwingControl,
                            "get another serdesCfg.ffeSignalSwingControl than was set: %d, %d",
                                                     dev, port);
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.ffeResistorSelect,
                                                     serdesCfgGet.ffeResistorSelect,
                            "get another serdesCfg.ffeResistorSelect than was set: %d, %d",
                                                     dev, port);
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.ffeCapacitorSelect,
                                                     serdesCfgGet.ffeCapacitorSelect,
                            "get another serdesCfg.ffeCapacitorSelect than was set: %d, %d",
                                                     dev, port);
                    }
                }
            }

            /* call with third group of the values */
            serdesCfg.txAmp = 31;
            serdesCfg.txEmphEn = GT_TRUE;
            serdesCfg.txEmphAmp = 15;
            serdesCfg.txAmpAdj = 3;
            serdesCfg.ffeSignalSwingControl = 3;
            serdesCfg.ffeResistorSelect = 7;
            serdesCfg.ffeCapacitorSelect = 15;

            st = cpssDxChPortSerdesConfigSet(dev, port, &serdesCfg);
            if( (IS_PORT_FE_E(dev,port)) || (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E) )
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call cpssDxChPortSerdesConfigGet with not NULL serdesCfgPtr.
                    Expected: GT_OK and the same serdesCfgPtr as was set.
                */

                st = cpssDxChPortSerdesConfigGet(dev, port, &serdesCfgGet);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                    "cpssDxChPortSerdesConfigGet: %d, %d", dev, port);

                if (GT_OK == st)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.txAmp, serdesCfgGet.txAmp,
                        "get another serdesCfg.txAmp than was set: %d, %d", dev, port);
                    UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.txEmphAmp,
                                                 serdesCfgGet.txEmphAmp,
                        "get another serdesCfg.txEmphAmp than was set: %d, %d", dev, port);

                    /* check fields relevant only for ch3 */
                    if (CPSS_PP_FAMILY_CHEETAH3_E == devFamily)
                    {
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.txEmphEn, serdesCfgGet.txEmphEn,
                             "get another serdesCfg.txEmphEn than was set: %d, %d", dev, port);
                    }
                    /* check fields relevant only for xCat and above */
                    else
                    {
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.txAmpAdj, serdesCfgGet.txAmpAdj,
                            "get another serdesCfg.txAmpAdj than was set: %d, %d", dev, port);
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.ffeSignalSwingControl,
                                                     serdesCfgGet.ffeSignalSwingControl,
                            "get another serdesCfg.ffeSignalSwingControl than was set: %d, %d",
                                                     dev, port);
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.ffeResistorSelect,
                                                     serdesCfgGet.ffeResistorSelect,
                            "get another serdesCfg.ffeResistorSelect than was set: %d, %d",
                                                     dev, port);
                        UTF_VERIFY_EQUAL2_STRING_MAC(serdesCfg.ffeCapacitorSelect,
                                                     serdesCfgGet.ffeCapacitorSelect,
                            "get another serdesCfg.ffeCapacitorSelect than was set: %d, %d",
                                                     dev, port);
                    }
                }
            }

            /*
                1.1.3. Call with serdesCfg->txAmp [32] out of range, and other params from 1.1.1.
                Expected: NOT GT_OK.
            */
            serdesCfg.txAmp = 32;

            st = cpssDxChPortSerdesConfigSet(dev, port, &serdesCfg);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                        "%d, %d, serdesCfg.txAmp = %d", dev, port, serdesCfg.txAmp);

            serdesCfg.txAmp = 0;

            /*
                1.1.4. Call with serdesCfg->txEmphAmp [16] out of range, and other params from 1.1.1.
                Expected: NOT GT_OK.
            */
            serdesCfg.txEmphAmp = 16;

            st = cpssDxChPortSerdesConfigSet(dev, port, &serdesCfg);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, serdesCfg.txEmphAmp = %d", dev, port, serdesCfg.txEmphAmp);

            serdesCfg.txEmphAmp = 0;

            /*
                1.1.5. Call with serdesCfg [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortSerdesConfigSet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                "%d, %d, serdesCfgPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortSerdesConfigSet(dev, port, &serdesCfg);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortSerdesConfigSet(dev, port, &serdesCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortSerdesConfigSet(dev, port, &serdesCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortSerdesConfigSet(dev, port, &serdesCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortSerdesConfigSet(dev, port, &serdesCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortSerdesConfigGet
(
    IN GT_U8  devNum,
    IN GT_U8  portNum,
    OUT CPSS_DXCH_PORT_SERDES_CONFIG_STC  *serdesCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortSerdesConfigGet)
{
/*
    ITERATE_DEVICE_PHY_PORT (DxCh3 and above)
    1.1.1. Call with not NULL serdesCfgPtr.
    Expected: GT_OK.
    1.1.2. Call with serdesCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;

    CPSS_DXCH_PORT_SERDES_CONFIG_STC  serdesCfg;

    cpssOsBzero((GT_VOID*) &serdesCfg, sizeof(serdesCfg));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = cpssDxChPortInterfaceModeGet(dev, port, &ifMode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortInterfaceModeGet: %d, %d",
                                         dev, port);
            /*
                1.1.1. Call with not NULL serdesCfgPtr.
                Expected: GT_OK.
            */
            st = cpssDxChPortSerdesConfigGet(dev, port, &serdesCfg);
            if(IS_PORT_FE_E(dev,port))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else if(ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }

            /*
                1.1.2. Call with serdesCfgPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortSerdesConfigGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, serdesCfgPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortSerdesConfigGet(dev, port, &serdesCfg);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortSerdesConfigGet(dev, port, &serdesCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortSerdesConfigGet(dev, port, &serdesCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortSerdesConfigGet(dev, port, &serdesCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortSerdesConfigGet(dev, port, &serdesCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPaddingEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPaddingEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORT (DxCh)
    1.1.1. Call with enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortPaddingEnableGet with not-NULL enablePtr.
    Expected: GT_OK and the same enable as was set.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable[GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* iterate with enable - GT_FALSE */
            enable = GT_FALSE;

            st = cpssDxChPortPaddingEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssDxChPortPaddingEnableSet with not-NULL enablePtr.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPortPaddingEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortPaddingEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable then was set: %d, %d", dev, port);

            /* iterate with enable - GT_TRUE */
            enable = GT_TRUE;

            st = cpssDxChPortPaddingEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssDxChPortPaddingEnableSet with not-NULL enablePtr.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPortPaddingEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortPaddingEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable then was set: %d, %d", dev, port);
        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortPaddingEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortPaddingEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortPaddingEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_FALSE;
    port   = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPaddingEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPaddingEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPaddingEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPaddingEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORT (DxCh)
    1.1.1. Call with not-NULL enable.
    Expected: GT_OK.
    1.1.2. Call enablePtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not-NULL enable.
                Expected: GT_OK.
            */
            st = cpssDxChPortPaddingEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call enablePtr[NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortPaddingEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortPaddingEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortPaddingEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortPaddingEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPaddingEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPaddingEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortExcessiveCollisionDropEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortExcessiveCollisionDropEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORT (DxCh)
    1.1.1. Call with enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortExcessiveCollisionDropEnableGet with not-NULL enablePtr.
    Expected: GT_OK and the same enable as was set.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable[GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* iterate with enable - GT_FALSE */
            enable = GT_FALSE;

            st = cpssDxChPortExcessiveCollisionDropEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssDxChPortExcessiveCollisionDropEnableGet with not-NULL enablePtr.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPortExcessiveCollisionDropEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortExcessiveCollisionDropEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable then was set: %d, %d", dev, port);

            /* iterate with enable - GT_TRUE */
            enable = GT_TRUE;

            st = cpssDxChPortExcessiveCollisionDropEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssDxChPortExcessiveCollisionDropEnableGet with not-NULL enablePtr.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPortExcessiveCollisionDropEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortExcessiveCollisionDropEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable then was set: %d, %d", dev, port);
        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortExcessiveCollisionDropEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortExcessiveCollisionDropEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortExcessiveCollisionDropEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_FALSE;
    port   = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortExcessiveCollisionDropEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortExcessiveCollisionDropEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortExcessiveCollisionDropEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortExcessiveCollisionDropEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORT (DxCh)
    1.1.1. Call with not-NULL enable.
    Expected: GT_OK.
    1.1.2. Call enablePtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not-NULL enable.
                Expected: GT_OK.
            */
            st = cpssDxChPortExcessiveCollisionDropEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call enablePtr[NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortExcessiveCollisionDropEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortExcessiveCollisionDropEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortExcessiveCollisionDropEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortExcessiveCollisionDropEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortExcessiveCollisionDropEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortExcessiveCollisionDropEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortXgLanesSwapEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortXgLanesSwapEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORT (DxCh)
    1.1.1. Call with enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortXgLanesSwapEnableGet with not-NULL enablePtr.
    Expected: GT_OK and the same enable as was set.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* Supported for XAUI or HyperG.Stack ports only */
            if( IS_PORT_XG_E(dev, port) )
            {
                /*
                    1.1.1. Call with enable[GT_FALSE / GT_TRUE].
                    Expected: GT_OK.
                */
                /* iterate with enable - GT_FALSE */
                enable = GT_FALSE;

                st = cpssDxChPortXgLanesSwapEnableSet(dev, port, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                /*
                    1.1.2. Call cpssDxChPortPaddingEnableSet with not-NULL enablePtr.
                    Expected: GT_OK and the same enable as was set.
                */
                st = cpssDxChPortXgLanesSwapEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortXgLanesSwapEnableGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable then was set: %d, %d", dev, port);

                /* iterate with enable - GT_TRUE */
                enable = GT_TRUE;

                st = cpssDxChPortXgLanesSwapEnableSet(dev, port, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                /*
                    1.1.2. Call cpssDxChPortXgLanesSwapEnableGet with not-NULL enablePtr.
                    Expected: GT_OK and the same enable as was set.
                */
                st = cpssDxChPortXgLanesSwapEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortXgLanesSwapEnableGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable then was set: %d, %d", dev, port);
            }
            else
            {
                /* not supported NOT XG_E ports*/
                enable = GT_FALSE;

                st = cpssDxChPortXgLanesSwapEnableSet(dev, port, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, enable);
            }
        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortXgLanesSwapEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortXgLanesSwapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortXgLanesSwapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_FALSE;
    port   = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortXgLanesSwapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortXgLanesSwapEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortXgLanesSwapEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortXgLanesSwapEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORT (DxCh)
    1.1.1. Call with not-NULL enable.
    Expected: GT_OK.
    1.1.2. Call enablePtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* Supported for XAUI or HyperG.Stack ports only */
            if( IS_PORT_XG_E(dev, port) )
            {
                /*
                    1.1.1. Call with not-NULL enable.
                    Expected: GT_OK.
                */
                st = cpssDxChPortXgLanesSwapEnableGet(dev, port, &enable);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call enablePtr[NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssDxChPortXgLanesSwapEnableGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
            }
            else
            {
                /* not supported NOT XG_E ports*/
                enable = GT_FALSE;

                st = cpssDxChPortXgLanesSwapEnableSet(dev, port, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, enable);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortXgLanesSwapEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortXgLanesSwapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortXgLanesSwapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortXgLanesSwapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortXgLanesSwapEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortInBandAutoNegBypassEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortInBandAutoNegBypassEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxCh)
    1.1.1. Call with enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortInBandAutoNegBypassEnableGet with not-NULL enablePtr.
    Expected: GT_OK and the same enable as was set.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            if (!IS_PORT_XG_E(dev, port))
            {
                /* Set Inband Auto-Negotiation */
                st = cpssDxChPortInbandAutoNegEnableSet(dev, port, GT_TRUE);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssDxChPortInbandAutoNegEnableSet: %d, %d, GT_TRUE", dev, port);

                /*
                    1.1.1. Call with enable[GT_FALSE / GT_TRUE].
                    Expected: GT_OK.
                */
                /* iterate with enable - GT_FALSE */
                enable = GT_FALSE;

                st = cpssDxChPortInBandAutoNegBypassEnableSet(dev, port, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                /*
                    1.1.2. Call cpssDxChPortInBandAutoNegBypassEnableGet with not-NULL enablePtr.
                    Expected: GT_OK and the same enable as was set.
                */
                st = cpssDxChPortInBandAutoNegBypassEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortInBandAutoNegBypassEnableGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable then was set: %d, %d", dev, port);

                /* iterate with enable - GT_TRUE */
                enable = GT_TRUE;

                st = cpssDxChPortInBandAutoNegBypassEnableSet(dev, port, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

                /*
                    1.1.2. Call cpssDxChPortInBandAutoNegBypassEnableGet with not-NULL enablePtr.
                    Expected: GT_OK and the same enable as was set.
                */
                st = cpssDxChPortInBandAutoNegBypassEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortInBandAutoNegBypassEnableGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable then was set: %d, %d", dev, port);
            }
        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortInBandAutoNegBypassEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortInBandAutoNegBypassEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortInBandAutoNegBypassEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_FALSE;
    port   = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortInBandAutoNegBypassEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortInBandAutoNegBypassEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortInBandAutoNegBypassEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortInBandAutoNegBypassEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxCh)
    1.1.1. Call with not-NULL enable.
    Expected: GT_OK.
    1.1.2. Call enablePtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            if (!IS_PORT_XG_E(dev, port))
            {
                /* Set Inband Auto-Negotiation */
                st = cpssDxChPortInbandAutoNegEnableSet(dev, port, GT_TRUE);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssDxChPortInbandAutoNegEnableSet: %d, %d, GT_TRUE", dev, port);
                /*
                    1.1.1. Call with not-NULL enable.
                    Expected: GT_OK.
                */
                st = cpssDxChPortInBandAutoNegBypassEnableGet(dev, port, &enable);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call enablePtr[NULL].
                    Expected: GT_BAD_PTR.
                */
                st = cpssDxChPortInBandAutoNegBypassEnableGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortInBandAutoNegBypassEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortInBandAutoNegBypassEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortInBandAutoNegBypassEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortInBandAutoNegBypassEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortInBandAutoNegBypassEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortMacErrorIndicationPortSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortMacErrorIndicationPortSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORT (DxCh)
    1.1.1. Call with port.
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortMacErrorIndicationPortGet with not-NULL portPtr.
    Expected: GT_OK and the same port as was set.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_U8       portGet = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_PP_FAMILY_TYPE_ENT                 devFamily;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfDeviceFamilyGet(dev, &devFamily);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

            /*
                1.1.1. Call with not null port.
                Expected: GT_OK.
            */
            st = cpssDxChPortMacErrorIndicationPortSet(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call cpssDxChPortInBandAutoNegBypassEnableGet
                       with not-NULL enablePtr.
                Expected: GT_OK and the same enable as was set.
            */
            st = cpssDxChPortMacErrorIndicationPortGet(dev, &portGet);

            /* get function works only for ch1/ch2*/
            if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev, port);
            }
            else /* CPSS_PP_FAMILY_CHEETAH_E  or CPSS_PP_FAMILY_CHEETAH2_E */
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                        "cpssDxChPortMacErrorIndicationPortGet: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(port, portGet,
                        "got another port then was set: %d, %d", dev);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortMacErrorIndicationPortSet(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortMacErrorIndicationPortSet(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortMacErrorIndicationPortSet(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortMacErrorIndicationPortSet(dev, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortMacErrorIndicationPortSet(dev, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortMacErrorIndicationPortGet
(
    IN  GT_U8      devNum,
    OUT GT_U8      *portNumPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortMacErrorIndicationPortGet)
{
/*
    ITERATE_DEVICES (DxCh)
    1.1.1. Call with not-NULL portPtr.
    Expected: GT_OK.
    1.1.2. Call portPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1.1. Call with not-NULL portPtr.
            Expected: GT_OK.
        */
        st = cpssDxChPortMacErrorIndicationPortGet(dev, &port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.1.2. Call with null portPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortMacErrorIndicationPortGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, port = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortMacErrorIndicationPortGet(dev, &port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortMacErrorIndicationPortGet(dev, &port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortMacErrorIndicationGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT   *typePtr,
    OUT GT_ETHERADDR                        *macPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortMacErrorIndicationGet)
{
/*
    ITERATE_DEVICES (DxCh)
    1.1. Call with not-NULL typePtr and not-NULL macPtr.
    Expected: GT_OK.
    1.2. Call typePtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call macPtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT   type = CPSS_DXCH_PORT_MAC_ERROR_JABBER_E;
    GT_ETHERADDR                        mac;

    cpssOsBzero((GT_VOID*) &mac, sizeof(mac));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not-NULL typePtr and not-NULL macPtr.
            Expected: GT_OK.
        */
        st = cpssDxChPortMacErrorIndicationGet(dev, &type, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call typePtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortMacErrorIndicationGet(dev, NULL, &mac);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, typePtr = NULL", dev);

        /*
            1.3. Call macPtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPortMacErrorIndicationGet(dev, &type, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortMacErrorIndicationGet(dev, &type, &mac);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortMacErrorIndicationGet(dev, &type, &mac);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortXgPscLanesSwapSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS],
    IN GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortXgPscLanesSwapSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh3 and above)
    1.1.1. Call with rxSerdesLaneArr [0,1,2,3 / 0,1,2,3],
                     txSerdesLaneArr [0,1,2,3 / 3,2,1,0].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortXgPscLanesSwapGet with not NULL pointers.
    Expected: GT_OK and the same params as was set.
    1.1.3. Call with out of range rxSerdesLaneArr [0] [4]
                     and other params from 1.1.
    Expected: NOT GT_OK.
    1.1.4. Call with out of range txSerdesLaneArr [0] [4]
                     and other params from 1.1.
    Expected: NOT GT_OK.
    1.1.5. Call with rxSerdesLaneArr [0, 1, 2, 2] (same SERDES lane)
                     and other params from 1.1.
    Expected: NOT GT_OK.
    1.1.6. Call with txSerdesLaneArr [0, 1, 2, 2] (same SERDES lane)
                     and other params from 1.1.
    Expected: NOT GT_OK.
    1.1.7. Call with rxSerdesLaneArr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.1.8. Call with txSerdesLaneArr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];
    GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];

    GT_U32 rxSerdesLaneArrGet[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];
    GT_U32 txSerdesLaneArrGet[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* Supported for XAUI or HyperG.Stack ports only! */
            if(PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType == PRV_CPSS_PORT_XG_E)
            {
                /*
                    1.1.1. Call with rxSerdesLaneArr [0,1,2,3 / 0,1,2,3],
                                     txSerdesLaneArr [0,1,2,3 / 3,2,1,0].
                    Expected: GT_OK.
                */
                /* iterate with 0 */
                rxSerdesLaneArr[0] = 0;
                rxSerdesLaneArr[1] = 1;
                rxSerdesLaneArr[2] = 2;
                rxSerdesLaneArr[3] = 3;

                txSerdesLaneArr[0] = 0;
                txSerdesLaneArr[1] = 1;
                txSerdesLaneArr[2] = 2;
                txSerdesLaneArr[3] = 3;

                st = cpssDxChPortXgPscLanesSwapSet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call cpssDxChPortXgPscLanesSwapGet with not NULL pointers.
                    Expected: GT_OK and the same params as was set.
                */
                st = cpssDxChPortXgPscLanesSwapGet(dev, port, rxSerdesLaneArrGet, txSerdesLaneArrGet);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /* verifuing values */
                UTF_VERIFY_EQUAL2_STRING_MAC(rxSerdesLaneArr[0], rxSerdesLaneArrGet[0],
                                             "got another rxSerdesLaneArr[0] than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(rxSerdesLaneArr[1], rxSerdesLaneArrGet[1],
                                             "got another rxSerdesLaneArr[1] than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(rxSerdesLaneArr[2], rxSerdesLaneArrGet[2],
                                             "got another rxSerdesLaneArr[2] than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(rxSerdesLaneArr[3], rxSerdesLaneArrGet[3],
                                             "got another rxSerdesLaneArr[3] than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(txSerdesLaneArr[0], txSerdesLaneArrGet[0],
                                             "got another txSerdesLaneArr[0] than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(txSerdesLaneArr[1], txSerdesLaneArrGet[1],
                                             "got another txSerdesLaneArr[1] than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(txSerdesLaneArr[2], txSerdesLaneArrGet[2],
                                             "got another txSerdesLaneArr[2] than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(txSerdesLaneArr[3], txSerdesLaneArrGet[3],
                                             "got another txSerdesLaneArr[3] than was set: %d, %d", dev, port);

                /* iterate with 3 */
                rxSerdesLaneArr[0] = 0;
                rxSerdesLaneArr[1] = 1;
                rxSerdesLaneArr[2] = 2;
                rxSerdesLaneArr[3] = 3;

                txSerdesLaneArr[0] = 3;
                txSerdesLaneArr[1] = 2;
                txSerdesLaneArr[2] = 1;
                txSerdesLaneArr[3] = 0;

                st = cpssDxChPortXgPscLanesSwapSet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call cpssDxChPortXgPscLanesSwapGet with not NULL pointers.
                    Expected: GT_OK and the same params as was set.
                */
                st = cpssDxChPortXgPscLanesSwapGet(dev, port, rxSerdesLaneArrGet, txSerdesLaneArrGet);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /* verifuing values */
                UTF_VERIFY_EQUAL2_STRING_MAC(rxSerdesLaneArr[0], rxSerdesLaneArrGet[0],
                                             "got another rxSerdesLaneArr[0] than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(rxSerdesLaneArr[1], rxSerdesLaneArrGet[1],
                                             "got another rxSerdesLaneArr[1] than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(rxSerdesLaneArr[2], rxSerdesLaneArrGet[2],
                                             "got another rxSerdesLaneArr[2] than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(rxSerdesLaneArr[3], rxSerdesLaneArrGet[3],
                                             "got another rxSerdesLaneArr[3] than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(txSerdesLaneArr[0], txSerdesLaneArrGet[0],
                                             "got another txSerdesLaneArr[0] than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(txSerdesLaneArr[1], txSerdesLaneArrGet[1],
                                             "got another txSerdesLaneArr[1] than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(txSerdesLaneArr[2], txSerdesLaneArrGet[2],
                                             "got another txSerdesLaneArr[2] than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(txSerdesLaneArr[3], txSerdesLaneArrGet[3],
                                             "got another txSerdesLaneArr[3] than was set: %d, %d", dev, port);

                /*
                    1.1.3. Call with out of range rxSerdesLaneArr [0] [4]
                                     and other params from 1.1.
                    Expected: NOT GT_OK.
                */
                rxSerdesLaneArr[0] = 4;

                st = cpssDxChPortXgPscLanesSwapSet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, rxSerdesLaneArr[0] = %d",
                                                 dev, port, rxSerdesLaneArr[0]);

                rxSerdesLaneArr[0] = 0;

                /*
                    1.1.4. Call with out of range txSerdesLaneArr [0] [4]
                                     and other params from 1.1.
                    Expected: NOT GT_OK.
                */
                txSerdesLaneArr[0] = 4;

                st = cpssDxChPortXgPscLanesSwapSet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, txSerdesLaneArr[0] = %d",
                                                 dev, port, txSerdesLaneArr[0]);

                txSerdesLaneArr[0] = 3;

                /*
                    1.1.5. Call with rxSerdesLaneArr [0, 1, 2, 2] (same SERDES lane)
                                     and other params from 1.1.
                    Expected: NOT GT_OK.
                */
                rxSerdesLaneArr[0] = 0;
                rxSerdesLaneArr[1] = 1;
                rxSerdesLaneArr[2] = 2;
                rxSerdesLaneArr[3] = 2;

                st = cpssDxChPortXgPscLanesSwapSet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, rxSerdesLaneArr[2] = %d, rxSerdesLaneArr[3] = %d",
                                                 dev, port, rxSerdesLaneArr[2], rxSerdesLaneArr[3]);

                rxSerdesLaneArr[0] = 0;
                rxSerdesLaneArr[1] = 1;
                rxSerdesLaneArr[2] = 2;
                rxSerdesLaneArr[3] = 3;

                /*
                    1.1.6. Call with txSerdesLaneArr [0, 1, 2, 2] (same SERDES lane)
                                     and other params from 1.1.
                    Expected: NOT GT_OK.
                */
                txSerdesLaneArr[0] = 0;
                txSerdesLaneArr[1] = 1;
                txSerdesLaneArr[2] = 2;
                txSerdesLaneArr[3] = 2;

                st = cpssDxChPortXgPscLanesSwapSet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, txSerdesLaneArr[2] = %d, txSerdesLaneArr[3] = %d",
                                                 dev, port, txSerdesLaneArr[2], txSerdesLaneArr[3]);

                txSerdesLaneArr[0] = 0;
                txSerdesLaneArr[1] = 1;
                txSerdesLaneArr[2] = 2;
                txSerdesLaneArr[3] = 3;

                /*
                    1.1.3. Call with rxSerdesLaneArr [NULL] and other params from 1.1.
                    Expected: GT_BAD_PTR.
                */
                st = cpssDxChPortXgPscLanesSwapSet(dev, port, NULL, txSerdesLaneArr);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%D, %d, rxSerdesLaneArr = NULL", dev, port);

                /*
                    1.1.4. Call with txSerdesLaneArr [NULL] and other params from 1.1.
                    Expected: GT_BAD_PTR.
                */
                st = cpssDxChPortXgPscLanesSwapSet(dev, port, rxSerdesLaneArr, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%D, %d, txSerdesLaneArr = NULL", dev, port);
            }
        }

        /*
            1.2. For all active devices go over all non available physical
            ports.
        */
        rxSerdesLaneArr[0] = 0;
        rxSerdesLaneArr[1] = 1;
        rxSerdesLaneArr[2] = 2;
        rxSerdesLaneArr[3] = 3;

        txSerdesLaneArr[0] = 0;
        txSerdesLaneArr[1] = 1;
        txSerdesLaneArr[2] = 2;
        txSerdesLaneArr[3] = 3;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortXgPscLanesSwapSet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* macSaLsb == 0 */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortXgPscLanesSwapSet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    rxSerdesLaneArr[0] = 0;
    rxSerdesLaneArr[1] = 1;
    rxSerdesLaneArr[2] = 2;
    rxSerdesLaneArr[3] = 3;

    txSerdesLaneArr[0] = 0;
    txSerdesLaneArr[1] = 1;
    txSerdesLaneArr[2] = 2;
    txSerdesLaneArr[3] = 3;

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortXgPscLanesSwapSet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, macSaLsb == 0 */

    st = cpssDxChPortXgPscLanesSwapSet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortXgPscLanesSwapGet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    OUT GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS],
    OUT GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortXgPscLanesSwapGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh3 and above)
    1.1.1. Call with not NULL rxSerdesLaneArr and txSerdesLaneArr;
    Expected: GT_OK.
    1.1.2. Call with rxSerdesLaneArr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.1.3. Call with txSerdesLaneArr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;

    GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];
    GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* Supported for XAUI or HyperG.Stack ports only! */
            if(PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType == PRV_CPSS_PORT_XG_E)
            {
                /*
                    1.1.1. Call with not NULL rxSerdesLaneArr and txSerdesLaneArr;
                    Expected: GT_OK.
                */
                st = cpssDxChPortXgPscLanesSwapGet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

                /*
                    1.1.2. Call with rxSerdesLaneArr [NULL] and other params from 1.1.
                    Expected: GT_BAD_PTR.
                */
                st = cpssDxChPortXgPscLanesSwapGet(dev, port, NULL, txSerdesLaneArr);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%D, %d, rxSerdesLaneArr = NULL", dev, port);

                /*
                    1.1.3. Call with txSerdesLaneArr [NULL] and other params from 1.1.
                    Expected: GT_BAD_PTR.
                */
                st = cpssDxChPortXgPscLanesSwapGet(dev, port, rxSerdesLaneArr, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%D, %d, txSerdesLaneArr = NULL", dev, port);
            }
        }

        /* 1.2. For all active devices go over all non available physical
           ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortXgPscLanesSwapGet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* macSaLsb == 0 */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortXgPscLanesSwapGet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortXgPscLanesSwapGet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, macSaLsb == 0 */

    st = cpssDxChPortXgPscLanesSwapGet(dev, port, rxSerdesLaneArr, txSerdesLaneArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortBackPressureEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortBackPressureEnableGet)
{
    /*
        ITERATE_DEVICES_PHY_CPU_PORTS (DxCh)
        1.1.1. Call with non-null enablePtr.
        Expected: GT_OK.
        1.1.2. Call with enablePtr [NULL].
        Expected: GT_BAD_PTR
    */
    GT_U8                  dev;
    GT_STATUS              st       = GT_OK;
    GT_U8                  port     = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL                enable   = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT portType = PRV_CPSS_PORT_NOT_EXISTS_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);
            /* 1.1.1. Call with non-null enablePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortBackPressureEnableGet(dev, port, &enable);

            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }

            /* 1.1.2. Call with enablePtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortBackPressureEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortBackPressureEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortBackPressureEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortBackPressureEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortBackPressureEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortBackPressureEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortCrcCheckEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortCrcCheckEnableGet)
{
    /*
        ITERATE_DEVICES_PHY_PORTS (DxCh)
        1.1.1. Call with non-null enablePtr.
        Expected: GT_OK.
        1.1.2. Call with enablePtr [NULL].
        Expected: GT_BAD_PTR
    */
    GT_U8       dev;
    GT_STATUS   st     = GT_OK;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null enablePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortCrcCheckEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with enablePtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortCrcCheckEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortCrcCheckEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortCrcCheckEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortCrcCheckEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortCrcCheckEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortCrcCheckEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortDuplexAutoNegEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortDuplexAutoNegEnableGet)
{
    /*
        ITERATE_DEVICES_PHY_PORTS (DxCh)
        1.1.1. Call with non-null enablePtr.
        Expected: GT_OK.
        1.1.2. Call with enablePtr [NULL].
        Expected: GT_BAD_PTR
    */
    GT_U8       dev;
    GT_STATUS   st     = GT_OK;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null enablePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortDuplexAutoNegEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with enablePtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortDuplexAutoNegEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortDuplexAutoNegEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortDuplexAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortDuplexAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortDuplexAutoNegEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortExtraIpgGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_U8    *numberPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortExtraIpgGet)
{
    /*
        ITERATE_DEVICES_PHY_PORTS (DxCh)
        1.1.1. Call with non-null numberPtr.
        Expected: GT_OK.
        1.1.2. Call with numberPtr [NULL].
        Expected: GT_BAD_PTR
    */
    GT_U8       dev;
    GT_STATUS   st     = GT_OK;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_U8       number = 1;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null numberPtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortExtraIpgGet(dev, port, &number);

            if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev,port) != PRV_CPSS_PORT_XG_E)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, number);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, number);
            }

            /* 1.1.2. Call with numberPtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortExtraIpgGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortExtraIpgGet(dev, port, &number);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortExtraIpgGet(dev, port, &number);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortExtraIpgGet(dev, port, &number);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortExtraIpgGet(dev, port, &number);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortFlowCntrlAutoNegEnableGet
(
    IN   GT_U8       devNum,
    IN   GT_U8       portNum,
    OUT  GT_BOOL     *statePtr,
    OUT  GT_BOOL     *pauseAdvertisePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortFlowCntrlAutoNegEnableGet)
{
    /*
        ITERATE_DEVICES_PHY_PORTS (DxCh)
        1.1.1. Call with non-null enablePtr.
        Expected: GT_OK.
        1.1.2. Call with enablePtr [NULL].
        Expected: GT_BAD_PTR
    */
    GT_U8       dev;
    GT_STATUS   st     = GT_OK;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     state;
    GT_BOOL     pause;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null enablePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortFlowCntrlAutoNegEnableGet(dev, port, &state, &pause);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with enablePtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortFlowCntrlAutoNegEnableGet(dev, port, NULL, &pause);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);

            st = cpssDxChPortFlowCntrlAutoNegEnableGet(dev, port, &state, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortFlowCntrlAutoNegEnableGet(dev, port, &state, &pause);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortFlowCntrlAutoNegEnableGet(dev, port, &state, &pause);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortFlowCntrlAutoNegEnableGet(dev, port, &state, &pause);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortFlowCntrlAutoNegEnableGet(dev, port, &state, &pause);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortInbandAutoNegEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortInbandAutoNegEnableGet)
{
    /*
        ITERATE_DEVICES_PHY_PORTS (DxCh)
        1.1.1. Call with non-null enablePtr.
        Expected: GT_OK.
        1.1.2. Call with enablePtr [NULL].
        Expected: GT_BAD_PTR
    */
    GT_U8       dev;
    GT_STATUS   st     = GT_OK;

    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);

            /* 1.1.1. Call with non-null enablePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortInbandAutoNegEnableGet(dev, port, &enable);

            if(PRV_CPSS_PORT_XG_E != portType)
            {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }

            /* 1.1.2. Call with enablePtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortInbandAutoNegEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortInbandAutoNegEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortInbandAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortInbandAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortInbandAutoNegEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortIpgBaseGet
(
    IN   GT_U8                        devNum,
    IN   GT_U8                        portNum,
    OUT  CPSS_PORT_XG_FIXED_IPG_ENT  *ipgBase
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortIpgBaseGet)
{
    /*
        ITERATE_DEVICES_PHY_PORTS (DxCh)
        1.1.1. Call with ipgBase = [CPSS_PORT_XG_FIXED_IPG_8_BYTES_E /
                                    CPSS_PORT_XG_FIXED_IPG_12_BYTES_E]
        Expected: GT_OK.
        1.1.2. Call with  [NULL].
        Expected: GT_BAD_PTR
    */
    GT_U8                        dev;
    GT_STATUS                    st        = GT_OK;
    GT_U8                        port      = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_PORT_XG_FIXED_IPG_ENT   ipgBase;
    PRV_CPSS_PORT_TYPE_ENT       portType  = PRV_CPSS_PORT_NOT_EXISTS_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);
            /*
               1.1.1. Call with ipgBase = [CPSS_PORT_XG_FIXED_IPG_8_BYTES_E /
                                           CPSS_PORT_XG_FIXED_IPG_12_BYTES_E]
               Expected: GT_OK.
            */
            st = cpssDxChPortIpgBaseGet(dev, port, &ipgBase);

            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }

            /* 1.1.2. Call with  [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortIpgBaseGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortIpgBaseGet(dev, port, &ipgBase);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortIpgBaseGet(dev, port, &ipgBase);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortIpgBaseGet(dev, port, &ipgBase);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortIpgBaseGet(dev, port, &ipgBase);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortIpgBaseSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_XG_FIXED_IPG_ENT  ipgBase
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortIpgBaseSet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxCh)
    1.1.1. Call with ipgBase = [CPSS_PORT_XG_FIXED_IPG_8_BYTES_E /
                                CPSS_PORT_XG_FIXED_IPG_12_BYTES_E]
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortIpgBaseSet with not-NULL ipgBase.
    Expected: GT_OK and the same ipgBase as was set.
*/
    GT_STATUS                   st        = GT_OK;
    GT_U8                       dev;
    GT_U8                       port      = PORT_CTRL_VALID_PHY_PORT_CNS;
    PRV_CPSS_PORT_TYPE_ENT      portType  = PRV_CPSS_PORT_NOT_EXISTS_E;
    CPSS_PORT_XG_FIXED_IPG_ENT  ipgBase   = CPSS_PORT_XG_FIXED_IPG_8_BYTES_E;
    CPSS_PORT_XG_FIXED_IPG_ENT  ipgBaseGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);
            /*
                1.1.1. Call with ipgBase = [CPSS_PORT_XG_FIXED_IPG_8_BYTES_E /
                                            CPSS_PORT_XG_FIXED_IPG_12_BYTES_E]
                Expected: GT_OK.
            */
            ipgBase = CPSS_PORT_XG_FIXED_IPG_8_BYTES_E;

            st = cpssDxChPortIpgBaseSet(dev, port, ipgBase);

            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssDxChPortIpgBaseSet: %d, %d, GT_TRUE", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }

            ipgBase = CPSS_PORT_XG_FIXED_IPG_12_BYTES_E;

            st = cpssDxChPortIpgBaseSet(dev, port, ipgBase);

            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssDxChPortIpgBaseSet: %d, %d, GT_TRUE", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }

            /*
                1.1.2. Call cpssDxChPortIpgBaseGet with not-NULL ipgBase.
                Expected: GT_OK and the same ipgBase as was set.
            */
            st = cpssDxChPortIpgBaseGet(dev, port, &ipgBaseGet);

            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChPortIpgBaseGet: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(ipgBase, ipgBaseGet,
                    "got another ipgBase then was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }

            /*  1.1.3. Call with wrong enum values ipgBase.   */
            /*  Expected: GT_BAD_PARAM.                     */
            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_ENUMS_CHECK_MAC(cpssDxChPortIpgBaseSet
                                    (dev, port, ipgBase),
                                    ipgBase);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortIpgBaseSet(dev, port, ipgBase);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port    = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        ipgBase = CPSS_PORT_XG_FIXED_IPG_12_BYTES_E;

        st = cpssDxChPortIpgBaseSet(dev, port, ipgBase);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortIpgBaseSet(dev, port, ipgBase);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port    = PORT_CTRL_VALID_PHY_PORT_CNS;
    ipgBase = CPSS_PORT_XG_FIXED_IPG_12_BYTES_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortIpgBaseSet(dev, port, ipgBase);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    ipgBase = CPSS_PORT_XG_FIXED_IPG_8_BYTES_E;

    st = cpssDxChPortIpgBaseSet(dev, port, ipgBase);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortMruGet
(
    IN   GT_U8                        devNum,
    IN   GT_U8                        portNum,
    OUT  GT_U32                      *mruSizePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortMruGet)
{
    /*
        ITERATE_DEVICES_PHY_PORTS (DxCh)
        1.1.1. Call with not null mruSizePtr.
        Expected: GT_OK.
        1.1.2. Call with null mruSizePtr [NULL].
        Expected: GT_BAD_PTR
    */
    GT_U8                        dev;
    GT_STATUS                    st     = GT_OK;
    GT_U8                        port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_U32                       mruSize;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not null mruSizePtr.
                Expected: GT_OK.
            */
            st = cpssDxChPortMruGet(dev, port, &mruSize);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with null mruSizePtr [NULL].
                Expected: GT_BAD_PTR
            */
            st = cpssDxChPortMruGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortMruGet(dev, port, &mruSize);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortMruGet(dev, port, &mruSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortMruGet(dev, port, &mruSize);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortMruGet(dev, port, &mruSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortMruGet(dev, port, &mruSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPeriodicFcEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPeriodicFcEnableGet)
{
    /*
        ITERATE_DEVICES_PHY_PORTS (DxCh)
        1.1.1. Call with non-null enablePtr.
        Expected: GT_OK.
        1.1.2. Call with enablePtr [NULL].
        Expected: GT_BAD_PTR
    */
    GT_U8       dev;
    GT_STATUS   st     = GT_OK;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null enablePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortPeriodicFcEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with enablePtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortPeriodicFcEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortPeriodicFcEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortPeriodicFcEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortPeriodicFcEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPeriodicFcEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPeriodicFcEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortPreambleLengthGet
(
    IN  GT_U8                    devNum,
    IN  GT_U8                    portNum,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_U32                  *length
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortPreambleLengthGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with direction [CPSS_PORT_DIRECTION_TX_E].
    Expected: GT_OK for GE ports and not GT_OK for others.
    1.1.2. Call with direction [CPSS_PORT_DIRECTION_RX_E].
    Expected: GT_OK for XG 10Gbps ports and not GT_OK for others.
    1.1.3. Call with direction [CPSS_PORT_DIRECTION_BOTH_E].
    Expected: GT_BAD_PARAM.
    1.1.4. Call with wrong enum values direction.
    Expected: GT_BAD_PARAM.
    1.1.5. Call with length [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_U8                    dev;
    GT_STATUS                st        = GT_OK;
    GT_U8                    port      = PORT_CTRL_VALID_PHY_PORT_CNS;
    PRV_CPSS_PORT_TYPE_ENT   portType  = PRV_CPSS_PORT_NOT_EXISTS_E;
    CPSS_PORT_DIRECTION_ENT  direction = CPSS_PORT_DIRECTION_RX_E;
    GT_U32                   length;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with direction [CPSS_PORT_DIRECTION_TX_E].
                Expected: GT_OK
            */

            direction = CPSS_PORT_DIRECTION_TX_E;

            st = cpssDxChPortPreambleLengthGet(dev, port, direction, &length);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);

            /*
                1.1.2. Call with direction [CPSS_PORT_DIRECTION_RX_E].
                Expected: GT_OK for XG ports and GT_NOT_SUPPORTED for others.
            */

            direction = CPSS_PORT_DIRECTION_RX_E;

            st = cpssDxChPortPreambleLengthGet(dev, port, direction, &length);

            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, length);
            }

            /*
                1.1.3. Call with direction [CPSS_PORT_DIRECTION_BOTH_E].
                Expected: GT_BAD_PARAM.
            */

            direction = CPSS_PORT_DIRECTION_BOTH_E;

            st = cpssDxChPortPreambleLengthGet(dev, port, direction, &length);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, direction, length);

            /*
                1.1.4. Call with wrong enum values direction.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPortPreambleLengthGet
                                (dev, port, direction, &length),
                                direction);

            /*
               1.1.5. Call with  NULL lenghtGet [NULL].
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortPreambleLengthGet(dev, port, direction, NULL);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PTR, st, dev, port, direction, length);
        }

        /*
           1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortPreambleLengthGet(dev, port, direction, &length);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortPreambleLengthGet(dev, port, direction, &length);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        direction = CPSS_PORT_DIRECTION_TX_E;
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortPreambleLengthGet(dev, port, direction, &length);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortPreambleLengthGet(dev, port, direction, &length);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortPreambleLengthGet(dev, port, direction, &length);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortSpeedAutoNegEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortSpeedAutoNegEnableGet)
{
    /*
        ITERATE_DEVICES_PHY_PORTS (DxCh)
        1.1.1. Call with non-null enablePtr.
        Expected: GT_OK.
        1.1.2. Call with enablePtr [NULL].
        Expected: GT_BAD_PTR
    */
    GT_U8       dev;
    GT_STATUS   st     = GT_OK;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null enablePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortSpeedAutoNegEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with enablePtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortSpeedAutoNegEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortSpeedAutoNegEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortSpeedAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortSpeedAutoNegEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortSpeedAutoNegEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortXGmiiModeGet
(
    IN  GT_U8                     devNum,
    IN  GT_U8                     portNum,
    OUT CPSS_PORT_XGMII_MODE_ENT *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortXGmiiModeGet)
{
    /*
        ITERATE_DEVICES_PHY_PORTS (DxCh)
        1.1.1. Call with non-null modePtr.
        Expected: GT_OK.
        1.1.2. Call with modePtr [NULL].
        Expected: GT_BAD_PTR
    */
    GT_U8                    dev;
    GT_STATUS                st       = GT_OK;
    GT_U8                    port     = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_PORT_XGMII_MODE_ENT mode;
    PRV_CPSS_PORT_TYPE_ENT   portType = PRV_CPSS_PORT_NOT_EXISTS_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null modePtr.
               Expected: GT_OK.
            */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);

            st = cpssDxChPortXGmiiModeGet(dev, port, &mode);

            if(PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, mode);
            }
            /* 1.1.2. Call with modePtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortXGmiiModeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortXGmiiModeGet(dev, port, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortXGmiiModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortXGmiiModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortXGmiiModeGet(dev, port, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortGroupSerdesPowerStatusSet
(
    IN  GT_U8      devNum,
    IN  GT_U32     portSerdesGroup,
    IN  GT_BOOL    powerUp
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortGroupSerdesPowerStatusSet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxCh3 and above)
    1.1. Call with portSerdesGroup[0] and powerUp[GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call with wrong enum values portSerdesGroup.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS  st        = GT_OK;
    GT_U8      dev;
    GT_U32     portSerdesGroup = 0;
    GT_BOOL    powerUp = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with portSerdesGroup[0] and powerUp[GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        portSerdesGroup = 0;

        st = cpssDxChPortGroupSerdesPowerStatusSet(dev, portSerdesGroup, powerUp);
        if (IS_PORT_FE_E(dev,portSerdesGroup))
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_SUPPORTED, st, dev);
        else
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong enum values portSerdesGroup.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPortGroupSerdesPowerStatusSet
                            (dev, portSerdesGroup, powerUp),
                            portSerdesGroup);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortGroupSerdesPowerStatusSet(dev, portSerdesGroup, powerUp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortGroupSerdesPowerStatusSet(dev, portSerdesGroup, powerUp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortSerdesGroupGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_U32   *portSerdesGroupPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortSerdesGroupGet)
{
    /*
        ITERATE_DEVICES_PHY_PORTS (DxCh3 and above)
        1.1.1. Call with non-null portSerdesGroupPtr.
        Expected: GT_OK.
        1.1.2. Call with portSerdesGroupPtr [NULL].
        Expected: GT_BAD_PTR
    */
    GT_U8                    dev;
    GT_STATUS                st       = GT_OK;
    GT_U8                    port     = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_U32                   portSerdesGroup;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
               1.1.1. Call with non-null portSerdesGroupPtr.
               Expected: GT_OK.
            */

            st = cpssDxChPortSerdesGroupGet(dev, port, &portSerdesGroup);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, portSerdesGroup);

            /*
               1.1.2. Call with portSerdesGroupPtr [NULL].
               Expected: GT_BAD_PTR
            */
            st = cpssDxChPortSerdesGroupGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChPortSerdesGroupGet(dev, port, &portSerdesGroup);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortSerdesGroupGet(dev, port, &portSerdesGroup);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortSerdesGroupGet(dev, port, &portSerdesGroup);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortSerdesGroupGet(dev, port, &portSerdesGroup);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortSerdesPowerStatusSet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  lanesBmp,
    IN  GT_BOOL                 powerUp
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortSerdesPowerStatusSet)
{
    /*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh)
    1.1.1. Call with
    Expected: GT_OK.
    */

    GT_STATUS   st     = GT_OK;
    CPSS_PP_FAMILY_TYPE_ENT                 devFamily;

    GT_U8       dev;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;

    CPSS_PORT_DIRECTION_ENT direction = 0;
    GT_U32                  lanesBmp = 0;
    GT_BOOL                 powerUp = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT  portType;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
               1.1.1. Call with direction [CPSS_PORT_DIRECTION_RX_E /
                                           CPSS_PORT_DIRECTION_TX_E /
                                           CPSS_PORT_DIRECTION_BOTH_E],
                              and powerUp [GT_TRUE / GT_FALSE].
               Expected: GT_OK.
            */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d",
                                         dev, port);

            st = cpssDxChPortInterfaceModeGet(dev, port, &ifMode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortInterfaceModeGet: %d, %d",
                                         dev, port);

            /*call with powerUp = GT_FALSE */
            powerUp = GT_FALSE;

            /* call with direction = CPSS_PORT_DIRECTION_RX_E */
            direction = CPSS_PORT_DIRECTION_RX_E;

            st = cpssDxChPortSerdesPowerStatusSet(dev, port, direction,
                                                  lanesBmp, powerUp);

            if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else
            {
                /* the function is supported for XG ports only */
                if (portType != PRV_CPSS_PORT_XG_E)
                {
                    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                }
                else
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }

            /* call with direction = CPSS_PORT_DIRECTION_TX_E */
            direction = CPSS_PORT_DIRECTION_TX_E;

            st = cpssDxChPortSerdesPowerStatusSet(dev, port, direction,
                                                  lanesBmp, powerUp);

            if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else
            {
                /* the function is supported for XG ports only */
                if (portType != PRV_CPSS_PORT_XG_E)
                {
                    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                }
                else
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }

            /* call with direction = CPSS_PORT_DIRECTION_BOTH_E */
            direction = CPSS_PORT_DIRECTION_BOTH_E;

            st = cpssDxChPortSerdesPowerStatusSet(dev, port, direction,
                                                  lanesBmp, powerUp);

            /* CH3 Gig ports does not supports this API */
            if(((devFamily == CPSS_PP_FAMILY_CHEETAH3_E) && (portType == PRV_CPSS_PORT_GE_E)) ||
               (portType == PRV_CPSS_PORT_FE_E))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else if(ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else
            {
                if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
                {
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                }
                else
                {
                    /* the function is supported for XG ports only */
                    if (portType != PRV_CPSS_PORT_XG_E)
                    {
                        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                    }
                    else
                        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                }
            }

            /*call with powerUp = GT_TRUE */
            powerUp = GT_TRUE;

            /* call with direction = CPSS_PORT_DIRECTION_RX_E */
            direction = CPSS_PORT_DIRECTION_RX_E;

            st = cpssDxChPortSerdesPowerStatusSet(dev, port, direction,
                                                  lanesBmp, powerUp);

            if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else if(ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else
            {
                /* the function is supported for XG ports only */
                if (portType != PRV_CPSS_PORT_XG_E)
                {
                    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                }
                else
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }

            /* call with direction = CPSS_PORT_DIRECTION_TX_E */
            direction = CPSS_PORT_DIRECTION_TX_E;

            st = cpssDxChPortSerdesPowerStatusSet(dev, port, direction,
                                                  lanesBmp, powerUp);

            if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else if(ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else
            {
                /* the function is supported for XG ports only */
                if (portType != PRV_CPSS_PORT_XG_E)
                {
                    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                }
                else
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }

            /* call with direction = CPSS_PORT_DIRECTION_BOTH_E */
            direction = CPSS_PORT_DIRECTION_BOTH_E;

            st = cpssDxChPortSerdesPowerStatusSet(dev, port, direction,
                                                  lanesBmp, powerUp);

            /* CH3 Gig ports does not supports this API */
            if(((devFamily == CPSS_PP_FAMILY_CHEETAH3_E) && (portType == PRV_CPSS_PORT_GE_E)) ||
               (portType == PRV_CPSS_PORT_FE_E))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else if(ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else
            {
                if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
                {
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                }
                else
                {
                    /* the function is supported for XG ports only */
                    if (portType != PRV_CPSS_PORT_XG_E)
                    {
                        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                    }
                    else
                        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
                }
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical
           ports.
        */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortSerdesPowerStatusSet(dev, port, direction,
                                                  lanesBmp, powerUp);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }


        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortSerdesPowerStatusSet(dev, port, direction,
                                              lanesBmp, powerUp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortSerdesPowerStatusSet(dev, port, direction,
                                              lanesBmp, powerUp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortSerdesPowerStatusSet(dev, port, direction,
                                              lanesBmp, powerUp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortSerdesPowerStatusSet(dev, port, direction,
                                          lanesBmp, powerUp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortFlowControlModeSet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    IN  CPSS_DXCH_PORT_FC_MODE_ENT  fcMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortFlowControlModeSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (Lion)
    1.1.1. Call with fcMode [CPSS_DXCH_PORT_FC_MODE_802_3X_E /
                             CPSS_DXCH_PORT_FC_MODE_PFC_E /
                             CPSS_DXCH_PORT_FC_MODE_LL_FC_E]
    Expected: GT_OK.
    1.1.2. Call with cpssDxChPortFlowControlModeGet with the same fcMode.
    Expected: GT_OK and the same fcMode.
    1.1.3. Call with wrong enum values fcMode
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS            st   = GT_OK;

    GT_U8                dev;
    GT_U8                port = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_DXCH_PORT_FC_MODE_ENT fcMode = CPSS_DXCH_PORT_FC_MODE_802_3X_E;
    CPSS_DXCH_PORT_FC_MODE_ENT fcModeGet;


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
                1.1.1. Call with fcMode [CPSS_DXCH_PORT_FC_MODE_802_3X_E /
                                         CPSS_DXCH_PORT_FC_MODE_PFC_E /
                                         CPSS_DXCH_PORT_FC_MODE_LL_FC_E]
               Expected: GT_OK.
            */

            fcMode = CPSS_DXCH_PORT_FC_MODE_802_3X_E;

            st = cpssDxChPortFlowControlModeSet(dev, port, fcMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, fcMode);

            if (GT_OK == st)
            {
                /*
                    1.1.2. Call with cpssDxChPortFlowControlModeGet with the same fcMode.
                    Expected: GT_OK and the same fcMode.
                */

                st = cpssDxChPortFlowControlModeGet(dev, port, &fcModeGet);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, fcMode);

                UTF_VERIFY_EQUAL2_STRING_MAC(fcMode, fcModeGet,
                            "get another fcMode value than was set: %d, %d", dev, port);
            }

            /*
                1.1.1. Call with fcMode [CPSS_DXCH_PORT_FC_MODE_802_3X_E /
                                         CPSS_DXCH_PORT_FC_MODE_PFC_E /
                                         CPSS_DXCH_PORT_FC_MODE_LL_FC_E]
               Expected: GT_OK.
            */

            fcMode = CPSS_DXCH_PORT_FC_MODE_PFC_E;

            st = cpssDxChPortFlowControlModeSet(dev, port, fcMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, fcMode);

            if (GT_OK == st)
            {
                /*
                    1.1.2. Call with cpssDxChPortFlowControlModeGet with the same fcMode.
                    Expected: GT_OK and the same fcMode.
                */

                st = cpssDxChPortFlowControlModeGet(dev, port, &fcModeGet);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, fcMode);

                UTF_VERIFY_EQUAL2_STRING_MAC(fcMode, fcModeGet,
                            "get another fcMode value than was set: %d, %d", dev, port);
            }

            /*
                1.1.1. Call with fcMode [CPSS_DXCH_PORT_FC_MODE_802_3X_E /
                                         CPSS_DXCH_PORT_FC_MODE_PFC_E /
                                         CPSS_DXCH_PORT_FC_MODE_LL_FC_E]
               Expected: GT_OK.
            */

            fcMode = CPSS_DXCH_PORT_FC_MODE_LL_FC_E;

            st = cpssDxChPortFlowControlModeSet(dev, port, fcMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, fcMode);

            if (GT_OK == st)
            {
                /*
                    1.1.2. Call with cpssDxChPortFlowControlModeGet with the same fcMode.
                    Expected: GT_OK and the same fcMode.
                */

                st = cpssDxChPortFlowControlModeGet(dev, port, &fcModeGet);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, fcMode);

                UTF_VERIFY_EQUAL2_STRING_MAC(fcMode, fcModeGet,
                            "get another fcMode value than was set: %d, %d", dev, port);
            }

            /*
               1.1.3. Call with wrong enum values fcMode
               Expected: GT_BAD_PARAM
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPortFlowControlModeSet
                                (dev, port, fcMode),
                                fcMode);
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
            /* fcMode [CPSS_PORT_FULL_DUPLEX_E]               */
            st = cpssDxChPortFlowControlModeSet(dev, port, fcMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortFlowControlModeSet(dev, port, fcMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortFlowControlModeSet(dev, port, fcMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, fcMode);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortFlowControlModeSet(dev, port, fcMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, fcMode == CPSS_PORT_FULL_DUPLEX_E */

    st = cpssDxChPortFlowControlModeSet(dev, port, fcMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortFlowControlModeGet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_DXCH_PORT_FC_MODE_ENT  *fcModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortFlowControlModeGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (Lion)
    1.1.1. Call with non-null fcModePtr.
    Expected: GT_OK.
    1.1.2. Call with fcModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS            st   = GT_OK;

    GT_U8                dev;
    GT_U8                port = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_DXCH_PORT_FC_MODE_ENT fcMode;

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
               1.1.1. Call with non-null fcModePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortFlowControlModeGet(dev, port, &fcMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
               1.1.2. Call with fcModePtr [NULL].
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortFlowControlModeGet(dev, port, NULL);
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
            st = cpssDxChPortFlowControlModeGet(dev, port, &fcMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortFlowControlModeGet(dev, port, &fcMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortFlowControlModeGet(dev, port, &fcMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortFlowControlModeGet(dev, port, &fcMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortFlowControlModeGet(dev, port, &fcMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortSerdesCalibrationStartSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortSerdesCalibrationStartSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORT (xCat and above)
    1.1.1. Call with port.
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PORT_CTRL_VALID_PHY_PORT_CNS;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = cpssDxChPortInterfaceModeGet(dev, port, &ifMode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortInterfaceModeGet: %d, %d",
                                         dev, port);

            /*
                1.1.1. Call with not null port.
                Expected: GT_NOT_SUPPORTED for FE ports, GT_OK for other ports.
            */
            st = cpssDxChPortSerdesCalibrationStartSet(dev, port);
            if (IS_PORT_FE_E(dev, port))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else if(ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port);
            }
            else
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPortSerdesCalibrationStartSet(dev, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortSerdesCalibrationStartSet(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortSerdesCalibrationStartSet(dev, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortSerdesCalibrationStartSet(dev, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortSerdesCalibrationStartSet(dev, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortMacResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortMacResetStateSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxChx)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
*/
    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
               1.1.1. Call with enable [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_FALSE] */
            enable = GT_FALSE;

            st = cpssDxChPortMacResetStateSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /* Call function with enable = GT_TRUE] */
            enable = GT_TRUE;

            st = cpssDxChPortMacResetStateSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
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
            st = cpssDxChPortMacResetStateSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* enable == GT_TRUE  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortMacResetStateSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        /* enable == GT_TRUE  */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortMacResetStateSet(dev, port, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    enable = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortMacResetStateSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, enable == GT_TRUE */

    st = cpssDxChPortMacResetStateSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortSerdesResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortSerdesResetStateSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (xCat and above)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
*/
    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;

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
            st = cpssDxChPortInterfaceModeGet(dev, port, &ifMode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChPortInterfaceModeGet: %d, %d",
                                         dev, port);
            /*
               1.1.1. Call with enable [GT_FALSE and GT_TRUE].
               Expected: GT_OK.
            */

            /* Call function with enable = GT_FALSE] */
            enable = GT_FALSE;

            st = cpssDxChPortSerdesResetStateSet(dev, port, enable);
            if (IS_PORT_FE_E(dev,port))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
            }
            else
            {
                if((ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
                    || ((0 == PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev)) && (ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E) && (port%2 != 0)))
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
                else
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }

            /* Call function with enable = GT_TRUE] */
            enable = GT_TRUE;

            st = cpssDxChPortSerdesResetStateSet(dev, port, enable);
            if (IS_PORT_FE_E(dev,port))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
            }
            else
            {
                if((ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
                    || ((0 == PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev)) && (ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E) && (port%2 != 0)))
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, enable);
                else
                    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
            }
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
            st = cpssDxChPortSerdesResetStateSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* enable == GT_TRUE  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortSerdesResetStateSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        /* enable == GT_TRUE  */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortSerdesResetStateSet(dev, port, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, enable);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    enable = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortSerdesResetStateSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0, enable == GT_TRUE */

    st = cpssDxChPortSerdesResetStateSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortForward802_3xEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortForward802_3xEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh3 and above)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     enable = GT_FALSE;
    CPSS_PP_FAMILY_TYPE_ENT devFamily;

    /* Prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
               1.1.1. Call with non-null enablePtr.
               Expected: GT_OK.
            */
            st = cpssDxChPortForward802_3xEnableGet(dev, port, &enable);
            if(!IS_PORT_XG_E(dev, port))
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            else
                UTF_VERIFY_EQUAL2_PARAM_MAC((devFamily == CPSS_PP_FAMILY_CHEETAH3_E) ? 
                                                GT_NOT_SUPPORTED : GT_OK, st, dev, port);

            /*
               1.1.2. Call with enablePtr [NULL].
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortForward802_3xEnableGet(dev, port, NULL);
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
            st = cpssDxChPortForward802_3xEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortForward802_3xEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortForward802_3xEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortForward802_3xEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortForward802_3xEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortForward802_3xEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortForward802_3xEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh3 and above)
    1.1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortForward802_3xEnableGet.
    Expected: GT_OK and the same enable value as was set.
*/
    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port      = PORT_CTRL_VALID_PHY_PORT_CNS;
    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_TRUE;
    CPSS_PP_FAMILY_TYPE_ENT devFamily;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* Call function with enable [GT_FALSE] */
            enable = GT_FALSE;
            st = cpssDxChPortForward802_3xEnableSet(dev, port, enable);
            if(!IS_PORT_XG_E(dev, port))
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            else
                UTF_VERIFY_EQUAL2_PARAM_MAC((devFamily == CPSS_PP_FAMILY_CHEETAH3_E) ? 
                                                GT_NOT_SUPPORTED : GT_OK, st, dev, port);

            /*
                1.1.2. Call cpssDxChPortForward802_3xEnableGet.
                Expected: GT_OK and the same enable value as was set.
            */
            st = cpssDxChPortForward802_3xEnableGet(dev, port, &enableGet);
            if(IS_PORT_XG_E(dev, port) && (devFamily == CPSS_PP_FAMILY_CHEETAH3_E))
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_SUPPORTED, st,
                        "cpssDxChPortForward802_3xEnableGet: %d", dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                        "cpssDxChPortForward802_3xEnableGet: %d", dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                        "get another enable than was set: %d, %d", dev, port);
            }

            /* Call function with enable [GT_TRUE] */
            enable = GT_TRUE;
            st = cpssDxChPortForward802_3xEnableSet(dev, port, enable);
            if(!IS_PORT_XG_E(dev, port))
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            else
                UTF_VERIFY_EQUAL2_PARAM_MAC((devFamily == CPSS_PP_FAMILY_CHEETAH3_E) ? 
                                                GT_NOT_SUPPORTED : GT_OK, st, dev, port);

            /*
                1.1.2. Call cpssDxChPortForward802_3xEnableGet.
                Expected: GT_OK and the same enable value as was set.
            */
            st = cpssDxChPortForward802_3xEnableGet(dev, port, &enableGet);
            if(IS_PORT_XG_E(dev, port) && (devFamily == CPSS_PP_FAMILY_CHEETAH3_E))
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_SUPPORTED, st,
                        "cpssDxChPortForward802_3xEnableGet: %d", dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                        "cpssDxChPortForward802_3xEnableGet: %d", dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                        "get another enable than was set: %d, %d", dev, port);
            }
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
            /* enable == GT_TRUE */
            st = cpssDxChPortForward802_3xEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* enable == GT_TRUE */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortForward802_3xEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        /* enable == GT_TRUE */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortForward802_3xEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, enable);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    enable = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortForward802_3xEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortForward802_3xEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortForwardUnknownMacControlFramesEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortForwardUnknownMacControlFramesEnableGet)
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
    GT_U8       port   = PORT_CTRL_VALID_PHY_PORT_CNS;
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
            st = cpssDxChPortForwardUnknownMacControlFramesEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
               1.1.2. Call with enablePtr [NULL].
               Expected: GT_BAD_PTR.
            */
            st = cpssDxChPortForwardUnknownMacControlFramesEnableGet(dev, port, NULL);
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
            st = cpssDxChPortForwardUnknownMacControlFramesEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortForwardUnknownMacControlFramesEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortForwardUnknownMacControlFramesEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortForwardUnknownMacControlFramesEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortForwardUnknownMacControlFramesEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPortForwardUnknownMacControlFramesEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChPortForwardUnknownMacControlFramesEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (xCat2)
    1.1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChPortForwardUnknownMacControlFramesEnableGet.
    Expected: GT_OK and the same enable value as was set.
*/
    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port      = PORT_CTRL_VALID_PHY_PORT_CNS;
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
            st = cpssDxChPortForwardUnknownMacControlFramesEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call cpssDxChPortForward802_3xEnableGet.
                Expected: GT_OK and the same enable value as was set.
            */
            st = cpssDxChPortForwardUnknownMacControlFramesEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                    "cpssDxChPortForwardUnknownMacControlFramesEnableGet: %d", dev);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                    "get another enable than was set: %d, %d", dev, port);

            /* Call function with enable [GT_TRUE] */
            enable = GT_TRUE;
            st = cpssDxChPortForwardUnknownMacControlFramesEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call cpssDxChPortForward802_3xEnableGet.
                Expected: GT_OK and the same enable value as was set.
            */
            st = cpssDxChPortForwardUnknownMacControlFramesEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                    "cpssDxChPortForwardUnknownMacControlFramesEnableGet: %d", dev);
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
            /* enable == GT_TRUE */
            st = cpssDxChPortForwardUnknownMacControlFramesEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        /* enable == GT_TRUE */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPortForwardUnknownMacControlFramesEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        /* enable == GT_TRUE */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChPortForwardUnknownMacControlFramesEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, enable);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    enable = GT_TRUE;
    port = PORT_CTRL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_LION_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPortForwardUnknownMacControlFramesEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPortForwardUnknownMacControlFramesEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChPortCtrl suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChPortCtrl)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortAttributesOnPortGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortMacSaLsbSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortMacSaLsbGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortDuplexModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortDuplexModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortSpeedSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortSpeedGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortDuplexAutoNegEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortFlowCntrlAutoNegEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortSpeedAutoNegEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortFlowControlEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortFlowControlEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPeriodicFcEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortBackPressureEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortLinkStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortInterfaceModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortInterfaceModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortForceLinkPassEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortForceLinkDownEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortMruSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCrcCheckEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortXGmiiModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortIpgSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortIpgGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortExtraIpgSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortXgmiiLocalFaultGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortXgmiiRemoteFaultGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortMacStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortInternalLoopbackEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortInternalLoopbackEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortInbandAutoNegEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPreambleLengthSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortMacSaBaseSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortMacSaBaseGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortSerdesComboModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortSerdesComboModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortSerdesConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortSerdesConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPaddingEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPaddingEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortExcessiveCollisionDropEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortExcessiveCollisionDropEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortXgLanesSwapEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortXgLanesSwapEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortInBandAutoNegBypassEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortInBandAutoNegBypassEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortMacErrorIndicationPortSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortMacErrorIndicationPortGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortMacErrorIndicationGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortXgPscLanesSwapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortXgPscLanesSwapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortBackPressureEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortCrcCheckEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortDuplexAutoNegEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortExtraIpgGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortFlowCntrlAutoNegEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortInbandAutoNegEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortIpgBaseGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortIpgBaseSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortMruGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPeriodicFcEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortPreambleLengthGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortSpeedAutoNegEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortXGmiiModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortGroupSerdesPowerStatusSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortSerdesGroupGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortSerdesPowerStatusSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortFlowControlModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortFlowControlModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortSerdesCalibrationStartSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortMacResetStateSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortSerdesResetStateSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortForward802_3xEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortForward802_3xEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortForwardUnknownMacControlFramesEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPortForwardUnknownMacControlFramesEnableSet)

UTF_SUIT_END_TESTS_MAC(cpssDxChPortCtrl)

