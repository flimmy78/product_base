/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmFabricPktSegmUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmFabricPktSegm, that provides
*       CPSS ExMxPm Fabric Connectivity Packet Segmentation
*       and Cell Scheduler APIs.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricPktSegm.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define FABRIC_PKT_SEGM_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmArbiterEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmArbiterEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricPktSegmArbiterEnableGet
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

        st = cpssExMxPmFabricPktSegmArbiterEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmFabricPktSegmArbiterEnableGet
                      with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmFabricPktSegmArbiterEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricPktSegmArbiterEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable then was set: %d", dev);

        /*
            1.1. Call with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        st = cpssExMxPmFabricPktSegmArbiterEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmFabricPktSegmArbiterEnableGet
                      with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmFabricPktSegmArbiterEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricPktSegmArbiterEnableGet: %d", dev);

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
        st = cpssExMxPmFabricPktSegmArbiterEnableSet(dev, enableGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmArbiterEnableSet(dev, enableGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmArbiterEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmArbiterEnableGet)
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
        st = cpssExMxPmFabricPktSegmArbiterEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktSegmArbiterEnableGet(dev, NULL);
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
        st = cpssExMxPmFabricPktSegmArbiterEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmArbiterEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmMiscConfigSet
(
    IN GT_U8                                       devNum,
    IN CPSS_EXMXPM_FABRIC_PKT_SEGM_CONFIG_MISC_STC *miscConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmMiscConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with miscConfigPtr {mcVoqId [CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_VOQ_ID_DP_LBH0_E /
                                           CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_VOQ_ID_SRC_DEV0_E],
                                  peelCrcEnable[GT_FALSE / GT_TRUE] }.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricPktSegmMiscConfigGet
              with non-NULL miscConfigPtr.
    Expected: GT_OK and the same miscConfigPtr.
    1.3. Call with out of range miscConfigPtr->mcVoqId [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with miscConfigPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_SEGM_CONFIG_MISC_STC miscConfig;
    CPSS_EXMXPM_FABRIC_PKT_SEGM_CONFIG_MISC_STC miscConfigGet;


    cpssOsBzero((GT_VOID*) &miscConfig, sizeof(miscConfig));
    cpssOsBzero((GT_VOID*) &miscConfigGet, sizeof(miscConfigGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with miscConfigPtr {mcVoqId [CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_VOQ_ID_DP_LBH0_E /
                                                   CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_VOQ_ID_SRC_DEV0_E],
                                          peelCrcEnable[GT_FALSE / GT_TRUE] }.
            Expected: GT_OK.
        */
        miscConfig.mcVoqId = CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_VOQ_ID_DP_LBH0_E;
        miscConfig.peelCrcEnable = GT_FALSE;

        st = cpssExMxPmFabricPktSegmMiscConfigSet(dev, &miscConfig);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, miscConfigPtr->mcVoqId = %d",
                                     dev, miscConfig.mcVoqId);

        /*
            1.2. Call cpssExMxPmFabricPktSegmMiscConfigGet
                      with non-NULL miscConfigPtr.
            Expected: GT_OK and the same miscConfigPtr.
        */
        st = cpssExMxPmFabricPktSegmMiscConfigGet(dev, &miscConfigGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricPktSegmMiscConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(miscConfig.mcVoqId, miscConfigGet.mcVoqId,
                   "get another miscConfig.mcVoqId then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(miscConfig.peelCrcEnable, miscConfigGet.peelCrcEnable,
                   "get another miscConfig.peelCrcEnable then was set: %d", dev);

        /*
            1.1. Call with miscConfigPtr {mcVoqId [CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_VOQ_ID_DP_LBH0_E /
                                                   CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_VOQ_ID_SRC_DEV0_E],
                                          peelCrcEnable[GT_FALSE / GT_TRUE] }.
            Expected: GT_OK.
        */
        miscConfig.mcVoqId = CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_VOQ_ID_SRC_DEV0_E;
        miscConfig.peelCrcEnable = GT_TRUE;

        st = cpssExMxPmFabricPktSegmMiscConfigSet(dev, &miscConfig);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, miscConfigPtr->mcVoqId = %d",
                                     dev, miscConfig.mcVoqId);

        /*
            1.2. Call cpssExMxPmFabricPktSegmMiscConfigGet
                      with non-NULL miscConfigPtr.
            Expected: GT_OK and the same miscConfigPtr.
        */
        st = cpssExMxPmFabricPktSegmMiscConfigGet(dev, &miscConfigGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricPktSegmMiscConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(miscConfig.mcVoqId, miscConfigGet.mcVoqId,
                   "get another miscConfig.mcVoqId then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(miscConfig.peelCrcEnable, miscConfigGet.peelCrcEnable,
                   "get another miscConfig.peelCrcEnable then was set: %d", dev);

    }

    miscConfig.mcVoqId = CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_VOQ_ID_DP_LBH0_E;
    miscConfig.peelCrcEnable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktSegmMiscConfigSet(dev, &miscConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmMiscConfigSet(dev, &miscConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmMiscConfigGet
(
    IN  GT_U8                                       devNum,
    OUT CPSS_EXMXPM_FABRIC_PKT_SEGM_CONFIG_MISC_STC *miscConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmMiscConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null miscConfigPtr.
    Expected: GT_OK.
    1.2. Call with miscConfigPtr [NULL].
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_SEGM_CONFIG_MISC_STC miscConfig;


    cpssOsBzero((GT_VOID*) &miscConfig, sizeof(miscConfig));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null miscConfigPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmFabricPktSegmMiscConfigGet(dev, &miscConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with miscConfigPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktSegmMiscConfigGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, miscConfigPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktSegmMiscConfigGet(dev, &miscConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmMiscConfigGet(dev, &miscConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmConfigFifoStatusModeSet
(
    IN GT_U8                                            devNum,
    IN CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_MODE_ENT mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmConfigFifoStatusModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mode [CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_FULL_E /
                        CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_EMPTY_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricPktSegmConfigFifoStatusModeGet
              with non-NULL modePtr.
    Expected: GT_OK and the same mode.
    1.3. Call with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_MODE_ENT mode    = CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_FULL_E;
    CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_MODE_ENT modeGet = CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_FULL_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_FULL_E /
                                CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_EMPTY_E].
            Expected: GT_OK.
        */
        mode = CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_FULL_E;

        st = cpssExMxPmFabricPktSegmConfigFifoStatusModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmFabricPktSegmConfigFifoStatusModeGet
                      with non-NULL modePtr.
            Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmFabricPktSegmConfigFifoStatusModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricPktSegmConfigFifoStatusModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                   "get another mode then was set: %d", dev);

        /*
            1.1. Call with mode [CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_FULL_E /
                                CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_EMPTY_E].
            Expected: GT_OK.
        */
        mode = CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_EMPTY_E;

        st = cpssExMxPmFabricPktSegmConfigFifoStatusModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmFabricPktSegmConfigFifoStatusModeGet
                      with non-NULL modePtr.
            Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmFabricPktSegmConfigFifoStatusModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricPktSegmConfigFifoStatusModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                   "get another mode then was set: %d", dev);
        /*
            1.3. Call with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = FABRIC_PKT_SEGM_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricPktSegmConfigFifoStatusModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_FULL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktSegmConfigFifoStatusModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmConfigFifoStatusModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmConfigFifoStatusModeGet
(
    IN  GT_U8                                            devNum,
    OUT CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_MODE_ENT *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmConfigFifoStatusModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null modePtr.
    Expected: GT_OK.
    1.2. Call with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_MODE_ENT mode = CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_FULL_E;


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
        st = cpssExMxPmFabricPktSegmConfigFifoStatusModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktSegmConfigFifoStatusModeGet(dev, NULL);
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
        st = cpssExMxPmFabricPktSegmConfigFifoStatusModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmConfigFifoStatusModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmFifoStatusGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   xbarNum,
    IN  GT_FPORT fport,
    IN  GT_U32   fabricTc,
    OUT GT_BOOL  *fifoStatusPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmFifoStatusGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with xbarNum [0 / 1],
                   fport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS],
                   fabricTc [0 / 3]
                   and non-NULL fifoStatusPtr.
    Expected: GT_OK.
    1.2. Call with out of range xbarNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range fport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range fabricTc [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with fifoStatusPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      xbarNum    = 0;
    GT_FPORT    fport      = 0;
    GT_U32      fabricTc   = 0;
    GT_BOOL     fifoStatus = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            ITERATE_DEVICES (ExMxPm)
            1.1. Call with xbarNum [0 / 1],
                           fport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS],
                           fabricTc [0 / 3]
                           and non-NULL fifoStatusPtr.
            Expected: GT_OK.
        */
        /* Call with xbarNum = 0 */
        xbarNum  = 0;
        fport    = 0;
        fabricTc = 0;

        st = cpssExMxPmFabricPktSegmFifoStatusGet(dev, xbarNum, fport, fabricTc, &fifoStatus);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, xbarNum, fport, fabricTc);

        /* Call with xbarNum = 1 */
        xbarNum  = 1;
        fport    = CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS;
        fabricTc = 3;

        st = cpssExMxPmFabricPktSegmFifoStatusGet(dev, xbarNum, fport, fabricTc, &fifoStatus);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, xbarNum, fport, fabricTc);

        /*
            1.2. Call with out of range xbarNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        xbarNum = 2;

        st = cpssExMxPmFabricPktSegmFifoStatusGet(dev, xbarNum, fport, fabricTc, &fifoStatus);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, xbarNum);

        xbarNum = 1;

        /*
            1.3. Call with out of range fport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS;

        st = cpssExMxPmFabricPktSegmFifoStatusGet(dev, xbarNum, fport, fabricTc, &fifoStatus);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS;

        /*
            1.4. Call with out of range fabricTc [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fabricTc = 4;

        st = cpssExMxPmFabricPktSegmFifoStatusGet(dev, xbarNum, fport, fabricTc, &fifoStatus);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fabricTc = %d", dev, fabricTc);

        fabricTc = 3;

        /*
            1.5. Call with fifoStatusPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktSegmFifoStatusGet(dev, xbarNum, fport, fabricTc, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fifoStatusPtr = NULL", dev);
    }

    xbarNum  = 0;
    fport    = 0;
    fabricTc = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktSegmFifoStatusGet(dev, xbarNum, fport, fabricTc, &fifoStatus);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmFifoStatusGet(dev, xbarNum, fport, fabricTc, &fifoStatus);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmArbConfigSet
(
    IN GT_U8                             devNum,
    IN GT_U32                            arbConfigSetNum,
    IN GT_U32                            fabricTc,
    IN CPSS_EXMXPM_FABRIC_ARB_POLICY_ENT arbPolicy,
    IN GT_U32                            wrrWeight
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmArbConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with arbConfigSetNum [0 / 3],
                   fabricTc [0 / 3],
                   arbPolicy [CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E /
                              CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E]
                   and wrrWeight [0 / 4095].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricPktSegmArbConfigGet
                   with the same arbConfigSetNum,
                   fabricTc
                   and non-NULL arbPolicyPtr,
                                wrrWeightPtr.
    Expected: GT_OK and the same arbPolicy, wrrWeight.
    1.3. Call with out of range arbConfigSetNum [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range fabricTc [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range arbPolicy [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range wrrWeight [4096]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/

    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                            arbConfigSetNum = 0;
    GT_U32                            fabricTc        = 0;
    CPSS_EXMXPM_FABRIC_ARB_POLICY_ENT arbPolicy       = CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E;
    GT_U32                            wrrWeight       = 0;
    CPSS_EXMXPM_FABRIC_ARB_POLICY_ENT arbPolicyGet    = CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E;
    GT_U32                            wrrWeightGet    = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with arbConfigSetNum [0 / 3],
                           fabricTc [0 / 3],
                           arbPolicy [CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E /
                                      CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E]
                           and wrrWeight [0 / 4095].
            Expected: GT_OK.
        */
        arbConfigSetNum = 0;
        fabricTc        = 0;
        arbPolicy       = CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E;
        wrrWeight       = 0;

        st = cpssExMxPmFabricPktSegmArbConfigSet(dev, arbConfigSetNum, fabricTc,
                                                 arbPolicy, wrrWeight);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, arbConfigSetNum,
                                    fabricTc, arbPolicy, wrrWeight);

        /*
            1.2. Call cpssExMxPmFabricPktSegmArbConfigGet
                           with the same arbConfigSetNum,
                           fabricTc
                           and non-NULL arbPolicyPtr,
                                        wrrWeightPtr.
            Expected: GT_OK and the same arbPolicy, wrrWeight.
        */
        st = cpssExMxPmFabricPktSegmArbConfigGet(dev, arbConfigSetNum, fabricTc,
                                                &arbPolicyGet, &wrrWeightGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktSegmArbConfigGet: %d, %d, %d",
                                     dev, arbConfigSetNum, fabricTc);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(arbPolicy, arbPolicyGet,
                   "get anotjer arbPolicy then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(wrrWeight, wrrWeightGet,
                   "get anotjer wrrWeight then was set: %d", dev);

        /*
            1.1. Call with arbConfigSetNum [0 / 3],
                           fabricTc [0 / 3],
                           arbPolicy [CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E /
                                      CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E]
                           and wrrWeight [0 / 4095].
            Expected: GT_OK.
        */
        arbConfigSetNum = 3;
        fabricTc        = 3;
        arbPolicy       = CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E;
        wrrWeight       = 4095;

        st = cpssExMxPmFabricPktSegmArbConfigSet(dev, arbConfigSetNum, fabricTc,
                                                 arbPolicy, wrrWeight);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, arbConfigSetNum,
                                    fabricTc, arbPolicy, wrrWeight);

        /*
            1.2. Call cpssExMxPmFabricPktSegmArbConfigGet
                           with the same arbConfigSetNum,
                           fabricTc
                           and non-NULL arbPolicyPtr,
                                        wrrWeightPtr.
            Expected: GT_OK and the same arbPolicy, wrrWeight.
        */
        st = cpssExMxPmFabricPktSegmArbConfigGet(dev, arbConfigSetNum, fabricTc,
                                                &arbPolicyGet, &wrrWeightGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktSegmArbConfigGet: %d, %d, %d",
                                     dev, arbConfigSetNum, fabricTc);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(arbPolicy, arbPolicyGet,
                   "get anotjer arbPolicy then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(wrrWeight, wrrWeightGet,
                   "get anotjer wrrWeight then was set: %d", dev);

        /*
            1.3. Call with out of range arbConfigSetNum [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        arbConfigSetNum = 4;

        st = cpssExMxPmFabricPktSegmArbConfigSet(dev, arbConfigSetNum, fabricTc,
                                                 arbPolicy, wrrWeight);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, arbConfigSetNum);

        arbConfigSetNum = 3;

        /*
            1.4. Call with out of range fabricTc [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fabricTc = 4;

        st = cpssExMxPmFabricPktSegmArbConfigSet(dev, arbConfigSetNum, fabricTc,
                                                 arbPolicy, wrrWeight);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fabricTc = %d", dev, fabricTc);

        fabricTc = 3;

        /*
            1.5. Call with out of range arbPolicy [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        arbPolicy = FABRIC_PKT_SEGM_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricPktSegmArbConfigSet(dev, arbConfigSetNum, fabricTc,
                                                 arbPolicy, wrrWeight);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, arbPolicy = %d", dev, arbPolicy);

        arbPolicy = CPSS_EXMXPM_FABRIC_ARB_POLICY_SP_E;

        /*
            1.6. Call with out of range wrrWeight [4096]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        wrrWeight = 4096;

        st = cpssExMxPmFabricPktSegmArbConfigSet(dev, arbConfigSetNum, fabricTc,
                                                 arbPolicy, wrrWeight);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, wrrWeight = %d", dev, wrrWeight);
    }

    arbConfigSetNum = 0;
    fabricTc        = 0;
    arbPolicy       = CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E;
    wrrWeight       = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktSegmArbConfigSet(dev, arbConfigSetNum, fabricTc,
                                                arbPolicy, wrrWeight);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmArbConfigSet(dev, arbConfigSetNum, fabricTc,
                                                arbPolicy, wrrWeight);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmArbConfigGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            arbConfigSetNum,
    IN  GT_U32                            fabricTc,
    OUT CPSS_EXMXPM_FABRIC_ARB_POLICY_ENT *arbPolicyPtr,
    OUT GT_U32                            *wrrWeightPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmArbConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with arbConfigSetNum [0 / 3],
                   fabricTc [0 / 3]
                   and non-NULL arbPolicyPrt,
                   non-NULL wrrWeightPtr.
    Expected: GT_OK.
    1.2. Call with out of range arbConfigSetNum [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range fabricTc [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with arbPolicyPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with wrrWeightPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                            arbConfigSetNum = 0;
    GT_U32                            fabricTc        = 0;
    CPSS_EXMXPM_FABRIC_ARB_POLICY_ENT arbPolicy       = CPSS_EXMXPM_FABRIC_ARB_POLICY_WRR_E;
    GT_U32                            wrrWeight       = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with arbConfigSetNum [0 / 3],
                           fabricTc [0 / 3]
                           and non-NULL arbPolicyPrt,
                           non-NULL wrrWeightPtr.
            Expected: GT_OK.
        */
        /* call with arbConfigSetNum = 0 */
        arbConfigSetNum = 0;
        fabricTc        = 0;

        st = cpssExMxPmFabricPktSegmArbConfigGet(dev, arbConfigSetNum, fabricTc,
                                                 &arbPolicy, &wrrWeight);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, arbConfigSetNum,
                                    fabricTc, arbPolicy, wrrWeight);

        /* call with arbConfigSetNum = 3 */
        arbConfigSetNum = 3;
        fabricTc        = 3;

        st = cpssExMxPmFabricPktSegmArbConfigGet(dev, arbConfigSetNum, fabricTc,
                                                &arbPolicy, &wrrWeight);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, arbConfigSetNum,
                                    fabricTc, arbPolicy, wrrWeight);

        /*
            1.2. Call with out of range arbConfigSetNum [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        arbConfigSetNum = 4;

        st = cpssExMxPmFabricPktSegmArbConfigGet(dev, arbConfigSetNum, fabricTc,
                                                 &arbPolicy, &wrrWeight);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, arbConfigSetNum);

        arbConfigSetNum = 3;

        /*
            1.3. Call with out of range fabricTc [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fabricTc = 4;

        st = cpssExMxPmFabricPktSegmArbConfigGet(dev, arbConfigSetNum, fabricTc,
                                                 &arbPolicy, &wrrWeight);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fabricTc = %d", dev, fabricTc);

        fabricTc = 3;

        /*
            1.4. Call with arbPolicyPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktSegmArbConfigGet(dev, arbConfigSetNum, fabricTc,
                                                 NULL, &wrrWeight);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, arbPolicyPtr = NULL", dev);

        /*
            1.5. Call with wrrWeightPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktSegmArbConfigGet(dev, arbConfigSetNum, fabricTc,
                                                 &arbPolicy, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, wrrWeightPtr = NULL", dev);
    }

    arbConfigSetNum = 0;
    fabricTc        = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktSegmArbConfigGet(dev, arbConfigSetNum, fabricTc,
                                                 &arbPolicy, &wrrWeight);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmArbConfigGet(dev, arbConfigSetNum, fabricTc,
                                             &arbPolicy, &wrrWeight);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmErrorCntrGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *errorCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmErrorCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null errorCntrPtr.
    Expected: GT_OK.
    1.2. Call with errorCntrPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      errorCntr = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null errorCntrPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmFabricPktSegmErrorCntrGet(dev, &errorCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with errorCntrPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktSegmErrorCntrGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, errorCntrPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktSegmErrorCntrGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmErrorCntrGet(dev, NULL);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmFportToArbConfigMapSet
(
    IN GT_U8    devNum,
    IN GT_U32   xbarNum,
    IN GT_FPORT fport,
    IN GT_U32   arbConfigSetNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmFportToArbConfigMapSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with xbarNum [0 / 1],
                   fport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS]
                   and arbConfigSetNum [0 / 3].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricPktSegmFportToArbConfigMapGet
              with the same xbarNum,
                   fport
                   and non-NULL arbConfigSetNumPtr.
    Expected: GT_OK and the same arbConfigSetNum.
    1.3. Call with out of range xbarNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range fport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range arbConfigSetNum [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      xbarNum = 0;
    GT_FPORT    fport   = 0;
    GT_U32      arbConfigSetNum    = 0;
    GT_U32      arbConfigSetNumGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with xbarNum [0 / 1],
                           fport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS]
                           and arbConfigSetNum [0 / 3].
            Expected: GT_OK.
        */
        xbarNum         = 0;
        fport           = 0;
        arbConfigSetNum = 0;

        st = cpssExMxPmFabricPktSegmFportToArbConfigMapSet(dev, xbarNum, fport, arbConfigSetNum);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, xbarNum, fport, arbConfigSetNum);

        /*
            1.2. Call cpssExMxPmFabricPktSegmFportToArbConfigMapGet
                      with the same xbarNum,
                           fport
                           and non-NULL arbConfigSetNumPtr.
            Expected: GT_OK and the same arbConfigSetNum.
        */
        st = cpssExMxPmFabricPktSegmFportToArbConfigMapGet(dev, xbarNum, fport, &arbConfigSetNumGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktSegmFportToArbConfigMapGet: %d, %d, %d",
                                     dev, xbarNum, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(arbConfigSetNum, arbConfigSetNumGet,
                   "get another arbConfigSetNum then was set: %d", dev);

        /*
            1.1. Call with xbarNum [0 / 1],
                           fport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS]
                           and arbConfigSetNum [0 / 3].
            Expected: GT_OK.
        */
        xbarNum         = 1;
        fport           = CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS;
        arbConfigSetNum = 3;

        st = cpssExMxPmFabricPktSegmFportToArbConfigMapSet(dev, xbarNum, fport, arbConfigSetNum);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, xbarNum, fport, arbConfigSetNum);

        /*
            1.2. Call cpssExMxPmFabricPktSegmFportToArbConfigMapGet
                      with the same xbarNum,
                           fport
                           and non-NULL arbConfigSetNumPtr.
            Expected: GT_OK and the same arbConfigSetNum.
        */
        st = cpssExMxPmFabricPktSegmFportToArbConfigMapGet(dev, xbarNum, fport, &arbConfigSetNumGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktSegmFportToArbConfigMapGet: %d, %d, %d",
                                     dev, xbarNum, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(arbConfigSetNum, arbConfigSetNumGet,
                   "get another arbConfigSetNum then was set: %d", dev);

        /*
            1.3. Call with out of range xbarNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        xbarNum = 2;

        st = cpssExMxPmFabricPktSegmFportToArbConfigMapSet(dev, xbarNum, fport, arbConfigSetNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, xbarNum);

        xbarNum = 1;

        /*
            1.4. Call with out of range fport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS;

        st = cpssExMxPmFabricPktSegmFportToArbConfigMapSet(dev, xbarNum, fport, arbConfigSetNum);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.5. Call with out of range arbConfigSetNum [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        arbConfigSetNum = 4;

        st = cpssExMxPmFabricPktSegmFportToArbConfigMapSet(dev, xbarNum, fport, arbConfigSetNum);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, arbConfigSetNum = %d", dev, arbConfigSetNum);
    }

    xbarNum         = 0;
    fport           = 0;
    arbConfigSetNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktSegmFportToArbConfigMapSet(dev, xbarNum, fport, arbConfigSetNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmFportToArbConfigMapSet(dev, xbarNum, fport, arbConfigSetNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmFportToArbConfigMapGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   xbarNum,
    IN  GT_FPORT fport,
    OUT GT_U32   *arbConfigSetNumPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmFportToArbConfigMapGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with xbarNum [0 / 1],
                   fport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS]
                   and non-NULL arbConfigSetNumPtr.
    Expected: GT_OK.
    1.2. Call with out of range xbarNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range fport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with arbConfigSetNumPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      xbarNum         = 0;
    GT_FPORT    fport           = 0;
    GT_U32      arbConfigSetNum = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with xbarNum [0 / 1],
                           fport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS]
                           and non-NULL arbConfigSetNumPtr.
            Expected: GT_OK.
        */
        /* call with xbarNum = 0 */
        xbarNum = 0;
        fport   = 0;

        st = cpssExMxPmFabricPktSegmFportToArbConfigMapGet(dev, xbarNum, fport, &arbConfigSetNum);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, xbarNum, fport);

        /* call with xbarNum = 1 */
        xbarNum = 1;
        fport   = CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS;

        st = cpssExMxPmFabricPktSegmFportToArbConfigMapGet(dev, xbarNum, fport, &arbConfigSetNum);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, xbarNum, fport);

        /*
            1.2. Call with out of range xbarNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        xbarNum = 2;

        st = cpssExMxPmFabricPktSegmFportToArbConfigMapGet(dev, xbarNum, fport, &arbConfigSetNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, xbarNum);

        xbarNum = 1;

        /*
            1.3. Call with out of range fport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS;

        st = cpssExMxPmFabricPktSegmFportToArbConfigMapGet(dev, xbarNum, fport, &arbConfigSetNum);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS;

        /*
            1.4. Call with arbConfigSetNumPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktSegmFportToArbConfigMapGet(dev, xbarNum, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, arbConfigSetNumPtr = NULL", dev);
    }

    xbarNum = 0;
    fport   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktSegmFportToArbConfigMapGet(dev, xbarNum, fport, &arbConfigSetNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmFportToArbConfigMapGet(dev, xbarNum, fport, &arbConfigSetNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmMcUcRatioSet
(
    IN GT_U8                                          devNum,
    IN CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_RATIO_ENT    mcUcRatio
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmMcUcRatioSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mcUcRatio [CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_1_RATIO_E /
                              CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_8_RATIO_E /
                              CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_64_RATIO_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricPktSegmMcUcRatioGet
              with non-NULL mcUcRatioPtr.
    Expected: GT_OK and the same mcUcRatio.
    1.3. Call with out of range mcUcRatio [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_RATIO_ENT mcUcRatio    = CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_1_RATIO_E;
    CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_RATIO_ENT mcUcRatioGet = CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_1_RATIO_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mcUcRatio [CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_1_RATIO_E /
                                      CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_8_RATIO_E /
                                      CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_64_RATIO_E].
            Expected: GT_OK.
        */
        mcUcRatio = CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_1_RATIO_E;

        st = cpssExMxPmFabricPktSegmMcUcRatioSet(dev, mcUcRatio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mcUcRatio);

        /*
            1.2. Call cpssExMxPmFabricPktSegmMcUcRatioGet
                      with non-NULL mcUcRatioPtr.
            Expected: GT_OK and the same mcUcRatio.
        */
        st = cpssExMxPmFabricPktSegmMcUcRatioGet(dev, &mcUcRatioGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktSegmMcUcRatioGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mcUcRatio, mcUcRatioGet,
                   "get another mcUcRatio then was set: %d", dev);

        /*
            1.1. Call with mcUcRatio [CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_1_RATIO_E /
                                      CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_8_RATIO_E /
                                      CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_64_RATIO_E].
            Expected: GT_OK.
        */
        mcUcRatio = CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_8_RATIO_E;

        st = cpssExMxPmFabricPktSegmMcUcRatioSet(dev, mcUcRatio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mcUcRatio);

        /*
            1.2. Call cpssExMxPmFabricPktSegmMcUcRatioGet
                      with non-NULL mcUcRatioPtr.
            Expected: GT_OK and the same mcUcRatio.
        */
        st = cpssExMxPmFabricPktSegmMcUcRatioGet(dev, &mcUcRatioGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktSegmMcUcRatioGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mcUcRatio, mcUcRatioGet,
                   "get another mcUcRatio then was set: %d", dev);

        /*
            1.1. Call with mcUcRatio [CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_1_RATIO_E /
                                      CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_8_RATIO_E /
                                      CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_64_RATIO_E].
            Expected: GT_OK.
        */
        mcUcRatio = CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_64_RATIO_E;

        st = cpssExMxPmFabricPktSegmMcUcRatioSet(dev, mcUcRatio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mcUcRatio);

        /*
            1.2. Call cpssExMxPmFabricPktSegmMcUcRatioGet
                      with non-NULL mcUcRatioPtr.
            Expected: GT_OK and the same mcUcRatio.
        */
        st = cpssExMxPmFabricPktSegmMcUcRatioGet(dev, &mcUcRatioGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktSegmMcUcRatioGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mcUcRatio, mcUcRatioGet,
                   "get another mcUcRatio then was set: %d", dev);

        /*
            1.3. Call with out of range mcUcRatio [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mcUcRatio = FABRIC_PKT_SEGM_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricPktSegmMcUcRatioSet(dev, mcUcRatio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mcUcRatio);
    }

    mcUcRatio = CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_1_RATIO_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktSegmMcUcRatioSet(dev, mcUcRatio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmMcUcRatioSet(dev, mcUcRatio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmMcUcRatioGet
(
    IN  GT_U8                                          devNum,
    OUT CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_RATIO_ENT    *mcUcRatioPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmMcUcRatioGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null mcUcRatioPtr.
    Expected: GT_OK.
    1.2. Call with mcUcRatioPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_RATIO_ENT mcUcRatio = CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_1_1_RATIO_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null mcUcRatioPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmFabricPktSegmMcUcRatioGet(dev, &mcUcRatio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with mcUcRatioPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktSegmMcUcRatioGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mcUcRatioPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktSegmMcUcRatioGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmMcUcRatioGet(dev, NULL);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricPktSegmEnableGet
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

        st = cpssExMxPmFabricPktSegmEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmFabricPktSegmEnableGet
                      with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmFabricPktSegmEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricPktSegmEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable then was set: %d", dev);

        /*
            1.1. Call with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        st = cpssExMxPmFabricPktSegmEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmFabricPktSegmEnableGet
                      with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmFabricPktSegmEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricPktSegmEnableGet: %d", dev);

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
        st = cpssExMxPmFabricPktSegmEnableSet(dev, enableGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmEnableSet(dev, enableGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmEnableGet)
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
        st = cpssExMxPmFabricPktSegmEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktSegmEnableGet(dev, NULL);
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
        st = cpssExMxPmFabricPktSegmEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet
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

        st = cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet
                      with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable then was set: %d", dev);

        /*
            1.1. Call with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        st = cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet
                      with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet: %d", dev);

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
        st = cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableSet(dev, enableGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableSet(dev, enableGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet)
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
        st = cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet(dev, NULL);
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
        st = cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmFabricPktSegm suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmFabricPktSegm)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmArbiterEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmArbiterEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmMiscConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmMiscConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmConfigFifoStatusModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmConfigFifoStatusModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmFifoStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmArbConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmArbConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmErrorCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmFportToArbConfigMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmFportToArbConfigMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmMcUcRatioSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmMcUcRatioGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmFabricPktSegm)

