/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmSctUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmBrgSct, that provides
*       CPSS ExMxPm SCT (Secure Controll Technology) APIs.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
#include <cpss/exMxPm/exMxPmGen/sct/cpssExMxPmSct.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define SCT_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define SCT_INVALID_ENUM_CNS    0x5AAAAAA5

/* Internal functions forward declaration */
static GT_STATUS prvUtfCoreClockGet(IN GT_U8 dev, OUT GT_U32* coreClockPtr);


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctPortIeeeReservedMcTrapEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctPortIeeeReservedMcTrapEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmSctPortIeeeReservedMcTrapEnableGet
    with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = SCT_VALID_VIRT_PORT_CNS;

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

            st = cpssExMxPmSctPortIeeeReservedMcTrapEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmSctPortIeeeReservedMcTrapEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmSctPortIeeeReservedMcTrapEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctPortIeeeReservedMcTrapEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmSctPortIeeeReservedMcTrapEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmSctPortIeeeReservedMcTrapEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmSctPortIeeeReservedMcTrapEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctPortIeeeReservedMcTrapEnableGet: %d, %d", dev, port);

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
            st = cpssExMxPmSctPortIeeeReservedMcTrapEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmSctPortIeeeReservedMcTrapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmSctPortIeeeReservedMcTrapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = SCT_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctPortIeeeReservedMcTrapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctPortIeeeReservedMcTrapEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctPortIeeeReservedMcTrapEnableGet
(
    IN GT_U8      devNum,
    IN GT_U8      portNum,
    OUT GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctPortIeeeReservedMcTrapEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = SCT_VALID_VIRT_PORT_CNS;

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
            st = cpssExMxPmSctPortIeeeReservedMcTrapEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmSctPortIeeeReservedMcTrapEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmSctPortIeeeReservedMcTrapEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmSctPortIeeeReservedMcTrapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmSctPortIeeeReservedMcTrapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = SCT_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctPortIeeeReservedMcTrapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctPortIeeeReservedMcTrapEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctPortIeeeReservedMcProfileSet
(
    IN GT_U8   devNum,
    IN GT_U8   portNum,
    IN GT_U32  profileIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctPortIeeeReservedMcProfileSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with profileIndex [0 / 1].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmSctPortIeeeReservedMcProfileGet with non-NULL profileIndexPtr.
    Expected: GT_OK and the same profileIndex.
    1.1.3. Call with out of range profileIndex [2].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = SCT_VALID_VIRT_PORT_CNS;

    GT_U32      profileIndex    = 0;
    GT_U32      profileIndexGet = 0;

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
                1.1.1. Call with profileIndex [0 / 1].
                Expected: GT_OK.
            */

            /* Call with profileIndex [0] */
            profileIndex = 0;

            st = cpssExMxPmSctPortIeeeReservedMcProfileSet(dev, port, profileIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileIndex);

            /*
                1.1.2. Call cpssExMxPmSctPortIeeeReservedMcProfileGet with non-NULL profileIndexPtr.
                Expected: GT_OK and the same profileIndex.
            */
            st = cpssExMxPmSctPortIeeeReservedMcProfileGet(dev, port, &profileIndexGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctPortIeeeReservedMcProfileGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(profileIndex, profileIndexGet,
                       "get another profileIndex than was set: %d, %d", dev, port);

            /* Call with profileIndex [1] */
            profileIndex = 1;

            st = cpssExMxPmSctPortIeeeReservedMcProfileSet(dev, port, profileIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileIndex);

            /*
                1.1.2. Call cpssExMxPmSctPortIeeeReservedMcProfileGet with non-NULL profileIndexPtr.
                Expected: GT_OK and the same profileIndex.
            */
            st = cpssExMxPmSctPortIeeeReservedMcProfileGet(dev, port, &profileIndexGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctPortIeeeReservedMcProfileGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(profileIndex, profileIndexGet,
                       "get another profileIndex than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range profileIndex [2].
                Expected: NOT GT_OK.
            */
            profileIndex = 2;

            st = cpssExMxPmSctPortIeeeReservedMcProfileSet(dev, port, profileIndex);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, profileIndex);
        }

        profileIndex = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmSctPortIeeeReservedMcProfileSet(dev, port, profileIndex);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmSctPortIeeeReservedMcProfileSet(dev, port, profileIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmSctPortIeeeReservedMcProfileSet(dev, port, profileIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    profileIndex = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = SCT_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctPortIeeeReservedMcProfileSet(dev, port, profileIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctPortIeeeReservedMcProfileSet(dev, port, profileIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctPortIeeeReservedMcProfileGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_U32   *profileIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctPortIeeeReservedMcProfileGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with non-NULL profileIndexPtr.
    Expected: GT_OK.
    1.1.2. Call with profileIndexPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = SCT_VALID_VIRT_PORT_CNS;

    GT_U32      profileIndex = 0;

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
                1.1.1. Call with non-NULL profileIndexPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmSctPortIeeeReservedMcProfileGet(dev, port, &profileIndex);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with profileIndexPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmSctPortIeeeReservedMcProfileGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, profileIndexPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmSctPortIeeeReservedMcProfileGet(dev, port, &profileIndex);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmSctPortIeeeReservedMcProfileGet(dev, port, &profileIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmSctPortIeeeReservedMcProfileGet(dev, port, &profileIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = SCT_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctPortIeeeReservedMcProfileGet(dev, port, &profileIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctPortIeeeReservedMcProfileGet(dev, port, &profileIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIeeeReservedMcProtocolCmdSet
(
    IN GT_U8                                    devNum,
    IN GT_U32                                   profileIndex,
    IN CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT protocolType,
    IN GT_U32                                   protocol,
    IN CPSS_PACKET_CMD_ENT                      cmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIeeeReservedMcProtocolCmdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileIndex [0 / 1],
                   protocolType [CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E /
                                 CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E],
                   protocol [0 / 1 / 10 / 0xF],
                   cmd [CPSS_PACKET_CMD_FORWARD_E /
                        CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                        CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                        CPSS_PACKET_CMD_DROP_SOFT_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctIeeeReservedMcProtocolCmdGet with the same profileIndex, protocolType, protocol and non-NULL cmdPtr.
    Expected:  GT_OK and the same cmd.
    1.3. Call with out of range profileIndex [2]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range protocolType [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range protocol [16]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.6. Call with cmd [CPSS_PACKET_CMD_DROP_HARD_E /
                        CPSS_PACKET_CMD_ROUTE_E /
                        CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E /
                        CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E /
                        CPSS_PACKET_CMD_BRIDGE_E] (not relevant)
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range cmd [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                   profileIndex = 0;
    CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;
    GT_U32                                   protocol     = 0;
    CPSS_PACKET_CMD_ENT                      cmd          = CPSS_PACKET_CMD_FORWARD_E;
    CPSS_PACKET_CMD_ENT                      cmdGet       = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileIndex [0 / 1],
                           protocolType [CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E /
                                         CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E],
                           protocol [0 / 1 / 10 / 0xF],
                           cmd [CPSS_PACKET_CMD_FORWARD_E /
                                CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                                CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                CPSS_PACKET_CMD_DROP_SOFT_E].
            Expected: GT_OK.
        */

        /* Call with cmd [CPSS_PACKET_CMD_FORWARD_E] */
        profileIndex = 0;
        protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;
        protocol     = 0;
        cmd          = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, profileIndex, protocolType, protocol, cmd);

        /*
            1.2. Call cpssExMxPmSctIeeeReservedMcProtocolCmdGet with the same profileIndex, protocolType, protocol and non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctIeeeReservedMcProtocolCmdGet(dev, profileIndex, protocolType, protocol, &cmdGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctIeeeReservedMcProtocolCmdGet: %d, %d, %d, %d", dev, profileIndex, protocolType, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                   "get another cmd than was set: %d", dev);

        /* Call with cmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E] */
        protocol = 1;
        cmd      = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, profileIndex, protocolType, protocol, cmd);

        /*
            1.2. Call cpssExMxPmSctIeeeReservedMcProtocolCmdGet with the same profileIndex, protocolType, protocol and non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctIeeeReservedMcProtocolCmdGet(dev, profileIndex, protocolType, protocol, &cmdGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctIeeeReservedMcProtocolCmdGet: %d, %d, %d, %d", dev, profileIndex, protocolType, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                   "get another cmd than was set: %d", dev);

        /* Call with cmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E] */
        profileIndex = 1;
        protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E;
        protocol     = 10;
        cmd          = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, profileIndex, protocolType, protocol, cmd);

        /*
            1.2. Call cpssExMxPmSctIeeeReservedMcProtocolCmdGet with the same profileIndex, protocolType, protocol and non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctIeeeReservedMcProtocolCmdGet(dev, profileIndex, protocolType, protocol, &cmdGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctIeeeReservedMcProtocolCmdGet: %d, %d, %d, %d", dev, profileIndex, protocolType, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                   "get another cmd than was set: %d", dev);

        /* Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E] */
        protocol = 0xF;
        cmd      = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, profileIndex, protocolType, protocol, cmd);

        /*
            1.2. Call cpssExMxPmSctIeeeReservedMcProtocolCmdGet with the same profileIndex, protocolType, protocol and non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctIeeeReservedMcProtocolCmdGet(dev, profileIndex, protocolType, protocol, &cmdGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctIeeeReservedMcProtocolCmdGet: %d, %d, %d, %d", dev, profileIndex, protocolType, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                   "get another cmd than was set: %d", dev);

        /*
            1.3. Call with out of range profileIndex [2]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        profileIndex = 2;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileIndex);

        profileIndex = 0;

        /*
            1.4. Call with out of range protocolType [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolType = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, protocolType = %d", dev, protocolType);

        protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;

        /*
            1.5. Call with out of range protocol [16]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        protocol = 16;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, protocol = %d", dev, protocol);

        protocol = 0;

        /*
            1.6. Call with cmd [CPSS_PACKET_CMD_DROP_HARD_E /
                                CPSS_PACKET_CMD_ROUTE_E /
                                CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E /
                                CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E /
                                CPSS_PACKET_CMD_BRIDGE_E] (not relevant)
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with cmd [CPSS_PACKET_CMD_DROP_HARD_E] */
        cmd = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_ROUTE_E] */
        cmd = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E] */
        cmd = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E] */
        cmd = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_BRIDGE_E] */
        cmd = CPSS_PACKET_CMD_BRIDGE_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /*
            1.7. Call with out of range cmd [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cmd = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cmd = %d", dev, cmd);
    }

    profileIndex = 0;
    protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;
    protocol     = 0;
    cmd          = CPSS_PACKET_CMD_FORWARD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(dev, profileIndex, protocolType, protocol, cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIeeeReservedMcProtocolCmdGet
(
    IN  GT_U8                                    devNum,
    IN  GT_U32                                   profileIndex,
    IN  CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT protocolType,
    IN  GT_U32                                   protocol,
    OUT CPSS_PACKET_CMD_ENT                      *cmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIeeeReservedMcProtocolCmdGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileIndex [0 / 1],
                   protocolType [CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E /
                                 CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E],
                   protocol [0 / 0xF]
                   and non-NULL cmdPtr.
    Expected: GT_OK.
    1.2. Call with out of range profileIndex [2]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range protocolType [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range protocol [16]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.5. Call with cmdPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                   profileIndex = 0;
    CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;
    GT_U32                                   protocol     = 0;
    CPSS_PACKET_CMD_ENT                      cmd          = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileIndex [0 / 1],
                           protocolType [CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E /
                                         CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E],
                           protocol [0 / 0xF]
                           and non-NULL cmdPtr.
            Expected: GT_OK.
        */

        /* Call with profileIndex [0] */
        profileIndex = 0;
        protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;
        protocol     = 0;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdGet(dev, profileIndex, protocolType, protocol, &cmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, profileIndex, protocolType, protocol);

        /* Call with profileIndex [1] */
        profileIndex = 1;
        protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E;
        protocol     = 0xF;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdGet(dev, profileIndex, protocolType, protocol, &cmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, profileIndex, protocolType, protocol);

        /*
            1.2. Call with out of range profileIndex [2]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        profileIndex = 2;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdGet(dev, profileIndex, protocolType, protocol, &cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileIndex);

        profileIndex = 0;

        /*
            1.3. Call with out of range protocolType [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolType = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdGet(dev, profileIndex, protocolType, protocol, &cmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, protocolType = %d", dev, protocolType);

        protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;

        /*
            1.4. Call with out of range protocol [16]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        protocol = 16;

        st = cpssExMxPmSctIeeeReservedMcProtocolCmdGet(dev, profileIndex, protocolType, protocol, &cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, protocol = %d", dev, protocol);

        protocol = 0;

        /*
            1.5. Call with cmdPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctIeeeReservedMcProtocolCmdGet(dev, profileIndex, protocolType, protocol, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cmdPtr = NULL", dev);
    }

    profileIndex = 0;
    protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;
    protocol     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIeeeReservedMcProtocolCmdGet(dev, profileIndex, protocolType, protocol, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIeeeReservedMcProtocolCmdGet(dev, profileIndex, protocolType, protocol, &cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet
(
    IN GT_U8                                    devNum,
    IN GT_U32                                   profileIndex,
    IN CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT protocolType,
    IN GT_U32                                   protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT                 cpuCode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileIndex [0 / 1],
                   protocolType [CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E /
                                 CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E],
                   protocol [0 / 1 / 10 / 0xF],
                   cpuCode [CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E /
                            CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E /
                            CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E /
                            CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet with the same profileIndex, protocolType, protocol and non-NULL cpuCodePtr.
    Expected:  GT_OK and the same cpuCode.
    1.3. Call with out of range profileIndex [2]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range protocolType [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range protocol [16]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.6. Call with cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP1_E /
                            CPSS_NET_UNREGISTERED_MULTICAST_E] (not relevant)
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range cpuCode [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                   profileIndex = 0;
    CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;
    GT_U32                                   protocol     = 0;
    CPSS_NET_RX_CPU_CODE_ENT                 cpuCode      = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E;
    CPSS_NET_RX_CPU_CODE_ENT                 cpuCodeGet   = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileIndex [0 / 1],
                           protocolType [CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E /
                                         CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E],
                           protocol [0 / 1 / 10 / 0xF],
                           cpuCode [CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E /
                                    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E /
                                    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E /
                                    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E].
            Expected: GT_OK.
        */

        /* Call with cpuCode [CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E] */
        profileIndex = 0;
        protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;
        protocol     = 0;
        cpuCode      = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet(dev, profileIndex, protocolType, protocol, cpuCode);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, profileIndex, protocolType, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet with the same profileIndex, protocolType, protocol and non-NULL cpuCodePtr.
            Expected:  GT_OK and the same cpuCode.
        */
        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet(dev, profileIndex, protocolType, protocol, &cpuCodeGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet: %d, %d, %d, %d", dev, profileIndex, protocolType, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                   "get another cmd than was set: %d", dev);

        /* Call with cpuCode [CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E] */
        protocol     = 1;
        cpuCode      = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet(dev, profileIndex, protocolType, protocol, cpuCode);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, profileIndex, protocolType, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet with the same profileIndex, protocolType, protocol and non-NULL cpuCodePtr.
            Expected:  GT_OK and the same cpuCode.
        */
        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet(dev, profileIndex, protocolType, protocol, &cpuCodeGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet: %d, %d, %d, %d", dev, profileIndex, protocolType, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                   "get another cmd than was set: %d", dev);

        /* Call with cpuCode [CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E] */
        profileIndex = 0;
        protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E;
        protocol     = 10;
        cpuCode      = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet(dev, profileIndex, protocolType, protocol, cpuCode);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, profileIndex, protocolType, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet with the same profileIndex, protocolType, protocol and non-NULL cpuCodePtr.
            Expected:  GT_OK and the same cpuCode.
        */
        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet(dev, profileIndex, protocolType, protocol, &cpuCodeGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet: %d, %d, %d, %d", dev, profileIndex, protocolType, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                   "get another cmd than was set: %d", dev);

        /* Call with cpuCode [CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E] */
        protocol     = 0xF;
        cpuCode      = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet(dev, profileIndex, protocolType, protocol, cpuCode);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, profileIndex, protocolType, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet with the same profileIndex, protocolType, protocol and non-NULL cpuCodePtr.
            Expected:  GT_OK and the same cpuCode.
        */
        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet(dev, profileIndex, protocolType, protocol, &cpuCodeGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet: %d, %d, %d, %d", dev, profileIndex, protocolType, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                   "get another cmd than was set: %d", dev);

       /*
            1.3. Call with out of range profileIndex [2]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        profileIndex = 2;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet(dev, profileIndex, protocolType, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileIndex);

        profileIndex = 0;

        /*
            1.4. Call with out of range protocolType [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolType = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet(dev, profileIndex, protocolType, protocol, cpuCode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, protocolType = %d", dev, protocolType);

        protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;

        /*
            1.5. Call with out of range protocol [16]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        protocol = 16;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet(dev, profileIndex, protocolType, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, protocol = %d", dev, protocol);

        protocol = 0;

        /*
            1.6. Call with cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP1_E /
                                    CPSS_NET_UNREGISTERED_MULTICAST_E] (not relevant)
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP1_E] */
        cpuCode = CPSS_NET_UDP_BC_MIRROR_TRAP1_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet(dev, profileIndex, protocolType, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cpuCode = %d", dev, cpuCode);

        /* Call with cpuCode [CPSS_NET_UNREGISTERED_MULTICAST_E] */
        cpuCode = CPSS_NET_UNREGISTERED_MULTICAST_E;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet(dev, profileIndex, protocolType, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cpuCode = %d", dev, cpuCode);

        /*
            1.7. Call with out of range cpuCode [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cpuCode = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet(dev, profileIndex, protocolType, protocol, cpuCode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cpuCode = %d", dev, cpuCode);
    }

    profileIndex = 0;
    protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;
    protocol     = 0;
    cpuCode      = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet(dev, profileIndex, protocolType, protocol, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet(dev, profileIndex, protocolType, protocol, cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet
(
    IN  GT_U8                                    devNum,
    IN  GT_U32                                   profileIndex,
    IN  CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT protocolType,
    IN  GT_U32                                   protocol,
    OUT CPSS_NET_RX_CPU_CODE_ENT                 *cpuCodePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileIndex [0 / 1],
                   protocolType [CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E /
                                 CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E],
                   protocol [0 / 0xF]
                   and non-NULL cpuCodePtr.
    Expected: GT_OK.
    1.2. Call with out of range profileIndex [2]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range protocolType [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range protocol [16]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.5. Call with cpuCodePtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                   profileIndex = 0;
    CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;
    GT_U32                                   protocol     = 0;
    CPSS_NET_RX_CPU_CODE_ENT                 cpuCode      = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with profileIndex [0 / 1],
                           protocolType [CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E /
                                         CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E],
                           protocol [0 / 0xF]
                           and non-NULL cpuCodePtr.
            Expected: GT_OK.
        */

        /* Call with profileIndex [0] */
        profileIndex = 0;
        protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;
        protocol     = 0;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet(dev, profileIndex, protocolType, protocol, &cpuCode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, profileIndex, protocolType, protocol);

        /* Call with profileIndex [1] */
        profileIndex = 1;
        protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E;
        protocol     = 0xF;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet(dev, profileIndex, protocolType, protocol, &cpuCode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, profileIndex, protocolType, protocol);

       /*
            1.2. Call with out of range profileIndex [2]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        profileIndex = 2;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet(dev, profileIndex, protocolType, protocol, &cpuCode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileIndex);

        profileIndex = 0;

        /*
            1.3. Call with out of range protocolType [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolType = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet(dev, profileIndex, protocolType, protocol, &cpuCode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, protocolType = %d", dev, protocolType);

        protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;

        /*
            1.4. Call with out of range protocol [16]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        protocol = 16;

        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet(dev, profileIndex, protocolType, protocol, &cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, protocol = %d", dev, protocol);

        protocol = 0;

        /*
            1.5. Call with cpuCodePtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet(dev, profileIndex, protocolType, protocol, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cpuCodePtr = NULL", dev);
    }

    profileIndex = 0;
    protocolType = CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E;
    protocol     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet(dev, profileIndex, protocolType, protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet(dev, profileIndex, protocolType, protocol, &cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCiscoL2McCmdSet
(
    IN GT_U8                devNum,
    IN CPSS_PACKET_CMD_ENT  cmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCiscoL2McCmdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cmd [CPSS_PACKET_CMD_FORWARD_E /
                        CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                        CPSS_PACKET_CMD_TRAP_TO_CPU_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctCiscoL2McCmdGet with non-NULL cmdPtr.
    Expected:  GT_OK and the same cmd.
    1.3. Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E  /
                        CPSS_PACKET_CMD_DROP_HARD_E /
                        CPSS_PACKET_CMD_ROUTE_E] (not relevant).
    Expected: NOT GT_OK.
    1.4. Call with out of range cmd [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PACKET_CMD_ENT  cmd    = CPSS_PACKET_CMD_FORWARD_E;
    CPSS_PACKET_CMD_ENT  cmdGet = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cmd [CPSS_PACKET_CMD_FORWARD_E /
                                CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                                CPSS_PACKET_CMD_TRAP_TO_CPU_E].
            Expected: GT_OK.
        */

        /* Call with cmd [CPSS_PACKET_CMD_FORWARD_E] */
        cmd = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmSctCiscoL2McCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.2. Call cpssExMxPmSctCiscoL2McCmdGet with non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctCiscoL2McCmdGet(dev, &cmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctCiscoL2McCmdGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /* Call with cmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E] */
        cmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmSctCiscoL2McCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.2. Call cpssExMxPmSctCiscoL2McCmdGet with non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctCiscoL2McCmdGet(dev, &cmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctCiscoL2McCmdGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /* Call with cmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E] */
        cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmSctCiscoL2McCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.2. Call cpssExMxPmSctCiscoL2McCmdGet with non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctCiscoL2McCmdGet(dev, &cmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctCiscoL2McCmdGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /*
            1.3. Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E  /
                                CPSS_PACKET_CMD_DROP_HARD_E /
                                CPSS_PACKET_CMD_ROUTE_E] (not relevant).
            Expected: NOT GT_OK.
        */

        /* Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E] */
        cmd = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssExMxPmSctCiscoL2McCmdSet(dev, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_DROP_HARD_E] */
        cmd = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmSctCiscoL2McCmdSet(dev, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_ROUTE_E] */
        cmd = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmSctCiscoL2McCmdSet(dev, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.4. Call with out of range cmd [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        cmd = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctCiscoL2McCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cmd);
    }

    cmd = CPSS_PACKET_CMD_FORWARD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCiscoL2McCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCiscoL2McCmdSet(dev, cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCiscoL2McCmdGet
(
    IN  GT_U8                devNum,
    OUT CPSS_PACKET_CMD_ENT  *cmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCiscoL2McCmdGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL cmdPtr.
    Expected: GT_OK.
    1.2. Call with cmdPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PACKET_CMD_ENT  cmd = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null cmdPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmSctCiscoL2McCmdGet(dev, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null cmdPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctCiscoL2McCmdGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cmdPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCiscoL2McCmdGet(dev, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCiscoL2McCmdGet(dev, &cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIgmpSnoopModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_IGMP_SNOOP_MODE_ENT     mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIgmpSnoopModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mode [CPSS_IGMP_ALL_TRAP_MODE_E /
                         CPSS_IGMP_SNOOP_TRAP_MODE_E /
                         CPSS_IGMP_ROUTER_MIRROR_MODE_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctIgmpSnoopModeGet with non-NULL modePtr.
    Expected:  GT_OK and the same mode.
    1.3. Call with mode [CPSS_IGMP_FWD_DECISION_MODE_E] (not relevant).
    Expected: NOT GT_OK.
    1.4. Call with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_IGMP_SNOOP_MODE_ENT    mode    = CPSS_IGMP_ALL_TRAP_MODE_E;
    CPSS_IGMP_SNOOP_MODE_ENT    modeGet = CPSS_IGMP_ALL_TRAP_MODE_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [CPSS_IGMP_ALL_TRAP_MODE_E /
                                 CPSS_IGMP_SNOOP_TRAP_MODE_E /
                                 CPSS_IGMP_ROUTER_MIRROR_MODE_E].
            Expected: GT_OK.
        */

        /* Call with mode [CPSS_IGMP_ALL_TRAP_MODE_E] */
        mode = CPSS_IGMP_ALL_TRAP_MODE_E;

        st = cpssExMxPmSctIgmpSnoopModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmSctIgmpSnoopModeGet with non-NULL modePtr.
            Expected:  GT_OK and the same mode.
        */
        st = cpssExMxPmSctIgmpSnoopModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctIgmpSnoopModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_IGMP_SNOOP_TRAP_MODE_E] */
        mode = CPSS_IGMP_SNOOP_TRAP_MODE_E;

        st = cpssExMxPmSctIgmpSnoopModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmSctIgmpSnoopModeGet with non-NULL modePtr.
            Expected:  GT_OK and the same mode.
        */
        st = cpssExMxPmSctIgmpSnoopModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctIgmpSnoopModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_IGMP_ROUTER_MIRROR_MODE_E] */
        mode = CPSS_IGMP_ROUTER_MIRROR_MODE_E;

        st = cpssExMxPmSctIgmpSnoopModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmSctIgmpSnoopModeGet with non-NULL modePtr.
            Expected:  GT_OK and the same mode.
        */
        st = cpssExMxPmSctIgmpSnoopModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctIgmpSnoopModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /*
            1.3. Call with mode [CPSS_IGMP_FWD_DECISION_MODE_E] (not relevant).
            Expected: NOT GT_OK.
        */
        mode = CPSS_IGMP_FWD_DECISION_MODE_E;

        st = cpssExMxPmSctIgmpSnoopModeSet(dev, mode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.4. Call with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctIgmpSnoopModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_IGMP_ALL_TRAP_MODE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIgmpSnoopModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIgmpSnoopModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIgmpSnoopModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_IGMP_SNOOP_MODE_ENT     *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIgmpSnoopModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL modePtr.
    Expected: GT_OK.
    1.2. Call with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_IGMP_SNOOP_MODE_ENT    mode = CPSS_IGMP_ALL_TRAP_MODE_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null modePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmSctIgmpSnoopModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctIgmpSnoopModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIgmpSnoopModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIgmpSnoopModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIcmpV6MsgTypeSet
(
    IN GT_U8                devNum,
    IN GT_U32               index,
    IN GT_U32               msgType,
    IN CPSS_PACKET_CMD_ENT  cmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIcmpV6MsgTypeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 5 / 7],
                   msgType [0 / 100 / 255]
                   and cmd [CPSS_PACKET_CMD_FORWARD_E /
                            CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                            CPSS_PACKET_CMD_TRAP_TO_CPU_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctIcmpV6MsgTypeGet with the same index, non-NULL msgTypePtr and non-NULL cmdPtr.
    Expected:  GT_OK and the same msgType and cmd.
    1.3. Call with out of range index [8]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range msgType [256]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.5. Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E /
                        CPSS_PACKET_CMD_DROP_HARD_E /
                        CPSS_PACKET_CMD_ROUTE_E] (not relevant)
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range cmd [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32               index      = 0;
    GT_U32               msgType    = 0;
    CPSS_PACKET_CMD_ENT  cmd        = CPSS_PACKET_CMD_FORWARD_E;
    GT_U32               msgTypeGet = 0;
    CPSS_PACKET_CMD_ENT  cmdGet     = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 5 / 7],
                           msgType [0 / 100 / 255]
                           and cmd [CPSS_PACKET_CMD_FORWARD_E /
                                    CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                                    CPSS_PACKET_CMD_TRAP_TO_CPU_E].
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index   = 0;
        msgType = 0;
        cmd     = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmSctIcmpV6MsgTypeSet(dev, index, msgType, cmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, msgType, cmd);

        /*
            1.2. Call cpssExMxPmSctIcmpV6MsgTypeGet with the same index, non-NULL msgTypePtr and non-NULL cmdPtr.
            Expected:  GT_OK and the same msgType and cmd.
        */
        st = cpssExMxPmSctIcmpV6MsgTypeGet(dev, index, &msgTypeGet, &cmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctIcmpV6MsgTypeGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(msgType, msgTypeGet,
                       "get another msgType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /* Call with index [5] */
        index   = 5;
        msgType = 100;
        cmd     = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmSctIcmpV6MsgTypeSet(dev, index, msgType, cmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, msgType, cmd);

        /*
            1.2. Call cpssExMxPmSctIcmpV6MsgTypeGet with the same index, non-NULL msgTypePtr and non-NULL cmdPtr.
            Expected:  GT_OK and the same msgType and cmd.
        */
        st = cpssExMxPmSctIcmpV6MsgTypeGet(dev, index, &msgTypeGet, &cmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctIcmpV6MsgTypeGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(msgType, msgTypeGet,
                       "get another msgType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /* Call with index [7] */
        index   = 7;
        msgType = 255;
        cmd     = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmSctIcmpV6MsgTypeSet(dev, index, msgType, cmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, msgType, cmd);

        /*
            1.2. Call cpssExMxPmSctIcmpV6MsgTypeGet with the same index, non-NULL msgTypePtr and non-NULL cmdPtr.
            Expected:  GT_OK and the same msgType and cmd.
        */
        st = cpssExMxPmSctIcmpV6MsgTypeGet(dev, index, &msgTypeGet, &cmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctIcmpV6MsgTypeGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(msgType, msgTypeGet,
                       "get another msgType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /*
            1.3. Call with out of range index [8]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmSctIcmpV6MsgTypeSet(dev, index, msgType, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.4. Call with out of range msgType [256]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        msgType = 256;

        st = cpssExMxPmSctIcmpV6MsgTypeSet(dev, index, msgType, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, msgType = %d", dev, msgType);

        msgType = 0;

        /*
            1.5. Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E /
                                CPSS_PACKET_CMD_DROP_HARD_E /
                                CPSS_PACKET_CMD_ROUTE_E] (not relevant)
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E] */
        cmd = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssExMxPmSctIcmpV6MsgTypeSet(dev, index, msgType, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_DROP_HARD_E] */
        cmd = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmSctIcmpV6MsgTypeSet(dev, index, msgType, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_ROUTE_E] */
        cmd = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmSctIcmpV6MsgTypeSet(dev, index, msgType, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /*
            1.6. Call with out of range cmd [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cmd = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctIcmpV6MsgTypeSet(dev, index, msgType, cmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cmd = %d", dev, cmd);
    }

    index   = 0;
    msgType = 0;
    cmd     = CPSS_PACKET_CMD_FORWARD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIcmpV6MsgTypeSet(dev, index, msgType, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIcmpV6MsgTypeSet(dev, index, msgType, cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIcmpV6MsgTypeGet
(
    IN  GT_U8                devNum,
    IN  GT_U32               index,
    OUT GT_U32               *msgTypePtr,
    OUT CPSS_PACKET_CMD_ENT  *cmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIcmpV6MsgTypeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 5 / 7],
                   non-NULL msgTypePtr
                   and non-NULL cmdPtr.
    Expected: GT_OK.
    1.2. Call with out of range index [8]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.3. Call with msgTypePtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with cmdPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32               index   = 0;
    GT_U32               msgType = 0;
    CPSS_PACKET_CMD_ENT  cmd     = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 5 / 7],
                           non-NULL msgTypePtr
                           and non-NULL cmdPtr.
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index = 0;

        st = cpssExMxPmSctIcmpV6MsgTypeGet(dev, index, &msgType, &cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with index [5] */
        index = 5;

        st = cpssExMxPmSctIcmpV6MsgTypeGet(dev, index, &msgType, &cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with index [7] */
        index = 7;

        st = cpssExMxPmSctIcmpV6MsgTypeGet(dev, index, &msgType, &cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call with out of range index [8]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmSctIcmpV6MsgTypeGet(dev, index, &msgType, &cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call with msgTypePtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctIcmpV6MsgTypeGet(dev, index, NULL, &cmd);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, msgTypePtr = NULL", dev);

        /*
            1.4. Call with cmdPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctIcmpV6MsgTypeGet(dev, index, &msgType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cmdPtr = NULL", dev);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIcmpV6MsgTypeGet(dev, index, &msgType, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIcmpV6MsgTypeGet(dev, index, &msgType, &cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctUdpBcDestPortCfgSet
(
   IN GT_U8                     devNum,
   IN GT_U32                    entryIndex,
   IN GT_U32                    udpPortNum,
   IN CPSS_NET_RX_CPU_CODE_ENT  cpuCode,
   IN CPSS_PACKET_CMD_ENT       cmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctUdpBcDestPortCfgSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIndex [0 / 11],
                   udpPortNum [0 / 0xFFFF],
                   cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP0_E /
                            CPSS_NET_UDP_BC_MIRROR_TRAP1_E /
                            CPSS_NET_UDP_BC_MIRROR_TRAP2_E /
                            CPSS_NET_UDP_BC_MIRROR_TRAP3_E]
                   and cmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                            CPSS_PACKET_CMD_TRAP_TO_CPU_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctUdpBcDestPortCfgGet with the same entryIndex,
                                                    non-NULL udpPortNumPtr,
                                                    non-NULL cpuCodePtr
                                                    and non-NULL cmdPtr.
    Expected: GT_OK and the same udpPortNum, cpuCode and cmd.
    1.3. Call with out of range entryIndex [12]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range udpPortNum [65536]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.5. Call with cpuCode [CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E /
                            CPSS_NET_BRIDGED_PACKET_FORWARD_E] (not relevant)
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range cpuCode [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E /
                        CPSS_PACKET_CMD_DROP_HARD_E /
                        CPSS_PACKET_CMD_ROUTE_E] (not relevant)
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.8. Call with out of range cmd [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call cpssExMxPmSctUdpBcDestPortCfgInvalidate with same entryIndex to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    entryIndex    = 0;
    GT_U32                    udpPortNum    = 0;
    CPSS_NET_RX_CPU_CODE_ENT  cpuCode       = CPSS_NET_UDP_BC_MIRROR_TRAP0_E;
    CPSS_PACKET_CMD_ENT       cmd           = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
    GT_U32                    udpPortNumGet = 0;
    CPSS_NET_RX_CPU_CODE_ENT  cpuCodeGet    = CPSS_NET_UDP_BC_MIRROR_TRAP0_E;
    CPSS_PACKET_CMD_ENT       cmdGet        = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryIndex [0 / 11],
                           udpPortNum [0 / 0xFFFF],
                           cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP0_E /
                                    CPSS_NET_UDP_BC_MIRROR_TRAP1_E /
                                    CPSS_NET_UDP_BC_MIRROR_TRAP2_E /
                                    CPSS_NET_UDP_BC_MIRROR_TRAP3_E]
                           and cmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                                    CPSS_PACKET_CMD_TRAP_TO_CPU_E].
            Expected: GT_OK.
        */

        /* Call with cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP0_E] */
        entryIndex = 0;
        udpPortNum = 0;
        cpuCode    = CPSS_NET_UDP_BC_MIRROR_TRAP0_E;
        cmd        = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, entryIndex, udpPortNum, cpuCode, cmd);

        /*
            1.2. Call cpssExMxPmSctUdpBcDestPortCfgGet with the same entryIndex,
                                                            non-NULL udpPortNumPtr,
                                                            non-NULL cpuCodePtr
                                                            and non-NULL cmdPtr.
            Expected: GT_OK and the same udpPortNum, cpuCode and cmd.
        */
        st = cpssExMxPmSctUdpBcDestPortCfgGet(dev, entryIndex, &udpPortNumGet, &cpuCodeGet, &cmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctUdpBcDestPortCfgGet: %d, %d", dev, entryIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(udpPortNum, udpPortNumGet,
                       "get another udpPortNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                       "get another cpuCode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /* Call with cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP1_E] */
        cpuCode = CPSS_NET_UDP_BC_MIRROR_TRAP1_E;

        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, entryIndex, udpPortNum, cpuCode, cmd);

        /*
            1.2. Call cpssExMxPmSctUdpBcDestPortCfgGet with the same entryIndex,
                                                            non-NULL udpPortNumPtr,
                                                            non-NULL cpuCodePtr
                                                            and non-NULL cmdPtr.
            Expected: GT_OK and the same udpPortNum, cpuCode and cmd.
        */
        st = cpssExMxPmSctUdpBcDestPortCfgGet(dev, entryIndex, &udpPortNumGet, &cpuCodeGet, &cmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctUdpBcDestPortCfgGet: %d, %d", dev, entryIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(udpPortNum, udpPortNumGet,
                       "get another udpPortNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                       "get another cpuCode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /* Call with cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP2_E] */
        entryIndex = 11;
        udpPortNum = 0xFFFF;
        cpuCode    = CPSS_NET_UDP_BC_MIRROR_TRAP2_E;
        cmd        = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, entryIndex, udpPortNum, cpuCode, cmd);

        /*
            1.2. Call cpssExMxPmSctUdpBcDestPortCfgGet with the same entryIndex,
                                                            non-NULL udpPortNumPtr,
                                                            non-NULL cpuCodePtr
                                                            and non-NULL cmdPtr.
            Expected: GT_OK and the same udpPortNum, cpuCode and cmd.
        */
        st = cpssExMxPmSctUdpBcDestPortCfgGet(dev, entryIndex, &udpPortNumGet, &cpuCodeGet, &cmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctUdpBcDestPortCfgGet: %d, %d", dev, entryIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(udpPortNum, udpPortNumGet,
                       "get another udpPortNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                       "get another cpuCode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /* Call with cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP3_E] */
        cpuCode = CPSS_NET_UDP_BC_MIRROR_TRAP3_E;

        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, entryIndex, udpPortNum, cpuCode, cmd);

        /*
            1.2. Call cpssExMxPmSctUdpBcDestPortCfgGet with the same entryIndex,
                                                            non-NULL udpPortNumPtr,
                                                            non-NULL cpuCodePtr
                                                            and non-NULL cmdPtr.
            Expected: GT_OK and the same udpPortNum, cpuCode and cmd.
        */
        st = cpssExMxPmSctUdpBcDestPortCfgGet(dev, entryIndex, &udpPortNumGet, &cpuCodeGet, &cmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctUdpBcDestPortCfgGet: %d, %d", dev, entryIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(udpPortNum, udpPortNumGet,
                       "get another udpPortNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                       "get another cpuCode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /*
            1.3. Call with out of range entryIndex [12]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        entryIndex = 12;

        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        entryIndex = 0;

        /*
            1.4. Call with out of range udpPortNum [65536]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        udpPortNum = 65536;

        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, udpPortNum = %d", dev, udpPortNum);

        udpPortNum = 0;

        /*
            1.5. Call with cpuCode [CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E /
                                    CPSS_NET_BRIDGED_PACKET_FORWARD_E] (not relevant)
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with cpuCode [CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E] */
        cpuCode = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;

        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cpuCode = %d", dev, cpuCode);

        /* Call with cpuCode [CPSS_NET_BRIDGED_PACKET_FORWARD_E] */
        cpuCode = CPSS_NET_BRIDGED_PACKET_FORWARD_E;

        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cpuCode = %d", dev, cpuCode);

        /*
            1.6. Call with out of range cpuCode [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cpuCode = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cpuCode = %d", dev, cpuCode);

        /*
            1.7. Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E /
                                CPSS_PACKET_CMD_DROP_HARD_E /
                                CPSS_PACKET_CMD_ROUTE_E] (not relevant)
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E] */
        cmd = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_DROP_HARD_E] */
        cmd = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_ROUTE_E] */
        cmd = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cmd = %d", dev, cmd);

        /*
            1.8. Call with out of range cmd [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cmd = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cmd = %d", dev, cmd);

        /*
            1.9. Call cpssExMxPmSctUdpBcDestPortCfgInvalidate with same entryIndex to invalidate changes.
            Expected: GT_OK.
        */

        /* Call with enable entryIndex [0] */
        entryIndex = 0;

        st = cpssExMxPmSctUdpBcDestPortCfgInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctUdpBcDestPortCfgInvalidate: %d, %d", dev, entryIndex);

        /* Call with enable entryIndex [11] */
        entryIndex = 11;

        st = cpssExMxPmSctUdpBcDestPortCfgInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctUdpBcDestPortCfgInvalidate: %d, %d", dev, entryIndex);
    }

    entryIndex = 0;
    udpPortNum = 0;
    cpuCode    = CPSS_NET_UDP_BC_MIRROR_TRAP0_E;
    cmd        = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, entryIndex, udpPortNum, cpuCode, cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctUdpBcDestPortCfgGet
(
   IN  GT_U8                     devNum,
   IN  GT_U32                    entryIndex,
   OUT GT_U32                    *udpPortNumPtr,
   OUT CPSS_NET_RX_CPU_CODE_ENT  *cpuCodePtr,
   OUT CPSS_PACKET_CMD_ENT       *cmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctUdpBcDestPortCfgGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIndex [0 / 11],
                   non-NULL udpPortNumPtr,
                   non-NULL cpuCodePtr
                   and non-NULL cmdPtr.
    Expected: GT_OK.
    1.2. Call with out of range entryIndex [12]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.3. Call with udpPortNumPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with cpuCodePtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with cmdPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    entryIndex = 0;
    GT_U32                    udpPortNum = 0;
    CPSS_NET_RX_CPU_CODE_ENT  cpuCode    = CPSS_NET_UDP_BC_MIRROR_TRAP0_E;
    CPSS_PACKET_CMD_ENT       cmd        = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryIndex [0 / 11],
                           non-NULL udpPortNumPtr,
                           non-NULL cpuCodePtr
                           and non-NULL cmdPtr.
            Expected: GT_OK.
        */

        /* Call with entryIndex [0] */
        entryIndex = 0;

        st = cpssExMxPmSctUdpBcDestPortCfgGet(dev, entryIndex, &udpPortNum, &cpuCode, &cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /* Call with entryIndex [11] */
        entryIndex = 11;

        st = cpssExMxPmSctUdpBcDestPortCfgGet(dev, entryIndex, &udpPortNum, &cpuCode, &cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call with out of range entryIndex [12]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        entryIndex = 12;

        st = cpssExMxPmSctUdpBcDestPortCfgGet(dev, entryIndex, &udpPortNum, &cpuCode, &cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        entryIndex = 0;

        /*
            1.3. Call with udpPortNumPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctUdpBcDestPortCfgGet(dev, entryIndex, NULL, &cpuCode, &cmd);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, udpPortNumPtr = NULL", dev);

        /*
            1.4. Call with cpuCodePtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctUdpBcDestPortCfgGet(dev, entryIndex, &udpPortNum, NULL, &cmd);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cpuCodePtr = NULL", dev);

        /*
            1.5. Call with cmdPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctUdpBcDestPortCfgGet(dev, entryIndex, &udpPortNum, &cpuCode, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cmdPtr = NULL", dev);
    }

    entryIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctUdpBcDestPortCfgGet(dev, entryIndex, &udpPortNum, &cpuCode, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctUdpBcDestPortCfgGet(dev, entryIndex, &udpPortNum, &cpuCode, &cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctUdpBcDestPortCfgInvalidate
(
   IN GT_U8                                     devNum,
   IN GT_U32                                    entryIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctUdpBcDestPortCfgInvalidate)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIndex [0 / 5 / 11].
    Expected: GT_OK.
    1.2. Call with out of range entryIndex [12].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      entryIndex = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryIndex [0 / 5 / 11].
            Expected: GT_OK.
        */

        /* Call with entryIndex [0] */
        entryIndex = 0;

        st = cpssExMxPmSctUdpBcDestPortCfgInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /* Call with entryIndex [5] */
        entryIndex = 5;

        st = cpssExMxPmSctUdpBcDestPortCfgInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /* Call with entryIndex [11] */
        entryIndex = 11;

        st = cpssExMxPmSctUdpBcDestPortCfgInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call with out of range entryIndex [12].
            Expected: NOT GT_OK.
        */
        entryIndex = 12;

        st = cpssExMxPmSctUdpBcDestPortCfgInvalidate(dev, entryIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);
    }

    entryIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctUdpBcDestPortCfgInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctUdpBcDestPortCfgInvalidate(dev, entryIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctArpBcastCmdSet
(
    IN GT_U8                devNum,
    IN CPSS_PACKET_CMD_ENT  cmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctArpBcastCmdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cmd [CPSS_PACKET_CMD_FORWARD_E /
                        CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                        CPSS_PACKET_CMD_TRAP_TO_CPU_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctArpBcastCmdGet with non-NULL cmdPtr.
    Expected:  GT_OK and the same cmd.
    1.3. Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E /
                        CPSS_PACKET_CMD_DROP_HARD_E /
                        CPSS_PACKET_CMD_ROUTE_E] (not relevant).
    Expected: NOT GT_OK.
    1.4. Call with out of range cmd [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PACKET_CMD_ENT  cmd    = CPSS_PACKET_CMD_FORWARD_E;
    CPSS_PACKET_CMD_ENT  cmdGet = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cmd [CPSS_PACKET_CMD_FORWARD_E /
                                CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                                CPSS_PACKET_CMD_TRAP_TO_CPU_E].
            Expected: GT_OK.
        */

        /* Call with cmd [CPSS_PACKET_CMD_FORWARD_E] */
        cmd = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmSctArpBcastCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.2. Call cpssExMxPmSctArpBcastCmdGet with non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctArpBcastCmdGet(dev, &cmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctArpBcastCmdGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /* Call with cmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E] */
        cmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmSctArpBcastCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.2. Call cpssExMxPmSctArpBcastCmdGet with non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctArpBcastCmdGet(dev, &cmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctArpBcastCmdGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /* Call with cmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E] */
        cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmSctArpBcastCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.2. Call cpssExMxPmSctArpBcastCmdGet with non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctArpBcastCmdGet(dev, &cmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctArpBcastCmdGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /*
            1.3. Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E  /
                                CPSS_PACKET_CMD_DROP_HARD_E /
                                CPSS_PACKET_CMD_ROUTE_E] (not relevant).
            Expected: NOT GT_OK.
        */

        /* Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E] */
        cmd = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssExMxPmSctArpBcastCmdSet(dev, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_DROP_HARD_E] */
        cmd = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmSctArpBcastCmdSet(dev, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_ROUTE_E] */
        cmd = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmSctArpBcastCmdSet(dev, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.4. Call with out of range cmd [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        cmd = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctArpBcastCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cmd);
    }

    cmd = CPSS_PACKET_CMD_FORWARD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctArpBcastCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctArpBcastCmdSet(dev, cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctArpBcastCmdGet
(
    IN  GT_U8                devNum,
    OUT CPSS_PACKET_CMD_ENT  *cmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctArpBcastCmdGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL cmdPtr.
    Expected: GT_OK.
    1.2. Call with cmdPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PACKET_CMD_ENT  cmd = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null cmdPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmSctArpBcastCmdGet(dev, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null cmdPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctArpBcastCmdGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cmdPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctArpBcastCmdGet(dev, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctArpBcastCmdGet(dev, &cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS   cpssExMxPmSctArpToMeCmdSet
(
    IN GT_U8                devNum,
    IN CPSS_PACKET_CMD_ENT  cmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctArpToMeCmdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cmd [CPSS_PACKET_CMD_FORWARD_E /
                        CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                        CPSS_PACKET_CMD_TRAP_TO_CPU_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctArpToMeCmdGet with non-NULL cmdPtr.
    Expected:  GT_OK and the same cmd.
    1.3. Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E /
                        CPSS_PACKET_CMD_DROP_HARD_E /
                        CPSS_PACKET_CMD_ROUTE_E] (not relevant).
    Expected: NOT GT_OK.
    1.4. Call with out of range cmd [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PACKET_CMD_ENT  cmd    = CPSS_PACKET_CMD_FORWARD_E;
    CPSS_PACKET_CMD_ENT  cmdGet = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cmd [CPSS_PACKET_CMD_FORWARD_E /
                                CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                                CPSS_PACKET_CMD_TRAP_TO_CPU_E].
            Expected: GT_OK.
        */

        /* Call with cmd [CPSS_PACKET_CMD_FORWARD_E] */
        cmd = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmSctArpToMeCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.2. Call cpssExMxPmSctArpToMeCmdGet with non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctArpToMeCmdGet(dev, &cmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctArpToMeCmdGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /* Call with cmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E] */
        cmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmSctArpToMeCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.2. Call cpssExMxPmSctArpToMeCmdGet with non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctArpToMeCmdGet(dev, &cmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctArpToMeCmdGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /* Call with cmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E] */
        cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmSctArpToMeCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.2. Call cpssExMxPmSctArpToMeCmdGet with non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctArpToMeCmdGet(dev, &cmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctArpToMeCmdGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /*
            1.3. Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E  /
                                CPSS_PACKET_CMD_DROP_HARD_E /
                                CPSS_PACKET_CMD_ROUTE_E] (not relevant).
            Expected: NOT GT_OK.
        */

        /* Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E] */
        cmd = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssExMxPmSctArpToMeCmdSet(dev, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_DROP_HARD_E] */
        cmd = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmSctArpToMeCmdSet(dev, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_ROUTE_E] */
        cmd = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmSctArpToMeCmdSet(dev, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.4. Call with out of range cmd [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        cmd = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctArpToMeCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cmd);
    }

    cmd = CPSS_PACKET_CMD_FORWARD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctArpToMeCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctArpToMeCmdSet(dev, cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctArpToMeCmdGet
(
    IN  GT_U8                devNum,
    OUT CPSS_PACKET_CMD_ENT  *cmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctArpToMeCmdGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL cmdPtr.
    Expected: GT_OK.
    1.2. Call with cmdPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PACKET_CMD_ENT  cmd = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null cmdPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmSctArpToMeCmdGet(dev, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null cmdPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctArpToMeCmdGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cmdPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctArpToMeCmdGet(dev, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctArpToMeCmdGet(dev, &cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIpV6SolicitedNodeMcastPrefixSet
(
    IN GT_U8        devNum,
    IN GT_IPV6ADDR  *addressPtr,
    IN GT_IPV6ADDR  *maskPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIpV6SolicitedNodeMcastPrefixSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with addressPtr->u32Ip [0x0, 0x0, 0x0, 0x0 /
                                      0xAAAA, 0xABCD, 0xFFFF, 0xFFFF]
                   and maskPtr->u32Ip [0x0, 0x0, 0x0, 0x0 /
                                       0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet with non-NULL addressPtr and non-NULL maskPtr.
    Expected:  GT_OK and the same addressPtr and maskPtr.
    1.3. Call with addressPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with maskPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_IPV6ADDR address;
    GT_IPV6ADDR mask;
    GT_IPV6ADDR addressGet;
    GT_IPV6ADDR maskGet;
    GT_BOOL     isEqual = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with addressPtr->u32Ip [0x0, 0x0, 0x0, 0x0 /
                                              0xAAAA, 0xABCD, 0xFFFF, 0xFFFF]
                           and maskPtr->u32Ip [0x0, 0x0, 0x0, 0x0 /
                                               0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF].
            Expected: GT_OK.
        */

        /* Call with mask [0 .. 0] */
        address.u32Ip[0] = 0;
        address.u32Ip[1] = 0;
        address.u32Ip[2] = 0;
        address.u32Ip[3] = 0;

        mask.u32Ip[0] = 0;
        mask.u32Ip[1] = 0;
        mask.u32Ip[2] = 0;
        mask.u32Ip[3] = 0;

        st = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixSet(dev, &address, &mask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet with non-NULL addressPtr and non-NULL maskPtr.
            Expected:  GT_OK and the same addressPtr and maskPtr.
        */
        st = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet(dev, &addressGet, &maskGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet: %d", dev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &address,
                                     (GT_VOID*) &addressGet,
                                     sizeof(address))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another address than was set: %d", dev);
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask,
                                     (GT_VOID*) &maskGet,
                                     sizeof(mask))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another mask than was set: %d", dev);

        /* Call with mask [0xFFFF .. 0xFFFF] */
        address.u32Ip[0] = 0xAAAA;
        address.u32Ip[1] = 0xABCD;
        address.u32Ip[2] = 0xFFFF;
        address.u32Ip[3] = 0xFFFF;

        mask.u32Ip[0] = 0xFFFF;
        mask.u32Ip[1] = 0xFFFF;
        mask.u32Ip[2] = 0xFFFF;
        mask.u32Ip[3] = 0xFFFF;

        st = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixSet(dev, &address, &mask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet with non-NULL addressPtr and non-NULL maskPtr.
            Expected:  GT_OK and the same addressPtr and maskPtr.
        */
        st = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet(dev, &addressGet, &maskGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet: %d", dev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &address,
                                     (GT_VOID*) &addressGet,
                                     sizeof(address))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another address than was set: %d", dev);
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask,
                                     (GT_VOID*) &maskGet,
                                     sizeof(mask))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another mask than was set: %d", dev);

        /*
            1.3. Call with addressPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixSet(dev, NULL, &mask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, addressPtr = NULL", dev);

        /*
            1.4. Call with maskPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixSet(dev, &address, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, addressPtr = NULL", dev);
    }

    address.u32Ip[0] = 0;
    address.u32Ip[1] = 0;
    address.u32Ip[2] = 0;
    address.u32Ip[3] = 0;

    mask.u32Ip[0] = 0;
    mask.u32Ip[1] = 0;
    mask.u32Ip[2] = 0;
    mask.u32Ip[3] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixSet(dev, &address, &mask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixSet(dev, &address, &mask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet
(
    IN GT_U8         devNum,
    OUT GT_IPV6ADDR  *addressPtr,
    OUT GT_IPV6ADDR  *maskPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL addressPtr
                   and non-NULL maskPtr.
    Expected: GT_OK.
    1.2. Call with addressPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with maskPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_IPV6ADDR address;
    GT_IPV6ADDR mask;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL addressPtr
                           and non-NULL maskPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet(dev, &address, &mask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with addressPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet(dev, NULL, &mask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, addressPtr = NULL", dev);

        /*
            1.3. Call with maskPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet(dev, &address, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, addressPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet(dev, &address, &mask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet(dev, &address, &mask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIpV6SolicitedCmdSet
(
    IN GT_U8                devNum,
    IN CPSS_PACKET_CMD_ENT  cmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIpV6SolicitedCmdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cmd [CPSS_PACKET_CMD_FORWARD_E /
                        CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                        CPSS_PACKET_CMD_TRAP_TO_CPU_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctIpV6SolicitedCmdGet with non-NULL cmdPtr.
    Expected:  GT_OK and the same cmd.
    1.3. Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E /
                        CPSS_PACKET_CMD_DROP_HARD_E /
                        CPSS_PACKET_CMD_ROUTE_E] (not relevant).
    Expected: NOT GT_OK.
    1.4. Call with out of range cmd [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PACKET_CMD_ENT  cmd    = CPSS_PACKET_CMD_FORWARD_E;
    CPSS_PACKET_CMD_ENT  cmdGet = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cmd [CPSS_PACKET_CMD_FORWARD_E /
                                CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                                CPSS_PACKET_CMD_TRAP_TO_CPU_E].
            Expected: GT_OK.
        */

        /* Call with cmd [CPSS_PACKET_CMD_FORWARD_E] */
        cmd = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmSctIpV6SolicitedCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.2. Call cpssExMxPmSctIpV6SolicitedCmdGet with non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctIpV6SolicitedCmdGet(dev, &cmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctIpV6SolicitedCmdGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /* Call with cmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E] */
        cmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmSctIpV6SolicitedCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.2. Call cpssExMxPmSctIpV6SolicitedCmdGet with non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctIpV6SolicitedCmdGet(dev, &cmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctIpV6SolicitedCmdGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /* Call with cmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E] */
        cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmSctIpV6SolicitedCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.2. Call cpssExMxPmSctIpV6SolicitedCmdGet with non-NULL cmdPtr.
            Expected:  GT_OK and the same cmd.
        */
        st = cpssExMxPmSctIpV6SolicitedCmdGet(dev, &cmdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctIpV6SolicitedCmdGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d", dev);

        /*
            1.3. Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E  /
                                CPSS_PACKET_CMD_DROP_HARD_E /
                                CPSS_PACKET_CMD_ROUTE_E] (not relevant).
            Expected: NOT GT_OK.
        */

        /* Call with cmd [CPSS_PACKET_CMD_DROP_SOFT_E] */
        cmd = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssExMxPmSctIpV6SolicitedCmdSet(dev, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_DROP_HARD_E] */
        cmd = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmSctIpV6SolicitedCmdSet(dev, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /* Call with cmd [CPSS_PACKET_CMD_ROUTE_E] */
        cmd = CPSS_PACKET_CMD_ROUTE_E;

        st = cpssExMxPmSctIpV6SolicitedCmdSet(dev, cmd);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cmd);

        /*
            1.4. Call with out of range cmd [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        cmd = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctIpV6SolicitedCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cmd);
    }

    cmd = CPSS_PACKET_CMD_FORWARD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIpV6SolicitedCmdSet(dev, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIpV6SolicitedCmdSet(dev, cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIpV6SolicitedCmdGet
(
    IN  GT_U8                devNum,
    OUT CPSS_PACKET_CMD_ENT  *cmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIpV6SolicitedCmdGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL cmdPtr.
    Expected: GT_OK.
    1.2. Call with cmdPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_PACKET_CMD_ENT  cmd = CPSS_PACKET_CMD_FORWARD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null cmdPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmSctIpV6SolicitedCmdGet(dev, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null cmdPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctIpV6SolicitedCmdGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cmdPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIpV6SolicitedCmdGet(dev, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIpV6SolicitedCmdGet(dev, &cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableSet
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_U32                       protocol,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E],
                   protocol [0 / 255]
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet with the same protocolStack,
                                                                        protocol
                                                                        and non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.3. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range protocolStack [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range protocol [256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_IP_PROTOCOL_STACK_ENT   protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    GT_U32                       protocol      = 0;
    GT_BOOL                      enable        = GT_FALSE;
    GT_BOOL                      enableGet     = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E],
                           protocol [0 / 255]
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        protocol      = 0;
        enable        = GT_FALSE;

        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableSet(dev, protocolStack, protocol, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, protocolStack, protocol, enable);

        /*
            1.2. Call cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet with the same protocolStack,
                                                                                protocol
                                                                                and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet(dev, protocolStack, protocol, &enableGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet: %d, %d, %d", dev, protocolStack, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        protocol      = 0;
        enable        = GT_TRUE;

        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableSet(dev, protocolStack, protocol, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, protocolStack, protocol, enable);

        /*
            1.2. Call cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet with the same protocolStack,
                                                                                protocol
                                                                                and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet(dev, protocolStack, protocol, &enableGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet: %d, %d, %d", dev, protocolStack, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /*
            1.3. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableSet(dev, protocolStack, protocol, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocolStack);

        /*
            1.4. Call with out of range protocolStack [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolStack = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableSet(dev, protocolStack, protocol, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, protocolStack);

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.5. Call with out of range protocol [256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocol = 256;

        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableSet(dev, protocolStack, protocol, enable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, protocol = %d", dev, protocol);
    }

    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    protocol      = 0;
    enable        = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableSet(dev, protocolStack, protocol, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableSet(dev, protocolStack, protocol, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet
(
    IN  GT_U8                        devNum,
    IN  CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN  GT_U32                        protocol,
    OUT GT_BOOL                      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E],
                   protocol [0 / 255]
                   and non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range protocolStack [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range protocol [256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with enablePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_IP_PROTOCOL_STACK_ENT   protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    GT_U32                       protocol      = 0;
    GT_BOOL                      enable        = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E],
                           protocol [0 / 255]
                           and non-NULL enablePtr.
            Expected: GT_OK.
        */

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        protocol      = 0;

        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet(dev, protocolStack, protocol, &enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, protocol);

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        protocol      = 0;

        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet(dev, protocolStack, protocol, &enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, protocol);

        /*
            1.2. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet(dev, protocolStack, protocol, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocolStack);

        /*
            1.3. Call with out of range protocolStack [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolStack = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet(dev, protocolStack, protocol, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, protocolStack);

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.4. Call with out of range protocol [256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocol = 256;

        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet(dev, protocolStack, protocol, &enable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, protocol = %d", dev, protocol);

        protocol = 0;

        /*
            1.5. Call with enablePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet(dev, protocolStack, protocol, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    protocol      = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet(dev, protocolStack, protocol, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet(dev, protocolStack, protocol, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_U32                       protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT     cpuCode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E],
                   protocol [0 / 255]
                   and cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E /
                                CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E /
                                CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E /
                                CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet with the same protocolStack, protocol and non-NULL cpuCodePtr.
    Expected: GT_OK and the same cpuCode.
    1.3. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range protocolStack [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range protocol [256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with cpuCode [CPSS_NET_SEC_AUTO_LEARN_UNK_SRC_TRAP_E /
                            CPSS_NET_MPLS_PACKET_FORWARD_E] (not relevant)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range cpuCode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_IP_PROTOCOL_STACK_ENT   protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    GT_U32                       protocol      = 0;
    CPSS_NET_RX_CPU_CODE_ENT     cpuCode       = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
    CPSS_NET_RX_CPU_CODE_ENT     cpuCodeGet    = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E],
                           protocol [0 / 255]
                           and cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E /
                                        CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E /
                                        CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E /
                                        CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E].
            Expected: GT_OK.
        */

        /* Call with cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        protocol      = 0;
        cpuCode       = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet(dev, protocolStack, protocol, cpuCode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, protocolStack, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet with the same protocolStack, protocol and non-NULL cpuCodePtr.
            Expected: GT_OK and the same cpuCode.
        */
        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet(dev, protocolStack, protocol, &cpuCodeGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet: %d, %d, %d", dev, protocolStack, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                   "get another cpuCode than was set: %d", dev);

        /* Call with cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E] */
        cpuCode = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet(dev, protocolStack, protocol, cpuCode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, protocolStack, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet with the same protocolStack, protocol and non-NULL cpuCodePtr.
            Expected: GT_OK and the same cpuCode.
        */
        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet(dev, protocolStack, protocol, &cpuCodeGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet: %d, %d, %d", dev, protocolStack, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                   "get another cpuCode than was set: %d", dev);

        /* Call with cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        protocol      = 255;
        cpuCode       = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet(dev, protocolStack, protocol, cpuCode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, protocolStack, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet with the same protocolStack, protocol and non-NULL cpuCodePtr.
            Expected: GT_OK and the same cpuCode.
        */
        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet(dev, protocolStack, protocol, &cpuCodeGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet: %d, %d, %d", dev, protocolStack, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                   "get another cpuCode than was set: %d", dev);

        /* Call with cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E] */
        cpuCode = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet(dev, protocolStack, protocol, cpuCode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, protocolStack, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet with the same protocolStack, protocol and non-NULL cpuCodePtr.
            Expected: GT_OK and the same cpuCode.
        */
        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet(dev, protocolStack, protocol, &cpuCodeGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet: %d, %d, %d", dev, protocolStack, protocol);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                   "get another cpuCode than was set: %d", dev);

        /*
            1.3. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet(dev, protocolStack, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocolStack);

        /*
            1.4. Call with out of range protocolStack [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolStack = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet(dev, protocolStack, protocol, cpuCode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, protocolStack);

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.5. Call with out of range protocol [256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocol = 256;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet(dev, protocolStack, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, protocol = %d", dev, protocol);

        protocol = 0;

        /*
            1.6. Call with cpuCode [CPSS_NET_SEC_AUTO_LEARN_UNK_SRC_TRAP_E /
                                    CPSS_NET_MPLS_PACKET_FORWARD_E] (not relevant)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with cpuCode [CPSS_NET_SEC_AUTO_LEARN_UNK_SRC_TRAP_E] */
        cpuCode = CPSS_NET_SEC_AUTO_LEARN_UNK_SRC_TRAP_E;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet(dev, protocolStack, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cpuCode = %d", dev, cpuCode);

        /* Call with cpuCode [CPSS_NET_MPLS_PACKET_FORWARD_E] */
        cpuCode = CPSS_NET_MPLS_PACKET_FORWARD_E;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet(dev, protocolStack, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cpuCode = %d", dev, cpuCode);

        /*
            1.7. Call with out of range cpuCode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        cpuCode = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet(dev, protocolStack, protocol, cpuCode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cpuCode = %d", dev, cpuCode);
    }

    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    protocol      = 0;
    cpuCode       = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet(dev, protocolStack, protocol, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet(dev, protocolStack, protocol, cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet
(
    IN  GT_U8                        devNum,
    IN  CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN  GT_U32                       protocol,
    OUT CPSS_NET_RX_CPU_CODE_ENT     *cpuCodePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E],
                   protocol [0 / 255]
                   and non-NULL cpuCodePtr.
    Expected: GT_OK.
    1.2. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range protocolStack [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range protocol [256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with cpuCodePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_IP_PROTOCOL_STACK_ENT   protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    GT_U32                       protocol      = 0;
    CPSS_NET_RX_CPU_CODE_ENT     cpuCode       = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E],
                           protocol [0 / 255]
                           and non-NULL cpuCodePtr.
            Expected: GT_OK.
        */

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        protocol      = 0;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet(dev, protocolStack, protocol, &cpuCode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, protocol);

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        protocol      = 0;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet(dev, protocolStack, protocol, &cpuCode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, protocol);

        /*
            1.2. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet(dev, protocolStack, protocol, &cpuCode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocolStack);

        /*
            1.3. Call with out of range protocolStack [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolStack = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet(dev, protocolStack, protocol, &cpuCode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, protocolStack);

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.4. Call with out of range protocol [256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocol = 256;

        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet(dev, protocolStack, protocol, &cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, protocol = %d", dev, protocol);

        protocol = 0;

        /*
            1.5. Call with cpuCodePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet(dev, protocolStack, protocol, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cpuCodePtr = NULL", dev);
    }

    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    protocol      = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet(dev, protocolStack, protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet(dev, protocolStack, protocol, &cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with state [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet with non-NULL enablePtr.
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

        st = cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet: %d", dev);

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
        st = cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet
(
    IN  GT_U8                        devNum,
    OUT GT_BOOL                      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet)
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
        st = cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet(dev, NULL);
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
        st = cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet
(
    IN GT_U8                            devNum,
    IN GT_U32                           rangeIndex,
    IN GT_U32                           minDstPort,
    IN GT_U32                           maxDstPort,
    IN CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetType,
    IN CPSS_NET_PROT_ENT                protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT         cpuCode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with rangeIndex [0 / 10 / 15],
                   minDstPort [0 / 12 / 0xFFFF],
                   maxDstPort [1 / 32 / 0xFFFF],
                   packetType [CPSS_NET_TCP_UDP_PACKET_UC_E /
                               CPSS_NET_TCP_UDP_PACKET_MC_E /
                               CPSS_NET_TCP_UDP_PACKET_BOTH_UC_MC_E],
                   protocol [CPSS_NET_PROT_UDP_E /
                             CPSS_NET_PROT_TCP_E /
                             CPSS_NET_PROT_BOTH_UDP_TCP_E]
                   and cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E /
                                CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E /
                                CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet with same rangeIndex,
                                                                         non-NULL validPtr,
                                                                         non-NULL minDstPortPtr,
                                                                         non-NULL maxDstPortPtr,
                                                                         non-NULL packetTypePtr,
                                                                         non-NULL protocolPtr and cpuCodePtr.
    Expected: GT_OK valid == GT_TRUE and the same minDstPort, maxDstPort, packetType, protocol, cpuCode.
    1.3. Call with out of range rangeIndex [16]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of rangeIndex minDstPort [65536]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of rangeIndex maxDstPort [65536]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range packetType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with out of range protocol [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with out of range cpuCode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate with same rangeIndex to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                           rangeIndex    = 0;
    GT_U32                           minDstPort    = 0;
    GT_U32                           maxDstPort    = 0;
    CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetType    = CPSS_NET_TCP_UDP_PACKET_UC_E;
    CPSS_NET_PROT_ENT                protocol      = CPSS_NET_PROT_UDP_E;
    CPSS_NET_RX_CPU_CODE_ENT         cpuCode       = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
    GT_U32                           minDstPortGet = 0;
    GT_U32                           maxDstPortGet = 0;
    CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetTypeGet = CPSS_NET_TCP_UDP_PACKET_UC_E;
    CPSS_NET_PROT_ENT                protocolGet   = CPSS_NET_PROT_UDP_E;
    CPSS_NET_RX_CPU_CODE_ENT         cpuCodeGet    = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
    GT_BOOL                          isValid       = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rangeIndex [0 / 10 / 15],
                           minDstPort [0 / 12 / 0xFFFF],
                           maxDstPort [1 / 32 / 0xFFFF],
                           packetType [CPSS_NET_TCP_UDP_PACKET_UC_E /
                                       CPSS_NET_TCP_UDP_PACKET_MC_E /
                                       CPSS_NET_TCP_UDP_PACKET_BOTH_UC_MC_E],
                           protocol [CPSS_NET_PROT_UDP_E /
                                     CPSS_NET_PROT_TCP_E /
                                     CPSS_NET_PROT_BOTH_UDP_TCP_E]
                           and cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E /
                                        CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E /
                                        CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E].
            Expected: GT_OK.
        */

        /* Call with enable rangeIndex [0] */
        rangeIndex = 0;
        maxDstPort = 0;
        maxDstPort = 1;
        packetType = CPSS_NET_TCP_UDP_PACKET_UC_E;
        protocol   = CPSS_NET_PROT_UDP_E;
        cpuCode    = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort,
                                                                   packetType, protocol, cpuCode);
        UTF_VERIFY_EQUAL7_PARAM_MAC(GT_OK, st, dev, rangeIndex, minDstPort,
                                    maxDstPort, packetType, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet with same rangeIndex,
                                                                                 non-NULL validPtr,
                                                                                 non-NULL minDstPortPtr,
                                                                                 non-NULL maxDstPortPtr,
                                                                                 non-NULL packetTypePtr,
                                                                                 non-NULL protocolPtr and cpuCodePtr.
            Expected: GT_OK valid == GT_TRUE and the same minDstPort, maxDstPort, packetType, protocol, cpuCode.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &isValid, &maxDstPortGet, &maxDstPortGet,
                                                                   &packetTypeGet, &protocolGet, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet: %d, %d", dev, rangeIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValid,
                   "get invalid entry: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(minDstPort, minDstPortGet,
                       "get another minDstPort than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(maxDstPort, maxDstPortGet,
                       "get another maxDstPort than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(packetType, packetTypeGet,
                       "get another packetType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(protocol, protocolGet,
                       "get another protocol than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                       "get another cpuCode than was set: %d", dev);

        /* Call with enable rangeIndex [10] */
        rangeIndex = 10;
        maxDstPort = 12;
        maxDstPort = 32;
        packetType = CPSS_NET_TCP_UDP_PACKET_MC_E;
        protocol   = CPSS_NET_PROT_TCP_E;
        cpuCode    = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort,
                                                                   packetType, protocol, cpuCode);
        UTF_VERIFY_EQUAL7_PARAM_MAC(GT_OK, st, dev, rangeIndex, minDstPort,
                                    maxDstPort, packetType, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet with same rangeIndex,
                                                                                 non-NULL validPtr,
                                                                                 non-NULL minDstPortPtr,
                                                                                 non-NULL maxDstPortPtr,
                                                                                 non-NULL packetTypePtr,
                                                                                 non-NULL protocolPtr and cpuCodePtr.
            Expected: GT_OK valid == GT_TRUE and the same minDstPort, maxDstPort, packetType, protocol, cpuCode.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &isValid, &maxDstPortGet, &maxDstPortGet,
                                                                   &packetTypeGet, &protocolGet, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet: %d, %d", dev, rangeIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValid,
                   "get invalid entry: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(minDstPort, minDstPortGet,
                       "get another minDstPort than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(maxDstPort, maxDstPortGet,
                       "get another maxDstPort than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(packetType, packetTypeGet,
                       "get another packetType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(protocol, protocolGet,
                       "get another protocol than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                       "get another cpuCode than was set: %d", dev);

        /* Call with enable rangeIndex [15] */
        rangeIndex = 15;
        maxDstPort = 0xFFFF;
        maxDstPort = 0xFFFF;
        packetType = CPSS_NET_TCP_UDP_PACKET_BOTH_UC_MC_E;
        protocol   = CPSS_NET_PROT_BOTH_UDP_TCP_E;
        cpuCode    = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort,
                                                                   packetType, protocol, cpuCode);
        UTF_VERIFY_EQUAL7_PARAM_MAC(GT_OK, st, dev, rangeIndex, minDstPort,
                                    maxDstPort, packetType, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet with same rangeIndex,
                                                                                 non-NULL validPtr,
                                                                                 non-NULL minDstPortPtr,
                                                                                 non-NULL maxDstPortPtr,
                                                                                 non-NULL packetTypePtr,
                                                                                 non-NULL protocolPtr and cpuCodePtr.
            Expected: GT_OK valid == GT_TRUE and the same minDstPort, maxDstPort, packetType, protocol, cpuCode.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &isValid, &maxDstPortGet, &maxDstPortGet,
                                                                   &packetTypeGet, &protocolGet, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet: %d, %d", dev, rangeIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValid,
                   "get invalid entry: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(minDstPort, minDstPortGet,
                       "get another minDstPort than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(maxDstPort, maxDstPortGet,
                       "get another maxDstPort than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(packetType, packetTypeGet,
                       "get another packetType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(protocol, protocolGet,
                       "get another protocol than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                       "get another cpuCode than was set: %d", dev);

        /*
            1.3. Call with out of range rangeIndex [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        rangeIndex = 16;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort,
                                                                   packetType, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rangeIndex);

        rangeIndex = 0;

        /*
            1.4. Call with out of rangeIndex minDstPort [65536]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        minDstPort = 65536;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort,
                                                                   packetType, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, minDstPort = %d", dev, minDstPort);

        minDstPort = 0;

        /*
            1.5. Call with out of rangeIndex maxDstPort [65536]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        maxDstPort = 65536;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort,
                                                                   packetType, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, maxDstPort = %d", dev, maxDstPort);

        maxDstPort = 1;

        /*
            1.6. Call with out of range packetType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        packetType = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort,
                                                                   packetType, protocol, cpuCode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, packetType = %d", dev, packetType);

        packetType = CPSS_NET_TCP_UDP_PACKET_UC_E;

        /*
            1.7. Call with out of range protocol [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocol = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort,
                                                                   packetType, protocol, cpuCode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, protocol = %d", dev, protocol);

        protocol = CPSS_NET_PROT_UDP_E;

        /*
            1.8. Call with out of range cpuCode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        cpuCode = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort,
                                                                   packetType, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cpuCode = %d", dev, cpuCode);

        cpuCode = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;

        /*
            1.9. Call cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate with same rangeIndex to invalidate changes.
            Expected: GT_OK.
        */

        /* Call with enable rangeIndex [0] */
        rangeIndex = 0;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate: %d, %d", dev, rangeIndex);

        /* Call with enable rangeIndex [10] */
        rangeIndex = 10;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate: %d, %d", dev, rangeIndex);

        /* Call with enable rangeIndex [15] */
        rangeIndex = 15;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate: %d, %d", dev, rangeIndex);
    }

    rangeIndex = 0;
    maxDstPort = 0;
    maxDstPort = 1;
    packetType = CPSS_NET_TCP_UDP_PACKET_UC_E;
    protocol   = CPSS_NET_PROT_UDP_E;
    cpuCode    = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort,
                                                                   packetType, protocol, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort,
                                                                   packetType, protocol, cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet
(
    IN GT_U8                             devNum,
    IN GT_U32                            rangeIndex,
    OUT GT_BOOL                          *validPtr,
    OUT GT_U32                           *minDstPortPtr,
    OUT GT_U32                           *maxDstPortPtr,
    OUT CPSS_NET_TCP_UDP_PACKET_TYPE_ENT *packetTypePtr,
    OUT CPSS_NET_PROT_ENT                *protocolPtr,
    OUT CPSS_NET_RX_CPU_CODE_ENT         *cpuCodePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with rangeIndex [0 / 15],
                   non-NULL validPtr,
                   non-NULL minDstPortPtr,
                   non-NULL maxDstPortPtr,
                   non-NULL packetTypePtr,
                   non-NULL protocolPtr
                   and non-NULL cpuCodePtr.
    Expected: GT_OK.
    1.2. Call with out of range rangeIndex [16]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with validPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with minDstPortPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with maxDstPortPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call with packetTypePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.7. Call with protocolPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.8. Call with cpuCodePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                           rangeIndex = 0;
    GT_U32                           minDstPort = 0;
    GT_U32                           maxDstPort = 0;
    CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetType = CPSS_NET_TCP_UDP_PACKET_UC_E;
    CPSS_NET_PROT_ENT                protocol   = CPSS_NET_PROT_UDP_E;
    CPSS_NET_RX_CPU_CODE_ENT         cpuCode    = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
    GT_BOOL                          isValid    = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rangeIndex [0 / 15],
                           non-NULL validPtr,
                           non-NULL minDstPortPtr,
                           non-NULL maxDstPortPtr,
                           non-NULL packetTypePtr,
                           non-NULL protocolPtr
                           and non-NULL cpuCodePtr.
            Expected: GT_OK.
        */

        /* Call with enable rangeIndex [0] */
        rangeIndex = 0;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &isValid, &minDstPort, &maxDstPort,
                                                                   &packetType, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rangeIndex);

        /* Call with enable rangeIndex [15] */
        rangeIndex = 15;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &isValid, &minDstPort, &maxDstPort,
                                                                   &packetType, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rangeIndex);

        /*
            1.2. Call with out of range rangeIndex [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        rangeIndex = 16;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &isValid, &minDstPort, &maxDstPort,
                                                                   &packetType, &protocol, &cpuCode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rangeIndex);

        rangeIndex = 0;

        /*
            1.3. Call with validPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, NULL, &minDstPort, &maxDstPort,
                                                                   &packetType, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);

        /*
            1.4. Call with minDstPortPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &isValid, NULL, &maxDstPort,
                                                                   &packetType, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, minDstPortPtr = NULL", dev);

        /*
            1.5. Call with maxDstPortPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &isValid, &minDstPort, NULL,
                                                                   &packetType, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maxDstPortPtr = NULL", dev);

        /*
            1.6. Call with packetTypePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &isValid, &minDstPort, &maxDstPort,
                                                                   NULL, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, packetTypePtr = NULL", dev);

        /*
            1.7. Call with protocolPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &isValid, &minDstPort, &maxDstPort,
                                                                   &packetType, NULL, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, protocolPtr = NULL", dev);

        /*
            1.8. Call with cpuCodePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &isValid, &minDstPort, &maxDstPort,
                                                                   &packetType, &protocol, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cpuCodePtr = NULL", dev);
    }

    rangeIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &isValid, &minDstPort, &maxDstPort,
                                                                   &packetType, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &isValid, &minDstPort, &maxDstPort,
                                                               &packetType, &protocol, &cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate
(
    IN GT_U8                            devNum,
    IN GT_U32                           rangeIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate)
{
/*
    1.1. Call with rangeIndex [0 / 15].
    Expected: GT_OK.
    1.2. Call with out of range rangeIndex [16].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      rangeIndex = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rangeIndex [0 / 15].
            Expected: GT_OK.
        */

        /* Call with enable rangeIndex [0] */
        rangeIndex = 0;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rangeIndex);

        /* Call with enable rangeIndex [15] */
        rangeIndex = 15;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rangeIndex);

        /*
            1.2. Call with out of range rangeIndex [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        rangeIndex = 16;

        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rangeIndex);
    }

    rangeIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet
(
    IN GT_U8                                   devNum,
    IN GT_U32                                  entryIndex,
    IN GT_U32                                  icmpType,
    IN CPSS_EXMXPM_SCT_ICMP_PACKET_TYPE_ENT    packetType,
    IN CPSS_IP_PROTOCOL_STACK_ENT              protocolStack,
    IN CPSS_NET_RX_CPU_CODE_ENT                cpuCode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIndex [0 / 5 / 11],
                   icmpType [0 / 65 / 255],
                   packetType [CPSS_EXMXPM_SCT_ICMP_PACKET_UC_E /
                               CPSS_EXMXPM_SCT_ICMP_PACKET_MC_E /
                               CPSS_EXMXPM_SCT_ICMP_PACKET_BOTH_UC_MC_E],
                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E /
                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                   cpuCode [CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E /
                            CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E /
                            CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet with the same entryIndex,
                                                              non-NULL validPtr,
                                                              non-NULL icmpTypePtr,
                                                              non-NULL packetTypePtr,
                                                              non-NULL protocolStackPtr
                                                              and non-NULL cpuCodePtr.
    Expected:  GT_OK, validPtr = GT_TRUE and the same icmpType, packetType, protocolStack and cpuCode.
    1.3. Call with out of range entryIndex [12]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range icmpType [256]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range packetType [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range protocolStack [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with out of range cpuCode [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate with same entryIndex to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                  entryIndex    = 0;
    GT_U32                                  icmpType      = 0;
    CPSS_EXMXPM_SCT_ICMP_PACKET_TYPE_ENT    packetType    = CPSS_EXMXPM_SCT_ICMP_PACKET_UC_E;
    CPSS_IP_PROTOCOL_STACK_ENT              protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_NET_RX_CPU_CODE_ENT                cpuCode       = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;
    GT_U32                                  icmpTypeGet   = 0;
    CPSS_EXMXPM_SCT_ICMP_PACKET_TYPE_ENT    packetTypeGet = CPSS_EXMXPM_SCT_ICMP_PACKET_UC_E;
    CPSS_IP_PROTOCOL_STACK_ENT              protStackGet  = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_NET_RX_CPU_CODE_ENT                cpuCodeGet    = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;
    GT_BOOL                                 isValid       = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryIndex [0 / 5 / 11],
                           icmpType [0 / 65 / 255],
                           packetType [CPSS_EXMXPM_SCT_ICMP_PACKET_UC_E /
                                       CPSS_EXMXPM_SCT_ICMP_PACKET_MC_E /
                                       CPSS_EXMXPM_SCT_ICMP_PACKET_BOTH_UC_MC_E],
                           protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E /
                                          CPSS_IP_PROTOCOL_IPV4V6_E],
                           cpuCode [CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E /
                                    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E /
                                    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E].
            Expected: GT_OK.
        */

        /* Call with enable entryIndex [0] */
        entryIndex = 0;
        icmpType      = 0;
        packetType    = CPSS_NET_TCP_UDP_PACKET_UC_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        cpuCode       = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet(dev, entryIndex, icmpType, packetType,
                                                        protocolStack, cpuCode);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, entryIndex, icmpType,
                                    packetType, protocolStack, cpuCode);

        /*
            1.2. Call cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet with the same entryIndex,
                                                                      non-NULL validPtr,
                                                                      non-NULL icmpTypePtr,
                                                                      non-NULL packetTypePtr,
                                                                      non-NULL protocolStackPtr
                                                                      and non-NULL cpuCodePtr.
            Expected:  GT_OK, validPtr = GT_TRUE and the same icmpType, packetType, protocolStack and cpuCode.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, entryIndex, &isValid, &icmpTypeGet,
                                                        &packetTypeGet, &protStackGet, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet: %d, %d", dev, entryIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValid,
                   "get invalid entry: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(icmpType, icmpTypeGet,
                       "get another icmpType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(packetType, packetTypeGet,
                       "get another packetType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(protocolStack, protStackGet,
                       "get another protocolStack than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                       "get another cpuCode than was set: %d", dev);

        /* Call with enable entryIndex [5] */
        entryIndex    = 5;
        icmpType      = 65;
        packetType    = CPSS_NET_TCP_UDP_PACKET_MC_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        cpuCode       = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet(dev, entryIndex, icmpType, packetType,
                                                        protocolStack, cpuCode);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, entryIndex, icmpType,
                                    packetType, protocolStack, cpuCode);

        /*
            1.2. Call cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet with the same entryIndex,
                                                                      non-NULL validPtr,
                                                                      non-NULL icmpTypePtr,
                                                                      non-NULL packetTypePtr,
                                                                      non-NULL protocolStackPtr
                                                                      and non-NULL cpuCodePtr.
            Expected:  GT_OK, validPtr = GT_TRUE and the same icmpType, packetType, protocolStack and cpuCode.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, entryIndex, &isValid, &icmpTypeGet,
                                                        &packetTypeGet, &protStackGet, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet: %d, %d", dev, entryIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValid,
                   "get invalid entry: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(icmpType, icmpTypeGet,
                       "get another icmpType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(packetType, packetTypeGet,
                       "get another packetType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(protocolStack, protStackGet,
                       "get another protocolStack than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                       "get another cpuCode than was set: %d", dev);

        /* Call with enable entryIndex [11] */
        entryIndex    = 11;
        icmpType      = 255;
        packetType    = CPSS_EXMXPM_SCT_ICMP_PACKET_BOTH_UC_MC_E;
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
        cpuCode       = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet(dev, entryIndex, icmpType, packetType,
                                                        protocolStack, cpuCode);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, entryIndex, icmpType,
                                    packetType, protocolStack, cpuCode);

        /*
            1.2. Call cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet with the same entryIndex,
                                                                      non-NULL validPtr,
                                                                      non-NULL icmpTypePtr,
                                                                      non-NULL packetTypePtr,
                                                                      non-NULL protocolStackPtr
                                                                      and non-NULL cpuCodePtr.
            Expected:  GT_OK, validPtr = GT_TRUE and the same icmpType, packetType, protocolStack and cpuCode.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, entryIndex, &isValid, &icmpTypeGet,
                                                        &packetTypeGet, &protStackGet, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet: %d, %d", dev, entryIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValid,
                   "get invalid entry: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(icmpType, icmpTypeGet,
                       "get another icmpType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(packetType, packetTypeGet,
                       "get another packetType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(protocolStack, protStackGet,
                       "get another protocolStack than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                       "get another cpuCode than was set: %d", dev);

        /*
            1.3. Call with out of range entryIndex [12]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        entryIndex = 12;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet(dev, entryIndex, icmpType, packetType,
                                                        protocolStack, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        entryIndex = 0;

        /*
            1.4. Call with out of range icmpType [256]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        icmpType = 256;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet(dev, entryIndex, icmpType, packetType,
                                                        protocolStack, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, icmpType = %d", dev, icmpType);

        icmpType = 0;

        /*
            1.5. Call with out of range packetType [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        packetType = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet(dev, entryIndex, icmpType, packetType,
                                                        protocolStack, cpuCode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, packetType = %d", dev, packetType);

        packetType = CPSS_NET_TCP_UDP_PACKET_UC_E;

        /*
            1.6. Call with out of range protocolStack [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolStack = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet(dev, entryIndex, icmpType, packetType,
                                                        protocolStack, cpuCode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, protocolStack = %d", dev, protocolStack);

        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

        /*
            1.7. Call with out of range cpuCode [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        cpuCode = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet(dev, entryIndex, icmpType, packetType,
                                                        protocolStack, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cpuCode = %d", dev, cpuCode);

        cpuCode = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;

        /*
            1.8. Call cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate with same entryIndex to invalidate changes.
            Expected: GT_OK.
        */

        /* Call with enable entryIndex [0] */
        entryIndex = 0;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate: %d, %d", dev, entryIndex);

        /* Call with enable entryIndex [5] */
        entryIndex = 5;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate: %d, %d", dev, entryIndex);

        /* Call with enable entryIndex [11] */
        entryIndex = 11;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate: %d, %d", dev, entryIndex);
    }

    entryIndex    = 0;
    icmpType      = 0;
    packetType    = CPSS_NET_TCP_UDP_PACKET_UC_E;
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    cpuCode       = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet(dev, entryIndex, icmpType, packetType,
                                                        protocolStack, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet(dev, entryIndex, icmpType, packetType,
                                                    protocolStack, cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet
(
    IN GT_U8                                   devNum,
    IN GT_U32                                  entryIndex,
    OUT GT_BOOL                                *validPtr,
    OUT GT_U32                                 *icmpTypePtr,
    OUT CPSS_EXMXPM_SCT_ICMP_PACKET_TYPE_ENT   *packetTypePtr,
    OUT CPSS_IP_PROTOCOL_STACK_ENT             *protocolStackPtr,
    OUT CPSS_NET_RX_CPU_CODE_ENT               *cpuCodePtr

)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIndex [0 / 11],
                   non-NULL validPtr,
                   non-NULL icmpTypePtr,
                   non-NULL packetTypePtr,
                   non-NULL protocolStackPtr
                   and non-NULL cpuCodePtr.
    Expected: GT_OK.
    1.2. Call with out of range entryIndex [12]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.3. Call with validPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with icmpTypePtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with packetTypePtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call with protocolStackPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.7. Call with cpuCodePtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                  entryIndex    = 0;
    GT_U32                                  icmpType      = 0;
    CPSS_EXMXPM_SCT_ICMP_PACKET_TYPE_ENT    packetType    = CPSS_EXMXPM_SCT_ICMP_PACKET_UC_E;
    CPSS_IP_PROTOCOL_STACK_ENT              protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_NET_RX_CPU_CODE_ENT                cpuCode       = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;
    GT_BOOL                                 isValid       = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryIndex [0 / 11],
                           non-NULL validPtr,
                           non-NULL icmpTypePtr,
                           non-NULL packetTypePtr,
                           non-NULL protocolStackPtr
                           and non-NULL cpuCodePtr.
            Expected: GT_OK.
        */

        /* Call with enable entryIndex [0] */
        entryIndex = 0;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, entryIndex, &isValid, &icmpType,
                                                        &packetType, &protocolStack, &cpuCode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /* Call with enable entryIndex [11] */
        entryIndex = 11;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, entryIndex, &isValid, &icmpType,
                                                        &packetType, &protocolStack, &cpuCode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call with out of range entryIndex [12]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        entryIndex = 16;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, entryIndex, &isValid, &icmpType,
                                                        &packetType, &protocolStack, &cpuCode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        entryIndex = 0;

        /*
            1.3. Call with validPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, entryIndex, NULL, &icmpType,
                                                        &packetType, &protocolStack, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);

        /*
            1.4. Call with icmpTypePtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, entryIndex, &isValid, NULL,
                                                        &packetType, &protocolStack, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, icmpTypePtr = NULL", dev);

        /*
            1.5. Call with packetTypePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, entryIndex, &isValid, &icmpType,
                                                        NULL, &protocolStack, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, packetTypePtr = NULL", dev);

        /*
            1.6. Call with protocolStackPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, entryIndex, &isValid, &icmpType,
                                                        &packetType, NULL, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, protocolStackPtr = NULL", dev);

        /*
            1.8. Call with cpuCodePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, entryIndex, &isValid, &icmpType,
                                                        &packetType, &protocolStack, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cpuCodePtr = NULL", dev);
    }

    entryIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, entryIndex, &isValid, &icmpType,
                                                        &packetType, &protocolStack, &cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, entryIndex, &isValid, &icmpType,
                                                        &packetType, &protocolStack, &cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with rangeIndex [0 / 11].
    Expected: GT_OK.
    1.2. Call with out of range entryIndex [12].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      entryIndex = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryIndex [0 / 11].
            Expected: GT_OK.
        */

        /* Call with enable entryIndex [0] */
        entryIndex = 0;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /* Call with enable entryIndex [11] */
        entryIndex = 11;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call with out of range entryIndex [12]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        entryIndex = 12;

        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate(dev, entryIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);
    }

    entryIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate(dev, entryIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet
(
    IN GT_U8            devNum,
    IN GT_U32           index,
    IN GT_U32           protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT   cpuCode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 7],
                   protocol [0 / 255]
                   and cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E /
                                CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E /
                                CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E /
                                CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet with the same index,
                                                                non-NULL validPtr,
                                                                non-NULL protocolPtr
                                                                and non-NULL cpuCodePtr.
    Expected: GT_OK valid = GT_TRUE and the same cpuCode.
    1.3. Call with out of range index [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range protocol [256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range cpuCode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate with same entryIndex to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      index      = 0;
    GT_U32                      protocol   = 0;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode    = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
    GT_U32                      protGet    = 0;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCodeGet = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
    GT_BOOL                     isValid    = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 7],
                           protocol [0 / 255]
                           and cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E /
                                        CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E /
                                        CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E /
                                        CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E].
            Expected: GT_OK.
        */

        /* Call with cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E] */
        index    = 0;
        protocol = 0;
        cpuCode  = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet(dev, index, protocol, cpuCode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet with the same index,
                                                                        non-NULL validPtr,
                                                                        non-NULL protocolPtr
                                                                        and non-NULL cpuCodePtr.
            Expected: GT_OK valid = GT_TRUE and the same cpuCode.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(dev, index, &isValid, &protGet, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValid,
                   "get invalid entry: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(protocol, protGet,
                   "get another protocol than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                   "get another cpuCode than was set: %d", dev);

        /* Call with cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E] */
        cpuCode = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet(dev, index, protocol, cpuCode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet with the same index,
                                                                        non-NULL validPtr,
                                                                        non-NULL protocolPtr
                                                                        and non-NULL cpuCodePtr.
            Expected: GT_OK valid = GT_TRUE and the same cpuCode.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(dev, index, &isValid, &protGet, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValid,
                   "get invalid entry: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(protocol, protGet,
                   "get another protocol than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                   "get another cpuCode than was set: %d", dev);

        /* Call with cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E] */
        index    = 7;
        protocol = 255;
        cpuCode  = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet(dev, index, protocol, cpuCode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet with the same index,
                                                                        non-NULL validPtr,
                                                                        non-NULL protocolPtr
                                                                        and non-NULL cpuCodePtr.
            Expected: GT_OK valid = GT_TRUE and the same cpuCode.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(dev, index, &isValid, &protGet, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValid,
                   "get invalid entry: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(protocol, protGet,
                   "get another protocol than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                   "get another cpuCode than was set: %d", dev);

        /* Call with cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E] */
        cpuCode = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet(dev, index, protocol, cpuCode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, protocol, cpuCode);

        /*
            1.2. Call cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet with the same index,
                                                                        non-NULL validPtr,
                                                                        non-NULL protocolPtr
                                                                        and non-NULL cpuCodePtr.
            Expected: GT_OK valid = GT_TRUE and the same cpuCode.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(dev, index, &isValid, &protGet, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValid,
                   "get invalid entry: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(protocol, protGet,
                   "get another protocol than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                   "get another cpuCode than was set: %d", dev);

        /*
            1.3. Call with out of range index [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet(dev, index, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.4. Call with out of range protocol [256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocol = 256;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet(dev, index, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, protocol = %d", dev, protocol);

        protocol = 0;

        /*
            1.5. Call with out of range cpuCode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        cpuCode = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet(dev, index, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cpuCode = %d", dev, cpuCode);

        cpuCode = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;

        /*
            1.6. Call cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate with same entryIndex to invalidate changes.
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index = 0;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate: %d, %d", dev, index);

        /* Call with index [7] */
        index = 7;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate: %d, %d", dev, index);
    }

    index    = 0;
    protocol = 0;
    cpuCode  = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet(dev, index, protocol, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet(dev, index, protocol, cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet
(
    IN GT_U8                        devNum,
    IN GT_U32                       index,
    OUT  GT_BOOL                    *validPtr,
    OUT  GT_U32                     *protocolPtr,
    OUT  CPSS_NET_RX_CPU_CODE_ENT   *cpuCodePtr

)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 7],
                   non-NULL validPtr,
                   non-NULL protocolPtr
                   and non-NULL cpuCodePtr.
    Expected: GT_OK.
    1.2. Call with out of range index [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with validPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with protocolPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with cpuCodePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      index    = 0;
    GT_U32                      protocol = 0;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode  = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
    GT_BOOL                     isValid  = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 7],
                           non-NULL validPtr,
                           non-NULL protocolPtr
                           and non-NULL cpuCodePtr.
            Expected: GT_OK.
        */

        /* Call with enable index [0] */
        index = 0;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(dev, index, &isValid, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with enable index [7] */
        index = 7;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(dev, index, &isValid, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call with out of range index [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 16;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(dev, index, &isValid, &protocol, &cpuCode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call with validPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(dev, index, NULL, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);


        /*
            1.4. Call with protocolPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(dev, index, &isValid, NULL, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, protocolPtr = NULL", dev);

        /*
            1.5. Call with cpuCodePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(dev, index, &isValid, &protocol, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cpuCodePtr = NULL", dev);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(dev, index, &isValid, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(dev, index, &isValid, &protocol, &cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with rangeIndex [0 / 7].
    Expected: GT_OK.
    1.2. Call with out of range entryIndex [8].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      entryIndex = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryIndex [0 / 7].
            Expected: GT_OK.
        */

        /* Call with enable entryIndex [0] */
        entryIndex = 0;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /* Call with enable entryIndex [7] */
        entryIndex = 7;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call with out of range entryIndex [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        entryIndex = 8;

        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate(dev, entryIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);
    }

    entryIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate(dev, entryIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate(dev, entryIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCpuCodeTableSet
(
    IN GT_U8                                     devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT                  cpuCode,
    IN CPSS_EXMXPM_SCT_CPU_CODE_TABLE_ENTRY_STC  *entryInfoPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeTableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cpuCode [CPSS_NET_ALL_CPU_OPCODES_E /
                            CPSS_NET_UDP_BC_MIRROR_TRAP0_E],
                   entryInfoPtr {tc [0 / 7],
                                 dp [CPSS_DP_RED_E / CPSS_DP_GREEN_E],
                                 truncate [GT_FALSE / GT_TRUE],
                                 cpuCodeRateLimiterIndex  [0 / 31],
                                 cpuCodeStatRateLimitIndex  [0 / 31],
                                 TrgCpuDestIndex [0 / 7],
                                 DropCounterEnable [GT_FALSE / GT_TRUE]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctCpuCodeTableGet with the same cpuCode and non-NULL entryInfoPtr.
    Expected: GT_OK and the same entryInfoPtr.
    1.3. Call with out of range cpuCode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range entryInfoPtr->tc [CPSS_TC_RANGE_CNS]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range entryInfoPtr->dp [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range entryInfoPtr->cpuCodeRateLimiterIndex [256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range entryInfoPtr->cpuCodeStatRateLimitIndex [32]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with out of range entryInfoPtr->TrgCpuDestIndex [32]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with entryInfoPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_NET_RX_CPU_CODE_ENT                  cpuCode = CPSS_NET_ALL_CPU_OPCODES_E;
    CPSS_EXMXPM_SCT_CPU_CODE_TABLE_ENTRY_STC  entryInfo;
    CPSS_EXMXPM_SCT_CPU_CODE_TABLE_ENTRY_STC  entryInfoGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cpuCode [CPSS_NET_ALL_CPU_OPCODES_E /
                                    CPSS_NET_UDP_BC_MIRROR_TRAP0_E],
                           entryInfoPtr {tc [0 / 7],
                                         dp [CPSS_DP_RED_E / CPSS_DP_GREEN_E],
                                         truncate [GT_FALSE / GT_TRUE],
                                         cpuCodeRateLimiterIndex  [0 / 31],
                                         cpuCodeStatRateLimitIndex  [0 / 31],
                                         TrgCpuDestIndex [0 / 7],
                                         DropCounterEnable [GT_FALSE / GT_TRUE]}.
            Expected: GT_OK.
        */

        /* Call with enable cpuCode [CPSS_NET_ALL_CPU_OPCODES_E] */
        cpuCode = CPSS_NET_ALL_CPU_OPCODES_E;

        entryInfo.tc                        = 0;
        entryInfo.dp                        = CPSS_DP_RED_E;
        entryInfo.truncate                  = GT_FALSE;
        entryInfo.cpuCodeRateLimiterIndex   = 0;
        entryInfo.cpuCodeStatRateLimitIndex = 0;
        entryInfo.TrgCpuDestIndex           = 0;
        entryInfo.DropCounterEnable         = GT_FALSE;

        st = cpssExMxPmSctCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        cpuCode = CPSS_NET_UDP_BC_MIRROR_TRAP1_E;

        /*
            1.2. Call cpssExMxPmSctCpuCodeTableGet with the same cpuCode and non-NULL entryInfoPtr.
            Expected: GT_OK and the same entryInfoPtr.
        */
        st = cpssExMxPmSctCpuCodeTableGet(dev, cpuCode, &entryInfoGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctCpuCodeTableGet: %d, %d", dev, cpuCode);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(entryInfo.tc, entryInfoGet.tc,
                   "get another entryInfoPtr->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entryInfo.dp, entryInfoGet.dp,
                   "get another entryInfoPtr->dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entryInfo.truncate, entryInfoGet.truncate,
                   "get another entryInfoPtr->truncate than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entryInfo.cpuCodeRateLimiterIndex, entryInfoGet.cpuCodeRateLimiterIndex,
                   "get another entryInfoPtr->cpuCodeRateLimiterIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entryInfo.cpuCodeStatRateLimitIndex, entryInfoGet.cpuCodeStatRateLimitIndex,
                   "get another entryInfoPtr->cpuCodeStatRateLimitIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entryInfo.TrgCpuDestIndex, entryInfoGet.TrgCpuDestIndex,
                   "get another entryInfoPtr->TrgCpuDestIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entryInfo.DropCounterEnable, entryInfoGet.DropCounterEnable,
                   "get another entryInfoPtr->DropCounterEnable than was set: %d", dev);

        /* Call with enable cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP0_E] */
        cpuCode = CPSS_NET_UDP_BC_MIRROR_TRAP0_E;

        entryInfo.tc                        = 7;
        entryInfo.dp                        = CPSS_DP_GREEN_E;
        entryInfo.truncate                  = GT_TRUE;
        entryInfo.cpuCodeRateLimiterIndex   = 31;
        entryInfo.cpuCodeStatRateLimitIndex = 31;
        entryInfo.TrgCpuDestIndex           = 7;
        entryInfo.DropCounterEnable         = GT_TRUE;

        st = cpssExMxPmSctCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        /*
            1.2. Call cpssExMxPmSctCpuCodeTableGet with the same cpuCode and non-NULL entryInfoPtr.
            Expected: GT_OK and the same entryInfoPtr.
        */
        st = cpssExMxPmSctCpuCodeTableGet(dev, cpuCode, &entryInfoGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctCpuCodeTableGet: %d, %d", dev, cpuCode);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(entryInfo.tc, entryInfoGet.tc,
                   "get another entryInfoPtr->tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entryInfo.dp, entryInfoGet.dp,
                   "get another entryInfoPtr->dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entryInfo.truncate, entryInfoGet.truncate,
                   "get another entryInfoPtr->truncate than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entryInfo.cpuCodeRateLimiterIndex, entryInfoGet.cpuCodeRateLimiterIndex,
                   "get another entryInfoPtr->cpuCodeRateLimiterIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entryInfo.cpuCodeStatRateLimitIndex, entryInfoGet.cpuCodeStatRateLimitIndex,
                   "get another entryInfoPtr->cpuCodeStatRateLimitIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entryInfo.TrgCpuDestIndex, entryInfoGet.TrgCpuDestIndex,
                   "get another entryInfoPtr->TrgCpuDestIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(entryInfo.DropCounterEnable, entryInfoGet.DropCounterEnable,
                   "get another entryInfoPtr->DropCounterEnable than was set: %d", dev);

        /*
            1.3. Call with out of range cpuCode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        cpuCode = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        cpuCode = CPSS_NET_ALL_CPU_OPCODES_E;

        /*
            1.4. Call with out of range entryInfoPtr->tc [CPSS_TC_RANGE_CNS]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        entryInfo.tc = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmSctCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryInfoPtr->tc = %d", dev, entryInfo.tc);

        entryInfo.tc = 0;

        /*
            1.5. Call with out of range entryInfoPtr->dp [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        entryInfo.dp = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryInfoPtr->dp = %d", dev, entryInfo.dp);

        entryInfo.dp = 0;

        /*
            1.6. Call with out of range entryInfoPtr->cpuCodeRateLimiterIndex [256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        entryInfo.cpuCodeRateLimiterIndex = 256;

        st = cpssExMxPmSctCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryInfoPtr->cpuCodeRateLimiterIndex = %d",
                                         dev, entryInfo.cpuCodeRateLimiterIndex);

        entryInfo.cpuCodeRateLimiterIndex = 0;

        /*
            1.7. Call with out of range entryInfoPtr->cpuCodeStatRateLimitIndex [32]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        entryInfo.cpuCodeStatRateLimitIndex = 32;

        st = cpssExMxPmSctCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryInfoPtr->cpuCodeStatRateLimitIndex = %d",
                                         dev, entryInfo.cpuCodeStatRateLimitIndex);

        entryInfo.cpuCodeStatRateLimitIndex = 0;

        /*
            1.8. Call with out of range entryInfoPtr->TrgCpuDestIndex [32]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        entryInfo.TrgCpuDestIndex = 32;

        st = cpssExMxPmSctCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryInfoPtr->TrgCpuDestIndex = %d",
                                         dev, entryInfo.TrgCpuDestIndex);

        entryInfo.TrgCpuDestIndex = 0;

        /*
            1.9. Call with entryInfoPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctCpuCodeTableSet(dev, cpuCode, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryInfoPtr = NULL", dev);
    }

    cpuCode = CPSS_NET_ALL_CPU_OPCODES_E;

    entryInfo.tc                        = 0;
    entryInfo.dp                        = CPSS_DP_RED_E;
    entryInfo.truncate                  = GT_FALSE;
    entryInfo.cpuCodeRateLimiterIndex   = 0;
    entryInfo.cpuCodeStatRateLimitIndex = 0;
    entryInfo.TrgCpuDestIndex           = 0;
    entryInfo.DropCounterEnable         = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeTableSet(dev, cpuCode, &entryInfo);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCpuCodeTableGet
(
    IN GT_U8                                     devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT                  cpuCode,
    OUT CPSS_EXMXPM_SCT_CPU_CODE_TABLE_ENTRY_STC *entryInfoPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeTableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP1_E /
                            CPSS_NET_UDP_BC_MIRROR_TRAP0_E]
                   and non_NULL entryInfoPtr.
    Expected: GT_OK.
    1.2. Call with out of range cpuCode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with entryInfoPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_NET_RX_CPU_CODE_ENT                  cpuCode = CPSS_NET_ALL_CPU_OPCODES_E;
    CPSS_EXMXPM_SCT_CPU_CODE_TABLE_ENTRY_STC  entryInfo;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP2_E /
                                    CPSS_NET_UDP_BC_MIRROR_TRAP0_E]
                           and non_NULL entryInfoPtr.
            Expected: GT_OK.
        */

        /* Call with enable cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP1_E] */
        cpuCode = CPSS_NET_UDP_BC_MIRROR_TRAP1_E;

        st = cpssExMxPmSctCpuCodeTableGet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        /* Call with enable cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP0_E] */
        cpuCode = CPSS_NET_UDP_BC_MIRROR_TRAP0_E;

        st = cpssExMxPmSctCpuCodeTableGet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        /*
            1.2. Call with out of range cpuCode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        cpuCode = SCT_INVALID_ENUM_CNS;

        st = cpssExMxPmSctCpuCodeTableGet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        cpuCode = CPSS_NET_ALL_CPU_OPCODES_E;

        /*
            1.3. Call with entryInfoPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctCpuCodeTableGet(dev, cpuCode, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryInfoPtr = NULL", dev);
    }

    cpuCode = CPSS_NET_ALL_CPU_OPCODES_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeTableGet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeTableGet(dev, cpuCode, &entryInfo);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet
(
    IN GT_U8  devNum,
    IN GT_U32 windowResolution
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with windowResolution [fieldResolution / fieldResolution*2].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet with non-NULL windowResolutionPtr.
    Expected: GT_OK and the same windowResolution.
    1.3. Call with out of range windowResolution [0].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      windowResolution = 0;
    GT_U32      windowResGet     = 0;
    GT_U32      coreClock        = 0;
    GT_U32      fieldResolution  = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting core clock */
        st = prvUtfCoreClockGet(dev, &coreClock);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCoreClockGet: %d", dev);

        fieldResolution = (1000 * 256) / coreClock;

        /*
            1.1. Call with windowResolution [fieldResolution / fieldResolution*2].
            Expected: GT_OK.
        */

        /* Call with enable windowResolution [fieldResolution] */
        windowResolution = fieldResolution;

        st = cpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet(dev, windowResolution);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, windowResolution);

        /*
            1.2. Call cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet with non-NULL windowResolutionPtr.
            Expected: GT_OK and the same windowResolution.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet(dev, &windowResGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(windowResolution, windowResGet,
                   "get another windowResolution than was set: %d", dev);

        /* Call with enable windowResolution [fieldResolution*2] */
        windowResolution = fieldResolution * 2;

        st = cpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet(dev, windowResolution);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, windowResolution);

        /*
            1.2. Call cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet with non-NULL windowResolutionPtr.
            Expected: GT_OK and the same windowResolution.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet(dev, &windowResGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(windowResolution, windowResGet,
                   "get another windowResolution than was set: %d", dev);

        /*
            1.3. Call with out of range windowResolution [0].
            Expected: NOT GT_OK.
        */
        windowResolution = 0;

        st = cpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet(dev, windowResolution);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, windowResolution);
    }

    windowResolution = fieldResolution;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet(dev, windowResolution);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet(dev, windowResolution);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet
(
    IN GT_U8   devNum,
    OUT GT_U32 *windowResolutionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null windowResolutionPtr.
    Expected: GT_OK.
    1.2. Call with windowResolutionPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      windowResolution = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null windowResolutionPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet(dev, &windowResolution);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null windowResolutionPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, windowResolutionPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet(dev, &windowResolution);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet(dev, &windowResolution);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCpuCodeRateLimiterTableSet
(
    IN GT_U8  devNum,
    IN GT_U32 rateLimiterIndex,
    IN GT_U32 windowSize,
    IN GT_U32 pktLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeRateLimiterTableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with rateLimiterIndex [1 / 0xFF],
                   windowSize [0 / 0xFFF],
                   pktLimit [0 / 0xFFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctCpuCodeRateLimiterTableGet with the same rateLimiterIndex
                                                           and non-NULL windowSizePtr
                                                           non-NULL pktLimitPtr.
    Expected: GT_OK and the same windowSize , pktLimit.

    1.3. Call with out of range rateLimiterIndex [0 / 256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range windowSize [0xFFF + 1]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range pktLimit [0xFFFFF + 1]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      rateLimiterIndex = 0;
    GT_U32      windowSize       = 0;
    GT_U32      pktLimit         = 0;
    GT_U32      windowSizeGet    = 0;
    GT_U32      pktLimitGet      = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rateLimiterIndex [1 / 0xFF],
                           windowSize [0 / 0xFFF],
                           pktLimit [0 / 0xFFFFF].
            Expected: GT_OK.
        */

        /* Call with enable rateLimiterIndex [1] */
        rateLimiterIndex = 1;
        windowSize       = 0;
        pktLimit         = 0;

        st = cpssExMxPmSctCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, windowSize, pktLimit);

        /*
            1.2. Call cpssExMxPmSctCpuCodeRateLimiterTableGet with the same rateLimiterIndex
                                                                   and non-NULL windowSizePtr
                                                                   non-NULL pktLimitPtr.
            Expected: GT_OK and the same windowSize , pktLimit.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSizeGet, &pktLimitGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctCpuCodeRateLimiterTableGet: %d, %d", dev, rateLimiterIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(windowSize, windowSizeGet,
                   "get another windowSize than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pktLimit, pktLimitGet,
                   "get another pktLimit than was set: %d", dev);

        /* Call with enable rateLimiterIndex [0xFF] */
        rateLimiterIndex = 0xFF;
        windowSize       = 0xFFF;
        pktLimit         = 0xFFFFF;

        st = cpssExMxPmSctCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, windowSize, pktLimit);

        /*
            1.2. Call cpssExMxPmSctCpuCodeRateLimiterTableGet with the same rateLimiterIndex
                                                                   and non-NULL windowSizePtr
                                                                   non-NULL pktLimitPtr.
            Expected: GT_OK and the same windowSize , pktLimit.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSizeGet, &pktLimitGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctCpuCodeRateLimiterTableGet: %d, %d", dev, rateLimiterIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(windowSize, windowSizeGet,
                   "get another windowSize than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pktLimit, pktLimitGet,
                   "get another pktLimit than was set: %d", dev);


        /*
            1.4. Call with out of range rateLimiterIndex [0 / 256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with rateLimiterIndex [0] */
        rateLimiterIndex = 0;

        st = cpssExMxPmSctCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        /* Call with rateLimiterIndex [256] */
        rateLimiterIndex = 256;

        st = cpssExMxPmSctCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        rateLimiterIndex = 1;

        /*
            1.5. Call with out of range windowSize [0xFFF + 1]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        windowSize = 4096;

        st = cpssExMxPmSctCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, windowSize = %d", dev, windowSize);

        windowSize = 0;

        /*
            1.6. Call with out of range pktLimit [0xFFFFF + 1]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        pktLimit = 0x100000;

        st = cpssExMxPmSctCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pktLimit = %d", dev, pktLimit);
    }

    rateLimiterIndex = 1;
    windowSize       = 0;
    pktLimit         = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCpuCodeRateLimiterTableGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  rateLimiterIndex,
    OUT GT_U32  *windowSizePtr,
    OUT GT_U32  *pktLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeRateLimiterTableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with rateLimiterIndex [1 / 0xFF],
                   non-NULL windowSizePtr non-NULL pktLimitPtr.
    Expected: GT_OK.
    1.2. Call with out of range rateLimiterIndex [0 / 256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with windowSizePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with pktLimitPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      rateLimiterIndex = 0;
    GT_U32      windowSize       = 0;
    GT_U32      pktLimit         = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rateLimiterIndex [1 / 0xFF],
                           non-NULL windowSizePtr non-NULL pktLimitPtr.
            Expected: GT_OK.
        */

        /* Call with enable rateLimiterIndex [1] */
        rateLimiterIndex = 1;

        st = cpssExMxPmSctCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSize, &pktLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        /* Call with enable rateLimiterIndex [0xFF] */
        rateLimiterIndex = 0xFF;

        st = cpssExMxPmSctCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSize, &pktLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        /*
            1.2. Call with out of range rateLimiterIndex [0 / 256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with rateLimiterIndex [0] */
        rateLimiterIndex = 0;

        st = cpssExMxPmSctCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSize, &pktLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        /* Call with rateLimiterIndex [256] */
        rateLimiterIndex = 256;

        st = cpssExMxPmSctCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSize, &pktLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        rateLimiterIndex = 1;

        /*
            1.3. Call with windowSizePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, NULL, &pktLimit);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, windowSizePtr = NULL", dev);

        /*
            1.4. Call with pktLimitPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSize, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pktLimitPtr = NULL", dev);
    }

    rateLimiterIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSize, &pktLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSize, &pktLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCpuCodeRateLimiterPacketCntrGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  rateLimiterIndex,
    OUT GT_U32  *packetsCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeRateLimiterPacketCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with rateLimiterIndex [1 / 0xFF]
                   and non-NULL packetsCntrPtr.
    Expected: GT_OK.
    1.2. Call with out of range rateLimiterIndex [0 / 256]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with packetsCntrPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      rateLimiterIndex = 0;
    GT_U32      pktLimit         = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rateLimiterIndex [1 / 0xFF]
                           and non-NULL packetsCntrPtr.
            Expected: GT_OK.
        */

        /* Call with enable rateLimiterIndex [1] */
        rateLimiterIndex = 1;

        st = cpssExMxPmSctCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, &pktLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        /* Call with enable rateLimiterIndex [0xFF] */
        rateLimiterIndex = 0xFF;

        st = cpssExMxPmSctCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, &pktLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        /*
            1.2. Call with out of range rateLimiterIndex [0 / 256]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with rateLimiterIndex [0] */
        rateLimiterIndex = 0;

        st = cpssExMxPmSctCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, &pktLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        /* Call with rateLimiterIndex [256] */
        rateLimiterIndex = 256;

        st = cpssExMxPmSctCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, &pktLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        rateLimiterIndex = 1;

        /*
            1.3. Call with packetsCntrPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, packetsCntrPtr = NULL", dev);
    }

    rateLimiterIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, &pktLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, &pktLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/


/*
GT_STATUS cpssExMxPmSctCpuCodeRateLimiterDropCntrGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *valuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeRateLimiterDropCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null valuePtr.
    Expected: GT_OK.
    1.2. Call with valuePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      value = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null valuePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterDropCntrGet(dev, &value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null valuePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterDropCntrGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, valuePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeRateLimiterDropCntrGet(dev, &value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeRateLimiterDropCntrGet(dev, &value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/

/*
GT_STATUS cpssExMxPmSctCpuCodeStatisticalRateLimitsTableSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_U32       statisticalRateLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeStatisticalRateLimitsTableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 16 / 31]
                   and statisticalRateLimit [0 / 0xFFFF / 0xFFFFFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet with the same index
                                                                     and non-NULL statisticalRateLimitPtr.
    Expected: GT_OK and the same statisticalRateLimit.
    1.3. Call with out of range index [32]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32       index          = 0;
    GT_U32       statRateLimit  = 0;
    GT_U32       statRateLimGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 16 / 31]
                           and statisticalRateLimit [0 / 0xFFFF / 0xFFFFFFFF].
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index         = 0;
        statRateLimit = 0;

        st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableSet(dev, index, statRateLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, statRateLimit);

        /*
            1.2. Call cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet with the same index
                                                                             and non-NULL statisticalRateLimitPtr.
            Expected: GT_OK and the same statisticalRateLimit.
        */
        st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet(dev, index, &statRateLimGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(statRateLimit, statRateLimGet,
                   "get another statRateLimit than was set: %d", dev);

        /* Call with index [16] */
        index         = 16;
        statRateLimit = 0xFFFF;

        st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableSet(dev, index, statRateLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, statRateLimit);

        /*
            1.2. Call cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet with the same index
                                                                             and non-NULL statisticalRateLimitPtr.
            Expected: GT_OK and the same statisticalRateLimit.
        */
        st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet(dev, index, &statRateLimGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(statRateLimit, statRateLimGet,
                   "get another statRateLimit than was set: %d", dev);

        /* Call with index [31] */
        index         = 31;
        statRateLimit = 0xFFFFFFFF;

        st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableSet(dev, index, statRateLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, statRateLimit);

        /*
            1.2. Call cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet with the same index
                                                                             and non-NULL statisticalRateLimitPtr.
            Expected: GT_OK and the same statisticalRateLimit.
        */
        st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet(dev, index, &statRateLimGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(statRateLimit, statRateLimGet,
                   "get another statRateLimit than was set: %d", dev);

        /*
            1.3. Call with out of range index [32]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 32;

        st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableSet(dev, index, statRateLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);
    }

    index         = 0;
    statRateLimit = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableSet(dev, index, statRateLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableSet(dev, index, statRateLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    OUT GT_U32      *statisticalRateLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 31]
                   and non-NULL statisticalRateLimitPtr.
    Expected: GT_OK.
    1.2. Call with out of range index [32]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with statisticalRateLimitPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32       index         = 0;
    GT_U32       statRateLimit = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 31]
                           and non-NULL statisticalRateLimitPtr.
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index = 0;

        st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet(dev, index, &statRateLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with index [31] */
        index = 31;

        st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet(dev, index, &statRateLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.3. Call with out of range index [32]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 32;

        st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet(dev, index, &statRateLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call with statisticalRateLimitPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet(dev, index, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, statisticalRateLimitPtr = NULL", dev);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet(dev, index, &statRateLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet(dev, index, &statRateLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctPortDuplicateToCpuEnableSet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    IN  GT_BOOL                        enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctPortDuplicateToCpuEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmSctPortDuplicateToCpuEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = SCT_VALID_VIRT_PORT_CNS;

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

            st = cpssExMxPmSctPortDuplicateToCpuEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmSctPortDuplicateToCpuEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmSctPortDuplicateToCpuEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctPortDuplicateToCpuEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmSctPortDuplicateToCpuEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmSctPortDuplicateToCpuEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmSctPortDuplicateToCpuEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctPortDuplicateToCpuEnableGet: %d, %d", dev, port);

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
            st = cpssExMxPmSctPortDuplicateToCpuEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmSctPortDuplicateToCpuEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmSctPortDuplicateToCpuEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = SCT_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctPortDuplicateToCpuEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctPortDuplicateToCpuEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctPortDuplicateToCpuEnableGet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    OUT GT_BOOL                        *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctPortDuplicateToCpuEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = SCT_VALID_VIRT_PORT_CNS;

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
            st = cpssExMxPmSctPortDuplicateToCpuEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmSctPortDuplicateToCpuEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmSctPortDuplicateToCpuEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmSctPortDuplicateToCpuEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmSctPortDuplicateToCpuEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = SCT_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctPortDuplicateToCpuEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctPortDuplicateToCpuEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctDuplicateEnableSet
(
    IN  GT_U8                          devNum,
    IN  GT_BOOL                        enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctDuplicateEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmSctDuplicateEnableGet with non-NULL enablePtr.
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

        st = cpssExMxPmSctDuplicateEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmSctDuplicateEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmSctDuplicateEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctDuplicateEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxPmSctDuplicateEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmSctDuplicateEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmSctDuplicateEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctDuplicateEnableGet: %d", dev);

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
        st = cpssExMxPmSctDuplicateEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctDuplicateEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctDuplicateEnableGet
(
    IN  GT_U8                          devNum,
    OUT GT_BOOL                        *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctDuplicateEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
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
        st = cpssExMxPmSctDuplicateEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctDuplicateEnableGet(dev, NULL);
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
        st = cpssExMxPmSctDuplicateEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctDuplicateEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCpuCodeTrgDevPortTableSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_U8        trgDevice,
    IN GT_U8        trgPort
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeTrgDevPortTableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 31],
                   trgDevice [0 / devNum]
                   and trgPort [0 / 12].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctCpuCodeTrgDevPortTableGet with the same index,
                                                          non-NULL trgDevicePtr
                                                          and non-NULL trgPortPtr.
    Expected: GT_OK and the same trgDevice and trgPort.
    1.3. Call with out of range index [32]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range trgDevice [PRV_CPSS_MAX_PP_DEVICES_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range trgPort [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      index      = 0;
    GT_U8       trgDevice  = 0;
    GT_U8       trgPort    = 0;
    GT_U8       trgDevGet  = 0;
    GT_U8       trgPortGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 31],
                           trgDevice [0 / devNum]
                           and trgPort [0 / 12].
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index     = 0;
        trgDevice = 0;
        trgPort   = 0;

        st = cpssExMxPmSctCpuCodeTrgDevPortTableSet(dev, index, trgDevice, trgPort);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, trgDevice, trgPort);

        /*
            1.2. Call cpssExMxPmSctCpuCodeTrgDevPortTableGet with the same index,
                                                                  non-NULL trgDevicePtr
                                                                  and non-NULL trgPortPtr.
            Expected: GT_OK and the same trgDevice and trgPort.
        */
        st = cpssExMxPmSctCpuCodeTrgDevPortTableGet(dev, index, &trgDevGet, &trgPortGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctCpuCodeTrgDevPortTableGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(trgDevice, trgDevGet,
                   "get another trgDevice than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(trgPort, trgPortGet,
                   "get another trgPort than was set: %d", dev);

        /* Call with index [31] */
        index     = 31;
        trgDevice = dev;
        trgPort   = 12;

        st = cpssExMxPmSctCpuCodeTrgDevPortTableSet(dev, index, trgDevice, trgPort);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, trgDevice, trgPort);

        /*
            1.2. Call cpssExMxPmSctCpuCodeTrgDevPortTableGet with the same index,
                                                                  non-NULL trgDevicePtr
                                                                  and non-NULL trgPortPtr.
            Expected: GT_OK and the same trgDevice and trgPort.
        */
        st = cpssExMxPmSctCpuCodeTrgDevPortTableGet(dev, index, &trgDevGet, &trgPortGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmSctCpuCodeTrgDevPortTableGet: %d, %d", dev, index);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(trgDevice, trgDevGet,
                   "get another trgDevice than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(trgPort, trgPortGet,
                   "get another trgPort than was set: %d", dev);

        /*
            1.3. Call with out of range index [32]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 32;

        st = cpssExMxPmSctCpuCodeTrgDevPortTableSet(dev, index, trgDevice, trgPort);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.4. Call with out of range trgDevice [PRV_CPSS_MAX_PP_DEVICES_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        trgDevice = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmSctCpuCodeTrgDevPortTableSet(dev, index, trgDevice, trgPort);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, trgDevice = %d", dev, trgDevice);

        trgDevice = dev;

        /*
            1.5. Call with out of range trgDevice [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        trgPort = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmSctCpuCodeTrgDevPortTableSet(dev, index, trgDevice, trgPort);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, trgPort = %d", dev, trgPort);
    }

    index     = 0;
    trgDevice = 0;
    trgPort   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeTrgDevPortTableSet(dev, index, trgDevice, trgPort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeTrgDevPortTableSet(dev, index, trgDevice, trgPort);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCpuCodeTrgDevPortTableGet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    OUT GT_U8       *trgDevicePtr,
    OUT GT_U8       *trgPortPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeTrgDevPortTableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 31],
                   non-NULL trgDevicePtr
                   and non-NULL trgPortPtr.
    Expected: GT_OK.
    1.2. Call with out of range index [32]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with trgDevicePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with trgPortPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      index     = 0;
    GT_U8       trgDevice = 0;
    GT_U8       trgPort   = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 31],
                           non-NULL trgDevicePtr
                           and non-NULL trgPortPtr.
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index = 0;

        st = cpssExMxPmSctCpuCodeTrgDevPortTableGet(dev, index, &trgDevice, &trgPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with index [31] */
        index = 31;

        st = cpssExMxPmSctCpuCodeTrgDevPortTableGet(dev, index, &trgDevice, &trgPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call with out of range index [32]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        index = 32;

        st = cpssExMxPmSctCpuCodeTrgDevPortTableGet(dev, index, &trgDevice, &trgPort);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call with trgDevicePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctCpuCodeTrgDevPortTableGet(dev, index, NULL, &trgPort);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, trgDevicePtr = NULL", dev);

        /*
            1.4. Call with trgPortPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctCpuCodeTrgDevPortTableGet(dev, index, &trgDevice, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, trgDevicePtr = NULL", dev);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeTrgDevPortTableGet(dev, index, &trgDevice, &trgPort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeTrgDevPortTableGet(dev, index, &trgDevice, &trgPort);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIeeeReservedMcTrappedLearnEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIeeeReservedMcTrappedLearnEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet with non-NULL enablePtr.
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

        st = cpssExMxPmSctIeeeReservedMcTrappedLearnEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxPmSctIeeeReservedMcTrappedLearnEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);
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
        st = cpssExMxPmSctIeeeReservedMcTrappedLearnEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIeeeReservedMcTrappedLearnEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
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
        st = cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet(dev, NULL);
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
        st = cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet
(
    IN GT_U8  devNum,
    IN GT_U32 rateLimiterIndex,
    IN GT_U32 windowCountdown
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with rateLimiterIndex[1 / 255], windowCountdown[0 / 0xFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet with not NULL windowCountdown.
    Expected: GT_OK and the same windowCountdown as was set.
    1.3. Call with rateLimiterIndex [0 / 256] (out of range)
    Expected: NOT GT_OK.
    1.4. Call with windowCountdown [0x1000] (out of range)
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32 rateLimiterIndex = 0;
    GT_U32 windowCountdown = 0;
    GT_U32 windowCountdownGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rateLimiterIndex[1 / 255], windowCountdown[0 / 0xFFF].
            Expected: GT_OK.
        */
        rateLimiterIndex = 1;
        windowCountdown = 0;

        st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet(dev, rateLimiterIndex, windowCountdown);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, windowCountdown);

        /*
            1.2. Call cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet with not NULL windowCountdown.
            Expected: GT_OK and the same windowCountdown as was set.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet(dev, rateLimiterIndex, &windowCountdownGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet: %d, %d", dev, rateLimiterIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(windowCountdown, windowCountdownGet, "got another windowCountdownGet than was set: %d", dev);

        rateLimiterIndex = 255;
        windowCountdown = 0xFFF;

        st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet(dev, rateLimiterIndex, windowCountdown);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, windowCountdown);

        /*
            1.2. Call cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet with not NULL windowCountdown.
            Expected: GT_OK and the same windowCountdown as was set.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet(dev, rateLimiterIndex, &windowCountdownGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet: %d, %d", dev, rateLimiterIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(windowCountdown, windowCountdownGet, "got another windowCountdownGet than was set: %d", dev);

        /*
            1.3. Call with rateLimiterIndex [0 / 256] (out of range)
            Expected: NOT GT_OK.
        */
        rateLimiterIndex = 0;

        st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet(dev, rateLimiterIndex, windowCountdown);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, windowCountdown);

        rateLimiterIndex = 256;

        st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet(dev, rateLimiterIndex, windowCountdown);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, windowCountdown);

        rateLimiterIndex = 1;

        /*
            1.4. Call with windowCountdown [0x1000] (out of range)
            Expected: NOT GT_OK.
        */
        windowCountdown = 0x1000;

        st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet(dev, rateLimiterIndex, windowCountdown);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, windowCountdown);
    }

    rateLimiterIndex = 1;
    windowCountdown = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet(dev, rateLimiterIndex, windowCountdown);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet(dev, rateLimiterIndex, windowCountdown);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  rateLimiterIndex,
    OUT GT_U32  *windowCountdownPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with rateLimiterIndex[1 / 255], not NULL windowCountdownPtr.
    Expected: GT_OK.
    1.2. Call with rateLimiterIndex [0 / 256] (out of range)
    Expected: NOT GT_OK.
    1.3. Call with windowCountdown [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;


    GT_U32  rateLimiterIndex = 0;
    GT_U32  windowCountdown = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rateLimiterIndex[1 / 255], not NULL windowCountdownPtr.
            Expected: GT_OK.
        */
        rateLimiterIndex = 1;

        st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet(dev, rateLimiterIndex, &windowCountdown);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        rateLimiterIndex = 255;

        st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet(dev, rateLimiterIndex, &windowCountdown);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        /*
            1.2. Call with rateLimiterIndex [0 / 256] (out of range)
            Expected: NOT GT_OK.
        */
        rateLimiterIndex = 0;

        st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet(dev, rateLimiterIndex, &windowCountdown);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        rateLimiterIndex = 256;

        st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet(dev, rateLimiterIndex, &windowCountdown);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        rateLimiterIndex = 1;

        /*
            1.3. Call with windowCountdown [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet(dev, rateLimiterIndex, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, "%d, windowCountdownPtr = NULL", dev);
    }

    rateLimiterIndex = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet(dev, rateLimiterIndex, &windowCountdown);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet(dev, rateLimiterIndex, &windowCountdown);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet
(
    IN GT_U8  devNum,
    IN GT_U32 rateLimiterIndex,
    IN GT_U32 pktThreshold
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with rateLimiterIndex[1 / 255], pktThreshold[1 / 0xFFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet with not NULL pktThreshold.
    Expected: GT_OK and the same pktThreshold as was set.
    1.3. Call with rateLimiterIndex [0 / 256] (out of range)
    Expected: NOT GT_OK.
    1.4. Call with pktThreshold [0 / 0x100000] (out of range)
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32 rateLimiterIndex = 0;
    GT_U32 pktThreshold = 0;
    GT_U32 pktThresholdGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rateLimiterIndex[1 / 255], pktThreshold[1 / 0xFFFFF].
            Expected: GT_OK.
        */
        rateLimiterIndex = 1;
        pktThreshold = 1;

        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet(dev, rateLimiterIndex, pktThreshold);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, pktThreshold);

        /*
            1.2. Call cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet with not NULL pktThreshold.
            Expected: GT_OK and the same pktThreshold as was set.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet(dev, rateLimiterIndex, &pktThresholdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet: %d, %d", dev, rateLimiterIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(pktThreshold, pktThresholdGet, "got another pktThresholdGet than was set: %d", dev);

        rateLimiterIndex = 255;
        pktThreshold = 0xFFFFF;

        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet(dev, rateLimiterIndex, pktThreshold);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, pktThreshold);

        /*
            1.2. Call cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet with not NULL pktThreshold.
            Expected: GT_OK and the same pktThreshold as was set.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet(dev, rateLimiterIndex, &pktThresholdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet: %d, %d", dev, rateLimiterIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(pktThreshold, pktThresholdGet, "got another pktThresholdGet than was set: %d", dev);

        /*
            1.3. Call with rateLimiterIndex [0 / 256] (out of range)
            Expected: NOT GT_OK.
        */
        rateLimiterIndex = 0;

        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet(dev, rateLimiterIndex, pktThreshold);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, pktThreshold);

        rateLimiterIndex = 256;

        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet(dev, rateLimiterIndex, pktThreshold);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, pktThreshold);

        rateLimiterIndex = 1;

        /*
            1.4. Call with pktThreshold [0 / 0x100000] (out of range)
            Expected: NOT GT_OK.
        */
        pktThreshold = 0;

        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet(dev, rateLimiterIndex, pktThreshold);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, pktThreshold);

        pktThreshold = 0x100000;

        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet(dev, rateLimiterIndex, pktThreshold);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, pktThreshold);
    }

    rateLimiterIndex = 1;
    pktThreshold = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet(dev, rateLimiterIndex, pktThreshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet(dev, rateLimiterIndex, pktThreshold);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  rateLimiterIndex,
    OUT GT_U32  *pktThresholdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with rateLimiterIndex[1 / 255], not NULL pktThresholdPtr.
    Expected: GT_OK.
    1.2. Call with rateLimiterIndex [0 / 256] (out of range)
    Expected: NOT GT_OK.
    1.3. Call with pktThreshold [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;


    GT_U32  rateLimiterIndex = 0;
    GT_U32  pktThreshold = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rateLimiterIndex[1 / 255], not NULL pktThresholdPtr.
            Expected: GT_OK.
        */
        rateLimiterIndex = 1;

        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet(dev, rateLimiterIndex, &pktThreshold);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        rateLimiterIndex = 255;

        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet(dev, rateLimiterIndex, &pktThreshold);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        /*
            1.2. Call with rateLimiterIndex [0 / 256] (out of range)
            Expected: NOT GT_OK.
        */
        rateLimiterIndex = 0;

        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet(dev, rateLimiterIndex, &pktThreshold);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        rateLimiterIndex = 256;

        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet(dev, rateLimiterIndex, &pktThreshold);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        rateLimiterIndex = 1;

        /*
            1.3. Call with pktThreshold [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet(dev, rateLimiterIndex, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, "%d, pktThresholdPtr = NULL", dev);
    }

    rateLimiterIndex = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet(dev, rateLimiterIndex, &pktThreshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet(dev, rateLimiterIndex, &pktThreshold);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill UDP Broadcast Destination Port configuration tAble.
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctFillUdpBcDestPortCfgTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in UDP Broadcast Destination Port configuration table.
         Call cpssExMxPmSctUdpBcDestPortCfgSet with entryIndex [0..numEntries - 1],
                                                   udpPortNum [0],
                                                   cpuCode [CPSS_NET_UDP_BC_MIRROR_TRAP0_E]
                                                   and cmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E].
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmSctUdpBcDestPortCfgSet with entryIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in UDP Broadcast Destination Port configuration table and compare with original.
         Call cpssExMxPmSctUdpBcDestPortCfgGet with the same entryIndex,
                                                            non-NULL udpPortNumPtr,
                                                            non-NULL cpuCodePtr
                                                            and non-NULL cmdPtr.
    Expected: GT_OK and the same udpPortNum, cpuCode and cmd.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmSctUdpBcDestPortCfgGet with entryIndex [numEntries],
    Expected: NOT GT_OK.
    1.6. Delete all entries in UDP Broadcast Destination Port configuration table.
         Call cpssExMxPmSctUdpBcDestPortCfgInvalidate with same entryIndex to invalidate changes.
    Expected: GT_OK.
    1.7. Try to delete entry with index out of range.
         Call cpssExMxPmSctUdpBcDestPortCfgInvalidate with entryIndex [numEntries].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    GT_U32                    udpPortNum    = 0;
    CPSS_NET_RX_CPU_CODE_ENT  cpuCode       = CPSS_NET_UDP_BC_MIRROR_TRAP0_E;
    CPSS_PACKET_CMD_ENT       cmd           = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
    GT_U32                    udpPortNumGet = 0;
    CPSS_NET_RX_CPU_CODE_ENT  cpuCodeGet    = CPSS_NET_UDP_BC_MIRROR_TRAP0_E;
    CPSS_PACKET_CMD_ENT       cmdGet        = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;


    /* Fill the entry for UDP Broadcast Destination Port configuration table */
    udpPortNum = 0;
    cpuCode    = CPSS_NET_UDP_BC_MIRROR_TRAP0_E;
    cmd        = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 12;

        /* 1.2. Fill all entries in UDP Broadcast Destination Port configuration table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, iTemp, udpPortNum, cpuCode, cmd);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmSctUdpBcDestPortCfgSet: %d, %d, %d", dev, iTemp, udpPortNum);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmSctUdpBcDestPortCfgSet(dev, numEntries, udpPortNum, cpuCode, cmd);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmSctUdpBcDestPortCfgSet: %d, %d, %d", dev, iTemp, udpPortNum);

        /* 1.4. Read all entries in UDP Broadcast Destination Port configuration table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmSctUdpBcDestPortCfgGet(dev, iTemp, &udpPortNumGet, &cpuCodeGet, &cmdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctUdpBcDestPortCfgGet: %d, %d", dev, iTemp);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(udpPortNum, udpPortNumGet,
                           "get another udpPortNum than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                           "get another cpuCode than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cmd, cmdGet,
                           "get another cmd than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmSctUdpBcDestPortCfgGet(dev, numEntries, &udpPortNumGet, &cpuCodeGet, &cmdGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctUdpBcDestPortCfgGet: %d, %d", dev, iTemp);

        /* 1.6. Delete all entries in UDP Broadcast Destination Port configuration table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmSctUdpBcDestPortCfgInvalidate(dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctUdpBcDestPortCfgInvalidate: %d, %d", dev, iTemp);
        }

        /* 1.7. Try to delete entry with index out of range. */
        st = cpssExMxPmSctUdpBcDestPortCfgInvalidate(dev, numEntries);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctUdpBcDestPortCfgInvalidate: %d, %d", dev, iTemp);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill TCP/UPD Destination Port Range CPU Code tAble.
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctFillAppSpecificCpuCodeTcpUdpDestPortRangeTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in TCP/UPD Destination Port Range CPU Code table.
         Call cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet with rangeIndex [0..numEntries - 1],
                           minDstPort [0],
                           maxDstPort [1],
                           packetType [CPSS_NET_TCP_UDP_PACKET_UC_E],
                           protocol [CPSS_NET_PROT_UDP_E]
                           and cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E].
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet with rangeIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in TCP/UPD Destination Port Range CPU Code table and compare with original.
         Call cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet with same rangeIndex,
                                                                                 non-NULL validPtr,
                                                                                 non-NULL minDstPortPtr,
                                                                                 non-NULL maxDstPortPtr,
                                                                                 non-NULL packetTypePtr,
                                                                                 non-NULL protocolPtr and cpuCodePtr.
    Expected: GT_OK valid == GT_TRUE and the same minDstPort, maxDstPort, packetType, protocol, cpuCode.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet with rangeIndex [numEntries].
    Expected: GT_OK valid == GT_FALSE.
    1.6. Delete all entries in TCP/UPD Destination Port Range CPU Code table.
         Call cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate with same rangeIndex to invalidate changes.
    Expected: GT_OK.
    1.7. Try to delete entry with index out of range.
         Call cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate with rangeIndex [numEntries].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    GT_U32                           minDstPort    = 0;
    GT_U32                           maxDstPort    = 0;
    CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetType    = CPSS_NET_TCP_UDP_PACKET_UC_E;
    CPSS_NET_PROT_ENT                protocol      = CPSS_NET_PROT_UDP_E;
    CPSS_NET_RX_CPU_CODE_ENT         cpuCode       = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
    GT_U32                           minDstPortGet = 0;
    GT_U32                           maxDstPortGet = 0;
    CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetTypeGet = CPSS_NET_TCP_UDP_PACKET_UC_E;
    CPSS_NET_PROT_ENT                protocolGet   = CPSS_NET_PROT_UDP_E;
    CPSS_NET_RX_CPU_CODE_ENT         cpuCodeGet    = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
    GT_BOOL                          isValid       = GT_FALSE;


    /* Fill the entry for TCP/UPD Destination Port Range CPU Code table */
    maxDstPort = 0;
    maxDstPort = 1;
    packetType = CPSS_NET_TCP_UDP_PACKET_UC_E;
    protocol   = CPSS_NET_PROT_UDP_E;
    cpuCode    = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 16;

        /* 1.2. Fill all entries in TCP/UPD Destination Port Range CPU Code table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, iTemp, minDstPort, maxDstPort,
                                                                       packetType, protocol, cpuCode);
           UTF_VERIFY_EQUAL7_STRING_MAC(GT_OK, st, "cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet: %d, %d, %d, %d, %d, %d, %d",
                                         dev, iTemp, minDstPort, maxDstPort, packetType, protocol, cpuCode);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, numEntries, minDstPort, maxDstPort,
                                                                   packetType, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL7_STRING_MAC(GT_OK, st, "cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet: %d, %d, %d, %d, %d, %d, %d",
                                     dev, iTemp, minDstPort, maxDstPort, packetType, protocol, cpuCode);


        /* 1.4. Read all entries in TCP/UPD Destination Port Range CPU Code table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, iTemp, &isValid, &maxDstPortGet, &maxDstPortGet,
                                                                       &packetTypeGet, &protocolGet, &cpuCodeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet: %d, %d", dev, iTemp);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValid,
                       "get invalid entry: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(minDstPort, minDstPortGet,
                           "get another minDstPort than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(maxDstPort, maxDstPortGet,
                           "get another maxDstPort than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(packetType, packetTypeGet,
                           "get another packetType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(protocol, protocolGet,
                           "get another protocol than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                           "get another cpuCode than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, numEntries, &isValid, &maxDstPortGet, &maxDstPortGet,
                                                                   &packetTypeGet, &protocolGet, &cpuCodeGet);
        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, isValid, "get invalid entry: %d", dev);
        }

        /* 1.6. Delete all entries in TCP/UPD Destination Port Range CPU Code table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate: %d, %d", dev, iTemp);
        }

        /* 1.7. Try to delete entry with index out of range. */
        st = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, numEntries);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate: %d, %d", dev, iTemp);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill CPU codes for specific ICMP type tAble.
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctFillAppSpecificCpuCodeIcmpTypeTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size. numEntries [12].
    1.2. Fill all entries in CPU codes for specific ICMP type table.
            Call cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet with entryIndex [0 ... numEntries-1],
                           icmpType [0],
                           packetType [CPSS_EXMXPM_SCT_ICMP_PACKET_UC_E],
                           protocolStack [CPSS_IP_PROTOCOL_IPV4_E],
                           cpuCode [CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E].
            Expected: GT_OK.
    1.3. Try to write entry with index out of range.
            Call cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet with entryIndex [numEntries] and other params from 1.2.
            Expected: NOT GT_OK.
    1.4. Read all entries in CPU codes for specific ICMP type table and compare with original.
            Call cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet with the same entryIndex,
                                                                      non-NULL validPtr,
                                                                      non-NULL icmpTypePtr,
                                                                      non-NULL packetTypePtr,
                                                                      non-NULL protocolStackPtr
                                                                      and non-NULL cpuCodePtr.
            Expected:  GT_OK, validPtr = GT_TRUE and the same icmpType, packetType, protocolStack and cpuCode.
    1.5. Try to read entry with index out of range.
            Call cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet with entryIndex [numEntries].
            Expected:  GT_OK, validPtr = GT_FALSE.
    1.6. Delete all entries in CPU codes for specific ICMP type table.
            Call cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate with same entryIndex to invalidate changes.
            Expected: GT_OK.
    1.7. Try to delete entry with index out of range.
            Call cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate with entryIndex [numEntries].
            Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U32      numEntries  = 0;
    GT_U32      iTemp       = 0;

    GT_U32                                  icmpType      = 0;
    CPSS_EXMXPM_SCT_ICMP_PACKET_TYPE_ENT    packetType    = CPSS_EXMXPM_SCT_ICMP_PACKET_UC_E;
    CPSS_IP_PROTOCOL_STACK_ENT              protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_NET_RX_CPU_CODE_ENT                cpuCode       = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;
    GT_U32                                  icmpTypeGet   = 0;
    CPSS_EXMXPM_SCT_ICMP_PACKET_TYPE_ENT    packetTypeGet = CPSS_EXMXPM_SCT_ICMP_PACKET_UC_E;
    CPSS_IP_PROTOCOL_STACK_ENT              protStackGet  = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_NET_RX_CPU_CODE_ENT                cpuCodeGet    = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;
    GT_BOOL                                 isValid       = GT_FALSE;

    /* Fill the entry for CPU codes for specific ICMP type table */
    icmpType      = 0;
    packetType    = CPSS_NET_TCP_UDP_PACKET_UC_E;
    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    cpuCode       = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 12;

        /* 1.2. Fill all entries in CPU codes for specific ICMP type table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet(dev, iTemp, icmpType, packetType,
                                                            protocolStack, cpuCode);
            UTF_VERIFY_EQUAL6_STRING_MAC(GT_OK, st, "cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet: %d, %d, %d, %d, %d, %d",
                                         dev, iTemp, icmpType, packetType, protocolStack, cpuCode);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet(dev, numEntries, icmpType, packetType,
                                                        protocolStack, cpuCode);
        UTF_VERIFY_NOT_EQUAL6_STRING_MAC(GT_OK, st, "cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet: %d, %d, %d, %d, %d, %d",
                                     dev, iTemp, icmpType, packetType, protocolStack, cpuCode);

        /* 1.4. Read all entries in CPU codes for specific ICMP type table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, iTemp, &isValid, &icmpTypeGet,
                                                            &packetTypeGet, &protStackGet, &cpuCodeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet: %d, %d", dev, iTemp);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValid,
                       "get invalid entry: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(icmpType, icmpTypeGet,
                           "get another icmpType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(packetType, packetTypeGet,
                           "get another packetType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(protocolStack, protStackGet,
                           "get another protocolStack than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet,
                           "get another cpuCode than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(dev, numEntries, &isValid, &icmpTypeGet,
                                                        &packetTypeGet, &protStackGet, &cpuCodeGet);
        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, isValid,"get invalid entry: %d", dev);
        }

        /* 1.5. Delete all entries in CPU codes for specific ICMP type table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate(dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate: %d, %d", dev, iTemp);
        }

        /* 1.7. Try to delete entry with index out of range. */
        st = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate(dev, numEntries);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate: %d, %d", dev, iTemp);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill IP Protocol CPU Code tAble.
*/
UTF_TEST_CASE_MAC(cpssExMxPmSctFillAppSpecificCpuCodeIpProtocolTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in IP Protocol CPU Code table.
         Call cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet with index [0 .. numEntries-1],
                                                               protocol [0]
                                                               and cpuCode [CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E].
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet with index [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in IP Protocol CPU Code table and compare with original.
         Call cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet with the same index,
                                                                        non-NULL validPtr,
                                                                        non-NULL protocolPtr
                                                                        and non-NULL cpuCodePtr.
    Expected: GT_OK valid = GT_TRUE and the same cpuCode.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet with index [numEntries].
    Expected: GT_OK valid = GT_FALSE.
    1.6. Delete all entries in IP Protocol CPU Code table.
         Call cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate with same entryIndex to invalidate changes.
    Expected: GT_OK.
    1.7. Try to delete entry with index out of range.
         Call cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate with index [numEntries].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    GT_U32                      protocol   = 0;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode    = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
    GT_U32                      protGet    = 0;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCodeGet = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
    GT_BOOL                     isValid    = GT_FALSE;


    /* Fill the entry for IP Protocol CPU Code table */
    protocol = 0;
    cpuCode  = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 8;

        /* 1.2. Fill all entries in IP Protocol CPU Code table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet(dev, iTemp, protocol, cpuCode);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet: %d, %d, %d", dev, iTemp, protocol);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet(dev, numEntries, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet: %d, %d, %d", dev, iTemp, protocol);

        /* 1.4. Read all entries in IP Protocol CPU Code table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(dev, iTemp, &isValid, &protGet, &cpuCodeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet: %d, %d", dev, iTemp);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValid, "get invalid entry: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(protocol, protGet, "get another protocol than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet, "get another cpuCode than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(dev, numEntries, &isValid, &protGet, &cpuCodeGet);
        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, isValid, "get invalid entry: %d", dev);
        }

        /* 1.6. Delete all entries in IP Protocol CPU Code table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate(dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate: %d, %d", dev, iTemp);
        }

        /* 1.7. Try to delete entry with index out of range. */
        st = cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate(dev, numEntries);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate: %d, %d", dev, iTemp);
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmSct suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmSct)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctPortIeeeReservedMcTrapEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctPortIeeeReservedMcTrapEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctPortIeeeReservedMcProfileSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctPortIeeeReservedMcProfileGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIeeeReservedMcProtocolCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIeeeReservedMcProtocolCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCiscoL2McCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCiscoL2McCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIgmpSnoopModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIgmpSnoopModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIcmpV6MsgTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIcmpV6MsgTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctUdpBcDestPortCfgSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctUdpBcDestPortCfgGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctUdpBcDestPortCfgInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctArpBcastCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctArpBcastCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctArpToMeCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctArpToMeCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIpV6SolicitedNodeMcastPrefixSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIpV6SolicitedCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIpV6SolicitedCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeTableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeRateLimiterTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeRateLimiterTableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeRateLimiterPacketCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeRateLimiterDropCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeStatisticalRateLimitsTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctPortDuplicateToCpuEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctPortDuplicateToCpuEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctDuplicateEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctDuplicateEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeTrgDevPortTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeTrgDevPortTableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIeeeReservedMcTrappedLearnEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet)
    /* Tests for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctFillUdpBcDestPortCfgTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctFillAppSpecificCpuCodeTcpUdpDestPortRangeTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctFillAppSpecificCpuCodeIcmpTypeTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmSctFillAppSpecificCpuCodeIpProtocolTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmSct)


/*******************************************************************************
* prvUtfCoreClockGet
*
* DESCRIPTION:
*       This routine returns core clock per device.
*
* INPUTS:
*       dev - device id
*
* OUTPUTS:
*       coreClockPtr - (pointer to) core clock
*
*       GT_OK        - Get core clock was OK
*       GT_BAD_PARAM - Invalid device id
*       GT_BAD_PTR   - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfCoreClockGet
(
    IN  GT_U8       dev,
    OUT GT_U32      *coreClockPtr
)
{
    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(coreClockPtr);

    /* check if dev active and from ExMxPm family */
    PRV_CPSS_EXMXPM_DEV_CHECK_MAC(dev);

    *coreClockPtr = PRV_CPSS_PP_MAC(dev)->coreClock;

    return(GT_OK);
}

