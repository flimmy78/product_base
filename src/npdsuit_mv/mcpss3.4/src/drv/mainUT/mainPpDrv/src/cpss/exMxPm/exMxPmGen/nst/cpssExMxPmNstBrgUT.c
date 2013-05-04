/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmNstBrgUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmNstBrg, that provides
*        CPSS EXMXPM Bridge Network Shield Technology facility API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/nst/cpssExMxPmNstBrg.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define NST_BRG_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define NST_BRG_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstBrgAccessMatrixCmdSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      saAccessLevel,
    IN  GT_U32                      daAccessLevel,
    IN  CPSS_PACKET_CMD_ENT         command
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstBrgAccessMatrixCmdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with saAccessLevel[0 / 15],
                   daAccessLevel[5 / 15]
                   and command [CPSS_PACKET_CMD_FORWARD_E /
                                CPSS_PACKET_CMD_DROP_HARD_E /
                                CPSS_PACKET_CMD_DROP_SOFT_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmNstBrgAccessMatrixCmdGet with the same saAccessLevel, daAccessLevel and non-NULL commandPtr.
    Expected: GT_OK and the same command.
    1.3. Call with out of range saAccessLevel [16]
                   and other parameters from 1.1.
    Expected: NON GT_OK.
    1.4. Call with out of range daAccessLevel[16]
                   and other parameters from 1.1.
    Expected: NON GT_OK.
    1.5. Call with command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                            CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                            CPSS_PACKET_CMD_ROUTE_E] (not supported)
                   and other parameters from 1.1.
    Expected: NON GT_OK.
    1.6. Call with out of range command [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                  saAccessLevel = 0;
    GT_U32                  daAccessLevel = 0;
    CPSS_PACKET_CMD_ENT     command       = CPSS_PACKET_CMD_FORWARD_E;
    CPSS_PACKET_CMD_ENT     commandGet    = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with saAccessLevel[0 / 15],
                           daAccessLevel[5 / 15]
                           and command [CPSS_PACKET_CMD_FORWARD_E /
                                        CPSS_PACKET_CMD_DROP_HARD_E /
                                        CPSS_PACKET_CMD_DROP_SOFT_E].
            Expected: GT_OK.
        */

        /* Call with command [CPSS_PACKET_CMD_FORWARD_E] */
        saAccessLevel = 0;
        daAccessLevel = 5;
        command       = CPSS_PACKET_CMD_FORWARD_E;
        
        st = cpssExMxPmNstBrgAccessMatrixCmdSet(dev, saAccessLevel, daAccessLevel, command);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, saAccessLevel, daAccessLevel, command);

        /*
            1.2. Call cpssExMxPmNstBrgAccessMatrixCmdGet with the same saAccessLevel, daAccessLevel and non-NULL commandPtr.
            Expected: GT_OK and the same command.
        */
        st = cpssExMxPmNstBrgAccessMatrixCmdGet(dev, saAccessLevel, daAccessLevel, &commandGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstBrgAccessMatrixCmdGet: %d, %d, %d", dev, saAccessLevel, daAccessLevel);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet,
                       "get another command than was set: %d", dev);

        /* Call with command [CPSS_PACKET_CMD_DROP_HARD_E] */
        saAccessLevel = 15;
        daAccessLevel = 15;
        command       = CPSS_PACKET_CMD_DROP_HARD_E;
        
        st = cpssExMxPmNstBrgAccessMatrixCmdSet(dev, saAccessLevel, daAccessLevel, command);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, saAccessLevel, daAccessLevel, command);

        /*
            1.2. Call cpssExMxPmNstBrgAccessMatrixCmdGet with the same saAccessLevel, daAccessLevel and non-NULL commandPtr.
            Expected: GT_OK and the same command.
        */
        st = cpssExMxPmNstBrgAccessMatrixCmdGet(dev, saAccessLevel, daAccessLevel, &commandGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstBrgAccessMatrixCmdGet: %d, %d, %d", dev, saAccessLevel, daAccessLevel);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet,
                       "get another command than was set: %d", dev);

        /* Call with command [CPSS_PACKET_CMD_DROP_SOFT_E] */
        command = CPSS_PACKET_CMD_DROP_SOFT_E;
        
        st = cpssExMxPmNstBrgAccessMatrixCmdSet(dev, saAccessLevel, daAccessLevel, command);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, saAccessLevel, daAccessLevel, command);

        /*
            1.2. Call cpssExMxPmNstBrgAccessMatrixCmdGet with the same saAccessLevel, daAccessLevel and non-NULL commandPtr.
            Expected: GT_OK and the same command.
        */
        st = cpssExMxPmNstBrgAccessMatrixCmdGet(dev, saAccessLevel, daAccessLevel, &commandGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstBrgAccessMatrixCmdGet: %d, %d, %d", dev, saAccessLevel, daAccessLevel);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet,
                       "get another command than was set: %d", dev);

        /*
            1.3. Call with out of range saAccessLevel [16]
                           and other parameters from 1.1.
            Expected: NON GT_OK.
        */
        saAccessLevel = 16;

        st = cpssExMxPmNstBrgAccessMatrixCmdSet(dev, saAccessLevel, daAccessLevel, command);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, saAccessLevel);

        saAccessLevel = 0;

        /*
            1.4. Call with out of range daAccessLevel[16]
                           and other parameters from 1.1.
            Expected: NON GT_OK.
        */
        daAccessLevel = 16;

        st = cpssExMxPmNstBrgAccessMatrixCmdSet(dev, saAccessLevel, daAccessLevel, command);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, daAccessLevel = %d", dev, daAccessLevel);

        daAccessLevel = 0;

        /*
            1.5. Call with command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                                    CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                    CPSS_PACKET_CMD_ROUTE_E] (not supported)
                           and other parameters from 1.1.
            Expected: NON GT_OK.
        */

        /* Call with command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E] */
        command = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmNstBrgAccessMatrixCmdSet(dev, saAccessLevel, daAccessLevel, command);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, command = %d", dev, command);

        /* Call with command [CPSS_PACKET_CMD_TRAP_TO_CPU_E] */
        command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmNstBrgAccessMatrixCmdSet(dev, saAccessLevel, daAccessLevel, command);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, command = %d", dev, command);

        /* Call with command [CPSS_PACKET_CMD_ROUTE_E] */
        command = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmNstBrgAccessMatrixCmdSet(dev, saAccessLevel, daAccessLevel, command);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, command = %d", dev, command);

        /*
            1.6. Call with out of range command [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        command = NST_BRG_INVALID_ENUM_CNS;

        st = cpssExMxPmNstBrgAccessMatrixCmdSet(dev, saAccessLevel, daAccessLevel, command);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, command = %d", dev, command);
    }

    saAccessLevel = 0;
    daAccessLevel = 5;
    command       = CPSS_PACKET_CMD_FORWARD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstBrgAccessMatrixCmdSet(dev, saAccessLevel, daAccessLevel, command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNstBrgAccessMatrixCmdSet(dev, saAccessLevel, daAccessLevel, command);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstBrgAccessMatrixCmdGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      saAccessLevel,
    IN  GT_U32                      daAccessLevel,
    OUT CPSS_PACKET_CMD_ENT         *commandPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstBrgAccessMatrixCmdGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with saAccessLevel[15],
                   daAccessLevel[0]
                   and non-null commandPtr.
    Expected: GT_OK.
    1.2. Call with out of range saAccessLevel[16]
                   and other parameters from 1.1.
    Expected: NON GT_OK.
    1.3. Call with out of range daAccessLevel[16]
                   and other parameters from 1.1.
    Expected: NON GT_OK.
    1.4. Call with commandPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                  saAccessLevel = 0;
    GT_U32                  daAccessLevel = 0;
    CPSS_PACKET_CMD_ENT     command       = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with saAccessLevel[15],
                           daAccessLevel[0]
                           and non-null commandPtr.
            Expected: GT_OK.
        */
        saAccessLevel = 15;
        daAccessLevel = 0;
        
        st = cpssExMxPmNstBrgAccessMatrixCmdGet(dev, saAccessLevel, daAccessLevel, &command);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, saAccessLevel, daAccessLevel);

        /*
            1.2. Call with out of range saAccessLevel [16]
                           and other parameters from 1.1.
            Expected: NON GT_OK.
        */
        saAccessLevel = 16;

        st = cpssExMxPmNstBrgAccessMatrixCmdGet(dev, saAccessLevel, daAccessLevel, &command);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, saAccessLevel);

        saAccessLevel = 0;

        /*
            1.3. Call with out of range daAccessLevel[16]
                           and other parameters from 1.1.
            Expected: NON GT_OK.
        */
        daAccessLevel = 16;

        st = cpssExMxPmNstBrgAccessMatrixCmdGet(dev, saAccessLevel, daAccessLevel, &command);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, daAccessLevel = %d", dev, daAccessLevel);

        daAccessLevel = 0;

        /*
            1.4. Call with commandPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNstBrgAccessMatrixCmdGet(dev, saAccessLevel, daAccessLevel, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, commandPtr = %d", dev);
    }

    saAccessLevel = 0;
    daAccessLevel = 5;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstBrgAccessMatrixCmdGet(dev, saAccessLevel, daAccessLevel, &command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNstBrgAccessMatrixCmdGet(dev, saAccessLevel, daAccessLevel, &command);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstBrgPortAccessMatrixEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  GT_BOOL                     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstBrgPortAccessMatrixEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmNstBrgPortAccessMatrixEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = NST_BRG_VALID_VIRT_PORT_CNS;

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
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmNstBrgPortAccessMatrixEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmNstBrgPortAccessMatrixEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmNstBrgPortAccessMatrixEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstBrgPortAccessMatrixEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            
            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmNstBrgPortAccessMatrixEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmNstBrgPortAccessMatrixEnableGet with non-NULL enablePtr. 
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmNstBrgPortAccessMatrixEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstBrgPortAccessMatrixEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmNstBrgPortAccessMatrixEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmNstBrgPortAccessMatrixEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmNstBrgPortAccessMatrixEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = NST_BRG_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstBrgPortAccessMatrixEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmNstBrgPortAccessMatrixEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstBrgPortAccessMatrixEnableGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    OUT GT_BOOL                     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstBrgPortAccessMatrixEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr. 
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = NST_BRG_VALID_VIRT_PORT_CNS;

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
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmNstBrgPortAccessMatrixEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmNstBrgPortAccessMatrixEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmNstBrgPortAccessMatrixEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmNstBrgPortAccessMatrixEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmNstBrgPortAccessMatrixEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = NST_BRG_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstBrgPortAccessMatrixEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmNstBrgPortAccessMatrixEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstBrgDefaultAccessLevelsSet
(
    IN GT_U8                    devNum,
    IN CPSS_NST_AM_PARAM_ENT    paramType,
    IN GT_U32                   accessLevel
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstBrgDefaultAccessLevelsSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with paramType [CPSS_NST_AM_SA_AUTO_LEARNED_E /
                              CPSS_NST_AM_DA_AUTO_LEARNED_E /
                              CPSS_NST_AM_SA_UNKNOWN_E /
                              CPSS_NST_AM_DA_UNKNOWN_E]
                   and accessLevel [0 / 2 / 10 / 15].
    Expected: GT_OK.
    1.2. Call cpssExMxPmNstBrgDefaultAccessLevelsGet with the same paramType and non-NULL accessLevelPtr.
    Expected: GT_OK and the same accessLevel.
    1.3. Call with out of range paramType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range accessLevel [16]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_NST_AM_PARAM_ENT    paramType      = CPSS_NST_AM_SA_AUTO_LEARNED_E;
    GT_U32                   accessLevel    = 0;
    GT_U32                   accessLevelGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with paramType [CPSS_NST_AM_SA_AUTO_LEARNED_E /
                                      CPSS_NST_AM_DA_AUTO_LEARNED_E /
                                      CPSS_NST_AM_SA_UNKNOWN_E /
                                      CPSS_NST_AM_DA_UNKNOWN_E]
                           and accessLevel [0 / 2 / 10 / 15].
            Expected: GT_OK.
        */

        /* Call with paramType [CPSS_NST_AM_SA_AUTO_LEARNED_E] */
        paramType   = CPSS_NST_AM_SA_AUTO_LEARNED_E;
        accessLevel = 0;
        
        st = cpssExMxPmNstBrgDefaultAccessLevelsSet(dev, paramType, accessLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, paramType, accessLevel);

        /*
            1.2. Call cpssExMxPmNstBrgDefaultAccessLevelsGet with the same paramType and non-NULL accessLevelPtr.
            Expected: GT_OK and the same accessLevel.
        */
        st = cpssExMxPmNstBrgDefaultAccessLevelsGet(dev, paramType, &accessLevelGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstBrgDefaultAccessLevelsGet: %d, %d", dev, paramType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(accessLevel, accessLevelGet,
                       "get another accessLevel than was set: %d", dev);

        /* Call with paramType [CPSS_NST_AM_DA_AUTO_LEARNED_E] */
        paramType   = CPSS_NST_AM_DA_AUTO_LEARNED_E;
        accessLevel = 0;
        
        st = cpssExMxPmNstBrgDefaultAccessLevelsSet(dev, paramType, accessLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, paramType, accessLevel);

        /*
            1.2. Call cpssExMxPmNstBrgDefaultAccessLevelsGet with the same paramType and non-NULL accessLevelPtr.
            Expected: GT_OK and the same accessLevel.
        */
        st = cpssExMxPmNstBrgDefaultAccessLevelsGet(dev, paramType, &accessLevelGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstBrgDefaultAccessLevelsGet: %d, %d", dev, paramType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(accessLevel, accessLevelGet,
                       "get another accessLevel than was set: %d", dev);

        /* Call with paramType [CPSS_NST_AM_SA_UNKNOWN_E] */
        paramType   = CPSS_NST_AM_SA_UNKNOWN_E;
        accessLevel = 0;
        
        st = cpssExMxPmNstBrgDefaultAccessLevelsSet(dev, paramType, accessLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, paramType, accessLevel);

        /*
            1.2. Call cpssExMxPmNstBrgDefaultAccessLevelsGet with the same paramType and non-NULL accessLevelPtr.
            Expected: GT_OK and the same accessLevel.
        */
        st = cpssExMxPmNstBrgDefaultAccessLevelsGet(dev, paramType, &accessLevelGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstBrgDefaultAccessLevelsGet: %d, %d", dev, paramType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(accessLevel, accessLevelGet,
                       "get another accessLevel than was set: %d", dev);

        /* Call with paramType [CPSS_NST_AM_DA_UNKNOWN_E] */
        paramType   = CPSS_NST_AM_DA_UNKNOWN_E;
        accessLevel = 0;
        
        st = cpssExMxPmNstBrgDefaultAccessLevelsSet(dev, paramType, accessLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, paramType, accessLevel);

        /*
            1.2. Call cpssExMxPmNstBrgDefaultAccessLevelsGet with the same paramType and non-NULL accessLevelPtr.
            Expected: GT_OK and the same accessLevel.
        */
        st = cpssExMxPmNstBrgDefaultAccessLevelsGet(dev, paramType, &accessLevelGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstBrgDefaultAccessLevelsGet: %d, %d", dev, paramType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(accessLevel, accessLevelGet,
                       "get another accessLevel than was set: %d", dev);

        /*
            1.3. Call with out of range paramType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        paramType = NST_BRG_INVALID_ENUM_CNS;

        st = cpssExMxPmNstBrgDefaultAccessLevelsSet(dev, paramType, accessLevel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, paramType);

        paramType = CPSS_NST_AM_SA_AUTO_LEARNED_E;

        /*
            1.4. Call with out of range accessLevel [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        accessLevel = 16;

        st = cpssExMxPmNstBrgDefaultAccessLevelsSet(dev, paramType, accessLevel);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, accessLevel = %d", dev, accessLevel);
    }

    paramType   = CPSS_NST_AM_SA_AUTO_LEARNED_E;
    accessLevel = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstBrgDefaultAccessLevelsSet(dev, paramType, accessLevel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNstBrgDefaultAccessLevelsSet(dev, paramType, accessLevel);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstBrgDefaultAccessLevelsGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_NST_AM_PARAM_ENT    paramType,
    OUT GT_U32                   *accessLevelPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstBrgDefaultAccessLevelsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with paramType [CPSS_NST_AM_SA_AUTO_LEARNED_E /
                              CPSS_NST_AM_DA_AUTO_LEARNED_E /
                              CPSS_NST_AM_SA_UNKNOWN_E /
                              CPSS_NST_AM_DA_UNKNOWN_E]
                   and non-NULL accessLevelPtr.
    Expected: GT_OK.
    1.2. Call with out of range paramType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with accessLevelPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_NST_AM_PARAM_ENT    paramType   = CPSS_NST_AM_SA_AUTO_LEARNED_E;
    GT_U32                   accessLevel = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with paramType [CPSS_NST_AM_SA_AUTO_LEARNED_E /
                                      CPSS_NST_AM_DA_AUTO_LEARNED_E /
                                      CPSS_NST_AM_SA_UNKNOWN_E /
                                      CPSS_NST_AM_DA_UNKNOWN_E]
                           and non-NULL accessLevelPtr.
            Expected: GT_OK.
        */

        /* Call with paramType [CPSS_NST_AM_SA_AUTO_LEARNED_E] */
        paramType   = CPSS_NST_AM_SA_AUTO_LEARNED_E;
        
        st = cpssExMxPmNstBrgDefaultAccessLevelsGet(dev, paramType, &accessLevel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, paramType);

        /* Call with paramType [CPSS_NST_AM_DA_AUTO_LEARNED_E] */
        paramType   = CPSS_NST_AM_DA_AUTO_LEARNED_E;
        
        st = cpssExMxPmNstBrgDefaultAccessLevelsGet(dev, paramType, &accessLevel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, paramType);

        /* Call with paramType [CPSS_NST_AM_SA_UNKNOWN_E] */
        paramType   = CPSS_NST_AM_SA_UNKNOWN_E;
        
        st = cpssExMxPmNstBrgDefaultAccessLevelsGet(dev, paramType, &accessLevel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, paramType);

        /* Call with paramType [CPSS_NST_AM_DA_UNKNOWN_E] */
        paramType   = CPSS_NST_AM_DA_UNKNOWN_E;
        
        st = cpssExMxPmNstBrgDefaultAccessLevelsGet(dev, paramType, &accessLevel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, paramType);

        /*
            1.2. Call with out of range paramType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        paramType = NST_BRG_INVALID_ENUM_CNS;

        st = cpssExMxPmNstBrgDefaultAccessLevelsGet(dev, paramType, &accessLevel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, paramType);

        paramType = CPSS_NST_AM_SA_AUTO_LEARNED_E;

        /*
            1.3. Call with accessLevelPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNstBrgDefaultAccessLevelsGet(dev, paramType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, accessLevel = NULL", dev);
    }

    paramType = CPSS_NST_AM_SA_AUTO_LEARNED_E;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstBrgDefaultAccessLevelsGet(dev, paramType, &accessLevel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNstBrgDefaultAccessLevelsGet(dev, paramType, &accessLevel);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstBrgProtSanityCheckSet
(
    IN GT_U8                 devNum,
    IN CPSS_NST_CHECK_ENT    checkType,
    IN GT_BOOL               enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstBrgProtSanityCheckSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with checkType [CPSS_NST_CHECK_TCP_SYN_DATA_E /
                              CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E /
                              CPSS_NST_CHECK_TCP_ALL_E /
                              CPSS_NST_CHECK_FRAG_IPV4_ICMP_E]
                   and enable [GT_FALSE and GT_TRUE]. 
    Expected: GT_OK.
    1.2. Call cpssExMxPmNstBrgProtSanityCheckGet with the same checkType and non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.3. Call with out of range checkType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_NST_CHECK_ENT    checkType = CPSS_NST_CHECK_TCP_SYN_DATA_E;
    GT_BOOL               enable    = GT_FALSE;
    GT_BOOL               enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with checkType [CPSS_NST_CHECK_TCP_SYN_DATA_E /
                                      CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E /
                                      CPSS_NST_CHECK_TCP_ALL_E /
                                      CPSS_NST_CHECK_FRAG_IPV4_ICMP_E]
                           and enable [GT_FALSE and GT_TRUE]. 
            Expected: GT_OK.
        */

        /* Call with checkType [CPSS_NST_CHECK_TCP_SYN_DATA_E] */
        checkType = CPSS_NST_CHECK_TCP_SYN_DATA_E;
        enable    = GT_FALSE;
        
        st = cpssExMxPmNstBrgProtSanityCheckSet(dev, checkType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, checkType, enable);

        /*
            1.2. Call cpssExMxPmNstBrgProtSanityCheckGet with the same checkType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmNstBrgProtSanityCheckGet(dev, checkType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstBrgProtSanityCheckGet: %d, %d", dev, checkType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);

        /* Call with checkType [CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E] */
        checkType = CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E;
        
        st = cpssExMxPmNstBrgProtSanityCheckSet(dev, checkType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, checkType, enable);

        /*
            1.2. Call cpssExMxPmNstBrgProtSanityCheckGet with the same checkType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmNstBrgProtSanityCheckGet(dev, checkType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstBrgProtSanityCheckGet: %d, %d", dev, checkType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);

        /* Call with checkType [CPSS_NST_CHECK_TCP_ALL_E] */
        checkType = CPSS_NST_CHECK_TCP_ALL_E;
        enable    = GT_TRUE;
        
        st = cpssExMxPmNstBrgProtSanityCheckSet(dev, checkType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, checkType, enable);

        /*
            1.2. Call cpssExMxPmNstBrgProtSanityCheckGet with the same checkType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmNstBrgProtSanityCheckGet(dev, checkType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstBrgProtSanityCheckGet: %d, %d", dev, checkType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);

        /* Call with checkType [CPSS_NST_CHECK_FRAG_IPV4_ICMP_E] */
        checkType = CPSS_NST_CHECK_FRAG_IPV4_ICMP_E;
        
        st = cpssExMxPmNstBrgProtSanityCheckSet(dev, checkType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, checkType, enable);

        /*
            1.2. Call cpssExMxPmNstBrgProtSanityCheckGet with the same checkType and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmNstBrgProtSanityCheckGet(dev, checkType, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstBrgProtSanityCheckGet: %d, %d", dev, checkType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);

        /*
            1.3. Call with out of range checkType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM
        */
        checkType = NST_BRG_INVALID_ENUM_CNS;

        st = cpssExMxPmNstBrgProtSanityCheckSet(dev, checkType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, checkType);
    }

    checkType = CPSS_NST_CHECK_TCP_SYN_DATA_E;
    enable    = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstBrgProtSanityCheckSet(dev, checkType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNstBrgProtSanityCheckSet(dev, checkType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstBrgProtSanityCheckGet
(
    IN  GT_U8                 devNum,
    IN  CPSS_NST_CHECK_ENT    checkType,
    OUT GT_BOOL               *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstBrgProtSanityCheckGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with checkType [CPSS_NST_CHECK_TCP_SYN_DATA_E /
                              CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E /
                              CPSS_NST_CHECK_TCP_ALL_E]
                   and non-NULL enablePtr. 
    Expected: GT_OK.
    1.2. Call with out of range checkType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM
    1.3. Call with enablePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_NST_CHECK_ENT    checkType = CPSS_NST_CHECK_TCP_SYN_DATA_E;
    GT_BOOL               enable    = GT_FALSE;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with checkType [CPSS_NST_CHECK_TCP_SYN_DATA_E /
                                      CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E /
                                      CPSS_NST_CHECK_TCP_ALL_E]
                           and non-NULL enablePtr. 
            Expected: GT_OK.
        */

        /* Call with checkType [CPSS_NST_CHECK_TCP_SYN_DATA_E] */
        checkType = CPSS_NST_CHECK_TCP_SYN_DATA_E;
        
        st = cpssExMxPmNstBrgProtSanityCheckGet(dev, checkType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, checkType);

        /* Call with checkType [CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E] */
        checkType = CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E;
        
        st = cpssExMxPmNstBrgProtSanityCheckGet(dev, checkType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, checkType);

        /* Call with checkType [CPSS_NST_CHECK_TCP_ALL_E] */
        checkType = CPSS_NST_CHECK_TCP_ALL_E;
        
        st = cpssExMxPmNstBrgProtSanityCheckGet(dev, checkType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, checkType);

        /*
            1.2. Call with out of range checkType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM
        */
        checkType = NST_BRG_INVALID_ENUM_CNS;

        st = cpssExMxPmNstBrgProtSanityCheckGet(dev, checkType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, checkType);

        checkType = CPSS_NST_CHECK_TCP_SYN_DATA_E;

        /*
            1.3. Call with enablePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNstBrgProtSanityCheckGet(dev, checkType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    checkType = CPSS_NST_CHECK_TCP_SYN_DATA_E;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstBrgProtSanityCheckGet(dev, checkType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNstBrgProtSanityCheckGet(dev, checkType, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmNstBrg suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmNstBrg)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstBrgAccessMatrixCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstBrgAccessMatrixCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstBrgPortAccessMatrixEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstBrgPortAccessMatrixEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstBrgDefaultAccessLevelsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstBrgDefaultAccessLevelsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstBrgProtSanityCheckSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstBrgProtSanityCheckGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmNstBrg)

