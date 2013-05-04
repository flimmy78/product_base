/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmFabricPktReasmUT.c
*
* DESCRIPTION:
*   Unit tests for cpssExMxPmFabricPktReasm, that provides
*   CPSS ExMxPm Fabric Connectivity Packet reassembly APIs.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricPktReasm.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define FABRIC_PKT_REASM_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktReasmControlConfigSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_EXMXPM_FABRIC_PKT_REASM_CONTROL_CONFIG_STC *controlConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktReasmControlConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with controlConfigPtr {remoteCpuCellAction [CPSS_EXMXPM_FABRIC_PKT_REASM_REMOTE_CPU_CELL_DROP_E / 
                                                          CPSS_EXMXPM_FABRIC_PKT_REASM_REMOTE_CPU_CELL_PASS_E], 
                                     bindContextMode [CPSS_EXMXPM_FABRIC_BIND_CONTEXT_CLASS_BASED_LBI_E / 
                                                      CPSS_EXMXPM_FABRIC_BIND_CONTEXT_FLOW_BASED_E], 
                                     buffersDropEnable [GT_FALSE / GT_TRUE], 
                                     maxNumOfBuffers [0 / 31], 
                                     checkCrc [GT_FALSE / GT_TRUE]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricPktReasmControlConfigGet 
              with non-NULL controlConfigPtr.
    Expected: GT_OK and the same controlConfigPtr.
    1.3. Call with out of range controlConfigPtr->remoteCpuCellAction [0x5AAAAAA5] 
              and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range controlConfigPtr->bindContextMode [0x5AAAAAA5] 
              and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range controlConfigPtr->maxNumOfBuffers [32] 
              and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with controlConfigPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_REASM_CONTROL_CONFIG_STC controlConfig;
    CPSS_EXMXPM_FABRIC_PKT_REASM_CONTROL_CONFIG_STC controlConfigGet;


    cpssOsBzero((GT_VOID*) &controlConfig, sizeof(controlConfig));
    cpssOsBzero((GT_VOID*) &controlConfigGet, sizeof(controlConfigGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with controlConfigPtr {remoteCpuCellAction [CPSS_EXMXPM_FABRIC_PKT_REASM_REMOTE_CPU_CELL_DROP_E / 
                                                                  CPSS_EXMXPM_FABRIC_PKT_REASM_REMOTE_CPU_CELL_PASS_E], 
                                             bindContextMode [CPSS_EXMXPM_FABRIC_BIND_CONTEXT_CLASS_BASED_LBI_E / 
                                                              CPSS_EXMXPM_FABRIC_BIND_CONTEXT_FLOW_BASED_E], 
                                             buffersDropEnable [GT_FALSE / GT_TRUE], 
                                             maxNumOfBuffers [0 / 31], 
                                             checkCrc [GT_FALSE / GT_TRUE]}.
            Expected: GT_OK.
        */
        /* Call with remoteCpuCellAction = CPSS_EXMXPM_FABRIC_PKT_REASM_REMOTE_CPU_CELL_DROP_E */
        controlConfig.remoteCpuCellAction = CPSS_EXMXPM_FABRIC_PKT_REASM_REMOTE_CPU_CELL_DROP_E;
        controlConfig.bindContextMode     = CPSS_EXMXPM_FABRIC_BIND_CONTEXT_CLASS_BASED_LBI_E;
        controlConfig.buffersDropEnable   = GT_FALSE;
        controlConfig.maxNumOfBuffers     = 0;
        controlConfig.checkCrc            = GT_FALSE;

        st = cpssExMxPmFabricPktReasmControlConfigSet(dev, &controlConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmFabricPktReasmControlConfigGet 
                      with non-NULL controlConfigPtr.
            Expected: GT_OK and the same controlConfigPtr.
        */
        st = cpssExMxPmFabricPktReasmControlConfigGet(dev, &controlConfigGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktReasmControlConfigGet: %d", dev);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(controlConfig.remoteCpuCellAction, controlConfigGet.remoteCpuCellAction, 
                                    "get another controlConfigGetPtr->remoteCpuCellAction then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(controlConfig.bindContextMode, controlConfigGet.bindContextMode, 
                                    "get another controlConfigGetPtr->bindContextMode then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(controlConfig.buffersDropEnable, controlConfigGet.buffersDropEnable, 
                                    "get another controlConfigGetPtr->buffersDropEnable then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(controlConfig.maxNumOfBuffers, controlConfigGet.maxNumOfBuffers, 
                                    "get another controlConfigGetPtr->maxNumOfBuffers then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(controlConfig.checkCrc, controlConfigGet.checkCrc, 
                                    "get another controlConfigGetPtr->checkCrc then was set: %d", dev);

        /* Call with remoteCpuCellAction = CPSS_EXMXPM_FABRIC_PKT_REASM_REMOTE_CPU_CELL_PASS_E */
        controlConfig.remoteCpuCellAction = CPSS_EXMXPM_FABRIC_PKT_REASM_REMOTE_CPU_CELL_PASS_E;
        controlConfig.bindContextMode     = CPSS_EXMXPM_FABRIC_BIND_CONTEXT_FLOW_BASED_E;
        controlConfig.buffersDropEnable   = GT_TRUE;
        controlConfig.maxNumOfBuffers     = 31;
        controlConfig.checkCrc            = GT_TRUE;

        st = cpssExMxPmFabricPktReasmControlConfigSet(dev, &controlConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmFabricPktReasmControlConfigGet 
                      with non-NULL controlConfigPtr.
            Expected: GT_OK and the same controlConfigPtr.
        */
        st = cpssExMxPmFabricPktReasmControlConfigGet(dev, &controlConfigGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktReasmControlConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(controlConfig.remoteCpuCellAction, controlConfigGet.remoteCpuCellAction, 
                                    "get another controlConfigGetPtr->remoteCpuCellAction then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(controlConfig.bindContextMode, controlConfigGet.bindContextMode, 
                                    "get another controlConfigGetPtr->bindContextMode then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(controlConfig.buffersDropEnable, controlConfigGet.buffersDropEnable, 
                                    "get another controlConfigGetPtr->buffersDropEnable then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(controlConfig.maxNumOfBuffers, controlConfigGet.maxNumOfBuffers, 
                                    "get another controlConfigGetPtr->maxNumOfBuffers then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(controlConfig.checkCrc, controlConfigGet.checkCrc, 
                                    "get another controlConfigGetPtr->checkCrc then was set: %d", dev);

        /*
            1.3. Call with out of range controlConfigPtr->remoteCpuCellAction [0x5AAAAAA5] 
                      and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        controlConfig.remoteCpuCellAction = FABRIC_PKT_REASM_INVALID_ENUM_CNS;
        
        st = cpssExMxPmFabricPktReasmControlConfigSet(dev, &controlConfig);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, controlConfigPtr->remoteCpuCellAction = %d",
                                     dev, controlConfig.remoteCpuCellAction);

        controlConfig.remoteCpuCellAction = CPSS_EXMXPM_FABRIC_PKT_REASM_REMOTE_CPU_CELL_PASS_E;

        /*
            1.4. Call with out of range controlConfigPtr->bindContextMode [0x5AAAAAA5] 
                      and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        controlConfig.bindContextMode = FABRIC_PKT_REASM_INVALID_ENUM_CNS;
        
        st = cpssExMxPmFabricPktReasmControlConfigSet(dev, &controlConfig);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, controlConfigPtr->bindContextMode = %d",
                                     dev, controlConfig.bindContextMode);

        controlConfig.bindContextMode = CPSS_EXMXPM_FABRIC_BIND_CONTEXT_FLOW_BASED_E;

        /*
            1.5. Call with out of range controlConfigPtr->maxNumOfBuffers [32] 
                      and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        controlConfig.maxNumOfBuffers = 32;
        
        st = cpssExMxPmFabricPktReasmControlConfigSet(dev, &controlConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, controlConfigPtr->maxNumOfBuffers = %d",
                                         dev, controlConfig.maxNumOfBuffers);

        controlConfig.maxNumOfBuffers = 31;

        /*
            1.6. Call with controlConfigPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktReasmControlConfigSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, controlConfigPtr = NULL", dev);
    }

    controlConfig.remoteCpuCellAction = CPSS_EXMXPM_FABRIC_PKT_REASM_REMOTE_CPU_CELL_DROP_E;
    controlConfig.bindContextMode     = CPSS_EXMXPM_FABRIC_BIND_CONTEXT_CLASS_BASED_LBI_E;
    controlConfig.buffersDropEnable   = GT_FALSE;
    controlConfig.maxNumOfBuffers     = 0;
    controlConfig.checkCrc            = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktReasmControlConfigSet(dev, &controlConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktReasmControlConfigSet(dev, &controlConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktReasmControlConfigGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_EXMXPM_FABRIC_PKT_REASM_CONTROL_CONFIG_STC *controlConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktReasmControlConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL controlConfigPtr.
    Expected: GT_OK.
    1.2. Call with controlConfigPtr [NULL].
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_REASM_CONTROL_CONFIG_STC controlConfig;


    cpssOsBzero((GT_VOID*) &controlConfig, sizeof(controlConfig));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL controlConfigPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmFabricPktReasmControlConfigGet(dev, &controlConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with controlConfigPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktReasmControlConfigGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktReasmControlConfigGet(dev, &controlConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktReasmControlConfigGet(dev, &controlConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktReasmFifosConfigSet
(
    IN  GT_U8                                            devNum,
    IN  CPSS_EXMXPM_FABRIC_PKT_REASM_FIFOS_CONFIG_STC    *fifoSizeThrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktReasmFifosConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fifoSizeThrPtr {rxDataThrXOff [0 / 127], 
                                   rxDataThrXOn [0 / 127], 
                                   e2eThrXOff [0 / 31], 
                                   e2eThrXOn [0 / 31]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricPktReasmFifosConfigGet 
              with non-NULL fifoSizeThrPtr.
    Expected: GT_OK and the same fifoSizeThrPtr.
    1.3. Call with out of range fifoSizeThrPtr->rxDataThrXOff [128] 
              and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range fifoSizeThrPtr-> rxDataThrXOn [128] 
              and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range fifoSizeThrPtr->e2eThrXOff [32] 
              and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range fifoSizeThrPtr->e2eThrXOn [32] 
              and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with fifoSizeThrPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_REASM_FIFOS_CONFIG_STC   fifoSizeThr;
    CPSS_EXMXPM_FABRIC_PKT_REASM_FIFOS_CONFIG_STC   fifoSizeThrGet;
    
    
    cpssOsBzero((GT_VOID*) &fifoSizeThr, sizeof(fifoSizeThr));
    cpssOsBzero((GT_VOID*) &fifoSizeThrGet, sizeof(fifoSizeThrGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fifoSizeThrPtr {rxDataThrXOff [0 / 127], 
                                           rxDataThrXOn [0 / 127], 
                                           e2eThrXOff [0 / 31], 
                                           e2eThrXOn [0 / 31]}.
            Expected: GT_OK.
        */
        fifoSizeThr.rxDataThrXOff = 0;
        fifoSizeThr.rxDataThrXOn  = 0;
        fifoSizeThr.e2eThrXOff    = 0;
        fifoSizeThr.e2eThrXOn     = 0;

        st = cpssExMxPmFabricPktReasmFifosConfigSet(dev, &fifoSizeThr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmFabricPktReasmFifosConfigGet 
                      with non-NULL fifoSizeThrPtr.
            Expected: GT_OK and the same fifoSizeThrPtr.
        */
        st = cpssExMxPmFabricPktReasmFifosConfigGet(dev, &fifoSizeThrGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktReasmFifosConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fifoSizeThr.rxDataThrXOff, fifoSizeThrGet.rxDataThrXOff,
                                     "get another fifoSizeThrPtr->rxDataThrXOff then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fifoSizeThr.rxDataThrXOn, fifoSizeThrGet.rxDataThrXOn,
                                     "get another fifoSizeThrPtr->rxDataThrXOn then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fifoSizeThr.e2eThrXOff, fifoSizeThrGet.e2eThrXOff,
                                     "get another fifoSizeThrPtr->e2eThrXOff then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fifoSizeThr.e2eThrXOn, fifoSizeThrGet.e2eThrXOn,
                                     "get another fifoSizeThrPtr->e2eThrXOn then was set: %d", dev);
        
        /*
            1.1. Call with fifoSizeThrPtr {rxDataThrXOff [0 / 127], 
                                           rxDataThrXOn [0 / 127], 
                                           e2eThrXOff [0 / 31], 
                                           e2eThrXOn [0 / 31]}.
            Expected: GT_OK.
        */
        fifoSizeThr.rxDataThrXOff = 127;
        fifoSizeThr.rxDataThrXOn  = 127;
        fifoSizeThr.e2eThrXOff    = 31;
        fifoSizeThr.e2eThrXOn     = 31;

        st = cpssExMxPmFabricPktReasmFifosConfigSet(dev, &fifoSizeThr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmFabricPktReasmFifosConfigGet 
                      with non-NULL fifoSizeThrPtr.
            Expected: GT_OK and the same fifoSizeThrPtr.
        */
        st = cpssExMxPmFabricPktReasmFifosConfigGet(dev, &fifoSizeThrGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktReasmFifosConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fifoSizeThr.rxDataThrXOff, fifoSizeThrGet.rxDataThrXOff,
                                     "get another fifoSizeThrPtr->rxDataThrXOff then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fifoSizeThr.rxDataThrXOn, fifoSizeThrGet.rxDataThrXOn,
                                     "get another fifoSizeThrPtr->rxDataThrXOn then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fifoSizeThr.e2eThrXOff, fifoSizeThrGet.e2eThrXOff,
                                     "get another fifoSizeThrPtr->e2eThrXOff then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fifoSizeThr.e2eThrXOn, fifoSizeThrGet.e2eThrXOn,
                                     "get another fifoSizeThrPtr->e2eThrXOn then was set: %d", dev);

        /*
            1.3. Call with out of range fifoSizeThrPtr->rxDataThrXOff [128] 
                      and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fifoSizeThr.rxDataThrXOff = 128;
        
        st = cpssExMxPmFabricPktReasmFifosConfigSet(dev, &fifoSizeThr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fifoSizeThrPtr->rxDataThrXOff = %d",
                                         dev, fifoSizeThr.rxDataThrXOff);
        
        fifoSizeThr.rxDataThrXOff = 127;

        /*
            1.4. Call with out of range fifoSizeThrPtr->rxDataThrXOn [128] 
                      and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fifoSizeThr.rxDataThrXOn = 128;
        
        st = cpssExMxPmFabricPktReasmFifosConfigSet(dev, &fifoSizeThr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fifoSizeThrPtr->rxDataThrXOn = %d",
                                         dev, fifoSizeThr.rxDataThrXOn);
        
        fifoSizeThr.rxDataThrXOn = 127;

        /*
            1.5. Call with out of range fifoSizeThrPtr->e2eThrXOff [32] 
                      and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fifoSizeThr.e2eThrXOff = 32;
        
        st = cpssExMxPmFabricPktReasmFifosConfigSet(dev, &fifoSizeThr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fifoSizeThrPtr->e2eThrXOff = %d",
                                         dev, fifoSizeThr.e2eThrXOff);
        
        fifoSizeThr.e2eThrXOff = 31;

        /*
            1.6. Call with out of range fifoSizeThrPtr->e2eThrXOn [32] 
                      and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fifoSizeThr.e2eThrXOn = 32;
        
        st = cpssExMxPmFabricPktReasmFifosConfigSet(dev, &fifoSizeThr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fifoSizeThrPtr->e2eThrXOn = %d",
                                         dev, fifoSizeThr.e2eThrXOn);
        
        fifoSizeThr.e2eThrXOn = 31;

        /*
            1.9. Call with fifoSizeThrPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktReasmFifosConfigSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fifoSizeThrPtr = NULL", dev);
    }

    fifoSizeThr.rxDataThrXOff = 0;
    fifoSizeThr.rxDataThrXOn  = 0;
    fifoSizeThr.e2eThrXOff    = 0;
    fifoSizeThr.e2eThrXOn     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktReasmFifosConfigSet(dev, &fifoSizeThr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktReasmFifosConfigSet(dev, &fifoSizeThr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktReasmFifosConfigGet
(
    IN  GT_U8                                               devNum,
    OUT CPSS_EXMXPM_FABRIC_PKT_REASM_FIFOS_CONFIG_STC       *fifoSizeThrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktReasmFifosConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL fifoSizeThrPtr.
    Expected: GT_OK.
    1.2. Call with fifoSizeThrPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_REASM_FIFOS_CONFIG_STC   fifoSizeThr;
    

    cpssOsBzero((GT_VOID*) &fifoSizeThr, sizeof(fifoSizeThr));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL fifoSizeThrPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmFabricPktReasmFifosConfigGet(dev, &fifoSizeThr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with fifoSizeThrPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktReasmFifosConfigGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fifoSizeThrPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktReasmFifosConfigGet(dev, &fifoSizeThr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktReasmFifosConfigGet(dev, &fifoSizeThr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktReasmLastCellDropStatusGet
(
    IN  GT_U8                                                     devNum,
    OUT CPSS_EXMXPM_FABRIC_PKT_REASM_LAST_CELL_DROP_STATUS_STC    *dropStatusPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktReasmLastCellDropStatusGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL dropStatusPtr.
    Expected: GT_OK.
    1.2. Call with dropStatusPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_REASM_LAST_CELL_DROP_STATUS_STC  dropStatus;


    cpssOsBzero((GT_VOID*) &dropStatus, sizeof(dropStatus));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL dropStatusPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmFabricPktReasmLastCellDropStatusGet(dev, &dropStatus);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with dropStatusPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktReasmLastCellDropStatusGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropStatusPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktReasmLastCellDropStatusGet(dev, &dropStatus);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktReasmLastCellDropStatusGet(dev, &dropStatus);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktReasmGlobalCellCounterGet
(
    IN  GT_U8                                             devNum,
    IN  CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_TYPE_ENT cellCounterType,
    OUT GT_U32                                           *cellCounterPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktReasmGlobalCellCounterGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cellCounterType [CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_DROPPED_PKT_E / 
                                    CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_DATA_FIFO_FULL_DROP_E / 
                                    CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_E2E_MESSAGES_E / 
                                    CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_UNKNOWN_TYPE_DROP_E] 
                   and non-NULL cellCounterPtr.
    Expected: GT_OK.
    1.2. Call with out of range cellCounterType [0x5AAAAAA5] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with cellCounterPtr [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_TYPE_ENT cellCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_DROPPED_PKT_E;
    GT_U32                                            cellCounter     = 0;
    
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cellCounterType [CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_DROPPED_PKT_E / 
                                            CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_DATA_FIFO_FULL_DROP_E / 
                                            CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_E2E_MESSAGES_E / 
                                            CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_UNKNOWN_TYPE_DROP_E] 
                           and non-NULL cellCounterPtr.
            Expected: GT_OK.
        */
        /* Call with cellCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_DROPPED_PKT_E */
        cellCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_DROPPED_PKT_E;

        st = cpssExMxPmFabricPktReasmGlobalCellCounterGet(dev, cellCounterType, &cellCounter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cellCounterType);

        /* Call with cellCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_DATA_FIFO_FULL_DROP_E */
        cellCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_DATA_FIFO_FULL_DROP_E;

        st = cpssExMxPmFabricPktReasmGlobalCellCounterGet(dev, cellCounterType, &cellCounter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cellCounterType);

        /* Call with cellCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_E2E_MESSAGES_E */
        cellCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_E2E_MESSAGES_E;

        st = cpssExMxPmFabricPktReasmGlobalCellCounterGet(dev, cellCounterType, &cellCounter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cellCounterType);

        /* Call with cellCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_UNKNOWN_TYPE_DROP_E */
        cellCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_UNKNOWN_TYPE_DROP_E;

        st = cpssExMxPmFabricPktReasmGlobalCellCounterGet(dev, cellCounterType, &cellCounter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cellCounterType);

        /*
            1.2. Call with out of range cellCounterType [0x5AAAAAA5] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        cellCounterType = FABRIC_PKT_REASM_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricPktReasmGlobalCellCounterGet(dev, cellCounterType, &cellCounter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cellCounterType);

        cellCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_UNKNOWN_TYPE_DROP_E;

        /*
            1.3. Call with cellCounterPtr [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktReasmGlobalCellCounterGet(dev, cellCounterType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cellCounterPtr = NULL", dev);
    }

    cellCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_DROPPED_PKT_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktReasmGlobalCellCounterGet(dev, cellCounterType, &cellCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktReasmGlobalCellCounterGet(dev, cellCounterType, &cellCounter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktReasmPerSetCellCounterGet
(
    IN  GT_U8                                                    devNum,
    IN  CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_TYPE_ENT       setCounterType,
    IN  GT_U32                                                   setNum,
    OUT GT_U32                                                   *cntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktReasmPerSetCellCounterGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with setCounterType [CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E / 
                                   CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CRC_ERROR_E / 
                                   CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E], 
                   setNum [0 / 2 / 3] 
                   and non-NULL cntrPtr.
    Expected: GT_OK.
    1.2. Call with out of range setCounterType [0x5AAAAAA5] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with out of range setNum [4] 
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with cntrPtr [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_TYPE_ENT      setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E;
    GT_U32                                                  setNum         = 0;
    GT_U32                                                  cntr           = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with setCounterType [CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E / 
                                           CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CRC_ERROR_E / 
                                           CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E], 
                           setNum [0 / 2 / 3] 
                           and non-NULL cntrPtr.
            Expected: GT_OK.
        */
        /* Call with setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E */
        setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E;
        setNum         = 0;
        
        st = cpssExMxPmFabricPktReasmPerSetCellCounterGet(dev, setCounterType, setNum, &cntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, setCounterType, setNum);

        /* Call with setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CRC_ERROR_E */
        setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CRC_ERROR_E;
        setNum         = 2;
        
        st = cpssExMxPmFabricPktReasmPerSetCellCounterGet(dev, setCounterType, setNum, &cntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, setCounterType, setNum);

        /* Call with setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E */
        setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E;
        setNum         = 3;
        
        st = cpssExMxPmFabricPktReasmPerSetCellCounterGet(dev, setCounterType, setNum, &cntr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, setCounterType, setNum);

        /*
            1.2. Call with out of range setCounterType [0x5AAAAAA5] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        setCounterType = FABRIC_PKT_REASM_INVALID_ENUM_CNS;
        
        st = cpssExMxPmFabricPktReasmPerSetCellCounterGet(dev, setCounterType, setNum, &cntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, setCounterType);

        setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E;

        /*
            1.3. Call with out of range setNum [4] 
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        setNum = 4;
        
        st = cpssExMxPmFabricPktReasmPerSetCellCounterGet(dev, setCounterType, setNum, &cntr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, setNum = %d", dev, setNum);

        setNum = 3;

        /*
            1.4. Call with cntrPtr [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktReasmPerSetCellCounterGet(dev, setCounterType, setNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntrPtr = NULL", dev);
    }

    setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E;
    setNum         = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktReasmPerSetCellCounterGet(dev, setCounterType, setNum, &cntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktReasmPerSetCellCounterGet(dev, setCounterType, setNum, &cntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet
(
    IN  GT_U8                                                 devNum,
    IN  CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_TYPE_ENT    setCounterType,
    IN  GT_U32                                                setNum,
    IN  CPSS_EXMXPM_FABRIC_PKT_REASM_CNTR_CONFIG_STC         *cellCntrConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with setCounterType [CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E / 
                                   CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CRC_ERROR_E / 
                                   CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E], 
                   setNum [0 / 2 / 3] 
                   and cellCntrConfigPtr {onlyEopCells [GT_FALSE / GT_TRUE], 
                                          cellType [CPSS_EXMXPM_FABRIC_CELL_TYPE_MC_E / 
                                                    CPSS_EXMXPM_FABRIC_CELL_TYPE_ANY_E], 
                                          allFabricTc [GT_FALSE / GT_TRUE], 
                                          fabricTc [0 / 3], 
                                          allSrcDev [GT_FALSE / GT_TRUE], 
                                          srcDev [0 / dev]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet 
              with the same setCounterType, 
              setNum 
              and non-NULL cellCntrConfigPtr.
    Expected: GT_OK and the same cellCntrConfigPtr.
    1.3. Call with out of range setCounterType [0x5AAAAAA5] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range setNum [4] 
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range cellCntrConfigPtr->cellType [0x5AAAAAA5] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with cellCntrConfigPtr->cellType [CPSS_EXMXPM_FABRIC_CELL_TYPE_DIAG_E /
                                                CPSS_EXMXPM_FABRIC_CELL_TYPE_BC_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range cellCntrConfigPtr->fabricTc [4] 
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with out of range cellCntrConfigPtr->srcDev [PRV_CPSS_MAX_PP_DEVICES_CNS] 
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with cellCntrConfigPtr [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_TYPE_ENT  setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E;
    GT_U32                                              setNum         = 0;
    CPSS_EXMXPM_FABRIC_PKT_REASM_CNTR_CONFIG_STC        cellCntrConfig;
    CPSS_EXMXPM_FABRIC_PKT_REASM_CNTR_CONFIG_STC        cellCntrConfigGet;
    
    
    cpssOsBzero((GT_VOID*) &cellCntrConfig, sizeof(cellCntrConfig));
    cpssOsBzero((GT_VOID*) &cellCntrConfigGet, sizeof(cellCntrConfigGet));
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with setCounterType [CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E / 
                                           CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CRC_ERROR_E / 
                                           CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E], 
                           setNum [0 / 2 / 3] 
                           and cellCntrConfigPtr {onlyEopCells [GT_FALSE / GT_TRUE], 
                                                  cellType [CPSS_EXMXPM_FABRIC_CELL_TYPE_MC_E / 
                                                            CPSS_EXMXPM_FABRIC_CELL_TYPE_ANY_E], 
                                                  allFabricTc [GT_FALSE / GT_TRUE], 
                                                  fabricTc [0 / 3], 
                                                  allSrcDev [GT_FALSE / GT_TRUE], 
                                                  srcDev [0 / dev]}.
            Expected: GT_OK.
        */
        /* Call with setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E */
        setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E;
        setNum         = 0;

        cellCntrConfig.onlyEopCells    = GT_TRUE;
        cellCntrConfig.cellType        = CPSS_EXMXPM_FABRIC_CELL_TYPE_MC_E;
        cellCntrConfig.allFabricTc = GT_FALSE;
        cellCntrConfig.fabricTc        = 0;
        cellCntrConfig.allSrcDev   = GT_FALSE;
        cellCntrConfig.srcDev          = 0;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, setCounterType, setNum);

        /*
            1.2. Call cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet 
                      with the same setCounterType, 
                      setNum 
                      and non-NULL cellCntrConfigPtr.
            Expected: GT_OK and the same cellCntrConfigPtr.
        */
        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet(dev, setCounterType, setNum, &cellCntrConfigGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet: %d, %d, %d",
                                     dev, setCounterType, setNum);
        
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.onlyEopCells, cellCntrConfigGet.onlyEopCells,
                                     "get another cellCntrConfigPtr->onlyEopCells then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.cellType, cellCntrConfigGet.cellType,
                                     "get another cellCntrConfigPtr->cellType then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.allFabricTc, cellCntrConfigGet.allFabricTc,
                                     "get another cellCntrConfigPtr->allFabricTc then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.fabricTc, cellCntrConfigGet.fabricTc,
                                     "get another cellCntrConfigPtr->fabricTc then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.allSrcDev, cellCntrConfigGet.allSrcDev,
                                     "get another cellCntrConfigPtr->allSrcDev then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.srcDev, cellCntrConfigGet.srcDev,
                                     "get another cellCntrConfigPtr->srcDev then was set: %d", dev);

        /* Call with setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CRC_ERROR_E */
        setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CRC_ERROR_E;
        setNum         = 2;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, setCounterType, setNum);

        /*
            1.2. Call cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet 
                      with the same setCounterType, 
                      setNum 
                      and non-NULL cellCntrConfigPtr.
            Expected: GT_OK and the same cellCntrConfigPtr.
        */
        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet(dev, setCounterType, setNum, &cellCntrConfigGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet: %d, %d, %d",
                                     dev, setCounterType, setNum);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.onlyEopCells, cellCntrConfigGet.onlyEopCells,
                                     "get another cellCntrConfigPtr->onlyEopCells then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.cellType, cellCntrConfigGet.cellType,
                                     "get another cellCntrConfigPtr->cellType then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.allFabricTc, cellCntrConfigGet.allFabricTc,
                                     "get another cellCntrConfigPtr->allFabricTc then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.fabricTc, cellCntrConfigGet.fabricTc,
                                     "get another cellCntrConfigPtr->fabricTc then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.allSrcDev, cellCntrConfigGet.allSrcDev,
                                     "get another cellCntrConfigPtr->allSrcDev then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.srcDev, cellCntrConfigGet.srcDev,
                                     "get another cellCntrConfigPtr->srcDev then was set: %d", dev);

        /* Call with setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E */
        setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E;
        setNum         = 3;

        cellCntrConfig.onlyEopCells    = GT_FALSE;
        cellCntrConfig.cellType        = CPSS_EXMXPM_FABRIC_CELL_TYPE_ANY_E;
        cellCntrConfig.allFabricTc = GT_TRUE;
        cellCntrConfig.fabricTc        = 3;
        cellCntrConfig.allSrcDev   = GT_TRUE;
        cellCntrConfig.srcDev          = dev;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, setCounterType, setNum);

        /*
            1.2. Call cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet 
                      with the same setCounterType, 
                      setNum 
                      and non-NULL cellCntrConfigPtr.
            Expected: GT_OK and the same cellCntrConfigPtr.
        */
        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet(dev, setCounterType, setNum, &cellCntrConfigGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet: %d, %d, %d",
                                     dev, setCounterType, setNum);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.onlyEopCells, cellCntrConfigGet.onlyEopCells,
                                     "get another cellCntrConfigPtr->onlyEopCells then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.cellType, cellCntrConfigGet.cellType,
                                     "get another cellCntrConfigPtr->cellType then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.allFabricTc, cellCntrConfigGet.allFabricTc,
                                     "get another cellCntrConfigPtr->allFabricTc then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.fabricTc, cellCntrConfigGet.fabricTc,
                                     "get another cellCntrConfigPtr->fabricTc then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.allSrcDev, cellCntrConfigGet.allSrcDev,
                                     "get another cellCntrConfigPtr->allSrcDev then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(cellCntrConfig.srcDev, cellCntrConfigGet.srcDev,
                                     "get another cellCntrConfigPtr->srcDev then was set: %d", dev);

        /*
            1.3. Call with out of range setCounterType [0x5AAAAAA5] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        setCounterType = FABRIC_PKT_REASM_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, setCounterType);

        setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E;

        /*
            1.4. Call with out of range setNum [4] 
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        setNum = 4;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, setNum = %d", dev, setNum);

        setNum = 3;
        
        /*
            1.5. Call with out of range cellCntrConfigPtr->cellType [0x5AAAAAA5] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        cellCntrConfig.cellType = FABRIC_PKT_REASM_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cellCntrConfigPtr->cellType = %d",
                                     dev, cellCntrConfig.cellType);

        /*
            1.6. Call with cellCntrConfigPtr->cellType [CPSS_EXMXPM_FABRIC_CELL_TYPE_DIAG_E /
                                                        CPSS_EXMXPM_FABRIC_CELL_TYPE_BC_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        /* Call with cellCntrConfigPtr->cellType [CPSS_EXMXPM_FABRIC_CELL_TYPE_DIAG_E] */
        cellCntrConfig.cellType = CPSS_EXMXPM_FABRIC_CELL_TYPE_DIAG_E;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cellCntrConfigPtr->cellType = %d",
                                         dev, cellCntrConfig.cellType);

        /* Call with cellCntrConfigPtr->cellType [CPSS_EXMXPM_FABRIC_CELL_TYPE_BC_E] */
        cellCntrConfig.cellType = CPSS_EXMXPM_FABRIC_CELL_TYPE_BC_E;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cellCntrConfigPtr->cellType = %d",
                                         dev, cellCntrConfig.cellType);

        cellCntrConfig.cellType = CPSS_EXMXPM_FABRIC_CELL_TYPE_MC_E;
        
        /*
            1.7. Call with out of range cellCntrConfigPtr->fabricTc [4] 
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        cellCntrConfig.fabricTc = 4;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet(dev,setCounterType,setNum,&cellCntrConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cellCntrConfigPtr->fabricTc = %d",
                                         dev, cellCntrConfig.fabricTc);

        cellCntrConfig.fabricTc = 3;
        
        /*
            1.8. Call with out of range cellCntrConfigPtr->srcDev [PRV_CPSS_MAX_PP_DEVICES_CNS] 
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        cellCntrConfig.srcDev = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cellCntrConfigPtr->srcDev = %d",
                                         dev, cellCntrConfig.srcDev);

        cellCntrConfig.srcDev = dev;
        
        /*
            1.9. Call with cellCntrConfigPtr [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet(dev, setCounterType, setNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cellCntrConfigPtr = NULL", dev);
    }

    setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E;
    setNum         = 0;

    cellCntrConfig.onlyEopCells = GT_FALSE;
    cellCntrConfig.cellType     = CPSS_EXMXPM_FABRIC_CELL_TYPE_MC_E;
    cellCntrConfig.allFabricTc  = GT_FALSE;
    cellCntrConfig.fabricTc     = 0;
    cellCntrConfig.allSrcDev    = GT_FALSE;
    cellCntrConfig.srcDev       = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet(dev, setCounterType, setNum, &cellCntrConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet
(
    IN  GT_U8                                                 devNum,
    IN  CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_TYPE_ENT    setCounterType,
    IN  GT_U32                                                setNum,
    OUT CPSS_EXMXPM_FABRIC_PKT_REASM_CNTR_CONFIG_STC         *cellCntrConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with setCounterType [CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E / 
                                   CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CRC_ERROR_E / 
                                   CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E], 
                   setNum [0 / 2 / 3] 
                   and non-NULL cellCntrConfigPtr.
    Expected: GT_OK.
    1.2. Call with out of range setCounterType [0x5AAAAAA5] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with out of range setNum [4] 
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with cellCntrConfigPtr [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_TYPE_ENT  setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E;
    GT_U32                                              setNum         = 0;
    CPSS_EXMXPM_FABRIC_PKT_REASM_CNTR_CONFIG_STC        cellCntrConfig;


    cpssOsBzero((GT_VOID*) &cellCntrConfig, sizeof(cellCntrConfig));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with setCounterType [CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E / 
                                           CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CRC_ERROR_E / 
                                           CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E], 
                           setNum [0 / 2 / 3] 
                           and non-NULL cellCntrConfigPtr.
            Expected: GT_OK.
        */
        /* Call with setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E */
        setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E;
        setNum         = 0;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, setCounterType, setNum);

        /* Call with setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CRC_ERROR_E */
        setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CRC_ERROR_E;
        setNum         = 2;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, setCounterType, setNum);

        /* Call with setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E */
        setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E;
        setNum         = 3;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, setCounterType, setNum);

        /*
            1.2. Call with out of range setCounterType [0x5AAAAAA5] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        setCounterType = FABRIC_PKT_REASM_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, setCounterType);

        setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_ERROR_DROP_E;

        /*
            1.3. Call with out of range setNum [4] 
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        setNum = 4;

        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, setNum = %d", dev, setNum);

        setNum = 3;

        /*
            1.4. Call with cellCntrConfigPtr [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet(dev, setCounterType, setNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cellCntrConfigPtr = NULL", dev);
    }

    setCounterType = CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_CELL_E;
    setNum         = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet(dev, setCounterType, setNum, &cellCntrConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet(dev, setCounterType, setNum, &cellCntrConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktReasmPsmAgingSet 
(
    IN  GT_U8                               devNum,
    IN  GT_BOOL                             enable,
    IN  GT_U32                              timer
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktReasmPsmAgingSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE / GT_TRUE] 
                   and timer [0 / 0xFFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricPktReasmPsmAgingGet 
              with non-NULL enablePtr 
                   and non-NULL timerPtr.
    Expected: GT_OK and the same enable and timer.
    1.3. Call with enable [GT_TRUE] 
                   and out of range timer [0x3FFFFFF + 1].
    Expected: NOT GT_OK.
    1.4. Call with enable [GT_FALSE] 
                   and out of range timer [0x3FFFFFF + 1] (not relevant).
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_U32      timer     = 0;
    GT_BOOL     enableGet = GT_FALSE;
    GT_U32      timerGet  = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_FALSE / GT_TRUE] 
                           and timer [0 / 0xFFFFF].
            Expected: GT_OK.
        */
        /* call with enable = GT_FALSE */
        enable = GT_FALSE;
        timer  = 0;

        st = cpssExMxPmFabricPktReasmPsmAgingSet(dev, enable, timer);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, timer);
        
        /*
            1.2. Call cpssExMxPmFabricPktReasmPsmAgingGet 
                      with non-NULL enablePtr 
                           and non-NULL timerPtr.
            Expected: GT_OK and the same enable and timer.
        */
        st = cpssExMxPmFabricPktReasmPsmAgingGet(dev, &enableGet, &timerGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktReasmPsmAgingGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(timer, timerGet,
                   "get another timer then was set: %d", dev);

        /* Call with enable = GT_TRUE */
        enable = GT_TRUE;
        timer  = 0xFFFFF;

        st = cpssExMxPmFabricPktReasmPsmAgingSet(dev, enable, timer);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, timer);

        /*
            1.2. Call cpssExMxPmFabricPktReasmPsmAgingGet 
                      with non-NULL enablePtr 
                           and non-NULL timerPtr.
            Expected: GT_OK and the same enable and timer.
        */
        st = cpssExMxPmFabricPktReasmPsmAgingGet(dev, &enableGet, &timerGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmFabricPktReasmPsmAgingGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable then was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(timer, timerGet,
                   "get another timer then was set: %d", dev);

        /*
            1.3. Call with enable [GT_TRUE] 
                           and out of range timer [0x3FFFFFF + 1].
            Expected: NOT GT_OK.
        */
        enable = GT_TRUE;
        timer  = 0x3FFFFFF + 1;

        st = cpssExMxPmFabricPktReasmPsmAgingSet(dev, enable, timer);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, timer);

        /*
            1.4. Call with enable [GT_FALSE] 
                           and out of range timer [0x3FFFFFF + 1] (not relevant).
            Expected: GT_OK.
        */
        enable = GT_FALSE;
        timer  = 0x3FFFFFF + 1;

        st = cpssExMxPmFabricPktReasmPsmAgingSet(dev, enable, timer);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, timer);
    }

    enable = GT_FALSE;
    timer  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktReasmPsmAgingSet(dev, enable, timer);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktReasmPsmAgingSet(dev, enable, timer);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricPktReasmPsmAgingGet
(
    IN  GT_U8                               devNum,
    OUT GT_BOOL                             *enablePtr,
    OUT GT_U32                              *timerPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricPktReasmPsmAgingGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr 
                   and non-NULL timerPtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with timerPtr [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;
    GT_U32      timer  = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL enablePtr 
                           and non-NULL timerPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmFabricPktReasmPsmAgingGet(dev, &enable, &timer);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
                                               
        /*
            1.2. Call with enablePtr [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktReasmPsmAgingGet(dev, NULL, &timer);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);

        /*
            1.3. Call with timerPtr [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricPktReasmPsmAgingGet(dev, &enable, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, timerPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricPktReasmPsmAgingGet(dev, &enable, &timer);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricPktReasmPsmAgingGet(dev, &enable, &timer);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmFabricPktReasm suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmFabricPktReasm)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktReasmControlConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktReasmControlConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktReasmFifosConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktReasmFifosConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktReasmLastCellDropStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktReasmGlobalCellCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktReasmPerSetCellCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktReasmPsmAgingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricPktReasmPsmAgingGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmFabricPktReasm)

