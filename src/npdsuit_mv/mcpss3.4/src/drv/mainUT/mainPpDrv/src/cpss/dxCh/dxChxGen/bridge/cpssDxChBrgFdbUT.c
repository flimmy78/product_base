/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgFdbUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChBrgFdb, that provides
*       FDB tables facility CPSS DxCh implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
/* get the device info and common info */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgFdbHash.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdbHash.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

#ifdef ASIC_SIMULATION
#include <gtOs/gtOsTimer.h>
#endif

/* Defines */
/* #define UT_FDB_DEBUG */

#ifdef UT_FDB_DEBUG
#define UT_FDB_DUMP(_x) cpssOsPrintf _x
#else
#define UT_FDB_DUMP(_x)
#endif

/* Default valid value for port id */
#define BRG_FDB_VALID_PHY_PORT_CNS  0

/* Maximum value for trunk id. The max value from all searched sources where taken   */
#define MAX_TRUNK_ID_CNS 128

/* Internal functions forward declaration */
static GT_STATUS prvUtfCoreClockGet(IN GT_U8 dev, OUT GT_U32* coreClockPtr);

static GT_STATUS prvUtfMacTableSize(IN GT_U8 dev, OUT GT_U32* tableSizePtr);

static void prvUtBuildMacAddrKeyFromBitArray(IN GT_U32 bitArray[3],
                                             INOUT CPSS_MAC_ENTRY_EXT_KEY_STC *macKeyPtr);

static GT_STATUS prvUtTestFdbHash(IN GT_U8 dev,
                                  IN CPSS_MAC_ENTRY_EXT_KEY_STC *macKeyPtr);


static GT_STATUS prvUtTestFdbHashCases(IN GT_U8 dev,
                                       IN CPSS_MAC_ENTRY_EXT_KEY_STC *macKeyPtr);

static GT_STATUS prvUtCheckMacEntry(IN GT_BOOL valid, IN GT_BOOL skip,
                                    IN CPSS_MAC_ENTRY_EXT_STC *entryPtr,
                                    IN CPSS_MAC_ENTRY_EXT_KEY_STC *macKeyPtr);

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbPortLearnStatusSet
(
    IN GT_U8                    devNum,
    IN GT_U8                    port,
    IN GT_BOOL                  status,
    IN CPSS_PORT_LOCK_CMD_ENT   cmd
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbPortLearnStatusSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with status [GT_FALSE, GT_TRUE]
                     and cmd [CPSS_LOCK_FRWRD_E /
                              CPSS_LOCK_DROP_E /
                              CPSS_LOCK_TRAP_E /
                              CPSS_LOCK_MIRROR_E].
    Expected: GT_OK.
    1.1.2. Call cpssDxChBrgFdbPortLearnStatusGet
    Expected: GT_OK and the same statusPtr and cmdPtr.
    1.1.3. Call with status [GT_FALSE / GT_TRUE] and out of range cmd.
    Expected: GT_BAD_PARAM.
*/

    GT_STATUS              st       = GT_OK;

    GT_U8                  dev;
    GT_U8                  port     = BRG_FDB_VALID_PHY_PORT_CNS;
    GT_BOOL                state    = GT_FALSE;
    CPSS_PORT_LOCK_CMD_ENT cmd      = CPSS_LOCK_FRWRD_E;
    GT_BOOL                stateGet = GT_FALSE;
    CPSS_PORT_LOCK_CMD_ENT cmdGet   = CPSS_LOCK_FRWRD_E;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with status [GT_FALSE, GT_TRUE]
                         and cmd [CPSS_LOCK_FRWRD_E /
                                  CPSS_LOCK_DROP_E /
                                  CPSS_LOCK_TRAP_E /
                                  CPSS_LOCK_MIRROR_E].
                Expected: GT_OK.
            */

            /*
                1.1.2. Call cpssDxChBrgFdbPortLearnStatusGet
                Expected: GT_OK and the same statusPtr and cmdPtr.
            */

            /* Call with status [GT_FALSE] and cmd [CPSS_LOCK_FRWRD_E] */
            state = GT_FALSE;
            cmd   = CPSS_LOCK_FRWRD_E;

            st = cpssDxChBrgFdbPortLearnStatusSet(dev, port, state, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, state, cmd);

            st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, &stateGet, &cmdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbPortLearnStatusGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another status than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d, %d", dev, port);

            /* Call with status [GT_FALSE] and cmd [CPSS_LOCK_DROP_E] */
            cmd = CPSS_LOCK_DROP_E;

            st = cpssDxChBrgFdbPortLearnStatusSet(dev, port, state, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, state, cmd);

            st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, &stateGet, &cmdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbPortLearnStatusGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another status than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d, %d", dev, port);

            /* Call with status [GT_FALSE] and cmd [CPSS_LOCK_TRAP_E] */
            cmd = CPSS_LOCK_TRAP_E;

            st = cpssDxChBrgFdbPortLearnStatusSet(dev, port, state, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, state, cmd);

            st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, &stateGet, &cmdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbPortLearnStatusGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another status than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d, %d", dev, port);

            /* Call with status [GT_FALSE] and cmd [CPSS_LOCK_MIRROR_E] */
            cmd = CPSS_LOCK_MIRROR_E;

            st = cpssDxChBrgFdbPortLearnStatusSet(dev, port, state, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, state, cmd);

            st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, &stateGet, &cmdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbPortLearnStatusGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another status than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(cmd, cmdGet,
                       "get another cmd than was set: %d, %d", dev, port);

            /* Call with status [GT_TRUE] and cmd [CPSS_LOCK_FRWRD_E] */
            state = GT_TRUE;
            cmd   = CPSS_LOCK_FRWRD_E;

            st = cpssDxChBrgFdbPortLearnStatusSet(dev, port, state, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, state, cmd);

            st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, &stateGet, &cmdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbPortLearnStatusGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another status than was set: %d, %d", dev, port);

            /* Call with status [GT_TRUE] and cmd [CPSS_LOCK_DROP_E] */
            cmd = CPSS_LOCK_DROP_E;

            st = cpssDxChBrgFdbPortLearnStatusSet(dev, port, state, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, state, cmd);

            st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, &stateGet, &cmdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbPortLearnStatusGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another status than was set: %d, %d", dev, port);

            /* Call with status [GT_TRUE] and cmd [CPSS_LOCK_TRAP_E] */
            cmd = CPSS_LOCK_TRAP_E;

            st = cpssDxChBrgFdbPortLearnStatusSet(dev, port, state, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, state, cmd);

            st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, &stateGet, &cmdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbPortLearnStatusGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another status than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with status [GT_FALSE / GT_TRUE] and wrong enum values cmd.
                Expected: GT_BAD_PARAM.
            */
            state = GT_FALSE;

            UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbPortLearnStatusSet
                                                        (dev, port, state, cmd),
                                                        cmd);
        }

        state = GT_TRUE;
        cmd   = CPSS_LOCK_FRWRD_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /*
            1.2. For all active devices go over all non available
            physical ports.
        */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChBrgFdbPortLearnStatusSet(dev, port, state, cmd);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgFdbPortLearnStatusSet(dev, port, state, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgFdbPortLearnStatusSet(dev, port, state, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    state = GT_TRUE;
    cmd   = CPSS_LOCK_FRWRD_E;

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = BRG_FDB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbPortLearnStatusSet(dev, port, state, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbPortLearnStatusSet(dev, port, state, cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbPortLearnStatusGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_BOOL *statusPtr,
    OUT CPSS_PORT_LOCK_CMD_ENT *cmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbPortLearnStatusGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with non-null statusPtr and non-null cmdPtr.
    Expected: GT_OK.
    1.1.2. Call with null statusPtr [NULL] and non-null cmdPtr.
    Expected: GT_BAD_PTR.
    1.1.3. Call with non-null statusPtr and null cmdPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS              st    = GT_OK;

    GT_U8                  dev;
    GT_U8                  port  = BRG_FDB_VALID_PHY_PORT_CNS;
    GT_BOOL                state = GT_FALSE;
    CPSS_PORT_LOCK_CMD_ENT cmd   = CPSS_LOCK_FRWRD_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-null statusPtr and non-null cmdPtr.
                Expected: GT_OK.
            */
            st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, &state, &cmd);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with null statusPtr [NULL] and non-null cmdPtr.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, NULL, &cmd);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);

            /*
                1.1.3. Call with non-null statusPtr and null cmdPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, &state, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, cmd = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /*
            1.2. For all active devices go over all non available
            physical ports.
        */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, &state, &cmd);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, &state, &cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, &state, &cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = BRG_FDB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, &state, &cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbPortLearnStatusGet(dev, port, &state, &cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbNaToCpuPerPortSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbNaToCpuPerPortSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChBrgFdbNaToCpuPerPortGet
    Expected: GT_OK and the same enablePtr.
*/

    GT_STATUS st       = GT_OK;

    GT_U8     dev;
    GT_U8     port     = BRG_FDB_VALID_PHY_PORT_CNS;
    GT_BOOL   state    = GT_FALSE;
    GT_BOOL   stateGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_FALSE and GT_TRUE].
                Expected: GT_OK.
            */

            /* enable = GT_FALSE */
            state = GT_FALSE;

            st = cpssDxChBrgFdbNaToCpuPerPortSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssDxChBrgFdbNaToCpuPerPortGet
                Expected: GT_OK and the same enablePtr.
            */
            st = cpssDxChBrgFdbNaToCpuPerPortGet(dev, port, &stateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbNaToCpuPerPortGet: %d, %d", dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* enablePtr = GT_TRUE*/
            state = GT_TRUE;

            st = cpssDxChBrgFdbNaToCpuPerPortSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssDxChBrgFdbNaToCpuPerPortGet
                Expected: GT_OK and the same enablePtr.
            */
            st = cpssDxChBrgFdbNaToCpuPerPortGet(dev, port, &stateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbNaToCpuPerPortGet: %d, %d", dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another enable than was set: %d, %d", dev, port);
        }
        state = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgFdbNaToCpuPerPortSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgFdbNaToCpuPerPortSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* enable = GT_FALSE */
        state = GT_FALSE;

        st = cpssDxChBrgFdbNaToCpuPerPortSet(dev, port, state);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

        /*
            1.4.1. Call cpssDxChBrgFdbNaToCpuPerPortGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbNaToCpuPerPortGet(dev, port, &stateGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgFdbNaToCpuPerPortGet: %d, %d", dev, port);

        UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                   "get another enable than was set: %d, %d", dev, port);

        /* enablePtr = GT_TRUE*/
        state = GT_TRUE;

        st = cpssDxChBrgFdbNaToCpuPerPortSet(dev, port, state);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

        /*
            1.4.2. Call cpssDxChBrgFdbNaToCpuPerPortGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbNaToCpuPerPortGet(dev, port, &stateGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgFdbNaToCpuPerPortGet: %d, %d", dev, port);

        UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                   "get another enable than was set: %d, %d", dev, port);

    }

    state = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_FDB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbNaToCpuPerPortSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbNaToCpuPerPortSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbNaToCpuPerPortGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbNaToCpuPerPortGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS  st    = GT_OK;

    GT_U8      dev;
    GT_U8      port  = BRG_FDB_VALID_PHY_PORT_CNS;
    GT_BOOL    state = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-null enablePtr.
                Expected: GT_OK.
            */
            st = cpssDxChBrgFdbNaToCpuPerPortGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with null enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChBrgFdbNaToCpuPerPortGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /*
            1.2. For all active devices go over all non available
            physical ports.
        */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChBrgFdbNaToCpuPerPortGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgFdbNaToCpuPerPortGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgFdbNaToCpuPerPortGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = BRG_FDB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbNaToCpuPerPortGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbNaToCpuPerPortGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbNaStormPreventSet
(
    IN GT_U8                       devNum,
    IN GT_U8                       port,
    IN GT_BOOL                     enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbNaStormPreventSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChBrgFdbNaStormPreventGet
    Expected: GT_OK and the same enablePtr.
*/

    GT_STATUS st       = GT_OK;

    GT_U8     dev;
    GT_U8     port     = BRG_FDB_VALID_PHY_PORT_CNS;
    GT_BOOL   state    = GT_FALSE;
    GT_BOOL   stateGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_FALSE and GT_TRUE].
                Expected: GT_OK.
            */

            /* enable = GT_FALSE */
            state = GT_FALSE;

            st = cpssDxChBrgFdbNaStormPreventSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssDxChBrgFdbNaStormPreventGet
                Expected: GT_OK and the same enablePtr.
            */
            st = cpssDxChBrgFdbNaStormPreventGet(dev, port, &stateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbNaStormPreventGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* enablePtr = GT_TRUE*/
            state = GT_TRUE;

            st = cpssDxChBrgFdbNaStormPreventSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            /*
                1.1.2. Call cpssDxChBrgFdbNaStormPreventGet
                Expected: GT_OK and the same enablePtr.
            */
            st = cpssDxChBrgFdbNaStormPreventGet(dev, port, &stateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbNaStormPreventGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        state = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgFdbNaStormPreventSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgFdbNaStormPreventSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgFdbNaStormPreventSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    state = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_FDB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbNaStormPreventSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbNaStormPreventSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbNaStormPreventGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbNaStormPreventGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS  st    = GT_OK;

    GT_U8      dev;
    GT_U8      port  = BRG_FDB_VALID_PHY_PORT_CNS;
    GT_BOOL    state = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-null enablePtr.
                Expected: GT_OK.
            */
            st = cpssDxChBrgFdbNaStormPreventGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with null enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChBrgFdbNaStormPreventGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /*
            1.2. For all active devices go over all non available
            physical ports.
        */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssDxChBrgFdbNaStormPreventGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgFdbNaStormPreventGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgFdbNaStormPreventGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = BRG_FDB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbNaStormPreventGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbNaStormPreventGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbDeviceTableSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  devTableBmp
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbDeviceTableSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with devTableBmp [1].
    Expected: GT_OK.
    1.2. Call with devTableBmp [0xFFFFFFFE] (no constraints in function's contract).
    Expected: GT_OK.
    1.3. Call cpssDxChBrgFdbDeviceTableGet
    Expected: GT_OK and the same devTableBmpPtr.
*/
    GT_STATUS  st        = GT_OK;

    GT_U8      dev;
    GT_U32     devTbl    = 0;
    GT_U32     devTblGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with devTableBmp [1].
            Expected: GT_OK.
        */
        devTbl = 1;

        st = cpssDxChBrgFdbDeviceTableSet(dev, devTbl);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, devTbl);

        /*
            1.3. Call cpssDxChBrgFdbDeviceTableGet
            Expected: GT_OK and the same devTableBmpPtr.
        */
        st = cpssDxChBrgFdbDeviceTableGet(dev, &devTblGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbDeviceTableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(devTbl, devTblGet,
                       "get another devTableBmp than was set: %d", dev);

        /*
            1.2. Call with devTableBmp [0xFFFFFFFE] (no constraints in function's contract).
            Expected: GT_OK.
        */
        devTbl = 0xFFFFFFFE;

        st = cpssDxChBrgFdbDeviceTableSet(dev, devTbl);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, devTbl);

        /*
            1.3. Call cpssDxChBrgFdbDeviceTableGet
            Expected: GT_OK and the same devTableBmpPtr.
        */
        st = cpssDxChBrgFdbDeviceTableGet(dev, &devTblGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbDeviceTableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(devTbl, devTblGet,
                       "get another devTableBmp than was set: %d", dev);
    }

    devTbl = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbDeviceTableSet(dev, devTbl);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbDeviceTableSet(dev, devTbl);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbDeviceTableGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *devTableBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbDeviceTableGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null devTableBmpPtr.
    Expected: GT_OK.
    1.2. Call with null devTableBmpPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS    st     = GT_OK;

    GT_U8        dev;
    GT_U32       devTbl = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null devTableBmpPtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbDeviceTableGet(dev, &devTbl);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null devTableBmpPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbDeviceTableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbDeviceTableGet(dev, &devTbl);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbDeviceTableGet(dev, &devTbl);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/* function will be commonly used from other FDB UT tests*/
void utfCpssDxChBrgFdbFromCpuAuMsgStatusGet
(
    IN GT_U8    devNum,
    IN GT_BOOL  mustSucceed
)
{
    GT_STATUS   st;
    GT_BOOL completed;
    GT_BOOL succeeded;
    GT_PORT_GROUPS_BMP completedBmp;
    GT_PORT_GROUPS_BMP succeededBmp;
    GT_PORT_GROUPS_BMP completedBmpSummary;
    GT_PORT_GROUPS_BMP succeededBmpSummary;

    if(usePortGroupsBmp == GT_FALSE)
    {
        completed = GT_FALSE;
        succeeded = GT_FALSE;

        do{
            #ifdef ASIC_SIMULATION
            osTimerWkAfter(1);
            #endif
            st = cpssDxChBrgFdbFromCpuAuMsgStatusGet(devNum, &completed, &succeeded);
            if(st != GT_OK)
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, devNum);
                return;
            }
        } while (completed == GT_FALSE);

        if(succeeded == GT_FALSE && mustSucceed == GT_TRUE)
        {
            /* operation finished but not succeeded */
            st = GT_FAIL;
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, devNum);
        }
    }
    else
    {
        completedBmpSummary = 0;
        succeededBmpSummary = 0;
        do{
            completedBmp = 0;
            succeededBmp = 0;
            #ifdef ASIC_SIMULATION
            osTimerWkAfter(1);
            #endif
            st = cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(devNum, currPortGroupsBmp, &completedBmp, &succeededBmp);
            if(st != GT_OK)
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, devNum);
                return;
            }

            completedBmpSummary |= completedBmp;
            succeededBmpSummary |= succeededBmp;
        }while (
            (completedBmpSummary & PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp) !=
            (currPortGroupsBmp & PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp));

        if(completedBmpSummary != succeededBmpSummary  && mustSucceed == GT_TRUE)
        {
            /* operation finished but not succeeded on all port groups */
            st = GT_FAIL;
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, devNum);
        }
    }
}

/* function will be commonly used from other FDB UT tests*/
GT_STATUS utfCpssDxChBrgFdbMacEntrySet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    if(usePortGroupsBmp == GT_FALSE)
    {
        return cpssDxChBrgFdbMacEntrySet(devNum,macEntryPtr);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupMacEntrySet(devNum,currPortGroupsBmp,macEntryPtr);
    }
}

/* function will be commonly used from other FDB UT tests*/
GT_STATUS utfCpssDxChBrgFdbMacEntryInvalidate
(
    IN GT_U8         devNum,
    IN GT_U32        index
)
{
    if(usePortGroupsBmp == GT_FALSE)
    {
        return cpssDxChBrgFdbMacEntryInvalidate(devNum,index);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupMacEntryInvalidate(devNum,currPortGroupsBmp,index);
    }
}

/* function will be commonly used from other FDB UT tests*/
GT_STATUS utfCpssDxChBrgFdbMacEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr,
    OUT GT_BOOL                 *agedPtr,
    OUT GT_U8                   *associatedDevNumPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  *entryPtr
)
{
    if(usePortGroupsBmp == GT_FALSE)
    {
        return cpssDxChBrgFdbMacEntryRead(devNum,index,validPtr,skipPtr,agedPtr,associatedDevNumPtr,entryPtr);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupMacEntryRead(devNum,currPortGroupsBmp,index,validPtr,skipPtr,agedPtr,associatedDevNumPtr,entryPtr);
    }
}

/* function will be commonly used from other FDB UT tests*/
GT_STATUS utfCpssDxChBrgFdbQaSend
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    if(usePortGroupsBmp == GT_FALSE)
    {
        return cpssDxChBrgFdbQaSend(devNum,macEntryKeyPtr);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupQaSend(devNum,currPortGroupsBmp,macEntryKeyPtr);
    }
}

/* function will be commonly used from other FDB UT tests*/
GT_STATUS utfCpssDxChBrgFdbMacEntryDelete
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    if(usePortGroupsBmp == GT_FALSE)
    {
        return cpssDxChBrgFdbMacEntryDelete(devNum,macEntryKeyPtr);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupMacEntryDelete(devNum,currPortGroupsBmp,macEntryKeyPtr);
    }
}

/* function will be commonly used from other FDB UT tests*/
GT_STATUS utfCpssDxChBrgFdbMacEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U32                       index,
    IN GT_BOOL                      skip,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    if(usePortGroupsBmp == GT_FALSE)
    {
        return cpssDxChBrgFdbMacEntryWrite(devNum,index,skip,macEntryPtr);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupMacEntryWrite(devNum,currPortGroupsBmp,index,skip,macEntryPtr);
    }
}

/* function will be commonly used from other FDB UT tests*/
GT_STATUS utfCpssDxChBrgFdbMacEntryStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
)
{
    if(usePortGroupsBmp == GT_FALSE)
    {
        return cpssDxChBrgFdbMacEntryStatusGet(devNum,index,validPtr,skipPtr);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupMacEntryStatusGet(devNum,currPortGroupsBmp,index,validPtr,skipPtr);
    }
}

/* function will be commonly used from other FDB UT tests*/
GT_STATUS utfCpssDxChBrgFdbMacEntryAgeBitSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_BOOL      age
)
{
    if(usePortGroupsBmp == GT_FALSE)
    {
        return cpssDxChBrgFdbMacEntryAgeBitSet(devNum,index,age);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupMacEntryAgeBitSet(devNum,currPortGroupsBmp,index,age);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbMacEntrySet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbMacEntrySet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with valid macEntryPtr {key {CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E,
                                                    macVlan->vlanId[100],
                                                    macVlan->macAddr[00:1A:FF:FF:FF:FF]},
                                               dstInterface {type[CPSS_INTERFACE_TRUNK_E],
                                                             devPort {devNum [devNum],
                                                                      portNum [0]},
                                                             trunkId [10],
                                                             vidx [10],
                                                             vlanId [100]},
                                               isStatic [GT_FALSE],
                                               daCommand [CPSS_MAC_TABLE_FRWRD_E],
                                               saCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E],
                                               daRoute [GT_FALSE],
                                               mirrorToRxAnalyzerPortEn [GT_FALSE],
                                               sourceID [0],
                                               userDefined [0],
                                               daQosIndex [0],
                                               saQosIndex [0],
                                               daSecurityLevel [0],
                                               saSecurityLevel [0],
                                               appSpecificCpuCode [GT_FALSE]}.
    Expected: GT_OK.
    1.2. Call function with valid macEntryPtr {key {CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E,
                                                    ipMcast->vlanId[100],
                                                    ipMcast->sip={123.12.12.3},
                                                    ipMcast->dip = {123.34.67.8}},
                                                dstInterface {type[CPSS_INTERFACE_VID_E],
                                                              devPort {devNum [devNum],
                                                                       portNum [0]},
                                                              trunkId [0],
                                                              vidx [10],
                                                              vlanId [100]},
                                                 isStatic [GT_FALSE],
                                                 daCommand [CPSS_MAC_TABLE_INTERV_E],
                                                 saCommand [CPSS_MAC_TABLE_CNTL_E],
                                                 daRoute [GT_FALSE],
                                                 mirrorToRxAnalyzerPortEn [GT_FALSE],
                                                 sourceID [0xFFFF0000],
                                                 userDefined [0],
                                                 daQosIndex [30],
                                                 saQosIndex [101],
                                                 daSecurityLevel [10],
                                                 saSecurityLevel [40],
                                                 appSpecificCpuCode [GT_TRUE]}
    Expected: GT_OK.
    1.3. Call with macEntryPtr->dstInterface->type[CPSS_INTERFACE_PORT_E]
                   and other - the same as in 1.2.
    Expected: NON GT_OK (IP MC support only VIDX and VID).
    1.4. Call function with valid macEntryPtr {key {CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E,
                                                    ipMcast->vlanId[100],
                                                    ipMcast->sip={123.12.12.3},
                                                    ipMcast->dip={123.34.67.8}},
                                               dstInterface {type[CPSS_INTERFACE_VIDX_E],
                                                             devPort {devNum [devNum],
                                                                      portNum [0]},
                                                             trunkId [0],
                                                             vidx [10],
                                                             vlanId [100]},
                                               isStatic [GT_FALSE],
                                               daCommand [CPSS_MAC_TABLE_SOFT_DROP_E],
                                               saCommand [CPSS_MAC_TABLE_DROP_E],
                                               daRoute [GT_TRUE],
                                               mirrorToRxAnalyzerPortEn [GT_TRUE],
                                               sourceID [0],
                                               userDefined [0],
                                               daQosIndex [0],
                                               saQosIndex [0],
                                               daSecurityLevel [0],
                                               saSecurityLevel [0],
                                               appSpecificCpuCode [GT_FALSE]}.
    Expected: GT_OK.
    1.5. Call with macEntryPtr->dstInterface->type [CPSS_INTERFACE_TRUNK_E]
                   and other - the same as in 1.4.
    Expected: NON GT_OK (IP MC support only VIDX and VID).
    1.6. Call function with out of range macEntryPtr->key->ipMcast->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                       key->entryType [CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E].
    Expected: GT_OUT_OF_RANGE.
    1.7. Call function with valid macEntryPtr {key { CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E,
                                               macVlan->vlanId[100],
                                               macVlan-> macAddr[00:1A:FF:FF:FF:FF]},
                                               dstInterface {type[CPSS_INTERFACE_PORT_E],
                                                             devPort {devNum [devNum],
                                                                      portNum [0]},
                                                             trunkId [0],
                                                             vidx [10],
                                                             vlanId [100]},
                                                isStatic [GT_FALSE],
                                                daCommand [CPSS_MAC_TABLE_FRWRD_E],
                                                saCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E],
                                                daRoute [GT_FALSE],
                                                mirrorToRxAnalyzerPortEn [GT_FALSE],
                                                sourceID [0],
                                                userDefined [0],
                                                daQosIndex [0],
                                                saQosIndex [0],
                                                daSecurityLevel [0],
                                                saSecurityLevel [0],
                                                appSpecificCpuCode [GT_FALSE]}.
    Expected: GT_OK.
    1.7. Call function with null macEntryPtr [NULL].
    Expected: GT_BAD_PTR.
    1.8. Call function with out of range key->entryType
                            and other parameters the same as in 1.7.
    Expected: GT_BAD_PARAM.
    1.9. Call function with out of range dstInterface->type
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.10. Call function with out of range macEntryPtr->key->macVlan->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                             key->entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E].
    Expected: GT_OUT_OF_RANGE.
    1.12. Call function with out of range dstInterface->devPort->portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                             dstInterface->type = CPSS_INTERFACE_PORT_E
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.13. Call function with out of range dstInterface->devPort->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS],
                             dstInterface->type = CPSS_INTERFACE_PORT_E
                             and other parameters the same as in 1.7.
    Expected: NON GT_OK.
    1.14. Call function with out of range dstInterface->trunkId [MAX_TRUNK_ID_CNS  = 128],
                             dstInterface->type = CPSS_INTERFACE_TRUNK_E
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.15. Call function with out of range dstInterface->vidx [maxMeGrInd(dev)],
                             dstInterface->type = CPSS_INTERFACE_VIDX_E
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.16. Call function with out of range dstInterface->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                             and dstInterface->type [CPSS_INTERFACE_VID_E].
    Expected: GT_OK.
    1.17. Call function with out of range macEntryPtr->daCommand
                             and other parameters the same as in 1.7.
    Expected: GT_BAD_PARAM.
    1.18. Call function with out of range macEntryPtr->saCommand,
                             key->entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E] (saCommand is not important for IPv4 and IPv6 MC).
                             and other parameters the same as in 1.7.
    Expected: GT_BAD_PARAM.
    1.19. Call with userDefined [16] and other params from 1.7.
    Expected: NOT GT_OK.
    1.20. Call with daQosIndex [8] and other params from 1.7.
    Expected: NOT GT_OK.
    1.21. Call with saQosIndex [8] and other params from 1.7.
    Expected: NOT GT_OK.
    1.22. Call with daSecurityLevel [8] and other params from 1.7.
    Expected: NOT GT_OK.
    1.23. Call with saSecurityLevel [8] and other params from 1.7.
    Expected: NOT GT_OK.
    1.24. Call with sourceID [32] and other params from 1.7.
    Expected: NOT GT_OK.
*/
    GT_STATUS               st          = GT_OK;

    GT_U8                   dev;
    CPSS_MAC_ENTRY_EXT_STC  macEntry;
    GT_U16                  maxMcGroups = 4096;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with valid macEntryPtr {key {CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E,
                                                    macVlan->vlanId[100],
                                                    macVlan->macAddr[00:1A:FF:FF:FF:FF]},
                                               dstInterface {type[CPSS_INTERFACE_TRUNK_E],
                                                             devPort {devNum [devNum],
                                                                      portNum [0]},
                                                             trunkId [10],
                                                             vidx [10],
                                                             vlanId [100]},
                                               isStatic [GT_FALSE],
                                               daCommand [CPSS_MAC_TABLE_FRWRD_E],
                                               saCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E],
                                               daRoute [GT_FALSE],
                                               mirrorToRxAnalyzerPortEn [GT_FALSE],
                                               sourceID [0],
                                               userDefined [0],
                                               daQosIndex [0],
                                               saQosIndex [0],
                                               daSecurityLevel [0],
                                               saSecurityLevel [0],
                                               appSpecificCpuCode [GT_FALSE]}.
            Expected: GT_OK.
        */
        macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        macEntry.key.key.macVlan.vlanId = 100;

        macEntry.key.key.macVlan.macAddr.arEther[0] = 0x0;
        macEntry.key.key.macVlan.macAddr.arEther[1] = 0x1A;
        macEntry.key.key.macVlan.macAddr.arEther[2] = 0xFF;
        macEntry.key.key.macVlan.macAddr.arEther[3] = 0xFF;
        macEntry.key.key.macVlan.macAddr.arEther[4] = 0xFF;
        macEntry.key.key.macVlan.macAddr.arEther[5] = 0xFF;

        macEntry.dstInterface.type            = CPSS_INTERFACE_TRUNK_E;
        macEntry.dstInterface.devPort.devNum  = dev;
        macEntry.dstInterface.devPort.portNum = 0;
        macEntry.dstInterface.trunkId         = 10;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
        macEntry.dstInterface.vidx            = 10;
        macEntry.dstInterface.vlanId          = 100;

        macEntry.isStatic                 = GT_FALSE;
        macEntry.daCommand                = CPSS_MAC_TABLE_FRWRD_E;
        macEntry.saCommand                = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
        macEntry.daRoute                  = GT_FALSE;
        macEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
        macEntry.sourceID                 = 0;
        macEntry.userDefined              = 0;
        macEntry.daQosIndex               = 0;
        macEntry.saQosIndex               = 0;
        macEntry.daSecurityLevel          = 0;
        macEntry.saSecurityLevel          = 0;
        macEntry.appSpecificCpuCode       = GT_FALSE;
        macEntry.age                      = GT_TRUE;
        macEntry.spUnknown                = GT_FALSE;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);


        /*
            1.2. Call function with valid macEntryPtr {key {CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E,
                                                    ipMcast->vlanId[100],
                                                    ipMcast->sip={123.12.12.3},
                                                    ipMcast->dip = {123.34.67.8}},
                                                dstInterface {type[CPSS_INTERFACE_VID_E],
                                                              devPort {devNum [devNum],
                                                                       portNum [0]},
                                                              trunkId [0],
                                                              vidx [10],
                                                              vlanId [100]},
                                                 isStatic [GT_FALSE],
                                                 daCommand [CPSS_MAC_TABLE_INTERV_E],
                                                 saCommand [CPSS_MAC_TABLE_CNTL_E],
                                                 daRoute [GT_FALSE],
                                                 mirrorToRxAnalyzerPortEn [GT_FALSE],
                                                 sourceID [0xFFFF0000],
                                                 userDefined [0],
                                                 daQosIndex [3],
                                                 saQosIndex [5],
                                                 daSecurityLevel [1],
                                                 saSecurityLevel [4],
                                                 appSpecificCpuCode [GT_TRUE]}
            Expected: GT_OK.
        */
        macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
        macEntry.key.key.ipMcast.vlanId = 100;

        macEntry.key.key.ipMcast.sip[0] = 123;
        macEntry.key.key.ipMcast.sip[1] = 12;
        macEntry.key.key.ipMcast.sip[2] = 12;
        macEntry.key.key.ipMcast.sip[3] = 3;
        macEntry.key.key.ipMcast.dip[0] = 123;
        macEntry.key.key.ipMcast.dip[1] = 34;
        macEntry.key.key.ipMcast.dip[2] = 67;
        macEntry.key.key.ipMcast.dip[3] = 8;

        macEntry.dstInterface.type            = CPSS_INTERFACE_VID_E;
        macEntry.dstInterface.devPort.devNum  = dev;
        macEntry.dstInterface.devPort.portNum = 0;
        macEntry.dstInterface.trunkId         = 10;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
        macEntry.dstInterface.vidx            = 10;
        macEntry.dstInterface.vlanId          = 100;

        macEntry.isStatic                 = GT_FALSE;
        macEntry.daCommand                = CPSS_MAC_TABLE_FRWRD_E;
        macEntry.saCommand                = CPSS_MAC_TABLE_CNTL_E;
        macEntry.daRoute                  = GT_FALSE;
        macEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
        macEntry.sourceID                 = 0xFFFF0000;
        macEntry.userDefined              = 0;
        macEntry.daQosIndex               = 3;
        macEntry.saQosIndex               = 5;
        macEntry.daSecurityLevel          = 1;
        macEntry.saSecurityLevel          = 4;
        macEntry.appSpecificCpuCode       = GT_TRUE;
        macEntry.age                      = GT_TRUE;
        macEntry.spUnknown                = GT_FALSE;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.3. Call with macEntryPtr->dstInterface->type[CPSS_INTERFACE_PORT_E]
                           and other - the same as in 1.2.
            Expected: NON GT_OK (IP MC support only VIDX and VID).
        */
        macEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);

        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, macEntryPtr->key->entryType = %d, macEntryPtr->dstInterface->type = %d",
                                        dev, macEntry.key.entryType, macEntry.dstInterface.type);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.4. Call function with valid macEntryPtr {key {CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E,
                                                    ipMcast->vlanId[100],
                                                    ipMcast->sip={123.12.12.3},
                                                    ipMcast->dip={123.34.67.8}},
                                               dstInterface {type[CPSS_INTERFACE_VIDX_E],
                                                             devPort {devNum [devNum],
                                                                      portNum [0]},
                                                             trunkId [0],
                                                             vidx [10],
                                                             vlanId [100]},
                                               isStatic [GT_FALSE],
                                               daCommand [CPSS_MAC_TABLE_SOFT_DROP_E],
                                               saCommand [CPSS_MAC_TABLE_DROP_E],
                                               daRoute [GT_TRUE],
                                               mirrorToRxAnalyzerPortEn [GT_TRUE],
                                               sourceID [0],
                                               userDefined [0],
                                               daQosIndex [0],
                                               saQosIndex [0],
                                               daSecurityLevel [0],
                                               saSecurityLevel [0],
                                               appSpecificCpuCode [GT_FALSE]}.
            Expected: GT_OK.
        */
        macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;
        macEntry.key.key.ipMcast.vlanId = 100;

        macEntry.key.key.ipMcast.sip[0] = 123;
        macEntry.key.key.ipMcast.sip[1] = 12;
        macEntry.key.key.ipMcast.sip[2] = 12;
        macEntry.key.key.ipMcast.sip[3] = 3;
        macEntry.key.key.ipMcast.dip[0] = 123;
        macEntry.key.key.ipMcast.dip[1] = 34;
        macEntry.key.key.ipMcast.dip[2] = 67;
        macEntry.key.key.ipMcast.dip[3] = 8;

        macEntry.dstInterface.type            = CPSS_INTERFACE_VID_E;
        macEntry.dstInterface.devPort.devNum  = dev;
        macEntry.dstInterface.devPort.portNum = 0;
        macEntry.dstInterface.trunkId         = 10;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
        macEntry.dstInterface.vidx            = 10;
        macEntry.dstInterface.vlanId          = 100;

        macEntry.isStatic                 = GT_FALSE;
        macEntry.daCommand                = CPSS_MAC_TABLE_SOFT_DROP_E;
        macEntry.saCommand                = CPSS_MAC_TABLE_DROP_E;
        macEntry.daRoute                  = GT_TRUE;
        macEntry.mirrorToRxAnalyzerPortEn = GT_TRUE;
        macEntry.sourceID                 = 0;
        macEntry.userDefined              = 0;
        macEntry.daQosIndex               = 0;
        macEntry.saQosIndex               = 0;
        macEntry.daSecurityLevel          = 0;
        macEntry.saSecurityLevel          = 0;
        macEntry.appSpecificCpuCode       = GT_FALSE;
        macEntry.age                      = GT_TRUE;
        macEntry.spUnknown                = GT_FALSE;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.5. Call with macEntryPtr->dstInterface->type [CPSS_INTERFACE_TRUNK_E]
                           and other - the same as in 1.4.
            Expected: NON GT_OK (IP MC support only VIDX and VID).
        */
        macEntry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, macEntryPtr->key->entryType = %d, macEntryPtr->dstInterface->type = %d",
                                        dev, macEntry.key.entryType, macEntry.dstInterface.type);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.6. Call function with out of range macEntryPtr->key->ipMcast->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                                    key->entryType [CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E].
            Expected: GT_OUT_OF_RANGE.
        */
        macEntry.key.key.ipMcast.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;
        macEntry.dstInterface.type      = CPSS_INTERFACE_VID_E;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, macEntryPtr->key->ipMcast->vlanId = %d",
                                     dev, macEntry.key.key.ipMcast.vlanId);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.7. Call function with null macEntryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = utfCpssDxChBrgFdbMacEntrySet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.8. Call function with wrong enum values key->entryType
                                    and other parameters the same as in 1.7.
            Expected: GT_BAD_PARAM.
        */
        macEntry.key.key.ipMcast.vlanId = 100;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbMacEntrySet
                                                        (dev, &macEntry),
                                                        macEntry.key.entryType);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.9. Call function with wrong enum values dstInterface->type
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        macEntry.key.entryType     = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbMacEntrySet
                                                        (dev, &macEntry),
                                                        macEntry.dstInterface.type);

        macEntry.dstInterface.type = CPSS_INTERFACE_VID_E;

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.10. Call function with out of range macEntryPtr->key->macVlan->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                                     key->entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E].
            Expected: GT_OUT_OF_RANGE.
        */
        macEntry.dstInterface.type      = CPSS_INTERFACE_VID_E;
        macEntry.key.key.macVlan.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st,
                                     "%d, macEntryPtr->key->key->macVlan->vlanId = %d",
                                     dev, macEntry.key.key.macVlan.vlanId);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.12. Call function with out of range
            dstInterface->devPort->portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                                     dstInterface->type = CPSS_INTERFACE_PORT_E
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        macEntry.key.key.macVlan.vlanId       = 100;
        macEntry.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        macEntry.dstInterface.type            = CPSS_INTERFACE_PORT_E;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                "%d, macEntryPtr->dstInterface.devPort.portNum = %d",
                                         dev, macEntry.dstInterface.devPort.portNum);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.13. Call function with out of range
            dstInterface->devPort->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS],
                                     dstInterface->type = CPSS_INTERFACE_PORT_E
                                     and other parameters the same as in 1.7.
            Expected: NON GT_OK.
        */
        macEntry.dstInterface.devPort.portNum = 0;
        macEntry.dstInterface.devPort.devNum  = PRV_CPSS_MAX_PP_DEVICES_CNS;
        macEntry.dstInterface.type            = CPSS_INTERFACE_PORT_E;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                "%d, macEntryPtr->dstInterface->devPort->devNum = %d",
                                     dev, macEntry.dstInterface.devPort.devNum);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.14. Call function with out of range
            dstInterface->trunkId [MAX_TRUNK_ID_CNS  = 128],
                                     dstInterface->type = CPSS_INTERFACE_TRUNK_E
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        macEntry.dstInterface.devPort.devNum = dev;
        macEntry.dstInterface.trunkId        = MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
        macEntry.dstInterface.type           = CPSS_INTERFACE_TRUNK_E;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->dstInterface->trunkId = %d",
                                     dev, macEntry.dstInterface.trunkId);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.15. Call function with out of range dstInterface->vidx [maxMeGrInd(dev)],
                                     dstInterface->type = CPSS_INTERFACE_VIDX_E
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        macEntry.dstInterface.trunkId = 10;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
        macEntry.dstInterface.vidx    = maxMcGroups;
        macEntry.dstInterface.type    = CPSS_INTERFACE_VIDX_E;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->dstInterface->vidx = %d",
                                         dev, macEntry.dstInterface.vidx);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.16. Call function with out of range
                    dstInterface->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                     and dstInterface->type [CPSS_INTERFACE_VID_E].
            Expected: GT_OK.
        */
        macEntry.dstInterface.vidx   = 10;
        macEntry.dstInterface.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        macEntry.dstInterface.type   = CPSS_INTERFACE_VID_E;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->dstInterface->vlanId = %d",
                                     dev, macEntry.dstInterface.vlanId);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.17. Call function with wrong enum values macEntryPtr->daCommand
                                     and other parameters the same as in 1.7.
            Expected: GT_BAD_PARAM.
        */
        macEntry.dstInterface.vlanId = 100;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbMacEntrySet
                                                        (dev, &macEntry),
                                                        macEntry.daCommand);

        macEntry.daCommand  = CPSS_MAC_TABLE_SOFT_DROP_E;

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.18. Call function with wrong enum values macEntryPtr->saCommand,
                                     key->entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E]
                                     (saCommand is not important for IPv4 and IPv6 MC).
                                     and other parameters the same as in 1.7.
            Expected: GT_BAD_PARAM.
        */
        macEntry.daCommand     = CPSS_MAC_TABLE_INTERV_E;
        macEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbMacEntrySet
                                                        (dev, &macEntry),
                                                        macEntry.saCommand);

        macEntry.saCommand                = CPSS_MAC_TABLE_DROP_E;

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.19. Call with userDefined [16] and other params from 1.7.
            Expected: NOT GT_OK.
        */
        macEntry.userDefined = 16;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->userDefined = %d",
                                         dev, macEntry.userDefined);

        macEntry.userDefined = 0;

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.20. Call with daQosIndex [8] and other params from 1.7.
            Expected: NOT GT_OK.
        */
        macEntry.daQosIndex = 8;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->daQosIndex = %d",
                                         dev, macEntry.daQosIndex);

        macEntry.daQosIndex = 0;

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.21. Call with saQosIndex [8] and other params from 1.7.
            Expected: NOT GT_OK.
        */
        macEntry.saQosIndex = 8;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->saQosIndex = %d",
                                         dev, macEntry.saQosIndex);

        macEntry.saQosIndex = 0;

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.22. Call with daSecurityLevel [8] and other params from 1.7.
            Expected: NOT GT_OK.
        */
        macEntry.daSecurityLevel = 8;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->daSecurityLevel = %d",
                                         dev, macEntry.daSecurityLevel);

        macEntry.daSecurityLevel = 0;

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.23. Call with saSecurityLevel [8] and other params from 1.7.
            Expected: NOT GT_OK.
        */
        macEntry.saSecurityLevel = 8;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->saSecurityLevel = %d",
                                         dev, macEntry.saSecurityLevel);

        macEntry.saSecurityLevel = 0;

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.24. Call with sourceID [32] and other params from 1.7.
            Expected: NOT GT_OK.
        */
        macEntry.sourceID = 32;

        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->sourceID = %d",
                                         dev, macEntry.sourceID);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

    }

    macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    macEntry.key.key.macVlan.vlanId = 100;

    macEntry.key.key.macVlan.macAddr.arEther[0] = 0x0;
    macEntry.key.key.macVlan.macAddr.arEther[1] = 0x1A;
    macEntry.key.key.macVlan.macAddr.arEther[2] = 0xFF;
    macEntry.key.key.macVlan.macAddr.arEther[3] = 0xFF;
    macEntry.key.key.macVlan.macAddr.arEther[4] = 0xFF;
    macEntry.key.key.macVlan.macAddr.arEther[5] = 0xFF;

    macEntry.dstInterface.type            = CPSS_INTERFACE_TRUNK_E;
    macEntry.dstInterface.devPort.devNum  = dev;
    macEntry.dstInterface.devPort.portNum = 0;
    macEntry.dstInterface.trunkId         = 10;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
    macEntry.dstInterface.vidx            = 10;
    macEntry.dstInterface.vlanId          = 100;

    macEntry.isStatic                 = GT_FALSE;
    macEntry.daCommand                = CPSS_MAC_TABLE_FRWRD_E;
    macEntry.saCommand                = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
    macEntry.daRoute                  = GT_FALSE;
    macEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
    macEntry.sourceID                 = 0;
    macEntry.userDefined              = 0;
    macEntry.daQosIndex               = 0;
    macEntry.saQosIndex               = 0;
    macEntry.daSecurityLevel          = 0;
    macEntry.saSecurityLevel          = 0;
    macEntry.appSpecificCpuCode       = GT_FALSE;
    macEntry.age                      = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbQaSend
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbQaSend)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with valid macEntryKeyPtr{entryType[CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                                 macVlan->vlanId[100],
                                                 macVlan->macAddr[00:1A:FF:FF:FF:FF]}
    Expected: GT_OK.
    1.2. Call function with valid macEntryKeyPtr{entryType[CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E],
                                                 ipMcast->vlanId[100],
                                                 ipMcast->sip[123.23.34.5],
                                                 ipMacst [123.45.67.8]}.
    Expected: GT_OK.
    1.3. Call function with null macEntryKeyPtr [NULL].
    Expected: GT_BAD_PTR.
    1.4. Call function with out of range macEntryKeyPtr >entryType
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call function with out of range macEntryKeyPtr->key->ipMcast->vlanId[PRV_CPSS_MAX_NUM_VLANS_CNS],
                            entryType [CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E].
    Expected: GT_OUT_OF_RANGE.
    1.6. Call function with out of range macEntryKeyPtr->key->macVlan->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                            entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E].
    Expected: GT_OUT_OF_RANGE.
*/
    GT_STATUS                  st = GT_OK;

    GT_U8                      dev;
    CPSS_MAC_ENTRY_EXT_STC     macEntry;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with valid macEntryKeyPtr{entryType[CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                                         macVlan->vlanId[100],
                                                         macVlan->macAddr[00:1A:FF:FF:FF:FF]}
            Expected: GT_OK.
        */
        macEntry.key.entryType              = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        macEntry.key.key.macVlan.vlanId = 100;

        macEntry.key.key.macVlan.macAddr.arEther[0] = 0x0;
        macEntry.key.key.macVlan.macAddr.arEther[1] = 0x1A;
        macEntry.key.key.macVlan.macAddr.arEther[2] = 0xFF;
        macEntry.key.key.macVlan.macAddr.arEther[3] = 0xFF;
        macEntry.key.key.macVlan.macAddr.arEther[4] = 0xFF;
        macEntry.key.key.macVlan.macAddr.arEther[5] = 0xFF;

        macEntry.dstInterface.type            = CPSS_INTERFACE_TRUNK_E;
        macEntry.dstInterface.devPort.devNum  = dev;
        macEntry.dstInterface.devPort.portNum = 0;
        macEntry.dstInterface.trunkId         = 10;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
        macEntry.dstInterface.vidx            = 10;
        macEntry.dstInterface.vlanId          = 100;

        macEntry.isStatic                 = GT_TRUE;
        macEntry.daCommand                = CPSS_MAC_TABLE_FRWRD_E;
        macEntry.saCommand                = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
        macEntry.daRoute                  = GT_FALSE;
        macEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
        macEntry.sourceID                 = 0;
        macEntry.userDefined              = 0;
        macEntry.daQosIndex               = 0;
        macEntry.saQosIndex               = 0;
        macEntry.daSecurityLevel          = 0;
        macEntry.saSecurityLevel          = 0;
        macEntry.appSpecificCpuCode       = GT_FALSE;
        macEntry.age                      = GT_TRUE;
        macEntry.spUnknown                = GT_FALSE;

        /* add MAC entry before QA send 
          this should be done to get SUCCESS during  
          utfCpssDxChBrgFdbFromCpuAuMsgStatusGet */
        st = utfCpssDxChBrgFdbMacEntrySet(dev, &macEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        st = utfCpssDxChBrgFdbQaSend(dev, &macEntry.key);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* wait for acknowledge that the QA was done and with SUCCESS */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /* clean FDB */
        st = utfCpssDxChBrgFdbMacEntryDelete(dev, &macEntry.key);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* wait for acknowledge that the delete was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.2. Call function with valid macEntryKeyPtr{entryType[CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E],
                                                         ipMcast->vlanId[100],
                                                         ipMcast->sip[123.23.34.5],
                                                         ipMacst->dip[123.45.67.8]}.
            Expected: GT_OK.
        */
        macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
        macEntry.key.key.ipMcast.vlanId = 100;

        macEntry.key.key.ipMcast.sip[0] = 123;
        macEntry.key.key.ipMcast.sip[1] = 23;
        macEntry.key.key.ipMcast.sip[2] = 34;
        macEntry.key.key.ipMcast.sip[3] = 5;

        macEntry.key.key.ipMcast.dip[0] = 123;
        macEntry.key.key.ipMcast.dip[1] = 45;
        macEntry.key.key.ipMcast.dip[2] = 67;
        macEntry.key.key.ipMcast.dip[3] = 8;

        st = utfCpssDxChBrgFdbQaSend(dev, &macEntry.key);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*  wait for acknowledge that the QA was done and without SUCCESS */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_FALSE);

        /*
            1.3. Call function with null macEntryKeyPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = utfCpssDxChBrgFdbQaSend(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);


        /*
            1.4. Call function with wrong enum values macEntryKeyPtr->entryType
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbQaSend
                            (dev, &macEntry.key),
                            macEntry.key.entryType);

        macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;


        /*
            1.5. Call function with out of range
                macEntryKeyPtr->key->ipMcast->vlanId[PRV_CPSS_MAX_NUM_VLANS_CNS],
                                    entryType [CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E].
            Expected: GT_OUT_OF_RANGE.
        */
        macEntry.key.entryType      = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
        macEntry.key.key.ipMcast.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = utfCpssDxChBrgFdbQaSend(dev, &macEntry.key);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st,
                                     "%d, macEntryPtr->key->ipMcast->vlanId = %d",
                                     dev, macEntry.key.key.ipMcast.vlanId);

        /*
            1.6. Call function with out of range
                    macEntryKeyPtr->key->macVlan->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                                    entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E].
            Expected: GT_OUT_OF_RANGE.
        */
        macEntry.key.key.ipMcast.vlanId     = 100;
        macEntry.key.key.macVlan.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

        st = utfCpssDxChBrgFdbQaSend(dev, &macEntry.key);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st,
                                     "%d, macEntryPtr->key->macVlan->vlanId = %d",
                                     dev, macEntry.key.key.macVlan.vlanId);

      /* wait for acknowledge that the QA was done and don't care about success */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_FALSE);
    }

    macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    macEntry.key.key.macVlan.vlanId = 100;

    macEntry.key.key.macVlan.macAddr.arEther[0] = 0x0;
    macEntry.key.key.macVlan.macAddr.arEther[1] = 0x1A;
    macEntry.key.key.macVlan.macAddr.arEther[2] = 0xFF;
    macEntry.key.key.macVlan.macAddr.arEther[3] = 0xFF;
    macEntry.key.key.macVlan.macAddr.arEther[4] = 0xFF;
    macEntry.key.key.macVlan.macAddr.arEther[5] = 0xFF;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = utfCpssDxChBrgFdbQaSend(dev, &macEntry.key);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = utfCpssDxChBrgFdbQaSend(dev, &macEntry.key);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbMacEntryDelete
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbMacEntryDelete)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with valid macEntryKeyPtr {entryType[CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                                  macVlan->vlanId[100],
                                                  macVlan->macAddr[00:1A:FF:FF:FF:FF]}
    Expected: GT_OK.
    1.2. Call function with valid macEntryKeyPtr {entryType[CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E],
                                                  ipMcast->vlanId[100],
                                                  ipMcast->sip[123.23.34.5],
                                                  ipMacst [123.45.67.8]}.
    Expected: GT_OK.
    1.3. Call function with null macEntryKeyPtr [NULL].
    Expected: GT_BAD_PTR.
    1.4. Call function with out of range macEntryKeyPtr->entryType
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call function with out of range macEntryKeyPtr->key->ipMcast->vlanId[PRV_CPSS_MAX_NUM_VLANS_CNS],
                            entryType [CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E].
    Expected: GT_OUT_OF_RANGE.
    1.6. Call function with out of range macEntryKeyPtr->key->macVlan->vlanId[PRV_CPSS_MAX_NUM_VLANS_CNS],
                            entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E].
    Expected: GT_OUT_OF_RANGE.
*/
    GT_STATUS                  st = GT_OK;


    GT_U8                      dev;
    CPSS_MAC_ENTRY_EXT_KEY_STC macEntry;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with valid macEntryKeyPtr {entryType[CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                                          macVlan->vlanId[100],
                                                          macVlan->macAddr[00:1A:FF:FF:FF:FF]}
            Expected: GT_OK.
        */
        macEntry.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        macEntry.key.macVlan.vlanId = 100;

        macEntry.key.macVlan.macAddr.arEther[0] = 0x0;
        macEntry.key.macVlan.macAddr.arEther[1] = 0x1A;
        macEntry.key.macVlan.macAddr.arEther[2] = 0xFF;
        macEntry.key.macVlan.macAddr.arEther[3] = 0xFF;
        macEntry.key.macVlan.macAddr.arEther[4] = 0xFF;
        macEntry.key.macVlan.macAddr.arEther[5] = 0xFF;

        st = utfCpssDxChBrgFdbMacEntryDelete(dev, &macEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.2. Call function with valid macEntryKeyPtr {entryType[CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E],
                                                          ipMcast->vlanId[100],
                                                          ipMcast->sip[123.23.34.5],
                                                          ipMacst->dip[123.45.67.8]}.
            Expected: GT_OK.
        */
        macEntry.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
        macEntry.key.ipMcast.vlanId = 100;

        macEntry.key.ipMcast.sip[0] = 123;
        macEntry.key.ipMcast.sip[1] = 23;
        macEntry.key.ipMcast.sip[2] = 34;
        macEntry.key.ipMcast.sip[3] = 5;

        macEntry.key.ipMcast.dip[0] = 123;
        macEntry.key.ipMcast.dip[1] = 45;
        macEntry.key.ipMcast.dip[2] = 67;
        macEntry.key.ipMcast.dip[3] = 8;

        st = utfCpssDxChBrgFdbMacEntryDelete(dev, &macEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.3. Call function with null macEntryKeyPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = utfCpssDxChBrgFdbMacEntryDelete(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

  /*
            1.4. Call function with wrong enum values macEntryKeyPtr->entryType
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbMacEntryDelete
                                                        (dev, &macEntry),
                                                        macEntry.entryType);

        macEntry.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.5. Call function with out of range
            macEntryKeyPtr->key->ipMcast->vlanId[PRV_CPSS_MAX_NUM_VLANS_CNS],
                                    entryType [CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E].
            Expected: GT_OUT_OF_RANGE.
        */
        macEntry.entryType      = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
        macEntry.key.ipMcast.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = utfCpssDxChBrgFdbMacEntryDelete(dev, &macEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st,
                        "%d, macEntryPtr->key->ipMcast->vlanId = %d",
                                     dev, macEntry.key.ipMcast.vlanId);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /*
            1.6. Call function with out of range
            macEntryKeyPtr->key->macVlan->vlanId[PRV_CPSS_MAX_NUM_VLANS_CNS],
                                    entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E].
            Expected: GT_OUT_OF_RANGE.
        */
        macEntry.key.ipMcast.vlanId     = 100;
        macEntry.key.macVlan.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        macEntry.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

        st = utfCpssDxChBrgFdbMacEntryDelete(dev, &macEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st,
                                     "%d, macEntryPtr->key->macVlan->vlanId = %d",
                                     dev, macEntry.key.macVlan.vlanId);

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

    }

    macEntry.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    macEntry.key.macVlan.vlanId = 100;

    macEntry.key.macVlan.macAddr.arEther[0] = 0x0;
    macEntry.key.macVlan.macAddr.arEther[1] = 0x1A;
    macEntry.key.macVlan.macAddr.arEther[2] = 0xFF;
    macEntry.key.macVlan.macAddr.arEther[3] = 0xFF;
    macEntry.key.macVlan.macAddr.arEther[4] = 0xFF;
    macEntry.key.macVlan.macAddr.arEther[5] = 0xFF;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = utfCpssDxChBrgFdbMacEntryDelete(dev, &macEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = utfCpssDxChBrgFdbMacEntryDelete(dev, &macEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbMacEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U32                       index,
    IN GT_BOOL                      skip,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbMacEntryWrite)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with index [0],
                            skip [GT_FALSE],
                            macEntryPtr {key {CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E,
                                              macVlan->vlanId[100],
                                              macVlan-> macAddr[00:1A:FF:FF:FF:FF]},
                                              dstInterface {type[CPSS_INTERFACE_TRUNK_E],
                                                            devPort {devNum [devNum],
                                                                     portNum [0]},
                                                             trunkId [10],
                                                             vidx [10],
                                                             vlanId [100]},
                                              isStatic [GT_FALSE],
                                              daCommand [CPSS_MAC_TABLE_FRWRD_E],
                                              saCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E],
                                              daRoute [GT_FALSE],
                                              mirrorToRxAnalyzerPortEn [GT_FALSE],
                                              sourceID [0],
                                              userDefined [0],
                                              daQosIndex [0],
                                              saQosIndex [0],
                                              daSecurityLevel [0],
                                              saSecurityLevel [0],
                                              appSpecificCpuCode [GT_FALSE]}.
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbMacEntryRead with index [0]
                                              and non-NULL pointers.
    Expected: GT_OK and the same parameters as input was - check by fields (only valid).
    1.3. Call function with index [1],
                            skip [GT_FALSE]
                            and valid macEntryPtr {key{CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E,
                                                       ipMcast->vlanId[100],
                                                       ipMcast->sip={123.12.12.3},
                                                       ipMcast->dip={123.34.67.8}},
                            dstInterface {type[CPSS_INTERFACE_VID_E],
                                          devPort {devNum [devNum],
                                                   portNum [0]},
                                          trunkId [0],
                                          vidx [10],
                                          vlanId [100]},
                            isStatic [GT_FALSE],
                            daCommand [CPSS_MAC_TABLE_INTERV_E],
                            saCommand [CPSS_MAC_TABLE_CNTL_E],
                            daRoute [GT_FALSE],
                            mirrorToRxAnalyzerPortEn [GT_FALSE],
                            sourceID [0xFFFF0000],
                            userDefined [0],
                            daQosIndex [30],
                            saQosIndex [101],
                            daSecurityLevel [10],
                            saSecurityLevel [40],
                            appSpecificCpuCode [GT_TRUE]}
    Expected: GT_OK.
    1.4. Call cpssDxChBrgFdbMacEntryRead with index [1]
                                              and non-NULL pointers.
    Expected: GT_OK and the same parameters as input
    1.5. Call with macEntryPtr->dstInterface->type [CPSS_INTERFACE_PORT_E]
                   and other - the same as in 1.3.
    Expected: NON GT_OK (IP MC support only VIDX and VID).
    1.6. Call function with index [2],
                            skip [GT_FALSE]
                            and valid macEntryPtr{key{CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E,
                                                      ipMcast->vlanId[100],
                                                      ipMcast->sip={123.12.12.3},
                                                      ipMcast->dip = {123.34.67.8}},
                             dstInterface {type[CPSS_INTERFACE_VIDX_E],
                                           devPort {devNum [devNum],
                                                    portNum [0]},
                                           trunkId [0],
                                           vidx [10],
                                           vlanId [100]},
                             isStatic [GT_FALSE],
                             daCommand [CPSS_MAC_TABLE_SOFT_DROP_E],
                             saCommand [CPSS_MAC_TABLE_DROP_E],
                             daRoute [GT_TRUE],
                             mirrorToRxAnalyzerPortEn [GT_TRUE],
                             sourceID [0],
                             userDefined [0],
                             daQosIndex [0],
                             saQosIndex [0],
                             daSecurityLevel [0],
                             saSecurityLevel [0],
                             appSpecificCpuCode [GT_FALSE]}.
    Expected: GT_OK.
    1.7. Call cpssDxChBrgFdbMacEntryRead with index [2]
                                              and non-NULL pointers.
    Expected: GT_OK and the same parameters as input
    1.8. Call with macEntryPtr->dstInterface->type [CPSS_INTERFACE_TRUNK_E]
                   and other - the same as in 1.6.
    Expected: NON GT_OK (IP MC support only VIDX and VID).
    1.9. Call function with out of range macEntryPtr->key->ipMcast->vlanId[PRV_CPSS_MAX_NUM_VLANS_CNS],
                            key->entryType [CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E].
    Expected: GT_BAD_PARAM.
    1.10. Call function with index [3]
                             and skip [GT_FALSE]
                             and valid macEntryPtr{key {CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E,
                                                        macVlan->vlanId[100],
                                                        macVlan->macAddr[00:1A:FF:FF:FF:FF]},
                             dstInterface {type[CPSS_INTERFACE_PORT_E],
                                           devPort {devNum [devNum],
                                                    portNum [0]},
                                           trunkId [0],
                                           vidx [10],
                                           vlanId [100]},
                             isStatic [GT_FALSE],
                             daCommand [CPSS_MAC_TABLE_FRWRD_E],
                             saCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E],
                             daRoute [GT_FALSE],
                             mirrorToRxAnalyzerPortEn [GT_FALSE],
                             sourceID [0],
                             userDefined [0],
                             daQosIndex [0],
                             saQosIndex [0],
                             daSecurityLevel [0],
                             saSecurityLevel [0],
                             appSpecificCpuCode [GT_FALSE]}.
    Expected: GT_OK.
    1.11. Call cpssDxChBrgFdbMacEntryRead with index [3]
                                               and non-NULL pointers.
    1.12. Call function with out of range index [macTableSize(dev)]
                             and other params from 1.10.
    Expected: NOT GT_OK.
    1.13. Call function with null macEntryPtr [NULL].
    Expected: GT_BAD_PTR.
    1.14. Call function with out of range key->entryType
                             and other parameters the same as in 1.10.
    Expected: GT_BAD_PARAM.
    1.15. Call function with out of range dstInterface->type
                             and other parameters the same as in 1.10.
    Expected: GT_BAD_PARAM.
    1.16. Call function with out of range macEntryPtr->key->macVlan->vlanId[PRV_CPSS_MAX_NUM_VLANS_CNS]
                             and other params from 1.10.
    Expected: GT_BAD_PARAM.
    1.17. Call function with out of range dstInterface->devPort->portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                             dstInterface->type = CPSS_INTERFACE_PORT_E
                             and other parameters the same as in 1.10.
    Expected: NOT GT_OK.
    1.18. Call function with out of range dstInterface->devPort->devNum[PRV_CPSS_MAX_PP_DEVICES_CNS],
                             dstInterface->type = CPSS_INTERFACE_PORT_E
                             and other parameters the same as in 1.10.
    Expected: NON GT_OK.
    1.19. Call function with out of range dstInterface->trunkId [128],
                             dstInterface->type = CPSS_INTERFACE_TRUNK_E
                             and other parameters the same as in 1.10.
    Expected: NOT GT_OK.
    1.20. Call function with out of range dstInterface->vidx [maxMeGrInd(dev)],
                             dstInterface->type = CPSS_INTERFACE_VIDX_E
                             and other parameters the same as in 1.10.
    Expected: NOT GT_OK.
    1.21. Call function with out of range dstInterface->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                             and dstInterface->type [CPSS_INTERFACE_VID_E].
    Expected: NON GT_OK.
    1.22. Call function with out of range macEntryPtr->daCommand
                             and other parameters the same as in 1.10.
    Expected: GT_BAD_PARAM.
    1.23. Call function with out of range macEntryPtr->saCommand
                             and other parameters the same as in 1.10.
    Expected: GT_BAD_PARAM.
    1.24. Call with userDefined [16] and other params from 1.10.
    Expected: NOT GT_OK.
    1.25. Call with daQosIndex [8] and other params from 1.10.
    Expected: NOT GT_OK.
    1.26. Call with saQosIndex [8] and other params from 1.10.
    Expected: NOT GT_OK.
    1.27. Call with daSecurityLevel [8] and other params from 1.10.
    Expected: NOT GT_OK.
    1.28. Call with saSecurityLevel [8] and other params from 1.10.
    Expected: NOT GT_OK.
    1.29. Call with sourceID [32] and other params from 1.10.
    Expected: NOT GT_OK.
    1.30. Call cpssDxChBrgFdbMacEntryInvalidate with index [0/ 1/ 2 / 3] to invalidate all changes
    Expected: GT_OK.
*/
    GT_STATUS               st          = GT_OK;

    GT_U8                   dev;
    GT_U32                  index       = 0;
    GT_BOOL                 skip        = GT_FALSE;
    CPSS_MAC_ENTRY_EXT_STC  macEntry;
    GT_BOOL                 validGet    = GT_FALSE;
    GT_BOOL                 skipGet     = GT_FALSE;
    GT_BOOL                 agedGet     = GT_FALSE;
    GT_U8                   devNumGet   = 0;
    CPSS_MAC_ENTRY_EXT_STC  entryGet;
    GT_BOOL                 isEqual     = GT_FALSE;
    GT_U16                  maxMcGroups = 4096;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with index [0],
                            skip [GT_FALSE],
                            macEntryPtr {key {CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E,
                                              macVlan->vlanId[100],
                                              macVlan-> macAddr[00:1A:FF:FF:FF:FF]},
                                              dstInterface {type[CPSS_INTERFACE_TRUNK_E],
                                                            devPort {devNum [devNum],
                                                                     portNum [0]},
                                                             trunkId [10],
                                                             vidx [10],
                                                             vlanId [100]},
                                              isStatic [GT_FALSE],
                                              daCommand [CPSS_MAC_TABLE_FRWRD_E],
                                              saCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E],
                                              daRoute [GT_FALSE],
                                              mirrorToRxAnalyzerPortEn [GT_FALSE],
                                              sourceID [0],
                                              userDefined [0],
                                              daQosIndex [0],
                                              saQosIndex [0],
                                              daSecurityLevel [0],
                                              saSecurityLevel [0],
                                              appSpecificCpuCode [GT_FALSE]}.
            Expected: GT_OK.
        */
        index = 0;
        skip  = GT_FALSE;

        macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        macEntry.key.key.macVlan.vlanId = 100;

        macEntry.key.key.macVlan.macAddr.arEther[0] = 0x0;
        macEntry.key.key.macVlan.macAddr.arEther[1] = 0x1A;
        macEntry.key.key.macVlan.macAddr.arEther[2] = 0xFF;
        macEntry.key.key.macVlan.macAddr.arEther[3] = 0xFF;
        macEntry.key.key.macVlan.macAddr.arEther[4] = 0xFF;
        macEntry.key.key.macVlan.macAddr.arEther[5] = 0xFF;

        macEntry.dstInterface.type            = CPSS_INTERFACE_TRUNK_E;
        macEntry.dstInterface.devPort.devNum  = dev;
        macEntry.dstInterface.devPort.portNum = 0;
        macEntry.dstInterface.trunkId         = 10;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
        macEntry.dstInterface.vidx            = 10;
        macEntry.dstInterface.vlanId          = 100;

        macEntry.isStatic                 = GT_FALSE;
        macEntry.daCommand                = CPSS_MAC_TABLE_FRWRD_E;
        macEntry.saCommand                = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
        macEntry.daRoute                  = GT_FALSE;
        macEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
        macEntry.sourceID                 = 0;
        macEntry.userDefined              = 0;
        macEntry.daQosIndex               = 0;
        macEntry.saQosIndex               = 0;
        macEntry.daSecurityLevel          = 0;
        macEntry.saSecurityLevel          = 0;
        macEntry.appSpecificCpuCode       = GT_FALSE;
        macEntry.age                      = GT_TRUE;
        macEntry.spUnknown                = GT_FALSE;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        /*
            1.2. Call cpssDxChBrgFdbMacEntryRead with index [0]
                                                      and non-NULL pointers.
            Expected: GT_OK and the same parameters as input was - check by fields (only valid).
        */
        index = 0;

        st = utfCpssDxChBrgFdbMacEntryRead(dev, index, &validGet, &skipGet, &agedGet, &devNumGet, &entryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgFdbMacEntryRead: %d, %d", dev, index);

        /* Verifying struct fields */
        UTF_VERIFY_EQUAL1_STRING_MAC(skip, skipGet,
                   "get another skipPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.key.entryType, entryGet.key.entryType,
                   "get another macEntryPtr->key.entryType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.key.key.macVlan.vlanId,
                                     entryGet.key.key.macVlan.vlanId,
                   "get another macEntryPtr->key.key.macVlan.vlanId than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macEntry.key.key.macVlan.macAddr,
                               (GT_VOID*) &entryGet.key.key.macVlan.macAddr,
                                sizeof(macEntry.key.key.macVlan.macAddr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another macEntryPtr->key.key.macVlan.macAddr than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.dstInterface.type, entryGet.dstInterface.type,
                   "get another macEntryPtr->dstInterface.type than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.dstInterface.trunkId,
                                     entryGet.dstInterface.trunkId,
                   "get another macEntryPtr->dstInterface.trunkId than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.isStatic, entryGet.isStatic,
                   "get another macEntryPtr->isStatic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daCommand, entryGet.daCommand,
                   "get another macEntryPtr->daCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.saCommand, entryGet.saCommand,
                   "get another macEntryPtr->saCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daRoute, entryGet.daRoute,
                   "get another macEntryPtr->daRoute than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.mirrorToRxAnalyzerPortEn,
                                     entryGet.mirrorToRxAnalyzerPortEn,
                   "get another macEntryPtr->mirrorToRxAnalyzerPortEn than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.sourceID, entryGet.sourceID,
                   "get another macEntryPtr->sourceID than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.userDefined, entryGet.userDefined,
                   "get another macEntryPtr->userDefined than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daQosIndex, entryGet.daQosIndex,
                   "get another macEntryPtr->daQosIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.saQosIndex, entryGet.saQosIndex,
                   "get another macEntryPtr->saQosIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daSecurityLevel, entryGet.daSecurityLevel,
                   "get another macEntryPtr->daSecurityLevel than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.saSecurityLevel, entryGet.saSecurityLevel,
                   "get another macEntryPtr->saSecurityLevel than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.appSpecificCpuCode, entryGet.appSpecificCpuCode,
                   "get another macEntryPtr->appSpecificCpuCode than was set: %d", dev);

        /*
            1.3. Call function with index [1], skip [GT_FALSE]
                            and valid macEntryPtr {key{CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E,
                                                       ipMcast->vlanId[100],
                                                       ipMcast->sip={123.12.12.3},
                                                       ipMcast->dip={123.34.67.8}},
                            dstInterface {type[CPSS_INTERFACE_VID_E],
                                          devPort {devNum [devNum],
                                                   portNum [0]},
                                          trunkId [0],
                                          vidx [10],
                                          vlanId [100]},
                            isStatic [GT_FALSE],
                            daCommand [CPSS_MAC_TABLE_INTERV_E],
                            saCommand [CPSS_MAC_TABLE_CNTL_E],
                            daRoute [GT_FALSE],
                            mirrorToRxAnalyzerPortEn [GT_FALSE],
                            sourceID [0xFFFF0000],
                            userDefined [0],
                            daQosIndex [30],
                            saQosIndex [101],
                            daSecurityLevel [0],
                            saSecurityLevel [7],
                            appSpecificCpuCode [GT_TRUE]}
            Expected: GT_OK.
        */
        index = 1;
        skip  = GT_FALSE;

        macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
        macEntry.key.key.ipMcast.vlanId = 100;

        macEntry.key.key.ipMcast.sip[0] = 123;
        macEntry.key.key.ipMcast.sip[1] = 12;
        macEntry.key.key.ipMcast.sip[2] = 12;
        macEntry.key.key.ipMcast.sip[3] = 3;

        macEntry.key.key.ipMcast.dip[0] = 123;
        macEntry.key.key.ipMcast.dip[1] = 34;
        macEntry.key.key.ipMcast.dip[2] = 67;
        macEntry.key.key.ipMcast.dip[3] = 8;

        macEntry.dstInterface.type            = CPSS_INTERFACE_VID_E;
        macEntry.dstInterface.devPort.devNum  = dev;
        macEntry.dstInterface.devPort.portNum = 0;
        macEntry.dstInterface.trunkId         = 0;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
        macEntry.dstInterface.vidx            = 10;
        macEntry.dstInterface.vlanId          = 100;

        macEntry.isStatic                 = GT_FALSE;
        macEntry.daCommand                = CPSS_MAC_TABLE_FRWRD_E;
        macEntry.saCommand                = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
        macEntry.daRoute                  = GT_FALSE;
        macEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
        macEntry.sourceID                 = 0xFFFF0000;
        macEntry.userDefined              = 0;
        macEntry.daQosIndex               = 4;
        macEntry.saQosIndex               = 5;
        macEntry.daSecurityLevel          = 0;
        macEntry.saSecurityLevel          = 7;
        macEntry.appSpecificCpuCode       = GT_TRUE;
        macEntry.age                      = GT_TRUE;
        macEntry.spUnknown                = GT_FALSE;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        /*
            1.4. Call cpssDxChBrgFdbMacEntryRead with index [1]
                                                      and non-NULL pointers.
            Expected: GT_OK and the same parameters as input
        */
        index = 1;

        st = utfCpssDxChBrgFdbMacEntryRead(dev, index, &validGet, &skipGet, &agedGet, &devNumGet, &entryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgFdbMacEntryRead: %d, %d", dev, index);

        /* Verifying struct fields */
        UTF_VERIFY_EQUAL1_STRING_MAC(skip, skipGet,
                   "get another skipPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.key.entryType, entryGet.key.entryType,
                   "get another macEntryPtr->key.entryType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.key.key.ipMcast.vlanId,
                                     entryGet.key.key.ipMcast.vlanId,
                   "get another macEntryPtr->key.key.ipMcast.vlanId than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macEntry.key.key.ipMcast.dip,
                               (GT_VOID*) &entryGet.key.key.ipMcast.dip,
                                sizeof(macEntry.key.key.ipMcast.dip))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another macEntryPtr->key.key.ipMcast.dip than was set: %d", dev);
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macEntry.key.key.ipMcast.sip,
                               (GT_VOID*) &entryGet.key.key.ipMcast.sip,
                                sizeof(macEntry.key.key.ipMcast.sip))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another macEntryPtr->key.key.ipMcast.sip than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.dstInterface.type, entryGet.dstInterface.type,
                   "get another macEntryPtr->dstInterface.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.dstInterface.vlanId,
                                     entryGet.dstInterface.vlanId,
                   "get another macEntryPtr->dstInterface.vlanId than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.isStatic, entryGet.isStatic,
                   "get another macEntryPtr->isStatic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daCommand, entryGet.daCommand,
                   "get another macEntryPtr->daCommand than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daRoute, entryGet.daRoute,
                   "get another macEntryPtr->daRoute than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.mirrorToRxAnalyzerPortEn,
                                     entryGet.mirrorToRxAnalyzerPortEn,
                   "get another macEntryPtr->mirrorToRxAnalyzerPortEn than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.userDefined, entryGet.userDefined,
                   "get another macEntryPtr->userDefined than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daQosIndex, entryGet.daQosIndex,
                   "get another macEntryPtr->daQosIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.saQosIndex, entryGet.saQosIndex,
                   "get another macEntryPtr->saQosIndex than was set: %d", dev);
        if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
           UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daSecurityLevel, entryGet.daSecurityLevel,
                   "get another macEntryPtr->daSecurityLevel than was set: %d", dev);
        if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
           UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.saSecurityLevel, entryGet.saSecurityLevel,
                   "get another macEntryPtr->saSecurityLevel than was set: %d", dev);
        if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
           UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.appSpecificCpuCode,
                                        entryGet.appSpecificCpuCode,
                   "get another macEntryPtr->appSpecificCpuCode than was set: %d", dev);

        /*
            1.5. Call with macEntryPtr->dstInterface->type [CPSS_INTERFACE_PORT_E]
                           and other - the same as in 1.3.
            Expected: NON GT_OK (IP MC support only VIDX and VID).
        */
        macEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, macEntryPtr->key.entryType = %d, macEntryPtr->dstInterface->type = %d",
                                         dev, macEntry.key.entryType, macEntry.dstInterface.type);

        /*
            1.6. Call function with index [2],
                            skip [GT_FALSE]
                            and valid macEntryPtr{key{CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E,
                                                      ipMcast->vlanId[100],
                                                      ipMcast->sip={123.12.12.3},
                                                      ipMcast->dip = {123.34.67.8}},
                             dstInterface {type[CPSS_INTERFACE_VIDX_E],
                                           devPort {devNum [devNum],
                                                    portNum [0]},
                                           trunkId [0],
                                           vidx [10],
                                           vlanId [100]},
                             isStatic [GT_FALSE],
                             daCommand [CPSS_MAC_TABLE_SOFT_DROP_E],
                             saCommand [CPSS_MAC_TABLE_DROP_E],
                             daRoute [GT_TRUE],
                             mirrorToRxAnalyzerPortEn [GT_TRUE],
                             sourceID [0],
                             userDefined [0],
                             daQosIndex [0],
                             saQosIndex [0],
                             daSecurityLevel [0],
                             saSecurityLevel [0],
                             appSpecificCpuCode [GT_FALSE]}.
            Expected: GT_OK.
        */
        index = 2;
        skip  = GT_FALSE;

        macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;
        macEntry.key.key.ipMcast.vlanId = 100;

        macEntry.key.key.ipMcast.sip[0] = 123;
        macEntry.key.key.ipMcast.sip[1] = 12;
        macEntry.key.key.ipMcast.sip[2] = 12;
        macEntry.key.key.ipMcast.sip[3] = 3;

        macEntry.key.key.ipMcast.dip[0] = 123;
        macEntry.key.key.ipMcast.dip[1] = 34;
        macEntry.key.key.ipMcast.dip[2] = 67;
        macEntry.key.key.ipMcast.dip[3] = 8;

        macEntry.dstInterface.type            = CPSS_INTERFACE_VID_E;
        macEntry.dstInterface.devPort.devNum  = dev;
        macEntry.dstInterface.devPort.portNum = 0;
        macEntry.dstInterface.trunkId         = 10;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
        macEntry.dstInterface.vidx            = 10;
        macEntry.dstInterface.vlanId          = 100;

        macEntry.isStatic                 = GT_FALSE;
        macEntry.daCommand                = CPSS_MAC_TABLE_SOFT_DROP_E;
        macEntry.saCommand                = CPSS_MAC_TABLE_DROP_E;
        macEntry.daRoute                  = GT_TRUE;
        macEntry.mirrorToRxAnalyzerPortEn = GT_TRUE;
        macEntry.sourceID                 = 0;
        macEntry.userDefined              = 0;
        macEntry.daQosIndex               = 0;
        macEntry.saQosIndex               = 0;
        macEntry.daSecurityLevel          = 0;
        macEntry.saSecurityLevel          = 0;
        macEntry.appSpecificCpuCode       = GT_FALSE;
        macEntry.age                      = GT_TRUE;
        macEntry.spUnknown                = GT_FALSE;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        /*
            1.7. Call cpssDxChBrgFdbMacEntryRead with index [2]
                                                      and non-NULL pointers.
            Expected: GT_OK and the same parameters as input
        */
        index = 2;

        st = utfCpssDxChBrgFdbMacEntryRead(dev, index, &validGet, &skipGet, &agedGet, &devNumGet, &entryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgFdbMacEntryRead: %d, %d", dev, index);

        /* Verifying struct fields */
        UTF_VERIFY_EQUAL1_STRING_MAC(skip, skipGet,
                   "get another skipPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.key.entryType, entryGet.key.entryType,
                   "get another macEntryPtr->key.entryType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.key.key.ipMcast.vlanId,
                                     entryGet.key.key.ipMcast.vlanId,
                   "get another macEntryPtr->key.key.ipMcast.vlanId than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macEntry.key.key.ipMcast.dip,
                               (GT_VOID*) &entryGet.key.key.ipMcast.dip,
                                sizeof(macEntry.key.key.ipMcast.dip))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another macEntryPtr->key.key.ipMcast.dip than was set: %d", dev);
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macEntry.key.key.ipMcast.sip,
                               (GT_VOID*) &entryGet.key.key.ipMcast.sip,
                               sizeof(macEntry.key.key.ipMcast.sip))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another macEntryPtr->key.key.ipMcast.sip than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.dstInterface.type, entryGet.dstInterface.type,
                   "get another macEntryPtr->dstInterface.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.dstInterface.vlanId, entryGet.dstInterface.vlanId,
                   "get another macEntryPtr->dstInterface.vlanId than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.isStatic, entryGet.isStatic,
                   "get another macEntryPtr->isStatic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daCommand, entryGet.daCommand,
                   "get another macEntryPtr->daCommand than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daRoute, entryGet.daRoute,
                   "get another macEntryPtr->daRoute than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.mirrorToRxAnalyzerPortEn,
                                     entryGet.mirrorToRxAnalyzerPortEn,
                   "get another macEntryPtr->mirrorToRxAnalyzerPortEn than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.userDefined, entryGet.userDefined,
                   "get another macEntryPtr->userDefined than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daQosIndex, entryGet.daQosIndex,
                   "get another macEntryPtr->daQosIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.saQosIndex, entryGet.saQosIndex,
                   "get another macEntryPtr->saQosIndex than was set: %d", dev);
        if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daSecurityLevel, entryGet.daSecurityLevel,
                   "get another macEntryPtr->daSecurityLevel than was set: %d", dev);
        if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.saSecurityLevel, entryGet.saSecurityLevel,
                   "get another macEntryPtr->saSecurityLevel than was set: %d", dev);
        if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
           UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.appSpecificCpuCode, entryGet.appSpecificCpuCode,
                   "get another macEntryPtr->appSpecificCpuCode than was set: %d", dev);

        /*
            1.8. Call with macEntryPtr->dstInterface->type [CPSS_INTERFACE_TRUNK_E]
                           and other - the same as in 1.6.
            Expected: NON GT_OK (IP MC support only VIDX and VID).
        */
        macEntry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, macEntryPtr->key.entryType = %d, macEntryPtr->dstInterface->type = %d",
                                         dev, macEntry.key.entryType, macEntry.dstInterface.type);

        /*
            1.9. Call function with out of range
                macEntryPtr->key->ipMcast->vlanId[PRV_CPSS_MAX_NUM_VLANS_CNS],
                                    key->entryType [CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E].
            Expected: GT_BAD_PARAM.
        */
        macEntry.key.key.ipMcast.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;
        macEntry.dstInterface.type      = CPSS_INTERFACE_VID_E;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st,
                        "%d, macEntryPtr->key->key->ipMcast->vlanId = %d",
                                     dev, macEntry.key.key.ipMcast.vlanId);

        /*
            1.10. Call function with index [3]
                             and skip [GT_FALSE]
                             and valid macEntryPtr{key {CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E,
                                                        macVlan->vlanId[100],
                                                        macVlan->macAddr[00:1A:FF:FF:FF:FF]},
                             dstInterface {type[CPSS_INTERFACE_PORT_E],
                                           devPort {devNum [devNum],
                                                    portNum [0]},
                                           trunkId [0],
                                           vidx [10],
                                           vlanId [100]},
                             isStatic [GT_FALSE],
                             daCommand [CPSS_MAC_TABLE_FRWRD_E],
                             saCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E],
                             daRoute [GT_FALSE],
                             mirrorToRxAnalyzerPortEn [GT_FALSE],
                             sourceID [0],
                             userDefined [0],
                             daQosIndex [0],
                             saQosIndex [0],
                             daSecurityLevel [0],
                             saSecurityLevel [0],
                             appSpecificCpuCode [GT_FALSE]}.
            Expected: GT_OK.
        */
        index = 3;
        skip  = GT_FALSE;

        macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        macEntry.key.key.macVlan.vlanId = 100;

        macEntry.key.key.macVlan.macAddr.arEther[0] = 0x0;
        macEntry.key.key.macVlan.macAddr.arEther[1] = 0x1A;
        macEntry.key.key.macVlan.macAddr.arEther[2] = 0xFF;
        macEntry.key.key.macVlan.macAddr.arEther[3] = 0xFF;
        macEntry.key.key.macVlan.macAddr.arEther[4] = 0xFF;
        macEntry.key.key.macVlan.macAddr.arEther[5] = 0xFF;

        macEntry.dstInterface.type            = CPSS_INTERFACE_PORT_E;
        macEntry.dstInterface.devPort.devNum  = dev;
        macEntry.dstInterface.devPort.portNum = 0;
        macEntry.dstInterface.trunkId         = 0;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
        macEntry.dstInterface.vidx            = 10;
        macEntry.dstInterface.vlanId          = 100;

        macEntry.isStatic                 = GT_FALSE;
        macEntry.daCommand                = CPSS_MAC_TABLE_FRWRD_E;
        macEntry.saCommand                = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
        macEntry.daRoute                  = GT_FALSE;
        macEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
        macEntry.sourceID                 = 0;
        macEntry.userDefined              = 0;
        macEntry.daQosIndex               = 0;
        macEntry.saQosIndex               = 0;
        macEntry.daSecurityLevel          = 0;
        macEntry.saSecurityLevel          = 0;
        macEntry.appSpecificCpuCode       = GT_FALSE;
        macEntry.age                      = GT_TRUE;
        macEntry.spUnknown                = GT_FALSE;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        /*
            1.11. Call cpssDxChBrgFdbMacEntryRead with index [3]
                                                       and non-NULL pointers.
        */
        index = 3;

        st = utfCpssDxChBrgFdbMacEntryRead(dev, index, &validGet, &skipGet, &agedGet, &devNumGet, &entryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgFdbMacEntryRead: %d, %d", dev, index);

        /* Verifying struct fields */
        UTF_VERIFY_EQUAL1_STRING_MAC(skip, skipGet,
                   "get another skipPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.key.entryType, entryGet.key.entryType,
                   "get another macEntryPtr->key.entryType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.key.key.macVlan.vlanId,
                                     entryGet.key.key.macVlan.vlanId,
                   "get another macEntryPtr->key.key.macVlan.vlanId than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macEntry.key.key.macVlan.macAddr,
                               (GT_VOID*) &entryGet.key.key.macVlan.macAddr,
                                sizeof(macEntry.key.key.macVlan.macAddr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another macEntryPtr->key.macVlan.key.macAddr than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.dstInterface.type, entryGet.dstInterface.type,
                   "get another macEntryPtr->dstInterface.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.dstInterface.devPort.devNum,
                                     entryGet.dstInterface.devPort.devNum,
                   "get another macEntryPtr->dstInterface.devPort.devNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.dstInterface.devPort.portNum,
                                     entryGet.dstInterface.devPort.portNum,
                   "get another macEntryPtr->dstInterface.devPort.portNum than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.isStatic, entryGet.isStatic,
                   "get another macEntryPtr->isStatic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daCommand, entryGet.daCommand,
                   "get another macEntryPtr->daCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.saCommand, entryGet.saCommand,
                   "get another macEntryPtr->saCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daRoute, entryGet.daRoute,
                   "get another macEntryPtr->daRoute than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.mirrorToRxAnalyzerPortEn,
                                     entryGet.mirrorToRxAnalyzerPortEn,
                   "get another macEntryPtr->mirrorToRxAnalyzerPortEn than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.sourceID, entryGet.sourceID,
                   "get another macEntryPtr->sourceID than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.userDefined, entryGet.userDefined,
                   "get another macEntryPtr->userDefined than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daQosIndex, entryGet.daQosIndex,
                   "get another macEntryPtr->daQosIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.saQosIndex, entryGet.saQosIndex,
                   "get another macEntryPtr->saQosIndex than was set: %d", dev);
        if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
           UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daSecurityLevel, entryGet.daSecurityLevel,
                   "get another macEntryPtr->daSecurityLevel than was set: %d", dev);
        if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
           UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.saSecurityLevel, entryGet.saSecurityLevel,
                   "get another macEntryPtr->saSecurityLevel than was set: %d", dev);
        if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
           UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.appSpecificCpuCode, entryGet.appSpecificCpuCode,
                   "get another macEntryPtr->appSpecificCpuCode than was set: %d", dev);

        /*
            1.12. Call function with out of range index [macTableSize(dev)]
                                     and other params from 1.10.
            Expected: NOT GT_OK.
        */
        st = prvUtfMacTableSize(dev, &index);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMacTableSize: %d", dev);

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.13. Call function with null macEntryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        index = 0;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macEntryPtr = NULL", dev);

        /*
            1.14. Call function with wrong enum values key->entryType
                             and other parameters the same as in 1.10.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbMacEntryWrite
                                                        (dev, index, skip, &macEntry),
                                                        macEntry.key.entryType);

        macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

        /*
            1.15. Call function with wrong enum values dstInterface->type
                                     and other parameters the same as in 1.10.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbMacEntryWrite
                                                        (dev, index, skip, &macEntry),
                                                        macEntry.dstInterface.type);

        macEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        /*
            1.16. Call function with out of range
                macEntryPtr->key->macVlan->vlanId[PRV_CPSS_MAX_NUM_VLANS_CNS]
                                     and other params from 1.10.
            Expected: GT_OUT_OF_RANGE.
        */
        macEntry.key.key.macVlan.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st,
                        "%d, macEntryPtr->key->key->macVlan->vlanId = %d",
                                     dev, macEntry.key.key.macVlan.vlanId);

        /*
            1.17. Call function with out of range
                dstInterface->devPort->portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                             dstInterface->type = CPSS_INTERFACE_PORT_E
                             and other parameters the same as in 1.10.
            Expected: NOT GT_OK.
        */
        macEntry.key.key.macVlan.vlanId       = 100;
        macEntry.dstInterface.devPort.portNum = 128;
        macEntry.dstInterface.type            = CPSS_INTERFACE_PORT_E;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                            "%d, macEntryPtr->dstInterface.devPort.portNum = %d",
                                         dev, macEntry.dstInterface.devPort.portNum);

        /*
            1.18. Call function with out of range
                        dstInterface->devPort->devNum[PRV_CPSS_MAX_PP_DEVICES_CNS],
                                     dstInterface->type = CPSS_INTERFACE_PORT_E
                                     and other parameters the same as in 1.10.
            Expected: NON GT_OK.
        */
        macEntry.dstInterface.devPort.portNum = BRG_FDB_VALID_PHY_PORT_CNS;
        macEntry.dstInterface.devPort.devNum  = PRV_CPSS_MAX_PP_DEVICES_CNS;
        macEntry.dstInterface.type            = CPSS_INTERFACE_PORT_E;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                        "%d, macEntryPtr->dstInterface->devPort->devNum = %d",
                                     dev, macEntry.dstInterface.devPort.devNum);

        /*
            1.19. Call function with out of range dstInterface->trunkId [128],
                             dstInterface->type = CPSS_INTERFACE_TRUNK_E
                             and other parameters the same as in 1.10.
            Expected: NOT GT_OK.
        */
        macEntry.dstInterface.devPort.devNum = dev;
        macEntry.dstInterface.trunkId        = MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
        macEntry.dstInterface.type           = CPSS_INTERFACE_TRUNK_E;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                            "%d, macEntryPtr->dstInterface->trunkId = %d",
                                         dev, macEntry.dstInterface.trunkId);

        /*
            1.20. Call function with out of range dstInterface->vidx [maxMeGrInd(dev)],
                             dstInterface->type = CPSS_INTERFACE_VIDX_E
                             and other parameters the same as in 1.10.
            Expected: NOT GT_OK.
        */
        macEntry.dstInterface.trunkId = 10;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
        macEntry.dstInterface.vidx    = maxMcGroups;
        macEntry.dstInterface.type    = CPSS_INTERFACE_VIDX_E;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                            "%d, macEntryPtr->dstInterface->vidx = %d",
                                         dev, macEntry.dstInterface.vidx);

        /*
            1.21. Call function with out of range
                    dstInterface->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                     and dstInterface->type [CPSS_INTERFACE_VID_E].
            Expected: NON GT_OK.
        */
        macEntry.dstInterface.vidx   = 10;
        macEntry.dstInterface.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        macEntry.dstInterface.type   = CPSS_INTERFACE_VID_E;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->dstInterface->vlanId = %d",
                                     dev, macEntry.dstInterface.vlanId);

        /*
            1.22. Call function with wrong enum values macEntryPtr->daCommand
                                     and other parameters the same as in 1.10.
            Expected: GT_BAD_PARAM.
        */
        macEntry.dstInterface.vlanId = 100;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbMacEntryWrite
                                                        (dev, index, skip, &macEntry),
                                                        macEntry.daCommand);

        macEntry.daCommand     = CPSS_MAC_TABLE_INTERV_E;

        /*
            1.23. Call function with wrong enum values macEntryPtr->saCommand
                                     and other parameters the same as in 1.10.
            Expected: GT_BAD_PARAM.
        */
        macEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbMacEntryWrite
                                                        (dev, index, skip, &macEntry),
                                                        macEntry.saCommand);

        macEntry.saCommand  = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;

        /*
            1.24. Call with userDefined [16] and other params from 1.10.
            Expected: NOT GT_OK.
        */
        macEntry.userDefined = 16;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->userDefined = %d",
                                     dev, macEntry.userDefined);

        macEntry.userDefined = 0;

        /*
            1.25. Call with daQosIndex [8] and other params from 1.10.
            Expected: NOT GT_OK.
        */
        macEntry.daQosIndex = 8;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->daQosIndex = %d",
                                     dev, macEntry.daQosIndex);

        macEntry.daQosIndex = 0;

        /*
            1.26. Call with saQosIndex [8] and other params from 1.10.
            Expected: NOT GT_OK.
        */
        macEntry.saQosIndex = 8;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->saQosIndex = %d",
                                     dev, macEntry.saQosIndex);

        macEntry.saQosIndex = 0;

        /*
            1.27. Call with daSecurityLevel [8] and other params from 1.10.
            Expected: NOT GT_OK.
        */
        macEntry.daSecurityLevel = 8;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->daSecurityLevel = %d",
                                     dev, macEntry.daSecurityLevel);

        macEntry.daSecurityLevel = 0;

        /*
            1.28. Call with saSecurityLevel [8] and other params from 1.10.
            Expected: NOT GT_OK.
        */
        macEntry.saSecurityLevel = 8;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->saSecurityLevel = %d",
                                     dev, macEntry.saSecurityLevel);

        macEntry.saSecurityLevel = 0;

        /*
            1.29. Call with sourceID [32] and other params from 1.10.
            Expected: NOT GT_OK.
        */
        macEntry.sourceID = 32;

        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macEntryPtr->sourceID = %d",
                                     dev, macEntry.sourceID);

        macEntry.sourceID = 0;

        /*
            1.30. Call cpssDxChBrgFdbMacEntryInvalidate with
                index [0/ 1/ 2 / 3] to invalidate all changes
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index = 0;
        st = utfCpssDxChBrgFdbMacEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChBrgFdbMacEntryInvalidate: %d, %d", dev, index);

        /* Call with index [1] */
        index = 1;

        st = utfCpssDxChBrgFdbMacEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChBrgFdbMacEntryInvalidate: %d, %d", dev, index);

        /* Call with index [2] */
        index = 2;

        st = utfCpssDxChBrgFdbMacEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChBrgFdbMacEntryInvalidate: %d, %d", dev, index);

        /* Call with index [3] */
        index = 3;

        st = utfCpssDxChBrgFdbMacEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChBrgFdbMacEntryInvalidate: %d, %d", dev, index);
    }

    index = 0;
    skip  = GT_FALSE;

    macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    macEntry.key.key.macVlan.vlanId = 100;

    macEntry.key.key.macVlan.macAddr.arEther[0] = 0x0;
    macEntry.key.key.macVlan.macAddr.arEther[1] = 0x1A;
    macEntry.key.key.macVlan.macAddr.arEther[2] = 0xFF;
    macEntry.key.key.macVlan.macAddr.arEther[3] = 0xFF;
    macEntry.key.key.macVlan.macAddr.arEther[4] = 0xFF;
    macEntry.key.key.macVlan.macAddr.arEther[5] = 0xFF;

    macEntry.dstInterface.type            = CPSS_INTERFACE_TRUNK_E;
    macEntry.dstInterface.devPort.devNum  = dev;
    macEntry.dstInterface.devPort.portNum = 0;
    macEntry.dstInterface.trunkId         = 10;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
    macEntry.dstInterface.vidx            = 10;
    macEntry.dstInterface.vlanId          = 100;

    macEntry.isStatic                 = GT_FALSE;
    macEntry.daCommand                = CPSS_MAC_TABLE_FRWRD_E;
    macEntry.saCommand                = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
    macEntry.daRoute                  = GT_FALSE;
    macEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
    macEntry.sourceID                 = 0;
    macEntry.userDefined              = 0;
    macEntry.daQosIndex               = 0;
    macEntry.saQosIndex               = 0;
    macEntry.daSecurityLevel          = 0;
    macEntry.saSecurityLevel          = 0;
    macEntry.appSpecificCpuCode       = GT_FALSE;
    macEntry.age                      = GT_TRUE;
    macEntry.spUnknown                = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = utfCpssDxChBrgFdbMacEntryWrite(dev, index, skip, &macEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbMacEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr,
    OUT GT_BOOL                 *agedPtr,
    OUT GT_U8                   *associatedDevNumPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbMacEntryRead)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with index [0],
                   non-null validPtr,
                   non-null skipPtr,
                   non-null agedPtr,
                   non-null associatedDevNumPtr
                   and non-null entryPtr.
    Expected: GT_OK.
    1.2. Call function with out of range index [macTableSize(dev)].
    Expected: NOT GT_OK.
    1.3. Call with index [0],
                   null validPtr [NULL],
                   non-null skipPtr,
                   non-null agedPtr,
                   non-null associatedDevNumPtr
                   and non-null entryPtr.
    Expected: GT_BAD_PTR.
    1.4. Call with index [0],
                   non-null validPtr,
                   null skipPtr [NULL],
                   non-null agedPtr,
                   non-null associatedDevNumPtr
                   and non-null entryPtr.
    Expected: GT_BAD_PTR.
    1.5. Call with index [0],
                   non-null validPtr,
                   non-null skipPtr,
                   null agedPtr [NULL],
                   non-null associatedDevNumPtr
                   and non-null entryPtr.
    Expected: GT_BAD_PTR.
    1.6. Call with index [0],
                   non-null validPtr,
                   non-null skipPtr,
                   non-null agedPtr,
                   null associatedDevNumPtr [NULL]
                   and non-null entryPtr.
    Expected: GT_BAD_PTR.
    1.7. Call with index [0],
                   non-null validPtr,
                   non-null skipPtr,
                   non-null agedPtr,
                   non-null associatedDevNumPtr
                   and null entryPtr [NULL].
    Expected: GT_BAD_PTR.
    1.8. Call with non-null msgRatePtr
                   and null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS              st        = GT_OK;

    GT_U8                  dev;
    GT_U32                 index     = 0;
    GT_U32                 tableSize = 0;
    GT_BOOL                valid     = GT_FALSE;
    GT_BOOL                skip      = GT_FALSE;
    GT_BOOL                aged      = GT_FALSE;
    GT_U8                  devNum    = 0;
    CPSS_MAC_ENTRY_EXT_STC entry;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0],
                           non-null validPtr,
                           non-null skipPtr,
                           non-null agedPtr,
                           non-null associatedDevNumPtr
                           and non-null entryPtr.
            Expected: GT_OK.
        */
        st = utfCpssDxChBrgFdbMacEntryRead(dev, index, &valid, &skip,
                                        &aged, &devNum, &entry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Getting table size */
        st = prvUtfMacTableSize(dev, &tableSize);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMacTableSize: %d", dev);

        /*
            1.2. Call function with out of range index [macTableSize(dev)].
            Expected: NOT GT_OK.
        */
        index = tableSize;

        st = utfCpssDxChBrgFdbMacEntryRead(dev, index, &valid, &skip,
                                        &aged, &devNum, &entry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.3. Call with index [0],
                           null validPtr [NULL],
                           non-null skipPtr,
                           non-null agedPtr,
                           non-null associatedDevNumPtr
                           and non-null entryPtr.
            Expected: GT_BAD_PTR.
        */
        index = 0;

        st = utfCpssDxChBrgFdbMacEntryRead(dev, index, NULL, &skip,
                                        &aged, &devNum, &entry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);

        /*
            1.4. Call with index [0],
                           non-null validPtr,
                           null skipPtr [NULL],
                           non-null agedPtr,
                           non-null associatedDevNumPtr
                           and non-null entryPtr.
            Expected: GT_BAD_PTR.
        */
        st = utfCpssDxChBrgFdbMacEntryRead(dev, index, &valid, NULL,
                                        &aged, &devNum, &entry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, skipPtr = NULL", dev);

        /*
            1.5. Call with index [0],
                           non-null validPtr,
                           non-null skipPtr,
                           null agedPtr [NULL],
                           non-null associatedDevNumPtr
                           and non-null entryPtr.
            Expected: GT_BAD_PTR.
        */
        st = utfCpssDxChBrgFdbMacEntryRead(dev, index, &valid, &skip,
                                        NULL, &devNum, &entry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, agedPtr = NULL", dev);

        /*
            1.6. Call with index [0],
                           non-null validPtr,
                           non-null skipPtr,
                           non-null agedPtr,
                           null associatedDevNumPtr [NULL]
                           and non-null entryPtr.
            Expected: GT_BAD_PTR.
        */
        index = 0;

        st = utfCpssDxChBrgFdbMacEntryRead(dev, index, &valid, &skip,
                                        &aged, NULL, &entry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, associatedDevNumPtr = NULL", dev);

        /*
            1.7. Call with index [0],
                           non-null validPtr,
                           non-null skipPtr,
                           non-null agedPtr,
                           non-null associatedDevNumPtr
                           and null entryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = utfCpssDxChBrgFdbMacEntryRead(dev, index, &valid, &skip,
                                        &aged, &devNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = utfCpssDxChBrgFdbMacEntryRead(dev, index, &valid, &skip,
                                        &aged, &devNum, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = utfCpssDxChBrgFdbMacEntryRead(dev, index, &valid, &skip,
                                        &aged, &devNum, &entry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbMacEntryInvalidate
(
    IN GT_U8         devNum,
    IN GT_U32        index
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbMacEntryInvalidate)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with index [0].
    Expected: GT_OK.
    1.2. Call with out of range index [macTableSize(dev)].
    Expected: NOT GT_OK.
*/
    GT_STATUS  st        = GT_OK;

    GT_U8      dev;
    GT_U32     index     = 0;
    GT_U32     tableSize = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with index [0].
            Expected: GT_OK.
        */
        index = 0;

        st = utfCpssDxChBrgFdbMacEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Getting table size */
        st = prvUtfMacTableSize(dev, &tableSize);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMacTableSize: %d", dev);

        /*
            1.2. Call with out of range index [macTableSize(dev)].
            Expected: NOT GT_OK.
        */
        index = tableSize;

        st = utfCpssDxChBrgFdbMacEntryInvalidate(dev, index);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = utfCpssDxChBrgFdbMacEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = utfCpssDxChBrgFdbMacEntryInvalidate(dev, index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbMaxLookupLenSet
(
    IN GT_U8    devNum,
    IN GT_U32   lookupLen
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbMaxLookupLenSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with lookupLen [4].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbMaxLookupLenGet
    Expected: GT_OK and the same lookupLenPtr.
    1.3. Call function with out of range lookupLen [0, 36].
    Expected: NOT GT_OK.
    1.4. Call function with out of range lookupLen [5].
    Expected: NOT GT_OK.
*/
    GT_STATUS  st     = GT_OK;

    GT_U8      dev;
    GT_U32     len    = 0;
    GT_U32     lenGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with lookupLen [4].
            Expected: GT_OK.
        */
        len = 4;

        st = cpssDxChBrgFdbMaxLookupLenSet(dev, len);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, len);

        /*
            1.2. Call cpssDxChBrgFdbMaxLookupLenGet
            Expected: GT_OK and the same lookupLenPtr.
        */
        st = cpssDxChBrgFdbMaxLookupLenGet(dev, &lenGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbMaxLookupLenGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(len, lenGet,
                       "get another lookupLenPtr than was set: %d", dev);

        /*
            1.3. Call function with out of range lookupLen [0, 36].
            Expected: NOT GT_OK.
        */

        /* Call with lookupLen [0] */
        len = 0;

        st = cpssDxChBrgFdbMaxLookupLenSet(dev, len);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, len);

        /* Call with lookupLen [36] */
        len = 36;

        st = cpssDxChBrgFdbMaxLookupLenSet(dev, len);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, len);

        /*
            1.4. Call function with out of range lookupLen [5].
            Expected: NOT GT_OK.
        */
        len = 5;

        st = cpssDxChBrgFdbMaxLookupLenSet(dev, len);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, len);
    }

    len = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbMaxLookupLenSet(dev, len);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbMaxLookupLenSet(dev, len);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbMaxLookupLenGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *lookupLenPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbMaxLookupLenGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null lookupLenPtr.
    Expected: GT_OK.
    1.2. Call with null lookupLenPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;

    GT_U8       dev;
    GT_U32      len = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null lookupLenPtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbMaxLookupLenGet(dev, &len);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null lookupLenPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbMaxLookupLenGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbMaxLookupLenGet(dev, &len);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbMaxLookupLenGet(dev, &len);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbMacVlanLookupModeSet
(
    IN GT_U8            devNum,
    IN CPSS_MAC_VL_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbMacVlanLookupModeSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with mode [CPSS_IVL_EE / CPSS_SVL_E].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbMacVlanLookupModeGet
    Expected: GT_OK and the same modePtr.
    1.3. Call function with out of range mode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS          st      = GT_OK;


    GT_U8              dev;
    CPSS_MAC_VL_ENT    mode    = CPSS_IVL_E;
    CPSS_MAC_VL_ENT    modeGet = CPSS_IVL_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_IVL_EE / CPSS_SVL_E].
            Expected: GT_OK.
        */

        /* 1.1. Call with mode [CPSS_IVL_EE] */
        mode = CPSS_IVL_E;

        st = cpssDxChBrgFdbMacVlanLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssDxChBrgFdbMacVlanLookupModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssDxChBrgFdbMacVlanLookupModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbMacVlanLookupModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /* 1.1. Call with mode [CPSS_SVL_E] */
        mode = CPSS_SVL_E;

        st = cpssDxChBrgFdbMacVlanLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssDxChBrgFdbMacVlanLookupModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssDxChBrgFdbMacVlanLookupModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbMacVlanLookupModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /*
            1.3. Call function with wrong enum values mode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbMacVlanLookupModeSet
                                                        (dev, mode),
                                                        mode);
    }

    mode = CPSS_IVL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbMacVlanLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbMacVlanLookupModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbMacVlanLookupModeGet
(
    IN  GT_U8            devNum,
    OUT CPSS_MAC_VL_ENT  *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbMacVlanLookupModeGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null modePtr.
    Expected: GT_OK.
    1.2. Call with null modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS        st  = GT_OK;

    GT_U8            dev;
    CPSS_MAC_VL_ENT  mode = CPSS_IVL_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null modePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbMacVlanLookupModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbMacVlanLookupModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbMacVlanLookupModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbMacVlanLookupModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbAuMsgRateLimitSet
(
    IN GT_U8                       devNum,
    IN GT_U32                      msgRate,
    IN GT_BOOL                     enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbAuMsgRateLimitSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with msgRate [2 * (200 * coreClock / 200)], enable [GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbAuMsgRateLimitGet
    Expected: GT_OK and the same msgRatePtr and enablePtr.
    1.3. Call with out of range
    msgRate [(51200/200 + 1) * (200 * coreClock / 200)], enable [GT_TRUE].
    Expected: NOT GT_OK.
    1.4. Call with out of range
    msgRate [(51200/200 + 1) * (200 * coreClock / 200)], enable [GT_FALSE].
    Expected: GT_OK.
*/
    GT_STATUS   st        = GT_OK;

    GT_U8       dev;
    GT_U32      rate      = 0;
    GT_BOOL     enable    = GT_FALSE;
    GT_U32      rateGet   = 0;
    GT_BOOL     enableGet = GT_FALSE;
    GT_U32      coreClock = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting core clock */
        st = prvUtfCoreClockGet(dev, &coreClock);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCoreClockGet: %d", dev);

        /*
            1.1. Call with msgRate [1], enable [GT_TRUE].
            Expected: GT_OK.
        */
        rate   = 2 * (200 * coreClock / 200);
        enable = GT_TRUE;

        st = cpssDxChBrgFdbAuMsgRateLimitSet(dev, rate, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, rate, enable);

        /*
            1.2. Call cpssDxChBrgFdbAuMsgRateLimitGet
            Expected: GT_OK and the same msgRatePtr and enablePtr.
        */
        st = cpssDxChBrgFdbAuMsgRateLimitGet(dev,&rateGet,&enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbAuMsgRateLimitGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /*
            1.3. Call with out of range msgRate [51200*devClock(dev)/200],
                   enable [GT_TRUE].
            Expected: NOT GT_OK.
        */
        rate   = (51200/200 + 1) * (200 * coreClock / 200);
        enable = GT_TRUE;

        st = cpssDxChBrgFdbAuMsgRateLimitSet(dev, rate, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, rate, enable);

        /*
            1.4. Call with out of range msgRate [51000*devClock(dev)/200],
                           enable [GT_FALSE].
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        st = cpssDxChBrgFdbAuMsgRateLimitSet(dev, rate, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, rate, enable);
    }

    rate   = 200;
    enable = GT_TRUE;


    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbAuMsgRateLimitSet(dev, rate, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbAuMsgRateLimitSet(dev, rate, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbAuMsgRateLimitGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *msgRatePtr,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbAuMsgRateLimitGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null msgRatePtr and non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null msgRatePtr [NULL] and non-null enablePtr.
    Expected: GT_BAD_PTR.
    1.3. Call with non-null msgRatePtr and null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U32      rate   = 0;
    GT_BOOL     enable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null msgRatePtr and non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbAuMsgRateLimitGet(dev, &rate, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null msgRatePtr [NULL] and non-null enablePtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbAuMsgRateLimitGet(dev, NULL, &enable);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);

        /*
            1.3. Call with non-null msgRatePtr and null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbAuMsgRateLimitGet(dev, &rate, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbAuMsgRateLimitGet(dev, &rate, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbAuMsgRateLimitGet(dev, &rate, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbNaMsgOnChainTooLongSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbNaMsgOnChainTooLongSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbNaMsgOnChainTooLongGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgFdbNaMsgOnChainTooLongSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbNaMsgOnChainTooLongGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbNaMsgOnChainTooLongGet(dev, &stateGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbNaMsgOnChainTooLongGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgFdbNaMsgOnChainTooLongSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbNaMsgOnChainTooLongGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbNaMsgOnChainTooLongGet(dev, &stateGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbNaMsgOnChainTooLongGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbNaMsgOnChainTooLongSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbNaMsgOnChainTooLongSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbNaMsgOnChainTooLongGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbNaMsgOnChainTooLongGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbNaMsgOnChainTooLongGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbNaMsgOnChainTooLongGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbNaMsgOnChainTooLongGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbNaMsgOnChainTooLongGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbSpAaMsgToCpuSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbSpAaMsgToCpuSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbSpAaMsgToCpuGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgFdbSpAaMsgToCpuSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbSpAaMsgToCpuGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbSpAaMsgToCpuGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbSpAaMsgToCpuGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgFdbSpAaMsgToCpuSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbSpAaMsgToCpuGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbSpAaMsgToCpuGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbSpAaMsgToCpuGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbSpAaMsgToCpuSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbSpAaMsgToCpuSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbSpAaMsgToCpuGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbSpAaMsgToCpuGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbSpAaMsgToCpuGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbSpAaMsgToCpuGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbSpAaMsgToCpuGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbSpAaMsgToCpuGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbAAandTAToCpuSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbAAandTAToCpuSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbAAandTAToCpuGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgFdbAAandTAToCpuSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbAAandTAToCpuGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbAAandTAToCpuGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbAAandTAToCpuGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgFdbAAandTAToCpuSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbAAandTAToCpuGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbAAandTAToCpuGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbAAandTAToCpuGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbAAandTAToCpuSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbAAandTAToCpuSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbAAandTAToCpuGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbAAandTAToCpuGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbAAandTAToCpuGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbAAandTAToCpuGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbAAandTAToCpuGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbAAandTAToCpuGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbHashModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_HASH_FUNC_MODE_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbHashModeSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with mode [CPSS_MAC_HASH_FUNC_XOR_E /
                                  CPSS_MAC_HASH_FUNC_CRC_E].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbHashModeGet
    Expected: GT_OK and the same modePtr.
    1.3. Call function with out of range mode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                   st      = GT_OK;

    GT_U8                       dev;
    CPSS_MAC_HASH_FUNC_MODE_ENT mode    = CPSS_MAC_HASH_FUNC_XOR_E;
    CPSS_MAC_HASH_FUNC_MODE_ENT modeGet = CPSS_MAC_HASH_FUNC_XOR_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_MAC_HASH_FUNC_XOR_E /
                                          CPSS_MAC_HASH_FUNC_CRC_E].
            Expected: GT_OK.
        */

        /* 1.1. Call with mode [CPSS_MAC_HASH_FUNC_XOR_E] */
        mode = CPSS_MAC_HASH_FUNC_XOR_E;

        st = cpssDxChBrgFdbHashModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssDxChBrgFdbHashModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssDxChBrgFdbHashModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbHashModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /* 1.1. Call with mode [CPSS_MAC_HASH_FUNC_CRC_E] */
        mode = CPSS_MAC_HASH_FUNC_CRC_E;

        st = cpssDxChBrgFdbHashModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssDxChBrgFdbHashModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssDxChBrgFdbHashModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbHashModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /*
            1.3. Call function with wrong enum values mode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbHashModeSet
                                                        (dev, mode),
                                                        mode);
    }

    mode = CPSS_MAC_HASH_FUNC_XOR_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbHashModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbHashModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbHashModeGet
(
    IN  GT_U8                         devNum,
    OUT CPSS_MAC_HASH_FUNC_MODE_ENT   *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbHashModeGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null modePtr.
    Expected: GT_OK.
    1.2. Call with null modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                    st   = GT_OK;

    GT_U8                        dev;
    CPSS_MAC_HASH_FUNC_MODE_ENT  mode = CPSS_MAC_HASH_FUNC_XOR_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null modePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbHashModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbHashModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbHashModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbHashModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbAgingTimeoutSet
(
    IN GT_U8  devNum,
    IN GT_U32 timeout
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbAgingTimeoutSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with timeout [300].
    Expected: GT_OK.
    1.2. Call function with timeout [615].
    Expected: NOT GT_OK for device with 200 MHZ.
    1.3. Call function with timeout [890].
    Expected: NOT GT_OK for device with 144 MHZ
              and NOT GT_OK for device with 200 MHZ.
    1.4. Call function with timeout [140, 143].
    Expected: GT_OK.
    1.5. Call cpssDxChBrgFdbAgingTimeoutGet
    Expected: GT_OK and timeoutPtr == 140.
    1.6. Call function with timeout [150].
    Expected: GT_OK.
    1.7. Call cpssDxChBrgFdbAgingTimeoutGet
    Expected: GT_OK and timeoutPtr == 140 for clock 144 MHZ
              and timeoutPtr == 150 for clock 200 MHZ.
    1.8. Call function with timeout [160].
    Expected: GT_OK.
    1.9. Call cpssDxChBrgFdbAgingTimeoutGet
    Expected: GT_OK and timeoutPtr == 154 for clock 144 MHZ
              and timeoutPtr == 160 for clock 200 MHZ.
*/
    GT_STATUS   st         = GT_OK;

    GT_U8       dev;
    GT_U32      timeout    = 0;
    GT_U32      timeoutGet = 0;
    GT_U32      coreClock  = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with timeout [300].
            Expected: GT_OK.
        */
        timeout = 300;

        st = cpssDxChBrgFdbAgingTimeoutSet(dev, timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);

        /* Getting core clock */
        st = prvUtfCoreClockGet(dev, &coreClock);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCoreClockGet: %d", dev);

        /*
            1.2. Call function with timeout [615].
            Expected: NOT GT_OK for device with 200 MHZ.
        */
        timeout = 615;

        st = cpssDxChBrgFdbAgingTimeoutSet(dev, timeout);
        if (200 == coreClock)
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);
        }

        /*
            1.3. Call function with timeout [890].
            Expected: NOT GT_OK for device with 144 MHZ
                      and NOT GT_OK for device with 200 MHZ.
        */
        timeout = 890;

        st = cpssDxChBrgFdbAgingTimeoutSet(dev, timeout);
        if ((200 == coreClock) || (144 == coreClock))
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);
        }

        /*
            1.4. Call function with timeout [140, 143].
            Expected: GT_OK.
        */

        /* timeout [140] */
        timeout = 140;

        st = cpssDxChBrgFdbAgingTimeoutSet(dev, timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);

        /*
            1.5. Call cpssDxChBrgFdbAgingTimeoutGet
            Expected: GT_OK and timeoutPtr == 140.
        */
        st = cpssDxChBrgFdbAgingTimeoutGet(dev, &timeoutGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbAgingTimeoutGet: %d", dev);

        if ((200 == coreClock) || (220 == coreClock))
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(140, timeoutGet,
                           "get another timeoutPtr than was set: %d", dev);
        }

        /* timeout [143] */
        timeout = 143;

        st = cpssDxChBrgFdbAgingTimeoutSet(dev, timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);

        /*
            1.5. Call cpssDxChBrgFdbAgingTimeoutGet
            Expected: GT_OK and timeoutPtr == 140.
        */
        st = cpssDxChBrgFdbAgingTimeoutGet(dev, &timeoutGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbAgingTimeoutGet: %d", dev);

        if ((200 == coreClock) || (220 == coreClock))
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(140, timeoutGet,
                           "get another timeoutPtr than was set: %d", dev);
        }

        /*
            1.6. Call function with timeout [150].
            Expected: GT_OK.
        */
        timeout = 150;

        st = cpssDxChBrgFdbAgingTimeoutSet(dev, timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);

        /*
            1.7. Call cpssDxChBrgFdbAgingTimeoutGet
            Expected: GT_OK and timeoutPtr == 140 for clock 144 MHZ
                      and timeoutPtr == 150 for clock 200 MHZ.
        */
        st = cpssDxChBrgFdbAgingTimeoutGet(dev, &timeoutGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbAgingTimeoutGet: %d", dev);
        if (200 == coreClock)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(150, timeoutGet,
                       "get another timeoutPtr than was set: %d", dev);
        }
        else if (144 == coreClock)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(140, timeoutGet,
                       "get another timeoutPtr than was set: %d", dev);
        }

        /*
            1.8. Call function with timeout [160].
            Expected: GT_OK.
        */
        timeout = 160;

        st = cpssDxChBrgFdbAgingTimeoutSet(dev, timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);

        /*
            1.9. Call cpssDxChBrgFdbAgingTimeoutGet
            Expected: GT_OK and timeoutPtr == 154 for clock 144 MHZ
                      and timeoutPtr == 160 for clock 200 MHZ.
        */
        st = cpssDxChBrgFdbAgingTimeoutGet(dev, &timeoutGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbAgingTimeoutGet: %d", dev);
        if (200 == coreClock)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(160, timeoutGet,
                       "get another timeoutPtr than was set: %d", dev);
        }
        else if (144 == coreClock)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(154, timeoutGet,
                       "get another timeoutPtr than was set: %d", dev);
        }
    }

    timeout = 300;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbAgingTimeoutSet(dev, timeout);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbAgingTimeoutSet(dev, timeout);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbAgingTimeoutGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *timeoutPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbAgingTimeoutGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null timeoutPtr.
    Expected: GT_OK.
    1.2. Call with null timeoutPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS    st      = GT_OK;

    GT_U8        dev;
    GT_U32       timeout = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null timeoutPtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbAgingTimeoutGet(dev, &timeout);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null devTableBmpPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbAgingTimeoutGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbAgingTimeoutGet(dev, &timeout);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbAgingTimeoutGet(dev, &timeout);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbSecureAgingSet
(
    IN GT_U8          devNum,
    IN GT_BOOL        enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbSecureAgingSet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbSecureAgingGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgFdbSecureAgingSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbSecureAgingGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbSecureAgingGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbSecureAgingGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgFdbSecureAgingSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbSecureAgingGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbSecureAgingGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbSecureAgingGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbSecureAgingSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbSecureAgingSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbSecureAgingGet
(
    IN  GT_U8          devNum,
    OUT GT_BOOL        *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbSecureAgingGet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbSecureAgingGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbSecureAgingGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbSecureAgingGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbSecureAgingGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbSecureAutoLearnSet
(
    IN GT_U8                                devNum,
    IN CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbSecureAutoLearnSet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call function with mode [CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E /
                                  CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E /
                                  CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E /
                                  CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbSecureAutoLearnGet
    Expected: GT_OK and the same modePtr.
    1.3. Call function with out of range mode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                           st      = GT_OK;

    GT_U8                               dev;
    CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT mode    = CPSS_IVL_E;
    CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT modeGet = CPSS_IVL_E;



    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E /
                                  CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E /
                                  CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E /
                                  CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E].
            Expected: GT_OK.
        */

        /* 1.1. Call with mode [CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E] */
        mode = CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E;

        st = cpssDxChBrgFdbSecureAutoLearnSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssDxChBrgFdbSecureAutoLearnGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssDxChBrgFdbSecureAutoLearnGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbSecureAutoLearnGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* 1.1. Call with mode [CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E] */
        mode = CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E;

        st = cpssDxChBrgFdbSecureAutoLearnSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssDxChBrgFdbSecureAutoLearnGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssDxChBrgFdbSecureAutoLearnGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbSecureAutoLearnGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* 1.1. Call with mode [CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E] */
        mode = CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E;

        st = cpssDxChBrgFdbSecureAutoLearnSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssDxChBrgFdbSecureAutoLearnGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssDxChBrgFdbSecureAutoLearnGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbSecureAutoLearnGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /* 1.1. Call with mode [CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E] */
        mode = CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E;

        st = cpssDxChBrgFdbSecureAutoLearnSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssDxChBrgFdbSecureAutoLearnGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssDxChBrgFdbSecureAutoLearnGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbSecureAutoLearnGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /*
            1.3. Call function with wrong enum values mode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbSecureAutoLearnSet
                                                        (dev, mode),
                                                        mode);
    }

    mode = CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbSecureAutoLearnSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbSecureAutoLearnSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbSecureAutoLearnGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbSecureAutoLearnGet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with non-null modePtr.
    Expected: GT_OK.
    1.2. Call with null modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                           st   = GT_OK;

    GT_U8                               dev;
    CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT mode = CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null modePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbSecureAutoLearnGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbSecureAutoLearnGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbSecureAutoLearnGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbSecureAutoLearnGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbStaticTransEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbStaticTransEnable)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbStaticTransEnableGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgFdbStaticTransEnable(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbStaticTransEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbStaticTransEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbStaticTransEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgFdbStaticTransEnable(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbStaticTransEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbStaticTransEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbStaticTransEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbStaticTransEnable(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbStaticTransEnable(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbStaticTransEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbStaticTransEnableGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbStaticTransEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbStaticTransEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbStaticTransEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbStaticTransEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbStaticDelEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbStaticDelEnable)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbStaticDelEnableGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgFdbStaticDelEnable(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbStaticDelEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbStaticDelEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbStaticDelEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgFdbStaticDelEnable(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbStaticDelEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbStaticDelEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbStaticDelEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbStaticDelEnable(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbStaticDelEnable(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbStaticDelEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbStaticDelEnableGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbStaticDelEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbStaticDelEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbStaticDelEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbStaticDelEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbActionsEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbActionsEnableSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbActionsEnableGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgFdbActionsEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbActionsEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbActionsEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbActionsEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgFdbActionsEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbActionsEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbActionsEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbActionsEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbActionsEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbActionsEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbActionsEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbActionsEnableGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbActionsEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbActionsEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbActionsEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbActionsEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbTrigActionStatusGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *actFinishedPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbTrigActionStatusGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null actFinishedPtr.
    Expected: GT_OK.
    1.2. Call with null actFinishedPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null actFinishedPtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbTrigActionStatusGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null actFinishedPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbTrigActionStatusGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbTrigActionStatusGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbTrigActionStatusGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbMacTriggerModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ACTION_MODE_ENT     mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbMacTriggerModeSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with mode [CPSS_ACT_TRIG_E/
                                  CPSS_ACT_AUTO_E].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbMacTriggerModeGet
    Expected: GT_OK and the same modePtr.
    1.3. Call function with out of range mode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                st      = GT_OK;


    GT_U8                    dev;
    CPSS_MAC_ACTION_MODE_ENT mode    = CPSS_ACT_TRIG_E;
    CPSS_MAC_ACTION_MODE_ENT modeGet = CPSS_ACT_TRIG_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_ACT_TRIG_E/
                                          CPSS_ACT_AUTO_E].
            Expected: GT_OK.
        */

        /* 1.1. Call with mode [CPSS_ACT_TRIG_E] */
        mode = CPSS_ACT_TRIG_E;

        st = cpssDxChBrgFdbMacTriggerModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssDxChBrgFdbMacTriggerModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssDxChBrgFdbMacTriggerModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbMacTriggerModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /* 1.1. Call with mode [CPSS_ACT_AUTO_E] */
        mode = CPSS_ACT_AUTO_E;

        st = cpssDxChBrgFdbMacTriggerModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssDxChBrgFdbMacTriggerModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssDxChBrgFdbMacTriggerModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbMacTriggerModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /*
            1.3. Call function with wrong enum values mode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbMacTriggerModeSet
                                                        (dev, mode),
                                                        mode);
    }

    mode = CPSS_ACT_TRIG_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbMacTriggerModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbMacTriggerModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbMacTriggerModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_MAC_ACTION_MODE_ENT     *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbMacTriggerModeGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null modePtr.
    Expected: GT_OK.
    1.2. Call with null modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                st   = GT_OK;

    GT_U8                    dev;
    CPSS_MAC_ACTION_MODE_ENT mode = CPSS_ACT_AUTO_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null modePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbMacTriggerModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbMacTriggerModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbMacTriggerModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbMacTriggerModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbStaticOfNonExistDevRemove
(
    IN GT_U8                       devNum,
    IN GT_BOOL                     deleteStatic
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbStaticOfNonExistDevRemove)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with deleteStatic [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
    Expected: GT_OK and the same deleteStaticPtr.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with deleteStatic [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with deleteStatic [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgFdbStaticOfNonExistDevRemove(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
            Expected: GT_OK and the same deleteStaticPtr.
        */
        st = cpssDxChBrgFdbStaticOfNonExistDevRemoveGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbStaticOfNonExistDevRemoveGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another deleteStaticPtr than was set: %d", dev);

        /* Call with deleteStatic [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgFdbStaticOfNonExistDevRemove(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
            Expected: GT_OK and the same deleteStaticPtr.
        */
        st = cpssDxChBrgFdbStaticOfNonExistDevRemoveGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbStaticOfNonExistDevRemoveGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another deleteStaticPtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbStaticOfNonExistDevRemove(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbStaticOfNonExistDevRemove(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *deleteStaticPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbStaticOfNonExistDevRemoveGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null deleteStaticPtr.
    Expected: GT_OK.
    1.2. Call with null deleteStaticPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null deleteStaticPtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbStaticOfNonExistDevRemoveGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null deleteStaticPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbStaticOfNonExistDevRemoveGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbStaticOfNonExistDevRemoveGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbStaticOfNonExistDevRemoveGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbDropAuEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbDropAuEnableSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbDropAuEnableGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgFdbDropAuEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbDropAuEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbDropAuEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbDropAuEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgFdbDropAuEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbDropAuEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbDropAuEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbDropAuEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbDropAuEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbDropAuEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbDropAuEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbDropAuEnableGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbDropAuEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbDropAuEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbDropAuEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbDropAuEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  ageOutAllDevOnTrunkEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with ageOutAllDevOnTrunkEnable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
    Expected: GT_OK and the same ageOutAllDevOnTrunkEnablePtr.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with ageOutAllDevOnTrunkEnable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with ageOutAllDevOnTrunkEnable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
            Expected: GT_OK and the same ageOutAllDevOnTrunkEnablePtr.
        */
        st = cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another ageOutAllDevOnTrunkEnablePtr than was set: %d", dev);

        /* Call with ageOutAllDevOnTrunkEnable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
            Expected: GT_OK and the same ageOutAllDevOnTrunkEnablePtr.
        */
        st = cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another ageOutAllDevOnTrunkEnablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *ageOutAllDevOnTrunkEnablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null ageOutAllDevOnTrunkEnablePtr.
    Expected: GT_OK.
    1.2. Call with null ageOutAllDevOnTrunkEnablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null ageOutAllDevOnTrunkEnablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null ageOutAllDevOnTrunkEnablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  ageOutAllDevOnNonTrunkEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with ageOutAllDevOnNonTrunkEnable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
    Expected: GT_OK and the same ageOutAllDevOnNonTrunkEnablePtr.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with ageOutAllDevOnNonTrunkEnable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with ageOutAllDevOnNonTrunkEnable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
            Expected: GT_OK and the same ageOutAllDevOnNonTrunkEnablePtr.
        */
        st = cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another ageOutAllDevOnNonTrunkEnablePtr than was set: %d", dev);

        /* Call with ageOutAllDevOnNonTrunkEnable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
            Expected: GT_OK and the same ageOutAllDevOnNonTrunkEnablePtr.
        */
        st = cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another ageOutAllDevOnNonTrunkEnablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *ageOutAllDevOnNonTrunkEnablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null ageOutAllDevOnNonTrunkEnablePtr.
    Expected: GT_OK.
    1.2. Call with null ageOutAllDevOnNonTrunkEnablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null ageOutAllDevOnNonTrunkEnablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null ageOutAllDevOnNonTrunkEnablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbActionTransplantDataSet
(
    IN GT_U8                    devNum,
    IN CPSS_INTERFACE_INFO_STC  *oldInterfacePtr,
    IN CPSS_INTERFACE_INFO_STC  *newInterfacePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbActionTransplantDataSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function oldInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [100]},
                       newInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                         trunkId [2],
                                         vidx [0],
                                         vlanId [0]}.
    Expected: GT_OK.
    1.2. Call function oldInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [100]},
                       newInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [100]}.
    Expected: GT_OK.
    1.3. Call cpssDxChBrgFdbActionTransplantDataGet.
    Expected: GT_OK and the same oldInterfacePtr and newInterfacePtr.
    1.4. Call function oldInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]},
                       newInterfacePtr {type[CPSS_INTERFACE_VID_E /
                                             CPSS_INTERFACE_VIDX_E] (not supported),
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]}.
    Expected: NOT GT_OK.
    1.5. Call function oldInterfacePtr {type[CPSS_INTERFACE_VID_E /
                                             CPSS_INTERFACE_VIDX_E] (not supported),
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]},
                       newInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]}.
    Expected: NOT GT_OK.
    1.6. Call function oldInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        out of range trunkId [MAX_TRUNK_ID_CNS = 128],
                                        vidx [0],
                                        vlanId [0]},
                       newInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]}.
    Expected: NOT GT_OK.
    1.7. Call function oldInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [MAX_TRUNK_ID_CNS = 128],
                                        vidx [0],
                                        vlanId [0]},
                       newInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]}.
    Expected: GT_OK.
    1.8. Call function oldInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                        out of range devPort{
                                            out of range devNum[PRV_CPSS_MAX_PP_DEVICES_CNS],
                                                             portNum [0]},
                                        trunkId [1],
                                        vidx [0],
                                        vlanId [0]},
                       newInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]}.
    Expected: NOT GT_OK.
    1.9. Call function oldInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                        out of range devPort {devNum [2],
                                        out of range  portNum[PRV_CPSS_MAX_PP_PORTS_NUM_CNS]},
                                        trunkId [1], vidx [0], vlanId [0]},
                       newInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]}.
    Expected: NOT GT_OK.
    1.10. Call function with oldInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                              out of range devPort {devNum [2],
                                              out of range portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]},
                                              trunkId [1],
                                              vidx [0],
                                              vlanId [0]},
                             newInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                              devPort {devNum [devNum],
                                                       portNum [0]},
                                              trunkId [0],
                                              vidx [0],
                                              vlanId [0]}.
    Expected: GT_OK.
    1.11. Call function with null oldInterfacePtr [NULL]
                             and valid newInterfacePtr
    Expected: GT_BAD_PTR.
    1.12. Call function with valid non-null oldInterfacePtr
                             and null newInterfacePtr[NULL].
    Expected: GT_BAD_PTR.
    1.13. Call function with out of range oldInterfacePtr->type
                             and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.14. Call function with out of range newInterfacePtr->type
                             and other params from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;


    CPSS_INTERFACE_INFO_STC oldInterface;
    CPSS_INTERFACE_INFO_STC newInterface;
    CPSS_INTERFACE_INFO_STC oldIfGet;
    CPSS_INTERFACE_INFO_STC newIfGet;

    cpssOsBzero((GT_VOID*) &oldInterface, sizeof(oldInterface));
    cpssOsBzero((GT_VOID*) &newInterface, sizeof(newInterface));
    cpssOsBzero((GT_VOID*) &oldIfGet, sizeof(oldIfGet));
    cpssOsBzero((GT_VOID*) &newIfGet, sizeof(newIfGet));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function oldInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                                devPort {devNum [devNum],
                                                         portNum [0]},
                                                trunkId [0],
                                                vidx [0],
                                                vlanId [100]},
                       newInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                         trunkId [2],
                                         vidx [0],
                                         vlanId [0]}.
            Expected: GT_OK.
        */
        oldInterface.type            = CPSS_INTERFACE_PORT_E;
        oldInterface.devPort.devNum  = dev;
        oldInterface.devPort.portNum = 0;
        oldInterface.trunkId         = 0;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(oldInterface.trunkId);
        oldInterface.vidx            = 0;
        oldInterface.vlanId          = 100;

        newInterface.type            = CPSS_INTERFACE_TRUNK_E;
        newInterface.devPort.devNum  = dev;
        newInterface.devPort.portNum = 0;
        newInterface.trunkId         = 2;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(newInterface.trunkId);
        newInterface.vidx            = 0;
        newInterface.vlanId          = 0;

        st = cpssDxChBrgFdbActionTransplantDataSet(dev, &oldInterface, &newInterface);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function oldInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                                 trunkId [0],
                                                 vidx [0],
                                                 vlanId [100]},
                                newInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                                 devPort {devNum [devNum],
                                                          portNum [0]},
                                                 trunkId [0],
                                                 vidx [0],
                                                 vlanId [100]}.
            Expected: GT_OK.
        */
        oldInterface.type            = CPSS_INTERFACE_TRUNK_E;
        oldInterface.devPort.devNum  = dev;
        oldInterface.devPort.portNum = 0;
        oldInterface.trunkId         = 0;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(oldInterface.trunkId);
        oldInterface.vidx            = 0;
        oldInterface.vlanId          = 100;

        newInterface.type            = CPSS_INTERFACE_PORT_E;
        newInterface.devPort.devNum  = dev;
        newInterface.devPort.portNum = 0;
        newInterface.trunkId         = 0;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(newInterface.trunkId);
        newInterface.vidx            = 0;
        newInterface.vlanId          = 100;

        st = cpssDxChBrgFdbActionTransplantDataSet(dev, &oldInterface, &newInterface);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.3. Call cpssDxChBrgFdbActionTransplantDataGet.
            Expected: GT_OK and the same oldInterfacePtr and newInterfacePtr.
        */
        st = cpssDxChBrgFdbActionTransplantDataGet(dev, &oldIfGet, &newIfGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbActionTransplantDataGet: %d", dev);

        /* Verifying oldInterfacePtr */
        UTF_VERIFY_EQUAL1_STRING_MAC(oldInterface.type, oldIfGet.type,
                       "get another oldInterfacePtr->type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(oldInterface.trunkId, oldIfGet.trunkId,
                       "get another oldInterfacePtr->trunkId than was set: %d", dev);

        /* Verifying newInterfacePtr */
        UTF_VERIFY_EQUAL1_STRING_MAC(newInterface.type, newIfGet.type,
                       "get another newInterfacePtr->type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(newInterface.devPort.portNum, newIfGet.devPort.portNum,
                       "get another newInterfacePtr->devPort.portNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(newInterface.devPort.devNum, newIfGet.devPort.devNum,
                       "get another newInterfacePtr->devPort.devNum than was set: %d", dev);

        /*
            1.4. Call function oldInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]},
                       newInterfacePtr {type[CPSS_INTERFACE_VID_E /
                                             CPSS_INTERFACE_VIDX_E] (not supported),
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]}.
            Expected: NOT GT_OK.
        */

        /* Call with newInterfacePtr->type[CPSS_INTERFACE_VID_E] */
        oldInterface.type = CPSS_INTERFACE_PORT_E;
        newInterface.type = CPSS_INTERFACE_VID_E;

        st = cpssDxChBrgFdbActionTransplantDataSet(dev, &oldInterface, &newInterface);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                            "%d, newInterfacePtr->type = %d", dev, newInterface.type);

        /* Call with newInterfacePtr->type[CPSS_INTERFACE_VIDX_E] */
        newInterface.type = CPSS_INTERFACE_VIDX_E;

        st = cpssDxChBrgFdbActionTransplantDataSet(dev, &oldInterface, &newInterface);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                            "%d, newInterfacePtr->type = %d", dev, newInterface.type);

        /*
            1.5. Call function oldInterfacePtr {type[CPSS_INTERFACE_VID_E /
                                             CPSS_INTERFACE_VIDX_E] (not supported),
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]},
                       newInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]}.
            Expected: NOT GT_OK.
        */

        /* Call with oldInterfacePtr->type[CPSS_INTERFACE_VID_E] */
        oldInterface.type = CPSS_INTERFACE_VID_E;
        newInterface.type = CPSS_INTERFACE_TRUNK_E;

        st = cpssDxChBrgFdbActionTransplantDataSet(dev, &oldInterface, &newInterface);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                            "%d, oldInterfacePtr->type = %d", dev, oldInterface.type);

        /* Call with oldInterfacePtr->type[CPSS_INTERFACE_VIDX_E] */
        oldInterface.type = CPSS_INTERFACE_VIDX_E;

        st = cpssDxChBrgFdbActionTransplantDataSet(dev, &oldInterface, &newInterface);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                            "%d, oldInterfacePtr->type = %d", dev, oldInterface.type);

        /*
            1.6. Call function oldInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        out of range trunkId [MAX_TRUNK_ID_CNS = 128],
                                        vidx [0],
                                        vlanId [0]},
                       newInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]}.
            Expected: NOT GT_OK.
        */
        oldInterface.type    = CPSS_INTERFACE_TRUNK_E;
        oldInterface.trunkId = MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(oldInterface.trunkId);

        newInterface.type    = CPSS_INTERFACE_PORT_E;

        st = cpssDxChBrgFdbActionTransplantDataSet(dev, &oldInterface, &newInterface);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, oldInterfacePtr->type = %d, oldInterfacePtr->trunkId = %d",
                                        dev, oldInterface.type, oldInterface.trunkId);

        /*
            1.7. Call function oldInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [MAX_TRUNK_ID_CNS = 128],
                                        vidx [0],
                                        vlanId [0]},
                       newInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]}.
            Expected: GT_OK.
        */
        oldInterface.type    = CPSS_INTERFACE_PORT_E;
        oldInterface.trunkId = MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(oldInterface.trunkId);

        st = cpssDxChBrgFdbActionTransplantDataSet(dev, &oldInterface, &newInterface);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, oldInterfacePtr->type = %d, oldInterfacePtr->trunkId = %d",
                                    dev, oldInterface.type, oldInterface.trunkId);

        /*
            1.8. Call function oldInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                      out of range devPort{out of range devNum[PRV_CPSS_MAX_PP_DEVICES_CNS],
                                                             portNum [0]},
                                        trunkId [1],
                                        vidx [0],
                                        vlanId [0]},
                       newInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                        devPort {devNum [devNum],
                                                 portNum [0]},
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]}.
            Expected: NOT GT_OK.
        */
        oldInterface.type           = CPSS_INTERFACE_PORT_E;
        oldInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        oldInterface.trunkId        = 1;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(oldInterface.trunkId);

        newInterface.type = CPSS_INTERFACE_TRUNK_E;

        st = cpssDxChBrgFdbActionTransplantDataSet(dev, &oldInterface, &newInterface);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, oldInterfacePtr->type = %d, oldInterfacePtr->devPort->devNum = %d",
                                     dev, oldInterface.type, oldInterface.devPort.devNum);

        /*
            1.9. Call function oldInterfacePtr {type[CPSS_INTERFACE_PORT_E],
                                        out of range devPort {devNum [2],
                                        out of range  portNum[PRV_CPSS_MAX_PP_PORTS_NUM_CNS]},
                                        trunkId [1], vidx [0], vlanId [0]},
                       newInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                        trunkId [0],
                                        vidx [0],
                                        vlanId [0]}.
            Expected: NOT GT_OK.
        */
        oldInterface.type            = CPSS_INTERFACE_PORT_E;
        oldInterface.devPort.devNum  = dev;
        oldInterface.devPort.portNum = 128;

        st = cpssDxChBrgFdbActionTransplantDataSet(dev, &oldInterface, &newInterface);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, oldInterfacePtr->type = %d, oldInterfacePtr->devPort->portNum = %d",
                                     dev, oldInterface.type, oldInterface.devPort.portNum);

        /*
            1.10. Call function with oldInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                              out of range devPort {devNum [2],
                                               out of range portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]},
                                              trunkId [1],
                                              vidx [0],
                                              vlanId [0]},
                             newInterfacePtr {type[CPSS_INTERFACE_TRUNK_E],
                                              devPort {devNum [devNum],
                                                       portNum [0]},
                                              trunkId [0],
                                              vidx [0],
                                              vlanId [0]}.
            Expected: GT_OK.
        */
        oldInterface.type            = CPSS_INTERFACE_TRUNK_E;
        oldInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgFdbActionTransplantDataSet(dev, &oldInterface, &newInterface);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, oldInterfacePtr->type = %d, oldInterfacePtr->devPort->portNum = %d",
                                     dev, oldInterface.type, oldInterface.devPort.portNum);

        /*
            1.11. Call function with null oldInterfacePtr [NULL]
                             and valid newInterfacePtr
            Expected: GT_BAD_PTR.
        */
        oldInterface.devPort.portNum = BRG_FDB_VALID_PHY_PORT_CNS;

        st = cpssDxChBrgFdbActionTransplantDataSet(dev, NULL, &newInterface);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, oldInterfacePtr = NULL", dev);

        /*
            1.12. Call function with valid non-null oldInterfacePtr
                             and null newInterfacePtr[NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbActionTransplantDataSet(dev, &oldInterface, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, newInterfacePtr = NULL", dev);

        /*
            1.13. Call function with wrong enum values oldInterfacePtr->type
                             and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbActionTransplantDataSet
                                                        (dev, &oldInterface, &newInterface),
                                                        oldInterface.type);

        /*
            1.14. Call function with wrong enum values newInterfacePtr->type
                             and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbActionTransplantDataSet
                                                        (dev, &oldInterface, &newInterface),
                                                        newInterface.type);
    }

    oldInterface.type            = CPSS_INTERFACE_PORT_E;
    oldInterface.devPort.devNum  = dev;
    oldInterface.devPort.portNum = 0;
    oldInterface.trunkId         = 0;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(oldInterface.trunkId);
    oldInterface.vidx            = 0;
    oldInterface.vlanId          = 100;

    newInterface.type            = CPSS_INTERFACE_TRUNK_E;
    newInterface.devPort.devNum  = dev;
    newInterface.devPort.portNum = 0;
    newInterface.trunkId         = 2;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(newInterface.trunkId);
    newInterface.vidx            = 0;
    newInterface.vlanId          = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbActionTransplantDataSet(dev, &oldInterface, &newInterface);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbActionTransplantDataSet(dev, &oldInterface, &newInterface);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbActionTransplantDataGet
(
    IN  GT_U8                    devNum,
    OUT CPSS_INTERFACE_INFO_STC  *oldInterfacePtr,
    OUT CPSS_INTERFACE_INFO_STC  *newInterfacePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbActionTransplantDataGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null oldInterfacePtr and non-null newInterfacePtr.
    Expected: GT_OK.
    1.2. Call with null oldInterfacePtr [NULL] and non-null newInterfacePtr.
    Expected: GT_BAD_PTR.
    1.3. Call with non-null oldInterfacePtr and null newInterfacePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_INTERFACE_INFO_STC oldInterface;
    CPSS_INTERFACE_INFO_STC newInterface;

    cpssOsBzero((GT_VOID*) &oldInterface, sizeof(oldInterface));
    cpssOsBzero((GT_VOID*) &newInterface, sizeof(newInterface));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null oldInterfacePtr and non-null newInterfacePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbActionTransplantDataGet(dev, &oldInterface, &newInterface);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null oldInterfacePtr [NULL] and non-null newInterfacePtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbActionTransplantDataGet(dev, NULL, &newInterface);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, oldInterfacePtr = NULL", dev);

        /*
            1.3. Call with non-null oldInterfacePtr and null newInterfacePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbActionTransplantDataGet(dev, &oldInterface, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, newInterfacePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbActionTransplantDataGet(dev, &oldInterface, &newInterface);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbActionTransplantDataGet(dev, &oldInterface, &newInterface);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbFromCpuAuMsgStatusGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *completedPtr,
    OUT GT_BOOL  *succeededPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbFromCpuAuMsgStatusGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null completedPtr and non-null succeededPtr.
    Expected: GT_OK.
    1.2. Call with null completedPtr [NULL] and non-null succeededPtr.
    Expected: GT_BAD_PTR.
    1.3. Call with non-null completedPtr and null succeededPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st        = GT_OK;

    GT_U8       dev;
    GT_BOOL     completed = GT_FALSE;
    GT_BOOL     succeeded = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null completedPtr and non-null succeededPtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbFromCpuAuMsgStatusGet(dev, &completed, &succeeded);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null completedPtr [NULL] and non-null succeededPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbFromCpuAuMsgStatusGet(dev, NULL, &succeeded);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, completedPtr = NULL", dev);

        /*
            1.3. Call with non-null completedPtr and null succeededPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbFromCpuAuMsgStatusGet(dev, &completed, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, completedPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbFromCpuAuMsgStatusGet(dev, &completed, &succeeded);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbFromCpuAuMsgStatusGet(dev, &completed, &succeeded);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbActionActiveVlanSet
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_U16   vlanMask
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbActionActiveVlanSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with vlanId [1] and vlanMask [2].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbActionActiveVlanGet
    Expected: GT_OK and the same vlanIdPtr and vlanMaskPtr.
    1.3. Call function with vlanId [0] and vlanMask [0].
    Expected: GT_OK.
    1.4. Call cpssDxChBrgFdbActionActiveVlanGet
    Expected: GT_OK and the same vlanIdPtr and vlanMaskPtr.
    1.5. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                            and vlanMask [1].
    Expected: GT_OUT_OF_RANGE.
    1.6. Call function with vlanId [2]
                            and out of range vlanMask [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: GT_OUT_OF_RANGE.
*/
    GT_STATUS   st          = GT_OK;

    GT_U8       dev;
    GT_U16      vlanId      = 0;
    GT_U16      vlanMask    = 0;
    GT_U16      vlanIdGet   = 0;
    GT_U16      vlanMaskGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with vlanId [1]
                                    and vlanMask [2].
            Expected: GT_OK.
        */
        vlanId   = 1;
        vlanMask = 2;

        st = cpssDxChBrgFdbActionActiveVlanSet(dev, vlanId, vlanMask);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, vlanMask);

        /*
            1.2. Call cpssDxChBrgFdbActionActiveVlanGet
            Expected: GT_OK and the same vlanIdPtr and vlanMaskPtr.
        */
        st = cpssDxChBrgFdbActionActiveVlanGet(dev, &vlanIdGet, &vlanMaskGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbActionActiveVlanGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vlanId, vlanIdGet,
                       "get another vlanIdPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vlanMask, vlanMaskGet,
                       "get another vlanMaskPtr than was set: %d", dev);

        /*
            1.3. Call function with vlanId [100]
                                    and vlanMask [0].
            Expected: GT_OK.
        */
        vlanId   = 100;
        vlanMask = 0;

        st = cpssDxChBrgFdbActionActiveVlanSet(dev, vlanId, vlanMask);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, vlanMask);

        /*
            1.4. Call cpssDxChBrgFdbActionActiveVlanGet
            Expected: GT_OK and the same vlanIdPtr and vlanMaskPtr.
        */
        st = cpssDxChBrgFdbActionActiveVlanGet(dev, &vlanIdGet, &vlanMaskGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbActionActiveVlanGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vlanId, vlanIdGet,
                       "get another vlanIdPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vlanMask, vlanMaskGet,
                       "get another vlanMaskPtr than was set: %d", dev);

        /*
            1.5. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                    and vlanMask [1].
            Expected: GT_OUT_OF_RANGE.
        */
        vlanId   = PRV_CPSS_MAX_NUM_VLANS_CNS;
        vlanMask = 1;

        st = cpssDxChBrgFdbActionActiveVlanSet(dev, vlanId, vlanMask);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, vlanId);

        /*
            1.6. Call function with vlanId [2]
                                    and out of range vlanMask [PRV_CPSS_MAX_NUM_VLANS_CNS].
            Expected: GT_OUT_OF_RANGE.
        */
        vlanId   = 2;
        vlanMask = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgFdbActionActiveVlanSet(dev, vlanId, vlanMask);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, vlanMask = %d", dev ,vlanMask);
    }

    vlanId   = 1;
    vlanMask = 2;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbActionActiveVlanSet(dev, vlanId, vlanMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbActionActiveVlanSet(dev, vlanId, vlanMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbActionActiveVlanGet
(
    IN  GT_U8    devNum,
    OUT GT_U16   *vlanIdPtr,
    OUT GT_U16   *vlanMaskPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbActionActiveVlanGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null vlanIdPtr
                   and non-null vlanMaskPtr.
    Expected: GT_OK.
    1.2. Call with null vlanIdPtr [NULL]
                   and non-null vlanMaskPtr.
    Expected: GT_BAD_PTR.
    1.3. Call with non-null vlanIdPtr
                   and null vlanMaskPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U16      vlanId   = 0;
    GT_U16      vlanMask = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null vlanIdPtr
                   and non-null vlanMaskPtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbActionActiveVlanGet(dev, &vlanId, &vlanMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null vlanIdPtr [NULL]
                   and non-null vlanMaskPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbActionActiveVlanGet(dev, NULL, &vlanMask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vlanIdPtr = NULL", dev);

        /*
            1.3. Call with non-null vlanIdPtr
                           and null vlanMaskPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbActionActiveVlanGet(dev, &vlanId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vlanMaskPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbActionActiveVlanGet(dev, &vlanId, &vlanMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbActionActiveVlanGet(dev, &vlanId, &vlanMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbActionActiveDevSet
(
    IN GT_U8    devNum,
    IN GT_U32   actDev,
    IN GT_U32   actDevMask
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbActionActiveDevSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with actDev [0, 31]
                            and actDevMask [1, 31].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbActionActiveDevGet
    Expected: GT_OK and the same actDevPtr and actDevMaskPtr.
    1.3. Call function with out of range actDev [32]
                            and actDevMask [1].
    Expected: NOT GT_OK.
    1.4. Call function with actDev [2] and out of range actDevMask [32].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st            = GT_OK;

    GT_U8       dev;
    GT_U32      actDev        = 0;
    GT_U32      actDevMask    = 0;
    GT_U32      actDevGet     = 0;
    GT_U32      actDevMaskGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with actDev [0, 31]
                            and actDevMask [1, 31].
            Expected: GT_OK.
        */

        /* Call with actDev [0] and actDevMask [1] */
        actDev     = 0;
        actDevMask = 1;

        st = cpssDxChBrgFdbActionActiveDevSet(dev, actDev, actDevMask);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, actDev, actDevMask);

        /*
            1.2. Call cpssDxChBrgFdbActionActiveDevGet
            Expected: GT_OK and the same actDevPtr and actDevMaskPtr.
        */
        st = cpssDxChBrgFdbActionActiveDevGet(dev, &actDevGet, &actDevMaskGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbActionActiveDevGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(actDev, actDevGet,
                       "get another actDevPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(actDevMask, actDevMaskGet,
                       "get another actDevMaskPtr than was set: %d", dev);

        /* Call with actDev [31] and actDevMask [31] */
        actDev     = 31;
        actDevMask = 31;

        st = cpssDxChBrgFdbActionActiveDevSet(dev, actDev, actDevMask);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, actDev, actDevMask);

        /*
            1.2. Call cpssDxChBrgFdbActionActiveDevGet
            Expected: GT_OK and the same actDevPtr and actDevMaskPtr.
        */
        st = cpssDxChBrgFdbActionActiveDevGet(dev, &actDevGet, &actDevMaskGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbActionActiveDevGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(actDev, actDevGet,
                       "get another actDevPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(actDevMask, actDevMaskGet,
                       "get another actDevMaskPtr than was set: %d", dev);

        /*
            1.3. Call function with out of range actDev [32]
                                    and actDevMask [1].
            Expected: NOT GT_OK.
        */
        actDev     = 32;
        actDevMask = 1;

        st = cpssDxChBrgFdbActionActiveDevGet(dev, &actDevGet, &actDevMaskGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, actDev);

        /*
            1.4. Call function with actDev [2] and out of range actDevMask [32].
            Expected: NOT GT_OK.
        */
        actDev     = 2;
        actDevMask = 32;

        st = cpssDxChBrgFdbActionActiveDevGet(dev, &actDevGet, &actDevMaskGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, actDevMask = %d", dev, actDevMask);
    }

    actDev     = 0;
    actDevMask = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbActionActiveDevSet(dev, actDev, actDevMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbActionActiveDevSet(dev, actDev, actDevMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbActionActiveDevGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *actDevPtr,
    OUT GT_U32   *actDevMaskPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbActionActiveDevGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null actDevPtr
                   and non-null actDevMaskPtr.
    Expected: GT_OK.
    1.2. Call with null actDevPtr [NULL]
                   and non-null actDevMaskPtr.
    Expected: GT_BAD_PTR.
    1.3. Call with non-null actDevPtr
                   and null actDevMaskPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st         = GT_OK;

    GT_U8       dev;
    GT_U32      actDev     = 0;
    GT_U32      actDevMask = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null actDevPtr and non-null actDevMaskPtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbActionActiveDevGet(dev, &actDev, &actDevMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null actDevPtr [NULL] and non-null actDevMaskPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbActionActiveDevGet(dev, NULL, &actDevMask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actDevPtr = NULL", dev);

        /*
            1.3. Call with non-null actDevPtr and null actDevMaskPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbActionActiveDevGet(dev, &actDev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actDevMaskPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbActionActiveDevGet(dev, &actDev, &actDevMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbActionActiveDevGet(dev, &actDev, &actDevMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbActionActiveInterfaceSet
(
    IN GT_U8   devNum,
    IN GT_U32  actIsTrunk,
    IN GT_U32  actIsTrunkMask,
    IN GT_U32  actTrunkPort,
    IN GT_U32  actTrunkPortMask
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbActionActiveInterfaceSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with actIsTrunk [1],
                   actIsTrunkMask [1],
                   actTrunkPort[1],
                   actTrunkPortMask[0x7F]
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbActionActiveInterfaceGet
    Expected: GT_OK and the same actIsTrunkPtr,
                                 actIsTrunkMaskPtr,
                                 actTrunkPortPtr
                                 and actTrunkPortMaskPtr.
    1.3. Call with actIsTrunk [0],
                   actIsTrunkMask [0],
                   actTrunkPort[1],
                   actTrunkPortMask[0x7F]
    Expected: GT_OK.
    1.4. Call cpssDxChBrgFdbActionActiveInterfaceGet
    Expected: GT_OK and the same actIsTrunkPtr,
                                 actTrunkPortPtr
                                 and actTrunkPortMaskPtr.
    1.5. Call with actIsTrunk [1],
                   actIsTrunkMask [1],
                   out of range actTrunkPort[MAX_TRUNK_ID_CNS =128],
                   actTrunkPortMask[0x7F]
    Expected: NOT GT_OK.
    1.6. Call with actIsTrunk [1],
                   actIsTrunkMask [1],
                   actTrunkPort[1],
                   out of range actTrunkPortMask[0x8F]
    Expected: NOT GT_OK.
    1.7. Call with actIsTrunk [0],
                   actIsTrunkMask [0],
                   out of range port ID actTrunkPort [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   actTrunkPortMask [0x7F]
    Expected: NOT GT_OK.
    1.8. Call with actIsTrunk [0],
                   actIsTrunkMask [0],
                   actTrunkPort[1],
                   out of range actTrunkPortMask[0x8F]
    Expected: NOT GT_OK.
    1.9. Call with actIsTrunk [0],
                   actIsTrunkMask [0],
                   actTrunkPort [0],
                   actTrunkPortMask[0]
    Expected: GT_OK.
*/
    GT_STATUS   st                  = GT_OK;

    GT_U8       dev;
    GT_U32      actIsTrunk          = 0;
    GT_U32      actIsTrunkMask      = 0;
    GT_U32      actTrunkPort        = 0;
    GT_U32      actTrunkPortMask    = 0;
    GT_U32      actIsTrunkGet       = 0;
    GT_U32      actIsTrunkMaskGet   = 0;
    GT_U32      actTrunkPortGet     = 0;
    GT_U32      actTrunkPortMaskGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with actIsTrunk [1],
                           actIsTrunkMask [1],
                           actTrunkPort[1],
                           actTrunkPortMask[0x7F]
            Expected: GT_OK.
        */
        actIsTrunk       = 1;
        actIsTrunkMask   = 1;
        actTrunkPort     = 1;
        actTrunkPortMask = 0x7F;

        st = cpssDxChBrgFdbActionActiveInterfaceSet(dev, actIsTrunk, actIsTrunkMask,
                                                    actTrunkPort, actTrunkPortMask);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, actIsTrunk, actIsTrunkMask,
                                                    actTrunkPort, actTrunkPortMask);

        /*
            1.2. Call cpssDxChBrgFdbActionActiveInterfaceGet
            Expected: GT_OK and the same actIsTrunkPtr,
                                         actIsTrunkMaskPtr,
                                         actTrunkPortPtr
                                         and actTrunkPortMaskPtr.
        */
        st = cpssDxChBrgFdbActionActiveInterfaceGet(dev, &actIsTrunkGet, &actIsTrunkMaskGet,
                                                    &actTrunkPortGet, &actTrunkPortMaskGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbActionActiveInterfaceGet: %d", dev);

        /* Verifying parameters */
        UTF_VERIFY_EQUAL1_STRING_MAC(actIsTrunk, actIsTrunkGet,
                       "get another actIsTrunk than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(actIsTrunkMask, actIsTrunkMaskGet,
                       "get another actIsTrunkMask than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(actTrunkPort, actTrunkPortGet,
                       "get another actTrunkPort than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(actTrunkPortMask, actTrunkPortMaskGet,
                       "get another actTrunkPortMask than was set: %d", dev);

        /*
            1.3. Call with actIsTrunk [0],
                   actIsTrunkMask [0],
                   actTrunkPort[1],
                   actTrunkPortMask[0x7F]
            Expected: GT_OK.
        */
        actIsTrunk       = 0;
        actIsTrunkMask   = 0;
        actTrunkPort     = 1;
        actTrunkPortMask = 0x7F;

        st = cpssDxChBrgFdbActionActiveInterfaceSet(dev, actIsTrunk, actIsTrunkMask,
                                                    actTrunkPort, actTrunkPortMask);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, actIsTrunk, actIsTrunkMask,
                                                    actTrunkPort, actTrunkPortMask);

        /*
            1.4. Call cpssDxChBrgFdbActionActiveInterfaceGet
            Expected: GT_OK and the same actIsTrunkPtr,
                                         actTrunkPortPtr
                                         and actTrunkPortMaskPtr.
        */
        st = cpssDxChBrgFdbActionActiveInterfaceGet(dev, &actIsTrunkGet, &actIsTrunkMaskGet,
                                                    &actTrunkPortGet, &actTrunkPortMaskGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbActionActiveInterfaceGet: %d", dev);

        /* Verifying parameters */
        UTF_VERIFY_EQUAL1_STRING_MAC(actIsTrunk, actIsTrunkGet,
                       "get another actIsTrunkPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(actIsTrunkMask, actIsTrunkMaskGet,
                       "get another actIsTrunkMaskPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(actTrunkPort, actTrunkPortGet,
                       "get another actTrunkPortPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(actTrunkPortMask, actTrunkPortMaskGet,
                       "get another actTrunkPortMaskPtr than was set: %d", dev);

        /*
            1.5. Call with out of range actTrunkPort[MAX_TRUNK_ID_CNS =128],
                           actIsTrunkMask [1],
                           actIsTrunk [1],
                           actTrunkPortMask[0x7F]
            Expected: NOT GT_OK.
        */
        actTrunkPort       = MAX_TRUNK_ID_CNS;
        actIsTrunkMask      = 1;
        actIsTrunk          = 1;
        actTrunkPortMask    = 0x7F;

        st = cpssDxChBrgFdbActionActiveInterfaceSet(dev, actIsTrunk, actIsTrunkMask,
                                                    actTrunkPort, actTrunkPortMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, actTrunkPort = %d", dev, actIsTrunk);

        /*
            1.6. Call with actIsTrunk [1],
                            actIsTrunkMask [0x1],
                           actTrunkPort[1],
                           out of range actTrunkPortMask[0x8F]
            Expected: NOT GT_OK.
        */
        actIsTrunk       = 1;
        actIsTrunkMask   = 1;
        actTrunkPort     = 1;
        actTrunkPortMask = 0x8F;

        st = cpssDxChBrgFdbActionActiveInterfaceSet(dev, actIsTrunk, actIsTrunkMask,
                                                    actTrunkPort, actTrunkPortMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, actTrunkPortMask = %d", dev, actIsTrunkMask);

        /*
            1.7. Call with actIsTrunk [0],
                   actIsTrunkMask [0],
                   out of range port ID actTrunkPort [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   actTrunkPortMask [0x7F]
            Expected: NOT GT_OK.
        */
        actIsTrunk       = 0;
        actIsTrunkMask   = 0;
        actTrunkPort     = 128;
        actTrunkPortMask = 0x7F;

        st = cpssDxChBrgFdbActionActiveInterfaceSet(dev, actIsTrunk, actIsTrunkMask,
                                                    actTrunkPort, actTrunkPortMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, actTrunkPort = %d", dev, actTrunkPort);

        /*
            1.8. Call with actIsTrunk [0],
                           actIsTrunkMask [0],
                           actTrunkPort[1],
                           out of range actTrunkPortMask[0x8F]
            Expected: NOT GT_OK.
        */
        actIsTrunk       = 0;
        actIsTrunkMask   = 0;
        actTrunkPort     = 1;
        actTrunkPortMask = 0x8F;

        st = cpssDxChBrgFdbActionActiveInterfaceSet(dev, actIsTrunk, actIsTrunkMask,
                                                    actTrunkPort, actTrunkPortMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, actTrunkPortMask = %d", dev, actTrunkPortMask);

        /*
            1.9. Call with actIsTrunk [0],
                           actIsTrunkMask [0],
                           actTrunkPort [0],
                           actTrunkPortMask[0]
            Expected: GT_OK.
        */
        actIsTrunk       = 0;
        actIsTrunkMask   = 0;
        actTrunkPort     = 0;
        actTrunkPortMask = 0;

        st = cpssDxChBrgFdbActionActiveInterfaceSet(dev, actIsTrunk, actIsTrunkMask,
                                                    actTrunkPort, actTrunkPortMask);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, actIsTrunk, actIsTrunkMask,
                                                    actTrunkPort, actTrunkPortMask);
    }

    actIsTrunk       = 1;
    actIsTrunkMask   = 1;
    actTrunkPort     = 1;
    actTrunkPortMask = 0x7F;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbActionActiveInterfaceSet(dev, actIsTrunk, actIsTrunkMask,
                                                    actTrunkPort, actTrunkPortMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbActionActiveInterfaceSet(dev, actIsTrunk, actIsTrunkMask,
                                                    actTrunkPort, actTrunkPortMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbActionActiveInterfaceGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *actIsTrunkPtr,
    OUT GT_U32  *actIsTrunkMaskPtr,
    OUT GT_U32  *actTrunkPortPtr,
    OUT GT_U32  *actTrunkPortMaskPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbActionActiveInterfaceGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null actIsTrunkPtr,
                   non-null actIsTrunkMaskPtr,
                   non-null actTrunkPortPtr,
                   non-null actTrunkPortMaskPtr.
    Expected: GT_OK.
    1.2. Call with null actIsTrunkPtr [NULL],
                   non-null actIsTrunkMaskPtr,
                   non-null actTrunkPortPtr,
                   non-null actTrunkPortMaskPtr.
    Expected: GT_BAD_PTR.
    1.3. Call with non-null actIsTrunkPtr,
                   null actIsTrunkMaskPtr [NULL],
                   non-null actTrunkPortPtr,
                   non-null actTrunkPortMaskPtr.
    Expected: GT_BAD_PTR.
    1.4. Call with non-null actIsTrunkPtr,
                   non-null actIsTrunkMaskPtr,
                   null actTrunkPortPtr [NULL],
                   non-null actTrunkPortMaskPtr.
    Expected: GT_BAD_PTR.
    1.5. Call with non-null actIsTrunkPtr,
                   non-null actIsTrunkMaskPtr,
                   non-null actTrunkPortPtr,
                   null actTrunkPortMaskPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st               = GT_OK;

    GT_U8       dev;
    GT_U32      actIsTrunk       = 0;
    GT_U32      actIsTrunkMask   = 0;
    GT_U32      actTrunkPort     = 0;
    GT_U32      actTrunkPortMask = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null actIsTrunkPtr,
                           non-null actIsTrunkMaskPtr,
                           non-null actTrunkPortPtr,
                           non-null actTrunkPortMaskPtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbActionActiveInterfaceGet(dev, &actIsTrunk, &actIsTrunkMask,
                                                    &actTrunkPort, &actTrunkPortMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null actIsTrunkPtr [NULL],
                           non-null actIsTrunkMaskPtr,
                           non-null actTrunkPortPtr,
                           non-null actTrunkPortMaskPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbActionActiveInterfaceGet(dev, NULL, &actIsTrunkMask,
                                                    &actTrunkPort, &actTrunkPortMask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actIsTrunkPtr = NULL", dev);

        /*
            1.3. Call with non-null actIsTrunkPtr,
                           null actIsTrunkMaskPtr [NULL],
                           non-null actTrunkPortPtr,
                           non-null actTrunkPortMaskPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbActionActiveInterfaceGet(dev, &actIsTrunk, NULL,
                                                    &actTrunkPort, &actTrunkPortMask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actIsTrunkMaskPtr = NULL", dev);

        /*
            1.4. Call with non-null actIsTrunkPtr,
                           non-null actIsTrunkMaskPtr,
                           null actTrunkPortPtr [NULL],
                           non-null actTrunkPortMaskPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbActionActiveInterfaceGet(dev, &actIsTrunk, &actIsTrunkMask,
                                                    NULL, &actTrunkPortMask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actTrunkPortPtr = NULL", dev);

        /*
            1.5. Call with non-null actIsTrunkPtr,
                           non-null actIsTrunkMaskPtr,
                           non-null actTrunkPortPtr,
                           null actTrunkPortMaskPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbActionActiveInterfaceGet(dev, &actIsTrunk, &actIsTrunkMask,
                                                    &actTrunkPort, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actTrunkPortMaskPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbActionActiveInterfaceGet(dev, &actIsTrunk, &actIsTrunkMask,
                                                    &actTrunkPort, &actTrunkPortMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbActionActiveInterfaceGet(dev, &actIsTrunk, &actIsTrunkMask,
                                                    &actTrunkPort, &actTrunkPortMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbUploadEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbUploadEnableSet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbActionsEnableGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgFdbUploadEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbUploadEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbUploadEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbUploadEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgFdbUploadEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbUploadEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbUploadEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbUploadEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbUploadEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbUploadEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbUploadEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbUploadEnableGet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbUploadEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbUploadEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbUploadEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbUploadEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbTrigActionStart
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_ACTION_MODE_ENT     mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbTrigActionStart)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with mode [CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E /
                                  CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E].
    Expected: GT_OK.
    1.2. Call function with out of range mode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;


    CPSS_FDB_ACTION_MODE_ENT mode        = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;
    GT_BOOL                  isCompleted = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E /
                                          CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E].
            Expected: GT_OK.
        */

        /* 1.1. Call with mode [CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E] */
        mode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;

        st = cpssDxChBrgFdbTrigActionStart(dev, mode);
        switch(PRV_CPSS_DXCH_PP_MAC(dev)->portGroupsExtraInfo.fdbMode)
        {
            case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E:
            case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E:
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_STATE, st, dev, mode);
                break;
            default:
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

                /* wait that triggered action is completed */
                while (isCompleted != GT_TRUE)
                {
                    st = cpssDxChBrgFdbTrigActionStatusGet(dev, &isCompleted);
                    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                          "cpssDxChBrgFdbTrigActionStatusGet: %d", dev);
                }

                break;
        }

        /* 1.1. Call with mode [CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E] */
        mode = CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;

        st = cpssDxChBrgFdbTrigActionStart(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /* wait that triggered action is completed */
        isCompleted = GT_FALSE;

        while (isCompleted != GT_TRUE)
        {
            st = cpssDxChBrgFdbTrigActionStatusGet(dev, &isCompleted);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbTrigActionStatusGet: %d", dev);
        }

        /*
            1.2. Call function with wrong enum values mode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbTrigActionStart
                                                        (dev, mode),
                                                        mode);
    }

    mode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbTrigActionStart(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbTrigActionStart(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbActionModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_ACTION_MODE_ENT     mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbActionModeSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with mode [CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E /
                                  CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E /
                                  CPSS_FDB_ACTION_DELETING_E /
                                  CPSS_FDB_ACTION_TRANSPLANTING_E].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbActionModeGet
    Expected: GT_OK and the same modePtr.
    1.3. Call function with out of range mode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                st      = GT_OK;


    GT_U8                    dev;
    CPSS_FDB_ACTION_MODE_ENT mode    = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;
    CPSS_FDB_ACTION_MODE_ENT modeGet = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E /
                                          CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E /
                                          CPSS_FDB_ACTION_DELETING_E /
                                          CPSS_FDB_ACTION_TRANSPLANTING_E].
            Expected: GT_OK.
        */

        /* 1.1. Call with mode [CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E] */
        mode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;

        st = cpssDxChBrgFdbActionModeSet(dev, mode);
        switch(PRV_CPSS_DXCH_PP_MAC(dev)->portGroupsExtraInfo.fdbMode)
        {
            case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E:
            case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E:
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_STATE, st, dev, mode);
                break;
            default:
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

                /*
                    1.2. Call cpssDxChBrgFdbActionModeGet
                    Expected: GT_OK and the same modePtr.
                */
                st = cpssDxChBrgFdbActionModeGet(dev, &modeGet);
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                     "cpssDxChBrgFdbActionModeGet: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                     "get another modePtr than was set: %d", dev);
                break;
        }

        /* 1.1. Call with mode [CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E] */
        mode = CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;

        st = cpssDxChBrgFdbActionModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssDxChBrgFdbActionModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssDxChBrgFdbActionModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbActionModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /* 1.1. Call with mode [CPSS_FDB_ACTION_DELETING_E] */
        mode = CPSS_FDB_ACTION_DELETING_E;

        st = cpssDxChBrgFdbActionModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssDxChBrgFdbActionModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssDxChBrgFdbActionModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbActionModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /* 1.1. Call with mode [CPSS_FDB_ACTION_TRANSPLANTING_E] */
        mode = CPSS_FDB_ACTION_TRANSPLANTING_E;

        st = cpssDxChBrgFdbActionModeSet(dev, mode);
        switch(PRV_CPSS_DXCH_PP_MAC(dev)->portGroupsExtraInfo.fdbMode)
        {
            case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E:
            case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E:
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_STATE, st, dev, mode);
                break;
            default:
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);
        /*
            1.2. Call cpssDxChBrgFdbActionModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssDxChBrgFdbActionModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
             "cpssDxChBrgFdbActionModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
            "get another modePtr than was set: %d", dev);
                break;
        }

        /*
            1.3. Call function with wrong enum values mode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbActionModeSet
                                                        (dev, mode),
                                                        mode);
    }

    mode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbActionModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbActionModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbActionModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_FDB_ACTION_MODE_ENT     *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbActionModeGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null modePtr.
    Expected: GT_OK.
    1.2. Call with null modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                st   = GT_OK;

    GT_U8                    dev;
    CPSS_FDB_ACTION_MODE_ENT mode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null modePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbActionModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbActionModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbActionModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbActionModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbMacTriggerToggle
(
    IN GT_U8  devNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbMacTriggerToggle)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with correct dev.
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_OK;

    GT_U8       dev;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = cpssDxChBrgFdbMacTriggerToggle(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbMacTriggerToggle(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbMacTriggerToggle(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbAuMsgBlockGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfAuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagesPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbAuMsgBlockGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null pointers, numOfAuPtr [2].
    Expected: GT_OK.
    1.2. Call with null numOfAuPtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with null auMessagesPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;

    GT_U8                           dev;
    GT_U32                          numOfAu;
    CPSS_MAC_UPDATE_MSG_EXT_STC     auMessages[2];

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null pointers, numOfAuPtr [2].
            Expected: GT_OK.
        */
        numOfAu = 2;

        st = cpssDxChBrgFdbAuMsgBlockGet(dev, &numOfAu, auMessages);
        st = (GT_NO_MORE == st) ? GT_OK : st;
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, numOfAu);

        /*
            1.2. Call with null numOfAuPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbAuMsgBlockGet(dev, NULL, auMessages);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfAuPtr = NULL", dev);

        /*
            1.3. Call with null auMessagesPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbAuMsgBlockGet(dev, &numOfAu, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, auMessagesPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbAuMsgBlockGet(dev, &numOfAu, auMessages);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbAuMsgBlockGet(dev, &numOfAu, auMessages);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbFuMsgBlockGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfFuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *fuMessagesPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbFuMsgBlockGet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with non-null pointers, numOfAuPtr [2].
    Expected: GT_OK.
    1.2. Call with null numOfAuPtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with null auMessagesPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;

    GT_U8                           dev;
    GT_U32                          numOfFu;
    CPSS_MAC_UPDATE_MSG_EXT_STC     fuMessages[2];

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null pointers, numOfFuPtr [2].
            Expected: GT_OK.
        */
        numOfFu = 2;

        st = cpssDxChBrgFdbFuMsgBlockGet(dev, &numOfFu, fuMessages);
        st = (GT_NO_MORE == st) ? GT_OK : st;
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, numOfFu);

        /*
            1.2. Call with null numOfFuPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbFuMsgBlockGet(dev, NULL, fuMessages);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfFuPtr = NULL", dev);

        /*
            1.3. Call with null fuMessagesPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbFuMsgBlockGet(dev, &numOfFu, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fuMessagesPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbFuMsgBlockGet(dev, &numOfFu, fuMessages);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbFuMsgBlockGet(dev, &numOfFu, fuMessages);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbTrunkAgingModeSet
(
    IN GT_U8                         devNum,
    IN CPSS_FDB_AGE_TRUNK_MODE_ENT  trunkAgingMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbTrunkAgingModeSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with mode [CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E/
                                  CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E].
    Expected: GT_OK.
    1.2. Call function cpssDxChBrgFdbTrunkAgingModeGet with non NULL trunkAgingModePtr.
    Expected: GT_OK and trunkAgingMode the same as was set.
    1.3. Call function with out of range trunkAgingMode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS  st    = GT_OK;


    GT_U8                       dev;
    CPSS_FDB_AGE_TRUNK_MODE_ENT trunkAgingMode;
    CPSS_FDB_AGE_TRUNK_MODE_ENT trunkAgingModeGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E/
                                          CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E].
            Expected: GT_OK.
        */

        /* 1.1. for trunkAgingMode == CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E */
        trunkAgingMode = CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E;

        st = cpssDxChBrgFdbTrunkAgingModeSet(dev, trunkAgingMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkAgingMode);

        /*
            1.2. Call function cpssDxChBrgFdbTrunkAgingModeGet with non NULL trunkAgingModePtr.
            Expected: GT_OK and trunkAgingMode the same as was set.
        */
        /* 1.2. for trunkAgingMode == CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E */
        st = cpssDxChBrgFdbTrunkAgingModeGet(dev, &trunkAgingModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbTrunkAgingModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(trunkAgingMode, trunkAgingModeGet,
                       "get another trunkAgingMode than was set: %d", dev);

        /* 1.1. for trunkAgingMode == CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E */
        trunkAgingMode = CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E;

        st = cpssDxChBrgFdbTrunkAgingModeSet(dev, trunkAgingMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkAgingMode);

        /*
            1.2. Call function cpssDxChBrgFdbTrunkAgingModeGet with non NULL trunkAgingModePtr.
            Expected: GT_OK and trunkAgingMode the same as was set.
        */
        /* 1.2. for trunkAgingMode == CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E */
        st = cpssDxChBrgFdbTrunkAgingModeGet(dev, &trunkAgingModeGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbTrunkAgingModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(trunkAgingMode, trunkAgingModeGet,
                       "get another trunkAgingMode than was set: %d", dev);

        /*
            1.3. Call with wrong enum values trunkAgingMode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbTrunkAgingModeSet
                                                        (dev, trunkAgingMode),
                                                        trunkAgingMode);
    }

    trunkAgingMode = CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbTrunkAgingModeSet(dev, trunkAgingMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbTrunkAgingModeSet(dev, trunkAgingMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbTrunkAgingModeGet
(
    IN GT_U8                        devNum,
    OUT CPSS_FDB_AGE_TRUNK_MODE_ENT *trunkAgingModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbTrunkAgingModeGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with non NULL trunkAgingModePtr.
    Expected: GT_OK.
    1.2. Call function with trunkAgingModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS  st        = GT_OK;

    GT_U8                       dev;
    CPSS_FDB_AGE_TRUNK_MODE_ENT trunkAgingModeGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non NULL trunkAgingModePtr.
        Expected: GT_OK. */

        st = cpssDxChBrgFdbTrunkAgingModeGet(dev, &trunkAgingModeGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with trunkAgingModePtr [NULL].
        Expected: GT_BAD_PTR. */

        st = cpssDxChBrgFdbTrunkAgingModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbTrunkAgingModeGet(dev, &trunkAgingModeGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbTrunkAgingModeGet(dev, &trunkAgingModeGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbAgeBitDaRefreshEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbAgeBitDaRefreshEnableSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbActionsEnableGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgFdbAgeBitDaRefreshEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbAgeBitDaRefreshEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbAgeBitDaRefreshEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbAgeBitDaRefreshEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgFdbAgeBitDaRefreshEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbAgeBitDaRefreshEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbAgeBitDaRefreshEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbAgeBitDaRefreshEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbAgeBitDaRefreshEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbAgeBitDaRefreshEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbAgeBitDaRefreshEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbAgeBitDaRefreshEnableGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbAgeBitDaRefreshEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbAgeBitDaRefreshEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbAgeBitDaRefreshEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbAgeBitDaRefreshEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbAuqFuqMessagesNumberGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_U32                       *numOfAuPtr,
    OUT GT_BOOL                      *endOfQueueReachedPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbAuqFuqMessagesNumberGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with queueType [CPSS_DXCH_FDB_QUEUE_TYPE_AU_E] and non-null pointers.
    Expected: GT_OK.
    1.2. Call with queueType [CPSS_DXCH_FDB_QUEUE_TYPE_FU_E] and  non-null pointers.
    Expected: GT_OK for cheetah2 and above and not GT_OK for cheetah.
    1.3. Call with wrong queueType and non-null pointers.
    Expected: GT_BAD_PARAM.
    1.4. Call with null numOfAuPtr [NULL].
    Expected: GT_BAD_PTR.
    1.5. Call with null endOfQueueReachedPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;


    GT_U8       dev;

    CPSS_DXCH_FDB_QUEUE_TYPE_ENT queueType = CPSS_DXCH_FDB_QUEUE_TYPE_AU_E;
    GT_U32                       numOfAuPtr;
    GT_BOOL                      endOfQueueReachedPtr;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with queueType [CPSS_DXCH_FDB_QUEUE_TYPE_AU_E] and  non-null pointers.
            Expected: GT_OK.
        */
        queueType = CPSS_DXCH_FDB_QUEUE_TYPE_AU_E;

        st = cpssDxChBrgFdbAuqFuqMessagesNumberGet(dev, queueType, &numOfAuPtr, &endOfQueueReachedPtr);
        if(PRV_CPSS_DXCH_PP_MAC(dev)->errata.
            info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.enabled == GT_TRUE)
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_INITIALIZED, st, dev);
        }
        else
        {
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }

        /*
            1.2. Call with queueType [CPSS_DXCH_FDB_QUEUE_TYPE_FU_E] and  non-null pointers.
            Expected: GT_OK for cheetah2 and above and not GT_OK for cheetah.
        */
        queueType = CPSS_DXCH_FDB_QUEUE_TYPE_FU_E;

        st = cpssDxChBrgFdbAuqFuqMessagesNumberGet(dev, queueType, &numOfAuPtr, &endOfQueueReachedPtr);

        if(PRV_CPSS_DXCH_PP_MAC(dev)->errata.
            info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.enabled == GT_TRUE)
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_INITIALIZED, st, dev);
        }
        else
        {
        if(PRV_CPSS_PP_MAC(dev)->devFamily > CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }
        }

        /*
            1.3. Call with wrong enum values queueType and non-null pointers.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbAuqFuqMessagesNumberGet
                                                        (dev, queueType, &numOfAuPtr, &endOfQueueReachedPtr),
                                                        queueType);

        /*
            1.4. Call with null numOfAuPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbAuqFuqMessagesNumberGet(dev, queueType, NULL, &endOfQueueReachedPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);

        /*
            1.5. Call with null endOfQueueReachedPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbAuqFuqMessagesNumberGet(dev, queueType, &numOfAuPtr, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbAuqFuqMessagesNumberGet(dev, queueType,
                                            &numOfAuPtr, &endOfQueueReachedPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbAuqFuqMessagesNumberGet(dev, queueType,
                                            &numOfAuPtr, &endOfQueueReachedPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbRoutedLearningEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbRoutedLearningEnableSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgFdbActionsEnableGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgFdbRoutedLearningEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbRoutedLearningEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbRoutedLearningEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbRoutedLearningEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgFdbRoutedLearningEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgFdbRoutedLearningEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgFdbRoutedLearningEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgFdbRoutedLearningEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbRoutedLearningEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbRoutedLearningEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbRoutedLearningEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbRoutedLearningEnableGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbRoutedLearningEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbRoutedLearningEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbRoutedLearningEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbRoutedLearningEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbInit
(
    IN  GT_U8   devNum,
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbInit)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with correct dev.
    Expected: GT_OK.
*/
    GT_STATUS   st    = GT_OK;
    GT_U8       dev;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with correct dev.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbInit(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbInit(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbInit(dev);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbMacEntryAgeBitSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_BOOL      age
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbMacEntryAgeBitSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with index [0] and age [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call with out of range index.
    Expected: GT_OUT_OF_RANGE.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U32      index = 0;
    GT_BOOL     age = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0] and age [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */
        index = 0;
        age = GT_FALSE;

        st = utfCpssDxChBrgFdbMacEntryAgeBitSet(dev, index, age);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        age = GT_TRUE;

        st = utfCpssDxChBrgFdbMacEntryAgeBitSet(dev, index, age);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);


        index = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.fdb - 1;
        age = GT_FALSE;

        st = utfCpssDxChBrgFdbMacEntryAgeBitSet(dev, index, age);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        age = GT_TRUE;

        st = utfCpssDxChBrgFdbMacEntryAgeBitSet(dev, index, age);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with out of range index.
            Expected: GT_OUT_OF_RANGE.
        */
        index = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.fdb;

        st = utfCpssDxChBrgFdbMacEntryAgeBitSet(dev, index, age);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);

        index = 0;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = utfCpssDxChBrgFdbMacEntryAgeBitSet(dev, index, age);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = utfCpssDxChBrgFdbMacEntryAgeBitSet(dev, index, age);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbMacEntryStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbMacEntryStatusGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with index [0] and non-null pointers.
    Expected: GT_OK.
    1.2. Call with out of range index.
    Expected: GT_OUT_OF_RANGE.
    1.3. Call with wrong validPtr [NULL].
    Expected: GT_BAD_PTR.
    1.4. Call with wrong skipPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U32      index = 0;
    GT_BOOL     valid;
    GT_BOOL     skip;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0] and non-null pointers.
            Expected: GT_OK.
        */
        index = 0;

        st = utfCpssDxChBrgFdbMacEntryStatusGet(dev, index, &valid, &skip);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with out of range index.
            Expected: GT_OUT_OF_RANGE.
        */
        index = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.fdb;

        st = utfCpssDxChBrgFdbMacEntryStatusGet(dev, index, &valid, &skip);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);

        index = 0;

        /*
            1.3. Call with wrong validPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = utfCpssDxChBrgFdbMacEntryStatusGet(dev, index, NULL, &skip);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.4. Call with wrong skipPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = utfCpssDxChBrgFdbMacEntryStatusGet(dev, index, &valid, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = utfCpssDxChBrgFdbMacEntryStatusGet(dev, index, &valid, &skip);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = utfCpssDxChBrgFdbMacEntryStatusGet(dev, index, &valid, &skip);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbQueueFullGet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_BOOL *isFullPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbQueueFullGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with queueType [CPSS_DXCH_FDB_QUEUE_TYPE_AU_E /
                              CPSS_DXCH_FDB_QUEUE_TYPE_FU_E]
                              and non-null isFullPtr.
    Expected: GT_OK.
    1.2. Call with wrong queueType.
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong isFullPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType = CPSS_DXCH_FDB_QUEUE_TYPE_AU_E;
    GT_BOOL     isFull;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with queueType [CPSS_DXCH_FDB_QUEUE_TYPE_AU_E /
                                      CPSS_DXCH_FDB_QUEUE_TYPE_FU_E]
                                      and non-null isFullPtr.
            Expected: GT_OK.
        */
        queueType = CPSS_DXCH_FDB_QUEUE_TYPE_AU_E;

        st = cpssDxChBrgFdbQueueFullGet(dev, queueType, &isFull);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);


        queueType = CPSS_DXCH_FDB_QUEUE_TYPE_FU_E;

        st = cpssDxChBrgFdbQueueFullGet(dev, queueType, &isFull);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong enum values queueType.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbQueueFullGet
                                                        (dev, queueType, &isFull),
                                                        queueType);
        queueType = CPSS_DXCH_FDB_QUEUE_TYPE_FU_E;

        /*
            1.3. Call with wrong isFull [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbQueueFullGet(dev, queueType, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbQueueFullGet(dev, queueType, &isFull);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbQueueFullGet(dev, queueType, &isFull);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbQueueRewindStatusGet
(
    IN  GT_U8                          devNum,
    OUT GT_BOOL                        *rewindPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbQueueRewindStatusGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null rewindPtr.
    Expected: GT_OK.
    1.2. Call with null rewindPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     rewind = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null rewindPtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbQueueRewindStatusGet(dev, &rewind);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null rewindPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbQueueRewindStatusGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbQueueRewindStatusGet(dev, &rewind);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbQueueRewindStatusGet(dev, &rewind);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbSecondaryAuMsgBlockGet
(
    IN      GT_U8                                    devNum,
    INOUT   GT_U32                                   *numOfAuPtr,
    OUT     CPSS_MAC_UPDATE_MSG_EXT_STC              *auMessagesPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbSecondaryAuMsgBlockGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with numOfAuPtr [1] and non-null auMessagesPtr.
    Expected: GT_OK.
    1.2. Call with null numOfAuPtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with null auMessagesPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U32                       numOfAu;
    CPSS_MAC_UPDATE_MSG_EXT_STC  auMessages[1];

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with numOfAuPtr [1] and non-null auMessagesPtr.
            Expected: GT_OK.
        */
        numOfAu = 1;

        st = cpssDxChBrgFdbSecondaryAuMsgBlockGet(dev, &numOfAu, auMessages);
        if (GT_NO_MORE == st)
        {
            continue;
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }

        /*
            1.2. Call with null numOfAuPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbSecondaryAuMsgBlockGet(dev, NULL, auMessages);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);

        /*
            1.3. Call with null auMessagesPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbSecondaryAuMsgBlockGet(dev, &numOfAu, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbSecondaryAuMsgBlockGet(dev, &numOfAu, auMessages);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbSecondaryAuMsgBlockGet(dev, &numOfAu, auMessages);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbTriggerAuQueueWa
(
    IN      GT_U8                                    devNum,
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbTriggerAuQueueWa)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with correct devNum.
    Expected: GT_OK.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with correct devNum.
            Expected: GT_NOT_SUPPORTED - CPSS does not supports the API
                      Only PSS over CPSS supports it.
        */
        st = cpssDxChBrgFdbTriggerAuQueueWa(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_SUPPORTED, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbTriggerAuQueueWa(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbTriggerAuQueueWa(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/

/*
GT_STATUS cpssDxChBrgFdbPortGroupMacEntrySet
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbPortGroupMacEntrySet)
{
    usePortGroupsBmp = GT_TRUE;
    /* call test of cpssDxChBrgFdbMacEntrySet */
    UTF_TEST_CALL_MAC(cpssDxChBrgFdbMacEntrySet);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbPortGroupQaSend
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbPortGroupQaSend)
{
    usePortGroupsBmp = GT_TRUE;
    /* call test of cpssDxChBrgFdbPortGroupQaSend */
    UTF_TEST_CALL_MAC(cpssDxChBrgFdbQaSend);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryDelete
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbPortGroupMacEntryDelete)
{
    usePortGroupsBmp = GT_TRUE;
    /* call test of cpssDxChBrgFdbMacEntryDelete */
    UTF_TEST_CALL_MAC(cpssDxChBrgFdbMacEntryDelete);
}
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN GT_U32                       index,
    IN GT_BOOL                      skip,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbPortGroupMacEntryWrite)
{
    usePortGroupsBmp = GT_TRUE;
    /* call test of cpssDxChBrgFdbMacEntryWrite */
    UTF_TEST_CALL_MAC(cpssDxChBrgFdbMacEntryWrite);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbPortGroupMacEntryStatusGet)
{
    usePortGroupsBmp = GT_TRUE;
    /* call test of cpssDxChBrgFdbMacEntryStatusGet */
    UTF_TEST_CALL_MAC(cpssDxChBrgFdbMacEntryStatusGet);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr,
    OUT GT_BOOL                 *agedPtr,
    OUT GT_U8                   *associatedDevNumPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbPortGroupMacEntryRead)
{
    usePortGroupsBmp = GT_TRUE;
    /* call test of cpssDxChBrgFdbMacEntryRead */
    UTF_TEST_CALL_MAC(cpssDxChBrgFdbMacEntryRead);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryInvalidate
(
    IN GT_U8         devNum,
    IN GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN GT_U32        index
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbPortGroupMacEntryInvalidate)
{
    usePortGroupsBmp = GT_TRUE;
    /* call test of cpssDxChBrgFdbMacEntryInvalidate */
    UTF_TEST_CALL_MAC(cpssDxChBrgFdbMacEntryInvalidate);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet
(
    IN  GT_U8    devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT GT_PORT_GROUPS_BMP  *completedPortGroupsBmpPtr,
    OUT GT_PORT_GROUPS_BMP  *succeededPortGroupsBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxChx)
    1.1. Call with non-null completedPtr and non-null succeededPtr.
    Expected: GT_OK.
    1.2. Call with null completedPtr [NULL] and non-null succeededPtr.
    Expected: GT_BAD_PTR.
    1.3. Call with non-null completedPtr and null succeededPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st        = GT_OK;

    GT_U8       dev;
    GT_PORT_GROUPS_BMP  portGroupsBmp = 1;
    GT_PORT_GROUPS_BMP  completed;
    GT_PORT_GROUPS_BMP  succeeded;
    GT_U32    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* initialize port group */
        portGroupsBmp = 1;

        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /*
                1.1. Call with non-null completedPtr and non-null succeededPtr.
                Expected: GT_OK.
            */

        if( PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E == 
                PRV_CPSS_DXCH_PP_MAC(dev)->portGroupsExtraInfo.fdbMode )
        {
            portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }

        st = cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(dev, portGroupsBmp, &completed, &succeeded);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null completedPtr [NULL] and non-null succeededPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(dev, portGroupsBmp,  NULL, &succeeded);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, completedPtr = NULL", dev);

        /*
            1.3. Call with non-null completedPtr and null succeededPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(dev, portGroupsBmp, &completed, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, completedPtr = NULL", dev);
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* restore port group */
        portGroupsBmp = 1;

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            st = cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(dev,
                    portGroupsBmp, &completed, &succeeded);
            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev)) 
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }

            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
    }

    portGroupsBmp = 1;
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(dev, portGroupsBmp, &completed, &succeeded);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(dev, portGroupsBmp, &completed, &succeeded);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}



/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryAgeBitSet
(
    IN GT_U8        devNum,
    IN GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN GT_U32       index,
    IN GT_BOOL      age
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbPortGroupMacEntryAgeBitSet)
{
    usePortGroupsBmp = GT_TRUE;
    /* call test of cpssDxChBrgFdbMacEntryAgeBitSet */
    UTF_TEST_CALL_MAC(cpssDxChBrgFdbMacEntryAgeBitSet);
}





/*----------------------------------------------------------------------------*/
/*
GT_STATUS prvCpssDxChTestFdbHash
(
    IN GT_U8                        devNum
)
*/
UTF_TEST_CASE_MAC(prvCpssDxChTestFdbHash)
{
    GT_U8                       dev = 0;
    CPSS_MAC_ENTRY_EXT_KEY_STC  macEntryKey;
    GT_STATUS                   st = GT_OK;

    /* set the entry type to be MAC Address */
    macEntryKey.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

    /* test FDB Hash for MAC Address */
    UT_FDB_DUMP(("prvCpssDxChTestFdbHash: start MAC hash check\n"));
    st = prvUtTestFdbHashCases(dev, &macEntryKey);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

    /* set the entry type to be IP Address */
    macEntryKey.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;

    /* test FDB Hash for IP Address */
    UT_FDB_DUMP(("prvCpssDxChTestFdbHash: start IP hash check\n"));
    st = prvUtTestFdbHashCases(dev, &macEntryKey);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function for filling FDB table.
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFillFdbTable)
{
/*
    ITERATE_DEVICE (DxChx)
    1.1. Get table Size.
         Call cpssDxChCfgTableNumEntriesGet with table [CPSS_DXCH_CFG_TABLE_FDB_E]
                                                 and non-NULL numEntriesPtr.
    Expected: GT_OK.
    1.2. Fill all entries in FDB table.
         Call cpssDxChBrgFdbMacEntryWrite with index [0..numEntries-1],
                                               skip [GT_FALSE]
                                               and valid macEntryPtr.
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssDxChBrgFdbMacEntryWrite with index [numEntries],
                                               skip [GT_FALSE]
                                               and valid macEntryPtr.
    Expected: NOT GT_OK.
    1.4. Read all entries in FDB table and compare with original.
         Call cpssDxChBrgFdbMacEntryRead with index [0..numEntries-1],
         and non-NULL validPtr, skipPtr, agedPtr, associatedDevNumPtr, entryPtr.
    Expected: GT_OK and the same entries as was written.
    1.5. Try to read entry with index out of range.
         Call cpssDxChBrgFdbMacEntryRead with index [numEntries],
         and non-NULL validPtr, skipPtr, agedPtr, associatedDevNumPtr, entryPtr.
    Expected: NOT GT_OK.
    1.6. Delete all entries in FDB table.
         Call cpssDxChBrgFdbMacEntryInvalidate with index [0..numEntries-1],
    Expected: GT_OK.
    1.7. Try to delete entry with index out of range.
         Call cpssDxChBrgFdbMacEntryInvalidate with index [numEntries],
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL                 isEqual    = GT_FALSE;
    GT_U32                  numEntries = 0;
    GT_U32                  iTemp      = 0;
    GT_BOOL                 skip       = GT_FALSE;
    CPSS_MAC_ENTRY_EXT_STC  macEntry;

    GT_BOOL                 validGet  = GT_FALSE;
    GT_BOOL                 skipGet   = GT_FALSE;
    GT_BOOL                 agedGet   = GT_FALSE;
    GT_U8                   devNumGet = 0;
    CPSS_MAC_ENTRY_EXT_STC  entryGet;


    cpssOsBzero((GT_VOID*) &macEntry, sizeof(macEntry));
    cpssOsBzero((GT_VOID*) &entryGet, sizeof(entryGet));

    /* Fill the entry for FDB table */
    skip  = GT_FALSE;
    macEntry.key.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    macEntry.key.key.macVlan.vlanId = 100;

    macEntry.key.key.macVlan.macAddr.arEther[0] = 0x0;
    macEntry.key.key.macVlan.macAddr.arEther[1] = 0x1A;
    macEntry.key.key.macVlan.macAddr.arEther[2] = 0xFF;
    macEntry.key.key.macVlan.macAddr.arEther[3] = 0xFF;
    macEntry.key.key.macVlan.macAddr.arEther[4] = 0xFF;
    macEntry.key.key.macVlan.macAddr.arEther[5] = 0xFF;

    macEntry.dstInterface.type            = CPSS_INTERFACE_TRUNK_E;
    macEntry.dstInterface.trunkId         = 10;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
    macEntry.dstInterface.vidx            = 0;
    macEntry.dstInterface.vlanId          = 0;

    macEntry.isStatic                     = GT_FALSE;
    macEntry.daCommand                    = CPSS_MAC_TABLE_FRWRD_E;
    macEntry.saCommand                    = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
    macEntry.daRoute                      = GT_FALSE;
    macEntry.mirrorToRxAnalyzerPortEn     = GT_FALSE;
    macEntry.sourceID                     = 0;
    macEntry.userDefined                  = 0;
    macEntry.daQosIndex                   = 0;
    macEntry.saQosIndex                   = 0;
    macEntry.daSecurityLevel              = 0;
    macEntry.saSecurityLevel              = 0;
    macEntry.appSpecificCpuCode           = GT_FALSE;
    macEntry.age                          = GT_TRUE;
    macEntry.spUnknown                    = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        macEntry.dstInterface.devPort.devNum  = dev;
        macEntry.dstInterface.devPort.portNum = 0;

        /* 1.1. Get table Size */
        st = cpssDxChCfgTableNumEntriesGet(dev, CPSS_DXCH_CFG_TABLE_FDB_E, &numEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChCfgTableNumEntriesGet: %d", dev);

        /* 1.2. Fill all entries in FDB table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            macEntry.dstInterface.trunkId = (GT_TRUNK_ID)(1 + iTemp % 127);
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
            macEntry.userDefined          = iTemp % 0xF;
            macEntry.daQosIndex           = iTemp % 7;
            macEntry.saQosIndex           = iTemp % 7;
            macEntry.daSecurityLevel      = iTemp % 7;
            macEntry.saSecurityLevel      = iTemp % 7;

            st = utfCpssDxChBrgFdbMacEntryWrite(dev, iTemp, skip, &macEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgFdbMacEntryWrite: %d, %d, %d", dev, iTemp, skip);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = utfCpssDxChBrgFdbMacEntryWrite(dev, numEntries, skip, &macEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "cpssDxChBrgFdbMacEntryWrite: %d, %d, %d", dev, numEntries, skip);

        /* 1.4. Read all entries in FDB table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            macEntry.dstInterface.trunkId = (GT_TRUNK_ID)(1 + iTemp % 127);
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);
            macEntry.userDefined          = iTemp % 0xF;
            macEntry.daQosIndex           = iTemp % 7;
            macEntry.saQosIndex           = iTemp % 7;
            macEntry.daSecurityLevel      = iTemp % 7;
            macEntry.saSecurityLevel      = iTemp % 7;

            st = utfCpssDxChBrgFdbMacEntryRead(dev, iTemp, &validGet, &skipGet, &agedGet, &devNumGet, &entryGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChBrgFdbMacEntryRead: %d, %d", dev, iTemp);

            /* Verifying struct fields */
            UTF_VERIFY_EQUAL1_STRING_MAC(skip, skipGet,
                       "get another skipPtr than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.key.entryType, entryGet.key.entryType,
                       "get another macEntryPtr->key.entryType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.key.key.macVlan.vlanId,
                                         entryGet.key.key.macVlan.vlanId,
                       "get another macEntryPtr->key.key.macVlan.vlanId than was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macEntry.key.key.macVlan.macAddr,
                                   (GT_VOID*) &entryGet.key.key.macVlan.macAddr,
                                    sizeof(macEntry.key.key.macVlan.macAddr))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another macEntryPtr->key.key.macVlan.macAddr than was set: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.dstInterface.type, entryGet.dstInterface.type,
                       "get another macEntryPtr->dstInterface.type than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.dstInterface.trunkId,
                                         entryGet.dstInterface.trunkId,
                       "get another macEntryPtr->dstInterface.trunkId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.isStatic, entryGet.isStatic,
                       "get another macEntryPtr->isStatic than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daCommand, entryGet.daCommand,
                       "get another macEntryPtr->daCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.saCommand, entryGet.saCommand,
                       "get another macEntryPtr->saCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daRoute, entryGet.daRoute,
                       "get another macEntryPtr->daRoute than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.mirrorToRxAnalyzerPortEn,
                                         entryGet.mirrorToRxAnalyzerPortEn,
                       "get another macEntryPtr->mirrorToRxAnalyzerPortEn than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.sourceID, entryGet.sourceID,
                       "get another macEntryPtr->sourceID than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.userDefined, entryGet.userDefined,
                       "get another macEntryPtr->userDefined than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daQosIndex, entryGet.daQosIndex,
                       "get another macEntryPtr->daQosIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.saQosIndex, entryGet.saQosIndex,
                       "get another macEntryPtr->saQosIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.daSecurityLevel, entryGet.daSecurityLevel,
                       "get another macEntryPtr->daSecurityLevel than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.saSecurityLevel, entryGet.saSecurityLevel,
                       "get another macEntryPtr->saSecurityLevel than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(macEntry.appSpecificCpuCode, entryGet.appSpecificCpuCode,
                       "get another macEntryPtr->appSpecificCpuCode than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = utfCpssDxChBrgFdbMacEntryRead(dev, numEntries, &validGet, &skipGet, &agedGet, &devNumGet, &entryGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChBrgFdbMacEntryRead: %d, %d", dev, numEntries);

        /* 1.6. Delete all entries in FDB table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = utfCpssDxChBrgFdbMacEntryInvalidate(dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                        "cpssDxChBrgFdbMacEntryInvalidate: %d, %d", dev, iTemp);
        }

        /* 1.7. Try to delete entry with index out of range. */
        st = utfCpssDxChBrgFdbMacEntryInvalidate(dev, numEntries);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgFdbMacEntryInvalidate: %d, %d", dev, numEntries);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbHashCalc
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC *macEntryKeyPtr
    OUT GT_U32                     *hashPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbHashCalc)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call function with valid macEntryKeyPtr{entryType[CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                                 macVlan->vlanId[100],
                                                 macVlan->macAddr[00:1A:FF:FF:FF:FF]}
    Expected: GT_OK.
    1.2. Call function with valid macEntryKeyPtr{entryType[CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E],
                                                 ipMcast->vlanId[100],
                                                 ipMcast->sip[123.23.34.5],
                                                 ipMacst [123.45.67.8]}.
    Expected: GT_OK.
    1.3. Call function with valid macEntryKeyPtr{entryType[CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E],
                                                 ipMcast->vlanId[100],
                                                 ipMcast->sip[123.23.34.5],
                                                 ipMacst [123.45.67.8]}.
    Expected: GT_OK.
    1.4. Call function with null macEntryKeyPtr [NULL].
    Expected: GT_BAD_PTR.
    1.5. Call function with wrong enum values macEntryKeyPtr >entryType
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call function with out of range
         macEntryKeyPtr->key->ipMcast->vlanId[PRV_CPSS_MAX_NUM_VLANS_CNS],
                         entryType [CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E].
    Expected: GT_BAD_PARAM.
    1.7. Call function with out of range
         macEntryKeyPtr->key->macVlan->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                            entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E].
    Expected: GT_BAD_PARAM.
    1.8. Call function with null hashPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                  st = GT_OK;

    GT_U8                      dev;
    CPSS_MAC_ENTRY_EXT_KEY_STC macEntry;
    GT_U32                     hash;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with valid
                 macEntryKeyPtr{entryType[CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                          macVlan->vlanId[100],
                                          macVlan->macAddr[00:1A:FF:FF:FF:FF]}
            Expected: GT_OK.
        */
        macEntry.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        macEntry.key.macVlan.vlanId = 100;

        macEntry.key.macVlan.macAddr.arEther[0] = 0x0;
        macEntry.key.macVlan.macAddr.arEther[1] = 0x1A;
        macEntry.key.macVlan.macAddr.arEther[2] = 0xFF;
        macEntry.key.macVlan.macAddr.arEther[3] = 0xFF;
        macEntry.key.macVlan.macAddr.arEther[4] = 0xFF;
        macEntry.key.macVlan.macAddr.arEther[5] = 0xFF;

        st = cpssDxChBrgFdbHashCalc(dev, &macEntry, &hash);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with valid
                 macEntryKeyPtr{entryType[CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E],
                                ipMcast->vlanId[100],
                                ipMcast->sip[123.23.34.5],
                                ipMacst->dip[123.45.67.8]}.
            Expected: GT_OK.
        */
        macEntry.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
        macEntry.key.ipMcast.vlanId = 100;

        macEntry.key.ipMcast.sip[0] = 123;
        macEntry.key.ipMcast.sip[1] = 23;
        macEntry.key.ipMcast.sip[2] = 34;
        macEntry.key.ipMcast.sip[3] = 5;

        macEntry.key.ipMcast.dip[0] = 123;
        macEntry.key.ipMcast.dip[1] = 45;
        macEntry.key.ipMcast.dip[2] = 67;
        macEntry.key.ipMcast.dip[3] = 8;

        st = cpssDxChBrgFdbHashCalc(dev, &macEntry, &hash);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.3. Call function with valid
                 macEntryKeyPtr{entryType[CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E],
                                ipMcast->vlanId[100],
                                ipMcast->sip[123.23.34.5],
                                ipMacst->dip[123.45.67.8]}.
            Expected: GT_OK.
        */
        macEntry.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;

        st = cpssDxChBrgFdbHashCalc(dev, &macEntry, &hash);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.4. Call function with null macEntryKeyPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbHashCalc(dev, NULL, &hash);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);

        /*
            1.5. Call function with wrong enum values macEntryKeyPtr->entryType
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgFdbHashCalc
                                                        (dev, &macEntry, &hash),
                                                        macEntry.entryType);

        macEntry.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;

        /*
            1.6. Call function with out of range
                 macEntryKeyPtr->key->ipMcast->vlanId[PRV_CPSS_MAX_NUM_VLANS_CNS],
                                    entryType [CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E].
            Expected: GT_BAD_PARAM.
        */
        macEntry.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
        macEntry.key.ipMcast.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgFdbHashCalc(dev, &macEntry, &hash);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, macEntryPtr->key->ipMcast->vlanId = %d",
                                     dev, macEntry.key.ipMcast.vlanId);

        /*
            1.7. Call function with out of range
                 macEntryKeyPtr->key->macVlan->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                                    entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E].
            Expected: GT_BAD_PARAM.
        */
        macEntry.key.ipMcast.vlanId = 100;
        macEntry.key.macVlan.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        macEntry.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

        st = cpssDxChBrgFdbHashCalc(dev, &macEntry, &hash);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, macEntryPtr->key->macVlan->vlanId = %d",
                                     dev, macEntry.key.macVlan.vlanId);

        /*
            1.8. Call function with null hashPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbHashCalc(dev, &macEntry, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    macEntry.entryType          = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    macEntry.key.macVlan.vlanId = 100;

    macEntry.key.macVlan.macAddr.arEther[0] = 0x0;
    macEntry.key.macVlan.macAddr.arEther[1] = 0x1A;
    macEntry.key.macVlan.macAddr.arEther[2] = 0xFF;
    macEntry.key.macVlan.macAddr.arEther[3] = 0xFF;
    macEntry.key.macVlan.macAddr.arEther[4] = 0xFF;
    macEntry.key.macVlan.macAddr.arEther[5] = 0xFF;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbHashCalc(dev, &macEntry, &hash);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    st = cpssDxChBrgFdbHashCalc(dev, &macEntry, &hash);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbNaMsgVid1EnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbNaMsgVid1EnableSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_BOOL     enable = GT_FALSE;
    GT_BOOL     enableGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                     UTF_XCAT_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        st = cpssDxChBrgFdbNaMsgVid1EnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        st = cpssDxChBrgFdbNaMsgVid1EnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        UTF_VERIFY_EQUAL1_PARAM_MAC(enable, enableGet, dev);

        enable = GT_TRUE;

        st = cpssDxChBrgFdbNaMsgVid1EnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        st = cpssDxChBrgFdbNaMsgVid1EnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        UTF_VERIFY_EQUAL1_PARAM_MAC(enable, enableGet, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                     UTF_XCAT_E | UTF_LION_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbNaMsgVid1EnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbNaMsgVid1EnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgFdbNaMsgVid1EnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgFdbNaMsgVid1EnableGet)
{

/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;
    GT_BOOL     enable;
    GT_U8       dev;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                     UTF_XCAT_E | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgFdbNaMsgVid1EnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgFdbNaMsgVid1EnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | 
                                     UTF_XCAT_E | UTF_LION_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgFdbNaMsgVid1EnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgFdbNaMsgVid1EnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChBrgFdb suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChBrgFdb)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbNaToCpuPerPortSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbPortLearnStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbPortLearnStatusSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbNaToCpuPerPortGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbNaStormPreventSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbNaStormPreventGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbDeviceTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbDeviceTableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbMacEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbQaSend)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbMacEntryDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbMacEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbMacEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbMacEntryInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbMaxLookupLenSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbMaxLookupLenGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbMacVlanLookupModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbMacVlanLookupModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbAuMsgRateLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbAuMsgRateLimitGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbNaMsgOnChainTooLongSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbNaMsgOnChainTooLongGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbSpAaMsgToCpuSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbSpAaMsgToCpuGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbAAandTAToCpuSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbAAandTAToCpuGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbHashModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbHashModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbAgingTimeoutSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbAgingTimeoutGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbSecureAgingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbSecureAgingGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbSecureAutoLearnSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbSecureAutoLearnGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbStaticTransEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbStaticTransEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbStaticDelEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbStaticDelEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbActionsEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbActionsEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbTrigActionStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbMacTriggerModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbMacTriggerModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbStaticOfNonExistDevRemove)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbStaticOfNonExistDevRemoveGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbDropAuEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbDropAuEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbActionTransplantDataSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbActionTransplantDataGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbFromCpuAuMsgStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbActionActiveVlanSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbActionActiveVlanGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbActionActiveDevSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbActionActiveDevGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbActionActiveInterfaceSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbActionActiveInterfaceGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbUploadEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbUploadEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbTrigActionStart)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbActionModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbActionModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbMacTriggerToggle)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbAuMsgBlockGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbFuMsgBlockGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbTrunkAgingModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbTrunkAgingModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbAgeBitDaRefreshEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbAgeBitDaRefreshEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbAuqFuqMessagesNumberGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbRoutedLearningEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbRoutedLearningEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbInit)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbMacEntryAgeBitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbMacEntryStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbQueueFullGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbQueueRewindStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbSecondaryAuMsgBlockGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbTriggerAuQueueWa)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbHashCalc)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbNaMsgVid1EnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbNaMsgVid1EnableGet)
    /* portGroup tests */
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbPortGroupMacEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbPortGroupQaSend)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbPortGroupMacEntryDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbPortGroupMacEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbPortGroupMacEntryStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbPortGroupMacEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbPortGroupMacEntryInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFdbPortGroupMacEntryAgeBitSet)

    /* Test filling FDB Table */
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgFillFdbTable)
    /* FDB hash tests */
    UTF_SUIT_DECLARE_TEST_MAC(prvCpssDxChTestFdbHash)
UTF_SUIT_END_TESTS_MAC(cpssDxChBrgFdb)

/*******************************************************************************
* prvUtfCoreClockGet
*
* DESCRIPTION:
*       This routine returns core clock per device.
*
* INPUTS:
*       dev      -  device id
*
* OUTPUTS:
*       coreClockPtr    -  (pointer to) core clock
*
*       GT_OK           -  Get core clock was OK
*       GT_BAD_PARAM    -  Invalid device id
*       GT_BAD_PTR      -  Null pointer
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

    /* check if dev active and from DxChx family */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    *coreClockPtr = PRV_CPSS_PP_MAC(dev)->coreClock;

    return(GT_OK);
}

/*******************************************************************************
* prvUtfMacTableSize
*
* DESCRIPTION:
*       This routine returns mac table size per device.
*
* INPUTS:
*       dev      -  device id
*
* OUTPUTS:
*       tableSizePtr    - (pointer to) mac table size
*
*       GT_OK           - Get core clock was OK
*       GT_BAD_PARAM    - Invalid device id
*       GT_BAD_PTR      - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfMacTableSize
(
    IN  GT_U8       dev,
    OUT GT_U32      *tableSizePtr
)
{
    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(tableSizePtr);

    /* check if dev active and from DxChx family */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    *tableSizePtr = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.fdb;

    return(GT_OK);
}

/*******************************************************************************
* prvUtTestFdbHashCases
*
* DESCRIPTION:
*       This routine test all cases of CPSS FDB Hash function, according
*       to entry type.
*
* INPUTS:
*       dev      -  device id
*       macKeyPtr   - pointer to mac entry key (entry type)
*
* OUTPUTS:
*       GT_OK           - the test was ok
*       GT_BAD_PARAM    - Invalid device id
*       GT_BAD_PTR      - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtTestFdbHashCases
(
    IN GT_U8 dev,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC *macKeyPtr
)
{
    GT_STATUS   st;

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(macKeyPtr);

    /* check if dev active and from DxChx family */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    /* set vlan mode to SVL */
    st = cpssDxChBrgFdbMacVlanLookupModeSet(dev, CPSS_SVL_E);
    if (st != GT_OK)
    {
        return st;
    }

    /* set hash mode to CRC */
    st = cpssDxChBrgFdbHashModeSet(dev, CPSS_MAC_HASH_FUNC_CRC_E);
    if (st != GT_OK)
    {
        return st;
    }

    /* test FDB Hash for SVL, CRC */
    UT_FDB_DUMP(("prvUtTestFdbHashCases: start SVL CRC check\n"));
    st = prvUtTestFdbHash(dev, macKeyPtr);
    if (st != GT_OK)
    {
        return st;
    }

    /* set vlan mode to IVL */
    st = cpssDxChBrgFdbMacVlanLookupModeSet(dev, CPSS_IVL_E);
    if (st != GT_OK)
    {
        return st;
    }

    /* test FDB Hash for IVL, CRC */
    UT_FDB_DUMP(("prvUtTestFdbHashCases: start IVL CRC check\n"));
    st = prvUtTestFdbHash(dev, macKeyPtr);
    if (st != GT_OK)
    {
        return st;
    }

    /* set hash mode to XOR */
    st = cpssDxChBrgFdbHashModeSet(dev, CPSS_MAC_HASH_FUNC_XOR_E);
    if (st != GT_OK)
    {
        return st;
    }

    /* test FDB Hash for IVL, XOR */
    UT_FDB_DUMP(("prvUtTestFdbHashCases: start IVL XOR check\n"));
    st = prvUtTestFdbHash(dev, macKeyPtr);
    if (st != GT_OK)
    {
        return st;
    }

    /* set vlan mode to SVL */
    st = cpssDxChBrgFdbMacVlanLookupModeSet(dev, CPSS_SVL_E);
    if (st != GT_OK)
    {
        return st;
    }

    /* test FDB Hash for SVL, XOR */
    UT_FDB_DUMP(("prvUtTestFdbHashCases: start SVL XOR check\n"));
    st = prvUtTestFdbHash(dev, macKeyPtr);
    if (st != GT_OK)
    {
        return st;
    }

    return GT_OK;
}

/*******************************************************************************
* prvUtTestFdbHash
*
* DESCRIPTION:
*       This routine test CPSS FDB Hash function, according to entry type.
*
* INPUTS:
*       dev      -  device id
*       macKeyPtr   - pointer to mac entry key (entry type)
*
* OUTPUTS:
*       GT_OK           - the test was ok
*       GT_BAD_PARAM    - Invalid device id
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtTestFdbHash
(
    IN GT_U8 dev,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC *macKeyPtr
)
{
    GT_U32                      hashIndex;
    GT_BOOL                     valid;
    GT_BOOL                     skip;
    GT_BOOL                     aged;
    GT_U8                       associatedDevNum;
    CPSS_MAC_ENTRY_EXT_STC      entry;
    GT_U32                      bitArray[3];
    GT_U32                      deltaWord0 = 0x10624d;
    GT_U32                      deltaWord1 = 0x10624;
    GT_U32                      deltaWord2 = 0x1;
    GT_U32                      i;
    GT_STATUS   st;
    GT_U32                      tableLength = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.fdb;

    /* initialize bit array */
    bitArray[0] = 0;
    bitArray[1] = 0;
    bitArray[2] = 0;

#ifdef ASIC_SIMULATION
    /* indicate start of function */
    cpssOsPrintf("prvUtTestFdbHash: start \n");
#endif /*ASIC_SIMULATION*/

    /* initialize mac entry */
    cpssOsMemSet(&entry,0,sizeof(CPSS_MAC_ENTRY_EXT_STC));

    /* build MAC Address from bit array */
    for (i = 0; i < tableLength; i++)
    {
#ifdef ASIC_SIMULATION
        if((i & 0x3FF) < 16)
        {
            /* show that the function alive */
            cpssOsPrintf(".");
        }
#endif /*ASIC_SIMULATION*/


        prvUtBuildMacAddrKeyFromBitArray(bitArray, macKeyPtr);

        /* calculate the entry hash index by CPSS hash algorithm */
        st = cpssDxChBrgFdbHashCalc(dev, macKeyPtr, &hashIndex);
        if (st != GT_OK)
        {
            return st;
        }

        entry.key = *macKeyPtr;

        /* Set destination interface type to be VID */
        entry.dstInterface.type = CPSS_INTERFACE_VID_E;

        /* add the entry to FDB table */
        st = utfCpssDxChBrgFdbMacEntrySet(dev, &entry);
        if (st != GT_OK)
        {
            return st;
        }

        /* wait for acknowledge that the set was done */
        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(dev,GT_TRUE);

        /* Read the entry by previously calculated hash index */
        st = utfCpssDxChBrgFdbMacEntryRead(dev, hashIndex, &valid, &skip, &aged,
                                        &associatedDevNum, &entry);
        if (st != GT_OK)
        {
            return st;
        }

        /* check if the entry exists, if yes check that there is match between the keys */
        st = prvUtCheckMacEntry(valid, skip, &entry, macKeyPtr);
        if (st != GT_OK)
        {
            UT_FDB_DUMP(("prvUtCheckMacEntry: check fail i %d hashIndex %d \n",
                         i, hashIndex));
            return st;
        }

                /* remove the entry from FDB */
        st = utfCpssDxChBrgFdbMacEntryInvalidate(dev, hashIndex);
        if (st != GT_OK)
        {
            return st;
        }

        /* increase bit array */
        bitArray[0] += deltaWord0;
        bitArray[1] += deltaWord1;
        bitArray[2] += deltaWord2;


        #ifdef ASIC_SIMULATION
        /* test is too long without this skipping */
            i += 16;
        #endif /*ASIC_SIMULATION*/
    }

#ifdef ASIC_SIMULATION
    /* indicate end of function */
    cpssOsPrintf("prvUtTestFdbHash: end \n");
#endif /*_WIN32*/
    return GT_OK;
}

/*******************************************************************************
* prvUtBuildMacAddrKeyFromBitArray
*
* DESCRIPTION:
*       This routine build MAC entry key from bit array, according to entry type.
*
* INPUTS:
*       dev      -  device id
*       bitArray - bit array
*       macKeyPtr   - mac entry key (entry type)
*
* OUTPUTS:
*       macKeyPtr   - mac entry key
*
* COMMENTS:
*       None.
*******************************************************************************/
static void prvUtBuildMacAddrKeyFromBitArray
(
    IN  GT_U32                          bitArray[3],
    INOUT CPSS_MAC_ENTRY_EXT_KEY_STC    *macKeyPtr
)
{
    if (macKeyPtr->entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
    {
        /* build MAC Address */
        macKeyPtr->key.macVlan.macAddr.arEther[0] = (GT_U8)(bitArray[0] & 0xFF);
        macKeyPtr->key.macVlan.macAddr.arEther[1] = (GT_U8)((bitArray[0] >> 8) & 0xFF);
        macKeyPtr->key.macVlan.macAddr.arEther[2] = (GT_U8)((bitArray[0] >> 16) & 0xFF);
        macKeyPtr->key.macVlan.macAddr.arEther[3] = (GT_U8)((bitArray[0] >> 24) & 0xFF);
        macKeyPtr->key.macVlan.macAddr.arEther[4] = (GT_U8)(bitArray[1] & 0xFF);
        macKeyPtr->key.macVlan.macAddr.arEther[5] = (GT_U8)((bitArray[1] >> 8) & 0xFF);
        /* Vlan ID */
        macKeyPtr->key.macVlan.vlanId = (GT_U16)((bitArray[1] >> 16) & 0xFFF);
    }
    else
    {
        /* build IP Address*/
        /* Source IP */
        macKeyPtr->key.ipMcast.sip[0] = (GT_U8)(bitArray[0] & 0xFF);
        macKeyPtr->key.ipMcast.sip[1] = (GT_U8)((bitArray[0] >> 8) & 0xFF);
        macKeyPtr->key.ipMcast.sip[2] = (GT_U8)((bitArray[0] >> 16) & 0xFF);
        macKeyPtr->key.ipMcast.sip[3] = (GT_U8)((bitArray[0] >> 24) & 0xFF);
        /* Destination IP */
        macKeyPtr->key.ipMcast.dip[0] = (GT_U8)(bitArray[1] & 0xFF);
        macKeyPtr->key.ipMcast.dip[1] = (GT_U8)((bitArray[1] >> 8) & 0xFF);
        macKeyPtr->key.ipMcast.dip[2] = (GT_U8)((bitArray[1] >> 16) & 0xFF);
        macKeyPtr->key.ipMcast.dip[3] = (GT_U8)((bitArray[1] >> 24) & 0xFF);
        /* Vlan ID */
        macKeyPtr->key.ipMcast.vlanId = (GT_U16)(bitArray[2] & 0xFFF);
    }
}

/*******************************************************************************
* prvUtCheckMacEntry
*
* DESCRIPTION:
*       This routine checks if the entry exists in FDB in the index created
*       by CPSS, if yes check that there a match between entries keys.
*
* INPUTS:
*       valid       -  entry validity
*       skip        -  entry skip bit
*       entry       - mac entry
*       macKeyPtr   - mac entry key
*
* OUTPUTS:
*       macKeyPtr   - mac entry key
*
*       GT_OK           - the test was ok
*       GT_FAIL         - on fail
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtCheckMacEntry
(
    IN GT_BOOL                      valid,
    IN GT_BOOL                      skip,
    IN CPSS_MAC_ENTRY_EXT_STC      *entryPtr,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC  *macKeyPtr
)
{

    if (valid == GT_FALSE)
    {
        return GT_FAIL;
    }

    if (skip == GT_TRUE)
    {
        return GT_FAIL;
    }

    if (macKeyPtr->entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
    {
        /* compare MAC Address entries */
        if ((entryPtr->key.key.macVlan.macAddr.arEther[0] !=
            macKeyPtr->key.macVlan.macAddr.arEther[0]) ||
            (entryPtr->key.key.macVlan.macAddr.arEther[1] !=
            macKeyPtr->key.macVlan.macAddr.arEther[1]) ||
            (entryPtr->key.key.macVlan.macAddr.arEther[2] !=
            macKeyPtr->key.macVlan.macAddr.arEther[2]) ||
            (entryPtr->key.key.macVlan.macAddr.arEther[3] !=
            macKeyPtr->key.macVlan.macAddr.arEther[3]) ||
            (entryPtr->key.key.macVlan.macAddr.arEther[4] !=
            macKeyPtr->key.macVlan.macAddr.arEther[4]) ||
            (entryPtr->key.key.macVlan.macAddr.arEther[5] !=
            macKeyPtr->key.macVlan.macAddr.arEther[5]) ||
            (entryPtr->key.key.macVlan.vlanId !=
             macKeyPtr->key.macVlan.vlanId))
        {
            return GT_FAIL;
        }
    }
    else
    {
        /* compare MAC Address entries */
        if ((entryPtr->key.key.ipMcast.sip[0] != macKeyPtr->key.ipMcast.sip[0]) ||
            (entryPtr->key.key.ipMcast.sip[1] != macKeyPtr->key.ipMcast.sip[1]) ||
            (entryPtr->key.key.ipMcast.sip[2] != macKeyPtr->key.ipMcast.sip[2]) ||
            (entryPtr->key.key.ipMcast.sip[3] != macKeyPtr->key.ipMcast.sip[3]) ||
            (entryPtr->key.key.ipMcast.dip[0] != macKeyPtr->key.ipMcast.dip[0]) ||
            (entryPtr->key.key.ipMcast.dip[1] != macKeyPtr->key.ipMcast.dip[1]) ||
            (entryPtr->key.key.ipMcast.dip[2] != macKeyPtr->key.ipMcast.dip[2]) ||
            (entryPtr->key.key.ipMcast.dip[3] != macKeyPtr->key.ipMcast.dip[3]) ||
            (entryPtr->key.key.ipMcast.vlanId != macKeyPtr->key.ipMcast.vlanId))
        {
            return GT_FAIL;
        }
    }

    return GT_OK;
}
