/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmFabricHGLinkUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmFabricHGLink, that provides
*       CPSS ExMxPm Fabric Connectivity HyperG.Link interface API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricHGLink.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define FABRIC_HGLINK_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricHGLinkPcsConfigSet
(
    IN  GT_U8                                       devNum,
    IN  GT_FPORT                                    fport,
    IN  CPSS_EXMXPM_FABRIC_HGLINK_PCS_CONFIG_STC    *pcsConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricHGLinkPcsConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3]
                   and pcsConfigPtr {loopbackEnable [GT_FALSE / GT_TRUE],
                                     repeaterModeEnable [GT_FALSE / GT_TRUE],
                                     maxIdleCnt [0 / 0xFFFF]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricHGLinkPcsConfigGet with the same fport
                                                      and non-NULL pcsConfigPtr.
    Expected: GT_OK and the same pcsConfigPtr.
    1.4. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range pcsConfigPtr->maxIdleCnt [65536]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with pcsConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                    fport   = 0;
    CPSS_EXMXPM_FABRIC_HGLINK_PCS_CONFIG_STC    pcsConfig;
    CPSS_EXMXPM_FABRIC_HGLINK_PCS_CONFIG_STC    pcsConfigGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3]
                           and pcsConfigPtr {loopbackEnable [GT_FALSE / GT_TRUE],
                                             repeaterModeEnable [GT_FALSE / GT_TRUE],
                                             maxIdleCnt [0 / 0xFFFF]}.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        pcsConfig.loopbackEnable     = GT_FALSE;
        pcsConfig.repeaterModeEnable = GT_FALSE;
        pcsConfig.maxIdleCnt      = 0;

        st = cpssExMxPmFabricHGLinkPcsConfigSet(dev, fport, &pcsConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricHGLinkPcsConfigGet with the same fport
                                                              and non-NULL pcsConfigPtr.
            Expected: GT_OK and the same pcsConfigPtr.
        */
        st = cpssExMxPmFabricHGLinkPcsConfigGet(dev, (GT_U8)fport, &pcsConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricHGLinkPcsConfigGet: %d, %d", dev, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(pcsConfig.loopbackEnable, pcsConfigGet.loopbackEnable,
                   "get another pcsConfigPtr->loopbackEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pcsConfig.repeaterModeEnable, pcsConfigGet.repeaterModeEnable,
                   "get another pcsConfigPtr->repeaterModeEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pcsConfig.maxIdleCnt, pcsConfigGet.maxIdleCnt,
                   "get another pcsConfigPtr->maxIdleCnt than was set: %d", dev);

        /* Call with fport [3] */
        fport   = 3;

        pcsConfig.loopbackEnable     = GT_TRUE;
        pcsConfig.repeaterModeEnable = GT_TRUE;
        pcsConfig.maxIdleCnt      = 0xFFFF;

        st = cpssExMxPmFabricHGLinkPcsConfigSet(dev, fport, &pcsConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricHGLinkPcsConfigGet with the same fport
                                                              and non-NULL pcsConfigPtr.
            Expected: GT_OK and the same pcsConfigPtr.
        */
        st = cpssExMxPmFabricHGLinkPcsConfigGet(dev, (GT_U8)fport, &pcsConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricHGLinkPcsConfigGet: %d, %d", dev, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(pcsConfig.loopbackEnable, pcsConfigGet.loopbackEnable,
                   "get another pcsConfigPtr->loopbackEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pcsConfig.repeaterModeEnable, pcsConfigGet.repeaterModeEnable,
                   "get another pcsConfigPtr->repeaterModeEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pcsConfig.maxIdleCnt, pcsConfigGet.maxIdleCnt,
                   "get another pcsConfigPtr->maxIdleCnt than was set: %d", dev);

        /*
            1.4. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmFabricHGLinkPcsConfigSet(dev, fport, &pcsConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.5. Call with out of range pcsConfigPtr->maxIdleCnt [65536]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        pcsConfig.maxIdleCnt = 65536;

        st = cpssExMxPmFabricHGLinkPcsConfigSet(dev, fport, &pcsConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pcsConfigPtr->maxIdleCnt = %d",
                                         dev, pcsConfig.maxIdleCnt);

        pcsConfig.maxIdleCnt = 0;

        /*
            1.6. Call with pcsConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricHGLinkPcsConfigSet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pcsConfigPtr = NULL", dev);
    }

    fport   = 0;

    pcsConfig.loopbackEnable     = GT_FALSE;
    pcsConfig.repeaterModeEnable = GT_FALSE;
    pcsConfig.maxIdleCnt      = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricHGLinkPcsConfigSet(dev, fport, &pcsConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricHGLinkPcsConfigSet(dev, fport, &pcsConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricHGLinkPcsConfigGet
(
    IN  GT_U8                                       devNum,
    IN  GT_U8                                       fport,
    OUT CPSS_EXMXPM_FABRIC_HGLINK_PCS_CONFIG_STC   *pcsConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricHGLinkPcsConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3]
                   and non-NULL pcsConfigPtr.
    Expected: GT_OK.
    1.3. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with pcsConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                    fport   = 0;
    CPSS_EXMXPM_FABRIC_HGLINK_PCS_CONFIG_STC    pcsConfig;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3]
                           and non-NULL pcsConfigPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        st = cpssExMxPmFabricHGLinkPcsConfigGet(dev, (GT_U8)fport, &pcsConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [4] */
        fport   = 3;

        st = cpssExMxPmFabricHGLinkPcsConfigGet(dev, (GT_U8)fport, &pcsConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.3. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmFabricHGLinkPcsConfigGet(dev, (GT_U8)fport, &pcsConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with pcsConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricHGLinkPcsConfigGet(dev, (GT_U8)fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pcsConfigPtr = NULL", dev);
    }

    fport   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricHGLinkPcsConfigGet(dev, (GT_U8)fport, &pcsConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricHGLinkPcsConfigGet(dev, (GT_U8)fport, &pcsConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricHGLinkPcsRxStatusGet
(
    IN  GT_U8                                         devNum,
    IN  GT_FPORT                                      fport,
    OUT CPSS_EXMXPM_FABRIC_HGLINK_PCS_RX_STATUS_STC   *statusPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricHGLinkPcsRxStatusGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3]
                   and non-NULL statusPtr.
    Expected: GT_OK.
    1.3. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with statusPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                    fport   = 0;
    CPSS_EXMXPM_FABRIC_HGLINK_PCS_RX_STATUS_STC status;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3]
                           and non-NULL pcsConfigPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        st = cpssExMxPmFabricHGLinkPcsRxStatusGet(dev, fport, &status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [1] */
        fport   = 3;

        st = cpssExMxPmFabricHGLinkPcsRxStatusGet(dev, fport, &status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.3. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmFabricHGLinkPcsRxStatusGet(dev, fport, &status);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with pcsConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricHGLinkPcsRxStatusGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pcsConfigPtr = NULL", dev);
    }

    fport   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricHGLinkPcsRxStatusGet(dev, fport, &status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricHGLinkPcsRxStatusGet(dev, fport, &status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricHGLinkMacConfigSet
(
    IN  GT_U8                                       devNum,
    IN  GT_FPORT                                    fport,
    IN  CPSS_EXMXPM_FABRIC_HGLINK_MAC_CONFIG_STC    *macConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricHGLinkMacConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3]
                   and macConfigPtr {fcDeadLockLimit [0 / 0xFFFF],
                                     serdesMode [CPSS_EXMXPM_FABRIC_HGLINK_SERDES4_E /
                                                 CPSS_EXMXPM_FABRIC_HGLINK_SERDES6_E]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricHGLinkMacConfigGet with the same fport
                                                      and non-NULL macConfigPtr.
    Expected: GT_OK and the same macConfigPtr.
    1.4. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range macConfigPtr->fcDeadLockLimit [65536]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range macConfigPtr->serdesMode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with macConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                    fport   = 0;
    CPSS_EXMXPM_FABRIC_HGLINK_MAC_CONFIG_STC    macConfig;
    CPSS_EXMXPM_FABRIC_HGLINK_MAC_CONFIG_STC    macConfigGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3]
                           and macConfigPtr {fcDeadLockLimit [0 / 0xFFFF],
                                             serdesMode [CPSS_EXMXPM_FABRIC_HGLINK_SERDES4_E /
                                                         CPSS_EXMXPM_FABRIC_HGLINK_SERDES6_E]}.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        macConfig.fcDeadLockLimit = 0;
        macConfig.serdesMode      = CPSS_EXMXPM_FABRIC_HGLINK_SERDES4_E;

        st = cpssExMxPmFabricHGLinkMacConfigSet(dev, fport, &macConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricHGLinkMacConfigGet with the same fport
                                                              and non-NULL macConfigPtr.
            Expected: GT_OK and the same macConfigPtr.
        */
        st = cpssExMxPmFabricHGLinkMacConfigGet(dev, fport, &macConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricHGLinkMacConfigGet: %d, %d", dev, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(macConfig.fcDeadLockLimit, macConfigGet.fcDeadLockLimit,
                   "get another macConfigPtr->fcDeadLockLimit than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macConfig.serdesMode, macConfigGet.serdesMode,
                   "get another macConfigPtr->serdesMode than was set: %d", dev);

        /* Call with fport [1] */
        fport   = 3;

        macConfig.fcDeadLockLimit = 0xFFFF;
        macConfig.serdesMode      = CPSS_EXMXPM_FABRIC_HGLINK_SERDES6_E;

        st = cpssExMxPmFabricHGLinkMacConfigSet(dev, fport, &macConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricHGLinkMacConfigGet with the same fport
                                                              and non-NULL macConfigPtr.
            Expected: GT_OK and the same macConfigPtr.
        */
        st = cpssExMxPmFabricHGLinkMacConfigGet(dev, fport, &macConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricHGLinkMacConfigGet: %d, %d", dev, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(macConfig.fcDeadLockLimit, macConfigGet.fcDeadLockLimit,
                   "get another macConfigPtr->fcDeadLockLimit than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(macConfig.serdesMode, macConfigGet.serdesMode,
                   "get another macConfigPtr->serdesMode than was set: %d", dev);

        /*
            1.4. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmFabricHGLinkMacConfigSet(dev, fport, &macConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.5. Call with out of range macConfigPtr->fcDeadLockLimit [65536]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        macConfig.fcDeadLockLimit = 65536;

        st = cpssExMxPmFabricHGLinkMacConfigSet(dev, fport, &macConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, macConfigPtr->fcDeadLockLimit = %d",
                                         dev, macConfig.fcDeadLockLimit);

        macConfig.fcDeadLockLimit = 0;

        /*
            1.6. Call with out of range macConfigPtr->serdesMode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        macConfig.serdesMode = FABRIC_HGLINK_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricHGLinkMacConfigSet(dev, fport, &macConfig);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, macConfigPtr->serdesMode = %d",
                                     dev, macConfig.serdesMode);

        macConfig.serdesMode = CPSS_EXMXPM_FABRIC_HGLINK_SERDES4_E;

        /*
            1.7. Call with macConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricHGLinkMacConfigSet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macConfigPtr = NULL", dev);
    }

    fport   = 0;

    macConfig.fcDeadLockLimit = 0;
    macConfig.serdesMode      = CPSS_EXMXPM_FABRIC_HGLINK_SERDES4_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricHGLinkMacConfigSet(dev, fport, &macConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricHGLinkMacConfigSet(dev, fport, &macConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricHGLinkMacConfigGet
(
    IN  GT_U8                                       devNum,
    IN  GT_FPORT                                    fport,
    OUT CPSS_EXMXPM_FABRIC_HGLINK_MAC_CONFIG_STC    *macConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricHGLinkMacConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3]
                   and non-NULL macConfigPtr.
    Expected: GT_OK.
    1.3. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with macConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                    fport   = 0;
    CPSS_EXMXPM_FABRIC_HGLINK_MAC_CONFIG_STC    macConfig;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3]
                           and non-NULL macConfigPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        st = cpssExMxPmFabricHGLinkMacConfigGet(dev, fport, &macConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [3] */
        fport   = 3;

        st = cpssExMxPmFabricHGLinkMacConfigGet(dev, fport, &macConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.3. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmFabricHGLinkMacConfigGet(dev, fport, &macConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with macConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricHGLinkMacConfigGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macConfigPtr = NULL", dev);
    }

    fport   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricHGLinkMacConfigGet(dev, fport, &macConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricHGLinkMacConfigGet(dev, fport, &macConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricHGLinkFlowControlEnableSet
(
    IN  GT_U8                                       devNum,
    IN  GT_FPORT                                    fport,
    IN  CPSS_PORTS_BMP_STC                          *enableBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricHGLinkFlowControlEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3]
                   and enableBmpPtr->ports [0, 0 / 0xFFFFFF, 0].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricHGLinkFlowControlEnableGet with the same fport
                                                              and non-NULL enableBmpPtr.
    Expected: GT_OK and the same enableBmpPtr.
    1.3. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with enableBmpPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT            fport   = 0;
    CPSS_PORTS_BMP_STC  enableBmp;
    CPSS_PORTS_BMP_STC  enableBmpGet;
    GT_BOOL             isEqual = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3]
                           and enableBmpPtr->ports [0, 0 / 0xFFFFFF, 0].
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        enableBmp.ports[0] = 0;
        enableBmp.ports[1] = 0;

        st = cpssExMxPmFabricHGLinkFlowControlEnableSet(dev, fport, &enableBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricHGLinkFlowControlEnableGet with the same fport
                                                                      and non-NULL enableBmpPtr.
            Expected: GT_OK and the same enableBmpPtr.
        */
        st = cpssExMxPmFabricHGLinkFlowControlEnableGet(dev, fport, &enableBmpGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricHGLinkFlowControlEnableGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &enableBmp,
                                     (GT_VOID*) &enableBmpGet,
                                     sizeof(enableBmp))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another enableBmp than was set: %d", dev);

        /* Call with fport [3] */
        fport = 3;

        enableBmp.ports[0] = 0xFFFFFF;
        enableBmp.ports[1] = 0;

        st = cpssExMxPmFabricHGLinkFlowControlEnableSet(dev, fport, &enableBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricHGLinkFlowControlEnableGet with the same fport
                                                                      and non-NULL enableBmpPtr.
            Expected: GT_OK and the same enableBmpPtr.
        */
        st = cpssExMxPmFabricHGLinkFlowControlEnableGet(dev, fport, &enableBmpGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricHGLinkFlowControlEnableGet: %d, %d", dev, fport);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &enableBmp,
                                     (GT_VOID*) &enableBmpGet,
                                     sizeof(enableBmp))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another enableBmp than was set: %d", dev);

        /*
            1.3. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmFabricHGLinkFlowControlEnableSet(dev, fport, &enableBmp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.5. Call with enableBmpPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricHGLinkFlowControlEnableSet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enableBmpPtr = NULL", dev);
    }

    fport   = 0;

    enableBmp.ports[0] = 0;
    enableBmp.ports[1] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricHGLinkFlowControlEnableSet(dev, fport, &enableBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricHGLinkFlowControlEnableSet(dev, fport, &enableBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricHGLinkFlowControlEnableGet
(
    IN  GT_U8                                       devNum,
    IN  GT_FPORT                                    fport,
    OUT CPSS_PORTS_BMP_STC                         *enableBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricHGLinkFlowControlEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3]
                   and non-NULL enableBmpPtr.
    Expected: GT_OK.
    1.2. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with enableBmpPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT            fport   = 0;
    CPSS_PORTS_BMP_STC  enableBmp;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0]
                           and non-NULL enableBmpPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        st = cpssExMxPmFabricHGLinkFlowControlEnableGet(dev, fport, &enableBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [3] */
        fport = 3;

        st = cpssExMxPmFabricHGLinkFlowControlEnableGet(dev, fport, &enableBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.3. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmFabricHGLinkFlowControlEnableGet(dev, fport, &enableBmp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with enableBmpPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricHGLinkFlowControlEnableGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enableBmpPtr = NULL", dev);
    }

    fport   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricHGLinkFlowControlEnableGet(dev, fport, &enableBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricHGLinkFlowControlEnableGet(dev, fport, &enableBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricHGLinkFlowControlStatusGet
(
    IN  GT_U8                                       devNum,
    IN  GT_FPORT                                    fport,
    IN  GT_U32                                      tc,
    OUT CPSS_PORTS_BMP_STC                          *enableBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricHGLinkFlowControlStatusGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3], tc [0 / 3],
                   and non-NULL enableBmpPtr.
    Expected: GT_OK.
    1.2. Call with out of range tc [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with enableBmpPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT            fport = 0;
    GT_U32              tc    = 0;
    CPSS_PORTS_BMP_STC  enableBmp;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3], tc [0 / 3],
                           and non-NULL enableBmpPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport = 0;
        tc    = 0;

        st = cpssExMxPmFabricHGLinkFlowControlStatusGet(dev, fport, tc, &enableBmp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, tc);

        /* Call with fport [3] */
        fport = 3;
        tc    = 3;

        st = cpssExMxPmFabricHGLinkFlowControlStatusGet(dev, fport, tc, &enableBmp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, tc);

        /*
            1.2. Call with out of range tc [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        tc = 4;

        st = cpssExMxPmFabricHGLinkFlowControlStatusGet(dev, fport, tc, &enableBmp);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, tc);

        tc = 0;

        /*
            1.3. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmFabricHGLinkFlowControlStatusGet(dev, fport, tc, &enableBmp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with enableBmpPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricHGLinkFlowControlStatusGet(dev, fport, tc, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enableBmpPtr = NULL", dev);
    }

    fport = 0;
    tc = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricHGLinkFlowControlStatusGet(dev, fport, tc, &enableBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricHGLinkFlowControlStatusGet(dev, fport, tc, &enableBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricHGLinkFlowControlTxConfigSet
(
    IN  GT_U8                                       devNum,
    IN  GT_FPORT                                    fport,
    IN  CPSS_EXMXPM_FABRIC_HGLINK_FC_TX_CONFIG_STC  *fcTxConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricHGLinkFlowControlTxConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3]
                   and fcTxConfigPtr {fcRefeshRate [0 / 0xFFFFFF],
                                      tcFcAwarePeer [GT_FALSE / GT_TRUE],
                                      periodFcTxEnable [GT_FALSE / GT_TRUE],
                                      fcTxEnable [GT_FALSE / GT_TRUE]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricHGLinkFlowControlTxConfigGet with the same
                                                                fport
                                                                and non-NULL fcTxConfigPtr.
    Expected: GT_OK and the same fcTxConfigPtr.
    1.3. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range fcTxConfigPtr->fcRefeshRate [0xFFFFFF+1]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with fcTxConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                    fport   = 0;
    CPSS_EXMXPM_FABRIC_HGLINK_FC_TX_CONFIG_STC  fcTxConfig;
    CPSS_EXMXPM_FABRIC_HGLINK_FC_TX_CONFIG_STC  fcTxConfigGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3]
                           and fcTxConfigPtr {fcRefeshRate [0 / 0xFFFFFF],
                                              tcFcAwarePeer [GT_FALSE / GT_TRUE],
                                              periodFcTxEnable [GT_FALSE / GT_TRUE],
                                              fcTxEnable [GT_FALSE / GT_TRUE]}.
            Expected: GT_OK.
        */
        /* Call with fport [0] */
        fport   = 0;

        fcTxConfig.fcRefeshRate     = 0;
        fcTxConfig.tcFcAwarePeer    = GT_FALSE;
        fcTxConfig.periodFcTxEnable = GT_FALSE;
        fcTxConfig.fcTxEnable       = GT_FALSE;

        st = cpssExMxPmFabricHGLinkFlowControlTxConfigSet(dev, fport, &fcTxConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricHGLinkFlowControlTxConfigGet with the same
                                                                        fport
                                                                        and non-NULL fcTxConfigPtr.
            Expected: GT_OK and the same fcTxConfigPtr.
        */
        st = cpssExMxPmFabricHGLinkFlowControlTxConfigGet(dev, fport, &fcTxConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricHGLinkFlowControlTxConfigGet: %d, %d, %d", dev, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fcTxConfig.fcRefeshRate, fcTxConfigGet.fcRefeshRate,
                   "get another fcTxConfigPtr->fcRefeshRate then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fcTxConfig.tcFcAwarePeer, fcTxConfigGet.tcFcAwarePeer,
                   "get another fcTxConfigPtr->tcFcAwarePeer then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fcTxConfig.periodFcTxEnable, fcTxConfigGet.periodFcTxEnable,
                   "get another fcTxConfigPtr->periodFcTxEnable then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fcTxConfig.fcTxEnable, fcTxConfigGet.fcTxEnable,
                   "get another fcTxConfigPtr->fcTxEnable then was set: %d", dev);

        /* Call with fport [3] */
        fport   = 3;

        fcTxConfig.fcRefeshRate     = 0xFFFFFF;
        fcTxConfig.tcFcAwarePeer    = GT_TRUE;
        fcTxConfig.periodFcTxEnable = GT_TRUE;
        fcTxConfig.fcTxEnable       = GT_TRUE;

        st = cpssExMxPmFabricHGLinkFlowControlTxConfigSet(dev, fport, &fcTxConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.2. Call cpssExMxPmFabricHGLinkFlowControlTxConfigGet with the same
                                                                        fport
                                                                        and non-NULL fcTxConfigPtr.
            Expected: GT_OK and the same fcTxConfigPtr.
        */
        st = cpssExMxPmFabricHGLinkFlowControlTxConfigGet(dev, fport, &fcTxConfigGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricHGLinkFlowControlTxConfigGet: %d, %d, %d", dev, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fcTxConfig.fcRefeshRate, fcTxConfigGet.fcRefeshRate,
                   "get another fcTxConfigPtr->fcRefeshRate then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fcTxConfig.tcFcAwarePeer, fcTxConfigGet.tcFcAwarePeer,
                   "get another fcTxConfigPtr->tcFcAwarePeer then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fcTxConfig.periodFcTxEnable, fcTxConfigGet.periodFcTxEnable,
                   "get another fcTxConfigPtr->periodFcTxEnable then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fcTxConfig.fcTxEnable, fcTxConfigGet.fcTxEnable,
                   "get another fcTxConfigPtr->fcTxEnable then was set: %d", dev);


        /*
            1.3. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmFabricHGLinkFlowControlTxConfigSet(dev, fport, &fcTxConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with out of range fcTxConfigPtr->fcRefeshRate [0xFFFFFF+1]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fcTxConfig.fcRefeshRate = 0xFFFFFF + 1;

        st = cpssExMxPmFabricHGLinkFlowControlTxConfigSet(dev, fport, &fcTxConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fcTxConfigPtr->fcRefeshRate = %d",
                                         dev, fcTxConfig.fcRefeshRate);

        fcTxConfig.fcRefeshRate = 0;

        /*
            1.5. Call with fcTxConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricHGLinkFlowControlTxConfigSet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fcTxConfigPtr = NULL", dev);
    }

    fport   = 0;

    fcTxConfig.fcRefeshRate     = 0;
    fcTxConfig.tcFcAwarePeer    = GT_FALSE;
    fcTxConfig.periodFcTxEnable = GT_FALSE;
    fcTxConfig.fcTxEnable       = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricHGLinkFlowControlTxConfigSet(dev, fport, &fcTxConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricHGLinkFlowControlTxConfigSet(dev, fport, &fcTxConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricHGLinkFlowControlTxConfigGet
(
    IN  GT_U8                                       devNum,
    IN  GT_FPORT                                    fport,
    OUT CPSS_EXMXPM_FABRIC_HGLINK_FC_TX_CONFIG_STC  *fcTxConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricHGLinkFlowControlTxConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3]
                   and non-NULL fcTxConfigPtr.
    Expected: GT_OK.
    1.2. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with fcTxConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                    fport   = 0;
    CPSS_EXMXPM_FABRIC_HGLINK_FC_TX_CONFIG_STC  fcTxConfig;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call fport [0 / 3]
                           and non-NULL fcTxConfigPtr.
            Expected: GT_OK.
        */

        fport = 0;

        st = cpssExMxPmFabricHGLinkFlowControlTxConfigGet(dev, fport, &fcTxConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        fport = 3;

        st = cpssExMxPmFabricHGLinkFlowControlTxConfigGet(dev, fport, &fcTxConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);


        /*
            1.2. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmFabricHGLinkFlowControlTxConfigGet(dev, fport, &fcTxConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.3. Call with fcTxConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricHGLinkFlowControlTxConfigGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fcTxConfigPtr = NULL", dev);
    }

    fport = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricHGLinkFlowControlTxConfigGet(dev, fport, &fcTxConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricHGLinkFlowControlTxConfigGet(dev, fport, &fcTxConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricHGLinkFlowControlCellsCntrGet
(
    IN  GT_U8                            devNum,
    IN  GT_FPORT                         fport,
    IN  CPSS_EXMXPM_FABRIC_DIR_ENT       cellDirection,
    OUT GT_U32                           *fcCellCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricHGLinkFlowControlCellsCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3],
                   cellDirection [CPSS_EXMXPM_FABRIC_DIR_RX_E /
                                  CPSS_EXMXPM_FABRIC_DIR_TX_E]
                   and non-NULL fcCellCntrPtr.
    Expected: GT_OK.
    1.3. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with fcCellCntrPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                    fport         = 0;
    CPSS_EXMXPM_FABRIC_DIR_ENT  cellDirection = CPSS_EXMXPM_FABRIC_DIR_RX_E;
    GT_U32                      fcCellCntr    = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3],
                           cellDirection [CPSS_EXMXPM_FABRIC_DIR_RX_E /
                                          CPSS_EXMXPM_FABRIC_DIR_TX_E]
                           and non-NULL fcCellCntrPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport         = 0;
        cellDirection = CPSS_EXMXPM_FABRIC_DIR_RX_E;

        st = cpssExMxPmFabricHGLinkFlowControlCellsCntrGet(dev, fport, cellDirection, &fcCellCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, cellDirection);

        /* Call with fport [3] */
        fport         = 3;
        cellDirection = CPSS_EXMXPM_FABRIC_DIR_RX_E;

        st = cpssExMxPmFabricHGLinkFlowControlCellsCntrGet(dev, fport, cellDirection, &fcCellCntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, cellDirection);

        /*
            1.2. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmFabricHGLinkFlowControlCellsCntrGet(dev, fport, cellDirection, &fcCellCntr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with fcCellCntrPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricHGLinkFlowControlCellsCntrGet(dev, fport, cellDirection, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fcCellCntrPtr = NULL", dev);
    }

    fport         = 0;
    cellDirection = CPSS_EXMXPM_FABRIC_DIR_RX_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricHGLinkFlowControlCellsCntrGet(dev, fport, cellDirection, &fcCellCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricHGLinkFlowControlCellsCntrGet(dev, fport, cellDirection, &fcCellCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricHGLinkRxErrorCntrGet
(
    IN  GT_U8                                           devNum,
    IN  GT_FPORT                                        fport,
    OUT CPSS_EXMXPM_FABRIC_HGLINK_RX_ERROR_CNTRS_STC    *rxErrorsCntrsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricHGLinkRxErrorCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fport [0 / 3]
                   and non-NULL rxErrorsCntrsPtr.
    Expected: GT_OK.
    1.3. Call with out of range fport [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with rxErrorsCntrsPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_FPORT                                        fport   = 0;
    CPSS_EXMXPM_FABRIC_HGLINK_RX_ERROR_CNTRS_STC    rxErrorsCntrs;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fport [0 / 3]
                           and non-NULL rxErrorsCntrsPtr.
            Expected: GT_OK.
        */

        /* Call with fport [0] */
        fport   = 0;

        st = cpssExMxPmFabricHGLinkRxErrorCntrGet(dev, fport, &rxErrorsCntrs);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /* Call with fport [3] */
        fport   = 3;

        st = cpssExMxPmFabricHGLinkRxErrorCntrGet(dev, fport, &rxErrorsCntrs);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

        /*
            1.3. Call with out of range fport [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = 4;

        st = cpssExMxPmFabricHGLinkRxErrorCntrGet(dev, fport, &rxErrorsCntrs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with rxErrorsCntrsPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricHGLinkRxErrorCntrGet(dev, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rxErrorsCntrsPtr = NULL", dev);
    }

    fport   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricHGLinkRxErrorCntrGet(dev, fport, &rxErrorsCntrs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricHGLinkRxErrorCntrGet(dev, fport, &rxErrorsCntrs);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmFabricHGLink suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmFabricHGLink)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricHGLinkPcsConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricHGLinkPcsConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricHGLinkPcsRxStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricHGLinkMacConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricHGLinkMacConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricHGLinkFlowControlEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricHGLinkFlowControlEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricHGLinkFlowControlStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricHGLinkFlowControlTxConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricHGLinkFlowControlTxConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricHGLinkFlowControlCellsCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricHGLinkRxErrorCntrGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmFabricHGLink)

