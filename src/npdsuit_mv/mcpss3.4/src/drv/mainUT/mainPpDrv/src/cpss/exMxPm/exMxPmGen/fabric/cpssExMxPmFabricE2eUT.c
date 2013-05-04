/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmFabricE2eUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmFabricE2e, that provides
*       CPSS ExMxPm Fabric Connectivity End to End flow control APIs.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricE2e.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define FABRIC_E2E_VALID_VIRT_PORT_CNS  0

/* Invalid enum */
#define FABRIC_E2E_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eAdvertiseEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  GT_U32   tc,
    IN  GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eAdvertiseEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with tc [0 / 7]
                     and enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmFabricE2eAdvertiseEnableGet with the same tc
                                                           non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.1.3. Call with out of range tc [8]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = FABRIC_E2E_VALID_VIRT_PORT_CNS;

    GT_U32      tc        = 0;
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

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with tc [0 / 7]
                                 and enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */

            /* Call with tc [0] */
            tc     = 0;
            enable = GT_FALSE;

            st = cpssExMxPmFabricE2eAdvertiseEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, enable);

            /*
                1.1.2. Call cpssExMxPmFabricE2eAdvertiseEnableGet with the same tc
                                                                       non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmFabricE2eAdvertiseEnableGet(dev, port, tc, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmFabricE2eAdvertiseEnableGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            
            /* Call with tc [7] */
            tc     = 7;
            enable = GT_TRUE;

            st = cpssExMxPmFabricE2eAdvertiseEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, tc, enable);

            /*
                1.1.2. Call cpssExMxPmFabricE2eAdvertiseEnableGet with the same tc
                                                                       non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmFabricE2eAdvertiseEnableGet(dev, port, tc, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmFabricE2eAdvertiseEnableGet: %d, %d, %d", dev, port, tc);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range tc [8]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            tc = 8;

            st = cpssExMxPmFabricE2eAdvertiseEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, tc);
        }

        tc     = 0;
        enable = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmFabricE2eAdvertiseEnableSet(dev, port, tc, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_PP_MAC(dev)->numOfVirtPorts;

        st = cpssExMxPmFabricE2eAdvertiseEnableSet(dev, port, tc, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmFabricE2eAdvertiseEnableSet(dev, port, tc, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    tc     = 0;
    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = FABRIC_E2E_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricE2eAdvertiseEnableSet(dev, port, tc, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmFabricE2eAdvertiseEnableSet(dev, port, tc, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eAdvertiseEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  GT_U32   tc,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eAdvertiseEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORTS (ExMxPm)
    1.1.1. Call with tc [0 / 7]
                     and non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with out of range tc [8]
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with enablePtr [NULL]
                     and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = FABRIC_E2E_VALID_VIRT_PORT_CNS;

    GT_U32      tc     = 0;
    GT_BOOL     enable = GT_FALSE;
    

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
                1.1.1. Call with tc [0 / 7]
                                 and non-NULL enablePtr.
                Expected: GT_OK.
            */

            /* Call with tc [0] */
            tc = 0;
            
            st = cpssExMxPmFabricE2eAdvertiseEnableGet(dev, port, tc, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);
            
            /* Call with tc [7] */
            tc = 7;

            st = cpssExMxPmFabricE2eAdvertiseEnableGet(dev, port, tc, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, tc);

            /*
                1.1.2. Call with out of range tc [8]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            tc = 8;

            st = cpssExMxPmFabricE2eAdvertiseEnableGet(dev, port, tc, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, tc);

            tc = 0;

            /*
                1.1.3. Call with enablePtr [NULL]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmFabricE2eAdvertiseEnableGet(dev, port, tc, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        tc = 0;
        
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmFabricE2eAdvertiseEnableGet(dev, port, tc, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_PP_MAC(dev)->numOfVirtPorts;

        st = cpssExMxPmFabricE2eAdvertiseEnableGet(dev, port, tc, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmFabricE2eAdvertiseEnableGet(dev, port, tc, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    tc = 0;
    
    /* 2. For not active devices check that function returns non GT_OK.*/
    port = FABRIC_E2E_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricE2eAdvertiseEnableGet(dev, port, tc, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmFabricE2eAdvertiseEnableGet(dev, port, tc, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eMsgConfigSet
(
    IN GT_U8                                 devNum,
    IN CPSS_EXMXPM_FABRIC_E2E_MSG_CONFIG_STC *msgConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eMsgConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with msgConfigPtr {e2eEnable [GT_FALSE / GT_TRUE],
                                 urgentMode [CPSS_EXMXPM_FABRIC_E2E_URGENT_NOT_READY_FOR_DP1_E /
                                             CPSS_EXMXPM_FABRIC_E2E_URGENT_CHANGE_OF_READY_STATE_E],
                                 urgentCellFrequence [1 / 0xFFFF],
                                 periodicCellFrequence [1 / 0xFFFF]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricE2eMsgConfigGet with non-NULL msgConfigPtr.
    Expected: GT_OK and the same msgConfigPtr.
    1.3. Call with out of range msgConfigPtr->urgentMode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range msgConfigPtr->urgentCellFrequence [0 / 65536]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range msgConfigPtr->periodicCellFrequence [0 / 65536]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with msgConfigPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_E2E_MSG_CONFIG_STC   msgConfig;
    CPSS_EXMXPM_FABRIC_E2E_MSG_CONFIG_STC   msgConfigGet;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with msgConfigPtr {e2eEnable [GT_FALSE / GT_TRUE],
                                         urgentMode [CPSS_EXMXPM_FABRIC_E2E_URGENT_NOT_READY_FOR_DP1_E /
                                                     CPSS_EXMXPM_FABRIC_E2E_URGENT_CHANGE_OF_READY_STATE_E],
                                         urgentCellFrequence [1 / 0xFFFF],
                                         periodicCellFrequence [1 / 0xFFFF]}.
            Expected: GT_OK.
        */

        /* Call with msgConfigPtr->e2eEnable [GT_FALSE] */
        msgConfig.e2eEnable             = GT_FALSE;
        msgConfig.urgentMode            = CPSS_EXMXPM_FABRIC_E2E_URGENT_NOT_READY_FOR_DP1_E;
        msgConfig.urgentCellFrequence   = 1;
        msgConfig.periodicCellFrequence = 1;

        st = cpssExMxPmFabricE2eMsgConfigSet(dev, &msgConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmFabricE2eMsgConfigGet with non-NULL msgConfigPtr.
            Expected: GT_OK and the same msgConfigPtr.
        */
        st = cpssExMxPmFabricE2eMsgConfigGet(dev, &msgConfigGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                   "cpssExMxPmFabricE2eMsgConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(msgConfig.e2eEnable, msgConfigGet.e2eEnable,
                   "get another msgConfigPtr->e2eEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(msgConfig.urgentMode, msgConfigGet.urgentMode,
                   "get another msgConfigPtr->urgentMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(msgConfig.urgentCellFrequence, msgConfigGet.urgentCellFrequence,
                   "get another msgConfigPtr->urgentCellFrequence than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(msgConfig.periodicCellFrequence, msgConfigGet.periodicCellFrequence,
                   "get another msgConfigPtr->periodicCellFrequence than was set: %d", dev);
        
        /* Call with msgConfigPtr->e2eEnable [GT_TRUE] */
        msgConfig.e2eEnable             = GT_TRUE;
        msgConfig.urgentMode            = CPSS_EXMXPM_FABRIC_E2E_URGENT_CHANGE_OF_READY_STATE_E;
        msgConfig.urgentCellFrequence   = 0xFFFF;
        msgConfig.periodicCellFrequence = 0xFFFF;

        st = cpssExMxPmFabricE2eMsgConfigSet(dev, &msgConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmFabricE2eMsgConfigGet with non-NULL msgConfigPtr.
            Expected: GT_OK and the same msgConfigPtr.
        */
        st = cpssExMxPmFabricE2eMsgConfigGet(dev, &msgConfigGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                   "cpssExMxPmFabricE2eMsgConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(msgConfig.e2eEnable, msgConfigGet.e2eEnable,
                   "get another msgConfigPtr->e2eEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(msgConfig.urgentMode, msgConfigGet.urgentMode,
                   "get another msgConfigPtr->urgentMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(msgConfig.urgentCellFrequence, msgConfigGet.urgentCellFrequence,
                   "get another msgConfigPtr->urgentCellFrequence than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(msgConfig.periodicCellFrequence, msgConfigGet.periodicCellFrequence,
                   "get another msgConfigPtr->periodicCellFrequence than was set: %d", dev);

        /*
            1.3. Call with out of range msgConfigPtr->urgentMode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        msgConfig.urgentMode = FABRIC_E2E_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricE2eMsgConfigSet(dev, &msgConfig);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, msgConfigPtr->urgentMode = %d",
                                     dev, msgConfig.urgentMode);

        msgConfig.urgentMode = CPSS_EXMXPM_FABRIC_E2E_URGENT_NOT_READY_FOR_DP1_E;

        /*
            1.4. Call with out of range msgConfigPtr->urgentCellFrequence [0 / 65536]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with msgConfigPtr->urgentCellFrequence [0] */
        msgConfig.urgentCellFrequence = 0;

        st = cpssExMxPmFabricE2eMsgConfigSet(dev, &msgConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, msgConfigPtr->urgentCellFrequence = %d",
                                         dev, msgConfig.urgentCellFrequence);

        /* Call with msgConfigPtr->urgentCellFrequence [65536] */
        msgConfig.urgentCellFrequence = 65536;

        st = cpssExMxPmFabricE2eMsgConfigSet(dev, &msgConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, msgConfigPtr->urgentCellFrequence = %d",
                                         dev, msgConfig.urgentCellFrequence);

        msgConfig.urgentCellFrequence = 1;

        /*
            1.5. Call with out of range msgConfigPtr->periodicCellFrequence [0 / 65536]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with msgConfigPtr->periodicCellFrequence [0] */
        msgConfig.periodicCellFrequence = 0;

        st = cpssExMxPmFabricE2eMsgConfigSet(dev, &msgConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, msgConfigPtr->periodicCellFrequence = %d",
                                         dev, msgConfig.periodicCellFrequence);

        /* Call with msgConfigPtr->periodicCellFrequence [65536] */
        msgConfig.periodicCellFrequence = 65536;

        st = cpssExMxPmFabricE2eMsgConfigSet(dev, &msgConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, msgConfigPtr->periodicCellFrequence = %d",
                                         dev, msgConfig.periodicCellFrequence);

        msgConfig.periodicCellFrequence = 1;

        /*
            1.6. Call with msgConfigPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricE2eMsgConfigSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, msgConfigPtr = NULL", dev);
    }

    msgConfig.e2eEnable             = GT_FALSE;
    msgConfig.urgentMode            = CPSS_EXMXPM_FABRIC_E2E_URGENT_NOT_READY_FOR_DP1_E;
    msgConfig.urgentCellFrequence   = 1;
    msgConfig.periodicCellFrequence = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricE2eMsgConfigSet(dev, &msgConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricE2eMsgConfigSet(dev, &msgConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eMsgConfigGet
(
    IN  GT_U8                                 devNum,
    OUT CPSS_EXMXPM_FABRIC_E2E_MSG_CONFIG_STC *msgConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eMsgConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL msgConfigPtr.
    Expected: GT_OK.
    1.2. Call with msgConfigPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_E2E_MSG_CONFIG_STC   msgConfig;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-NULL msgConfigPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmFabricE2eMsgConfigGet(dev, &msgConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with msgConfigPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricE2eMsgConfigGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, msgConfigPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricE2eMsgConfigGet(dev, &msgConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricE2eMsgConfigGet(dev, &msgConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eMsgPathSet
(
    IN GT_U8                                 devNum,
    IN CPSS_EXMXPM_FABRIC_E2E_MSG_PATH_STC   *msgPathPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eMsgPathSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with msgPathPtr {e2eVoq [0 / 63],
                               e2eFabricTc [0 / 3] }.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricE2eMsgPathGet with non-NULL msgPathPtr.
    Expected: GT_OK and the same msgPathPtr.
    1.3. Call with out of range msgPathPtr->e2eVoq [64]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range msgPathPtr->e2eFabricTc [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with msgPathPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_E2E_MSG_PATH_STC msgPath;
    CPSS_EXMXPM_FABRIC_E2E_MSG_PATH_STC msgPathGet;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with msgPathPtr {e2eVoq [0 / 63],
                                       e2eFabricTc [0 / 3] }.
            Expected: GT_OK.
        */

        /* Call with msgConfigPtr->e2eVoq [0] */
        msgPath.e2eVoq      = 0;
        msgPath.e2eFabricTc = 0;

        st = cpssExMxPmFabricE2eMsgPathSet(dev, &msgPath);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmFabricE2eMsgPathGet with non-NULL msgPathPtr.
            Expected: GT_OK and the same msgPathPtr.
        */
        st = cpssExMxPmFabricE2eMsgPathGet(dev, &msgPathGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                   "cpssExMxPmFabricE2eMsgPathGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(msgPath.e2eVoq, msgPathGet.e2eVoq,
                   "get another msgPathPtr->e2eVoq than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(msgPath.e2eFabricTc, msgPathGet.e2eFabricTc,
                   "get another msgPathPtr->e2eFabricTc than was set: %d", dev);
        
        /* Call with msgConfigPtr->e2eVoq [63] */
        msgPath.e2eVoq      = 63;
        msgPath.e2eFabricTc = 3;

        st = cpssExMxPmFabricE2eMsgPathSet(dev, &msgPath);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmFabricE2eMsgPathGet with non-NULL msgPathPtr.
            Expected: GT_OK and the same msgPathPtr.
        */
        st = cpssExMxPmFabricE2eMsgPathGet(dev, &msgPathGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                   "cpssExMxPmFabricE2eMsgPathGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(msgPath.e2eVoq, msgPathGet.e2eVoq,
                   "get another msgPathPtr->e2eVoq than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(msgPath.e2eFabricTc, msgPathGet.e2eFabricTc,
                   "get another msgPathPtr->e2eFabricTc than was set: %d", dev);

        /*
            1.3. Call with out of range msgPathPtr->e2eVoq [64]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        msgPath.e2eVoq = 64;

        st = cpssExMxPmFabricE2eMsgPathSet(dev, &msgPath);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, msgPathPtr->e2eVoq = %d",
                                         dev, msgPath.e2eVoq);

        msgPath.e2eVoq = 0;

        /*
            1.4. Call with out of range msgPathPtr->e2eFabricTc [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        msgPath.e2eFabricTc = 4;

        st = cpssExMxPmFabricE2eMsgPathSet(dev, &msgPath);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, msgPathPtr->e2eFabricTc = %d",
                                         dev, msgPath.e2eFabricTc);

        msgPath.e2eFabricTc = 0;

        /*
            1.6. Call with msgPathPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricE2eMsgPathSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, msgPathPtr = NULL", dev);
    }

    msgPath.e2eVoq      = 0;
    msgPath.e2eFabricTc = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricE2eMsgPathSet(dev, &msgPath);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricE2eMsgPathSet(dev, &msgPath);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eMsgPathGet
(
    IN  GT_U8                                 devNum,
    OUT CPSS_EXMXPM_FABRIC_E2E_MSG_PATH_STC   *msgPathPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eMsgPathGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL msgPathPtr.
    Expected: GT_OK.
    1.2. Call with msgPathPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_E2E_MSG_PATH_STC msgPath;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-NULL msgPathPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmFabricE2eMsgPathGet(dev, &msgPath);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with msgPathPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricE2eMsgPathGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, msgPathPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricE2eMsgPathGet(dev, &msgPath);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricE2eMsgPathGet(dev, &msgPath);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eMcStatusReportEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eMcStatusReportEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricE2eMcStatusReportEnableGet
              with non-NULL enablePtr.
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
            1.1. Call with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        st = cpssExMxPmFabricE2eMcStatusReportEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmFabricE2eMcStatusReportEnableGet
                      with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmFabricE2eMcStatusReportEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricE2eMcStatusReportEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable then was set: %d", dev);

        /*
            1.1. Call with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        st = cpssExMxPmFabricE2eMcStatusReportEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmFabricE2eMcStatusReportEnableGet
                      with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmFabricE2eMcStatusReportEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricE2eMcStatusReportEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable then was set: %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricE2eMcStatusReportEnableSet(dev, enableGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricE2eMcStatusReportEnableSet(dev, enableGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eMcStatusReportEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eMcStatusReportEnableGet)
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
        st = cpssExMxPmFabricE2eMcStatusReportEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricE2eMcStatusReportEnableGet(dev, NULL);
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
        st = cpssExMxPmFabricE2eMcStatusReportEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricE2eMcStatusReportEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eVoqDropEnableSet
(
    IN GT_U8   devNum,
    IN GT_U32  voqUnitNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eVoqDropEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1], enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricE2eVoqDropEnableGet with voqUnitNum and non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.3. Call with voqUnitNum [2].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32  voqUnitNum = 0;
    GT_BOOL enable = GT_FALSE;
    GT_BOOL enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1], enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */
        /* iterate with voqUnitNum = 0 */
        voqUnitNum = 0;
        enable = GT_TRUE;

        st = cpssExMxPmFabricE2eVoqDropEnableSet(dev, voqUnitNum, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, enable);

        /*
            1.2. Call cpssExMxPmFabricE2eVoqDropEnableGet with voqUnitNum and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmFabricE2eVoqDropEnableGet(dev, voqUnitNum, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricE2eVoqDropEnableGet: %d, %d", dev, voqUnitNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);

        /* iterate with voqUnitNum = 1 */
        voqUnitNum = 1;
        enable = GT_FALSE;

        st = cpssExMxPmFabricE2eVoqDropEnableSet(dev, voqUnitNum, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, enable);

        /*
            1.2. Call cpssExMxPmFabricE2eVoqDropEnableGet with voqUnitNum and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmFabricE2eVoqDropEnableGet(dev, voqUnitNum, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricE2eVoqDropEnableGet: %d, %d", dev, voqUnitNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);

        /*
            1.3. Call with voqUnitNum [2].
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricE2eVoqDropEnableSet(dev, voqUnitNum, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, enable);
    }

    voqUnitNum = 0;
    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricE2eVoqDropEnableSet(dev, voqUnitNum, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricE2eVoqDropEnableSet(dev, voqUnitNum, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eVoqDropEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   voqUnitNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eVoqDropEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1], not NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with voqUnitNum [2] (out of range).
    Expected: NOT GT_OK.
    1.3. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32  voqUnitNum = 0;
    GT_BOOL enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. Call with voqUnitNum [0 / 1], not NULL enablePtr.
            Expected: GT_OK.
        */
        /* iterate with voqUnitNum = 0 */
        voqUnitNum = 0;

        st = cpssExMxPmFabricE2eVoqDropEnableGet(dev, voqUnitNum, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /* iterate with voqUnitNum = 1 */
        voqUnitNum = 1;

        st = cpssExMxPmFabricE2eVoqDropEnableGet(dev, voqUnitNum, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. Call with voqUnitNum [2] (out of range).
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricE2eVoqDropEnableGet(dev, voqUnitNum, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 1;

        /*
            1.3. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricE2eVoqDropEnableGet(dev, voqUnitNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enable = NULL", dev);
    }

    voqUnitNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricE2eVoqDropEnableGet(dev, voqUnitNum, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricE2eVoqDropEnableGet(dev, voqUnitNum, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eScoreBoardDumpGet
(
    IN  GT_U8                                             devNum,
    IN  GT_U32                                            voqUnitNum,
    IN  GT_U32                                            startEntryIndex,
    IN  GT_U32                                            entryArrSize,
    OUT CPSS_EXMXPM_FABRIC_E2E_PORT_TC_STATUS_ENTRY_STC   *entryArrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eScoreBoardDumpGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1], startEntryIndex [1 / 5], entryArrSize [1 / 10], not NULL entryArrPtr.
    Expected: GT_OK.
    1.2. Call with voqUnitNum [2] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with entryArrPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    GT_U32      startEntryIndex = 0;
    GT_U32      entryArrSize = 0;
    CPSS_EXMXPM_FABRIC_E2E_PORT_TC_STATUS_ENTRY_STC   entryArr [10];

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1], 
                           startEntryIndex [1 / 5], 
                           entryArrSize [1 / 10], not NULL entryArrPtr.
            Expected: GT_OK.
        */
        /* iterate with voqUnitNum = 0 */
        voqUnitNum = 0;
        startEntryIndex = 1;
        entryArrSize = 1;

        st = cpssExMxPmFabricE2eScoreBoardDumpGet(dev, voqUnitNum, startEntryIndex, entryArrSize, entryArr);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqUnitNum, startEntryIndex, entryArrSize);

        /* iterate with voqUnitNum = 0 */
        voqUnitNum = 1;
        startEntryIndex = 5;
        entryArrSize = 10;

        st = cpssExMxPmFabricE2eScoreBoardDumpGet(dev, voqUnitNum, startEntryIndex, entryArrSize, entryArr);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqUnitNum, startEntryIndex, entryArrSize);

        /*
            1.2. Call with voqUnitNum [2] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricE2eScoreBoardDumpGet(dev, voqUnitNum, startEntryIndex, entryArrSize, entryArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with entryArrPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricE2eScoreBoardDumpGet(dev, voqUnitNum, startEntryIndex, entryArrSize, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryArr = NULL", dev);
    }

    voqUnitNum = 0;
    startEntryIndex = 1;
    entryArrSize = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[overrun-buffer-val] */
        st = cpssExMxPmFabricE2eScoreBoardDumpGet(dev, voqUnitNum, startEntryIndex, entryArrSize, entryArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricE2eScoreBoardDumpGet(dev, voqUnitNum, startEntryIndex, entryArrSize, entryArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eDevConfigSet
(
    IN  GT_U8                                         devNum,
    IN  GT_U32                                        voqUnitNum,
    IN  GT_U8                                         targetDevNum,
    IN  CPSS_EXMXPM_FABRIC_E2E_TARGET_DEV_CONFIG_STC  *targetDevCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eDevConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1], 
                   targetDevNum [0 / dev]
                   targetDevCfg { devPortsOffset [0 / 8191],
                                  devStatusMsgProcessedSize [CPSS_EXMXPM_FABRIC_E2E_STATUS_PROCESSED_SIZE_16B_E,
                                                             CPSS_EXMXPM_FABRIC_E2E_STATUS_PROCESSED_SIZE_32B_E,
                                                             CPSS_EXMXPM_FABRIC_E2E_STATUS_PROCESSED_SIZE_48B_E],
                                  devMaxPortId [0 / 63] }
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricE2eDevConfigGet with not NULL targetDevCfgPtr and other params from 1.1.
    Expected: GT_OK and the same targetDevCfg as was set.
    1.3. Call with voqUnitNum [2] (out of range).
    Expected: NOT GT_OK.
    1.4. Call with targetDevCfg->devPortsOffset [8192] (out of range).
    Expected: NOT GT_OK.
    1.5. Call with targetDevCfg->devStatusMsgProcessedSize [0x5AAAAAA5] (out of range).
    Expected: GT_BAD_PARAM.
    1.6. Call with targetDevCfg->devMaxPortId [64] (out of range).
    Expected: NOT GT_OK.
    1.7. Call with targetDevCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                        voqUnitNum = 0;
    GT_U8                                         targetDevNum = 0;
    CPSS_EXMXPM_FABRIC_E2E_TARGET_DEV_CONFIG_STC  targetDevCfg;
    CPSS_EXMXPM_FABRIC_E2E_TARGET_DEV_CONFIG_STC  targetDevCfgGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1], 
                           targetDevNum [0 / dev]
                           targetDevCfg { devPortsOffset [0 / 8191],
                                          devStatusMsgProcessedSize [CPSS_EXMXPM_FABRIC_E2E_STATUS_PROCESSED_SIZE_16B_E / 
                                                                     CPSS_EXMXPM_FABRIC_E2E_STATUS_PROCESSED_SIZE_48B_E],
                                          devMaxPortId [0 / 63] }
            Expected: GT_OK.
        */
        /* iterate with voqUnitNum = 0 */
        voqUnitNum = 0;
        targetDevNum = 0;
        targetDevCfg.devPortsOffset = 0;
        targetDevCfg.devStatusMsgProcessedSize = CPSS_EXMXPM_FABRIC_E2E_STATUS_PROCESSED_SIZE_16B_E;
        targetDevCfg.devMaxPortId = 0;

        st = cpssExMxPmFabricE2eDevConfigSet(dev, voqUnitNum, targetDevNum, &targetDevCfg);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, targetDevNum);

        /*
            1.2. Call cpssExMxPmFabricE2eDevConfigGet with not NULL targetDevCfgPtr and other params from 1.1.
            Expected: GT_OK and the same targetDevCfg as was set.
        */
        st = cpssExMxPmFabricE2eDevConfigGet(dev, voqUnitNum, targetDevNum, &targetDevCfgGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmFabricE2eDevConfigGet: %d, %d, %d, ", dev, voqUnitNum, targetDevNum);

        UTF_VERIFY_EQUAL1_STRING_MAC(targetDevCfg.devPortsOffset, targetDevCfgGet.devPortsOffset, 
                                     "got another targetDevCfg.devPortsOffset than was set: %d, ", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(targetDevCfg.devStatusMsgProcessedSize, targetDevCfgGet.devStatusMsgProcessedSize, 
                                     "got another targetDevCfg.devStatusMsgProcessedSize than was set: %d, ", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(targetDevCfg.devMaxPortId, targetDevCfgGet.devMaxPortId, 
                                     "got another targetDevCfg.devMaxPortId than was set: %d, ", dev);

        /* iterate with voqUnitNum = 1 */
        voqUnitNum = 1;
        targetDevNum = dev;
        targetDevCfg.devPortsOffset = 4095;
        targetDevCfg.devStatusMsgProcessedSize = CPSS_EXMXPM_FABRIC_E2E_STATUS_PROCESSED_SIZE_48B_E;
        targetDevCfg.devMaxPortId = 63;

        st = cpssExMxPmFabricE2eDevConfigSet(dev, voqUnitNum, targetDevNum, &targetDevCfg);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, targetDevNum);

        /*
            1.2. Call cpssExMxPmFabricE2eDevConfigGet with not NULL targetDevCfgPtr and other params from 1.1.
            Expected: GT_OK and the same targetDevCfg as was set.
        */
        st = cpssExMxPmFabricE2eDevConfigGet(dev, voqUnitNum, targetDevNum, &targetDevCfgGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmFabricE2eDevConfigGet: %d, %d, %d, ", dev, voqUnitNum, targetDevNum);

        UTF_VERIFY_EQUAL1_STRING_MAC(targetDevCfg.devPortsOffset, targetDevCfgGet.devPortsOffset, 
                                     "got another targetDevCfg.devPortsOffset than was set: %d, ", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(targetDevCfg.devStatusMsgProcessedSize, targetDevCfgGet.devStatusMsgProcessedSize, 
                                     "got another targetDevCfg.devStatusMsgProcessedSize than was set: %d, ", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(targetDevCfg.devMaxPortId, targetDevCfgGet.devMaxPortId, 
                                     "got another targetDevCfg.devMaxPortId than was set: %d, ", dev);

        /*
            1.3. Call with voqUnitNum [2] (out of range).
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricE2eDevConfigSet(dev, voqUnitNum, targetDevNum, &targetDevCfg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with targetDevCfg->devPortsOffset [8192] (out of range).
            Expected: NOT GT_OK.
        */
        targetDevCfg.devPortsOffset = 4096;

        st = cpssExMxPmFabricE2eDevConfigSet(dev, voqUnitNum, targetDevNum, &targetDevCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, targetDevCfg.devPortsOffset = %d", 
                                         dev, targetDevCfg.devPortsOffset);

        targetDevCfg.devPortsOffset = 0;

        /*
            1.5. Call with targetDevCfg->devStatusMsgProcessedSize [0x5AAAAAA5] (out of range).
            Expected: GT_BAD_PARAM.
        */
        targetDevCfg.devStatusMsgProcessedSize = FABRIC_E2E_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricE2eDevConfigSet(dev, voqUnitNum, targetDevNum, &targetDevCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, targetDevCfg.devStatusMsgProcessedSize = %d", 
                                         dev, targetDevCfg.devStatusMsgProcessedSize);

        targetDevCfg.devStatusMsgProcessedSize = CPSS_EXMXPM_FABRIC_E2E_STATUS_PROCESSED_SIZE_16B_E;

        /*
            1.6. Call with targetDevCfg->devMaxPortId [64] (out of range).
            Expected: NOT GT_OK.
        */
        targetDevCfg.devMaxPortId = 64;

        st = cpssExMxPmFabricE2eDevConfigSet(dev, voqUnitNum, targetDevNum, &targetDevCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, targetDevCfg.devMaxPortId = %d", 
                                         dev, targetDevCfg.devMaxPortId);
        /*
            1.7. Call with targetDevCfgPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricE2eDevConfigSet(dev, voqUnitNum, targetDevNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, targetDevCfg = NULL", dev);
    }

    voqUnitNum = 0;
    targetDevNum = 0;
    targetDevCfg.devPortsOffset = 0;
    targetDevCfg.devStatusMsgProcessedSize = CPSS_EXMXPM_FABRIC_E2E_STATUS_PROCESSED_SIZE_16B_E;
    targetDevCfg.devMaxPortId = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricE2eDevConfigSet(dev, voqUnitNum, targetDevNum, &targetDevCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricE2eDevConfigSet(dev, voqUnitNum, targetDevNum, &targetDevCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eDevConfigGet
(
    IN  GT_U8                                         devNum,
    IN  GT_U32                                        voqUnitNum,
    IN  GT_U8                                         targetDevNum,
    OUT CPSS_EXMXPM_FABRIC_E2E_TARGET_DEV_CONFIG_STC  *targetDevCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eDevConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1], 
                   targetDevNum [0 / dev]
                   not NULL targetDevCfgPtr.
    Expected: GT_OK.
    1.2. Call with voqUnitNum [2] (out of range).
    Expected: NOT GT_OK.
    1.3. Call with targetDevCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    GT_U8       targetDevNum = 0;
    CPSS_EXMXPM_FABRIC_E2E_TARGET_DEV_CONFIG_STC targetDevCfg;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1], 
                           targetDevNum [0 / dev]
                           not NULL targetDevCfgPtr.
            Expected: GT_OK.
        */
        /* iterate with voqUnitNum = 0 */
        voqUnitNum = 0;
        targetDevNum = 0;

        st = cpssExMxPmFabricE2eDevConfigGet(dev, voqUnitNum, targetDevNum, &targetDevCfg);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, targetDevNum);
        
        /* iterate with voqUnitNum = 1 */
        voqUnitNum = 1;
        targetDevNum = dev;

        st = cpssExMxPmFabricE2eDevConfigGet(dev, voqUnitNum, targetDevNum, &targetDevCfg);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, targetDevNum);

        /*
            1.2. Call with voqUnitNum [2] (out of range).
            Expected: NOT GT_OK.
        */
        voqUnitNum = 1;

        st = cpssExMxPmFabricE2eDevConfigGet(dev, voqUnitNum, targetDevNum, &targetDevCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 2;

        /*
            1.3. Call with targetDevCfgPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricE2eDevConfigGet(dev, voqUnitNum, targetDevNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, targetDevCfg = NULL", dev);
    }

    voqUnitNum = 0;
    targetDevNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricE2eDevConfigGet(dev, voqUnitNum, targetDevNum, &targetDevCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricE2eDevConfigGet(dev, voqUnitNum, targetDevNum, &targetDevCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eDropProbabilitySet
(
    IN  GT_U8                                        devNum,
    IN  GT_U32                                       voqUnitNum,
    IN  CPSS_EXMXPM_FABRIC_E2E_DROP_PROBABILITY_STC  *e2eDropProbabilityPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eDropProbabilitySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1], 
                   e2eDropProbabilityPtr { state1Dp0DropProbability [0 / 255],
                                           state1Dp1DropProbability [0 / 255],
                                           state1Dp2DropProbability [0 / 255],
                                           state2Dp0DropProbability [0 / 255],
                                           state2Dp1DropProbability [0 / 255],
                                           state2Dp2DropProbability [0 / 255] }
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricE2eDropProbabilityGet with not NULL e2eDropProbabilityPtr and other params from 1.1.
    Expected: GT_OK and the same e2eDropProbability as was set.
    1.3. Call with voqUnitNum [2] (out of range).
    Expected: NOT GT_OK.
    1.4. Call with e2eDropProbabilityPtr->state1Dp0DropProbability [256] (out of range).
    Expected: NOT GT_OK.
    1.5. Call with e2eDropProbabilityPtr->state1Dp1DropProbability [256] (out of range).
    Expected: NOT GT_OK.
    1.6. Call with e2eDropProbabilityPtr->state1Dp2DropProbability [256] (out of range).
    Expected: NOT GT_OK.
    1.7. Call with e2eDropProbabilityPtr->state2Dp0DropProbability [256] (out of range).
    Expected: NOT GT_OK.
    1.8. Call with e2eDropProbabilityPtr->state2Dp1DropProbability [256] (out of range).
    Expected: NOT GT_OK.
    1.9. Call with e2eDropProbabilityPtr->state2Dp2DropProbability [256] (out of range).
    Expected: NOT GT_OK.
    1.10. Call with e2eDropProbabilityPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    CPSS_EXMXPM_FABRIC_E2E_DROP_PROBABILITY_STC  e2eDropProbability;
    CPSS_EXMXPM_FABRIC_E2E_DROP_PROBABILITY_STC  e2eDropProbabilityGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1], 
                           e2eDropProbabilityPtr { state1Dp0DropProbability [0 / 255],
                                                   state1Dp1DropProbability [0 / 255],
                                                   state1Dp2DropProbability [0 / 255],
                                                   state2Dp0DropProbability [0 / 255],
                                                   state2Dp1DropProbability [0 / 255],
                                                   state2Dp2DropProbability [0 / 255] }
            Expected: GT_OK.
        */
        /* itertate with voqUnitNum = 0  */
        voqUnitNum = 0;
        e2eDropProbability.state1Dp0DropProbability = 0;
        e2eDropProbability.state1Dp1DropProbability = 0;
        e2eDropProbability.state1Dp2DropProbability = 0;
        e2eDropProbability.state2Dp0DropProbability = 0;
        e2eDropProbability.state2Dp1DropProbability = 0;
        e2eDropProbability.state2Dp2DropProbability = 0;

        st = cpssExMxPmFabricE2eDropProbabilitySet(dev, voqUnitNum, &e2eDropProbability);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. Call cpssExMxPmFabricE2eDropProbabilityGet with not NULL e2eDropProbabilityPtr and other params from 1.1.
            Expected: GT_OK and the same e2eDropProbability as was set.
        */
        st = cpssExMxPmFabricE2eDropProbabilityGet(dev, voqUnitNum, &e2eDropProbabilityGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricE2eDropProbabilityGet: %d, %d" ,dev, voqUnitNum);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(e2eDropProbability.state1Dp0DropProbability, 
                                     e2eDropProbabilityGet.state1Dp0DropProbability, 
                                     "got another e2eDropProbability.state1Dp0DropProbability than was set: %d" , dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(e2eDropProbability.state1Dp1DropProbability, 
                                     e2eDropProbabilityGet.state1Dp1DropProbability, 
                                     "got another e2eDropProbability.state1Dp1DropProbability than was set: %d" , dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(e2eDropProbability.state1Dp2DropProbability, 
                                     e2eDropProbabilityGet.state1Dp2DropProbability, 
                                     "got another e2eDropProbability.state1Dp2DropProbability than was set: %d" , dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(e2eDropProbability.state2Dp0DropProbability, 
                                     e2eDropProbabilityGet.state2Dp0DropProbability, 
                                     "got another e2eDropProbability.state2Dp0DropProbability than was set: %d" , dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(e2eDropProbability.state2Dp1DropProbability, 
                                     e2eDropProbabilityGet.state2Dp1DropProbability, 
                                     "got another e2eDropProbability.state2Dp1DropProbability than was set: %d" , dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(e2eDropProbability.state2Dp2DropProbability, 
                                     e2eDropProbabilityGet.state2Dp2DropProbability, 
                                     "got another e2eDropProbability.state2Dp2DropProbability than was set: %d" , dev);

        /* itertate with voqUnitNum = 1*/
        voqUnitNum = 1;
        e2eDropProbability.state1Dp0DropProbability = 255;
        e2eDropProbability.state1Dp1DropProbability = 255;
        e2eDropProbability.state1Dp2DropProbability = 255;
        e2eDropProbability.state2Dp0DropProbability = 255;
        e2eDropProbability.state2Dp1DropProbability = 255;
        e2eDropProbability.state2Dp2DropProbability = 255;

        st = cpssExMxPmFabricE2eDropProbabilitySet(dev, voqUnitNum, &e2eDropProbability);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. Call cpssExMxPmFabricE2eDropProbabilityGet with not NULL e2eDropProbabilityPtr and other params from 1.1.
            Expected: GT_OK and the same e2eDropProbability as was set.
        */
        st = cpssExMxPmFabricE2eDropProbabilityGet(dev, voqUnitNum, &e2eDropProbabilityGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricE2eDropProbabilityGet: %d, %d" ,dev, voqUnitNum);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(e2eDropProbability.state1Dp0DropProbability, 
                                     e2eDropProbabilityGet.state1Dp0DropProbability, 
                                     "got another e2eDropProbability.state1Dp0DropProbability than was set: %d" , dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(e2eDropProbability.state1Dp1DropProbability, 
                                     e2eDropProbabilityGet.state1Dp1DropProbability, 
                                     "got another e2eDropProbability.state1Dp1DropProbability than was set: %d" , dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(e2eDropProbability.state1Dp2DropProbability, 
                                     e2eDropProbabilityGet.state1Dp2DropProbability, 
                                     "got another e2eDropProbability.state1Dp2DropProbability than was set: %d" , dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(e2eDropProbability.state2Dp0DropProbability, 
                                     e2eDropProbabilityGet.state2Dp0DropProbability, 
                                     "got another e2eDropProbability.state2Dp0DropProbability than was set: %d" , dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(e2eDropProbability.state2Dp1DropProbability, 
                                     e2eDropProbabilityGet.state2Dp1DropProbability, 
                                     "got another e2eDropProbability.state2Dp1DropProbability than was set: %d" , dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(e2eDropProbability.state2Dp2DropProbability, 
                                     e2eDropProbabilityGet.state2Dp2DropProbability, 
                                     "got another e2eDropProbability.state2Dp2DropProbability than was set: %d" , dev);

        /*
            1.3. Call with voqUnitNum [2] (out of range).
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricE2eDropProbabilitySet(dev, voqUnitNum, &e2eDropProbability);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with e2eDropProbabilityPtr->state1Dp0DropProbability [256] (out of range).
            Expected: NOT GT_OK.
        */
        e2eDropProbability.state1Dp0DropProbability = 256;

        st = cpssExMxPmFabricE2eDropProbabilitySet(dev, voqUnitNum, &e2eDropProbability);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, e2eDropProbability.state1Dp0DropProbability = %d", 
                                         dev, e2eDropProbability.state1Dp0DropProbability);

        e2eDropProbability.state1Dp0DropProbability = 0;

        /*
            1.5. Call with e2eDropProbabilityPtr->state1Dp1DropProbability [256] (out of range).
            Expected: NOT GT_OK.
        */
        e2eDropProbability.state1Dp1DropProbability = 256;

        st = cpssExMxPmFabricE2eDropProbabilitySet(dev, voqUnitNum, &e2eDropProbability);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, e2eDropProbability.state1Dp1DropProbability = %d", 
                                         dev, e2eDropProbability.state1Dp1DropProbability);

        e2eDropProbability.state1Dp1DropProbability = 0;

        /*
            1.6. Call with e2eDropProbabilityPtr->state1Dp2DropProbability [256] (out of range).
            Expected: NOT GT_OK.
        */
        e2eDropProbability.state1Dp2DropProbability = 256;

        st = cpssExMxPmFabricE2eDropProbabilitySet(dev, voqUnitNum, &e2eDropProbability);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, e2eDropProbability.state1Dp2DropProbability = %d", 
                                         dev, e2eDropProbability.state1Dp2DropProbability);

        e2eDropProbability.state1Dp2DropProbability = 0;

        /*
            1.7. Call with e2eDropProbabilityPtr->state2Dp0DropProbability [256] (out of range).
            Expected: NOT GT_OK.
        */
        e2eDropProbability.state2Dp0DropProbability = 256;

        st = cpssExMxPmFabricE2eDropProbabilitySet(dev, voqUnitNum, &e2eDropProbability);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, e2eDropProbability.state2Dp0DropProbability = %d", 
                                         dev, e2eDropProbability.state2Dp0DropProbability);

        e2eDropProbability.state2Dp0DropProbability = 0;

        /*
            1.8. Call with e2eDropProbabilityPtr->state2Dp1DropProbability [256] (out of range).
            Expected: NOT GT_OK.
        */
        e2eDropProbability.state2Dp1DropProbability = 256;

        st = cpssExMxPmFabricE2eDropProbabilitySet(dev, voqUnitNum, &e2eDropProbability);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, e2eDropProbability.state2Dp1DropProbability = %d", 
                                         dev, e2eDropProbability.state2Dp1DropProbability);

        e2eDropProbability.state2Dp1DropProbability = 0;

        /*
            1.9. Call with e2eDropProbabilityPtr->state2Dp2DropProbability [256] (out of range).
            Expected: NOT GT_OK.
        */
        e2eDropProbability.state2Dp2DropProbability = 256;

        st = cpssExMxPmFabricE2eDropProbabilitySet(dev, voqUnitNum, &e2eDropProbability);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, e2eDropProbability.state2Dp2DropProbability = %d", 
                                         dev, e2eDropProbability.state2Dp2DropProbability);

        e2eDropProbability.state2Dp2DropProbability = 0;

        /*
            1.10. Call with e2eDropProbabilityPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricE2eDropProbabilitySet(dev, voqUnitNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, e2eDropProbabilityPtr = NULL", dev);
    }

    voqUnitNum = 0;
    e2eDropProbability.state1Dp0DropProbability = 0;
    e2eDropProbability.state1Dp1DropProbability = 0;
    e2eDropProbability.state1Dp2DropProbability = 0;
    e2eDropProbability.state2Dp0DropProbability = 0;
    e2eDropProbability.state2Dp1DropProbability = 0;
    e2eDropProbability.state2Dp2DropProbability = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricE2eDropProbabilitySet(dev, voqUnitNum, &e2eDropProbability);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricE2eDropProbabilitySet(dev, voqUnitNum, &e2eDropProbability);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricE2eDropProbabilityGet
(
    IN  GT_U8                                        devNum,
    IN  GT_U32                                       voqUnitNum,
    OUT CPSS_EXMXPM_FABRIC_E2E_DROP_PROBABILITY_STC  *e2eDropProbabilityPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricE2eDropProbabilityGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1], 
                   not NULL e2eDropProbabilityPtr.
    Expected: GT_OK.
    1.2. Call with voqUnitNum [2] (out of range).
    Expected: NOT GT_OK.
    1.3. Call with e2eDropProbabilityPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    CPSS_EXMXPM_FABRIC_E2E_DROP_PROBABILITY_STC  e2eDropProbability;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1], 
                           not NULL e2eDropProbabilityPtr.
            Expected: GT_OK.
        */
        /* iterate with voqUnitNum = 0 */
        voqUnitNum = 0;

        st = cpssExMxPmFabricE2eDropProbabilityGet(dev, voqUnitNum, &e2eDropProbability);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /* iterate with voqUnitNum = 1 */
        voqUnitNum = 1;

        st = cpssExMxPmFabricE2eDropProbabilityGet(dev, voqUnitNum, &e2eDropProbability);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. Call with voqUnitNum [2] (out of range).
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricE2eDropProbabilityGet(dev, voqUnitNum, &e2eDropProbability);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with e2eDropProbabilityPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricE2eDropProbabilityGet(dev, voqUnitNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, e2eDropProbabilityPtr = NULL", dev);
    }

    voqUnitNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricE2eDropProbabilityGet(dev, voqUnitNum, &e2eDropProbability);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricE2eDropProbabilityGet(dev, voqUnitNum, &e2eDropProbability);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmFabricE2e suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmFabricE2e)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eAdvertiseEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eAdvertiseEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eMsgConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eMsgConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eMsgPathSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eMsgPathGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eMcStatusReportEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eMcStatusReportEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eVoqDropEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eVoqDropEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eScoreBoardDumpGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eDevConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eDevConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eDropProbabilitySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricE2eDropProbabilityGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmFabricE2e)

