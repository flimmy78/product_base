/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtCpuMailUT.c
*
* DESCRIPTION:
*       Unit tests for gtCpuMail, that provides
*       Prestera API implementations for FA and XBAR mail box utility interface.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpssXbar/generic/cpuMail/gtCpuMail.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define XBAR_CPU_MAIL_VALID_FPORT_CNS    0


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpuMailGetRxStats
(
    IN   GT_U8  devNum,
    OUT  GT_U8  *rxMailCellCounter,
    OUT  GT_U8  *rxMailCellDropCounter
)
*/
UTF_TEST_CASE_MAC(cpuMailGetRxStats)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with non-NULL rxMailCellCounter 
                   and non-NULL rxMailCellDropCounter.
    Expected: GT_OK.
    1.2. Call with rxMailCellCounter [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with rxMailCellDropCounter [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;

    GT_U8   rxMailCellCounter     = 0;
    GT_U8   rxMailCellDropCounter = 0;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL rxMailCellCounter 
                           and non-NULL rxMailCellDropCounter.
            Expected: GT_OK.
        */
        st = cpuMailGetRxStats(dev, &rxMailCellCounter, &rxMailCellDropCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        
        /*
            1.2. Call with rxMailCellCounter [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpuMailGetRxStats(dev, NULL, &rxMailCellDropCounter);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rxMailCellCounter = NULL", dev);

        /*
            1.3. Call with rxMailCellDropCounter [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpuMailGetRxStats(dev, &rxMailCellCounter, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, rxMailCellDropCounter = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = cpuMailGetRxStats(dev, &rxMailCellCounter, &rxMailCellDropCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpuMailGetRxStats(dev, &rxMailCellCounter, &rxMailCellDropCounter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpuMailSend
(
    IN  GT_U8   devNum,
    IN  GT_U8   targetFport,
    IN  GT_U8   cellData[GT_CPU_MAIL_CELL_SIZE],
    IN  GT_U8   cellDataLen
)
*/
UTF_TEST_CASE_MAC(cpuMailSend)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with cellData [0, 0, 0, 0 / 0xFF, …, 0xFF], 
                     cellDataLen [4 / GT_CPU_MAIL_CELL_SIZE].
    Expected: GT_OK.
    1.1.2. Call with out of range cellDataLen [0 /  GT_CPU_MAIL_CELL_SIZE + 1] 
                     and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.3. Call with cellData [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_CPU_MAIL_VALID_FPORT_CNS;

    GT_U8   cellData[GT_CPU_MAIL_CELL_SIZE];
    GT_U8   cellDataLen = 0;
    GT_U8   i           = 0;
    

    cpssOsBzero(cellData, sizeof(cellData[0])*GT_CPU_MAIL_CELL_SIZE);

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_TRUE))
        {
            /*
                1.1.1. Call with cellData [0, 0, 0, 0 / 0xFF, …, 0xFF], 
                                 cellDataLen [4 / GT_CPU_MAIL_CELL_SIZE].
                Expected: GT_OK.
            */
            /* Call with cellDataLen = 4 */
            cellData[0] = 0;
            cellData[1] = 0;
            cellData[2] = 0;
            cellData[3] = 0;
            cellDataLen = 4;

            st = cpuMailSend(dev, fport, cellData, cellDataLen);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, cellDataLen = %d", dev, fport, cellDataLen);
            
            /* Call with cellDataLen = GT_CPU_MAIL_CELL_SIZE */
            for (i = 0; i < GT_CPU_MAIL_CELL_SIZE; ++i) cellData[i] = 0xFF;
            cellDataLen = GT_CPU_MAIL_CELL_SIZE;

            st = cpuMailSend(dev, fport, cellData, cellDataLen);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, cellDataLen = %d", dev, fport, cellDataLen);

            /*
                1.1.2. Call with out of range cellDataLen [0 /  GT_CPU_MAIL_CELL_SIZE + 1] 
                                 and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            /* Call with cellDataLen = 0 */
            cellDataLen = 0;

            st = cpuMailSend(dev, fport, cellData, cellDataLen);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, cellDataLen = %d", dev, fport, cellDataLen);

            /* Call with cellDataLen = GT_CPU_MAIL_CELL_SIZE+1 */
            cellDataLen = GT_CPU_MAIL_CELL_SIZE+1;

            st = cpuMailSend(dev, fport, cellData, cellDataLen);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, cellDataLen = %d", dev, fport, cellDataLen);

            cellDataLen = GT_CPU_MAIL_CELL_SIZE;
    
            /*
                1.1.3. Call with cellData [NULL] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpuMailSend(dev, fport, NULL, cellDataLen);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, cellData = NULL", dev, fport);
        }

        cellData[0] = 0;
        cellData[1] = 0;
        cellData[2] = 0;
        cellData[3] = 0;
        cellDataLen = 4;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpuMailSend(dev, fport, cellData, cellDataLen);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = cpuMailSend(dev, fport, cellData, cellDataLen);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    cellData[0] = 0;
    cellData[1] = 0;
    cellData[2] = 0;
    cellData[3] = 0;
    cellDataLen = 4;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_CPU_MAIL_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = cpuMailSend(dev, fport, cellData, cellDataLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpuMailSend(dev, fport, cellData, cellDataLen);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS xbarCoreRxMailGet
(
    IN  GT_U8           xbarId,
    IN  GT_U8           fport,
    OUT GT_U8           dataRcv[MAX_CELL_SIZE],
    OUT GT_BOOL         *payloadCrcOk
)
*/
UTF_TEST_CASE_MAC(xbarCoreRxMailGet)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with non-NULL dataRcv 
                     and non-NULL payloadCrcOk.
    Expected: GT_OK.
    1.1.2. Call with dataRcv [NULL]
                     and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.1.3. Call with payloadCrcOk [NULL]
                     and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_CPU_MAIL_VALID_FPORT_CNS;

    GT_U8       dataRcv[MAX_CELL_SIZE];
    GT_BOOL     payloadCrcOk = GT_FALSE;


    cpssOsBzero(dataRcv, sizeof(dataRcv[0])*MAX_CELL_SIZE);
    
    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL dataRcv 
                                 and non-NULL payloadCrcOk.
                Expected: GT_OK.
            */
            st = xbarCoreRxMailGet(dev, fport, dataRcv, &payloadCrcOk);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

            /*
                1.1.2. Call with dataRcv [NULL]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = xbarCoreRxMailGet(dev, fport, NULL, &payloadCrcOk);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, dataRcv = NULL", dev, fport);

            /*
                1.1.3. Call with payloadCrcOk [NULL]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = xbarCoreRxMailGet(dev, fport, dataRcv, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, payloadCrcOk = NULL", dev, fport);
        }

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = xbarCoreRxMailGet(dev, fport, dataRcv, &payloadCrcOk);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = xbarCoreRxMailGet(dev, fport, dataRcv, &payloadCrcOk);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = xbarCoreRxMailGet(dev, fport, dataRcv, &payloadCrcOk);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = xbarCoreRxMailGet(dev, fport, dataRcv, &payloadCrcOk);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of gtCpuMail suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(gtCpuMail)
    UTF_SUIT_DECLARE_TEST_MAC(cpuMailGetRxStats)
    UTF_SUIT_DECLARE_TEST_MAC(cpuMailSend)
    UTF_SUIT_DECLARE_TEST_MAC(xbarCoreRxMailGet)
UTF_SUIT_END_TESTS_MAC(gtCpuMail)

