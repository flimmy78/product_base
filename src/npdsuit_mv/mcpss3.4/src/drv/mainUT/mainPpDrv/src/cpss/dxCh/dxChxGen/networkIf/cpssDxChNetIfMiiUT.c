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
*       Unit tests for cpssDxChNetIfMii, that provides
*       DxCh MII/CPU Ethernet port network interface management functions.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define NETIF_VALID_PHY_PORT_CNS    0

/* check is netif init done */
#define IS_NETIF_INIT_DONE(devNum) \
     (PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.initDone == GT_TRUE)

/* check is mii init done */
#define IS_NETIF_MII_INIT_DONE(devNum) \
    ((PRV_CPSS_PP_MAC(devNum)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_MII_E) && \
     (PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.initDone == GT_TRUE))

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfMiiInit
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_NETIF_MII_INIT_STC *miiInitPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfMiiInit)
{
/*
    ITERATE_DEVICE (dxChx)
    1.1. Call with wrong miiInit->txInternalBufBlockPtr[NULL]
    Expected: GT_BAD_PTR.
    1.2. Call with wrong miiInit->txInternalBufBlockSize[0]
    Expected: NOT GT_OK.
    1.3. Call with wrong miiInit->bufferPercentage[0] [101] (out of range)
    Expected: NOT GT_OK.
    1.4. Call with wrong miiInit->rxBufBlockPtr[NULL]
    Expected: GT_BAD_PTR.
    1.5. Call with wrong miiInit->rxBufBlockSize[0]
    Expected: NOT GT_OK.
    1.6. Call with 
                miiInit->numOfTxDesc[0],
                miiInit->txInternalBufBlockPtr[0],
                miiInit->txInternalBufBlockSize[1],
                miiInit->bufferPercentage[0] [1],
                miiInit->bufferPercentage[1] [2],
                miiInit->bufferPercentage[2] [3],
                miiInit->rxBufSize [3],
                miiInit->headerOffset[0],
                miiInit->rxBufBlockPtr[0],
                miiInit->rxBufBlockSize[1].
    Expected: GT_OK.
*/
    GT_STATUS  st = GT_OK;

    GT_U8                        dev;
    CPSS_DXCH_NETIF_MII_INIT_STC miiInit;

    GT_U8  txInternalBufBlock = 0;
    GT_U8  rxBufBlock         = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if(!IS_NETIF_INIT_DONE(dev))
        {
            /*
                1.1. Call with wrong miiInit->txInternalBufBlockPtr[NULL]
                Expected: GT_BAD_PTR.
            */
            miiInit.numOfTxDesc            = 0;
            miiInit.txInternalBufBlockSize = 1;
            miiInit.bufferPercentage[0]    = 1;
            miiInit.bufferPercentage[1]    = 2;
            miiInit.bufferPercentage[2]    = 3;
            miiInit.rxBufSize              = 3;
            miiInit.headerOffset           = 0;
            miiInit.rxBufBlockPtr          = &rxBufBlock;
            miiInit.rxBufBlockSize         = 1;

            miiInit.txInternalBufBlockPtr  = NULL;

            st = cpssDxChNetIfMiiInit(dev, &miiInit);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

            miiInit.txInternalBufBlockPtr  = &txInternalBufBlock;

            /*
                1.2. Call with wrong miiInit->txInternalBufBlockSize[0]
                Expected: NOT GT_OK.
            */
            miiInit.txInternalBufBlockSize  = 0;

            st = cpssDxChNetIfMiiInit(dev, &miiInit);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            miiInit.txInternalBufBlockSize  = 1;

            /*
                1.3. Call with wrong miiInit->bufferPercentage[0] [101] (out of range)
                Expected: NOT GT_OK.
            */
            miiInit.bufferPercentage[0] = 101;

            st = cpssDxChNetIfMiiInit(dev, &miiInit);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            miiInit.bufferPercentage[0] = 1;

            /*
                1.4. Call with wrong miiInit->rxBufBlockPtr[NULL]
                Expected: GT_BAD_PTR.
            */
            miiInit.rxBufBlockPtr = NULL;

            st = cpssDxChNetIfMiiInit(dev, &miiInit);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

            miiInit.rxBufBlockPtr = &rxBufBlock;

            /*
                1.5. Call with wrong miiInit->rxBufBlockSize[0]
                Expected: NOT GT_OK.
            */
            miiInit.rxBufBlockSize  = 0;

            st = cpssDxChNetIfMiiInit(dev, &miiInit);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            miiInit.rxBufBlockSize  = 1;

            /*
                1.6. Call with 
                            miiInit->numOfTxDesc[0],
                            miiInit->txInternalBufBlockPtr[0],
                            miiInit->txInternalBufBlockSize[1],
                            miiInit->bufferPercentage[0] [1],
                            miiInit->bufferPercentage[1] [2],
                            miiInit->bufferPercentage[2] [3],
                            miiInit->rxBufSize [3],
                            miiInit->headerOffset[0],
                            miiInit->rxBufBlockPtr[0],
                            miiInit->rxBufBlockSize[1].
                Expected: GT_OK.
            */
            miiInit.numOfTxDesc            = 0;
            miiInit.txInternalBufBlockPtr  = &txInternalBufBlock;
            miiInit.txInternalBufBlockSize = 1;
            miiInit.bufferPercentage[0]    = 1;
            miiInit.bufferPercentage[1]    = 2;
            miiInit.bufferPercentage[2]    = 3;
            miiInit.rxBufSize              = 3;
            miiInit.headerOffset           = 0;
            miiInit.rxBufBlockPtr          = &rxBufBlock;
            miiInit.rxBufBlockSize         = 1;

            st = cpssDxChNetIfMiiInit(dev, &miiInit);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfMiiInit(dev, &miiInit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfMiiInit(dev, &miiInit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfMiiRxBufFree
(
    IN GT_U8    devNum,
    IN GT_U8    rxQueue,
    IN GT_U8    *rxBuffList[],
    IN GT_U32   buffListLen
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfMiiRxBufFree)
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
        if(IS_NETIF_MII_INIT_DONE(dev))
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                            "Cannot initialize net if mii to dev %d", dev);

            /*
                1.1. Call with rxQueue [0], buffListLen [5]. 
                Expected: GT_OK.
            */
            st = cpssDxChNetIfMiiRxBufFree(dev, rxQueue, rxBuffList, buffListLen);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.2. Call with wrong rxBuffList pointer [NULL]. 
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfMiiRxBufFree(dev, rxQueue, NULL, buffListLen);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
        }
        else
        {
            /*
                1.1. Call with rxQueue [0], buffListLen [5]. 
                Expected: GT_BAD_STATE.
            */
            st = cpssDxChNetIfMiiRxBufFree(dev, rxQueue, rxBuffList, buffListLen);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_STATE, st, dev);
        }
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfMiiRxBufFree(dev, rxQueue, rxBuffList, buffListLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfMiiRxBufFree(dev, rxQueue, rxBuffList, buffListLen);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfMiiRxPacketGet
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    INOUT GT_U32                            *numOfBuffPtr,
    OUT GT_U8                               *packetBuffs[],
    OUT GT_U32                              buffLen[],
    OUT CPSS_DXCH_NET_RX_PARAMS_STC         *rxParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfMiiRxPacketGet)
{
/*
    ITERATE_DEVICE (dxChx)
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

    GT_U8                       queueIdx  = 0;
    GT_U32                      numOfBuff = 0;
    GT_U8                       packetBuffs[100];
    GT_U32                      buffLen[100];
    CPSS_DXCH_NET_RX_PARAMS_STC rxParams;

    cpssOsBzero((GT_VOID*) packetBuffs, sizeof(packetBuffs));
    cpssOsBzero((GT_VOID*) buffLen, sizeof(buffLen));
    cpssOsBzero((GT_VOID*) &rxParams, sizeof(rxParams));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if(IS_NETIF_MII_INIT_DONE(dev))
        {

            /*
                1.1. Call with queueIdx[7], numOfBuff[100], 
                     not NULL packetBuffs, buffLen, rxParamsPtr.
                Expected: GT_OK.
            */
            queueIdx  = 7;
            numOfBuff = 100;

            st = cpssDxChNetIfMiiRxPacketGet(dev, queueIdx, &numOfBuff, 
                                             (GT_U8 **)packetBuffs, buffLen, &rxParams);
            st = (GT_NO_MORE == st) ? GT_OK : st;
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, queueIdx, numOfBuff);

            /*
                1.2. Call with out of range queueIdx[8] and other params from 1.1.
                Expected: NOT GT_OK.
            */
            queueIdx = 8;

            st = cpssDxChNetIfMiiRxPacketGet(dev, queueIdx, &numOfBuff, 
                                             (GT_U8 **)packetBuffs, buffLen, &rxParams);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, queueIdx);

            queueIdx = 0;

            /*
                1.3. Call with numOfBuffPtr[NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfMiiRxPacketGet(dev, queueIdx, NULL, 
                                             (GT_U8 **)packetBuffs, buffLen, &rxParams);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfBuffPtr = NULL", dev);

            /*
                1.4. Call with packetBuffs[NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfMiiRxPacketGet(dev, queueIdx, &numOfBuff, 
                                             NULL, buffLen, &rxParams);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, packetBuffsPtr = NULL", dev);

            /*
                1.5. Call with buffLen[NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfMiiRxPacketGet(dev, queueIdx, &numOfBuff, 
                                             (GT_U8 **)packetBuffs, NULL, &rxParams);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, buffLenPtr = NULL", dev);

            /*
                1.6. Call with rxParamsPtr[NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfMiiRxPacketGet(dev, queueIdx, &numOfBuff, 
                                             (GT_U8 **)packetBuffs, buffLen, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rxParamsPtr = NULL", dev);
        }
        else
        {
            /*
                1.1. Call with queueIdx[7], numOfBuff[100], 
                     not NULL packetBuffs, buffLen, rxParamsPtr.
                Expected: GT_BAD_STATE.
            */
            queueIdx  = 7;
            numOfBuff = 100;

            st = cpssDxChNetIfMiiRxPacketGet(dev, queueIdx, &numOfBuff, 
                                             (GT_U8 **)packetBuffs, buffLen, &rxParams);
            st = (GT_NO_MORE == st) ? GT_OK : st;
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_STATE, st, dev, queueIdx, numOfBuff);
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
        st = cpssDxChNetIfMiiRxPacketGet(dev, queueIdx, &numOfBuff, 
                                   (GT_U8 **)packetBuffs, buffLen, &rxParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfMiiRxPacketGet(dev, queueIdx, &numOfBuff, 
                                    (GT_U8 **)packetBuffs, buffLen, &rxParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfMiiTxBufferQueueGet
(
    IN GT_U32               hndl,
    OUT GT_U8               *devPtr,
    OUT GT_PTR              *cookiePtr,
    OUT GT_U8               *queuePtr,
    OUT GT_STATUS           *statusPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChNetIfMiiTxBufferQueueGet)
{
/*
    ITERATE_DEVICES (DxChx)
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
        1.2. Call with wrong buffLenList pointer [NULL]. 
        Expected: GT_BAD_PTR.
    */
    st = cpssDxChNetIfMiiTxBufferQueueGet(hndl, NULL, &cookie, &queue, &status);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

    /*
        1.3. Call with wrong buffLenList pointer [NULL]. 
        Expected: GT_BAD_PTR.
    */
    st = cpssDxChNetIfMiiTxBufferQueueGet(hndl, &dev, NULL, &queue, &status);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

    /*
        1.4. Call with wrong buffLenList pointer [NULL]. 
        Expected: GT_BAD_PTR.
    */
    st = cpssDxChNetIfMiiTxBufferQueueGet(hndl, &dev, &cookie, NULL, &status);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

    /*
        1.5. Call with wrong buffLenList pointer [NULL]. 
        Expected: GT_BAD_PTR.
    */
    st = cpssDxChNetIfMiiTxBufferQueueGet(hndl, &dev, &cookie, &queue, NULL);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfMiiTxPacketSend
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC              *pcktParamsPtr,
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
)
*/
static GT_U8 prvUtSdmaBuffer[64];
UTF_TEST_CASE_MAC(cpssDxChNetIfMiiTxPacketSend)
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
    GT_U8                       *buffList[1] = {prvUtSdmaBuffer};
    GT_U32                       buffLenList[1] = {sizeof(prvUtSdmaBuffer)};
    GT_U32                       numOfBufs = 1;

    cpssOsBzero((GT_VOID*) &pcktParams, sizeof(pcktParams));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if(IS_NETIF_MII_INIT_DONE(dev))
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
            
            st = cpssDxChNetIfMiiTxPacketSend(dev, &pcktParams, buffList,
                                                   buffLenList, numOfBufs);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.2. Call with wrong pcktParams pointer [NULL]. 
                Expected: GT_BAD_PTR.
            */

            st = cpssDxChNetIfMiiTxPacketSend(dev, NULL, buffList,
                                                   buffLenList, numOfBufs);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

            /*
                1.3. Call with wrong buffList pointer [NULL]. 
                Expected: GT_BAD_PTR.
            */

            st = cpssDxChNetIfMiiTxPacketSend(dev, &pcktParams, NULL,
                                                   buffLenList, numOfBufs);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

            /*
                1.4. Call with wrong buffLenList pointer [NULL]. 
                Expected: GT_BAD_PTR.
            */

            st = cpssDxChNetIfMiiTxPacketSend(dev, &pcktParams, buffList,
                                                   NULL, numOfBufs);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
        }
        else
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
            
            st = cpssDxChNetIfMiiTxPacketSend(dev, &pcktParams, buffList,
                                                   buffLenList, numOfBufs);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_STATE, st, dev);
        }
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfMiiTxPacketSend(dev, &pcktParams, buffList,
                                               buffLenList, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfMiiTxPacketSend(dev, &pcktParams, buffList,
                                           buffLenList, numOfBufs);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChNetIfMiiSyncTxPacketSend
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC *pcktParamsPtr,
    IN GT_U8                       *packetBuffsArrPtr[],
    IN GT_U32                       buffLenArr[],
    IN GT_U32                       numOfBufs
)
*/

UTF_TEST_CASE_MAC(cpssDxChNetIfMiiSyncTxPacketSend)
{
/*
    ITERATE_DEVICES (DxChx)
    ======= DSA_CMD_TO_CPU =======
    1.1. Call with pcktParamsPtr {packetIsTagged [GT_TRUE],
                                  cookie [],
                                  sdmaInfo {recalcCrc [GT_TRUE],
                                            txQueue [0],
                                            invokeTxBufferQueueEvent [GT_FALSE] (not relevant)},
                                  dsaParam {commonParams {dsaTagType [CPSS_DXCH_NET_DSA_TYPE_REGULAR_E],
                                                          vpt[0],
                                                          cfiBit [0],
                                                          vid [0],
                                                          dropOnSource [GT_FALSE],
                                                          packetIsLooped [GT_FALSE]}
                                            dsaType [CPSS_DXCH_NET_DSA_CMD_TO_CPU_E],
                                            dsaInfo.toCpu {isEgressPipe [GT_TRUE],
                                                           isTagged [GT_TRUE],
                                                           devNum [dev],
                                                           srcIsTrunk [GT_TRUE],
                                                           interface [2],
                                                           cpuCode [CPSS_NET_CONTROL_E],
                                                           wasTruncated [GT_TRUE],
                                                           originByteCount [0],
                                                           timestamp [0]}}}.
                   non NULL packetBuffsArrPtr,
                   non NULL buffLenArr,
                   numOfBufs [1]
    Expected: GT_OK.
    1.2. Call with out of range pcktParamsPtr->cookie [NULL]
                   and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with out of range pcktParamsPtr->sdmaInfo.txQueue [8]
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range pcktParamsPtr->dsaParam.commonParams.dsaTagType [wrong enum values]
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range pcktParamsPtr->dsaParam.commonParams.vpt [8]
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range pcktParamsPtr->dsaParam.commonParams.cfiBit [2]
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range pcktParamsPtr->dsaParam.commonParams.vid [4096]
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with out of range pcktParamsPtr->dsaParam.dsaType [wrong enum values]
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with out of range pcktParamsPtr->dsaParam.toCpu.devNum [128]
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.10. Call with out of range pcktParamsPtr->dsaParam.toCpu.srcTrunkId [128]
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.11. Call with out of range pcktParamsPtr->dsaParam.toCpu.cpuCode [wrong enum values]
                    and other params from 1.1.
    Expected: GT_BAD_PARAM.

    ======= DSA_CMD_FROM_CPU =======
    1.12. Call with pcktParamsPtr {packetIsTagged [GT_FALSE],
                                  cookie [],
                                  sdmaInfo {recalcCrc [GT_FALSE],
                                            txQueue [3],
                                            invokeTxBufferQueueEvent [GT_FALSE] (not relevant)},
                                  dsaParam {commonParams {dsaTagType [CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E],
                                                          vpt[3],
                                                          cfiBit [1],
                                                          vid [100],
                                                          dropOnSource [GT_TRUE],
                                                          packetIsLooped [GT_TRUE]}
                                            dsaType [CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E],
                                            dsaInfo.fromCpu {dstInterface.type [CPSS_INTERFACE_PORT_E],
                                                             tc [7],
                                                             dp [CPSS_DP_GREEN_E],
                                                             egrFilterEn [GT_TRUE],
                                                             cascadeControl [GT_TRUE],
                                                             egrFilterRegistered [GT_TRUE],
                                                             srcId [0],
                                                             srcDev [dev]
                                                             extDestInfo.devPort {dstIsTagged [GT_TRUE],
                                                                                  mailBoxToNeighborCPU [GT_TRUE]}}}.
                   non NULL packetBuffsArrPtr,
                   non NULL buffLenArr,
                   numOfBufs [1]
    Expected: GT_OK.
    1.13. Call with out of range pcktParamsPtr->dsaParam.fromCpu.dstInterface.type [wrong enum values]
                    and other params from 1.12.
    Expected: GT_BAD_PARAM.
    1.14. Call with out of range pcktParamsPtr->dsaParam.fromCpu.tc [8]
                    and other params from 1.12.
    Expected: NOT GT_OK.
    1.15. Call with out of range pcktParamsPtr->dsaParam.fromCpu.dp [wrong enum values]
                    and other params from 1.12.
    Expected: GT_BAD_PARAM.
    1.16. Call with out of range pcktParamsPtr->dsaParam.fromCpu.srcDev [128]
                    and other params from 1.12.
    Expected: GT_BAD_PARAM.

    ======= DSA_CMD_TO_ANALYZER =======
    1.17. Call with pcktParamsPtr {packetIsTagged [GT_TRUE],
                                  cookie [],
                                  sdmaInfo {recalcCrc [GT_FALSE],
                                            txQueue [5],
                                            invokeTxBufferQueueEvent [GT_FALSE] (not relevant)},
                                  dsaParam {commonParams {dsaTagType [CPSS_DXCH_NET_DSA_TYPE_REGULAR_E],
                                                          vpt[5],
                                                          cfiBit [0],
                                                          vid [1000],
                                                          dropOnSource [GT_FALSE],
                                                          packetIsLooped [GT_TRUE]}
                                            dsaType [CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E],
                                            dsaInfo.toAnalyzer {rxSniffer [GT_TRUE],
                                                                isTagged [GT_TRUE],
                                                                devPort.devNum [dev],
                                                                devPort.portNum [0]}}}.
                   non NULL packetBuffsArrPtr,
                   non NULL buffLenArr,
                   numOfBufs [1]
    Expected: GT_OK.
    1.18. Call with out of range pcktParamsPtr->dsaParam.toAnalyzer.devPort.devNum [128]
                    and other params from 1.17.
    Expected: GT_BAD_PARAM.
    1.19. Call with out of range pcktParamsPtr->dsaParam.toAnalyzer.devPort.portNum [64]
                    and other params from 1.17.
    Expected: GT_BAD_PARAM.

    ======= DSA_CMD_FORWARD =======
    1.20. Call with pcktParamsPtr {packetIsTagged [GT_FALSE],
                                  cookie [],
                                  sdmaInfo {recalcCrc [GT_TRUE],
                                            txQueue [7],
                                            invokeTxBufferQueueEvent [GT_FALSE] (not relevant)},
                                  dsaParam {commonParams {dsaTagType [CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E],
                                                          vpt[7],
                                                          cfiBit [1],
                                                          vid [0xFFF],
                                                          dropOnSource [GT_TRUE],
                                                          packetIsLooped [GT_FALSE]}
                                            dsaType [CPSS_DXCH_NET_DSA_CMD_FORWARD_E],
                                            dsaInfo.forward {srcIsTagged [GT_TRUE],
                                                             srcDev [dev]
                                                             srcIsTrunk [GT_TRUE],
                                                             source.trunkId [2],
                                                             srcId [0],
                                                             egrFilterRegistered [GT_TRUE],
                                                             wasRouted [GT_TRUE],
                                                             qosProfileIndex [0],
                                                             dstInterface}}}.
                   non NULL packetBuffsArrPtr,
                   non NULL buffLenArr,
                   numOfBufs [1]
    Expected: GT_OK.
    1.21. Call with out of range pcktParamsPtr->dsaParam.forward.srcDev [128]
                    and other params from 1.20.
    Expected: GT_BAD_PARAM.
    1.22. Call with out of range pcktParamsPtr->dsaParam.forward.source.trunkId [128]
                    and other params from 1.20.
    Expected: NOT GT_OK.
    1.23. Call with out of range pcktParamsPtr->dsaParam.forward.dstInterface.type [wrong enum values]
                    and other params from 1.20.
    Expected: GT_BAD_PARAM.

    1.24. Call with pcktParamsPtr [NULL]
                    and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.25. Call with packetBuffsArrPtr [NULL]
                    and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.26. Call with buffLenArr [NULL]
                    and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.27. Call with out of range numOfBufs [0 / 50]
                    and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_NET_TX_PARAMS_STC  pcktParams;
    GT_U8                       *packetBuffsArrPtr[1] = {prvUtSdmaBuffer};
    GT_U32                       buffLenArr[1] = {sizeof(prvUtSdmaBuffer)};
    GT_U32                       numOfBufs = 1;
    GT_U32                       cookie    = 0;

    
    cpssOsBzero((GT_VOID*) &pcktParams, sizeof(pcktParams));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if(IS_NETIF_MII_INIT_DONE(dev))
        {
            /*
                1.1. Call with pcktParamsPtr {packetIsTagged [GT_TRUE],
                                              cookie [],
                                              sdmaInfo {recalcCrc [GT_TRUE],
                                                        txQueue [0],
                                                        invokeTxBufferQueueEvent [GT_FALSE] (not relevant)},
                                              dsaParam {commonParams {dsaTagType [CPSS_DXCH_NET_DSA_TYPE_REGULAR_E],
                                                                      vpt[0],
                                                                      cfiBit [0],
                                                                      vid [0],
                                                                      dropOnSource [GT_FALSE],
                                                                      packetIsLooped [GT_FALSE]}
                                                        dsaType [CPSS_DXCH_NET_DSA_CMD_TO_CPU_E],
                                                        dsaInfo.toCpu {isEgressPipe [GT_TRUE],
                                                                       isTagged [GT_TRUE],
                                                                       devNum [dev],
                                                                       srcIsTrunk [GT_TRUE],
                                                                       interface [2],
                                                                       cpuCode [CPSS_NET_CONTROL_E],
                                                                       wasTruncated [GT_TRUE],
                                                                       originByteCount [0],
                                                                       timestamp [0]}}}.
                               non NULL packetBuffsArrPtr,
                               non NULL buffLenArr,
                               numOfBufs [1]
                Expected: GT_OK.
            */
            pcktParams.packetIsTagged = GT_TRUE;
            pcktParams.cookie         = &cookie;
    
            pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
            pcktParams.sdmaInfo.txQueue   = 0;
            pcktParams.sdmaInfo.evReqHndl = 0;
    
            pcktParams.dsaParam.commonParams.dsaTagType     = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E;
            pcktParams.dsaParam.commonParams.vpt            = 0;
            pcktParams.dsaParam.commonParams.cfiBit         = 0;
            pcktParams.dsaParam.commonParams.vid            = 0;
            pcktParams.dsaParam.commonParams.dropOnSource   = GT_FALSE;
            pcktParams.dsaParam.commonParams.packetIsLooped = GT_FALSE;
    
            pcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_TO_CPU_E;
    
            pcktParams.dsaParam.dsaInfo.toCpu.isEgressPipe    = GT_TRUE;
            pcktParams.dsaParam.dsaInfo.toCpu.isTagged        = GT_TRUE;
            pcktParams.dsaParam.dsaInfo.toCpu.devNum          = dev;
            pcktParams.dsaParam.dsaInfo.toCpu.srcIsTrunk      = GT_TRUE;
            pcktParams.dsaParam.dsaInfo.toCpu.interface.srcTrunkId = 2;
            pcktParams.dsaParam.dsaInfo.toCpu.cpuCode         = CPSS_NET_CONTROL_E;
            pcktParams.dsaParam.dsaInfo.toCpu.wasTruncated    = GT_TRUE;
            pcktParams.dsaParam.dsaInfo.toCpu.originByteCount = 0;
            pcktParams.dsaParam.dsaInfo.toCpu.timestamp       = 0;
    
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    
            /*
                1.2. Call with out of range pcktParamsPtr->cookie [NULL]
                               and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            pcktParams.cookie = NULL;
    
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pcktParamsPtr->cookie = NULL", dev);
    
            pcktParams.cookie = &cookie;
    
            /*
                1.3. Call with out of range pcktParamsPtr->sdmaInfo.txQueue [8]
                               and other params from 1.1.
                Expected: NOT GT_OK.
            */
            pcktParams.sdmaInfo.txQueue = 8;
    
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ->sdmaInfo.txQueue = %d",
                                             dev, pcktParams.sdmaInfo.txQueue);
    
            pcktParams.sdmaInfo.txQueue = 0;
    
            /*
                1.4. Call with out of range pcktParamsPtr->dsaParam.commonParams.dsaTagType [wrong enum values]
                               and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChNetIfMiiSyncTxPacketSend
                                (dev, &pcktParams, packetBuffsArrPtr, buffLenArr, numOfBufs),
                                pcktParams.dsaParam.commonParams.dsaTagType);
    
            /*
                1.5. Call with out of range pcktParamsPtr->dsaParam.commonParams.vpt [8]
                               and other params from 1.1.
                Expected: NOT GT_OK.
            */
            pcktParams.dsaParam.commonParams.vpt = 8;
    
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ->dsaParam.commonParams.vpt = %d",
                                             dev, pcktParams.dsaParam.commonParams.vpt);
    
            pcktParams.dsaParam.commonParams.vpt = 0;
    
            /*
                1.6. Call with out of range pcktParamsPtr->dsaParam.commonParams.cfiBit [2]
                               and other params from 1.1.
                Expected: NOT GT_OK.
            */
            pcktParams.dsaParam.commonParams.cfiBit = 2;
    
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ->dsaParam.commonParams.cfiBit = %d",
                                             dev, pcktParams.dsaParam.commonParams.cfiBit);
    
            pcktParams.dsaParam.commonParams.cfiBit = 0;
    
            /*
                1.7. Call with out of range pcktParamsPtr->dsaParam.commonParams.vid [4096]
                               and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            pcktParams.dsaParam.commonParams.vid = 4096;
    
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->dsaParam.commonParams.vid = %d",
                                         dev, pcktParams.dsaParam.commonParams.vid);
    
            pcktParams.dsaParam.commonParams.vid = 100;
    
            /*
                1.8. Call with out of range pcktParamsPtr->dsaParam.dsaType [wrong enum values]
                               and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChNetIfMiiSyncTxPacketSend
                                (dev, &pcktParams, packetBuffsArrPtr, buffLenArr, numOfBufs),
                                pcktParams.dsaParam.dsaType);

            /*
                1.9. Call with out of range pcktParamsPtr->dsaParam.toCpu.devNum [128]
                               and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            pcktParams.dsaParam.dsaInfo.toCpu.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->dsaParam.toCpu.devNum = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.toCpu.devNum);
    
            pcktParams.dsaParam.dsaInfo.toCpu.devNum = dev;

            /*
                1.10. Call with out of range pcktParamsPtr->dsaParam.toCpu.srcTrunkId [128]
                                and other params from 1.1.
                Expected: NOT GT_OK.
            */
            pcktParams.dsaParam.dsaInfo.toCpu.srcIsTrunk = 128;
    
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ->dsaParam.toCpu.srcIsTrunk = %d",
                                             dev, pcktParams.dsaParam.dsaInfo.toCpu.srcIsTrunk);
    
            pcktParams.dsaParam.dsaInfo.toCpu.srcIsTrunk = 2;

            /*
                1.11. Call with out of range pcktParamsPtr->dsaParam.toCpu.cpuCode [wrong enum values]
                                and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChNetIfMiiSyncTxPacketSend
                                (dev, &pcktParams, packetBuffsArrPtr, buffLenArr, numOfBufs),
                                pcktParams.dsaParam.dsaInfo.toCpu.cpuCode);

            /*
                1.12. Call with pcktParamsPtr {packetIsTagged [GT_FALSE],
                                               cookie [],
                                               sdmaInfo {recalcCrc [GT_FALSE],
                                                         txQueue [3],
                                                         invokeTxBufferQueueEvent [GT_FALSE] (not relevant)},
                                               dsaParam {commonParams {dsaTagType [CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E],
                                                                       vpt[3],
                                                                       cfiBit [1],
                                                                       vid [100],
                                                                       dropOnSource [GT_TRUE],
                                                                       packetIsLooped [GT_TRUE]}
                                                         dsaType [CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E],
                                                         dsaInfo.fromCpu {dstInterface.type [CPSS_INTERFACE_PORT_E],
                                                                          tc [7],
                                                                          dp [CPSS_DP_GREEN_E],
                                                                          egrFilterEn [GT_TRUE],
                                                                          cascadeControl [GT_TRUE],
                                                                          egrFilterRegistered [GT_TRUE],
                                                                          srcId [0],
                                                                          srcDev [dev]
                                                                          extDestInfo.devPort {dstIsTagged [GT_TRUE],
                                                                                               mailBoxToNeighborCPU [GT_TRUE]}}}.
                                non NULL packetBuffsArrPtr,
                                non NULL buffLenArr,
                                numOfBufs [1]
                 Expected: GT_OK.
            */
            pcktParams.packetIsTagged = GT_FALSE;
            
            pcktParams.sdmaInfo.recalcCrc = GT_FALSE;
            pcktParams.sdmaInfo.txQueue   = 3;
            
            pcktParams.dsaParam.commonParams.dsaTagType     = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;
            pcktParams.dsaParam.commonParams.vpt            = 3;
            pcktParams.dsaParam.commonParams.cfiBit         = 1;
            pcktParams.dsaParam.commonParams.vid            = 100;
            pcktParams.dsaParam.commonParams.dropOnSource   = GT_TRUE;
            pcktParams.dsaParam.commonParams.packetIsLooped = GT_TRUE;
    
            pcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;
    
            pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_PORT_E;
            pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum  = dev;
            pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = 0;

            pcktParams.dsaParam.dsaInfo.fromCpu.tc = 7;
            pcktParams.dsaParam.dsaInfo.fromCpu.dp = CPSS_DP_GREEN_E;
            pcktParams.dsaParam.dsaInfo.fromCpu.egrFilterEn         = GT_TRUE;
            pcktParams.dsaParam.dsaInfo.fromCpu.cascadeControl      = GT_TRUE;
            pcktParams.dsaParam.dsaInfo.fromCpu.egrFilterRegistered = GT_TRUE;
            pcktParams.dsaParam.dsaInfo.fromCpu.srcId  = 0;
            pcktParams.dsaParam.dsaInfo.fromCpu.srcDev = dev;

            pcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged = GT_TRUE;
            pcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.mailBoxToNeighborCPU = GT_TRUE;
            
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.13. Call with out of range pcktParamsPtr->dsaParam.fromCpu.dstInterface.type [wrong enum values]
                                and other params from 1.12.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChNetIfMiiSyncTxPacketSend
                                (dev, &pcktParams, packetBuffsArrPtr, buffLenArr, numOfBufs),
                                pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type);

            /*
                1.14. Call with out of range pcktParamsPtr->dsaParam.fromCpu.tc [8]
                                and other params from 1.12.
                Expected: NOT GT_OK.
            */
            pcktParams.dsaParam.dsaInfo.fromCpu.tc = 8;
    
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ->dsaParam.fromCpu.tc = %d",
                                             dev, pcktParams.dsaParam.dsaInfo.fromCpu.tc);
    
            pcktParams.dsaParam.dsaInfo.fromCpu.tc = 0;

            /*
                1.15. Call with out of range pcktParamsPtr->dsaParam.fromCpu.dp [wrong enum values]
                                and other params from 1.12.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChNetIfMiiSyncTxPacketSend
                                (dev, &pcktParams, packetBuffsArrPtr, buffLenArr, numOfBufs),
                                pcktParams.dsaParam.dsaInfo.fromCpu.dp);

            /*
                1.16. Call with out of range pcktParamsPtr->dsaParam.fromCpu.srcDev [128]
                                and other params from 1.12.
                Expected: GT_BAD_PARAM.
            */
            pcktParams.dsaParam.dsaInfo.fromCpu.srcDev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->dsaParam.fromCpu.srcDev = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.fromCpu.srcDev);
    
            pcktParams.dsaParam.dsaInfo.fromCpu.srcDev = dev;

            /*
                1.17. Call with pcktParamsPtr {packetIsTagged [GT_TRUE],
                                               cookie [],
                                               sdmaInfo {recalcCrc [GT_FALSE],
                                                         txQueue [5],
                                                         invokeTxBufferQueueEvent [GT_FALSE] (not relevant)},
                                               dsaParam {commonParams {dsaTagType [CPSS_DXCH_NET_DSA_TYPE_REGULAR_E],
                                                                       vpt[5],
                                                                       cfiBit [0],
                                                                       vid [1000],
                                                                       dropOnSource [GT_FALSE],
                                                                       packetIsLooped [GT_TRUE]}
                                                         dsaType [CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E],
                                                         dsaInfo.toAnalyzer {rxSniffer [GT_TRUE],
                                                                             isTagged [GT_TRUE],
                                                                             devPort.devNum [dev],
                                                                             devPort.portNum [0]}}}.
                                non NULL packetBuffsArrPtr,
                                non NULL buffLenArr,
                                numOfBufs [1]
                Expected: GT_OK.
            */
            pcktParams.packetIsTagged = GT_TRUE;
            
            pcktParams.sdmaInfo.recalcCrc = GT_FALSE;
            pcktParams.sdmaInfo.txQueue   = 5;
            
            pcktParams.dsaParam.commonParams.dsaTagType     = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E;
            pcktParams.dsaParam.commonParams.vpt            = 5;
            pcktParams.dsaParam.commonParams.cfiBit         = 0;
            pcktParams.dsaParam.commonParams.vid            = 1000;
            pcktParams.dsaParam.commonParams.dropOnSource   = GT_FALSE;
            pcktParams.dsaParam.commonParams.packetIsLooped = GT_TRUE;
    
            pcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E;
    
            pcktParams.dsaParam.dsaInfo.toAnalyzer.rxSniffer = GT_TRUE;
            pcktParams.dsaParam.dsaInfo.toAnalyzer.isTagged  = GT_TRUE;
            pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.devNum = dev;
            pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = 0;
            
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.18. Call with out of range pcktParamsPtr->dsaParam.toAnalyzer.devPort.devNum [128]
                                and other params from 1.17.
                Expected: GT_BAD_PARAM.
            */
            pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->dsaParam.toAnalyzer.devNum = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.devNum);
    
            pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.devNum = dev;

            /*
                1.19. Call with out of range pcktParamsPtr->dsaParam.toAnalyzer.devPort.portNum [64]
                                and other params from 1.17.
                Expected: GT_BAD_PARAM.
            */
            pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = CPSS_MAX_PORTS_NUM_CNS;
    
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->dsaParam.toAnalyzer.portNum = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum);
    
            pcktParams.dsaParam.dsaInfo.toAnalyzer.devPort.portNum = 0;

            /*
                1.20. Call with pcktParamsPtr {packetIsTagged [GT_FALSE],
                                               cookie [],
                                               sdmaInfo {recalcCrc [GT_TRUE],
                                                         txQueue [7],
                                                         invokeTxBufferQueueEvent [GT_FALSE] (not relevant)},
                                               dsaParam {commonParams {dsaTagType [CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E],
                                                                       vpt[7],
                                                                       cfiBit [1],
                                                                       vid [0xFFF],
                                                                       dropOnSource [GT_TRUE],
                                                                       packetIsLooped [GT_FALSE]}
                                                         dsaType [CPSS_DXCH_NET_DSA_CMD_FORWARD_E],
                                                         dsaInfo.forward {srcIsTagged [GT_TRUE],
                                                                          srcDev [dev]
                                                                          srcIsTrunk [GT_TRUE],
                                                                          source.trunkId [2],
                                                                          srcId [0],
                                                                          egrFilterRegistered [GT_TRUE],
                                                                          wasRouted [GT_TRUE],
                                                                          qosProfileIndex [0],
                                                                          dstInterface}}}.
                                non NULL packetBuffsArrPtr,
                                non NULL buffLenArr,
                                numOfBufs [1]
                Expected: GT_OK.
            */
            pcktParams.packetIsTagged = GT_FALSE;
            
            pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
            pcktParams.sdmaInfo.txQueue   = 7;
            
            pcktParams.dsaParam.commonParams.dsaTagType     = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;
            pcktParams.dsaParam.commonParams.vpt            = 7;
            pcktParams.dsaParam.commonParams.cfiBit         = 1;
            pcktParams.dsaParam.commonParams.vid            = 0xFFF;
            pcktParams.dsaParam.commonParams.dropOnSource   = GT_TRUE;
            pcktParams.dsaParam.commonParams.packetIsLooped = GT_FALSE;
    
            pcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FORWARD_E;
    
            pcktParams.dsaParam.dsaInfo.forward.srcIsTagged     = GT_TRUE;
            pcktParams.dsaParam.dsaInfo.forward.srcDev          = dev;
            pcktParams.dsaParam.dsaInfo.forward.srcIsTrunk      = GT_TRUE;
            pcktParams.dsaParam.dsaInfo.forward.source.trunkId  = 2;
            pcktParams.dsaParam.dsaInfo.forward.srcId           = 0;
            pcktParams.dsaParam.dsaInfo.forward.egrFilterRegistered = GT_TRUE;
            pcktParams.dsaParam.dsaInfo.forward.wasRouted       = GT_TRUE;
            pcktParams.dsaParam.dsaInfo.forward.qosProfileIndex = 0;
    
            pcktParams.dsaParam.dsaInfo.forward.dstInterface.type   = CPSS_INTERFACE_VID_E;
            pcktParams.dsaParam.dsaInfo.forward.dstInterface.vlanId = 0xFFF;
            
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.21. Call with out of range pcktParamsPtr->dsaParam.forward.srcDev [128]
                                and other params from 1.20.
                Expected: GT_BAD_PARAM.
            */
            pcktParams.dsaParam.dsaInfo.forward.srcDev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->dsaParam.forward.srcDev = %d",
                                         dev, pcktParams.dsaParam.dsaInfo.forward.srcDev);
    
            pcktParams.dsaParam.dsaInfo.forward.srcDev = dev;

            /*
                1.22. Call with out of range pcktParamsPtr->dsaParam.forward.source.trunkId [128]
                                and other params from 1.20.
                Expected: NOT GT_OK.
            */
            pcktParams.dsaParam.dsaInfo.forward.source.trunkId = 128;
    
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ->dsaParam.forward.source.trunkId = %d",
                                             dev, pcktParams.dsaParam.dsaInfo.forward.source.trunkId);
    
            pcktParams.dsaParam.dsaInfo.forward.source.trunkId = 2;

            /*
                1.23. Call with out of range pcktParamsPtr->dsaParam.forward.dstInterface.type [wrong enum values]
                                and other params from 1.20.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChNetIfMiiSyncTxPacketSend
                                (dev, &pcktParams, packetBuffsArrPtr, buffLenArr, numOfBufs),
                                pcktParams.dsaParam.dsaInfo.forward.dstInterface.type);

            /*
                1.24. Call with pcktParamsPtr [NULL]
                                and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, NULL, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pcktParamsPtr = NULL", dev);

            /*
                1.25. Call with packetBuffsArrPtr [NULL]
                                and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, NULL,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, packetBuffsArrPtr = NULL", dev);

            /*
                1.26. Call with buffLenArr [NULL]
                                and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  NULL, numOfBufs);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, buffLenArr = NULL", dev);

            /*
                1.27. Call with out of range numOfBufs [0 / 50]
                                and other params from 1.1.
                Expected: NOT GT_OK.
            */

            /* call with numOfBufs = 0 */
            numOfBufs = 0;

            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfBufs = %d",
                                             dev, numOfBufs);

            /* call with numOfBufs = 50 */
            numOfBufs = 50;

            st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                                  buffLenArr, numOfBufs);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfBufs = %d",
                                             dev, numOfBufs);

            numOfBufs = 1;
        }
    }

    pcktParams.packetIsTagged = GT_TRUE;
    pcktParams.cookie         = &cookie;

    pcktParams.sdmaInfo.recalcCrc = GT_TRUE;
    pcktParams.sdmaInfo.txQueue   = 0;
    pcktParams.sdmaInfo.evReqHndl = 0;

    pcktParams.dsaParam.commonParams.dsaTagType     = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E;
    pcktParams.dsaParam.commonParams.vpt            = 0;
    pcktParams.dsaParam.commonParams.cfiBit         = 0;
    pcktParams.dsaParam.commonParams.vid            = 0;
    pcktParams.dsaParam.commonParams.dropOnSource   = GT_FALSE;
    pcktParams.dsaParam.commonParams.packetIsLooped = GT_FALSE;

    pcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_TO_CPU_E;

    pcktParams.dsaParam.dsaInfo.toCpu.isEgressPipe    = GT_TRUE;
    pcktParams.dsaParam.dsaInfo.toCpu.isTagged        = GT_TRUE;
    pcktParams.dsaParam.dsaInfo.toCpu.devNum          = dev;
    pcktParams.dsaParam.dsaInfo.toCpu.srcIsTrunk      = GT_TRUE;
    pcktParams.dsaParam.dsaInfo.toCpu.interface.srcTrunkId = 2;
    pcktParams.dsaParam.dsaInfo.toCpu.cpuCode         = CPSS_NET_CONTROL_E;
    pcktParams.dsaParam.dsaInfo.toCpu.wasTruncated    = GT_TRUE;
    pcktParams.dsaParam.dsaInfo.toCpu.originByteCount = 0;
    pcktParams.dsaParam.dsaInfo.toCpu.timestamp       = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                              buffLenArr, numOfBufs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChNetIfMiiSyncTxPacketSend(dev, &pcktParams, packetBuffsArrPtr,
                                          buffLenArr, numOfBufs);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChNetIfMii suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChNetIfMii)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfMiiInit)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfMiiRxBufFree)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfMiiRxPacketGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfMiiTxBufferQueueGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfMiiTxPacketSend)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChNetIfMiiSyncTxPacketSend)
UTF_SUIT_END_TESTS_MAC(cpssDxChNetIfMii)

