/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmNetIfUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmNetIf
*       Include ExMxPm network interface API functions
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>
#include <cpss/exMxPm/exMxPmGen/monitor/cpssExMxPmMirror.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/* defines */

/* Default valid value for port id */
#define NETIF_VALID_PHY_PORT_CNS    0

/* Invalid enum */
#define NETIF_INVALID_ENUM_CNS      0x5AAAAAA5

/* Internal functions forward declaration */

/*  Set dsaInfoPtr->dsaInfo.toCpu structure.         */
static void prvSetDsaInfoToCpu      ( OUT CPSS_EXMXPM_NET_DSA_PARAMS_STC * dsaInfoPtr);

/*  Set dsaInfoPtr->dsaInfo.fromCpu structure.       */
static void prvSetDsaInfoFromCpu    ( OUT CPSS_EXMXPM_NET_DSA_PARAMS_STC * dsaInfoPtr);

/*  Set dsaInfoPtr->dsaInfo.toAnalizer structure.    */
static void prvSetDsaInfoToAnalizer ( IN  GT_U8   devNum,OUT CPSS_EXMXPM_NET_DSA_PARAMS_STC * dsaInfoPtr);

/* Set dsaInfoPtr->dsaInfo.forward structure.        */
static void prvSetDsaInfoForward    ( OUT CPSS_EXMXPM_NET_DSA_PARAMS_STC * dsaInfoPtr);

#define BUFF_SEGMENT_LEN_CNS 20

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNetIfPrePendTwoBytesHeaderSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNetIfPrePendTwoBytesHeaderSet)
{
/*
    ITERATE_DEVICE (EXMXPM)
    1.1. Call with enable [GT_FALSE / GT_TRUE]
    Expected: GT_OK.
    1.2. Call cpssExMxPmNetIfPrePendTwoBytesHeaderGet with not NULL enablePtr
    Expected: GT_OK and the same enablePtr as was set.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL  enable    = GT_FALSE;
    GT_BOOL  enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_FALSE / GT_TRUE]
            Expected: GT_OK.
        */
        /* iterate with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssExMxPmNetIfPrePendTwoBytesHeaderSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmNetIfPrePendTwoBytesHeaderGet with not NULL enablePtr
            Expected: GT_OK and the same enablePtr as was set
        */
        st = cpssExMxPmNetIfPrePendTwoBytesHeaderGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmNetIfPrePendTwoBytesHeaderGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);

        /* iterate with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssExMxPmNetIfPrePendTwoBytesHeaderSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmNetIfPrePendTwoBytesHeaderGet with not NULL enablePtr
            Expected: GT_OK and the same enablePtr as was set
        */
        st = cpssExMxPmNetIfPrePendTwoBytesHeaderGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmNetIfPrePendTwoBytesHeaderGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNetIfPrePendTwoBytesHeaderSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNetIfPrePendTwoBytesHeaderSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNetIfPrePendTwoBytesHeaderGet
(
    IN GT_U8     devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNetIfPrePendTwoBytesHeaderGet)
{
/*
    ITERATE_DEVICE (EXMXPM)
    1.1. Call with not NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmNetIfPrePendTwoBytesHeaderGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfPrePendTwoBytesHeaderGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNetIfPrePendTwoBytesHeaderGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNetIfPrePendTwoBytesHeaderGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNetIfSdmaRxPacketGet
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    INOUT GT_U32                            *numOfBuffPtr,
    OUT GT_U8                               *packetBuffs[],
    OUT GT_U32                              buffLen[],
    OUT CPSS_EXMXPM_NET_RX_PARAMS_STC  *rxParamsPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPmNetIfSdmaRxPacketGet)
{
/*
    ITERATE_DEVICE (EXMXPM)
    1.1. Call with queueIdx [0 / 7], numOfBuffPtr [1] and not NULL pointers
    Expected: GT_OK.
    1.2. Call with queueIdx [8] and other params as in 1.1.
    Expected: NOT GT_OK.
    1.3. Call with numOfBuffPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with packetBuffs [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with buffLen [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call with rxParamsPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_NET_RX_PARAMS_STC            rxParams;
    GT_U8                                    *packetBuffs[5];
    GT_U32                                   buffLen[5];
    GT_U32                                   numOfBuff = 5;
    GT_U8                                    queueIdx = 0;
    GT_U32                                   i,j;
    GT_U32                                   bufferSize = BUFF_SEGMENT_LEN_CNS;

    cpssOsBzero((GT_VOID*) &rxParams, sizeof(rxParams));
    cpssOsBzero((GT_VOID*) &buffLen, sizeof(buffLen));

    /* allocate buffers for the array of buffers */
    for (i = 0 ; i < 5 ; i++)
    {
        packetBuffs[i] = (GT_U8*)cpssOsMalloc(sizeof(GT_U8) * bufferSize);
        if (packetBuffs[i]==NULL)
        {
            for (j = 0 ; j < i ; j++)
            {
                cpssOsFree(packetBuffs[j]);
            }
            return;
        }
    }

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with queueIdx [0 / 7], numOfBuffPtr [1] and not NULL pointers
            Expected: GT_OK.
        */
        /* iterate with queueIdx = 0 */
        queueIdx  = 0;
        numOfBuff = 1;

        st = cpssExMxPmNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, packetBuffs, buffLen, &rxParams);
        if (st != GT_OK)
        {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NO_MORE, st, dev, queueIdx);
        }

        /* iterate with queueIdx = 7 */
        queueIdx  = 7;
        numOfBuff = 1;

        st = cpssExMxPmNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, packetBuffs, buffLen, &rxParams);
        if (st != GT_OK)
        {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NO_MORE, st, dev, queueIdx);
        }

        /*
            1.2. Call with queueIdx [8] and other params as in 1.1.
            Expected: NOT GT_OK.
        */
        queueIdx  = 8;

        st = cpssExMxPmNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, packetBuffs, buffLen, &rxParams);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, queueIdx);

        queueIdx  = 0;

        /*
            1.3. Call with numOfBuffPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaRxPacketGet(dev, queueIdx, NULL, packetBuffs, buffLen, &rxParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfBuffPtr = NULL", dev);

        /*
            1.4. Call with packetBuffs [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff,NULL, buffLen, &rxParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, packetBuffsPtr = NULL", dev);

        /*
            1.5. Call with buffLen [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, packetBuffs, NULL, &rxParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, buffLenPtr = NULL", dev);

        /*
            1.6. Call with rxParamsPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, packetBuffs, buffLen, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rxParamsPtr = NULL", dev);
    }

    queueIdx  = 0;
    numOfBuff = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, packetBuffs, buffLen, &rxParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNetIfSdmaRxPacketGet(dev, queueIdx, &numOfBuff, packetBuffs, buffLen, &rxParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

    /* free the buffers in the buffer list array */
    for (i = 0 ; i < 5 ; i++)
    {
        cpssOsFree(packetBuffs[i]);
    }
    return;
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNetIfSdmaRxCountersGet
(
    IN    GT_U8                                 devNum,
    IN    GT_U8                                 queueIdx,
    OUT   CPSS_EXMXPM_NET_SDMA_RX_COUNTERS_STC  *rxCountersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNetIfSdmaRxCountersGet)
{
/*
    ITERATE_DEVICE (EXMXPM)
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

    GT_U8                                queueIdx = 0;
    CPSS_EXMXPM_NET_SDMA_RX_COUNTERS_STC rxCounters;


    cpssOsBzero((GT_VOID*) &rxCounters, sizeof(rxCounters));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with queueIdx[7]
                           and not NULL rxCountersPtr.
            Expected: GT_OK.
        */
        queueIdx = 7;

        st = cpssExMxPmNetIfSdmaRxCountersGet(dev, queueIdx, &rxCounters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, queueIdx);

        /*
            1.2. Call with out of range queueIdx[8]
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        queueIdx = 8;

        st = cpssExMxPmNetIfSdmaRxCountersGet(dev, queueIdx, &rxCounters);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, queueIdx);

        queueIdx = 0;

        /*
            1.3. Call with rxCountersPtr [NULL]
                           and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaRxCountersGet(dev, queueIdx, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rxCountersPtr = NULL", dev);
    }

    queueIdx = 7;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNetIfSdmaRxCountersGet(dev, queueIdx, &rxCounters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNetIfSdmaRxCountersGet(dev, queueIdx, &rxCounters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNetIfSdmaRxErrorCountGet
(
    IN  GT_U8                                    devNum,
    OUT CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC      *rxErrCountPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNetIfSdmaRxErrorCountGet)
{
/*
    ITERATE_DEVICE (EXMXPM)
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
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL rxErrCountPtr;
            Expected: GT_OK.
        */
        st = cpssExMxPmNetIfSdmaRxErrorCountGet(dev, &rxErrCount);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with rxErrCountPtr [NULL];
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaRxErrorCountGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rxErrCountPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNetIfSdmaRxErrorCountGet(dev, &rxErrCount);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNetIfSdmaRxErrorCountGet(dev, &rxErrCount);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNetIfSdmaTxSyncPacketSend
(
    IN GT_U8                                    devNum,
    IN CPSS_EXMXPM_NET_TX_PARAMS_STC       *pcktParamsPtr,
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
);
*/
UTF_TEST_CASE_MAC(cpssExMxPmNetIfSdmaTxSyncPacketSend)
{
/*
    ITERATE_DEVICE (EXMXPM)
    {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E}
    1.1. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                cookie [not NULL],
                                sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                           txQueue [0 / 7],
                                           evReqHndl [0],
                                           invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                dsaParam{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                                dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E,
                                dsaInfo{toCpu {set to cpu}} }
                   buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                   buffLenList [5, 5, 5, 5, 5],
                   numOfBufs [5]
    1.2. Call with pcktParamsPtr->sdmaInfo.txQueue [8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with pcktParamsPtr->dsaParam.commonParams->dsaCmd[0x5AAAAAA5]and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range pcktParamsPtr->dsaParam.commonParams.vpt [8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range pcktParamsPtr->dsaParam.commonParams.cfiBit [2] and other params from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range pcktParamsPtr->dsaParam.commonParams.vid [4096] and other params from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with pcktParamsPtr->dsaParam.dsaInfo.toCpu.devNum[PRV_CPSS_MAX_PP_DEVICES_CNS] and other params from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with isEgressPipe [GT_FALSE],
                   srcIsTrunk[GT_FALSE],
                   pcktParamsPtr->dsaParam.dsaInfo.toCpu.interface.port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.10. Call with pcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode[0x5AAAAAA5] and other params from 1.1.
    Expected: NOT GT_OK.

    {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E}
    1.11. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                cookie [not NULL],
                                sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                           txQueue [0 / 7],
                                           evReqHndl [0],
                                           invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                                dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E,
                                dsaInfo{fromCpu {set from cpu}}
                    buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                    buffLenList [5, 5, 5, 5, 5],
                    numOfBufs [5]
    Expected: GT_OK.
    1.12. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_TRUNK_E] (not supported)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.13. Call with out of range pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[0x5AAAAAA5]
                    and other params from 1.10.
    Extended: GT_BAD_PARAM.
    1.14. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.15. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.16. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.
    1.17. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.
    1.18. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.vidx [4096] (out of range)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.19. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VID_E]
                    pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.vidx [4096] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.
    1.21. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.

    {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_ANALYZER_E}
    1.24. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                cookie [not NULL],
                                sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                           txQueue [0 / 7],
                                           evReqHndl [0],
                                           invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                              dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_ANALYZER_E,
                              dsaInfo{toAnalyzer {set to analyzer}}
                    buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                    buffLenList [5, 5, 5, 5, 5],
                    numOfBufs [5]
    Expected: GT_OK.
    1.26 Call with pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.rx_sniff [GT_TRUE] (not extended rule)
                   pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                   and other params from 1.24
    Expected: NOT GT_OK.
    1.27 Call with pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.rx_sniff [GT_FALSE] (rxtended rule)
                   pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.devPort.portNum [31] (out of range)
                   and other params from 1.24
    Expected: NOT GT_OK.

    {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E}
    1.28. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                cookie [not NULL],
                                sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                           txQueue [0 / 7],
                                           evReqHndl [0],
                                           invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                              dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E,
                              dsaInfo{forward {set forward}}
                    buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                    buffLenList [5, 5, 5, 5, 5],
                    numOfBufs [5]
    Expected: GT_OK.
    1.29 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.srcDev [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                   and other params from 1.28.
    Expected: NOT GT_OK.
    1.30 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.srcIsTrunk [GT_FALSE]
                   pcktParamsPtr->dsaParam.dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                   and other params from 1.28.
    Expected: NOT GT_OK.
    1.31 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.srcIsTrunk [GT_TRUE]
                   pcktParamsPtr->dsaParam.dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                   and other params from 1.28.
    Expected: GT_OK.
    1.32 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type [CPSS_INTERFACE_TRUNK_E] (not supported)
                   and other params from 1.28.
    Expected: NOT GT_OK.
    1.34. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                    and other params from 1.28.
    Extended: NOT GT_OK.
    1.35. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                    and other params from 1.28.
    Extended: NOT GT_OK.
    1.36. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.37. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.38. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.vidx [4096] (out of range)
                    and other params from 1.28.
    Extended: NOT GT_OK.
    1.39. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VID_E]
                    pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.vidx [4096] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.41. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.42. Call with pcktParamsPtr->sdmaInfo.cookie [NULL]
                   and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.43 Call with pcktParamsPtr [NULL]
                   and other params from 1.28.
    Expected: GT_BAD_PTR.
    1.44 Call with buffList [NULL]
                   and other params from 1.28.
    Expected: GT_BAD_PTR.
    1.45 Call with buffLenList [NULL]
                   and other params from 1.28.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       Temp = 0;
    GT_U32      i,j;

    CPSS_EXMXPM_NET_TX_PARAMS_STC           pcktParams;
    GT_U8                                   *buffList[5];
    GT_U32                                  buffLenList[5];
    GT_U32                                  numOfBufs;
    GT_U32                                  bufferSize = BUFF_SEGMENT_LEN_CNS;
    GT_U8                                   *dmaBuffer;


    cpssOsBzero((GT_VOID*) &pcktParams, sizeof(pcktParams));
    cpssOsBzero((GT_VOID*) &buffLenList, sizeof(buffLenList));

    /* use pool? */
    j = (bufferSize + 3) & ~3; /* align size for allocation */
    dmaBuffer = cpssOsCacheDmaMalloc(j * 5);
    if (dmaBuffer == NULL)
    {
        return;
    }
    for (i = 0 ; i < 5 ; i++)
    {
        buffList[i] = dmaBuffer + (j * i);
        cpssOsMemSet(buffList[i], 0x00, sizeof(GT_U8) * bufferSize);
    }

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E}
            1.1. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                        cookie [not NULL],
                                        sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                                   txQueue [0 / 7],
                                                   evReqHndl [0],
                                                   invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                        dsaParam{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                        dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E,
                                        dsaInfo{toCpu {set to cpu}} }
                           buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                           buffLenList [5, 5, 5, 5, 5],
                           numOfBufs [5]
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_TRUE;
        pcktParams.cookie = &Temp;

        pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
        pcktParams.sdmaInfo.txQueue = 0;
        pcktParams.sdmaInfo.evReqHndl = 0;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 0;
        pcktParams.dsaParam.commonParams.cfiBit = 0;
        pcktParams.dsaParam.commonParams.vid    = 0;

        pcktParams.dsaParam.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E;

        /*prvSetDsaInfoToCpu(&pcktParams.dsaParam);*/
        prvSetDsaInfoFromCpu(&pcktParams.dsaParam);

        /* set all buffers to all 0 */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0x0, sizeof(GT_U8) * bufferSize);
        }

        buffLenList[0] = bufferSize;
        buffLenList[1] = bufferSize;
        buffLenList[2] = bufferSize;
        buffLenList[3] = bufferSize;
        buffLenList[4] = bufferSize;

        numOfBufs = 5;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* iterate with pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_FALSE;
        pcktParams.sdmaInfo.recalcCrc = GT_FALSE;
        pcktParams.sdmaInfo.txQueue = 7;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_FALSE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 7;
        pcktParams.dsaParam.commonParams.cfiBit = 1;
        pcktParams.dsaParam.commonParams.vid    = 100;

        /* set all buffers to all 0xff */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0xff, sizeof(GT_U8) * bufferSize);
        }

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with pcktParamsPtr->sdmaInfo.txQueue [8] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pcktParams.sdmaInfo.txQueue = 8;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pcktParams.sdmaInfo.txQueue = %d", dev, pcktParams.sdmaInfo.txQueue);

        pcktParams.sdmaInfo.txQueue = 0;

        /*
            1.3. Call with pcktParamsPtr->dsaParam.dsaCmd[0x5AAAAAA5]and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pcktParams.dsaParam.dsaCmd = NETIF_INVALID_ENUM_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, pcktParams.dsaParam.dsaCmd = %d",
                                         dev, pcktParams.dsaParam.dsaCmd);

        pcktParams.dsaParam.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E;

        /*
            1.4. Call with out of range pcktParamsPtr->dsaParam.commonParams.vpt [8] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.commonParams.vpt = 8;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.commonParams.vpt = %d",
                                         dev, pcktParams.dsaParam.commonParams.vpt);

        pcktParams.dsaParam.commonParams.vpt = 0;

        /*
            1.5. Call with out of range pcktParamsPtr->dsaParam.commonParams.cfiBit [2] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.commonParams.cfiBit = 2;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.commonParams.cfiBit = %d",
                                         dev, pcktParams.dsaParam.commonParams.cfiBit);

        pcktParams.dsaParam.commonParams.cfiBit = 0;

        /*
            1.6. Call with out of range pcktParamsPtr->dsaParam.commonParams.vid [4096] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.commonParams.vid = 4096;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.commonParams.vid = %d",
                                         dev, pcktParams.dsaParam.commonParams.vid);

        pcktParams.dsaParam.commonParams.vid = 100;

        /*
            1.7. Call with pcktParamsPtr->dsaParam.dsaInfo.toCpu.devNum[PRV_CPSS_MAX_PP_DEVICES_CNS] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.toCpu.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.toCpu.devNum = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.toCpu.devNum);

        pcktParams.dsaParam.dsaInfo.toCpu.devNum = 0;

        /*
            1.8. Call with isEgressPipe [GT_FALSE],
                           srcIsTrunk[GT_FALSE],
                           pcktParamsPtr->dsaParam.dsaInfo.toCpu.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.toCpu.isEgressPipe = GT_FALSE;
        pcktParams.dsaParam.dsaInfo.toCpu.portNum      = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->.dsaInfo.toCpu.portNum = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.toCpu.portNum);

        pcktParams.dsaParam.dsaInfo.toCpu.portNum = NETIF_VALID_PHY_PORT_CNS;

        /*
            1.10. Call with pcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode[0x5AAAAAA5] and other params from 1.1.
            Expected: NOT GT_OK.
        */

        pcktParams.dsaParam.dsaInfo.toCpu.cpuCode = NETIF_INVALID_ENUM_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->.dsaInfo.toCpu.cpuCode = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.toCpu.cpuCode);

        pcktParams.dsaParam.dsaInfo.toCpu.cpuCode = CPSS_NET_ETH_BRIDGED_LLT_E;

        /*
            {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E}
            1.11. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                        cookie [not NULL],
                                        sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                                   txQueue [0 / 7],
                                                   evReqHndl [0],
                                                   invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                        dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                        dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E,
                                        dsaInfo{fromCpu {set from cpu}}
                            buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                            buffLenList [5, 5, 5, 5, 5],
                            numOfBufs [5]
            Expected: GT_OK.
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E*/
        pcktParams.packetIsTagged = GT_TRUE;
        pcktParams.cookie = &Temp;

        pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
        pcktParams.sdmaInfo.txQueue = 0;
        pcktParams.sdmaInfo.evReqHndl = 0;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 0;
        pcktParams.dsaParam.commonParams.cfiBit = 0;
        pcktParams.dsaParam.commonParams.vid    = 0;

        pcktParams.dsaParam.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E;
        /* set all buffers to all 0 */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0xff, sizeof(GT_U8) * bufferSize);
        }

        numOfBufs = 5;

        prvSetDsaInfoFromCpu(&pcktParams.dsaParam);

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* iterate with pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_FALSE;
        pcktParams.sdmaInfo.recalcCrc = GT_FALSE;
        pcktParams.sdmaInfo.txQueue = 7;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_FALSE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 7;
        pcktParams.dsaParam.commonParams.cfiBit = 1;
        pcktParams.dsaParam.commonParams.vid    = 100;

        /* set all buffers to all 0xff */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0xff, sizeof(GT_U8) * bufferSize);
        }

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.12. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_TRUNK_E] (not supported)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d",
                                    dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        /*
            1.13. Call with out of range pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[0x5AAAAAA5]
                            and other params from 1.10.
            Extended: GT_BAD_PARAM.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = NETIF_INVALID_ENUM_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d",
                                    dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        /*
            1.14. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type            = CPSS_INTERFACE_PORT_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.fromCpu.dstInterface.type = %d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = 0;

        /*
            1.15. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type            = CPSS_INTERFACE_PORT_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.fromCpu.dstInterface.type = %d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = NETIF_VALID_PHY_PORT_CNS;

        /*
            1.16. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type           = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = %d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = 0;

        /*
            1.17. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type            = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = %d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = NETIF_VALID_PHY_PORT_CNS;

        /*
            1.18. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.vidx [4096] (out of range)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx = 4096;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = %d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx = 100;

        /*
            1.19. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VID_E]
                            pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.vidx [4096] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VID_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx = 4096;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = %d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx = 100;

        /*
            1.21. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vlanId = 4096;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = %d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vlanId = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vlanId);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vlanId = 100;

        /*
            {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_ANALYZER_E}
            1.24. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                        cookie [not NULL],
                                        sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                                   txQueue [0 / 7],
                                                   evReqHndl [0],
                                                   invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                        dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                      dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_ANALYZER_E,
                                      dsaInfo{toAnalyzer {set to analyzer}}
                            buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                            buffLenList [5, 5, 5, 5, 5],
                            numOfBufs [5]
            Expected: GT_OK.
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_TRUE;
        pcktParams.cookie = &Temp;

        pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
        pcktParams.sdmaInfo.txQueue = 0;
        pcktParams.sdmaInfo.evReqHndl = 0;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 0;
        pcktParams.dsaParam.commonParams.cfiBit = 0;
        pcktParams.dsaParam.commonParams.vid    = 0;

        /* set all buffers to all 0x00 */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0x00, sizeof(GT_U8) * bufferSize);
        }

        numOfBufs = 5;

        prvSetDsaInfoToAnalizer(dev,&pcktParams.dsaParam);

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* iterate with pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_FALSE;
        pcktParams.sdmaInfo.recalcCrc = GT_FALSE;
        pcktParams.sdmaInfo.txQueue = 7;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_FALSE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 7;
        pcktParams.dsaParam.commonParams.cfiBit = 1;
        pcktParams.dsaParam.commonParams.vid    = 100;

        /* set all buffers to all 0xff */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0xff, sizeof(GT_U8) * bufferSize);
        }

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.26 Call with pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.rx_sniff [GT_TRUE] (not extended rule)
                           pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                           and other params from 1.24
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.toAnalyzer.rxSniffer       = GT_TRUE;
        pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.toAnalyzer.rxSniffer = %d, pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = %d",
                                    dev, pcktParams.dsaParam.dsaInfo.toAnalyzer.rxSniffer, pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum);

        pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = 0;
        pcktParams.dsaParam.dsaInfo.toAnalyzer.rxSniffer       = GT_FALSE;

        /*
            1.27 Call with pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.rx_sniff [GT_FALSE] (rxtended rule)
                           pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.devPort.portNum [31] (out of range)
                           and other params from 1.24
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.commonParams.dsaTagType            = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;
        /* portNum can be up to is 6 bits in extended tag and up to 5 bits in reglar tag .
           32 is 6 bits value --> legal value*/
        pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = 32;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.commonParams.dsaTagType = %d, pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = %d",
                                    dev, pcktParams.dsaParam.commonParams.dsaTagType, pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum);

        pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = 0;

        /*
            {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E}
            1.28. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                        cookie [not NULL],
                                        sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                                   txQueue [0 / 7],
                                                   evReqHndl [0],
                                                   invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                        dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                      dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E,
                                      dsaInfo{forward {set forward}}
                            buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                            buffLenList [5, 5, 5, 5, 5],
                            numOfBufs [5]
            Expected: GT_OK.
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_TRUE;
        pcktParams.cookie = &Temp;

        pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
        pcktParams.sdmaInfo.txQueue = 0;
        pcktParams.sdmaInfo.evReqHndl = 0;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 0;
        pcktParams.dsaParam.commonParams.cfiBit = 0;
        pcktParams.dsaParam.commonParams.vid    = 0;

        pcktParams.dsaParam.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E;

        /* set all buffers to all 0 */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0x00, sizeof(GT_U8) * 5);
        }

        numOfBufs = 5;

        prvSetDsaInfoForward(&pcktParams.dsaParam);

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* iterate with pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_FALSE;
        pcktParams.sdmaInfo.recalcCrc = GT_FALSE;
        pcktParams.sdmaInfo.txQueue = 7;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_FALSE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 7;
        pcktParams.dsaParam.commonParams.cfiBit = 1;
        pcktParams.dsaParam.commonParams.vid    = 100;

        /* set all buffers to all 0xff */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0xff, sizeof(GT_U8) * 5);
        }

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.29 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.srcDev [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                           and other params from 1.28.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.srcDev = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.forward.srcDev = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.forward.srcDev);

        pcktParams.dsaParam.dsaInfo.forward.srcDev = 0;

        /*
            1.30 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.srcIsTrunk [GT_FALSE]
                           pcktParamsPtr->dsaParam.dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                           and other params from 1.28.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.srcIsTrunk     = GT_FALSE;
        pcktParams.dsaParam.dsaInfo.forward.source.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.forward.srcIsTrunk = %d, pcktParams.dsaParam.dsaInfo.forward.source.portNum = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.forward.srcIsTrunk, pcktParams.dsaParam.dsaInfo.forward.source.portNum);

        pcktParams.dsaParam.dsaInfo.forward.source.portNum = 0;

        /*
            1.31 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.srcIsTrunk [GT_TRUE]
                           pcktParamsPtr->dsaParam.dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                           and other params from 1.28.
            Expected: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.srcIsTrunk     = GT_TRUE;
        pcktParams.dsaParam.dsaInfo.forward.source.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.srcIsTrunk = %d, dsaInfoPtr->dsaInfo.forward.source.portNum = %d",
                                    dev, pcktParams.dsaParam.dsaInfo.forward.srcIsTrunk, pcktParams.dsaParam.dsaInfo.forward.source.portNum);

        pcktParams.dsaParam.dsaInfo.forward.source.portNum = 0;

        /*
            1.32 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type [CPSS_INTERFACE_TRUNK_E] (not supported)
                           and other params from 1.28.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.srcIsTrunk  = GT_TRUE;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type       = CPSS_INTERFACE_TRUNK_E;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        /*
            1.34. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                            and other params from 1.28.
            Extended: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type           = CPSS_INTERFACE_PORT_E;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type, pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum = 0;

        /*
            1.35. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                            and other params from 1.28.
            Extended: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type            = CPSS_INTERFACE_PORT_E;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type, pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum = 0;

        /*
            1.36. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type           = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum = %d",
                                     dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type, pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum = 0;

        /*
            1.37. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type            = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum = %d",
                                     dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type, pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum = 0;

        /*
            1.38. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.vidx [4096] (out of range)
                            and other params from 1.28.
            Extended: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.vidx = 4096;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.vidx = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type, pcktParams.dsaParam.dsaInfo.forward.dstInterface.vidx);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.vidx = 0;

        /*
            1.39. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VID_E]
                            pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.vidx [4096] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type = CPSS_INTERFACE_VID_E;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.vidx = 4096;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.vidx = %d",
                                     dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type, pcktParams.dsaParam.dsaInfo.forward.dstInterface.vidx);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.vidx = 0;

        /*
            1.41. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type   = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.vlanId = 4096;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.vlanId = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type, pcktParams.dsaParam.dsaInfo.forward.dstInterface.vlanId);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.vlanId = 100;

        /*
            1.42. Call with pcktParamsPtr->sdmaInfo.cookie [NULL]
                           and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        pcktParams.cookie = NULL;

        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pcktParams.sdmaInfo.cookie = NULL", dev);

        pcktParams.cookie = &Temp;
        /*
            1.43 Call with pcktParamsPtr [NULL]
                           and other params from 1.28.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, NULL, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pcktParamsPtr = NULL", dev);

        /*
            1.44 Call with buffList [NULL]
                           and other params from 1.28.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, NULL, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, buffListPtr = NULL", dev);

        /*
            1.45 Call with buffLenList [NULL]
                           and other params from 1.28.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, NULL, numOfBufs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, buffLenListPtr = NULL", dev);
    }

    pcktParams.packetIsTagged = GT_TRUE;
    pcktParams.cookie = &Temp;

    pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
    pcktParams.sdmaInfo.txQueue = 0;
    pcktParams.sdmaInfo.evReqHndl = 0;
    pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

    pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

    pcktParams.dsaParam.commonParams.vpt    = 0;
    pcktParams.dsaParam.commonParams.cfiBit = 0;
    pcktParams.dsaParam.commonParams.vid    = 0;

    pcktParams.dsaParam.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E;

    prvSetDsaInfoToCpu(&pcktParams.dsaParam);

    /* set all buffers to all 0 */
    for (i = 0 ; i < 5 ; i++)
    {
        cpssOsMemSet(buffList[i], 0x0, sizeof(GT_U8) * bufferSize);
    }

    buffLenList[0] = bufferSize;
    buffLenList[1] = bufferSize;
    buffLenList[2] = bufferSize;
    buffLenList[3] = bufferSize;
    buffLenList[4] = bufferSize;

    numOfBufs = 5;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNetIfSdmaTxSyncPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

    cpssOsCacheDmaFree(dmaBuffer);
    return;

}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNetIfSdmaTxPacketSend
(
    IN GT_U8                                    devNum,
    IN CPSS_EXMXPM_NET_TX_PARAMS_STC       *pcktParamsPtr,
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
);
*/
UTF_TEST_CASE_MAC(cpssExMxPmNetIfSdmaTxPacketSend)
{
/*
    ITERATE_DEVICE (EXMXPM)
    {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E}
    1.1. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                cookie [not NULL],
                                sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                           txQueue [0 / 7],
                                           evReqHndl [0],
                                           invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                dsaParam{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                                dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E,
                                dsaInfo{toCpu {set to cpu}} }
                   buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                   buffLenList [5, 5, 5, 5, 5],
                   numOfBufs [5]
    1.2. Call with pcktParamsPtr->sdmaInfo.txQueue [8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with pcktParamsPtr->dsaParam.commonParams->dsaCmd[0x5AAAAAA5]and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range pcktParamsPtr->dsaParam.commonParams.vpt [8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range pcktParamsPtr->dsaParam.commonParams.cfiBit [2] and other params from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range pcktParamsPtr->dsaParam.commonParams.vid [4096] and other params from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with pcktParamsPtr->dsaParam.dsaInfo.toCpu.devNum[PRV_CPSS_MAX_PP_DEVICES_CNS] and other params from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with isEgressPipe [GT_FALSE],
                   srcIsTrunk[GT_FALSE],
                   pcktParamsPtr->dsaParam.dsaInfo.toCpu.interface.port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.10. Call with pcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode[0x5AAAAAA5] and other params from 1.1.
    Expected: NOT GT_OK.

    {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E}
    1.11. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                cookie [not NULL],
                                sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                           txQueue [0 / 7],
                                           evReqHndl [0],
                                           invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                                dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E,
                                dsaInfo{fromCpu {set from cpu}}
                    buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                    buffLenList [5, 5, 5, 5, 5],
                    numOfBufs [5]
    Expected: GT_OK.
    1.12. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_TRUNK_E] (not supported)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.13. Call with out of range pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[0x5AAAAAA5]
                    and other params from 1.10.
    Extended: GT_BAD_PARAM.
    1.14. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.15. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.16. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.
    1.17. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.
    1.18. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.vidx [4096] (out of range)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.19. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VID_E]
                    pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.vidx [4096] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.
    1.21. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.

    {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_ANALYZER_E}
    1.24. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                cookie [not NULL],
                                sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                           txQueue [0 / 7],
                                           evReqHndl [0],
                                           invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                              dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_ANALYZER_E,
                              dsaInfo{toAnalyzer {set to analyzer}}
                    buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                    buffLenList [5, 5, 5, 5, 5],
                    numOfBufs [5]
    Expected: GT_OK.
    1.26 Call with pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.rx_sniff [GT_TRUE] (not extended rule)
                   pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                   and other params from 1.24
    Expected: NOT GT_OK.
    1.27 Call with pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.rx_sniff [GT_FALSE] (rxtended rule)
                   pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.devPort.portNum [31] (out of range)
                   and other params from 1.24
    Expected: NOT GT_OK.

    {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E}
    1.28. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                cookie [not NULL],
                                sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                           txQueue [0 / 7],
                                           evReqHndl [0],
                                           invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                              dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E,
                              dsaInfo{forward {set forward}}
                    buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                    buffLenList [5, 5, 5, 5, 5],
                    numOfBufs [5]
    Expected: GT_OK.
    1.29 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.srcDev [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                   and other params from 1.28.
    Expected: NOT GT_OK.
    1.30 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.srcIsTrunk [GT_FALSE]
                   pcktParamsPtr->dsaParam.dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                   and other params from 1.28.
    Expected: NOT GT_OK.
    1.31 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.srcIsTrunk [GT_TRUE]
                   pcktParamsPtr->dsaParam.dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                   and other params from 1.28.
    Expected: GT_OK.
    1.32 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type [CPSS_INTERFACE_TRUNK_E] (not supported)
                   and other params from 1.28.
    Expected: NOT GT_OK.
    1.34. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                    and other params from 1.28.
    Extended: NOT GT_OK.
    1.35. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                    and other params from 1.28.
    Extended: NOT GT_OK.
    1.36. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.37. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.38. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.vidx [4096] (out of range)
                    and other params from 1.28.
    Extended: NOT GT_OK.
    1.39. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VID_E]
                    pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.vidx [4096] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.41. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.42. Call with pcktParamsPtr->sdmaInfo.cookie [NULL]
                   and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.43 Call with pcktParamsPtr [NULL]
                   and other params from 1.28.
    Expected: GT_BAD_PTR.
    1.44 Call with buffList [NULL]
                   and other params from 1.28.
    Expected: GT_BAD_PTR.
    1.45 Call with buffLenList [NULL]
                   and other params from 1.28.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       Temp = 0;

    CPSS_EXMXPM_NET_TX_PARAMS_STC           pcktParams;
    GT_U8                                   *buffList[5];
    GT_U32                                  buffLenList[5];
    GT_U32                                  numOfBufs;
    GT_U32      i,j;
    GT_U32                                  bufferSize = BUFF_SEGMENT_LEN_CNS;
    GT_U8                                   *dmaBuffer;


    cpssOsBzero((GT_VOID*) &pcktParams, sizeof(pcktParams));
    cpssOsBzero((GT_VOID*) &buffLenList, sizeof(buffLenList));

    /* use pool? */
    j = (bufferSize + 3) & ~3; /* align size for allocation */
    dmaBuffer = cpssOsCacheDmaMalloc(j * 5);
    if (dmaBuffer == NULL)
    {
        return;
    }
    for (i = 0 ; i < 5 ; i++)
    {
        buffList[i] = dmaBuffer + (j * i);
    }

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E}
            1.1. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                        cookie [not NULL],
                                        sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                                   txQueue [0 / 7],
                                                   evReqHndl [0],
                                                   invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                        dsaParam{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                        dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E,
                                        dsaInfo{toCpu {set to cpu}} }
                           buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                           buffLenList [5, 5, 5, 5, 5],
                           numOfBufs [5]
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_TRUE;
        pcktParams.cookie = &Temp;

        pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
        pcktParams.sdmaInfo.txQueue = 0;
        pcktParams.sdmaInfo.evReqHndl = 0;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 0;
        pcktParams.dsaParam.commonParams.cfiBit = 0;
        pcktParams.dsaParam.commonParams.vid    = 0;

        pcktParams.dsaParam.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E;

        prvSetDsaInfoToCpu(&pcktParams.dsaParam);

        /* set all buffers to all 0 */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0x0, sizeof(GT_U8) * bufferSize);
        }

        buffLenList[0] = bufferSize;
        buffLenList[1] = bufferSize;
        buffLenList[2] = bufferSize;
        buffLenList[3] = bufferSize;
        buffLenList[4] = bufferSize;

        numOfBufs = 5;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* iterate with pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_FALSE;
        pcktParams.sdmaInfo.recalcCrc = GT_FALSE;
        pcktParams.sdmaInfo.txQueue = 7;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_FALSE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 7;
        pcktParams.dsaParam.commonParams.cfiBit = 1;
        pcktParams.dsaParam.commonParams.vid    = 100;

        /* set all buffers to all 0xff*/
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0xff, sizeof(GT_U8) * 5);
        }

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with pcktParamsPtr->sdmaInfo.txQueue [8] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pcktParams.sdmaInfo.txQueue = 8;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pcktParams.sdmaInfo.txQueue = %d", dev, pcktParams.sdmaInfo.txQueue);

        pcktParams.sdmaInfo.txQueue = 0;

        /*
            1.3. Call with pcktParamsPtr->dsaParam.dsaCmd[0x5AAAAAA5]and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pcktParams.dsaParam.dsaCmd = NETIF_INVALID_ENUM_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, pcktParams.dsaParam.dsaCmd = %d",
                                         dev, pcktParams.dsaParam.dsaCmd);

        pcktParams.dsaParam.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E;

        /*
            1.4. Call with out of range pcktParamsPtr->dsaParam.commonParams.vpt [8] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.commonParams.vpt = 8;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.commonParams.vpt = %d",
                                         dev, pcktParams.dsaParam.commonParams.vpt);

        pcktParams.dsaParam.commonParams.vpt = 0;

        /*
            1.5. Call with out of range pcktParamsPtr->dsaParam.commonParams.cfiBit [2] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.commonParams.cfiBit = 2;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.commonParams.cfiBit = %d",
                                         dev, pcktParams.dsaParam.commonParams.cfiBit);

        pcktParams.dsaParam.commonParams.cfiBit = 0;

        /*
            1.6. Call with out of range pcktParamsPtr->dsaParam.commonParams.vid [4096] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.commonParams.vid = 4096;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.commonParams.vid = %d",
                                         dev, pcktParams.dsaParam.commonParams.vid);

        pcktParams.dsaParam.commonParams.vid = 100;

        /*
            1.7. Call with pcktParamsPtr->dsaParam.dsaInfo.toCpu.devNum[PRV_CPSS_MAX_PP_DEVICES_CNS] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.toCpu.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.toCpu.devNum = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.toCpu.devNum);

        pcktParams.dsaParam.dsaInfo.toCpu.devNum = 0;

        /*
            1.8. Call with isEgressPipe [GT_FALSE],
                           isTagged[GT_FALSE],
                           pcktParamsPtr->dsaParam.dsaInfo.toCpu.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.toCpu.isEgressPipe = GT_FALSE;
        pcktParams.dsaParam.dsaInfo.toCpu.isTagged     = GT_FALSE;
        pcktParams.dsaParam.dsaInfo.toCpu.portNum      = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->.dsaInfo.toCpu.portNum = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.toCpu.portNum);

        pcktParams.dsaParam.dsaInfo.toCpu.portNum = NETIF_VALID_PHY_PORT_CNS;

        /*
            1.10. Call with pcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode[0x5AAAAAA5] and other params from 1.1.
            Expected: NOT GT_OK.
        */

        pcktParams.dsaParam.dsaInfo.toCpu.cpuCode = NETIF_INVALID_ENUM_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->.dsaInfo.toCpu.cpuCode = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.toCpu.cpuCode);

        pcktParams.dsaParam.dsaInfo.toCpu.cpuCode = CPSS_NET_ETH_BRIDGED_LLT_E;

        /*
            {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E}
            1.11. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                        cookie [not NULL],
                                        sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                                   txQueue [0 / 7],
                                                   evReqHndl [0],
                                                   invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                        dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                        dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E,
                                        dsaInfo{fromCpu {set from cpu}}
                            buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                            buffLenList [5, 5, 5, 5, 5],
                            numOfBufs [5]
            Expected: GT_OK.
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_TRUE;
        pcktParams.cookie = &Temp;

        pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
        pcktParams.sdmaInfo.txQueue = 0;
        pcktParams.sdmaInfo.evReqHndl = 0;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 0;
        pcktParams.dsaParam.commonParams.cfiBit = 0;
        pcktParams.dsaParam.commonParams.vid    = 0;

        pcktParams.dsaParam.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E;

        /* set all buffers to all 0 */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0x0, sizeof(GT_U8) * bufferSize);
        }

        buffLenList[0] = bufferSize;
        buffLenList[1] = bufferSize;
        buffLenList[2] = bufferSize;
        buffLenList[3] = bufferSize;
        buffLenList[4] = bufferSize;

        numOfBufs = 5;

        prvSetDsaInfoFromCpu(&pcktParams.dsaParam);

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* iterate with pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_FALSE;
        pcktParams.sdmaInfo.recalcCrc = GT_FALSE;
        pcktParams.sdmaInfo.txQueue = 7;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_FALSE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 7;
        pcktParams.dsaParam.commonParams.cfiBit = 1;
        pcktParams.dsaParam.commonParams.vid    = 100;

        /* set all buffers to all 0xff */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0xff, sizeof(GT_U8) * 5);
        }

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.12. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_TRUNK_E] (not supported)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d",
                                    dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        /*
            1.13. Call with out of range pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[0x5AAAAAA5]
                            and other params from 1.10.
            Extended: GT_BAD_PARAM.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = NETIF_INVALID_ENUM_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d",
                                    dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        /*
            1.14. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type            = CPSS_INTERFACE_PORT_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.fromCpu.dstInterface.type = %d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = 0;

        /*
            1.15. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type            = CPSS_INTERFACE_PORT_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.fromCpu.dstInterface.type = %d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = NETIF_VALID_PHY_PORT_CNS;

        /*
            1.16. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type           = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = %d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = 0;

        /*
            1.17. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type            = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams,buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = %d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = NETIF_VALID_PHY_PORT_CNS;

        /*
            1.18. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.vidx [4096] (out of range)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx = 4096;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = %d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx = 100;

        /*
            1.19. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VID_E]
                            pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.vidx [4096] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VID_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx = 4096;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams,buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = %d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vidx = 100;

        /*
            1.21. Call with pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vlanId = 4096;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = %d, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vlanId = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type, pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vlanId);

        pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vlanId = 100;

        /*
            {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_ANALYZER_E}
            1.24. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                        cookie [not NULL],
                                        sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                                   txQueue [0 / 7],
                                                   evReqHndl [0],
                                                   invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                        dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                      dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_ANALYZER_E,
                                      dsaInfo{toAnalyzer {set to analyzer}}
                            buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                            buffLenList [5, 5, 5, 5, 5],
                            numOfBufs [5]
            Expected: GT_OK.
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_TRUE;
        pcktParams.cookie = &Temp;

        pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
        pcktParams.sdmaInfo.txQueue = 0;
        pcktParams.sdmaInfo.evReqHndl = 0;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 0;
        pcktParams.dsaParam.commonParams.cfiBit = 0;
        pcktParams.dsaParam.commonParams.vid    = 0;

        /* set all buffers to all 0 */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0x0, sizeof(GT_U8) * bufferSize);
        }

        buffLenList[0] = bufferSize;
        buffLenList[1] = bufferSize;
        buffLenList[2] = bufferSize;
        buffLenList[3] = bufferSize;
        buffLenList[4] = bufferSize;

        numOfBufs = 5;

        prvSetDsaInfoToAnalizer(dev,&pcktParams.dsaParam);

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* iterate with pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_FALSE;
        pcktParams.sdmaInfo.recalcCrc = GT_FALSE;
        pcktParams.sdmaInfo.txQueue = 7;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_FALSE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 7;
        pcktParams.dsaParam.commonParams.cfiBit = 1;
        pcktParams.dsaParam.commonParams.vid    = 100;
        /* set all buffers to all 0xff */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0xff, sizeof(GT_U8) * 5);
        }

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.26 Call with pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.rx_sniff [GT_TRUE] (not extended rule)
                           pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                           and other params from 1.24
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.toAnalyzer.rxSniffer       = GT_TRUE;
        pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.toAnalyzer.rxSniffer = %d, pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = %d",
                                    dev, pcktParams.dsaParam.dsaInfo.toAnalyzer.rxSniffer, pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum);

        pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = 0;
        pcktParams.dsaParam.dsaInfo.toAnalyzer.rxSniffer       = GT_FALSE;

        /*
            1.27 Call with pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.rx_sniff [GT_FALSE] (rxtended rule)
                           pcktParamsPtr->dsaParam.dsaInfo.toAnalyzer.devPort.portNum [31] (out of range)
                           and other params from 1.24
            Expected: GT_OK.
        */
        pcktParams.dsaParam.commonParams.dsaTagType            = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;
        /* portNum can be up to is 6 bits in extended tag and up to 5 bits in reglar tag .
           32 is 6 bits value --> legal value*/
        pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = 32;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.commonParams.dsaTagType = %d, pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = %d",
                                    dev, pcktParams.dsaParam.commonParams.dsaTagType, pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum);

        pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = 0;

        /*
            {pcktParamsPtr->dsaParam.dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E}
            1.28. Call with pcktParams { packetIsTagged [GT_TRUE / GT_FALSE],
                                        cookie [not NULL],
                                        sdmaInfo { recalcCrc [GT_TRUE / GT_FALSE],
                                                   txQueue [0 / 7],
                                                   evReqHndl [0],
                                                   invokeTxBufferQueueEvent [GT_TRUE / GT_FALSE] },
                                        dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                      dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E,
                                      dsaInfo{forward {set forward}}
                            buffList [0x00, ..., 0x00 / 0xff, ..., 0xff],
                            buffLenList [5, 5, 5, 5, 5],
                            numOfBufs [5]
            Expected: GT_OK.
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_TRUE;
        pcktParams.cookie = &Temp;

        pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
        pcktParams.sdmaInfo.txQueue = 0;
        pcktParams.sdmaInfo.evReqHndl = 0;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 0;
        pcktParams.dsaParam.commonParams.cfiBit = 0;
        pcktParams.dsaParam.commonParams.vid    = 0;

        pcktParams.dsaParam.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E;

        /* set all buffers to all 0 */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0x0, sizeof(GT_U8) * bufferSize);
        }

        buffLenList[0] = bufferSize;
        buffLenList[1] = bufferSize;
        buffLenList[2] = bufferSize;
        buffLenList[3] = bufferSize;
        buffLenList[4] = bufferSize;

        numOfBufs = 5;

        prvSetDsaInfoForward(&pcktParams.dsaParam);

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* iterate with pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        pcktParams.packetIsTagged = GT_FALSE;
        pcktParams.sdmaInfo.recalcCrc = GT_FALSE;
        pcktParams.sdmaInfo.txQueue = 7;
        pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_FALSE;

        pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        pcktParams.dsaParam.commonParams.vpt    = 7;
        pcktParams.dsaParam.commonParams.cfiBit = 1;
        pcktParams.dsaParam.commonParams.vid    = 100;

        /* set all buffers to all 0xff */
        for (i = 0 ; i < 5 ; i++)
        {
            cpssOsMemSet(buffList[i], 0xff, sizeof(GT_U8) * 5);
        }

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.29 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.srcDev [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                           and other params from 1.28.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.srcDev = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.forward.srcDev = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.forward.srcDev);

        pcktParams.dsaParam.dsaInfo.forward.srcDev = 0;

        /*
            1.30 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.srcIsTrunk [GT_FALSE]
                           pcktParamsPtr->dsaParam.dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                           and other params from 1.28.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.srcIsTrunk     = GT_FALSE;
        pcktParams.dsaParam.dsaInfo.forward.source.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, pcktParams.dsaParam.dsaInfo.forward.srcIsTrunk = %d, pcktParams.dsaParam.dsaInfo.forward.source.portNum = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.forward.srcIsTrunk, pcktParams.dsaParam.dsaInfo.forward.source.portNum);

        pcktParams.dsaParam.dsaInfo.forward.source.portNum = 0;

        /*
            1.31 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.srcIsTrunk [GT_TRUE]
                           pcktParamsPtr->dsaParam.dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                           and other params from 1.28.
            Expected: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.srcIsTrunk     = GT_TRUE;
        pcktParams.dsaParam.dsaInfo.forward.source.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.srcIsTrunk = %d, dsaInfoPtr->dsaInfo.forward.source.portNum = %d",
                                    dev, pcktParams.dsaParam.dsaInfo.forward.srcIsTrunk, pcktParams.dsaParam.dsaInfo.forward.source.portNum);

        pcktParams.dsaParam.dsaInfo.forward.source.portNum = 0;

        /*
            1.32 Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type [CPSS_INTERFACE_TRUNK_E] (not supported)
                           and other params from 1.28.
            Expected: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.srcIsTrunk  = GT_TRUE;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type       = CPSS_INTERFACE_TRUNK_E;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        /*
            1.34. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                            and other params from 1.28.
            Extended: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type           = CPSS_INTERFACE_PORT_E;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type, pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum = 0;

        /*
            1.35. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                            and other params from 1.28.
            Extended: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type            = CPSS_INTERFACE_PORT_E;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum = %d",
                                        dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type, pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum = 0;

        /*
            1.36. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type           = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum = %d",
                                     dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type, pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum = 0;

        /*
            1.37. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type            = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum = %d",
                                     dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type, pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum = 0;

        /*
            1.38. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.vidx [4096] (out of range)
                            and other params from 1.28.
            Extended: NOT GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.vidx = 4096;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.vidx = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type, pcktParams.dsaParam.dsaInfo.forward.dstInterface.vidx);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.vidx = 0;

        /*
            1.39. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VID_E]
                            pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.vidx [4096] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type = CPSS_INTERFACE_VID_E;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.vidx = 4096;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.vidx = %d",
                                     dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type, pcktParams.dsaParam.dsaInfo.forward.dstInterface.vidx);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.vidx = 0;

        /*
            1.41. Call with pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            pcktParamsPtr->dsaParam.dsaInfo.forward.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.type   = CPSS_INTERFACE_VIDX_E;
        pcktParams.dsaParam.dsaInfo.forward.dstInterface.vlanId = 4096;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.vlanId = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.forward.dstInterface.type, pcktParams.dsaParam.dsaInfo.forward.dstInterface.vlanId);

        pcktParams.dsaParam.dsaInfo.forward.dstInterface.vlanId = 100;

        /*
            1.42. Call with pcktParamsPtr->sdmaInfo.cookie [NULL]
                           and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        pcktParams.cookie = NULL;

        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pcktParams.cookie = NULL", dev);

        pcktParams.cookie = &Temp;
        /*
            1.43 Call with pcktParamsPtr [NULL]
                           and other params from 1.28.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, NULL, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pcktParamsPtr = NULL", dev);

        /*
            1.44 Call with buffList [NULL]
                           and other params from 1.28.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, NULL, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, buffListPtr = NULL", dev);

        /*
            1.45 Call with buffLenList [NULL]
                           and other params from 1.28.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, NULL, numOfBufs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, buffLenListPtr = NULL", dev);
    }

    pcktParams.packetIsTagged = GT_TRUE;
    pcktParams.cookie = &Temp;

    pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
    pcktParams.sdmaInfo.txQueue = 0;
    pcktParams.sdmaInfo.evReqHndl = 0;
    pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

    pcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

    pcktParams.dsaParam.commonParams.vpt    = 0;
    pcktParams.dsaParam.commonParams.cfiBit = 0;
    pcktParams.dsaParam.commonParams.vid    = 0;

    pcktParams.dsaParam.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E;

    prvSetDsaInfoToCpu(&pcktParams.dsaParam);

    /* set all buffers to all 0 */
    for (i = 0 ; i < 5 ; i++)
    {
        cpssOsMemSet(buffList[i], 0x00, sizeof(GT_U8) * 5);
    }

    cpssOsMemSet((GT_VOID*) &(buffLenList), 0x05, sizeof(buffLenList));

    numOfBufs = 5;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNetIfSdmaTxPacketSend(dev, &pcktParams, buffList, buffLenList, numOfBufs);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

    cpssOsCacheDmaFree(dmaBuffer);
    return;
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNetIfSdmaRxQueueEnableSet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           queue,
    IN  GT_BOOL                         enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNetIfSdmaRxQueueEnableSet)
{
/*
    ITERATE_DEVICE (EXMXPM)
    1.1. Call with queue [0 / 7],
                   enable [GT_FALSE / GT_TRUE];
    Expected: GT_OK.
    1.2. Call cpssExMxPmNetIfSdmaRxQueueEnableGet with not NULL enable.
    Expected: GT_OK and the same enable as was set.
    1.3. Call with out of range queue [8],
                   and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       queue  = 0;
    GT_BOOL     enable = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with queue [0 / 7],
                           enable [GT_FALSE / GT_TRUE];
            Expected: GT_OK.
        */
        /* iterate with queue = 0 */
        queue  = 0;
        enable = GT_FALSE;

        st = cpssExMxPmNetIfSdmaRxQueueEnableSet(dev, queue, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, enable);

        /*
            1.2. Call cpssExMxPmNetIfSdmaRxQueueEnableGet with not NULL enable.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssExMxPmNetIfSdmaRxQueueEnableGet(dev, queue, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmNetIfSdmaRxQueueEnableGet: %d, %d", dev, queue);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);

        /* iterate with queue = 7 */
        queue  = 7;
        enable = GT_TRUE;

        st = cpssExMxPmNetIfSdmaRxQueueEnableSet(dev, queue, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, enable);

        /*
            1.2. Call cpssExMxPmNetIfSdmaRxQueueEnableGet with not NULL enable.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssExMxPmNetIfSdmaRxQueueEnableGet(dev, queue, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmNetIfSdmaRxQueueEnableGet: %d, %d", dev, queue);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);

        /*
            1.3. Call with out of range queue [8],
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        queue = 8;

        st = cpssExMxPmNetIfSdmaRxQueueEnableSet(dev, queue, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, queue);
    }

    queue  = 0;
    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNetIfSdmaRxQueueEnableSet(dev, queue, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNetIfSdmaRxQueueEnableSet(dev, queue, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNetIfSdmaRxQueueEnableGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           queue,
    OUT GT_BOOL                         enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNetIfSdmaRxQueueEnableGet)
{
/*
    ITERATE_DEVICE (EXMXPM)
    1.1. Call with queue [0 / 7], not NULL enable.
    Expected: GT_OK.
    1.2. Call with out of range queue [8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range enable [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       queue  = 0;
    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with queue [0 / 7], not NULL enable.
            Expected: GT_OK.
        */
        /* iterate with queue = 0 */
        queue  = 0;

        st = cpssExMxPmNetIfSdmaRxQueueEnableGet(dev, queue, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, queue);

        /* iterate with queue = 7 */
        queue = 7;

        st = cpssExMxPmNetIfSdmaRxQueueEnableGet(dev, queue, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, queue);

        /*
            1.2. Call with out of range queue [8],
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        queue = 8;

        st = cpssExMxPmNetIfSdmaRxQueueEnableGet(dev, queue, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, queue);

        queue = 0;

        /*
            1.3. Call with out of range enable [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaRxQueueEnableGet(dev, queue, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enable = %d", dev);
    }

    queue  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNetIfSdmaRxQueueEnableGet(dev, queue, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNetIfSdmaRxQueueEnableGet(dev, queue, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNetIfSdmaTxQueueEnableSet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           queue,
    IN  GT_BOOL                         enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNetIfSdmaTxQueueEnableSet)
{
/*
    ITERATE_DEVICE (EXMXPM)
    1.1. Call with queue [0 / 7],
                   enable [GT_FALSE / GT_TRUE];
    Expected: GT_OK.
    1.2. Call cpssExMxPmNetIfSdmaTxQueueEnableGet with not NULL enable.
    Expected: GT_OK and the same enable as was set.
    1.3. Call with out of range queue [8],
                   and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       queue  = 0;
    GT_BOOL     enable = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with queue [0 / 7],
                           enable [GT_FALSE / GT_TRUE];
            Expected: GT_OK.
        */
        /* iterate with queue = 0 */
        queue  = 0;
        enable = GT_FALSE;

        st = cpssExMxPmNetIfSdmaTxQueueEnableSet(dev, queue, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, enable);

        /*
            1.2. Call cpssExMxPmNetIfSdmaTxQueueEnableGet with not NULL enable.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssExMxPmNetIfSdmaTxQueueEnableGet(dev, queue, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmNetIfSdmaTxQueueEnableGet: %d, %d", dev, queue);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);

        /* iterate with queue = 7 */
        queue  = 7;
        enable = GT_TRUE;

        st = cpssExMxPmNetIfSdmaTxQueueEnableSet(dev, queue, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, enable);

        /*
            1.2. Call cpssExMxPmNetIfSdmaTxQueueEnableGet with not NULL enable.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssExMxPmNetIfSdmaTxQueueEnableGet(dev, queue, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmNetIfSdmaTxQueueEnableGet: %d, %d", dev, queue);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);

        /*
            1.3. Call with out of range queue [8],
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        queue = 8;

        st = cpssExMxPmNetIfSdmaTxQueueEnableSet(dev, queue, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, queue);
    }

    queue  = 0;
    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNetIfSdmaTxQueueEnableSet(dev, queue, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNetIfSdmaTxQueueEnableSet(dev, queue, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNetIfSdmaTxQueueEnableGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           queue,
    OUT GT_BOOL                         enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNetIfSdmaTxQueueEnableGet)
{
/*
    ITERATE_DEVICE (EXMXPM)
    1.1. Call with queue [0 / 7], not NULL enable.
    Expected: GT_OK.
    1.2. Call with out of range queue [8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range enable [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       queue  = 0;
    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with queue [0 / 7], not NULL enable.
            Expected: GT_OK.
        */
        /* iterate with queue = 0 */
        queue  = 0;

        st = cpssExMxPmNetIfSdmaTxQueueEnableGet(dev, queue, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, queue);

        /* iterate with queue = 7 */
        queue = 7;

        st = cpssExMxPmNetIfSdmaTxQueueEnableGet(dev, queue, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, queue);

        /*
            1.2. Call with out of range queue [8],
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        queue = 8;

        st = cpssExMxPmNetIfSdmaTxQueueEnableGet(dev, queue, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, queue);

        queue = 0;

        /*
            1.3. Call with out of range enable [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaTxQueueEnableGet(dev, queue, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enable = NULL", dev);
    }

    queue  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNetIfSdmaTxQueueEnableGet(dev, queue, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNetIfSdmaTxQueueEnableGet(dev, queue, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNetIfDsaTagBuild
(
    IN  GT_U8                                 devNum,
    IN  CPSS_EXMXPM_NET_DSA_PARAMS_STC   *dsaInfoPtr,
    OUT GT_U8                                 *dsaBytesPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNetIfDsaTagBuild)
{
/*
    ITERATE_DEVICE (EXMXPM)
    {dsaInfoPtr->dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E}
    1.1. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                              dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E,
                              dsaInfo{toCpu {set to cpu}}
    Expected: GT_OK.
    1.2. Call cpssExMxPmNetIfDsaTagParse with the same dsaBytesPtr and not NULL dsaInfoPtr
    Expected: GT_OK and the same dsaInfoPtr as was set (for 1.1,  1.11, 1.25)
    1.3. Call with dsaInfoPtr->commonParams->dsaCmd[0x5AAAAAA5]and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range dsaInfoPtr->commonParams.vpt [8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range dsaInfoPtr->commonParams.cfiBit [2] and other params from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range dsaInfoPtr->commonParams.vid [4096] and other params from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with dsaInfoPtr->dsaInfo.toCpu.devNum[PRV_CPSS_MAX_PP_DEVICES_CNS] and other params from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with isEgressPipe [GT_FALSE],
                   srcIsTrunk[GT_FALSE],
                   dsaInfoPtr->dsaInfo.toCpu.interface.port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                   and other params from 1.1.
    Expected: NOT GT_OK.

    1.10. Call with dsaInfoPtr->dsaInfo.toCpu.cpuCode[0x5AAAAAA5] and other params from 1.1.
    Expected: NOT GT_OK.
    {dsaInfoPtr->dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E}
    1.11. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                              dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E,
                              dsaInfo{fromCpu {set from cpu}}
    Expected: GT_OK.
    1.12. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_TRUNK_E] (not supported)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.13. Call with out of range dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[0x5AAAAAA5]
                    and other params from 1.10.
    Extended: GT_BAD_PARAM.
    1.14. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.15. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.16. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.
    1.17. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.
    1.18. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vidx [4096] (out of range)
                    and other params from 1.10.
    Extended: NOT GT_OK.
    1.19. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VID_E]
                    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vidx [4096] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.
    1.22. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                    and other params from 1.10.
    Extended: GT_OK.

    {dsaInfoPtr->dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_ANALYZER_E}
    1.25. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                              dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_ANALYZER_E,
                              dsaInfo{toAnalyzer {set to analyzer}}
    Expected: GT_OK.
    1.27 Call with dsaInfoPtr->dsaInfo.toAnalyzer.rx_sniff [GT_TRUE] (not extended rule)
                   dsaInfoPtr->dsaInfo.toAnalyzer.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                   and other params from 1.24
    Expected: NOT GT_OK.
    1.28 Call with dsaInfoPtr->dsaInfo.toAnalyzer.rx_sniff [GT_FALSE] (rxtended rule)
                   dsaInfoPtr->dsaInfo.toAnalyzer.devPort.portNum [31] (out of range)
                   and other params from 1.24
    Expected: NOT GT_OK.
    {dsaInfoPtr->dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E}
    1.29. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                           vpt [0 / 7],
                                           cfiBit [0 / 1],
                                           vid [0 / 100],
                              dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E,
                              dsaInfo{forward {set forward}}
    Expected: GT_OK.
    1.30 Call with dsaInfoPtr->dsaInfo.forward.srcDev [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                   and other params from 1.28.
    Expected: NOT GT_OK.
    1.31 Call with dsaInfoPtr->dsaInfo.forward.srcIsTrunk [GT_FALSE]
                   dsaInfoPtr->dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                   and other params from 1.28.
    Expected: NOT GT_OK.
    1.32 Call with dsaInfoPtr->dsaInfo.forward.srcIsTrunk [GT_TRUE]
                   dsaInfoPtr->dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                   and other params from 1.28.
    Expected: GT_OK.
    1.35. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                    and other params from 1.28.
    Extended: NOT GT_OK.
    1.36. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                    dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                    and other params from 1.28.
    Extended: NOT GT_OK.
    1.37. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.38. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.39. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.forward.dstInterface.vidx [4096] (out of range)
                    and other params from 1.28.
    Extended: NOT GT_OK.
    1.40. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VID_E]
                    dsaInfoPtr->dsaInfo.forward.dstInterface.vidx [4096] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.42. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                    dsaInfoPtr->dsaInfo.forward.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                    and other params from 1.28.
    Extended: GT_OK.
    1.43 Call with dsaInfoPtr[NULL]
                   and other params from 1.28.
    Expected: GT_BAD_PTR.
    1.44 Call with dsaBytesPtr[NULL]
                   and other params from 1.28.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_NET_DSA_PARAMS_STC    dsaInfo;
    CPSS_EXMXPM_NET_DSA_PARAMS_STC    dsaInfoGet;
    GT_U8                                  dsaBytes[8];


    cpssOsBzero((GT_VOID*) &dsaInfo, sizeof(dsaInfo));
    cpssOsBzero((GT_VOID*) &dsaInfoGet, sizeof(dsaInfoGet));
    cpssOsBzero((GT_VOID*) dsaBytes, sizeof(dsaBytes));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            {dsaInfoPtr->dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E}
            1.1. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                      dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E,
                                      dsaInfo{toCpu {set to cpu}}
            Expected: GT_OK.
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        dsaInfo.commonParams.vpt    = 0;
        dsaInfo.commonParams.cfiBit = 0;
        dsaInfo.commonParams.vid    = 0;

        dsaInfo.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E;

        prvSetDsaInfoToCpu(&dsaInfo);

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmNetIfDsaTagParse with the same dsaBytesPtr and not NULL dsaInfoPtr
            Expected: GT_OK and the same dsaInfoPtr as was set (for 1.1,  1.11, 1.25)
        */
        cpssOsBzero((GT_VOID*) &dsaInfoGet, sizeof(dsaInfoGet));

        st = cpssExMxPmNetIfDsaTagParse(dev, dsaBytes, &dsaInfoGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmNetIfDsaTagParse: %d", dev);

        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaCmd,
                                         dsaInfoGet.dsaCmd,
                                         "got another dsaInfo.dsaCmd than was set: %d", dev);
            /* check commonParams */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.dsaTagType,
                                         dsaInfoGet.commonParams.dsaTagType,
                                         "got another dsaInfo.commonParams.dsaTagType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vpt,
                                         dsaInfoGet.commonParams.vpt,
                                         "got another dsaInfo.commonParams.vpt than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.cfiBit,
                                         dsaInfoGet.commonParams.cfiBit,
                                         "got another dsaInfo.commonParams.cfiBit than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vid,
                                         dsaInfoGet.commonParams.vid,
                                         "got another dsaInfo.commonParams.vid than was set: %d", dev);
            /* check toCpu */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toCpu.isEgressPipe,
                                         dsaInfoGet.dsaInfo.toCpu.isEgressPipe,
                                         "got another dsaInfo.dsaInfo.toCpu.isEgressPipe than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toCpu.isTagged,
                                         dsaInfoGet.dsaInfo.toCpu.isTagged,
                                         "got another dsaInfo.dsaInfo.toCpu.isTagged than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toCpu.devNum,
                                         dsaInfoGet.dsaInfo.toCpu.devNum,
                                         "got another dsaInfo.dsaInfo.toCpu.devNum than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toCpu.portNum,
                                         dsaInfoGet.dsaInfo.toCpu.portNum,
                                         "got another dsaInfo.dsaInfo.toCpu.portNum than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toCpu.cpuCode,
                                         dsaInfoGet.dsaInfo.toCpu.cpuCode,
                                         "got another dsaInfo.dsaInfo.toCpu.cpuCode than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toCpu.wasTruncated,
                                         dsaInfoGet.dsaInfo.toCpu.wasTruncated,
                                         "got another dsaInfo.dsaInfo.toCpu.wasTruncated than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toCpu.flowId,
                                         dsaInfoGet.dsaInfo.toCpu.flowId,
                                         "got another dsaInfo.dsaInfo.toCpu.flowId than was set: %d", dev);
        }

        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        dsaInfo.commonParams.vpt    = 7;
        dsaInfo.commonParams.cfiBit = 1;
        dsaInfo.commonParams.vid    = 100;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmNetIfDsaTagParse with the same dsaBytesPtr and not NULL dsaInfoPtr
            Expected: GT_OK and the same dsaInfoPtr as was set (for 1.1,  1.11, 1.25)
        */
        cpssOsBzero((GT_VOID*) &dsaInfoGet, sizeof(dsaInfoGet));

        st = cpssExMxPmNetIfDsaTagParse(dev, dsaBytes, &dsaInfoGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmNetIfDsaTagParse: %d", dev);

        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaCmd,
                                         dsaInfoGet.dsaCmd,
                                         "got another dsaInfo.dsaCmd than was set: %d", dev);
            /* check commonParams */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.dsaTagType,
                                         dsaInfoGet.commonParams.dsaTagType,
                                         "got another dsaInfo.commonParams.dsaTagType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vpt,
                                         dsaInfoGet.commonParams.vpt,
                                         "got another dsaInfo.commonParams.vpt than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.cfiBit,
                                         dsaInfoGet.commonParams.cfiBit,
                                         "got another dsaInfo.commonParams.cfiBit than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vid,
                                         dsaInfoGet.commonParams.vid,
                                         "got another dsaInfo.commonParams.vid than was set: %d", dev);
            /* check toCpu */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toCpu.isEgressPipe,
                                         dsaInfoGet.dsaInfo.toCpu.isEgressPipe,
                                         "got another dsaInfo.dsaInfo.toCpu.isEgressPipe than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toCpu.isTagged,
                                         dsaInfoGet.dsaInfo.toCpu.isTagged,
                                         "got another dsaInfo.dsaInfo.toCpu.isTagged than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toCpu.devNum,
                                         dsaInfoGet.dsaInfo.toCpu.devNum,
                                         "got another dsaInfo.dsaInfo.toCpu.devNum than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toCpu.portNum,
                                         dsaInfoGet.dsaInfo.toCpu.portNum,
                                         "got another dsaInfo.dsaInfo.toCpu.portNum than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toCpu.cpuCode,
                                         dsaInfoGet.dsaInfo.toCpu.cpuCode,
                                         "got another dsaInfo.dsaInfo.toCpu.cpuCode than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toCpu.wasTruncated,
                                         dsaInfoGet.dsaInfo.toCpu.wasTruncated,
                                         "got another dsaInfo.dsaInfo.toCpu.wasTruncated than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toCpu.flowId,
                                         dsaInfoGet.dsaInfo.toCpu.flowId,
                                         "got another dsaInfo.dsaInfo.toCpu.flowId than was set: %d", dev);
        }

        /*
            1.3. Call with dsaInfoPtr->commonParams.dsaCmd[0x5AAAAAA5] and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        dsaInfo.dsaCmd = NETIF_INVALID_ENUM_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, dsaInfoPtr->dsaCmd = %d",
                                     dev, dsaInfo.dsaCmd);

        dsaInfo.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E;

        /*
            1.4. Call with out of range dsaInfoPtr->commonParams.vpt [8] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        dsaInfo.commonParams.vpt = 8;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->commonParams.vpt = %d",
                                         dev, dsaInfo.commonParams.vpt);

        dsaInfo.commonParams.vpt = 7;

        /*
            1.5. Call with out of range dsaInfoPtr->commonParams.cfiBit [2] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        dsaInfo.commonParams.cfiBit = 2;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->commonParams.cfiBit = %d",
                                         dev, dsaInfo.commonParams.cfiBit);

        dsaInfo.commonParams.cfiBit = 1;

        /*
            1.6. Call with out of range dsaInfoPtr->commonParams.vid [4096] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        dsaInfo.commonParams.vid = 4096;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->commonParams.vid = %d",
                                         dev, dsaInfo.commonParams.vid);

        dsaInfo.commonParams.vid = 1;

        /*
            1.7. Call with dsaInfoPtr->dsaInfo.toCpu.devNum[PRV_CPSS_MAX_PP_DEVICES_CNS]
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        dsaInfo.dsaInfo.toCpu.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
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
        dsaInfo.dsaInfo.toCpu.isEgressPipe = GT_FALSE;
        dsaInfo.dsaInfo.toCpu.isTagged     = GT_FALSE;
        dsaInfo.dsaInfo.toCpu.portNum      = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->.dsaInfo.toCpu.interface.portNum = %d",
                                         dev, dsaInfo.dsaInfo.toCpu.portNum);

        dsaInfo.dsaInfo.toCpu.portNum = NETIF_VALID_PHY_PORT_CNS;

        /*
            1.11. Call with dsaInfoPtr->dsaInfo.toCpu.cpuCode[0x5AAAAAA5] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        dsaInfo.dsaInfo.toCpu.cpuCode = NETIF_INVALID_ENUM_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.toCpu.cpuCode = %d",
                                     dev, dsaInfo.dsaInfo.toCpu.cpuCode);

        dsaInfo.dsaInfo.toCpu.cpuCode = CPSS_NET_ETH_BRIDGED_LLT_E;

        /*
            {dsaInfoPtr->dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E}
            1.12. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                      dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E,
                                      dsaInfo{fromCpu {set from cpu}}
            Expected: GT_OK.
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        dsaInfo.commonParams.vpt    = 0;
        dsaInfo.commonParams.cfiBit = 0;
        dsaInfo.commonParams.vid    = 0;

        dsaInfo.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E;

        prvSetDsaInfoFromCpu(&dsaInfo);

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmNetIfDsaTagParse with the same dsaBytesPtr and not NULL dsaInfoPtr
            Expected: GT_OK and the same dsaInfoPtr as was set (for 1.1,  1.11, 1.25)
        */
        cpssOsBzero((GT_VOID*) &dsaInfoGet, sizeof(dsaInfoGet));

        st = cpssExMxPmNetIfDsaTagParse(dev, dsaBytes, &dsaInfoGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmNetIfDsaTagParse: %d", dev);

        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaCmd,
                                         dsaInfoGet.dsaCmd,
                                         "got another dsaInfo.dsaCmd than was set: %d", dev);
            /* check commonParams */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.dsaTagType,
                                         dsaInfoGet.commonParams.dsaTagType,
                                         "got another dsaInfo.commonParams.dsaTagType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vpt,
                                         dsaInfoGet.commonParams.vpt,
                                         "got another dsaInfo.commonParams.vpt than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.cfiBit,
                                         dsaInfoGet.commonParams.cfiBit,
                                         "got another dsaInfo.commonParams.cfiBit than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vid,
                                         dsaInfoGet.commonParams.vid,
                                         "got another dsaInfo.commonParams.vid than was set: %d", dev);
            /* check fromCpu */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.dstInterface.type,
                                         dsaInfo.dsaInfo.fromCpu.dstInterface.type,
                                         "got another dsaInfo.dsaInfo.fromCpu.dstInterface.type than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.dstInterface.vidx,
                                         dsaInfo.dsaInfo.fromCpu.dstInterface.vidx,
                                         "got another dsaInfo.dsaInfo.fromCpu.dstInterface.vidx than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.tc,
                                         dsaInfo.dsaInfo.fromCpu.tc,
                                         "got another dsaInfo.dsaInfo.fromCpu.tc than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.dp,
                                         dsaInfo.dsaInfo.fromCpu.dp,
                                         "got another dsaInfo.dsaInfo.fromCpu.dp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.egrFilterEn,
                                         dsaInfo.dsaInfo.fromCpu.egrFilterEn,
                                         "got another dsaInfo.dsaInfo.fromCpu.egrFilterEn than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.packetType,
                                         dsaInfo.dsaInfo.fromCpu.packetType,
                                         "got another dsaInfo.dsaInfo.fromCpu.packetType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.egrFilterRegistered,
                                         dsaInfo.dsaInfo.fromCpu.egrFilterRegistered,
                                         "got another dsaInfo.dsaInfo.fromCpu.egrFilterRegistered than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.srcId,
                                         dsaInfo.dsaInfo.fromCpu.srcId,
                                         "got another dsaInfo.dsaInfo.fromCpu.srcId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface,
                                         dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface,
                                         "got another dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type,
                                         dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type,
                                         "got another dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.vidx,
                                         dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.vidx,
                                         "got another dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.vidx than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.mirrorToAllCPUs,
                                         dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.mirrorToAllCPUs,
                                         "got another dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.mirrorToAllCPUs than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.isDit,
                                         dsaInfo.dsaInfo.fromCpu.isDit,
                                         "got another dsaInfo.dsaInfo.fromCpu.isDit than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.ditPtr,
                                         dsaInfo.dsaInfo.fromCpu.ditPtr,
                                         "got another dsaInfo.dsaInfo.fromCpu.ditPtr than was set: %d", dev);
        }

        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        dsaInfo.commonParams.vpt    = 7;
        dsaInfo.commonParams.cfiBit = 1;
        dsaInfo.commonParams.vid    = 100;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmNetIfDsaTagParse with the same dsaBytesPtr and not NULL dsaInfoPtr
            Expected: GT_OK and the same dsaInfoPtr as was set (for 1.1,  1.11, 1.25)
        */
        cpssOsBzero((GT_VOID*) &dsaInfoGet, sizeof(dsaInfoGet));

        st = cpssExMxPmNetIfDsaTagParse(dev, dsaBytes, &dsaInfoGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmNetIfDsaTagParse: %d", dev);

        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaCmd,
                                         dsaInfoGet.dsaCmd,
                                         "got another dsaInfo.dsaCmd than was set: %d", dev);
            /* check commonParams */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.dsaTagType,
                                         dsaInfoGet.commonParams.dsaTagType,
                                         "got another dsaInfo.commonParams.dsaTagType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vpt,
                                         dsaInfoGet.commonParams.vpt,
                                         "got another dsaInfo.commonParams.vpt than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.cfiBit,
                                         dsaInfoGet.commonParams.cfiBit,
                                         "got another dsaInfo.commonParams.cfiBit than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vid,
                                         dsaInfoGet.commonParams.vid,
                                         "got another dsaInfo.commonParams.vid than was set: %d", dev);
            /* check fromCpu */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.dstInterface.type,
                                         dsaInfo.dsaInfo.fromCpu.dstInterface.type,
                                         "got another dsaInfo.dsaInfo.fromCpu.dstInterface.type than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.dstInterface.vidx,
                                         dsaInfo.dsaInfo.fromCpu.dstInterface.vidx,
                                         "got another dsaInfo.dsaInfo.fromCpu.dstInterface.vidx than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.tc,
                                         dsaInfo.dsaInfo.fromCpu.tc,
                                         "got another dsaInfo.dsaInfo.fromCpu.tc than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.dp,
                                         dsaInfo.dsaInfo.fromCpu.dp,
                                         "got another dsaInfo.dsaInfo.fromCpu.dp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.egrFilterEn,
                                         dsaInfo.dsaInfo.fromCpu.egrFilterEn,
                                         "got another dsaInfo.dsaInfo.fromCpu.egrFilterEn than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.packetType,
                                         dsaInfo.dsaInfo.fromCpu.packetType,
                                         "got another dsaInfo.dsaInfo.fromCpu.packetType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.egrFilterRegistered,
                                         dsaInfo.dsaInfo.fromCpu.egrFilterRegistered,
                                         "got another dsaInfo.dsaInfo.fromCpu.egrFilterRegistered than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.srcId,
                                         dsaInfo.dsaInfo.fromCpu.srcId,
                                         "got another dsaInfo.dsaInfo.fromCpu.srcId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface,
                                         dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface,
                                         "got another dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type,
                                         dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type,
                                         "got another dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.vidx,
                                         dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.vidx,
                                         "got another dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.vidx than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.mirrorToAllCPUs,
                                         dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.mirrorToAllCPUs,
                                         "got another dsaInfo.dsaInfo.fromCpu.extDestInfo.multiDest.mirrorToAllCPUs than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.isDit,
                                         dsaInfo.dsaInfo.fromCpu.isDit,
                                         "got another dsaInfo.dsaInfo.fromCpu.isDit than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.fromCpu.ditPtr,
                                         dsaInfo.dsaInfo.fromCpu.ditPtr,
                                         "got another dsaInfo.dsaInfo.fromCpu.ditPtr than was set: %d", dev);
        }

        /*
            1.13. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_TRUNK_E] (not supported)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d",
                                        dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type);

        dsaInfo.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        /*
            1.14. Call with out of range dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[0x5AAAAAA5]
                            and other params from 1.10.
            Extended: GT_BAD_PARAM.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type = NETIF_INVALID_ENUM_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d",
                                    dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type);

        dsaInfo.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        /*
            1.15. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type           = CPSS_INTERFACE_PORT_E;
        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.devNum = %d",
                                        dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type, dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.devNum);

        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.devNum = 0;

        /*
            1.16. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type            = CPSS_INTERFACE_PORT_E;
        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.portNum = %d",
                                        dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type, dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum);

        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum = NETIF_VALID_PHY_PORT_CNS;

        /*
            1.17. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type           = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.devNum = %d",
                                        dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type, dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.devNum);

        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.devNum = 0;

        /*
            1.18. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type            = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.portNum = %d",
                                        dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type, dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum);

        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum = NETIF_VALID_PHY_PORT_CNS;

        /*
            1.19. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vidx [4096] (out of range)
                            and other params from 1.10.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.fromCpu.dstInterface.vidx = 4096;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vidx = %d",
                                        dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type, dsaInfo.dsaInfo.fromCpu.dstInterface.vidx);

        dsaInfo.dsaInfo.fromCpu.dstInterface.vidx = 100;

        /*
            1.20. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VID_E]
                            dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vidx [4096] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VID_E;
        dsaInfo.dsaInfo.fromCpu.dstInterface.vidx = 4096;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vidx = %d",
                                    dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type, dsaInfo.dsaInfo.fromCpu.dstInterface.vidx);

        dsaInfo.dsaInfo.fromCpu.dstInterface.vidx = 100;

        /*
            1.22. Call with dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                            and other params from 1.10.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.fromCpu.dstInterface.type   = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.fromCpu.dstInterface.vlanId = 4096;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = %d, dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vlanId = %d",
                                     dev, dsaInfo.dsaInfo.fromCpu.dstInterface.type, dsaInfo.dsaInfo.fromCpu.dstInterface.vlanId);

        dsaInfo.dsaInfo.fromCpu.dstInterface.vlanId = 100;

        /*
            {dsaInfoPtr->dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_ANALYZER_E}
            1.25. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                      dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_TO_ANALYZER_E,
                                      dsaInfo{toAnalyzer {set to analyzer}}
            Expected: GT_OK.
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        dsaInfo.commonParams.vpt    = 0;
        dsaInfo.commonParams.cfiBit = 0;
        dsaInfo.commonParams.vid    = 0;

        prvSetDsaInfoToAnalizer(dev,&dsaInfo);

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmNetIfDsaTagParse with the same dsaBytesPtr and not NULL dsaInfoPtr
            Expected: GT_OK and the same dsaInfoPtr as was set (for 1.1,  1.11, 1.25)
        */
        cpssOsBzero((GT_VOID*) &dsaInfoGet, sizeof(dsaInfoGet));

        st = cpssExMxPmNetIfDsaTagParse(dev, dsaBytes, &dsaInfoGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmNetIfDsaTagParse: %d", dev);

        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaCmd,
                                         dsaInfoGet.dsaCmd,
                                         "got another dsaInfo.dsaCmd than was set: %d", dev);
            /* check commonParams */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.dsaTagType,
                                         dsaInfoGet.commonParams.dsaTagType,
                                         "got another dsaInfo.commonParams.dsaTagType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vpt,
                                         dsaInfoGet.commonParams.vpt,
                                         "got another dsaInfo.commonParams.vpt than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.cfiBit,
                                         dsaInfoGet.commonParams.cfiBit,
                                         "got another dsaInfo.commonParams.cfiBit than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vid,
                                         dsaInfoGet.commonParams.vid,
                                         "got another dsaInfo.commonParams.vid than was set: %d", dev);
            /* check toAnalizer */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toAnalyzer.rxSniffer,
                                         dsaInfo.dsaInfo.toAnalyzer.rxSniffer,
                                         "got another dsaInfo.dsaInfo.toAnalyzer.rxSniffer than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toAnalyzer.isTagged,
                                         dsaInfo.dsaInfo.toAnalyzer.isTagged,
                                         "got another dsaInfo.dsaInfo.toAnalyzer.isTagged than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toAnalyzer.devPort.devNum,
                                         dsaInfo.dsaInfo.toAnalyzer.devPort.devNum,
                                         "got another dsaInfo.dsaInfo.toAnalyzer.devPort.devNum than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toAnalyzer.devPort.portNum,
                                         dsaInfo.dsaInfo.toAnalyzer.devPort.portNum,
                                         "got another dsaInfo.dsaInfo.toAnalyzer.devPort.portNum than was set: %d", dev);
        }

        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        dsaInfo.commonParams.vpt    = 7;
        dsaInfo.commonParams.cfiBit = 1;
        dsaInfo.commonParams.vid    = 100;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmNetIfDsaTagParse with the same dsaBytesPtr and not NULL dsaInfoPtr
            Expected: GT_OK and the same dsaInfoPtr as was set (for 1.1,  1.11, 1.25)
        */
        cpssOsBzero((GT_VOID*) &dsaInfoGet, sizeof(dsaInfoGet));

        st = cpssExMxPmNetIfDsaTagParse(dev, dsaBytes, &dsaInfoGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmNetIfDsaTagParse: %d", dev);

        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaCmd,
                                         dsaInfoGet.dsaCmd,
                                         "got another dsaInfo.dsaCmd than was set: %d", dev);
            /* check commonParams */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.dsaTagType,
                                         dsaInfoGet.commonParams.dsaTagType,
                                         "got another dsaInfo.commonParams.dsaTagType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vpt,
                                         dsaInfoGet.commonParams.vpt,
                                         "got another dsaInfo.commonParams.vpt than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.cfiBit,
                                         dsaInfoGet.commonParams.cfiBit,
                                         "got another dsaInfo.commonParams.cfiBit than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vid,
                                         dsaInfoGet.commonParams.vid,
                                         "got another dsaInfo.commonParams.vid than was set: %d", dev);
            /* check toAnalizer */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toAnalyzer.rxSniffer,
                                         dsaInfo.dsaInfo.toAnalyzer.rxSniffer,
                                         "got another dsaInfo.dsaInfo.toAnalyzer.rxSniffer than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toAnalyzer.isTagged,
                                         dsaInfo.dsaInfo.toAnalyzer.isTagged,
                                         "got another dsaInfo.dsaInfo.toAnalyzer.isTagged than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toAnalyzer.devPort.devNum,
                                         dsaInfo.dsaInfo.toAnalyzer.devPort.devNum,
                                         "got another dsaInfo.dsaInfo.toAnalyzer.devPort.devNum than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.toAnalyzer.devPort.portNum,
                                         dsaInfo.dsaInfo.toAnalyzer.devPort.portNum,
                                         "got another dsaInfo.dsaInfo.toAnalyzer.devPort.portNum than was set: %d", dev);
        }

        /*
            1.27 Call with dsaInfoPtr->dsaInfo.toAnalyzer.rxSniffer [GT_TRUE] (not extended rule)
                           dsaInfoPtr->dsaInfo.toAnalyzer.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                           and other params from 1.24
            Expected: NOT GT_OK.
        */
        dsaInfo.dsaInfo.toAnalyzer.rxSniffer       = GT_TRUE;
        dsaInfo.dsaInfo.toAnalyzer.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.toAnalyzer.rxSniffer = %d, dsaInfo.dsaInfo.toAnalyzer.devPort.portNum = %d",
                                    dev, dsaInfo.dsaInfo.toAnalyzer.rxSniffer, dsaInfo.dsaInfo.toAnalyzer.devPort.portNum);

        dsaInfo.dsaInfo.toAnalyzer.devPort.portNum = 0;
        dsaInfo.dsaInfo.toAnalyzer.rxSniffer       = GT_FALSE;

        /*
            1.28 Call with dsaInfo.commonParams.dsaTagType [CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E] (rxtended rule)
                           dsaInfoPtr->dsaInfo.toAnalyzer.devPort.portNum [31] (out of range)
                           and other params from 1.24
            Expected: NOT GT_OK.
        */
/*        dsaInfo.commonParams.dsaTagType            = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;
        dsaInfo.dsaInfo.toAnalyzer.devPort.portNum = 32;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->commonParams.dsaTagType = %d, dsaInfo.dsaInfo.toAnalyzer.devPort.portNum = %d",
                                    dev, dsaInfo.commonParams.dsaTagType, dsaInfo.dsaInfo.toAnalyzer.devPort.portNum);

        dsaInfo.dsaInfo.toAnalyzer.devPort.portNum = 0;
*/
        /*
            {dsaInfoPtr->dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E}
            1.29. Call with dsaInfoPtr{commonParams(dsaTagType[CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E],
                                                   vpt [0 / 7],
                                                   cfiBit [0 / 1],
                                                   vid [0 / 100],
                                      dsaCmd [CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E,
                                      dsaInfo{forward {set forward}}
            Expected: GT_OK.
        */
        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        dsaInfo.commonParams.vpt    = 0;
        dsaInfo.commonParams.cfiBit = 0;
        dsaInfo.commonParams.vid    = 0;

        dsaInfo.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E;

        prvSetDsaInfoForward(&dsaInfo);

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmNetIfDsaTagParse with the same dsaBytesPtr and not NULL dsaInfoPtr
            Expected: GT_OK and the same dsaInfoPtr as was set (for 1.1,  1.11, 1.25)
        */
        cpssOsBzero((GT_VOID*) &dsaInfoGet, sizeof(dsaInfoGet));

        st = cpssExMxPmNetIfDsaTagParse(dev, dsaBytes, &dsaInfoGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmNetIfDsaTagParse: %d", dev);

        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaCmd,
                                         dsaInfoGet.dsaCmd,
                                         "got another dsaInfo.dsaCmd than was set: %d", dev);
            /* check commonParams */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.dsaTagType,
                                         dsaInfoGet.commonParams.dsaTagType,
                                         "got another dsaInfo.commonParams.dsaTagType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vpt,
                                         dsaInfoGet.commonParams.vpt,
                                         "got another dsaInfo.commonParams.vpt than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.cfiBit,
                                         dsaInfoGet.commonParams.cfiBit,
                                         "got another dsaInfo.commonParams.cfiBit than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vid,
                                         dsaInfoGet.commonParams.vid,
                                         "got another dsaInfo.commonParams.vid than was set: %d", dev);
            /* check Forward */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.srcIsTagged,
                                         dsaInfo.dsaInfo.forward.srcIsTagged,
                                         "got another dsaInfo.dsaInfo.forward.srcIsTagged than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.srcDev,
                                         dsaInfo.dsaInfo.forward.srcDev,
                                         "got another dsaInfo.dsaInfo.forward.srcDev than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.srcIsTrunk,
                                         dsaInfo.dsaInfo.forward.srcIsTrunk,
                                         "got another dsaInfo.dsaInfo.forward.srcIsTrunk than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.source.portNum,
                                         dsaInfo.dsaInfo.forward.source.portNum,
                                         "got another dsaInfo.dsaInfo.forward.source.portNum than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.srcId,
                                         dsaInfo.dsaInfo.forward.srcId,
                                         "got another dsaInfo.dsaInfo.forward.srcId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.egrFilterRegistered,
                                         dsaInfo.dsaInfo.forward.egrFilterRegistered,
                                         "got another dsaInfo.dsaInfo.forward.egrFilterRegistered than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.wasRouted,
                                         dsaInfo.dsaInfo.forward.wasRouted,
                                         "got another dsaInfo.dsaInfo.forward.wasRouted than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.dstInterface.type,
                                         dsaInfo.dsaInfo.forward.dstInterface.type,
                                         "got another dsaInfo.dsaInfo.forward.dstInterface.type than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.dstInterface.vidx,
                                         dsaInfo.dsaInfo.forward.dstInterface.vidx,
                                         "got another dsaInfo.dsaInfo.forward.dstInterface.vidx than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.tc,
                                         dsaInfo.dsaInfo.forward.tc,
                                         "got another dsaInfo.dsaInfo.forward.tc than was set: %d", dev);
        }

        /* iterate with dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E */
        dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

        dsaInfo.commonParams.vpt    = 7;
        dsaInfo.commonParams.cfiBit = 1;
        dsaInfo.commonParams.vid    = 100;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmNetIfDsaTagParse with the same dsaBytesPtr and not NULL dsaInfoPtr
            Expected: GT_OK and the same dsaInfoPtr as was set (for 1.1,  1.11, 1.25)
        */
        cpssOsBzero((GT_VOID*) &dsaInfoGet, sizeof(dsaInfoGet));

        st = cpssExMxPmNetIfDsaTagParse(dev, dsaBytes, &dsaInfoGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmNetIfDsaTagParse: %d", dev);

        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaCmd,
                                         dsaInfoGet.dsaCmd,
                                         "got another dsaInfo.dsaCmd than was set: %d", dev);
            /* check commonParams */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.dsaTagType,
                                         dsaInfoGet.commonParams.dsaTagType,
                                         "got another dsaInfo.commonParams.dsaTagType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vpt,
                                         dsaInfoGet.commonParams.vpt,
                                         "got another dsaInfo.commonParams.vpt than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.cfiBit,
                                         dsaInfoGet.commonParams.cfiBit,
                                         "got another dsaInfo.commonParams.cfiBit than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.commonParams.vid,
                                         dsaInfoGet.commonParams.vid,
                                         "got another dsaInfo.commonParams.vid than was set: %d", dev);
            /* check Forward */
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.srcIsTagged,
                                         dsaInfo.dsaInfo.forward.srcIsTagged,
                                         "got another dsaInfo.dsaInfo.forward.srcIsTagged than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.srcDev,
                                         dsaInfo.dsaInfo.forward.srcDev,
                                         "got another dsaInfo.dsaInfo.forward.srcDev than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.srcIsTrunk,
                                         dsaInfo.dsaInfo.forward.srcIsTrunk,
                                         "got another dsaInfo.dsaInfo.forward.srcIsTrunk than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.source.portNum,
                                         dsaInfo.dsaInfo.forward.source.portNum,
                                         "got another dsaInfo.dsaInfo.forward.source.portNum than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.srcId,
                                         dsaInfo.dsaInfo.forward.srcId,
                                         "got another dsaInfo.dsaInfo.forward.srcId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.egrFilterRegistered,
                                         dsaInfo.dsaInfo.forward.egrFilterRegistered,
                                         "got another dsaInfo.dsaInfo.forward.egrFilterRegistered than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.wasRouted,
                                         dsaInfo.dsaInfo.forward.wasRouted,
                                         "got another dsaInfo.dsaInfo.forward.wasRouted than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.dstInterface.type,
                                         dsaInfo.dsaInfo.forward.dstInterface.type,
                                         "got another dsaInfo.dsaInfo.forward.dstInterface.type than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.dstInterface.vidx,
                                         dsaInfo.dsaInfo.forward.dstInterface.vidx,
                                         "got another dsaInfo.dsaInfo.forward.dstInterface.vidx than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(dsaInfo.dsaInfo.forward.tc,
                                         dsaInfo.dsaInfo.forward.tc,
                                         "got another dsaInfo.dsaInfo.forward.tc than was set: %d", dev);
        }

        /*
            1.30 Call with dsaInfoPtr->dsaInfo.forward.srcDev [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                           and other params from 1.28.
            Expected: NOT GT_OK.
        */
        dsaInfo.dsaInfo.forward.srcDev = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.srcDev = %d",
                                         dev, dsaInfo.dsaInfo.forward.srcDev);

        dsaInfo.dsaInfo.forward.srcDev = 0;

        /*
            1.31 Call with dsaInfoPtr->dsaInfo.forward.srcIsTrunk [GT_FALSE]
                           dsaInfoPtr->dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                           and other params from 1.28.
            Expected: NOT GT_OK.
        */
        dsaInfo.dsaInfo.forward.srcIsTrunk     = GT_FALSE;
        dsaInfo.dsaInfo.forward.source.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.srcIsTrunk = %d, dsaInfoPtr->dsaInfo.forward.source.portNum = %d",
                                        dev, dsaInfo.dsaInfo.forward.srcIsTrunk, dsaInfo.dsaInfo.forward.source.portNum);

        dsaInfo.dsaInfo.forward.source.portNum = 0;

        /*
            1.32 Call with dsaInfoPtr->dsaInfo.forward.srcIsTrunk [GT_TRUE]
                           dsaInfoPtr->dsaInfo.forward.source.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                           and other params from 1.28.
            Expected: GT_OK.
        */
        dsaInfo.dsaInfo.forward.srcIsTrunk     = GT_TRUE;
        dsaInfo.dsaInfo.forward.source.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.srcIsTrunk = %d, dsaInfoPtr->dsaInfo.forward.source.portNum = %d",
                                    dev, dsaInfo.dsaInfo.forward.srcIsTrunk, dsaInfo.dsaInfo.forward.source.portNum);

        dsaInfo.dsaInfo.forward.source.portNum = 0;

        /*
            1.35. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of range)
                            and other params from 1.28.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type           = CPSS_INTERFACE_PORT_E;
        dsaInfo.dsaInfo.forward.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum = %d",
                                        dev, dsaInfo.dsaInfo.forward.dstInterface.type, dsaInfo.dsaInfo.forward.dstInterface.devPort.devNum);

        dsaInfo.dsaInfo.forward.dstInterface.devPort.devNum = 0;

        /*
            1.36. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_PORT_E]
                            dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                            and other params from 1.28.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type            = CPSS_INTERFACE_PORT_E;
        dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum = %d",
                                        dev, dsaInfo.dsaInfo.forward.dstInterface.type, dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum);

        dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum = 0;

        /*
            1.37. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type           = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.forward.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.devNum = %d",
                                     dev, dsaInfo.dsaInfo.forward.dstInterface.type, dsaInfo.dsaInfo.forward.dstInterface.devPort.devNum);

        dsaInfo.dsaInfo.forward.dstInterface.devPort.devNum = 0;

        /*
            1.38. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type            = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.portNum = %d",
                                     dev, dsaInfo.dsaInfo.forward.dstInterface.type, dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum);

        dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum = 0;

        /*
            1.39. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.forward.dstInterface.vidx [4096] (out of range)
                            and other params from 1.28.
            Extended: NOT GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.forward.dstInterface.vidx = 4096;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.vidx = %d",
                                         dev, dsaInfo.dsaInfo.forward.dstInterface.type, dsaInfo.dsaInfo.forward.dstInterface.vidx);

        dsaInfo.dsaInfo.forward.dstInterface.vidx = 0;

        /*
            1.40. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VID_E]
                            dsaInfoPtr->dsaInfo.forward.dstInterface.vidx [4096] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type = CPSS_INTERFACE_VID_E;
        dsaInfo.dsaInfo.forward.dstInterface.vidx = 4096;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.vidx = %d",
                                     dev, dsaInfo.dsaInfo.forward.dstInterface.type, dsaInfo.dsaInfo.forward.dstInterface.vidx);

        dsaInfo.dsaInfo.forward.dstInterface.vidx = 0;

        /*
            1.42. Call with dsaInfoPtr->dsaInfo.forward.dstInterface.type[CPSS_INTERFACE_VIDX_E]
                            dsaInfoPtr->dsaInfo.forward.dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                            and other params from 1.28.
            Extended: GT_OK.
        */
        dsaInfo.dsaInfo.forward.dstInterface.type   = CPSS_INTERFACE_VIDX_E;
        dsaInfo.dsaInfo.forward.dstInterface.vlanId = 4096;

        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, dsaInfoPtr->dsaInfo.forward.dstInterface.type = %d, dsaInfoPtr->dsaInfo.forward.dstInterface.vlanId = %d",
                                         dev, dsaInfo.dsaInfo.forward.dstInterface.type, dsaInfo.dsaInfo.forward.dstInterface.vlanId);

        dsaInfo.dsaInfo.forward.dstInterface.vlanId = 100;

        /*
            1.43 Call with dsaInfoPtr[NULL]
                           and other params from 1.28.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfDsaTagBuild(dev, NULL, dsaBytes);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dsaInfoPtr = NULL", dev);

        /*
            1.44 Call with dsaBytesPtr[NULL]
                           and other params from 1.28.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dsaBytesPtr = NULL", dev);
    }

    dsaInfo.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;

    dsaInfo.commonParams.vpt    = 0;
    dsaInfo.commonParams.cfiBit = 0;
    dsaInfo.commonParams.vid    = 0;

    dsaInfo.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E;

    prvSetDsaInfoToCpu(&dsaInfo);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNetIfDsaTagBuild(dev, &dsaInfo, dsaBytes);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNetIfDsaTagParse
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               *dsaBytesPtr,
    OUT CPSS_EXMXPM_NET_DSA_PARAMS_STC *dsaInfoPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNetIfDsaTagParse)
{
/*
    ITERATE_DEVICE (EXMXPM)
    1.1. Call with dsaBytesPtr {0x0, ..., 0x0} and not NULL dsaInfoPtr
    Expected: GT_OK.
    1.2. Call with dsaBytesPtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with dsaInfoPtr [NULL].
    Expected: GT_BAF_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_NET_DSA_PARAMS_STC    dsaInfo;
    GT_U8                                  dsaBytes[8];


    cpssOsBzero((GT_VOID*) &dsaInfo, sizeof(dsaInfo));
    cpssOsBzero((GT_VOID*) dsaBytes, sizeof(dsaBytes));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with dsaBytesPtr {0x0, ..., 0x0} and not NULL dsaInfoPtr
            Expected: GT_OK.
        */
        cpssOsBzero((GT_VOID*) dsaBytes, sizeof(dsaBytes));

        st = cpssExMxPmNetIfDsaTagParse(dev, dsaBytes, &dsaInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with dsaBytesPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfDsaTagParse(dev, NULL, &dsaInfo);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dsaBytesPtr = NULL", dev);

        /*
            1.3. Call with dsaInfoPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfDsaTagParse(dev, dsaBytes, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dsaInfoPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNetIfDsaTagParse(dev, dsaBytes, &dsaInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNetIfDsaTagParse(dev, dsaBytes, &dsaInfo);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNetIfSdmaTxBufferQueueDescFreeAndCookieGet
(
    IN GT_U8                devNum,
    IN GT_U8                txQueue,
    OUT GT_PTR             *cookiePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNetIfSdmaTxBufferQueueDescFreeAndCookieGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with txQueue [8] (out of range)
    Expected: NOT GT_OK.
    1.2. Call with cookiePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       txQueue = 0;
    GT_PTR      cookiePtr;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with txQueue [8] (out of range)
            Expected: NOT GT_OK.
        */
        txQueue = 8;

        st = cpssExMxPmNetIfSdmaTxBufferQueueDescFreeAndCookieGet(dev, txQueue, &cookiePtr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, txQueue);

        txQueue = 0;

        /*
            1.2. Call with cookiePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNetIfSdmaTxBufferQueueDescFreeAndCookieGet(dev, txQueue, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "cpssExMxPmNetIfSdmaTxBufferQueueDescFreeAndCookieGet: %d, cookiePtr = NULL", dev);
    }

    txQueue = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNetIfSdmaTxBufferQueueDescFreeAndCookieGet(dev, txQueue, &cookiePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNetIfSdmaTxBufferQueueDescFreeAndCookieGet(dev, txQueue, &cookiePtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/* Private functions */
/*----------------------------------------------------------------------------*/
/*
    Set dsaInfoPtr->dsaInfo.toCpu structure.
*/
static void prvSetDsaInfoToCpu
(
    OUT CPSS_EXMXPM_NET_DSA_PARAMS_STC   *dsaInfoPtr
)
{
    /*
    dsaInfoPtr{dsaInfo{toCpu{commonToCpuParams {
                            isEgressPipe[GT_FALSE],
                            isTagged [GT_FALSE],
                            devNum [0],
                            portNum [NETIF_VALID_PHY_PORT_CNS],
                            cpuCode [CPSS_NET_ETH_BRIDGED_LLT_E],
                            wasTruncated [GT_FALSE],
                            flowId [0],
                            }
                        inlifId [10] } }}
    */
    dsaInfoPtr->dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E;
    dsaInfoPtr->dsaInfo.toCpu.isEgressPipe = GT_FALSE;
    dsaInfoPtr->dsaInfo.toCpu.isTagged     = GT_FALSE;
    dsaInfoPtr->dsaInfo.toCpu.devNum       = 0;
    dsaInfoPtr->dsaInfo.toCpu.portNum      = NETIF_VALID_PHY_PORT_CNS;
    dsaInfoPtr->dsaInfo.toCpu.cpuCode      = CPSS_NET_ETH_BRIDGED_LLT_E;
    dsaInfoPtr->dsaInfo.toCpu.wasTruncated = GT_FALSE;
    dsaInfoPtr->dsaInfo.toCpu.flowId       = 0;
}

/*----------------------------------------------------------------------------*/
/*
    Set dsaInfoPtr->dsaInfo.fromCpu structure.
*/
static void prvSetDsaInfoFromCpu
(
    OUT CPSS_EXMXPM_NET_DSA_PARAMS_STC   *dsaInfoPtr
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
                                extDestInfo{
                                            excludeInterface [GT_FALSE],
                                            excludedInterface{
                                                                type[CPSS_INTERFACE_VIDX_E],
                                                                vidx[100]}
                                            mirrorToAllCPUs[GT_FALSE]}
                                isDit [GT_FALSE],
                                ditPtr [0]
                            }
    */
    dsaInfoPtr->dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E;
    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_VIDX_E;
    dsaInfoPtr->dsaInfo.fromCpu.dstInterface.vidx = 100;

    dsaInfoPtr->dsaInfo.fromCpu.tc = 7;
    dsaInfoPtr->dsaInfo.fromCpu.dp = CPSS_DP_GREEN_E;

    dsaInfoPtr->dsaInfo.fromCpu.egrFilterEn         = GT_FALSE;
    dsaInfoPtr->dsaInfo.fromCpu.packetType          = CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ETHERNET_E;
    dsaInfoPtr->dsaInfo.fromCpu.egrFilterRegistered = GT_FALSE;

    dsaInfoPtr->dsaInfo.fromCpu.srcId  = 0;

    dsaInfoPtr->dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface       = GT_FALSE;
    dsaInfoPtr->dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type = CPSS_INTERFACE_VIDX_E;
    dsaInfoPtr->dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.vidx = 100;
    dsaInfoPtr->dsaInfo.fromCpu.extDestInfo.multiDest.mirrorToAllCPUs        = GT_FALSE;

    dsaInfoPtr->dsaInfo.fromCpu.isDit  = GT_FALSE;
    dsaInfoPtr->dsaInfo.fromCpu.ditPtr = 0;
}

/*----------------------------------------------------------------------------*/
/*
    Set dsaInfoPtr->dsaInfo.toAnalizer structure.
*/
static void prvSetDsaInfoToAnalizer
(
    IN  GT_U8   devNum,
    OUT CPSS_EXMXPM_NET_DSA_PARAMS_STC   *dsaInfoPtr
)
{
    CPSS_EXMXPM_OUTLIF_INFO_STC   interfaceInfo;
    /*
    dsaInfoPtr{dsaInfo{toAnalizer{
                                    rxSniffer [GT_FALSE],
                                    isTagged [GT_FALSE],
                                    devPort{
                                            devNum [0],
                                            portNum [0]
                                }
    */
    dsaInfoPtr->dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_TO_ANALYZER_E;
    dsaInfoPtr->dsaInfo.toAnalyzer.rxSniffer = GT_FALSE;
    dsaInfoPtr->dsaInfo.toAnalyzer.isTagged  = GT_FALSE;

    dsaInfoPtr->dsaInfo.toAnalyzer.devPort.devNum  = devNum;
    dsaInfoPtr->dsaInfo.toAnalyzer.devPort.portNum = NETIF_VALID_PHY_PORT_CNS;

    interfaceInfo.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    interfaceInfo.interfaceInfo.type = CPSS_INTERFACE_PORT_E;
    interfaceInfo.interfaceInfo.devPort.portNum = CPSS_NULL_PORT_NUM_CNS;
    interfaceInfo.interfaceInfo.devPort.devNum = devNum;
    /* make sure that analyzer port is not NETIF_VALID_PHY_PORT_CNS !!! */
    cpssExMxPmMirrorDestInterfaceSet(devNum,
        CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E,
        0,
        &interfaceInfo);

    cpssExMxPmMirrorDestInterfaceSet(devNum,
        CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E,
        0,
        &interfaceInfo);

    cpssExMxPmMirrorDestInterfaceSet(devNum,
        CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E,
        0,
        &interfaceInfo);
}

/*----------------------------------------------------------------------------*/
/*
    Set dsaInfoPtr->dsaInfo.forward structure.
*/
static void prvSetDsaInfoForward
(
    OUT CPSS_EXMXPM_NET_DSA_PARAMS_STC   *dsaInfoPtr
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
                                dstInterface{
                                             type [CPSS_INTERFACE_VIDX_E],
                                             dstInterface.vidx [100]
                                             }
                                tc [7]
                                }
    */
    dsaInfoPtr->dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E;
    dsaInfoPtr->dsaInfo.forward.srcIsTagged = GT_FALSE;
    dsaInfoPtr->dsaInfo.forward.srcDev      = 0;
    dsaInfoPtr->dsaInfo.forward.srcIsTrunk  = GT_FALSE;

    dsaInfoPtr->dsaInfo.forward.source.portNum = NETIF_VALID_PHY_PORT_CNS;

    dsaInfoPtr->dsaInfo.forward.srcId = 0;

    dsaInfoPtr->dsaInfo.forward.egrFilterRegistered = GT_FALSE;
    dsaInfoPtr->dsaInfo.forward.wasRouted           = GT_FALSE;
    dsaInfoPtr->dsaInfo.forward.dstInterface.type   = CPSS_INTERFACE_VIDX_E;
    dsaInfoPtr->dsaInfo.forward.dstInterface.vidx   = 100;
    dsaInfoPtr->dsaInfo.forward.tc  = 7;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmNetIf suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmNetIf)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfPrePendTwoBytesHeaderSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfPrePendTwoBytesHeaderGet)
    /* UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfSdmaRxBufFree)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfSdmaRxBufFreeWithSize) */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfSdmaRxPacketGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfSdmaRxCountersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfSdmaRxErrorCountGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfSdmaTxSyncPacketSend)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfSdmaTxPacketSend)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfSdmaRxQueueEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfSdmaRxQueueEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfSdmaTxQueueEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfSdmaTxQueueEnableGet)
    /* UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfSdmaTxBufferQueueInfoGet)  */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfDsaTagBuild)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfDsaTagParse)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNetIfSdmaTxBufferQueueDescFreeAndCookieGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmNetIf)


