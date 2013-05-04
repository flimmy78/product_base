/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNetIfUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChNetIf, that provides
*       Include DxCh network interface API functions
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define NETIF_VALID_PHY_PORT_CNS    0

/* Internal functions forward declaration */

/*  Set dsaInfoPtr->dsaInfo.toCpu structure.         */
static void prvSetDsaInfoToCpu      (OUT CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr);

/*  Set dsaInfoPtr->dsaInfo.fromCpu structure.       */
static void prvSetDsaInfoFromCpu    (OUT CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr);

/*  Set dsaInfoPtr->dsaInfo.toAnalizer structure.    */
static void prvSetDsaInfoToAnalizer (OUT CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr);

/* Set dsaInfoPtr->dsaInfo.forward structure.        */
static void prvSetDsaInfoForward     (OUT CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr);

/* This routine returns core clock per device.       */
static GT_STATUS prvUtfCoreClockGet  (IN GT_U8 dev, OUT GT_U32 *coreClock);

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfPrePendTwoBytesHeaderSet
(
    IN  GT_U8                           devNum,
    IN  GT_BOOL                         enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfPrePendTwoBytesHeaderSet)
{
/*
    ITERATE_DEVICE (DXCHX)
    1.1. Call with enable[GT_FALSE / GT_TRUE]
    Expected: GT_OK.
    1.2. Call cpssDxChNetIfPrePendTwoBytesHeaderGet with the same params.
    Expected: GT_OK and the same values.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    GT_BOOL     enable = GT_FALSE;
    GT_BOOL     enableGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable[GT_FALSE / GT_TRUE]
            Expected: GT_OK.
        */
        /* iterate with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssDxChNetIfPrePendTwoBytesHeaderSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChNetIfPrePendTwoBytesHeaderGet with the same params.
            Expected: GT_OK and the same values.
        */
        st = cpssDxChNetIfPrePendTwoBytesHeaderGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                         "got another enable than was set %d", dev);

        /* iterate with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxChNetIfPrePendTwoBytesHeaderSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChNetIfPrePendTwoBytesHeaderGet with the same params.
            Expected: GT_OK and the same values.
        */
        st = cpssDxChNetIfPrePendTwoBytesHeaderGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                         "got another enable than was set %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfPrePendTwoBytesHeaderSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfPrePendTwoBytesHeaderSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfPrePendTwoBytesHeaderGet
(
    IN  GT_U8                          devNum,
    OUT GT_BOOL                        *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfPrePendTwoBytesHeaderGet)
{
/*
    ITERATE_DEVICE (DxChx)
    1.1. Call with not NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChNetIfPrePendTwoBytesHeaderGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfPrePendTwoBytesHeaderGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfPrePendTwoBytesHeaderGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfPrePendTwoBytesHeaderGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfSdmaRxPacketGet
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    INOUT GT_U32                            *numOfBuffPtr,
    OUT GT_U8                               *packetBuffs[],
    OUT GT_U32                              buffLen[],
    OUT CPSS_DXCH_NET_RX_PARAMS_STC         *rxParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfSdmaRxPacketGet)
{
/*
    ITERATE_DEVICE (All DxCh devices exclude DxCh1_Diamond)
    1.1. Call with queueIdx[7], numOfBuff[100], not NULL packetBuffs, buffLen, rxParamsPtr.
    Expected: GT_OK.
    1.2. Call with out of range queueIdx[8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with numOfBuffPtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with packetBuffs[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with buffLen[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call with rxParamsPtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U8                               queueIdx  = 0;
    GT_U32                              numOfBuff = 0;
    GT_U8                               packetBuffs[100];
    GT_U32                              buffLen[100];
    CPSS_DXCH_NET_RX_PARAMS_STC         rxParams;


    cpssOsBzero((GT_VOID*) packetBuffs, sizeof(packetBuffs));
    cpssOsBzero((GT_VOID*) buffLen, sizeof(buffLen));
    cpssOsBzero((GT_VOID*) &rxParams, sizeof(rxParams));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* check that the device is not DxCh Diamond Cut */
        if (PRV_CPSS_PP_MAC(dev)->devType != CPSS_98DX106_CNS)
        {
            /*
                1.1. Call with queueIdx[7], numOfBuff[100], not NULL packetBuffs, buffLen, rxParamsPtr.
                Expected: GT_OK.
            */
            queueIdx  = 7;
            numOfBuff = 100;

            st = cpssDxChNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, (GT_U8 **)packetBuffs, buffLen, &rxParams);
            st = (GT_NO_MORE == st) ? GT_OK : st;
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, queueIdx, numOfBuff);

            /*
                1.2. Call with out of range queueIdx[8] and other params from 1.1.
                Expected: NOT GT_OK.
            */
            queueIdx = 8;

            st = cpssDxChNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, (GT_U8 **)packetBuffs, buffLen, &rxParams);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, queueIdx);

            queueIdx = 0;

            /*
                1.3. Call with numOfBuffPtr[NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfSdmaRxPacketGet(dev, queueIdx, NULL, (GT_U8 **)packetBuffs, buffLen, &rxParams);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfBuffPtr = NULL", dev);

            /*
                1.4. Call with packetBuffs[NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, NULL, buffLen, &rxParams);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, packetBuffsPtr = NULL", dev);

            /*
                1.5. Call with buffLen[NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, (GT_U8 **)packetBuffs, NULL, &rxParams);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, buffLenPtr = NULL", dev);

            /*
                1.6. Call with rxParamsPtr[NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, (GT_U8 **)packetBuffs, buffLen, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rxParamsPtr = NULL", dev);
        }
        else
        {
            queueIdx  = 7;
            numOfBuff = 100;

            st = cpssDxChNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, (GT_U8 **)packetBuffs, buffLen, &rxParams);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_APPLICABLE_DEVICE, st, "DxCh1_Diamond DEVICE: %d", dev);
        }
    }

    queueIdx  = 7;
    numOfBuff = 100;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, (GT_U8 **)packetBuffs, buffLen, &rxParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, (GT_U8 **)packetBuffs, buffLen, &rxParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfSdmaRxCountersGet
(
    IN    GT_U8      devNum,
    IN    GT_U8      queueIdx,
    OUT   CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC *rxCountersPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfSdmaRxCountersGet)
{
/*
    ITERATE_DEVICE (All DxCh devices exclude DxCh1_Diamond)
    1.1. Call with queueIdx[7]
                   and not NULL rxCountersPtr.
    Expected: GT_OK.
    1.2. Call with out of range queueIdx[8]
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with rxCountersPtr [NULL]
                   and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8                              queueIdx = 0;
    CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC rxCounters;


    cpssOsBzero((GT_VOID*) &rxCounters, sizeof(rxCounters));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* check that the device is not DxCh Diamond Cut */
        if (PRV_CPSS_PP_MAC(dev)->devType != CPSS_98DX106_CNS)
        {
            /*
                1.1. Call with queueIdx[7]
                               and not NULL rxCountersPtr.
                Expected: GT_OK.
            */
            queueIdx = 7;

            st = cpssDxChNetIfSdmaRxCountersGet(dev, queueIdx, &rxCounters);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, queueIdx);

            /*
                1.2. Call with out of range queueIdx[8]
                               and other params from 1.1.
                Expected: NOT GT_OK.
            */
            queueIdx = 8;

            st = cpssDxChNetIfSdmaRxCountersGet(dev, queueIdx, &rxCounters);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, queueIdx);

            queueIdx = 0;

            /*
                1.3. Call with rxCountersPtr [NULL]
                               and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfSdmaRxCountersGet(dev, queueIdx, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rxCountersPtr = NULL", dev);
        }
        else
        {
            queueIdx = 7;

            st = cpssDxChNetIfSdmaRxCountersGet(dev, queueIdx, &rxCounters);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_APPLICABLE_DEVICE, st, "DxCh1_Diamond DEVICE: %d", dev);
        }

    }

    queueIdx = 7;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfSdmaRxCountersGet(dev, queueIdx, &rxCounters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfSdmaRxCountersGet(dev, queueIdx, &rxCounters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfSdmaRxErrorCountGet
(
    IN GT_U8                                    devNum,
    OUT CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC     *rxErrCountPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfSdmaRxErrorCountGet)
{
/*
    ITERATE_DEVICE (All DxCh devices exclude DxCh1_Diamond)
    1.1. Call with not NULL rxErrCountPtr;
    Expected: GT_OK.
    1.2. Call with rxErrCountPtr [NULL];
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC rxErrCount;


    cpssOsBzero((GT_VOID*) &rxErrCount, sizeof(rxErrCount));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* check that the device is not DxCh Diamond Cut */
        if (PRV_CPSS_PP_MAC(dev)->devType != CPSS_98DX106_CNS)
        {
            /*
                1.1. Call with not NULL rxErrCountPtr;
                Expected: GT_OK.
            */
            st = cpssDxChNetIfSdmaRxErrorCountGet(dev, &rxErrCount);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.2. Call with rxErrCountPtr [NULL];
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfSdmaRxErrorCountGet(dev, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rxErrCountPtr = NULL", dev);
        }
        else
        {
            st = cpssDxChNetIfSdmaRxErrorCountGet(dev, &rxErrCount);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_APPLICABLE_DEVICE, st, "DxCh1_Diamond DEVICE: %d", dev);
        }
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfSdmaRxErrorCountGet(dev, &rxErrCount);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfSdmaRxErrorCountGet(dev, &rxErrCount);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfSdmaRxQueueEnable
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           queue,
    IN  GT_BOOL                         enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfSdmaRxQueueEnable)
{
/*
    ITERATE_DEVICE (All DxCh devices exclude DxCh1_Diamond)
    1.1. Call with queue [0 / 7],
                   enable [GT_FALSE / GT_TRUE];
    Expected: GT_OK.
    1.2. Call with out of range queue [8],
                   and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       queue  = 0;
    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* check that the device is not DxCh Diamond Cut */
        if (PRV_CPSS_PP_MAC(dev)->devType != CPSS_98DX106_CNS)
        {
            /*
                1.1. Call with queue [0 / 7],
                               enable [GT_FALSE / GT_TRUE];
                Expected: GT_OK.
            */
            /* iterate with queue = 0 */
            queue  = 0;
            enable = GT_FALSE;

            st = cpssDxChNetIfSdmaRxQueueEnable(dev, queue, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, enable);

            /* iterate with queue = 7 */
            queue  = 7;
            enable = GT_TRUE;

            st = cpssDxChNetIfSdmaRxQueueEnable(dev, queue, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, enable);

            /*
                1.2. Call with out of range queue [8],
                               and other params from 1.1.
                Expected: NOT GT_OK.
            */
            queue = 8;

            st = cpssDxChNetIfSdmaRxQueueEnable(dev, queue, enable);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, queue);
        }
        else
        {
            queue  = 0;
            enable = GT_FALSE;

            st = cpssDxChNetIfSdmaRxQueueEnable(dev, queue, enable);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_APPLICABLE_DEVICE, st, "DxCh1_Diamond DEVICE: %d", dev);
        }
    }

    queue  = 0;
    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfSdmaRxQueueEnable(dev, queue, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfSdmaRxQueueEnable(dev, queue, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfSdmaTxQueueEnable
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           queue,
    IN  GT_BOOL                         enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfSdmaTxQueueEnable)
{
/*
    ITERATE_DEVICE (All DxCh devices exclude DxCh1_Diamond)
    1.1. Call with queue [0 - 7], enable [GT_FALSE / GT_TRUE];
    Expected: GT_OK.
    1.2. Call with out of range queue [8], and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U8       queue  = 0;
    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* check that the device is not DxCh Diamond Cut */
        if (PRV_CPSS_PP_MAC(dev)->devType != CPSS_98DX106_CNS)
        {
            /*
                1.1. Call with queue [0 - 7],
                               enable [GT_FALSE / GT_TRUE];
                Expected: GT_OK.
            */
            enable = GT_FALSE;

            for(queue = 0; queue < 8; queue++)
            {
                st = cpssDxChNetIfSdmaTxQueueEnable(dev, queue, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, enable);
            }

            enable = GT_TRUE;

            for(queue = 0; queue < 8; queue++)
            {
                st = cpssDxChNetIfSdmaTxQueueEnable(dev, queue, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, enable);
            }

            queue  = 0;

            /*
                1.2. Call with out of range queue [8],
                               and other params from 1.1.
                Expected: NOT GT_OK.
            */
            queue = 8;

            st = cpssDxChNetIfSdmaTxQueueEnable(dev, queue, enable);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, queue);
        }
        else
        {
            queue  = 0;
            enable = GT_FALSE;

            st = cpssDxChNetIfSdmaTxQueueEnable(dev, queue, enable);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_APPLICABLE_DEVICE, st,
                                                "DxCh1_Diamond DEVICE: %d", dev);
        }
    }

    queue  = 0;
    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfSdmaTxQueueEnable(dev, queue, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfSdmaTxQueueEnable(dev, queue, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfDsaTagBuild
(
    IN  GT_U8                          devNum,
    IN  CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr,
    OUT GT_U8                          *dsaBytesPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfDsaTagBuild)
{
/*
    ITERATE_DEVICE (DXCH)
    {dsaInfoPtr->dsaType [CPSS_DXCH_NET_DSA_CMD_TO_CPU_E}
    1.1. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_DXCH_NET_DSA_TYPE_REGULAR_E /
                                                      CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                                           dropOnSource [GT_FAIL / GT_TRUE],
                                           packetIsLooped [GT_FAIL / GT_TRUE] }
                              dsaType [CPSS_DXCH_NET_DSA_CMD_TO_CPU_E,
                              dsaInfo{toCpu {set to cpu}}
    Expected: GT_OK.
    1.2. Call with dsaInfoPtr->commonParams->dsaType[wrong enum values]and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with out of range dsaInfoPtr->commonParams.vpt [8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range dsaInfoPtr->commonParams.cfiBit [2] and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range dsaInfoPtr->commonParams.vid [4096] and other params from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with dsaInfoPtr->dsaInfo.toCpu.devNum[PRV_CPSS_MAX_PP_DEVICES_CNS] and other params from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with isEgressPipe [GT_FALSE],
                   srcIsTrunk[GT_FALSE],
                   dsaInfoPtr->dsaInfo.toCpu.interface.port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with isEgressPipe [GT_FALSE],
                   srcIsTrunk[GT_TRUE],
                   dsaInfoPtr->dsaInfo.toCpu.interface.port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                   and other params from 1.1.
    Expected: GT_OK.
    1.9. Call with dsaInfoPtr->dsaInfo.toCpu.cpuCode[wrong enum values] and other params from 1.1.
    Expected: NOT GT_OK.


    {dsaInfoPtr->dsaType [CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E}
    1.10. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_DXCH_NET_DSA_TYPE_REGULAR_E /
                                                      CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                                           dropOnSource [GT_FAIL / GT_TRUE],
                                           packetIsLooped [GT_FAIL / GT_TRUE] }
                              dsaType [CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E,
                              dsaInfo{fromCpu {set from cpu}}
    Expected: GT_OK.
    1.11. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_TRUNK_E] (not supported)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.12. Call with wrong enum values dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type
                    and other params from 1.10.
    Extended: GT_BAD_PARAM.
    1.13. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.14. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.15. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.
    1.16. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.
    1.17. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vidx [4096] (out of range)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.18. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VID_E]
                    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vidx [4096] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.
    1.21. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.
    1.22. Call with dsaInfoPtr->dsaInfo.fromCpu.dp [wrong enum values]
                    and other params from 1.10.
    Extended: GT_BAD_PARAM.
    1.23. Call with dsaInfoPtr->dsaInfo.fromCpu.srcDev [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other params from 1.10.
    Extended: NOT GT_OK.


    {dsaInfoPtr->dsaType [CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E}
    1.24. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_DXCH_NET_DSA_TYPE_REGULAR_E /
                                                      CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                                           dropOnSource [GT_FAIL / GT_TRUE],
                                           packetIsLooped [GT_FAIL / GT_TRUE] }
                              dsaType [CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E,
                              dsaInfo{toAnalyzer {set to analyzer}}
    Expected: GT_OK.
    1.25 Call with dsaInfoPtr->dsaInfo.toAnalyzer.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                   and other params from 1.24
    Expected: NOT GT_OK.
    1.26 Call with dsaInfoPtr->dsaInfo.toAnalyzer.rx_sniff [GT_TRUE] (not extended rule)
                   dsaInfoPtr->dsaInfo.toAnalyzer.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                   and other params from 1.24
    Expected: NOT GT_OK.
    1.27 Call with dsaInfoPtr->dsaInfo.toAnalyzer.rx_sniff [GT_FALSE] (rxtended rule)
                   dsaInfoPtr->dsaInfo.toAnalyzer.devPort.portNum [31] (out of range)
                   and other params from 1.24
    Expected: NOT GT_OK.

    {dsaInfoPtr->dsaType [CPSS_DXCH_NET_DSA_CMD_FORWARD_E}
    1.28. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_DXCH_NET_DSA_TYPE_REGULAR_E /
                                                      CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                                           dropOnSource [GT_FAIL / GT_TRUE],
                                           packetIsLooped [GT_FAIL / GT_TRUE] }
                              dsaType [CPSS_DXCH_NET_DSA_CMD_FORWARD_E,
                              dsaInfo{forward {set forward}}
    Expected: GT_OK.
    1.29 Call with dsaInfoPtr->dsaInfo.forward.srcDev [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                   and other params from 1.28.
    Expected: NOT GT_OK.
    1.30 Call with dsaInfoPtr->dsaInfo.forward.srcIsTrunk [GT_FALSE]
                   dsaInfoPtr->dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                   and other params from 1.28.
    Expected: NOT GT_OK.
    1.30 Call with dsaInfoPtr->dsaInfo.forward.srcIsTrunk [GT_TRUE]
                   dsaInfoPtr->dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                   and other params from 1.28.
    Expected: GT_OK.
    1.31 Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type [CPSS_INTERFACE_TRUNK_E] (not supported)
                   and other params from 1.28.
    Expected: NOT GT_OK.
    1.32 Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type [wrong enum values] (out of range)
                   and other params from 1.28.
    Expected: GT_BAD_PARAM.

    1.33. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                    and other params from 1.28.
    Extended: NOT GT_OK.
    1.34. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                    and other params from 1.28.
    Extended: NOT GT_OK.
    1.35. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.36. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.37. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.forward.dstInterface.vidx [4096] (out of range)
                    and other params from 1.28.
    Extended: NOT GT_OK.
    1.38. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VID_E]
                    dsaInfoPtr->dsaInfo.forward.dstInterface.vidx [4096] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.41. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.forward.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.

    1.42 Call with dsaInfoPtr[NULL]
                   and other params from 1.28.
    Expected: GT_BAD_PTR.
    1.43 Call with dsaBytesPtr[NULL]
                   and other params from 1.28.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U32      index = 0;
    GT_U8       dev;

    CPSS_DXCH_NET_DSA_PARAMS_STC    dsaInfo;
    GT_U8                           dsaBytes[8];

    cpssOsBzero((GT_VOID*) &dsaInfo, sizeof(dsaInfo));
    cpssOsBzero((GT_VOID*) dsaBytes, sizeof(dsaBytes));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            {dsaInfoPtr->dsaType [CPSS_DXCH_NET_DSA_CMD_TO_CPU_E}
            1.1. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_DXCH_NET_DSA_TYPE_REGULAR_E /
                                                              CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                                   dropOnSource [GT_FAIL / GT_TRUE],
                                                   packetIsLooped [GT_FAIL / GT_TRUE] }
                                      dsaType [CPSS_DXCH_NET_DSA_CMD_TO_CPU_E,
                                      dsaInfo{toCpu {set to cpu}}
            Expected: GT_OK.
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E */
        dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E;

        dsaInfo.commonParams.vpt    = 0;
        dsaInfo.commonParams.cfiBit = 0;
        dsaInfo.commonParams.vid    = 0;

        dsaInfo.commonParams.dropOnSource   = GT_FALSE;
        dsaInfo.commonParams.packetIsLooped = GT_FALSE;

        dsaInfo.dsaType = CPSS_DXCH_NET_DSA_CMD_TO_CPU_E;

        prvSetDsaInfoToCpu(&dsaInfo);

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E */
        dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;

        dsaInfo.commonParams.vpt    = 7;
        dsaInfo.commonParams.cfiBit = 1;
        dsaInfo.commonParams.vid    = 100;

        dsaInfo.commonParams.dropOnSource   = GT_TRUE;
        dsaInfo.commonParams.packetIsLooped = GT_TRUE;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with dsaInfoPtr->commonParams.dsaType[wrong enum values]
                 and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfDsaTagBuild
                            (dev, &dsaInfo, dsaBytes),
                            dsaInfo.commonParams.dsaTagType);

        /*
            1.3. Call with out of range dsaInfoPtr->commonParams.vpt [8] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        dsaInfo.commonParams.vpt = 8;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->commonParams.vpt = %d",
                                         dev, dsaInfo.commonParams.vpt);

        dsaInfo.commonParams.vpt = 7;

        /*
            1.4. Call with out of range dsaInfoPtr->commonParams.cfiBit [2] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        dsaInfo.commonParams.cfiBit = 2;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->commonParams.cfiBit = %d",
                                         dev, dsaInfo.commonParams.cfiBit);

        dsaInfo.commonParams.cfiBit = 1;

        /*
            1.5. Call with out of range dsaInfoPtr->commonParams.vid [4096] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        dsaInfo.commonParams.vid = 4096;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->commonParams.vid = %d",
                                         dev, dsaInfo.commonParams.vid);

        dsaInfo.commonParams.vid = 1;

        /*
            1.6. Call with dsaInfoPtr->dsaInfo.toCpu.devNum[PRV_CPSS_MAX_PP_DEVICES_CNS]
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        dsaInfo.dsaInfo.toCpu.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.toCpu.devNum = %d",
                                         dev, dsaInfo.dsaInfo.toCpu.devNum);

        dsaInfo.dsaInfo.toCpu.devNum = 0;

        /*
            1.8. Call with isEgressPipe [GT_FALSE],
                           srcIsTrunk[GT_FALSE],
                           dsaInfoPtr->dsaInfo.toCpu.interface.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        dsaInfo.dsaInfo.toCpu.isEgressPipe      = GT_FALSE;
        dsaInfo.dsaInfo.toCpu.srcIsTrunk        = GT_FALSE;
        dsaInfo.dsaInfo.toCpu.interface.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->.dsaInfo.toCpu.interface.portNum = %d",
                                         dev, dsaInfo.dsaInfo.toCpu.interface.portNum);

        dsaInfo.dsaInfo.toCpu.interface.portNum = NETIF_VALID_PHY_PORT_CNS;

        /*
            1.8. Call with isEgressPipe [GT_FALSE],
                           srcIsTrunk[GT_TRUE],
                           dsaInfoPtr->dsaInfo.toCpu.interface.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                           and other params from 1.1.
            Expected: GT_OK.
        */
        dsaInfo.dsaInfo.toCpu.isEgressPipe      = GT_FALSE;
        dsaInfo.dsaInfo.toCpu.srcIsTrunk        = GT_TRUE;
        dsaInfo.dsaInfo.toCpu.interface.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->.dsaInfo.toCpu.interface.portNum = %d",
                                     dev, dsaInfo.dsaInfo.toCpu.interface.portNum);

        dsaInfo.dsaInfo.toCpu.interface.portNum = NETIF_VALID_PHY_PORT_CNS;

        /*
            1.9. Call with dsaInfoPtr->dsaInfo.toCpu.cpuCode[wrong enum values]
                 and other params from 1.1.
            Expected: NOT GT_OK.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfDsaTagBuild
                            (dev, &dsaInfo, dsaBytes),
                            dsaInfo.dsaInfo.toCpu.cpuCode);

        /*
            {dsaInfoPtr->dsaType [CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E}
            1.10. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_DXCH_NET_DSA_TYPE_REGULAR_E /
                                                              CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                                   dropOnSource [GT_FAIL / GT_TRUE],
                                                   packetIsLooped [GT_FAIL / GT_TRUE] }
                                      dsaType [CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E,
                                      dsaInfo{fromCpu {set from cpu}}
            Expected: GT_OK.
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E */
        dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E;

        dsaInfo.commonParams.vpt    = 0;
        dsaInfo.commonParams.cfiBit = 0;
        dsaInfo.commonParams.vid    = 0;

        dsaInfo.commonParams.dropOnSource   = GT_FALSE;
        dsaInfo.commonParams.packetIsLooped = GT_FALSE;

        dsaInfo.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;

        prvSetDsaInfoFromCpu(&dsaInfo);

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E */
        dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;

        dsaInfo.commonParams.vpt    = 7;
        dsaInfo.commonParams.cfiBit = 1;
        dsaInfo.commonParams.vid    = 100;

        dsaInfo.commonParams.dropOnSource   = GT_TRUE;
        dsaInfo.commonParams.packetIsLooped = GT_TRUE;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.11. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_TRUNK_E] (not supported)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d",
                                        dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type);

        dsaInfo.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        /*
            1.12. Call with wrong enum values dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type
                            and other params from 1.10.
            Extended: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfDsaTagBuild
                            (dev, &dsaInfo, dsaBytes),
                            dsaInfo.dsaInfo.fromCpu.dstInterface.type);

        /*
            1.13. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type           = CPSS_INTERFACE_PORT_E;
        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.devNum = %d",
                                        dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type, dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.devNum);

        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.devNum = 0;

        /*
            1.14. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type            = CPSS_INTERFACE_PORT_E;
        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.portNum = %d",
                                        dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type, dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum);

        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum = NETIF_VALID_PHY_PORT_CNS;

        /*
            1.15. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type           = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.devNum = %d",
                                        dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type, dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.devNum);

        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.devNum = 0;

        /*
            1.16. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type            = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.portNum = %d",
                                        dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type, dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum);

        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum = NETIF_VALID_PHY_PORT_CNS;

        /*
            1.17. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vidx [4096] (out of range)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.fromCpu.dstInterface.vidx = 4096;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vidx = %d",
                                        dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type, dsaInfo.dsaInfo.fromCpu.dstInterface.vidx);

        dsaInfo.dsaInfo.fromCpu.dstInterface.vidx = 100;

        /*
            1.18. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VID_E]
                            dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vidx [4096] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VID_E;
        dsaInfo.dsaInfo.fromCpu.dstInterface.vidx = 4096;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vidx = %d",
                                    dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type, dsaInfo.dsaInfo.fromCpu.dstInterface.vidx);

        dsaInfo.dsaInfo.fromCpu.dstInterface.vidx = 100;

        /*
            1.21. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type   = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.fromCpu.dstInterface.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vlanId = %d",
                                     dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type, dsaInfo.dsaInfo.fromCpu.dstInterface.vlanId);

        dsaInfo.dsaInfo.fromCpu.dstInterface.vlanId = 100;

        /*
            1.22. Call with dsaInfoPtr->dsaInfo.fromCpu.dp [wrong enum values]
                            and other params from 1.10.
            Extended: GT_BAD_PARAM. for DxCh3 - GT_OK(not relevant).
        */

        if (PRV_CPSS_PP_MAC(dev)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
        {
           for(index = 0; index < utfInvalidEnumArrSize; index++)
           {
                dsaInfo.dsaInfo.fromCpu.dp = utfInvalidEnumArr[index];

                st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dp = %d",
                                             dev, dsaInfo.dsaInfo.fromCpu.dp);
            }
        }
        else
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChNetIfDsaTagBuild
                                (dev, &dsaInfo, dsaBytes),
                                dsaInfo.dsaInfo.fromCpu.dp);
        }


        dsaInfo.dsaInfo.fromCpu.dp = CPSS_DP_GREEN_E;

        /*
            1.23. Call with dsaInfoPtr->dsaInfo.fromCpu.srcDev [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.srcDev = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.srcDev = %d",
                                         dev, dsaInfo.dsaInfo.fromCpu.srcDev);

        dsaInfo.dsaInfo.fromCpu.srcDev = 0;

        /*
            {dsaInfoPtr->dsaType [CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E}
            1.24. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_DXCH_NET_DSA_TYPE_REGULAR_E /
                                                              CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                                   dropOnSource [GT_FAIL / GT_TRUE],
                                                   packetIsLooped [GT_FAIL / GT_TRUE] }
                                      dsaType [CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E,
                                      dsaInfo{toAnalyzer {set to analyzer}}
            Expected: GT_OK.
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E */
        dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E;

        dsaInfo.commonParams.vpt    = 0;
        dsaInfo.commonParams.cfiBit = 0;
        dsaInfo.commonParams.vid    = 0;

        dsaInfo.commonParams.dropOnSource   = GT_FALSE;
        dsaInfo.commonParams.packetIsLooped = GT_FALSE;

        dsaInfo.dsaType = CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E;

        prvSetDsaInfoToAnalizer(&dsaInfo);

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E */
        dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;

        dsaInfo.commonParams.vpt    = 7;
        dsaInfo.commonParams.cfiBit = 1;
        dsaInfo.commonParams.vid    = 100;

        dsaInfo.commonParams.dropOnSource   = GT_TRUE;
        dsaInfo.commonParams.packetIsLooped = GT_TRUE;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.25 Call with dsaInfoPtr->dsaInfo.toAnalyzer.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                           and other params from 1.24
            Expected: NOT GT_OK.
        */
        dsaInfo.dsaInfo.toAnalyzer.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfo.dsaInfo.toAnalyzer.devPort.devNum = %d",
                                    dev, dsaInfo.dsaInfo.toAnalyzer.devPort.devNum);

        dsaInfo.dsaInfo.toAnalyzer.devPort.devNum = 0;

        /*
            1.26 Call with dsaInfoPtr->dsaInfo.toAnalyzer.rxSniffer [GT_TRUE] (not extended rule)
                           dsaInfoPtr->dsaInfo.toAnalyzer.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                           and other params from 1.24
            Expected: NOT GT_OK.
        */
        dsaInfo.dsaInfo.toAnalyzer.rxSniffer       = GT_TRUE;
        dsaInfo.dsaInfo.toAnalyzer.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.toAnalyzer.rxSniffer = %d, dsaInfo.dsaInfo.toAnalyzer.devPort.portNum = %d",
                                    dev, dsaInfo.dsaInfo.toAnalyzer.rxSniffer, dsaInfo.dsaInfo.toAnalyzer.devPort.portNum);

        dsaInfo.dsaInfo.toAnalyzer.devPort.portNum = 0;

        /*
            1.27 Call with dsaInfo.commonParams.dsaTagType [CPSS_DXCH_NET_DSA_TYPE_REGULAR_E] (rxtended rule)
                           dsaInfoPtr->dsaInfo.toAnalyzer.devPort.portNum [31] (out of range)
                           and other params from 1.24
            Expected: NOT GT_OK.
        */
        dsaInfo.commonParams.dsaTagType            = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E;
        dsaInfo.dsaInfo.toAnalyzer.devPort.portNum = 32;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->commonParams.dsaTagType = %d, dsaInfo.dsaInfo.toAnalyzer.devPort.portNum = %d",
                                    dev, dsaInfo.commonParams.dsaTagType, dsaInfo.dsaInfo.toAnalyzer.devPort.portNum);

        dsaInfo.dsaInfo.toAnalyzer.devPort.portNum = 0;

        /*
            {dsaInfoPtr->dsaType [CPSS_DXCH_NET_DSA_CMD_FORWARD_E}
            1.28. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_DXCH_NET_DSA_TYPE_REGULAR_E /
                                                              CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                                   dropOnSource [GT_FAIL / GT_TRUE],
                                                   packetIsLooped [GT_FAIL / GT_TRUE] }
                                      dsaType [CPSS_DXCH_NET_DSA_CMD_FORWARD_E,
                                      dsaInfo{forward {set forward}}
            Expected: GT_OK.
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E */
        dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E;

        dsaInfo.commonParams.vpt    = 0;
        dsaInfo.commonParams.cfiBit = 0;
        dsaInfo.commonParams.vid    = 0;

        dsaInfo.commonParams.dropOnSource   = GT_FALSE;
        dsaInfo.commonParams.packetIsLooped = GT_FALSE;

        dsaInfo.dsaType = CPSS_DXCH_NET_DSA_CMD_FORWARD_E;

        prvSetDsaInfoForward(&dsaInfo);

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E */
        dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;

        dsaInfo.commonParams.vpt    = 7;
        dsaInfo.commonParams.cfiBit = 1;
        dsaInfo.commonParams.vid    = 100;

        dsaInfo.commonParams.dropOnSource   = GT_TRUE;
        dsaInfo.commonParams.packetIsLooped = GT_TRUE;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.29 Call with dsaInfoPtr->dsaInfo.forward.srcDev [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                           and other params from 1.28.
            Expected: NOT GT_OK.
        */
        dsaInfo.dsaInfo.forward.srcDev = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.srcDev = %d",
                                         dev, dsaInfo.dsaInfo.forward.srcDev);

        dsaInfo.dsaInfo.forward.srcDev = 0;

        /*
            1.30 Call with dsaInfoPtr->dsaInfo.forward.srcIsTrunk [GT_FALSE]
                           dsaInfoPtr->dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                           and other params from 1.28.
            Expected: NOT GT_OK.
        */
        dsaInfo.dsaInfo.forward.srcIsTrunk     = GT_FALSE;
        dsaInfo.dsaInfo.forward.source.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.srcIsTrunk = %d, dsaInfoPtr->dsaInfo.forward.source.portNum = %d",
                                        dev, dsaInfo.dsaInfo.forward.srcIsTrunk, dsaInfo.dsaInfo.forward.source.portNum);

        dsaInfo.dsaInfo.forward.source.portNum = 0;

        /*
            1.30 Call with dsaInfoPtr->dsaInfo.forward.srcIsTrunk [GT_TRUE]
                           dsaInfoPtr->dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                           and other params from 1.28.
            Expected: GT_OK.
        */
        dsaInfo.dsaInfo.forward.srcIsTrunk     = GT_TRUE;
        dsaInfo.dsaInfo.forward.source.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.srcIsTrunk = %d, dsaInfoPtr->dsaInfo.forward.source.portNum = %d",
                                    dev, dsaInfo.dsaInfo.forward.srcIsTrunk, dsaInfo.dsaInfo.forward.source.portNum);

        dsaInfo.dsaInfo.forward.source.portNum = 0;

        /*
            1.31 Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type [CPSS_INTERFACE_TRUNK_E] (not supported)
                           and other params from 1.28.
            Expected: NOT GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type  = CPSS_INTERFACE_TRUNK_E;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d",
                                        dev, dsaInfo.dsaInfo.forward.dstInterface.type);

        dsaInfo.dsaInfo.forward.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        /*
            1.32 Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type [wrong enum values]
                           and other params from 1.28.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfDsaTagBuild
                            (dev, &dsaInfo, dsaBytes),
                            dsaInfo.dsaInfo.forward.dstInterface.type);

        /*
            1.33. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                            and other params from 1.28.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type           = CPSS_INTERFACE_PORT_E;
        dsaInfo.dsaInfo.forward.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum = %d",
                                        dev, dsaInfo.dsaInfo.forward.dstInterface.type, dsaInfo.dsaInfo.forward.dstInterface.devPort.devNum);

        dsaInfo.dsaInfo.forward.dstInterface.devPort.devNum = 0;

        /*
            1.34. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                            and other params from 1.28.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type            = CPSS_INTERFACE_PORT_E;
        dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum = %d",
                                        dev, dsaInfo.dsaInfo.forward.dstInterface.type, dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum);

        dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum = 0;

        /*
            1.35. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type           = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.forward.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum = %d",
                                     dev, dsaInfo.dsaInfo.forward.dstInterface.type, dsaInfo.dsaInfo.forward.dstInterface.devPort.devNum);

        dsaInfo.dsaInfo.forward.dstInterface.devPort.devNum = 0;

        /*
            1.36. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type            = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum = %d",
                                     dev, dsaInfo.dsaInfo.forward.dstInterface.type, dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum);

        dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum = 0;

        /*
            1.37. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.forward.dstInterface.vidx [4096] (out of range)
                            and other params from 1.28.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.forward.dstInterface.vidx = 4096;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.vidx = %d",
                                         dev, dsaInfo.dsaInfo.forward.dstInterface.type, dsaInfo.dsaInfo.forward.dstInterface.vidx);

        dsaInfo.dsaInfo.forward.dstInterface.vidx = 0;

        /*
            1.38. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VID_E]
                            dsaInfoPtr->dsaInfo.forward.dstInterface.vidx [4096] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type = CPSS_INTERFACE_VID_E;
        dsaInfo.dsaInfo.forward.dstInterface.vidx = 4096;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.vidx = %d",
                                     dev, dsaInfo.dsaInfo.forward.dstInterface.type, dsaInfo.dsaInfo.forward.dstInterface.vidx);

        dsaInfo.dsaInfo.forward.dstInterface.vidx = 0;

        /*
            1.41. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.forward.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type   = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.forward.dstInterface.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.vlanId = %d",
                                         dev, dsaInfo.dsaInfo.forward.dstInterface.type, dsaInfo.dsaInfo.forward.dstInterface.vlanId);

        dsaInfo.dsaInfo.forward.dstInterface.vlanId = 100;

        /*
            1.42 Call with dsaInfoPtr[NULL]
                           and other params from 1.28.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfDsaTagBuild(dev, NULL, dsaBytes);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dsaInfoPtr = NULL", dev);

        /*
            1.43 Call with dsaBytesPtr[NULL]
                           and other params from 1.28.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dsaBytesPtr = NULL", dev);
    }

    dsaInfo.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E;

    dsaInfo.commonParams.vpt    = 0;
    dsaInfo.commonParams.cfiBit = 0;
    dsaInfo.commonParams.vid    = 0;

    dsaInfo.commonParams.dropOnSource   = GT_FALSE;
    dsaInfo.commonParams.packetIsLooped = GT_FALSE;

    dsaInfo.dsaType = CPSS_DXCH_NET_DSA_CMD_TO_CPU_E;

    prvSetDsaInfoToCpu(&dsaInfo);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfDuplicateEnableSet
(
    IN  GT_U8                          devNum,
    IN  GT_BOOL                        enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfDuplicateEnableSet)
{
/*
    ITERATE DEVICE (DxCh2 and above)
    1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* iterate with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssDxChNetIfDuplicateEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* iterate with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxChNetIfDuplicateEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfDuplicateEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfDuplicateEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfPortDuplicateToCpuSet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    IN  GT_BOOL                        enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfPortDuplicateToCpuSet)
{
/*
    ITERATE_DEVICE_PHY_CPU_PORT (DxCh2 and above)
    1.1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
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
            /* iterate with enable = GT_FALSE */
            enable = GT_FALSE;

            st = cpssDxChNetIfPortDuplicateToCpuSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /* iterate with enable = GT_TRUE */
            enable = GT_TRUE;

            st = cpssDxChNetIfPortDuplicateToCpuSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChNetIfPortDuplicateToCpuSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChNetIfPortDuplicateToCpuSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChNetIfPortDuplicateToCpuSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_FALSE;

    port = NETIF_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfPortDuplicateToCpuSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfPortDuplicateToCpuSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpSynSet
(
    IN  GT_U8                          devNum,
    IN  GT_BOOL                        enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfAppSpecificCpuCodeTcpSynSet)
{
/*
    ITERATE_DEVICE (DxCh2 and above)
    1.1. Call with enable[GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChNetIfAppSpecificCpuCodeTcpSynGet with not NULL enablePtr
    Expected: GT_OK and the same enablePtr as was et.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable[GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* iterate with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssDxChNetIfAppSpecificCpuCodeTcpSynSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChNetIfAppSpecificCpuCodeTcpSynGet with not NULL enablePtr
            Expected: GT_OK.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeTcpSynGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChNetIfAppSpecificCpuCodeTcpSynGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set %d", dev);

        /* iterate with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxChNetIfAppSpecificCpuCodeTcpSynSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChNetIfAppSpecificCpuCodeTcpSynGet with not NULL enablePtr
            Expected: GT_OK.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeTcpSynGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChNetIfAppSpecificCpuCodeTcpSynGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfAppSpecificCpuCodeTcpSynSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfAppSpecificCpuCodeTcpSynSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpSynGet
(
    IN  GT_U8                          devNum,
    OUT GT_BOOL                        *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfAppSpecificCpuCodeTcpSynGet)
{
/*
    ITERATE_DEVICE (DxCh2 and above)
    1.1. Call with not NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeTcpSynGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeTcpSynGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfAppSpecificCpuCodeTcpSynGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfAppSpecificCpuCodeTcpSynGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet
(
    IN GT_U8            devNum,
    IN GT_U32           index,
    IN GT_U8            protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT   cpuCode
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet)
{
/*
    ITERATE_DEVICE (DXCH2 and above)
    1.1. Call with index[0 / 7],
                   protocol[0 / 255],
                   cpuCode[CPSS_NET_TCP_SYN_TO_CPU_E / CPSS_NET_MIRROR_IPV6_UC_ICMP_REDIRECT_E].
    Expected: GT_Ok.
    1.2. Call cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet with index [0 / 7]
              and not NULL validPtr, protocolPtr, cpuCodePtr.
    Expected: GT_Ok and the same protocolPtr, cpuCodePtr as was set.
    1.3. Call with out of range  index[8] and other parameters from 1.1.
    Expected: NOT GT_Ok.
    1.4. Call with wrong enum values cpuCode and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5 Call cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate with index[0 / 7].
    Expected: GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                      index;
    GT_U8                       protocol;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode;
    GT_U8                       protocolGet;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCodeGet;
    GT_BOOL                     valid;


    index       = 0;
    protocol    = 0;
    protocolGet = 0;
    valid       = GT_FALSE;
    cpuCode     = CPSS_NET_ALL_CPU_OPCODES_E;
    cpuCodeGet  = CPSS_NET_ALL_CPU_OPCODES_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index[0 / 7],
                           protocol[0 / 255],
                           cpuCode[CPSS_NET_TCP_SYN_TO_CPU_E / CPSS_NET_MIRROR_IPV6_UC_ICMP_REDIRECT_E].
            Expected: GT_Ok.
        */
        /* iterate with index = 0 */
        index    = 0;
        protocol = 0;
        cpuCode  = CPSS_NET_TCP_SYN_TO_CPU_E;

        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet(dev, index, protocol, cpuCode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, protocol, cpuCode);

        /*
            1.2. Call cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet with index [0 / 7]
                      and not NULL validPtr, protocolPtr, cpuCodePtr.
            Expected: GT_Ok and the same protocolPtr, cpuCodePtr as was set.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet(dev, index, &valid, &protocolGet, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet: %d, %d", dev, index);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(protocol, protocolGet, "got another protocol than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet, "got another cpuCode than was set: %d", dev);

        /* iterate with index = 7 */
        index    = 7;
        protocol = 255;
        cpuCode  = CPSS_NET_MIRROR_IPV6_UC_ICMP_REDIRECT_E;

        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet(dev, index, protocol, cpuCode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, index, protocol, cpuCode);

        /*
            1.2. Call cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet with index [0 / 7]
                      and not NULL validPtr, protocolPtr, cpuCodePtr.
            Expected: GT_Ok and the same protocolPtr, cpuCodePtr as was set.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet(dev, index, &valid, &protocolGet, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet: %d, %d", dev, index);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(protocol, protocolGet, "got another protocol than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet, "got another cpuCode than was set: %d", dev);

        /*
            1.3. Call with index[8] and other parameters from 1.1.
            Expected: NOT GT_Ok.
        */
        index = 8;

        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet(dev, index, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 7;

        /*
            1.4. Call with cpuCode[wrong enum values] and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet
                            (dev, index, protocol, cpuCode),
                            cpuCode);

        /*
            1.5 Call cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate with index[0 / 7].
            Expected: GT_OK.
        */
        /* iterate with index = 0 */
        index = 0;

        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate: %d, %d", dev, index);

        /* iterate with index = 7 */
        index = 7;

        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate: %d, %d", dev, index);
    }

    index    = 0;
    protocol = 0;
    cpuCode  = CPSS_NET_UN_REGISTERD_MC_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet(dev, index, protocol, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet(dev, index, protocol, cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate
(
    IN GT_U8            devNum,
    IN GT_U32           index
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate)
{
/*
    ITERATE_DEVICE (DXCH2 and above)
    1.1. Call with index[0 / 7].
    Expected: GT_OK.
    1.2. Call with out of range index[8].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      index;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index[0 / 7].
            Expected: GT_OK.
        */
        /* iterate with index = 0 */
        index = 0;

        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* iterate with index = 7 */
        index = 7;

        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call with index[8].
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate(dev, index);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate(dev, index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate(dev, index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet
(
    IN GT_U8            devNum,
    IN GT_U32           index,
    OUT GT_BOOL         *validPtr,
    OUT GT_U8           *protocolPtr,
    OUT CPSS_NET_RX_CPU_CODE_ENT   *cpuCodePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet)
{
/*
    ITERATE_DEVICE (DXCH2 and above)
    1.1. Call with index [0 / 7] and not NULL validPtr, protocolPtr, cpuCodePtr.
    Expected: GT_OK.
    1.2. Call with out of range index [8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with validPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with protocolPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with cpuCodePtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      index    = 0;
    GT_BOOL     valid    = GT_FALSE;
    GT_U8       protocol = 0;

    CPSS_NET_RX_CPU_CODE_ENT   cpuCode;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 7] and not NULL validPtr, protocolPtr, cpuCodePtr.
            Expected: GT_OK.
        */
        /* iterate with index = 0 */
        index = 0;

        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet(dev, index, &valid, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* iterate with index = 7 */
        index = 7;

        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet(dev, index, &valid, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call with index [8] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet(dev, index, &valid, &protocol, &cpuCode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 7;

        /*
            1.3. Call with validPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet(dev, index, NULL, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);

        /*
            1.4. Call with protocolPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet(dev, index, &valid, NULL, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, protocolPtr = NULL", dev);

        /*
            1.5. Call with cpuCodePtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet(dev, index, &valid, &protocol, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cpuCodePtr = NULL", dev);

    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet(dev, index, &valid, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet(dev, index, &valid, &protocol, &cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet
(
    IN GT_U8                            devNum,
    IN GT_U32                           rangeIndex,
    IN GT_U16                           minDstPort,
    IN GT_U16                           maxDstPort,
    IN CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetType,
    IN CPSS_NET_PROT_ENT                protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT         cpuCode
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet)
{
/*
    ITERATE_DEVICE (DXCH2 and above)
    1.1. Call with rangeIndex[0 / 15],
                   minDstPort[0],
                   maxDstPort[63],
                   packetType[CPSS_NET_TCP_UDP_PACKET_UC_E / CPSS_NET_TCP_UDP_PACKET_MC_E],
                   protocol[CPSS_NET_PROT_UDP_E / CPSS_NET_PROT_TCP_E,],
                   cpuCode[CPSS_NET_UN_REGISTERD_MC_E / CPSS_NET_INTERVENTION_DA_E]
    Expected: GT_OK.
    1.2. Call cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet with rangeIndex[0 / 15]
              and not NULL validPtr, rangeIndexPtr, minDstPortPtr, maxDstPortPtr, packetTypePtr, protocolPtr, cpuCodePtr.
    Expected: GT_OK and the same rangeIndexPtr, minDstPortPtr, maxDstPortPtr, packetTypePtr, protocolPtr, cpuCodePtr as was set.
    1.3. Call with out of range rangeIndex[16] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with wrong enum values packetType and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with wrong enum values protocol and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with wrong enum values cpuCode  and other params from 1.1.
    Expected: NOT GT_OK.
    1.7. Call cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate with rangeIndex[0 / 15]
    Expected: GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                           rangeIndex    = 0;
    GT_U16                           minDstPort    = 0;
    GT_U16                           maxDstPort    = 0;
    CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetType    = CPSS_NET_TCP_UDP_PACKET_UC_E;
    CPSS_NET_PROT_ENT                protocol      = CPSS_NET_PROT_UDP_E;
    CPSS_NET_RX_CPU_CODE_ENT         cpuCode       = CPSS_NET_UN_REGISTERD_MC_E;
    GT_U16                           minDstPortGet = 0;
    GT_U16                           maxDstPortGet = 0;
    CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetTypeGet = CPSS_NET_TCP_UDP_PACKET_UC_E;
    CPSS_NET_PROT_ENT                protocolGet   = CPSS_NET_PROT_UDP_E;
    CPSS_NET_RX_CPU_CODE_ENT         cpuCodeGet    = CPSS_NET_UN_REGISTERD_MC_E;
    GT_BOOL                          valid         = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rangeIndex[0 / 15],
                           minDstPort[0],
                           maxDstPort[63],
                           packetType[CPSS_NET_TCP_UDP_PACKET_UC_E / CPSS_NET_TCP_UDP_PACKET_MC_E],
                           protocol[CPSS_NET_PROT_UDP_E / CPSS_NET_PROT_TCP_E,],
                           cpuCode[CPSS_NET_TCP_SYN_TO_CPU_E / CPSS_NET_MIRROR_IPV6_UC_ICMP_REDIRECT_E]
            Expected: GT_OK.
        */
        /* iterate with rangeIndex = 0 */
        rangeIndex = 0;
        minDstPort = 0;
        maxDstPort = 0;
        packetType = CPSS_NET_TCP_UDP_PACKET_UC_E;
        protocol   = CPSS_NET_PROT_UDP_E;
        cpuCode    = CPSS_NET_TCP_SYN_TO_CPU_E;

        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort, packetType, protocol, cpuCode);
        UTF_VERIFY_EQUAL7_PARAM_MAC(GT_OK, st, dev, rangeIndex, minDstPort, maxDstPort, packetType, protocol, cpuCode);

        /*
            1.2. Call cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet with rangeIndex[0 / 15]
                      and not NULL validPtr, rangeIndexPtr, minDstPortPtr, maxDstPortPtr, packetTypePtr, protocolPtr, cpuCodePtr.
            Expected: GT_OK and the same rangeIndexPtr, minDstPortPtr, maxDstPortPtr, packetTypePtr, protocolPtr, cpuCodePtr as was set.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &valid, &minDstPortGet, &maxDstPortGet, &packetTypeGet, &protocolGet, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet: %d, %d", dev, rangeIndex);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(minDstPort, minDstPortGet, "got another minDstPort than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(maxDstPort, maxDstPortGet, "got another maxDstPort than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(packetType, packetTypeGet, "got another packetType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(protocol, protocolGet, "got another protocol than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet, "got another cpuCode than was set: %d", dev);

        /* iterate with rangeIndex = 15 */
        rangeIndex = 15;
        minDstPort = 0;
        maxDstPort = 0;
        packetType = CPSS_NET_TCP_UDP_PACKET_MC_E;
        protocol   = CPSS_NET_PROT_TCP_E;
        cpuCode    = CPSS_NET_MIRROR_IPV6_UC_ICMP_REDIRECT_E;

        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort, packetType, protocol, cpuCode);
        UTF_VERIFY_EQUAL7_PARAM_MAC(GT_OK, st, dev, rangeIndex, minDstPort, maxDstPort, packetType, protocol, cpuCode);

        /*
            1.2. Call cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet with rangeIndex[0 / 15]
                      and not NULL validPtr, rangeIndexPtr, minDstPortPtr, maxDstPortPtr, packetTypePtr, protocolPtr, cpuCodePtr.
            Expected: GT_OK and the same rangeIndexPtr, minDstPortPtr, maxDstPortPtr, packetTypePtr, protocolPtr, cpuCodePtr as was set.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &valid, &minDstPortGet, &maxDstPortGet, &packetTypeGet, &protocolGet, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet: %d, %d", dev, rangeIndex);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(minDstPort, minDstPortGet, "got another minDstPort than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(maxDstPort, maxDstPortGet, "got another maxDstPort than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(packetType, packetTypeGet, "got another packetType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(protocol, protocolGet, "got another protocol than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet, "got another cpuCode than was set: %d", dev);

        /*
            1.3. Call with out of range rangeIndex[16] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        rangeIndex = 16;

        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort, packetType, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rangeIndex);

        rangeIndex = 15;

        /*
            1.4. Call with wrong enum values packetType and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet
                            (dev, rangeIndex, minDstPort, maxDstPort,
                             packetType, protocol, cpuCode),
                            packetType);

        /*
            1.5. Call with wrong enum values protocol and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet
                            (dev, rangeIndex, minDstPort, maxDstPort,
                             packetType, protocol, cpuCode),
                            protocol);

        /*
            1.6. Call with wrong enum values cpuCode  and other params from 1.1.
            Expected: NOT GT_OK.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet
                            (dev, rangeIndex, minDstPort, maxDstPort,
                             packetType, protocol, cpuCode),
                            cpuCode);

        /*
            1.7. Call cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate with rangeIndex[0 / 15]
            Expected: GT_OK.
        */
        /* iterte with rangeIndex = 0 */
        rangeIndex = 0;

        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate: %d, %d", dev, rangeIndex);

        /* iterte with rangeIndex = 15 */
        rangeIndex = 15;

        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate: %d, %d", dev, rangeIndex);
    }

    rangeIndex = 0;
    minDstPort = 0;
    maxDstPort = 0;
    packetType = CPSS_NET_TCP_UDP_PACKET_UC_E;
    protocol   = CPSS_NET_PROT_UDP_E;
    cpuCode    = CPSS_NET_UN_REGISTERD_MC_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort, packetType, protocol, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet(dev, rangeIndex, minDstPort, maxDstPort, packetType, protocol, cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate
(
    IN GT_U8                            devNum,
    IN GT_U32                           rangeIndex
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate)
{
/*
    ITERATE_DEVICE (DXCH2 and above)
    1.1. Call with rangeIndex [0 / 15].
    Expected: GT_OK.
    1.2. Call with out of range rangeIndex [16].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      rangeIndex = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rangeIndex [0 / 15].
            Expected: GT_OK.
        */
        /* iterate with rangeIndex = 0 */
        rangeIndex = 0;

        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rangeIndex);

        /* iterate with rangeIndex = 15 */
        rangeIndex = 15;

        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rangeIndex);

        /*
            1.2. Call with out of range rangeIndex [16].
            Expected: NOT GT_OK.
        */
        rangeIndex = 16;

        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rangeIndex);
    }

    rangeIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(dev, rangeIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet
(
    IN GT_U8                             devNum,
    IN GT_U32                            rangeIndex,
    OUT GT_BOOL                          *validPtr,
    OUT GT_U16                           *minDstPortPtr,
    OUT GT_U16                           *maxDstPortPtr,
    OUT CPSS_NET_TCP_UDP_PACKET_TYPE_ENT *packetTypePtr,
    OUT CPSS_NET_PROT_ENT                *protocolPtr,
    OUT CPSS_NET_RX_CPU_CODE_ENT         *cpuCodePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet)
{
/*
    ITERATE_DEVICE (DXCH2 and above)
    1.1. Call with rangeIndex[0 / 15] and not NULL validPtr, minDstPortPtr, maxDstPortPtr, packetTypePtr, protocolPtr, cpuCodePtr.
    Expected: GT_OK.
    1.2. Call with out of range rangeIndex[16] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with validPtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with minDstPortPtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with maxDstPortPtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call with packetTypePtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.7. Call with protocolPtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.8. Call with cpuCodePtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                           rangeIndex    = 0;
    GT_U16                           minDstPort    = 0;
    GT_U16                           maxDstPort    = 0;
    CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetType    = CPSS_NET_TCP_UDP_PACKET_UC_E;
    CPSS_NET_PROT_ENT                protocol      = CPSS_NET_PROT_UDP_E;
    CPSS_NET_RX_CPU_CODE_ENT         cpuCode       = CPSS_NET_UN_REGISTERD_MC_E;
    GT_BOOL                          valid         = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rangeIndex[0 / 15] and not NULL validPtr, minDstPortPtr, maxDstPortPtr, packetTypePtr, protocolPtr, cpuCodePtr.
            Expected: GT_OK.
        */
        /* iterate with rangeIndex = 0 */
        rangeIndex = 0;

        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &valid, &minDstPort, &maxDstPort, &packetType, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rangeIndex);

        /* iterate with rangeIndex = 15 */
        rangeIndex = 15;

        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &valid, &minDstPort, &maxDstPort, &packetType, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rangeIndex);

        /*
            1.2. Call with out of range rangeIndex[16] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        rangeIndex = 16;

        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &valid, &minDstPort, &maxDstPort, &packetType, &protocol, &cpuCode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rangeIndex);

        /*
            1.3. Call with validPtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, NULL, &minDstPort, &maxDstPort, &packetType, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);

        /*
            1.4. Call with minDstPortPtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &valid, NULL, &maxDstPort, &packetType, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, minDstPortPtr = NULL", dev);

        /*
            1.5. Call with maxDstPortPtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &valid, &minDstPort, NULL, &packetType, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maxDstPortPtr = NULL", dev);

        /*
            1.6. Call with packetTypePtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &valid, &minDstPort, &maxDstPort, NULL, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, packetTypePtr = NULL", dev);

        /*
            1.7. Call with protocolPtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &valid, &minDstPort, &maxDstPort, &packetType, NULL, &cpuCode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, protocolPtr = NULL", dev);

        /*
            1.8. Call with cpuCodePtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &valid, &minDstPort, &maxDstPort, &packetType, &protocol, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cpuCodePtr = NULL", dev);
    }

    rangeIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &valid, &minDstPort, &maxDstPort, &packetType, &protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(dev, rangeIndex, &valid, &minDstPort, &maxDstPort, &packetType, &protocol, &cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeIpLinkLocalProtSet
(
    IN GT_U8                      devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT ipVer,
    IN GT_U8                      protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT   cpuCode
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeIpLinkLocalProtSet)
{
/*
    ITERATE_DEVICE (DXCH2 and above)
    1.1. Call with ipVer[CPSS_IP_PROTOCOL_IPV4_E, CPSS_IP_PROTOCOL_IPV6_E], protocol[0], cpuCode[CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E / CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E]
    Expected: GT_OK.
    1.2. Call cpssDxChNetIfCpuCodeIpLinkLocalProtGet with not NULL cpuCodePtr and other params from 1.1.
    Expected: GT_OK and the same cpuCode as was set.
    1.3. Call with wrong enum values ipVer and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong enum values cpuCode and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with cpuCode[CPSS_NET_CONTROL_DEST_MAC_TRAP_E] and other params from 1.1. (not supported)
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_IP_PROTOCOL_STACK_ENT    ipVer      = CPSS_IP_PROTOCOL_IPV4_E;
    GT_U8                         protocol   = 0;
    CPSS_NET_RX_CPU_CODE_ENT      cpuCode    = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
    CPSS_NET_RX_CPU_CODE_ENT      cpuCodeGet = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ipVer[CPSS_IP_PROTOCOL_IPV4_E, CPSS_IP_PROTOCOL_IPV6_E], protocol[0], cpuCode[CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E / CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E]
            Expected: GT_OK.
        */
        /* iterate with ipVer = CPSS_IP_PROTOCOL_IPV4_E */
        ipVer    = CPSS_IP_PROTOCOL_IPV4_E;
        protocol = 0;
        cpuCode  = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;

        st = cpssDxChNetIfCpuCodeIpLinkLocalProtSet(dev, ipVer, protocol, cpuCode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ipVer, protocol, cpuCode);

        /*
            1.2. Call cpssDxChNetIfCpuCodeIpLinkLocalProtGet with not NULL cpuCodePtr and other params from 1.1.
            Expected: GT_OK and the same cpuCode as was set.
        */
        st = cpssDxChNetIfCpuCodeIpLinkLocalProtGet(dev, ipVer, protocol, &cpuCodeGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeIpLinkLocalProtGet: %d, %d, %d", dev, ipVer, protocol);

        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet, "got another cpuCode than was set: %d", dev);

        /* iterate with ipVer = CPSS_IP_PROTOCOL_IPV6_E */
        ipVer    = CPSS_IP_PROTOCOL_IPV6_E;
        protocol = 0;
        cpuCode  = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E;

        st = cpssDxChNetIfCpuCodeIpLinkLocalProtSet(dev, ipVer, protocol, cpuCode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ipVer, protocol, cpuCode);

        /*
            1.2. Call cpssDxChNetIfCpuCodeIpLinkLocalProtGet with not NULL cpuCodePtr and other params from 1.1.
            Expected: GT_OK and the same cpuCode as was set.
        */
        st = cpssDxChNetIfCpuCodeIpLinkLocalProtGet(dev, ipVer, protocol, &cpuCodeGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeIpLinkLocalProtGet: %d, %d, %d", dev, ipVer, protocol);

        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet, "got another cpuCode than was set: %d", dev);

        /*
            1.3. Call with wrong enum values ipVer and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfCpuCodeIpLinkLocalProtSet
                            (dev, ipVer, protocol, cpuCode),
                            ipVer);

        /*
            1.4. Call with wrong enum values cpuCode and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfCpuCodeIpLinkLocalProtSet
                            (dev, ipVer, protocol, cpuCode),
                            cpuCode);

        /*
            1.5. Call with cpuCode[CPSS_NET_CONTROL_DEST_MAC_TRAP_E] and other params from 1.1. (not supported)
            Expected: NOT GT_OK.
        */
        cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E;

        st = cpssDxChNetIfCpuCodeIpLinkLocalProtSet(dev, ipVer, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cpuCode = %d", dev, cpuCode);
    }

    ipVer    = CPSS_IP_PROTOCOL_IPV4_E;
    protocol = 0;
    cpuCode  = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeIpLinkLocalProtSet(dev, ipVer, protocol, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeIpLinkLocalProtSet(dev, ipVer, protocol, cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeIpLinkLocalProtGet
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   ipVer,
    IN GT_U8                        protocol,
    OUT CPSS_NET_RX_CPU_CODE_ENT    *cpuCodePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeIpLinkLocalProtGet)
{
/*
    ITERATE_DEVICE (DXCH2 and above)
    1.1. Call with protocol[0], ipVer[CPSS_IP_PROTOCOL_IPV4_E] and not NULL cpuCodePtr.
    Expected: GT_OK.
    1.2. Call with wrong enum values ipVer and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with cpuCodePtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U8                      protocol = 0;
    CPSS_IP_PROTOCOL_STACK_ENT ipVer    = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_NET_RX_CPU_CODE_ENT   cpuCode  = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with protocol[0] and not NULL cpuCodePtr.
            Expected: GT_OK.
        */
        protocol = 0;

        st = cpssDxChNetIfCpuCodeIpLinkLocalProtGet(dev, ipVer, protocol, &cpuCode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ipVer, protocol);

        /*
            1.2. Call with wrong enum values ipVer and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfCpuCodeIpLinkLocalProtGet
                            (dev, ipVer, protocol, &cpuCode),
                            ipVer);

        /*
            1.3. Call with cpuCodePtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfCpuCodeIpLinkLocalProtGet(dev, ipVer, protocol, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cpuCodePtr = NULL", dev);
    }

    protocol = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeIpLinkLocalProtGet(dev, ipVer, protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeIpLinkLocalProtGet(dev, ipVer, protocol, &cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet
(
    IN GT_U8                    devNum,
    IN GT_U8                    protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT cpuCode
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet)
{
/*
    ITERATE_DEVICE (DXCH2 and above)
    1.1. Call with protocol[0], cpuCode[CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E / CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E]
    Expected: GT_OK.
    1.2. Call cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet with not NULL cpuCodePtr and other params from 1.1.
    Expected: GT_OK and the same cpuCode as was set.
    1.3. Call with wrong enum values cpuCode and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with cpuCode[CPSS_NET_CONTROL_DEST_MAC_TRAP_E] and other params from 1.1. (not supported)
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U8                       protocol   = 0;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode    = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCodeGet = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with protocol[0],
                           cpuCode[CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E /
                                   CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E]
            Expected: GT_OK.
        */
        /* iterate with cpuCode = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E */
        protocol = 0;
        cpuCode  = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E;

        st = cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet(dev, protocol, cpuCode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocol, cpuCode);

        /*
            1.2. Call cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet with not NULL cpuCodePtr
                                                                        and other params from 1.1.
            Expected: GT_OK and the same cpuCode as was set.
        */
        st = cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet(dev, protocol, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet: %d, %d", dev, protocol);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet, "got another cpuCode than was set: %d", dev);

        /* iterate with cpuCode = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E */
        protocol = 0;
        cpuCode  = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E;

        st = cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet(dev, protocol, cpuCode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocol, cpuCode);

        /*
            1.2. Call cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet with not NULL cpuCodePtr
                                                                        and other params from 1.1.
            Expected: GT_OK and the same cpuCode as was set.
        */
        st = cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet(dev, protocol, &cpuCodeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet: %d, %d", dev, protocol);
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuCode, cpuCodeGet, "got another cpuCode than was set: %d", dev);

        /*
            1.3. Call with wrong enum values cpuCode and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet
                            (dev, protocol, cpuCode),
                            cpuCode);

        /*
            1.4. Call with cpuCode[CPSS_NET_CONTROL_DEST_MAC_TRAP_E]
                           and other params from 1.1. (not supported)
            Expected: NOT GT_OK.
        */
        cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E;

        st = cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet(dev, protocol, cpuCode);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cpuCode = %d", dev, cpuCode);
    }

    protocol = 0;
    cpuCode  = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet(dev, protocol, cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet(dev, protocol, cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet
(
    IN GT_U8                      devNum,
    IN GT_U8                      protocol,
    OUT CPSS_NET_RX_CPU_CODE_ENT  *cpuCodePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet)
{
/*
    ITERATE_DEVICE (DXCH2 and above)
    1.1. Call with protocol[0], and not NULL cpuCodePtr.
    Expected: GT_OK.
    1.2. Call with cpuCodePtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U8                       protocol = 0;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode  = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with protocol[0],
                           and not NULL cpuCodePtr.
            Expected: GT_OK.
        */
        protocol = 0;

        st = cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet(dev, protocol, &cpuCode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocol);

        /*
            1.2. Call with cpuCodePtr[NULL]
                           and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet(dev, protocol, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cpuCodePtr = NULL", dev);
    }

    protocol = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet(dev, protocol, &cpuCode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet(dev, protocol, &cpuCode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeTableSet
(
    IN GT_U8                    devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT cpuCode,
    IN CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC *entryInfoPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeTableSet)
{
/*
    ITERATE_DEVICE (DXCH)
    1.1. Call with cpuCode[CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E / CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E],
                   entryInfo{
                            tc[0 / 7],
                            dp[CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                            truncate[GT_FALSE / GT_TRUE],
                            cpuRateLimitMode[CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E / CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E],
                            cpuCodeRateLimiterIndex[0 / 31],
                            cpuCodeStatRateLimitIndex[0 / 31],
                            designatedDevNumIndex[0 / 7] }.
    Expected: GT_OK
    1.2. Call cpssDxChNetIfCpuCodeTableGet with not NULL entryInfoPtr and other params from 1.1.
    Expected: GT_OK and the same entryInfo as aws set.
    1.3. Call with wrong enum values cpuCode and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with wrong enum values entryInfoPtr->dp and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with wrong enum values entryInfoPtr->cpuRateLimitMode and other params from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range entryInfoPtr->cpuCodeRateLimiterIndex[32] and other params from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range entryInfoPtr->cpuCodeStatRateLimitIndex[32] and other params from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with out of range entryInfoPtr->designatedDevNumIndex[8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with out of range entryInfoPtr->tc [8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.10. Call with out of range entryInfoPtr->dp [CPSS_DP_YELLOW_E] and other params from 1.1.
    Expected: NOT GT_OK.
    1.11. Call with out of range entryInfoPtr->entryInfoPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL                                 isEqual   = GT_FALSE;
    CPSS_NET_RX_CPU_CODE_ENT                cpuCode   = CPSS_NET_CONTROL_E;
    CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC  entryInfo;
    CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC  entryInfoGet;


    cpssOsBzero((GT_VOID*) &entryInfo, sizeof(entryInfo));
    cpssOsBzero((GT_VOID*) &entryInfoGet, sizeof(entryInfoGet));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cpuCode[CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E / CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E],
                           entryInfo{
                                    tc[0 / 7],
                                    dp[CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                                    truncate[GT_FALSE / GT_TRUE],
                                    cpuRateLimitMode[CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E / CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E],
                                    cpuCodeRateLimiterIndex[0 / 31],
                                    cpuCodeStatRateLimitIndex[0 / 31],
                                    designatedDevNumIndex[0 / 7] }.
            Expected: GT_OK
        */
        /* iterate with cpuCode = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E */
        cpuCode = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
        entryInfo.tc       = 0;
        entryInfo.dp       = CPSS_DP_GREEN_E;
        entryInfo.truncate = GT_FALSE;
        entryInfo.cpuRateLimitMode          = CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E;
        entryInfo.cpuCodeRateLimiterIndex   = 0;
        entryInfo.cpuCodeStatRateLimitIndex = 0;
        entryInfo.designatedDevNumIndex     = 0;

        st = cpssDxChNetIfCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        /*
            1.2. Call cpssDxChNetIfCpuCodeTableGet with not NULL entryInfoPtr and other params from 1.1.
            Expected: GT_OK and the same entryInfo as aws set.
        */
        st = cpssDxChNetIfCpuCodeTableGet(dev, cpuCode, &entryInfoGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeTableGet: %d, %d", dev, cpuCode);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &entryInfo, (GT_VOID*) &entryInfoGet, sizeof(entryInfo) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another entryInfo than was set", dev);

        /* iterate with cpuCode = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E */
        cpuCode = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E;
        entryInfo.tc       = 7;
        entryInfo.dp       = CPSS_DP_RED_E;
        entryInfo.truncate = GT_TRUE;
        entryInfo.cpuRateLimitMode          = CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E;
        entryInfo.cpuCodeRateLimiterIndex   = 31;
        entryInfo.cpuCodeStatRateLimitIndex = 31;
        entryInfo.designatedDevNumIndex     = 7;

        st = cpssDxChNetIfCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        /*
            1.2. Call cpssDxChNetIfCpuCodeTableGet with not NULL entryInfoPtr and other params from 1.1.
            Expected: GT_OK and the same entryInfo as aws set.
        */
        st = cpssDxChNetIfCpuCodeTableGet(dev, cpuCode, &entryInfoGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeTableGet: %d, %d", dev, cpuCode);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &entryInfo, (GT_VOID*) &entryInfoGet, sizeof(entryInfo) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another entryInfo than was set", dev);

        /*
            1.3. Call with wrong enum values cpuCode and other params from 1.1.
            Expected: NOT GT_OK.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfCpuCodeTableSet
                            (dev, cpuCode, &entryInfo),
                            cpuCode);

        /*
            1.4. Call with wrong enum values entryInfoPtr->dp and other params from 1.1.
            Expected: NOT GT_OK.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfCpuCodeTableSet
                            (dev, cpuCode, &entryInfo),
                            entryInfo.dp);

        /*
            1.5. Call with wrong enum values entryInfoPtr->cpuRateLimitMode
                 and other params from 1.1.
            Expected: NOT GT_OK.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfCpuCodeTableSet
                            (dev, cpuCode, &entryInfo),
                            entryInfo.cpuRateLimitMode);

        /*
            1.6. Call with out of range entryInfoPtr->cpuCodeRateLimiterIndex[32] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        entryInfo.cpuCodeRateLimiterIndex = 32;

        st = cpssDxChNetIfCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryInfo.cpuCodeRateLimiterIndex = %d",
                                     dev, entryInfo.cpuCodeRateLimiterIndex);

        entryInfo.cpuCodeRateLimiterIndex = 31;

        /*
            1.7. Call with out of range entryInfoPtr->cpuCodeStatRateLimitIndex[32] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        entryInfo.cpuCodeStatRateLimitIndex = 32;

        st = cpssDxChNetIfCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryInfo.cpuCodeStatRateLimitIndex = %d",
                                     dev, entryInfo.cpuCodeStatRateLimitIndex);

        entryInfo.cpuCodeStatRateLimitIndex = 31;

        /*
            1.8. Call with out of range entryInfoPtr->designatedDevNumIndex[8] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        entryInfo.designatedDevNumIndex = 8;

        st = cpssDxChNetIfCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryInfo.designatedDevNumIndex = %d",
                                     dev, entryInfo.designatedDevNumIndex);

        entryInfo.designatedDevNumIndex = 7;

        /*
            1.9. Call with out of range entryInfoPtr->tc [8] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        entryInfo.tc = 8;

        st = cpssDxChNetIfCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryInfo.tc = %d", dev, entryInfo.tc);

        entryInfo.tc = 7;

        /*
            1.10. Call with out of range entryInfoPtr->dp [CPSS_DP_YELLOW_E] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        entryInfo.dp = CPSS_DP_YELLOW_E;

        st = cpssDxChNetIfCpuCodeTableSet(dev, cpuCode, &entryInfo);
        if (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(
                GT_OK, st, "%d, entryInfo.dp = %d", dev, entryInfo.dp);
        }
        else
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(
                GT_OK, st, "%d, entryInfo.dp = %d", dev, entryInfo.dp);
        }

        if (st == GT_OK)
        {
            st = cpssDxChNetIfCpuCodeTableGet(dev, cpuCode, &entryInfoGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeTableGet: %d, %d", dev, cpuCode);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &entryInfo, (GT_VOID*) &entryInfoGet, sizeof(entryInfo) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another entryInfo than was set", dev);
        }

        entryInfo.dp = CPSS_DP_GREEN_E;

        /*
            1.11. Call with out of range entryInfoPtr->entryInfoPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfCpuCodeTableSet(dev, cpuCode, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryInfoPtr = NULL", dev);
    }

    cpuCode = CPSS_NET_INTERVENTION_SA_E;
    entryInfo.tc       = 0;
    entryInfo.dp       = CPSS_DP_GREEN_E;
    entryInfo.truncate = GT_FALSE;
    entryInfo.cpuRateLimitMode          = CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E;
    entryInfo.cpuCodeRateLimiterIndex   = 0;
    entryInfo.cpuCodeStatRateLimitIndex = 0;
    entryInfo.designatedDevNumIndex     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeTableSet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeTableSet(dev, cpuCode, &entryInfo);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeTableGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT cpuCode,
    OUT CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC *entryInfoPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeTableGet)
{
/*
    ITERATE_DEVICE (DXCH)
    1.1. Call with cpuCode[CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E] and not NULL entryInfoPtr.
    Expected: GT_OK.
    1.2. Call with cpuCode[wrong enum values] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with entryInfoPtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_NET_RX_CPU_CODE_ENT                cpuCode   = CPSS_NET_INTERVENTION_SA_E;
    CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC  entryInfo;


    cpssOsBzero((GT_VOID*) &entryInfo, sizeof(entryInfo));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cpuCode[CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E]
                           and not NULL entryInfoPtr.
            Expected: GT_OK.
        */
        cpuCode = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;

        st = cpssDxChNetIfCpuCodeTableGet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cpuCode);

        /*
            1.2. Call with cpuCode[wrong enum values] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfCpuCodeTableGet
                            (dev, cpuCode, &entryInfo),
                            cpuCode);

        /*
            1.3. Call with entryInfoPtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfCpuCodeTableGet(dev, cpuCode, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryInfoPtr=  NULL", dev);
    }

    cpuCode = CPSS_NET_INTERVENTION_SA_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeTableGet(dev, cpuCode, &entryInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeTableGet(dev, cpuCode, &entryInfo);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_U32       statisticalRateLimit
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet)
{
/*
    ITERATE_DEVICE (DXCHX)
    1.1. Call with index[0 / 31], statisticalRateLimit[0 / 0xFFFFFFFF].
    Expected: GT_OK.
    1.2. Call cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet with not NULL statisticalRateLimitPtr
                                                                     and other params from 1.1.
    Expected: GT_OK and the same statisticalRateLimit as was set.
    1.3. Call with index[32] and other param from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32       index = 0;
    GT_U32       statisticalRateLimit    = 0;
    GT_U32       statisticalRateLimitGet = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index[0 / 31], statisticalRateLimit[0 / 0xFFFFFFFF].
            Expected: GT_OK.
        */
        /* iterate with index = 0 */
        index = 0;
        statisticalRateLimit = 0;

        st = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet(dev, index, statisticalRateLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, statisticalRateLimit);

        /*
            1.2. Call cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet with not NULL statisticalRateLimitPtr
                                                                             and other params from 1.1.
            Expected: GT_OK and the same statisticalRateLimit as was set.
        */
        st = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet(dev, index, &statisticalRateLimitGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet: %d, %d", dev, index);

        UTF_VERIFY_EQUAL1_STRING_MAC(statisticalRateLimit, statisticalRateLimitGet,
                                     "got another statisticalRateLimit than was set: %d", dev);

        /* iterate with index = 31 */
        index = 31;
        statisticalRateLimit = 0xFFFFFFFF;

        st = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet(dev, index, statisticalRateLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, statisticalRateLimit);

        /*
            1.2. Call cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet with not NULL statisticalRateLimitPtr
                                                                             and other params from 1.1.
            Expected: GT_OK and the same statisticalRateLimit as was set.
        */
        st = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet(dev, index, &statisticalRateLimitGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet: %d, %d", dev, index);

        UTF_VERIFY_EQUAL1_STRING_MAC(statisticalRateLimit, statisticalRateLimitGet,
                                     "got another statisticalRateLimit than was set: %d", dev);

        /*
            1.3. Call with index[32] and other param from 1.1.
            Expected: NOT GT_OK.
        */
        index = 32;

        st = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet(dev, index, statisticalRateLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);
    }

    index = 0;
    statisticalRateLimit = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet(dev, index, statisticalRateLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet(dev, index, statisticalRateLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet
(
    IN  GT_U8       devNum,
    IN  GT_U32      index,
    OUT GT_U32      *statisticalRateLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet)
{
/*
    ITERATE_DEVICE (DXCHX)
    1.1. Call with index[0 / 31] and not NULL statisticalRateLimitPtr.
    Expected: GT_OK.
    1.2. Call with index[32] and other param from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with statisticalRateLimitPtr[NULL] and other param from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      index = 0;
    GT_U32      statisticalRateLimit = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index[0 / 31] and not NULL statisticalRateLimitPtr.
            Expected: GT_OK.
        */
        /* iterate with index = 0 */
        index = 0;

        st = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet(dev, index, &statisticalRateLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* iterate with index = 31 */
        index = 31;

        st = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet(dev, index, &statisticalRateLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call with index[32] and other param from 1.1.
            Expected: NOT GT_OK.
        */
        index = 32;

        st = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet(dev, index, &statisticalRateLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 31;

        /*
            1.3. Call with statisticalRateLimitPtr[NULL] and other param from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet(dev, index, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, "%d, statisticalRateLimit = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet(dev, index, &statisticalRateLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet(dev, index, &statisticalRateLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeDesignatedDeviceTableSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_U8        designatedDevNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeDesignatedDeviceTableSet)
{
/*
    ITERATE_DEVICE (DXCH)
    1.1. Call with index[1 / 7], designatedDevNum[0 / 31].
    Expected: GT_OK.
    1.2. Call cpssDxChNetIfCpuCodeDesignatedDeviceTableGet with not NULL designatedDevNumPtr
                                                                     and other params from 1.1.
    Expected: GT_OK and the same designatedDevNum as was set.
    1.3. Call with out of range index[8] and other param from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range index[0] and other param from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range designatedDevNum[32] and other param from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32       index = 0;
    GT_U8        designatedDevNum    = 0;
    GT_U8        designatedDevNumGet = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index[1 / 7], designatedDevNum[0 / 31].
            Expected: GT_OK.
        */
        /* iterate with index = 1 */
        index = 1;
        designatedDevNum = 0;

        st = cpssDxChNetIfCpuCodeDesignatedDeviceTableSet(dev, index, designatedDevNum);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, designatedDevNum);

        /*
            1.2. Call cpssDxChNetIfCpuCodeDesignatedDeviceTableGet with not NULL designatedDevNumPtr
                                                                             and other params from 1.1.
            Expected: GT_OK and the same designatedDevNum as was set.
        */
        st = cpssDxChNetIfCpuCodeDesignatedDeviceTableGet(dev, index, &designatedDevNumGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeDesignatedDeviceTableGet: %d, %d",
                                     dev, index);
        UTF_VERIFY_EQUAL1_STRING_MAC(designatedDevNum, designatedDevNumGet,
                                     "got another designatedDevNum than was set: %d", dev);

        /* iterate with index = 7 */
        index = 7;
        designatedDevNum = 31;

        st = cpssDxChNetIfCpuCodeDesignatedDeviceTableSet(dev, index, designatedDevNum);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, designatedDevNum);

        /*
            1.2. Call cpssDxChNetIfCpuCodeDesignatedDeviceTableGet with not NULL designatedDevNumPtr
                                                                             and other params from 1.1.
            Expected: GT_OK and the same designatedDevNum as was set.
        */
        st = cpssDxChNetIfCpuCodeDesignatedDeviceTableGet(dev, index, &designatedDevNumGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeDesignatedDeviceTableGet: %d, %d",
                                     dev, index);
        UTF_VERIFY_EQUAL1_STRING_MAC(designatedDevNum, designatedDevNumGet,
                                     "got another designatedDevNum than was set: %d", dev);

        /*
            1.3. Call with out of range index[8] and other param from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssDxChNetIfCpuCodeDesignatedDeviceTableSet(dev, index, designatedDevNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 7;

        /*
            1.4. Call with out of range index[0] and other param from 1.1.
            Expected: NOT GT_OK.
        */
        index = 0;

        st = cpssDxChNetIfCpuCodeDesignatedDeviceTableSet(dev, index, designatedDevNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 7;

        /*
            1.5. Call with out of range designatedDevNum[32] and other param from 1.1.
            Expected: NOT GT_OK.
        */
        designatedDevNum = 32;

        st = cpssDxChNetIfCpuCodeDesignatedDeviceTableSet(dev, index, designatedDevNum);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, designatedDevNum = %d", dev, designatedDevNum);
    }

    index = 0;
    designatedDevNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeDesignatedDeviceTableSet(dev, index, designatedDevNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeDesignatedDeviceTableSet(dev, index, designatedDevNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeDesignatedDeviceTableGet
(
    IN  GT_U8       devNum,
    IN  GT_U32      index,
    OUT GT_U8       *designatedDevNumPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeDesignatedDeviceTableGet)
{
/*
    ITERATE_DEVICE (DXCH)
    1.1. Call with index[1 / 7] and not NULL designatedDevNumPtr.
    Expected: GT_OK.
    1.2. Call with out of range index[8] and other param from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range index[0] and other param from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with designatedDevNumPtr [NULL] and other param from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      index = 0;
    GT_U8       designatedDevNum = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index[1 / 7] and not NULL designatedDevNumPtr.
            Expected: GT_OK.
        */
        /* iterate with index = 1 */
        index = 1;

        st = cpssDxChNetIfCpuCodeDesignatedDeviceTableGet(dev, index, &designatedDevNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* iterate with index = 7 */
        index = 7;

        st = cpssDxChNetIfCpuCodeDesignatedDeviceTableGet(dev, index, &designatedDevNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call with out of range index[8] and other param from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssDxChNetIfCpuCodeDesignatedDeviceTableGet(dev, index, &designatedDevNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 1;

        /*
            1.3. Call with out of range index[0] and other param from 1.1.
            Expected: NOT GT_OK.
        */
        index = 0;

        st = cpssDxChNetIfCpuCodeDesignatedDeviceTableGet(dev, index, &designatedDevNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 1;

        /*
            1.4. Call with designatedDevNumPtr [NULL] and other param from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfCpuCodeDesignatedDeviceTableGet(dev, index, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, designatedDevNumPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeDesignatedDeviceTableGet(dev, index, &designatedDevNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeDesignatedDeviceTableGet(dev, index, &designatedDevNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterTableSet
(
    IN GT_U8  devNum,
    IN GT_U32 rateLimiterIndex,
    IN GT_U32 windowSize,
    IN GT_U32 pktLimit
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeRateLimiterTableSet)
{
/*
    ITERATE_DEVICE (DxCh2 and above)
    1.1. Call with rateLimiterIndex[1 / 31],
                   windowSize[0 / 0xFFF],
                   pktLimit[0 / 0xFFFF]
    Expected: GT_OK.
    1.2. Call cpssDxChNetIfCpuCodeRateLimiterTableGet with not NULL windowSizePtr, pktLimitPtr and other params from 1.1.
    Expected: GT_OK and the same windowSize, pktLimit as was set.
    1.3. Call with out of range rateLimiterIndex[0] and other params from 1.1.
    Expected: NOT GT_OK
    1.4. Call with out of range rateLimiterIndex[32] and other params from 1.1.
    Expected: NOT GT_OK
    1.5. Call with out of range windowSize[0x1000] and other params from 1.1.
    Expected: NOT GT_OK
    1.6. Call with out of range pktLimit[0x10000] and other params from 1.1.
    Expected: NOT GT_OK
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      rateLimiterIndex = 0;
    GT_U32      windowSize       = 0;
    GT_U32      pktLimit         = 0;
    GT_U32      windowSizeGet    = 0;
    GT_U32      pktLimitGet      = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rateLimiterIndex[1 / 31],
                           windowSize[0 / 0xFFF],
                           pktLimit[0 / 0xFFFF]
            Expected: GT_OK.
        */
        /* iterate with rateLimiterIndex = 1 */
        rateLimiterIndex = 1;
        windowSize       = 0;
        pktLimit         = 0;

        st = cpssDxChNetIfCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, windowSize, pktLimit);

        /*
            1.2. Call cpssDxChNetIfCpuCodeRateLimiterTableGet with not NULL windowSizePtr, pktLimitPtr and other params from 1.1.
            Expected: GT_OK and the same windowSize, pktLimit as was set.
        */
        st = cpssDxChNetIfCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSizeGet, &pktLimitGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeRateLimiterTableGet: %d, %d", dev, rateLimiterIndex);

        UTF_VERIFY_EQUAL1_STRING_MAC(windowSize, windowSizeGet, "got another windowSize than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pktLimit, pktLimitGet, "got another pktLimit than was set: %d", dev);

        /* iterate with rateLimiterIndex = 31 */
        rateLimiterIndex = 31;
        windowSize       = 0xFFF;
        pktLimit         = 0xFFFF;

        st = cpssDxChNetIfCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex, windowSize, pktLimit);

        /*
            1.2. Call cpssDxChNetIfCpuCodeRateLimiterTableGet with not NULL windowSizePtr, pktLimitPtr and other params from 1.1.
            Expected: GT_OK and the same windowSize, pktLimit as was set.
        */
        st = cpssDxChNetIfCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSizeGet, &pktLimitGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeRateLimiterTableGet: %d, %d", dev, rateLimiterIndex);

        UTF_VERIFY_EQUAL1_STRING_MAC(windowSize, windowSizeGet, "got another windowSize than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(pktLimit, pktLimitGet, "got another pktLimit than was set: %d", dev);

        /*
            1.3. Call with out of range rateLimiterIndex[0] and other params from 1.1.
            Expected: NOT GT_OK
        */
        rateLimiterIndex = 0;

        st = cpssDxChNetIfCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        rateLimiterIndex = 31;

        /*
            1.4. Call with out of range rateLimiterIndex[32] and other params from 1.1.
            Expected: NOT GT_OK
        */
        rateLimiterIndex = 32;

        st = cpssDxChNetIfCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        rateLimiterIndex = 31;

        /*
            1.5. Call with out of range windowSize[0x1000] and other params from 1.1.
            Expected: NOT GT_OK
        */
        windowSize = 0x1000;

        st = cpssDxChNetIfCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, windowSize = %d", dev, windowSize);

        windowSize = 0xFFF;

        /*
            1.5. Call with out of range pktLimit[0x10000] and other params from 1.1.
            Expected: NOT GT_OK
        */
        pktLimit = 0x10000;

        st = cpssDxChNetIfCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pktLimit = %d", dev, pktLimit);
    }

    rateLimiterIndex = 1;
    windowSize       = 0;
    pktLimit         = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeRateLimiterTableSet(dev, rateLimiterIndex, windowSize, pktLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterTableGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  rateLimiterIndex,
    OUT GT_U32  *windowSizePtr,
    OUT GT_U32  *pktLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeRateLimiterTableGet)
{
/*
    ITERATE_DEVICE (DxCh2 and above)
    1.1. Call with rateLimiterIndex[1 / 31] and not NULL windowSizePtr, pktLimitPtr
    Expected: GT_OK.
    1.2. Call with rateLimiterIndex[0] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with rateLimiterIndex[32] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with windowSizePtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with pktLimitPtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      rateLimiterIndex = 0;
    GT_U32      windowSize       = 0;
    GT_U32      pktLimit         = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rateLimiterIndex[1 / 31] and not NULL windowSizePtr, pktLimitPtr
            Expected: GT_OK.
        */
        /* iterate with rateLimiterIndex = 1 */
        rateLimiterIndex = 1;

        st = cpssDxChNetIfCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSize, &pktLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        /* iterate with rateLimiterIndex = 31 */
        rateLimiterIndex = 31;

        st = cpssDxChNetIfCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSize, &pktLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        /*
            1.2. Call with rateLimiterIndex[0] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        rateLimiterIndex = 0;

        st = cpssDxChNetIfCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSize, &pktLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        rateLimiterIndex = 31;

        /*
            1.3. Call with rateLimiterIndex[32] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        rateLimiterIndex = 32;

        st = cpssDxChNetIfCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSize, &pktLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, rateLimiterIndex);

        rateLimiterIndex = 31;

        /*
            1.4. Call with windowSizePtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, NULL, &pktLimit);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, windowSizePtr = NULL", dev);

        /*
            1.5. Call with pktLimitPtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSize, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pktLimitPtr = NULL", dev);
    }

    rateLimiterIndex = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSize, &pktLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeRateLimiterTableGet(dev, rateLimiterIndex, &windowSize, &pktLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet
(
    IN GT_U8  devNum,
    IN GT_U32 windowResolution
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet)
{
/*
    ITERATE_DEVICE (DxCh2 and above)
    1.1. Call with windowResolution[1*granularity / 2*granularity], where granularity = (1000 * 32) / coreClock(dev).
    Expected: GT_OK.
    1.2. Call cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet with not NULL windowResolutionPtr.
    Expected: GT_OK and the same windowResolution as was set.
    1.3. Call with windowResolution[0]
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32 windowResolution    = 0;
    GT_U32 windowResolutionGet = 0;
    GT_U32 coreClock           = 0;
    GT_U32 granularity         = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting device coreClock */
        st = prvUtfCoreClockGet(dev, &coreClock);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCoreClockGet: %d", dev);

        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(0, coreClock, "prvUtfCoreClockGet: coreClock = 0, dev = %d", dev);
        if(0 == coreClock)
        {
            continue;
        }

        /*
            1.1. Call with windowResolution[1 * granularity / 2 * granularity]
            Expected: GT_OK.
        */
        /* iterate with windowResolution = 1 * granularity */
        granularity      = (1000 * 32) / coreClock;
        windowResolution = 1 * granularity;

        st = cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(dev, windowResolution);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, windowResolution);

        /*
            1.2. Call cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet with not NULL windowResolutionPtr.
            Expected: GT_OK and the same windowResolution as was set.
        */
        st = cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet(dev, &windowResolutionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(windowResolution, windowResolutionGet, "got another windowResolution than was set: %d", dev);

        /* iterate with windowResolution = 2 * granularity */
        windowResolution = 2 * granularity;

        st = cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(dev, windowResolution);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, windowResolution);

        /*
            1.2. Call cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet with not NULL windowResolutionPtr.
            Expected: GT_OK and the same windowResolution as was set.
        */
        st = cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet(dev, &windowResolutionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(windowResolution, windowResolutionGet, "got another windowResolution than was set: %d", dev);

        /*
            1.3. Call with windowResolution[0]
            Expected: NOT GT_OK.
        */
        windowResolution = 0;

        st = cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(dev, windowResolution);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, windowResolution);
    }

    windowResolution = 1 * granularity;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(dev, windowResolution);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(dev, windowResolution);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet
(
    IN GT_U8  devNum,
    IN GT_U32 *windowResolutionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet)
{
/*
    ITERATE_DEVICE (DxCh2 and above)
    1.1. Call with not NULL windowResolutionPtr.
    Expected: GT_OK.
    1.2. Call with windowResolutionPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      windowResolution = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL windowResolutionPtr.
            Expected: GT_OK.
        */
        st = cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet(dev, &windowResolution);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with windowResolutionPtr[NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, windowResolutionPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet(dev, &windowResolution);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet(dev, &windowResolution);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfFromCpuDpSet
(
    IN GT_U8               devNum,
    IN  CPSS_DP_LEVEL_ENT  dpLevel
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfFromCpuDpSet)
{
/*
    ITERATE_DEVICE (DxCh3 and above)
    1.1. Call with dpLevel [CPSS_DP_GREEN_E / CPSS_DP_RED_E].
    Expected: GT_OK.
    1.2. Call cpssDxChNetIfFromCpuDpGet with not NULL dpLevelPtr.
    Expected: GT_OK.
    1.3. Call with dpLevel [wrong enum values].
    Expected: GT_BAD_PARAM.
    1.4. Call with dpLevel [CPSS_DP_YELLOW_E].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DP_LEVEL_ENT  dpLevel    = CPSS_DP_GREEN_E;
    CPSS_DP_LEVEL_ENT  dpLevelGet = CPSS_DP_GREEN_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with dpLevel [CPSS_DP_GREEN_E / CPSS_DP_RED_E].
            Expected: GT_OK.
        */
        /* iterate with dpLevel = CPSS_DP_GREEN_E */
        dpLevel = CPSS_DP_GREEN_E;

        st = cpssDxChNetIfFromCpuDpSet(dev, dpLevel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dpLevel);

        /*
            1.2. Call cpssDxChNetIfFromCpuDpGet with not NULL dpLevelPtr.
            Expected: GT_OK.
        */
        st = cpssDxChNetIfFromCpuDpGet(dev, &dpLevelGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChNetIfFromCpuDpGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(dpLevel, dpLevelGet, "get another dpLevel than was set: %d", dev);

        /* iterate with dpLevel = CPSS_DP_RED_E */
        dpLevel = CPSS_DP_RED_E;

        st = cpssDxChNetIfFromCpuDpSet(dev, dpLevel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dpLevel);

        /*
            1.2. Call cpssDxChNetIfFromCpuDpGet with not NULL dpLevelPtr.
            Expected: GT_OK.
        */
        st = cpssDxChNetIfFromCpuDpGet(dev, &dpLevelGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChNetIfFromCpuDpGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(dpLevel, dpLevelGet, "get another dpLevel than was set: %d", dev);

        /*
            1.3. Call with dpLevel [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChNetIfFromCpuDpSet
                            (dev, dpLevel),
                            dpLevel);

        /*
            1.4. Call with dpLevel [CPSS_DP_YELLOW_E].
            Expected: NOT GT_OK.
        */
        dpLevel = CPSS_DP_YELLOW_E;

        st = cpssDxChNetIfFromCpuDpSet(dev, dpLevel);
        if (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, dpLevel);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dpLevel);
        }

        if (st == GT_OK)
        {
            st = cpssDxChNetIfFromCpuDpGet(dev, &dpLevelGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChNetIfFromCpuDpGet: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dpLevel, dpLevelGet, "get another dpLevel than was set: %d", dev);
        }
    }

    dpLevel = CPSS_DP_GREEN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfFromCpuDpSet(dev, dpLevel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfFromCpuDpSet(dev, dpLevel);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfFromCpuDpGet
(
    IN  GT_U8              devNum,
    OUT CPSS_DP_LEVEL_ENT  *dpLevelPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfFromCpuDpGet)
{
/*
    ITERATE_DEVICE (DxCh3 and above)
    1.1. Call with not NULL dpLevelPtr.
    Expected: GT_OK.
    1.2. Call with dpLevel [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DP_LEVEL_ENT  dpLevel = CPSS_DP_GREEN_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL dpLevelPtr.
            Expected: GT_OK.
        */
        st = cpssDxChNetIfFromCpuDpGet(dev, &dpLevel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with dpLevel [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfFromCpuDpGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dpLevelPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfFromCpuDpGet(dev, &dpLevel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfFromCpuDpGet(dev, &dpLevel);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfSdmaRxResourceErrorModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   queue,
    IN  CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT   mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfSdmaRxResourceErrorModeSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with queue[0 / 7],
                   mode [CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E / CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ABORT_E].
    Expected: GT_OK.
    1.2. Call with queue[8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with mode [wrong enum values] and other params from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8 queue = 0;
    CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT mode;

    mode = CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with queue[0 / 7],  PRV_CPSS_DXCH3_SDMA_WA_E
                           mode [CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E / CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ABORT_E].
            Expected: GT_OK.
        */
        /* iterate with queue = 0 */
        queue = 0;
        mode = CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E;

        st = cpssDxChNetIfSdmaRxResourceErrorModeSet(dev, queue, mode);
        /* Due to Cheetah 3 Errata - " FEr#2009: DSDMA resource errors may cause
           PCI Express packets reception malfunction"
           -- see PRV_CPSS_DXCH3_SDMA_WA_E */
        if (PRV_CPSS_DXCH_ERRATA_GET_MAC(dev,PRV_CPSS_DXCH3_SDMA_WA_E) == GT_TRUE)
        {
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, mode);
        }
        else
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, mode);
        }

        /* iterate with queue = 7 */
        queue = 7;
        mode = CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ABORT_E;

        st = cpssDxChNetIfSdmaRxResourceErrorModeSet(dev, queue, mode);
        /* Due to Cheetah 3 Errata - " FEr#2009: DSDMA resource errors may cause
           PCI Express packets reception malfunction"
           -- see PRV_CPSS_DXCH3_SDMA_WA_E */
        if (PRV_CPSS_DXCH_ERRATA_GET_MAC(dev,PRV_CPSS_DXCH3_SDMA_WA_E) == GT_TRUE)
        {
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, mode);
        }
        else
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, mode);
        }

        /*
            1.2. Call with queue[8] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        queue = 8;

        st = cpssDxChNetIfSdmaRxResourceErrorModeSet(dev, queue, mode);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, mode);

        queue = 0;

        /*
            1.3. Call with mode [wrong enum values] and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        /* Due to Cheetah 3 Errata - " FEr#2009: DSDMA resource errors may cause
           PCI Express packets reception malfunction"
           -- see PRV_CPSS_DXCH3_SDMA_WA_E */
        if (PRV_CPSS_DXCH_ERRATA_GET_MAC(dev,PRV_CPSS_DXCH3_SDMA_WA_E) == GT_TRUE)
        {
            for(enumsIndex = 0; enumsIndex < utfInvalidEnumArrSize; enumsIndex++)
            {
                mode = utfInvalidEnumArr[enumsIndex];
                st = cpssDxChNetIfSdmaRxResourceErrorModeSet(dev, queue, mode);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, queue, mode);
            }
        }
        else
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChNetIfSdmaRxResourceErrorModeSet
                                (dev, queue, mode),
                                mode);
        }
    }

    queue = 0;
    mode = CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfSdmaRxResourceErrorModeSet(dev, queue, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfSdmaRxResourceErrorModeSet(dev, queue, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfDsaTagParse
(
    IN  GT_U8                          devNum,
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfDsaTagParse)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with dsaBytesPtr[1] and non null dsaInfoPtr.
    Expected: GT_OK.
    1.2. Call with wrong  dsaBytesPtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with wrong  dsaInfoPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8                          dsaBytes = 1;
    CPSS_DXCH_NET_DSA_PARAMS_STC   dsaInfo;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with dsaBytesPtr[1] and non null dsaInfoPtr.
            Expected: GT_OK.
        */

        st = cpssDxChNetIfDsaTagParse(dev, &dsaBytes, &dsaInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong  dsaBytesPtr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChNetIfDsaTagParse(dev, NULL, &dsaInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.3. Call with wrong  dsaInfoPtr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChNetIfDsaTagParse(dev, &dsaBytes, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }


    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfDsaTagParse(dev, &dsaBytes, &dsaInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfDsaTagParse(dev, &dsaBytes, &dsaInfo);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfInit
(
    IN  GT_U8                          devNum,
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfInit)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with correct dev.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
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

        st = cpssDxChNetIfInit(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }


    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfInit(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfInit(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfRemove
(
    IN  GT_U8                          devNum,
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfRemove)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with correct dev.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
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

        st = cpssDxChNetIfRemove(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfRemove(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfRemove(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfRxBufFree
(
    IN GT_U8    devNum,
    IN GT_U8    rxQueue,
    IN GT_U8    *rxBuffList[],
    IN GT_U32   buffListLen
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfRxBufFree)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with rxQueue [0], buffListLen [5].
    Expected: GT_OK.
    1.2. Call with wrong rxBuffList pointer [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       rxQueue = 0;
    GT_U8      *rxBuffList[5];
    GT_U8       rxBuffer[10];
    GT_U32      buffListLen = 5;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* connect buffers to the list
       use same buffer */
    rxBuffList[0] = rxBuffList[1] = rxBuffList[2] =
    rxBuffList[3] = rxBuffList[4] = rxBuffer;

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rxQueue [0], buffListLen [5].
            Expected: GT_OK.
        */
        #if 0
        /* Only buffers that recived by dxChNetIfRxPacketGet can be free*/
        st = cpssDxChNetIfRxBufFree(dev, rxQueue, rxBuffList, buffListLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        #endif
        /*
            1.2. Call with wrong rxBuffList pointer [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChNetIfRxBufFree(dev, rxQueue, NULL, buffListLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfRxBufFree(dev, rxQueue, rxBuffList, buffListLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfRxBufFree(dev, rxQueue, rxBuffList, buffListLen);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfRxBufFreeWithSize
(
    IN GT_U8    devNum,
    IN GT_U8    rxQueue,
    IN GT_U8    *rxBuffList[],
    IN GT_U32   rxBuffSizeList[],
    IN GT_U32   buffListLen
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfRxBufFreeWithSize)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with rxQueue [0], buffListLen [5] and rxBuffSizeList[5].
    Expected: GT_OK.
    1.2. Call with wrong rxBuffList pointer [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with wrong rxBuffSizeList pointer [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       rxQueue = 0;
    GT_U8       *rxBuffList[5];
    GT_U32      rxBuffSizeList[5];
    GT_U32      buffListLen = 5;
    GT_U8       rxBuffer[10];

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* connect buffers to the list
       use same buffer */
    rxBuffList[0] = rxBuffList[1] = rxBuffList[2] =
    rxBuffList[3] = rxBuffList[4] = rxBuffer;

    rxBuffSizeList[0] = rxBuffSizeList[1] = rxBuffSizeList[2] =
    rxBuffSizeList[3] = rxBuffSizeList[4] = sizeof(rxBuffer);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rxQueue [0], buffListLen [5] and rxBuffSizeList[5].
            Expected: GT_OK.
        */

        st = cpssDxChNetIfRxBufFreeWithSize(dev, rxQueue, rxBuffList,
                                            rxBuffSizeList, buffListLen);
        if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            if((PRV_CPSS_PP_MAC(dev)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E) &&
                !PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(dev))
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_STATE, st, dev);
            }
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }


        /*
            1.2. Call with wrong rxBuffList pointer [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChNetIfRxBufFreeWithSize(dev, rxQueue, NULL,
                                            rxBuffSizeList, buffListLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.2. Call with wrong rxBuffSizeList pointer [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChNetIfRxBufFreeWithSize(dev, rxQueue, rxBuffList,
                                            NULL, buffListLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfRxBufFreeWithSize(dev, rxQueue, rxBuffList,
                                            rxBuffSizeList, buffListLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfRxBufFreeWithSize(dev, rxQueue, rxBuffList,
                                        rxBuffSizeList, buffListLen);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfSdmaSyncTxPacketSend
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC              *pcktParamsPtr,
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
)
*/

UTF_TEST_CASE_MAC(cpssDxChNetIfSdmaSyncTxPacketSend)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with correct param value.
    Expected: GT_OK.
    1.2. Call with wrong rxBuffList pointer [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with wrong rxBuffSizeList pointer [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    CPSS_DXCH_NET_TX_PARAMS_STC  pcktParams;
    GT_U8                       *buffList[1] ;
    GT_U32                       buffLenList[1];
    GT_U32                       numOfBufs = 1;

    buffList[0] = cpssOsCacheDmaMalloc(64);
    buffLenList[0] = 64;
    cpssOsBzero((GT_VOID*) &pcktParams, sizeof(pcktParams));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with correct params.
            Expected: GT_OK.
        */
        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E;
        pcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_PORT_E;
        pcktParams.packetIsTagged = GT_TRUE;
        pcktParams.cookie = 0;
        pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
        pcktParams.sdmaInfo.txQueue = 1;
        pcktParams.sdmaInfo.evReqHndl = 0;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

        st = cpssDxChNetIfSdmaSyncTxPacketSend(dev, &pcktParams, buffList,
                                               buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong pcktParams pointer [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChNetIfSdmaSyncTxPacketSend(dev, NULL, buffList,
                                               buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.3. Call with wrong buffList pointer [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChNetIfSdmaSyncTxPacketSend(dev, &pcktParams, NULL,
                                               buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.4. Call with wrong buffLenList pointer [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChNetIfSdmaSyncTxPacketSend(dev, &pcktParams, buffList,
                                               NULL, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfSdmaSyncTxPacketSend(dev, &pcktParams, buffList,
                                               buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfSdmaSyncTxPacketSend(dev, &pcktParams, buffList,
                                           buffLenList, numOfBufs);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

#ifdef ASIC_SIMULATION
    /* allow all packets to egress the devices */
    cpssOsTimerWkAfter(500);
#endif /*ASIC_SIMULATION*/
    st = cpssOsCacheDmaFree(buffList[0]);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet
(
    IN GT_U8                devNum,
    IN GT_U8                txQueue,
    OUT GT_PTR             *cookiePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with correct param value.
    Expected: GT_OK.
    1.2. Call with wrong cookiePtr pointer [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       txQueue = 0;
    GT_PTR      cookiePtr;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with correct params.
            Expected: GT_OK.
        */

        /*st = cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet(dev, txQueue, cookiePtr);*/
        /*UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);*/

        /*
            1.2. Call with wrong cookiePtr pointer [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet(dev, txQueue, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet(dev, txQueue, &cookiePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet(dev, txQueue, &cookiePtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet
(
    IN GT_U8                devNum,
    IN GT_U8                txQueue,
    OUT GT_U32             *numberOfFreeTxDescriptorsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with txQueue [0].
    Expected: GT_OK.
    1.2. Call with wrong txQueue [8].
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong numberOfFreeTxDescriptorsPtr pointer [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       txQueue = 0;
    GT_U32      numberOfFreeTxDescriptors;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with txQueue [0].
            Expected: GT_OK.
        */

        st = cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet(dev, txQueue,
                                                        &numberOfFreeTxDescriptors);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong txQueue [8].
            Expected: GT_BAD_PARAM.
        */

        st = cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet(dev, 8,
                                                        &numberOfFreeTxDescriptors);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /*
            1.3. Call with wrong numberOfFreeTxDescriptorsPtr pointer [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet(dev, txQueue, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet(dev, txQueue,
                                                        &numberOfFreeTxDescriptors);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet(dev, txQueue,
                                                    &numberOfFreeTxDescriptors);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfSdmaTxPacketSend
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC              *pcktParamsPtr,
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfSdmaTxPacketSend)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with correct param value.
    Expected: GT_OK.
    1.2. Call with wrong rxBuffList pointer [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with wrong rxBuffSizeList pointer [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    CPSS_DXCH_NET_TX_PARAMS_STC  pcktParams;
    GT_U8                       *buffList[1];
    GT_U32                       buffLenList[1];
    GT_U32                       numOfBufs = 1;
    CPSS_UNI_EV_CAUSE_ENT        evCause[1] = { CPSS_PP_TX_BUFFER_QUEUE_E };
    GT_U32                       evReqHndl;

    buffList[0] = cpssOsCacheDmaMalloc(64);
    buffLenList[0] = 64;

    cpssOsBzero((GT_VOID*) &pcktParams, sizeof(pcktParams));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    st = cpssEventBind(evCause, 1, &evReqHndl);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with correct params.
            Expected: GT_OK.
        */

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E;
        pcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_PORT_E;
        pcktParams.packetIsTagged = GT_TRUE;
        pcktParams.cookie = 0;
        pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
        pcktParams.sdmaInfo.txQueue = 1;
        pcktParams.sdmaInfo.evReqHndl = evReqHndl;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;
        st = cpssDxChNetIfSdmaTxPacketSend(dev, &pcktParams, buffList,
                                               buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong pcktParams pointer [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfSdmaTxPacketSend(dev, NULL, buffList,
                                               buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.3. Call with wrong buffList pointer [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChNetIfSdmaTxPacketSend(dev, &pcktParams, NULL,
                                               buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

        /*
            1.4. Call with wrong buffLenList pointer [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChNetIfSdmaTxPacketSend(dev, &pcktParams, buffList,
                                               NULL, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfSdmaTxPacketSend(dev, &pcktParams, buffList,
                                               buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    st = cpssDxChNetIfSdmaTxPacketSend(dev, &pcktParams, buffList,
                                           buffLenList, numOfBufs);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
#ifdef ASIC_SIMULATION
    /* allow all packets to egress the devices */
    cpssOsTimerWkAfter(500);
#endif /*ASIC_SIMULATION*/
    st = cpssOsCacheDmaFree(buffList[0]);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfTxBufferQueueGet
(
    IN  GT_U32               hndl,
    OUT GT_U8               *devPtr,
    OUT GT_PTR              *cookiePtr,
    OUT GT_U8               *queuePtr,
    OUT GT_STATUS           *statusPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfTxBufferQueueGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with correct param value.
    Expected: GT_OK.
    1.2. Call with wrong devPtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with wrong cookiePtr  [NULL].
    Expected: GT_BAD_PTR.
    1.4. Call with wrong queuePtr  [NULL].
    Expected: GT_BAD_PTR.
    1.5. Call with wrong statusPtr  [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U32      hndl = 0;
    GT_U8       dev;
    GT_PTR      cookie;
    GT_U8       queue;
    GT_STATUS   status;

    /*
        1.1. Call with correct param value.
        Expected: GT_OK.
    */

    /*st = cpssDxChNetIfTxBufferQueueGet(hndl, &dev, &cookie, &queue, &status);*/
    /*UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);*/

    /*
        1.2. Call with wrong buffLenList pointer [NULL].
        Expected: GT_BAD_PTR.
    */
    st = cpssDxChNetIfTxBufferQueueGet(hndl, NULL, &cookie, &queue, &status);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

    /*
        1.3. Call with wrong buffLenList pointer [NULL].
        Expected: GT_BAD_PTR.
    */
    st = cpssDxChNetIfTxBufferQueueGet(hndl, &dev, NULL, &queue, &status);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

    /*
        1.4. Call with wrong buffLenList pointer [NULL].
        Expected: GT_BAD_PTR.
    */
    st = cpssDxChNetIfTxBufferQueueGet(hndl, &dev, &cookie, NULL, &status);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

    /*
        1.5. Call with wrong buffLenList pointer [NULL].
        Expected: GT_BAD_PTR.
    */
    st = cpssDxChNetIfTxBufferQueueGet(hndl, &dev, &cookie, &queue, NULL);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterDropCntrGet
(
    IN  GT_U8   dev,
    OUT GT_U32  *dropCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeRateLimiterDropCntrGet)
{
/*
    ITERATE_DEVICES(DxCh2 and above)
    1.1. Call with not null dropCntrPtr.
    Expected: GT_OK.
    1.2. Call api with wrong dropCntrPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U32  dropCntr;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null dropCntrPtr.
            Expected: GT_OK.
        */
        st = cpssDxChNetIfCpuCodeRateLimiterDropCntrGet(dev, &dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call api with wrong dropCntrPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfCpuCodeRateLimiterDropCntrGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropCntrPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeRateLimiterDropCntrGet(dev, &dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeRateLimiterDropCntrGet(dev, &dropCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterDropCntrSet
(
    IN  GT_U8  dev,
    IN  GT_U32 dropCntrVal
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeRateLimiterDropCntrSet)
{
/*
    ITERATE_DEVICES(DxCh2 and above)
    1.1. Call with dropCntrVal[0 / 100 / 555],
    Expected: GT_OK.
    1.2. Call cpssDxChNetIfCpuCodeRateLimiterDropCntrGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
*/
    GT_STATUS st = GT_OK;

    GT_U8  dev;
    GT_U32 dropCntr = 0;
    GT_U32 dropCntrGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with dropCntr[0 / 100 / 555],
            Expected: GT_OK.
        */
        /* call with dropCntr[0] */
        dropCntr = 0;

        st = cpssDxChNetIfCpuCodeRateLimiterDropCntrSet(dev, dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChNetIfCpuCodeRateLimiterDropCntrGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChNetIfCpuCodeRateLimiterDropCntrGet(dev, &dropCntrGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChNetIfCpuCodeRateLimiterDropCntrGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropCntr, dropCntrGet,
                "got another dropCntr than was set: %d", dev);

        /* call with dropCntrVal[100] */
        dropCntr = 100;

        st = cpssDxChNetIfCpuCodeRateLimiterDropCntrSet(dev, dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChNetIfCpuCodeRateLimiterDropCntrGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChNetIfCpuCodeRateLimiterDropCntrGet(dev, &dropCntrGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChNetIfCpuCodeRateLimiterDropCntrGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropCntr, dropCntrGet,
                "got another dropCntr than was set: %d", dev);

        /* call with dropCntr[555] */
        dropCntr = 555;

        st = cpssDxChNetIfCpuCodeRateLimiterDropCntrSet(dev, dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChNetIfCpuCodeRateLimiterDropCntrGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChNetIfCpuCodeRateLimiterDropCntrGet(dev, &dropCntrGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChNetIfCpuCodeRateLimiterDropCntrGet: %d ", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropCntr, dropCntrGet,
                "got another dropCntr than was set: %d", dev);
    }

    /* restore correct values */
    dropCntr = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeRateLimiterDropCntrSet(dev, dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeRateLimiterDropCntrSet(dev, dropCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet
(
    IN  GT_U8   dev,
    IN  GT_U32  rateLimiterIndex,
    OUT GT_U32  *packetCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet)
{
/*
    ITERATE_DEVICES(DxCh2 and above)
    1.1. Call with rateLimiterIndex[1 / 10 / 31],
    Expected: GT_OK.
    1.2. Call api with wrong rateLimiterIndex [0 / 32].
    Expected: NOT GT_OK.
    1.3. Call api with wrong packetCntrPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U32  rateLimiterIndex = 0;
    GT_U32  packetCntr;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with rateLimiterIndex[1 / 10 / 31],
            Expected: GT_OK.
        */

        /* call with rateLimiterIndex[1] */
        rateLimiterIndex = 1;

        st = cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, &packetCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with rateLimiterIndex[10] */
        rateLimiterIndex = 10;

        st = cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, &packetCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with rateLimiterIndex[31] */
        rateLimiterIndex = 31;

        st = cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, &packetCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call api with wrong rateLimiterIndex [0 / 32].
            Expected: NOT GT_OK.
        */
        /* call with rateLimiterIndex[32] */
        rateLimiterIndex = 32;

        st = cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, &packetCntr);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with rateLimiterIndex[0] */
        rateLimiterIndex = 0;

        st = cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, &packetCntr);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        rateLimiterIndex = 1;

        /*
            1.3. Call api with wrong packetCntrPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, packetCntrPtr = NULL", dev);
    }

    /* restore correct values */
    rateLimiterIndex = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, &packetCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet(dev, rateLimiterIndex, &packetCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet
(
    IN  GT_U8               dev,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT GT_U32              *dropCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(DxCh2 and above)
    1.1.1. Call with not null dropCntrPtr.
    Expected: GT_OK.
    1.1.2. Call api with wrong dropCntrPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8               dev;
    GT_PORT_GROUPS_BMP  portGroupsBmp = 1;
    GT_U32              dropCntr;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with not null dropCntrPtr.
                Expected: GT_OK.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet(dev,
                                     portGroupsBmp, &dropCntr);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call api with wrong dropCntrPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet(dev,
                                     portGroupsBmp, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropCntrPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet(dev,
                                     portGroupsBmp, &dropCntr);
            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet(dev,
                                     portGroupsBmp, &dropCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet(dev,
                                     portGroupsBmp, &dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet(dev,
                                     portGroupsBmp, &dropCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet
(
    IN  GT_U8              dev,
    IN  GT_PORT_GROUPS_BMP portGroupsBmp,
    IN  GT_U32             dropCntrVal
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(DxCh2 and above)
    1.1.1. Call with dropCntrVal[0 / 100 / 555],
    Expected: GT_OK.
    1.1.2. Call cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8              dev;
    GT_PORT_GROUPS_BMP portGroupsBmp = 1;
    GT_U32             dropCntr = 0;
    GT_U32             dropCntrGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with dropCntrVal[0 / 100 / 555],
                Expected: GT_OK.
            */

            /* call with dropCntrVal[0] */
            dropCntr = 0;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet(dev,
                                     portGroupsBmp, dropCntr);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet(dev,
                                     portGroupsBmp, &dropCntrGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(dropCntr, dropCntrGet,
                    "got another dropCntr than was set: %d", dev);

            /* call with dropCntrVal[100] */
            dropCntr = 100;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet(dev,
                                     portGroupsBmp, dropCntr);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet(dev,
                                     portGroupsBmp, &dropCntrGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(dropCntr, dropCntrGet,
                    "got another dropCntr than was set: %d", dev);

            /* call with dropCntrVal[555] */
            dropCntr = 555;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet(dev,
                                     portGroupsBmp, dropCntr);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet(dev,
                                     portGroupsBmp, &dropCntrGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(dropCntr, dropCntrGet,
                    "got another dropCntr than was set: %d", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet(dev,
                                     portGroupsBmp, dropCntr);
            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet(dev,
                                     portGroupsBmp, dropCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* restore correct values */
    dropCntr = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet(dev,
                                     portGroupsBmp, dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet(dev,
                                     portGroupsBmp, dropCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet
(
    IN  GT_U8               dev,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32              rateLimiterIndex,
    OUT GT_U32              *packetCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(DxCh2 and above)
    1.1.1. Call with rateLimiterIndex[1 / 10 / 31],
    Expected: GT_OK.
    1.1.2. Call api with wrong rateLimiterIndex [1 / 32].
    Expected: NOT GT_OK.
    1.1.3. Call api with wrong packetCntrPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8               dev;
    GT_PORT_GROUPS_BMP  portGroupsBmp = 1;
    GT_U32              rateLimiterIndex = 0;
    GT_U32              packetCntr;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with rateLimiterIndex[1 / 10 / 31],
                Expected: GT_OK.
            */

            /* call with rateLimiterIndex[1] */
            rateLimiterIndex = 1;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet(dev,
                            portGroupsBmp, rateLimiterIndex, &packetCntr);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call with rateLimiterIndex[10] */
            rateLimiterIndex = 10;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet(dev,
                            portGroupsBmp, rateLimiterIndex, &packetCntr);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call with rateLimiterIndex[31] */
            rateLimiterIndex = 31;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet(dev,
                            portGroupsBmp, rateLimiterIndex, &packetCntr);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call api with wrong rateLimiterIndex [0 / 32].
                Expected: NOT GT_OK.
            */
            /* call with rateLimiterIndex[31] */
            rateLimiterIndex = 32;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet(dev,
                            portGroupsBmp, rateLimiterIndex, &packetCntr);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call with rateLimiterIndex[0] */
            rateLimiterIndex = 0;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet(dev,
                            portGroupsBmp, rateLimiterIndex, &packetCntr);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            rateLimiterIndex = 1;

            /*
                1.1.3. Call api with wrong packetCntrPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet(dev,
                            portGroupsBmp, rateLimiterIndex, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, packetCntrPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet(dev,
                            portGroupsBmp, rateLimiterIndex, &packetCntr);
            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet(dev,
                            portGroupsBmp, rateLimiterIndex, &packetCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* restore correct values */
    rateLimiterIndex = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet(dev,
                            portGroupsBmp, rateLimiterIndex, &packetCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet(dev,
                            portGroupsBmp, rateLimiterIndex, &packetCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet
(
    IN  GT_U8                dev,
    IN  GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN  GT_U32               rateLimiterIndex,
    OUT GT_U32               *windowSizePtr,
    OUT GT_U32               *pktLimitPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(DxCh2 and above)
    1.1.1. Call with rateLimiterIndex[1 / 10 / 31],
    Expected: GT_OK.
    1.1.2. Call api with wrong rateLimiterIndex [0 / 32].
    Expected: NOT GT_OK.
    1.1.3. Call api with wrong windowSizePtr [NULL].
    Expected: GT_BAD_PTR.
    1.1.4. Call api with wrong pktLimitPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8                dev;
    GT_PORT_GROUPS_BMP   portGroupsBmp = 1;
    GT_U32               rateLimiterIndex = 1;
    GT_U32               windowSize;
    GT_U32               pktLimit;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with rateLimiterIndex[1 / 10 / 31],
                Expected: GT_OK.
            */

            /* call with rateLimiterIndex[1] */
            rateLimiterIndex = 1;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(dev,
                   portGroupsBmp, rateLimiterIndex, &windowSize, &pktLimit);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call with rateLimiterIndex[10] */
            rateLimiterIndex = 10;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(dev,
                   portGroupsBmp, rateLimiterIndex, &windowSize, &pktLimit);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call with rateLimiterIndex[31] */
            rateLimiterIndex = 31;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(dev,
                   portGroupsBmp, rateLimiterIndex, &windowSize, &pktLimit);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call api with wrong rateLimiterIndex [0 / 32].
                Expected: NOT GT_OK.
            */
            /* call with rateLimiterIndex[32] */
            rateLimiterIndex = 32;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(dev,
                   portGroupsBmp, rateLimiterIndex, &windowSize, &pktLimit);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call with rateLimiterIndex[0] */
            rateLimiterIndex = 0;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(dev,
                   portGroupsBmp, rateLimiterIndex, &windowSize, &pktLimit);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            rateLimiterIndex = 1;

            /*
                1.1.3. Call api with wrong windowSizePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(dev,
                   portGroupsBmp, rateLimiterIndex, NULL, &pktLimit);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, windowSizePtr = NULL", dev);

            /*
                1.1.4. Call api with wrong pktLimitPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(dev,
                   portGroupsBmp, rateLimiterIndex, &windowSize, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pktLimitPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(dev,
                   portGroupsBmp, rateLimiterIndex, &windowSize, &pktLimit);
            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(dev,
                   portGroupsBmp, rateLimiterIndex, &windowSize, &pktLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* restore correct values */
    rateLimiterIndex = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(dev,
                   portGroupsBmp, rateLimiterIndex, &windowSize, &pktLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(dev,
                   portGroupsBmp, rateLimiterIndex, &windowSize, &pktLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet
(
    IN  GT_U8              dev,
    IN  GT_PORT_GROUPS_BMP portGroupsBmp,
    IN  GT_U32             rateLimiterIndex,
    IN  GT_U32             windowSize,
    IN  GT_U32             pktLimit
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(DxCh2 and above)
    1.1.1. Call with rateLimiterIndex[1 / 10 / 31],
                   windowSize[0 / 100 / 0xFFF],
                   pktLimit[0 / 100 / 0xFFFF],
    Expected: GT_OK.
    1.1.2. Call cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.1.3. Call api with wrong rateLimiterIndex [0 / 32].
    Expected: NOT GT_OK.
    1.1.4. Call api with wrong windowSize [0xFFF + 1].
    Expected: NOT GT_OK.
    1.1.5. Call api with wrong pktLimit [0xFFFF + 1].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8              dev;
    GT_PORT_GROUPS_BMP portGroupsBmp = 1;
    GT_U32             rateLimiterIndex = 1;
    GT_U32             windowSize = 0;
    GT_U32             pktLimit = 0;
    GT_U32             windowSizeGet = 0;
    GT_U32             pktLimitGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with rateLimiterIndex[1 / 10 / 31],
                               windowSize[0 / 100 / 0xFFF],
                               pktLimit[0 / 100 / 0xFFFF],
                Expected: GT_OK.
            */
            /* call with rateLimiterIndex[1], windowSize[0], pktLimit[0] */
            rateLimiterIndex = 1;
            windowSize = 0;
            pktLimit = 0;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(dev,
                   portGroupsBmp, rateLimiterIndex, windowSize, pktLimit);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(dev,
                   portGroupsBmp, rateLimiterIndex, &windowSizeGet, &pktLimitGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(windowSize, windowSizeGet,
                           "got another windowSize then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pktLimit, pktLimitGet,
                           "got another pktLimit then was set: %d", dev);

            /* call with rateLimiterIndex[10], windowSize[100], pktLimit[100] */
            rateLimiterIndex = 10;
            windowSize = 100;
            pktLimit = 100;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(dev,
                   portGroupsBmp, rateLimiterIndex, windowSize, pktLimit);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(dev,
                   portGroupsBmp, rateLimiterIndex, &windowSizeGet, &pktLimitGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(windowSize, windowSizeGet,
                           "got another windowSize then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pktLimit, pktLimitGet,
                           "got another pktLimit then was set: %d", dev);

            /* call with rateLimiterIndex[31], windowSize[0xFFF], pktLimit[0xFFFF] */
            rateLimiterIndex = 31;
            windowSize = 0xFFF;
            pktLimit = 0xFFFF;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(dev,
                   portGroupsBmp, rateLimiterIndex, windowSize, pktLimit);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(dev,
                   portGroupsBmp, rateLimiterIndex, &windowSizeGet, &pktLimitGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(windowSize, windowSizeGet,
                           "got another windowSize then was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pktLimit, pktLimitGet,
                           "got another pktLimit then was set: %d", dev);

            /*
                1.1.3. Call api with wrong rateLimiterIndex [0 / 32].
                Expected: NOT GT_OK.
            */
            /* call with rateLimiterIndex[0] */
            rateLimiterIndex = 0;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(dev,
                   portGroupsBmp, rateLimiterIndex, windowSize, pktLimit);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call with rateLimiterIndex[32] */
            rateLimiterIndex = 32;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(dev,
                   portGroupsBmp, rateLimiterIndex, windowSize, pktLimit);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            rateLimiterIndex = 1;

            /*
                1.1.4. Call api with wrong windowSize [0xFFF + 1].
                Expected: NOT GT_OK.
            */
            windowSize = 0xFFF + 1;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(dev,
                   portGroupsBmp, rateLimiterIndex, windowSize, pktLimit);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            windowSize = 0;

            /*
                1.1.5. Call api with wrong pktLimit [0xFFFF + 1].
                Expected: NOT GT_OK.
            */
            pktLimit = 0xFFFF + 1;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(dev,
                   portGroupsBmp, rateLimiterIndex, windowSize, pktLimit);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            pktLimit = 0;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(dev,
                   portGroupsBmp, rateLimiterIndex, windowSize, pktLimit);
            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(dev,
                   portGroupsBmp, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* restore correct values */
    rateLimiterIndex = 1;
    windowSize = 0;
    pktLimit = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(dev,
                   portGroupsBmp, rateLimiterIndex, windowSize, pktLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(dev,
                   portGroupsBmp, rateLimiterIndex, windowSize, pktLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet
(
    IN  GT_U8               dev,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT GT_U32              *windowResolutionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(DxCh2 and above)
    1.1.1. Call with not nullportGroupsBmp.
    Expected: GT_OK.
    1.1.2. Call api with wrong windowResolutionPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8               dev;
    GT_PORT_GROUPS_BMP  portGroupsBmp = 1;
    GT_U32              windowResolution;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with not nullportGroupsBmp.
                Expected: GT_OK.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet(dev,
                                     portGroupsBmp, &windowResolution);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call api with wrong windowResolutionPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet(dev,
                                     portGroupsBmp, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                    "%d, windowResolutionPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet(dev,
                                     portGroupsBmp, &windowResolution);
            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet(dev,
                                     portGroupsBmp, &windowResolution);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* restore correct values */

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet(dev,
                                     portGroupsBmp, &windowResolution);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet(dev,
                                     portGroupsBmp, &windowResolution);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet
(
    IN  GT_U8              dev,
    IN  GT_PORT_GROUPS_BMP portGroupsBmp,
    IN  GT_U32             windowResolution
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(DxCh2 and above)
    1.1.1. Call with windowResolution[1 * granularity / 2 * granularity]
    Expected: GT_OK.
    1.1.2. Call cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.1.3. Call with wrong windowResolution[0].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U32    portGroupId;

    GT_U8              dev;
    GT_PORT_GROUPS_BMP portGroupsBmp = 1;
    GT_U32             windowResolution = 0;
    GT_U32             windowResolutionGet = 0;

    GT_U32 coreClock   = 0;
    GT_U32 granularity = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting device coreClock */
        st = prvUtfCoreClockGet(dev, &coreClock);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCoreClockGet: %d", dev);

        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(0, coreClock,
                            "prvUtfCoreClockGet: coreClock = 0, dev = %d", dev);
        if(0 == coreClock)
        {
            continue;
        }

        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with windowResolution[1 * granularity / 2 * granularity]
                Expected: GT_OK.
            */
            /* iterate with windowResolution = 1 * granularity */
            granularity      = (1000 * 32) / coreClock;
            windowResolution = 1 * granularity;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet(dev,
                                     portGroupsBmp, windowResolution);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet(dev,
                                     portGroupsBmp, &windowResolutionGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                "cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(windowResolution, windowResolutionGet,
                           "got another windowResolution then was set: %d", dev);

            /* iterate with windowResolution = 2 * granularity */
            granularity      = (1000 * 32) / coreClock;
            windowResolution = 2 * granularity;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet(dev,
                                     portGroupsBmp, windowResolution);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.1.2. Call cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet(dev,
                                     portGroupsBmp, &windowResolutionGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                "cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(windowResolution, windowResolutionGet,
                           "got another windowResolution then was set: %d", dev);

            /*
                1.1.3. Call with wrong windowResolution[0].
                Expected: NOT GT_OK.
            */
            windowResolution = 0;

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet(dev,
                                     portGroupsBmp, windowResolution);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            windowResolution = 1 * granularity;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet(dev,
                                     portGroupsBmp, windowResolution);
            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet(dev,
                                     portGroupsBmp, windowResolution);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    /* restore correct values */
    windowResolution = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet(dev,
                                     portGroupsBmp, windowResolution);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet(dev,
                                     portGroupsBmp, windowResolution);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChNetIf suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChNetIf)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfPrePendTwoBytesHeaderSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfPrePendTwoBytesHeaderGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfSdmaRxPacketGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfSdmaRxCountersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfSdmaRxErrorCountGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfSdmaRxQueueEnable)       /*  not Get function */
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfSdmaTxQueueEnable)       /*  not Get function */
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfDsaTagBuild)             /*  not Get function */
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfDuplicateEnableSet)      /*  not Get function */
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfPortDuplicateToCpuSet)   /*  not Get function */
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfAppSpecificCpuCodeTcpSynSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfAppSpecificCpuCodeTcpSynGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeIpLinkLocalProtSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeIpLinkLocalProtGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeTableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeDesignatedDeviceTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeDesignatedDeviceTableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeRateLimiterTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeRateLimiterTableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfFromCpuDpSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfFromCpuDpGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfSdmaRxResourceErrorModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfDsaTagParse)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfInit)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfRemove)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfRxBufFree)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfRxBufFreeWithSize)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfSdmaTxPacketSend)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfSdmaSyncTxPacketSend)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfTxBufferQueueGet)

    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeRateLimiterDropCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeRateLimiterDropCntrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet)

UTF_SUIT_END_TESTS_MAC(cpssDxChNetIf)

/*----------------------------------------------------------------------------*/
/*
    Set dsaInfoPtr->dsaInfo.toCpu structure.
*/
static void prvSetDsaInfoToCpu
(
    OUT CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr
)
{
    /*
    dsaInfoPtr{dsaInfo{toCpu{
                            isEgressPipe[GT_FALSE],
                            isTagged [GT_FALSE],
                            devNum [0],
                            srcIsTrunk[GT_FALSE],
                            interface{portNum[0]},
                            cpuCode [CPSS_NET_CONTROL_BPDU_E],
                            wasTruncated [GT_FALSE],
                            originByteCount[0],
                            }
    */
    dsaInfoPtr->dsaInfo.toCpu.isEgressPipe = GT_FALSE;

    dsaInfoPtr->dsaInfo.toCpu.isTagged   = GT_FALSE;
    dsaInfoPtr->dsaInfo.toCpu.devNum     = 0;
    dsaInfoPtr->dsaInfo.toCpu.srcIsTrunk = GT_FALSE;

    dsaInfoPtr->dsaInfo.toCpu.interface.srcTrunkId = 0;
    dsaInfoPtr->dsaInfo.toCpu.interface.portNum    = NETIF_VALID_PHY_PORT_CNS;

    dsaInfoPtr->dsaInfo.toCpu.cpuCode         = CPSS_NET_CONTROL_BPDU_E;
    dsaInfoPtr->dsaInfo.toCpu.wasTruncated    = GT_FALSE;
    dsaInfoPtr->dsaInfo.toCpu.originByteCount = 0;
}

/*----------------------------------------------------------------------------*/
/*
    Set dsaInfoPtr->dsaInfo.fromCpu structure.
*/
static void prvSetDsaInfoFromCpu
(
    OUT CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr
)
{
    /*
    dsaInfoPtr{dsaInfo{fromCpu{
                                dstInterface [CPSS_INTERFACE_VIDX_E],
                                tc [7],
                                dp [CPSS_DP_GREEN_E],
                                egrFilterEn [GT_FALSE],
                                cascadeControl [GT_FALSE],
                                egrFilterRegistered [GT_FALSE],
                                srcId [0],
                                srcDev [0],
                                extDestInfo{
                                            excludeInterface [GT_FALSE],
                                            excludedInterface{
                                                                type[CPSS_INTERFACE_VIDX_E],
                                                                vidx[100]}
                                            mirrorToAllCPUs[GT_FALSE]}
                            }
    */
    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;
    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vidx = 100;

    dsaInfoPtr->dsaInfo.fromCpu.tc = 7;
    dsaInfoPtr->dsaInfo.fromCpu.dp = CPSS_DP_GREEN_E;

    dsaInfoPtr->dsaInfo.fromCpu.egrFilterEn         = GT_FALSE;
    dsaInfoPtr->dsaInfo.fromCpu.cascadeControl      = GT_FALSE;
    dsaInfoPtr->dsaInfo.fromCpu.egrFilterRegistered = GT_FALSE;

    dsaInfoPtr->dsaInfo.fromCpu.srcId  = 0;
    dsaInfoPtr->dsaInfo.fromCpu.srcDev = 0;

    dsaInfoPtr->dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface       = GT_FALSE;
    dsaInfoPtr->dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type = CPSS_INTERFACE_VIDX_E;
    dsaInfoPtr->dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.vidx = 100;
    dsaInfoPtr->dsaInfo.fromCpu.extDestInfo.multiDest.mirrorToAllCPUs        = GT_FALSE;
}

/*----------------------------------------------------------------------------*/
/*
    Set dsaInfoPtr->dsaInfo.toAnalizer structure.
*/
static void prvSetDsaInfoToAnalizer
(
    OUT CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr
)
{
    /*
    dsaInfoPtr{dsaInfo{toAnalizer{
                                    rxSniffer [GT_FALSE],
                                    isTagged [GT_FALSE],
                                    devPort{
                                            devNum [0],
                                            portNum [0]
                                }
    */
    dsaInfoPtr->dsaInfo.toAnalyzer.rxSniffer = GT_FALSE;
    dsaInfoPtr->dsaInfo.toAnalyzer.isTagged  = GT_FALSE;

    dsaInfoPtr->dsaInfo.toAnalyzer.devPort.devNum  = 0;
    dsaInfoPtr->dsaInfo.toAnalyzer.devPort.portNum = NETIF_VALID_PHY_PORT_CNS;
}

/*----------------------------------------------------------------------------*/
/*
    Set dsaInfoPtr->dsaInfo.forward structure.
*/
static void prvSetDsaInfoForward
(
    OUT CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr
)
{
    /*
    dsaInfoPtr{dsaInfo{forward{
                                srcIsTagged [GT_FALSE],
                                srcDev [0],
                                srcIsTrunk [GT_FALSE],
                                source{ portNum [0] }
                                srcId [0],
                                egrFilterRegistered [GT_FALSE],
                                wasRouted [GT_FALSE],
                                qosProfileIndex [10],
                                dstInterface{
                                             type [CPSS_INTERFACE_VIDX_E],
                                             dstInterface.vidx [100]
                                             }
                                }
    */

    dsaInfoPtr->dsaInfo.forward.srcIsTagged = GT_FALSE;
    dsaInfoPtr->dsaInfo.forward.srcDev      = 0;
    dsaInfoPtr->dsaInfo.forward.srcIsTrunk  = GT_FALSE;

    dsaInfoPtr->dsaInfo.forward.source.portNum = NETIF_VALID_PHY_PORT_CNS;

    dsaInfoPtr->dsaInfo.forward.srcId = 0;

    dsaInfoPtr->dsaInfo.forward.egrFilterRegistered = GT_FALSE;
    dsaInfoPtr->dsaInfo.forward.wasRouted           = GT_FALSE;
    dsaInfoPtr->dsaInfo.forward.qosProfileIndex     = 10;
    dsaInfoPtr->dsaInfo.forward.dstInterface.type   = CPSS_INTERFACE_VIDX_E;
    dsaInfoPtr->dsaInfo.forward.dstInterface.vidx   = 100;
}

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
*       coreClock       -  core clock
*
*       GT_OK           -   Get core clock was OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfCoreClockGet
(
    IN  GT_U8       dev,
    OUT GT_U32      *coreClock
)
{
    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(coreClock);

    /* check if dev active and from DxChx family */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    *coreClock = PRV_CPSS_PP_MAC(dev)->coreClock;

    return(GT_OK);
}
