/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtXbarUT.c
*
* DESCRIPTION:
*       Unit tests for gtXbar, that provides
*       Prestera API definitions for Fabric stacking XBAR interface.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpssXbar/generic/xbarControl/gtXbar.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define XBAR_XBAR_INVALID_ENUM_CNS  0x5AAAAAA5

#define XBAR_XBAR_VALID_FPORT_CNS   0

/* Externals */
extern GT_XBAR_SYS_CONFIG gtXbarSysConfig;


/*----------------------------------------------------------------------------*/
/*
GT_STATUS  gtGetNumXbarFport
(
    IN  GT_U8   xbarDevNum,
    OUT GT_U8   *numFports,
    OUT GT_U8   fportIndexes[PRV_CPSS_MAX_XBAR_FPORTS_NUM_CNS],
    OUT GT_BOOL *isFabricAdapter,
    OUT GT_U8   *faFportIndex
)
*/
UTF_TEST_CASE_MAC(gtGetNumXbarFport)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with non-NULL numFports, 
                   non-NULL fportIndexes, 
                   non-NULL isFabricAdapter, 
                   non-NULL faFportIndex.
    Expected: GT_OK.
    1.2. Call with numFports [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with fportIndexes [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with isFabricAdapter [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with faFportIndex [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;

    GT_U8   numFports       = 0;
    GT_U8   fportIndexes[PRV_CPSS_MAX_XBAR_FPORTS_NUM_CNS];
    GT_BOOL isFabricAdapter = GT_FALSE;
    GT_U8   faFportIndex    = 0;

    cpssOsBzero((GT_VOID*) &fportIndexes, sizeof(fportIndexes[0])*PRV_CPSS_MAX_XBAR_FPORTS_NUM_CNS);


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL numFports, 
                           non-NULL fportIndexes, 
                           non-NULL isFabricAdapter, 
                           non-NULL faFportIndex.
            Expected: GT_OK.
        */
        st = gtGetNumXbarFport(dev, &numFports, fportIndexes, &isFabricAdapter, &faFportIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with numFports [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = gtGetNumXbarFport(dev, NULL, fportIndexes, &isFabricAdapter, &faFportIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numFports = NULL", dev);

        /*
            1.3. Call with fportIndexes [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = gtGetNumXbarFport(dev, &numFports, NULL, &isFabricAdapter, &faFportIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fportIndexes = NULL", dev);

        /*
            1.4. Call with isFabricAdapter [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = gtGetNumXbarFport(dev, &numFports, fportIndexes, NULL, &faFportIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, isFabricAdapter = NULL", dev);

        /*
            1.5. Call with faFportIndex [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = gtGetNumXbarFport(dev, &numFports, fportIndexes, &isFabricAdapter, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, faFportIndex = NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
        
    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtGetNumXbarFport(dev, &numFports, fportIndexes, &isFabricAdapter, &faFportIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtGetNumXbarFport(dev, &numFports, fportIndexes, &isFabricAdapter, &faFportIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarFportEnable
(
    IN GT_U8   xbarDevNum,
    IN GT_U8   fport,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(gtXbarFportEnable)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_XBAR_VALID_FPORT_CNS;

    GT_BOOL     enable = GT_FALSE;


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
                1.1.1. Call with enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* Call with enable = GT_FALSE */
            enable = GT_FALSE;

            st = gtXbarFportEnable(dev, fport, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, enable);

            /* Call with enable = GT_TRUE */
            enable = GT_TRUE;

            st = gtXbarFportEnable(dev, fport, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, enable);
        }

        enable = GT_FALSE;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtXbarFportEnable(dev, fport, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtXbarFportEnable(dev, fport, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    enable = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_XBAR_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarFportEnable(dev, fport, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarFportEnable(dev, fport, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarEnableAllFports
(
    IN GT_U8   xbarDevNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(gtXbarEnableAllFports)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;

    GT_BOOL enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* Call with enable = GT_FALSE */
        enable = GT_FALSE;

        st = gtXbarEnableAllFports(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* Call with enable = GT_TRUE */
        enable = GT_TRUE;

        st = gtXbarEnableAllFports(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    enable = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
 
    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarEnableAllFports(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarEnableAllFports(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarSetMacTxXonXoffThresholds
(
    IN  GT_U8        xbarDevNum,
    IN  GT_U8        fport,
    IN  GT_U32       xonThreshold,
    IN  GT_U32       xoffThreshold
)
*/
UTF_TEST_CASE_MAC(gtXbarSetMacTxXonXoffThresholds)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with xonThreshold [0 / 0x1F], 
                     xoffThreshold [0 / 0x1F].
    Expected: GT_OK.
    1.1.2. Call with out of range xonThreshold [32] 
                     and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.3. Call with out of range xoffThreshold [32] 
                     and other parameters from 1.1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_XBAR_VALID_FPORT_CNS;

    GT_U32      xonThreshold  = 0;
    GT_U32      xoffThreshold = 0;


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
                1.1.1. Call with xonThreshold [0 / 0x1F], 
                                 xoffThreshold [0 / 0x1F].
                Expected: GT_OK.
            */
            /* Call with xonThreshold = 0 */
            xonThreshold  = 0;
            xoffThreshold = 0;

            st = gtXbarSetMacTxXonXoffThresholds(dev, fport, xonThreshold, xoffThreshold);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, xonThreshold, xoffThreshold);

            /* Call with xonThreshold = 0x1F */
            xonThreshold  = 0x1F;
            xoffThreshold = 0x1F;

            st = gtXbarSetMacTxXonXoffThresholds(dev, fport, xonThreshold, xoffThreshold);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, xonThreshold, xoffThreshold);

            /*
                1.1.2. Call with out of range xonThreshold [32] 
                                 and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            xonThreshold = 32;

            st = gtXbarSetMacTxXonXoffThresholds(dev, fport, xonThreshold, xoffThreshold);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, xonThreshold);

            xonThreshold = 0x1F;

            /*
                1.1.3. Call with out of range xoffThreshold [32] 
                                 and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            xoffThreshold = 32;

            st = gtXbarSetMacTxXonXoffThresholds(dev, fport, xonThreshold, xoffThreshold);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, xoffThreshold = %d", dev, fport, xoffThreshold);
        }

        xonThreshold  = 0;
        xoffThreshold = 0;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtXbarSetMacTxXonXoffThresholds(dev, fport, xonThreshold, xoffThreshold);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtXbarSetMacTxXonXoffThresholds(dev, fport, xonThreshold, xoffThreshold);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    xonThreshold  = 0;
    xoffThreshold = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_XBAR_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarSetMacTxXonXoffThresholds(dev, fport, xonThreshold, xoffThreshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarSetMacTxXonXoffThresholds(dev, fport, xonThreshold, xoffThreshold);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarSetBcCellTarget
(
    IN GT_U8 xbarDevNum,
    IN GT_U8 inFport,
    IN GT_U8 outFport[],
    IN GT_U8 outFportListLen
)
*/
UTF_TEST_CASE_MAC(gtXbarSetBcCellTarget)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with outFport [0, 0, 0 / 2, …, 2], 
                     OUTFportListLen [3 / 16].
    Expected: GT_OK.
    1.1.2. Call with out of range outFport[0] [maxFportNum(dev)+1], 
                     outFportListLen [1] 
                     and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.3. Call with out of range out FportListLen [0] 
                     and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.4. Call with outFport [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_XBAR_VALID_FPORT_CNS;

    GT_U8   outFport[16];
    GT_U8   outFportListLen = 0;
    GT_U8   i               = 0;


    cpssOsBzero((GT_VOID*) &outFport, sizeof(outFport[0])*16);

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
                1.1.1. Call with outFport [0, 0, 0 / 2, …, 2], 
                                 OUTFportListLen [3 / 16].
                Expected: GT_OK.
            */
            /* Call with outFportListLen = 3 */
            for(i = 0; i < 3; ++i) outFport[i] = 0;
            outFportListLen = 3;

            st = gtXbarSetBcCellTarget(dev, fport, outFport, outFportListLen);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, outFportListLen = %d", dev, fport, outFportListLen);

            /* Call with outFportListLen = 16 */
            for(i = 0; i < 16; ++i) outFport[i] = 2;
            outFportListLen = 16;

            st = gtXbarSetBcCellTarget(dev, fport, outFport, outFportListLen);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, outFportListLen = %d", dev, fport, outFportListLen);

            /*
                1.1.2. Call with out of range outFport[0] [maxFportNum(dev)+1], 
                                 outFportListLen [1] 
                                 and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            outFport[0]     = UTF_GET_MAX_FPORTS_NUM_MAC(dev) + 1;
            outFportListLen = 1;

            st = gtXbarSetBcCellTarget(dev, fport, outFport, outFportListLen);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, outFport[0] = %d", dev, fport, outFport[0]);

            outFport[0] = 0;

            /*
                1.1.3. Call with out of range out FportListLen [0] 
                                 and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            outFportListLen = 0;

            st = gtXbarSetBcCellTarget(dev, fport, outFport, outFportListLen);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, outFportListLen = %d", dev, fport, outFportListLen);

            outFportListLen = 1;

            /*
                1.1.4. Call with outFport [NULL] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtXbarSetBcCellTarget(dev, fport, NULL, outFportListLen);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, outFport = NULL", dev, fport);
        }

        for(i = 0; i < 3; ++i) outFport[i] = 0;
        outFportListLen = 3;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtXbarSetBcCellTarget(dev, fport, outFport, outFportListLen);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtXbarSetBcCellTarget(dev, fport, outFport, outFportListLen);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    for(i = 0; i < 3; ++i) outFport[i] = 0;
    outFportListLen = 3;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_XBAR_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarSetBcCellTarget(dev, fport, outFport, outFportListLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarSetBcCellTarget(dev, fport, outFport, outFportListLen);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarSetUcRoute
(
    IN GT_U8 xbarDevNum,
    IN GT_U8 inFport,
    IN GT_U8 outFport,
    IN GT_U8 tarDev,
    IN GT_U8 lbh
)
*/
UTF_TEST_CASE_MAC(gtXbarSetUcRoute)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Set LBH mode. Call function with outFport [0 / 2], 
                                       tarDev [0 / xbarDevNum], 
                                       lbh [0 / 3].
    Expected: GT_OK.
    1.1.2. Call with out of range outFport [maxFportNum(dev)+1] 
                     and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.3. Call with out of range tarDev [PRV_CPSS_MAX_PP_DEVICES_CNS] 
                     and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.4. Set LBH mode. Call with out of range tarDev [64] 
                                   and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.5. Disable LBH mode. Call with tarDev [64] 
                                       and other parameters from 1.1.1.
    Expected: GT_OK.
    1.1.6. Call with out of range lbh[4] 
                     and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.7. Disable LBH mode. Call with lbh[3] (must be 0) 
                                  and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.3. Call gtXbarClearUcRoute with the same xbarDevNum 
                                        and inFport from 1.1.1. to clear crossbar unicast routing table.
    Expected: GT_OK.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_XBAR_VALID_FPORT_CNS;

    GT_U8   outFport = 0;
    GT_U8   tarDev   = 0;
    GT_U8   lbh      = 0;


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
                1.1.1. Set LBH mode. Call function with outFport [0 / 2], 
                                                   tarDev [0 / xbarDevNum], 
                                                   lbh [0 / 3].
                Expected: GT_OK.
            */
            /* Call with outFport = 0 */
            gtXbarSysConfig.lbhMode = GT_FALSE;

            outFport = 0;
            tarDev   = 0;
            lbh      = 0;

            st = gtXbarSetUcRoute(dev, fport, outFport, tarDev, lbh);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, fport, outFport, tarDev, lbh);

            /* Call with outFport = 2 */
            gtXbarSysConfig.lbhMode = GT_TRUE;

            outFport = 2;
            tarDev   = dev;
            lbh      = 3;

            st = gtXbarSetUcRoute(dev, fport, outFport, tarDev, lbh);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, fport, outFport, tarDev, lbh);

            /*
                1.1.2. Call with out of range outFport [maxFportNum(dev)+1] 
                                 and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            outFport = UTF_GET_MAX_FPORTS_NUM_MAC(dev) + 1;

            st = gtXbarSetUcRoute(dev, fport, outFport, tarDev, lbh);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, outFport);

            outFport = XBAR_XBAR_VALID_FPORT_CNS;

            /*
                1.1.3. Call with out of range tarDev [PRV_CPSS_MAX_PP_DEVICES_CNS] 
                                 and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            tarDev = PRV_CPSS_MAX_PP_DEVICES_CNS;

            st = gtXbarSetUcRoute(dev, fport, outFport, tarDev, lbh);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, tarDev = %d", dev, fport, tarDev);

            tarDev = dev;

            /*
                1.1.4. Set LBH mode. Call with out of range tarDev [64] 
                                               and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            gtXbarSysConfig.lbhMode = GT_TRUE;

            tarDev = 64;

            st = gtXbarSetUcRoute(dev, fport, outFport, tarDev, lbh);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, tarDev = %d, LBH mode enabled",
                                             dev, fport, tarDev);

            tarDev = dev;

            /*
                1.1.5. Disable LBH mode. Call with tarDev [64] 
                                                   and other parameters from 1.1.1.
                Expected: GT_OK.
            */
            gtXbarSysConfig.lbhMode = GT_FALSE;

            tarDev = 64;

            st = gtXbarSetUcRoute(dev, fport, outFport, tarDev, lbh);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, tarDev = %d. LBH mode disabled",
                                         dev, fport, tarDev);

            tarDev = dev;

            /*
                1.1.6. Call with out of range lbh[4] 
                                 and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            gtXbarSysConfig.lbhMode = GT_TRUE;

            lbh = 4;

            st = gtXbarSetUcRoute(dev, fport, outFport, tarDev, lbh);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, lbh = %d", dev, fport, lbh);

            lbh = 3;

            /*
                1.1.7. Disable LBH mode. Call with lbh[3] (must be 0) 
                                              and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            gtXbarSysConfig.lbhMode = GT_FALSE;

            lbh = 3;

            st = gtXbarSetUcRoute(dev, fport, outFport, tarDev, lbh);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, lbh = %d. LBH mode disabled",
                                             dev, fport, lbh);

            lbh = 0;

            /*
                1.1.3. Call gtXbarClearUcRoute with the same xbarDevNum 
                                                    and inFport from 1.1.1. to clear crossbar unicast routing table.
                Expected: GT_OK.
            */
            st = gtXbarClearUcRoute(dev, fport);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "gtXbarClearUcRoute: %d, %d", dev, fport);
        }

        gtXbarSysConfig.lbhMode = GT_FALSE;

        outFport = 0;
        tarDev   = 0;
        lbh      = 0;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtXbarSetUcRoute(dev, fport, outFport, tarDev, lbh);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtXbarSetUcRoute(dev, fport, outFport, tarDev, lbh);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    gtXbarSysConfig.lbhMode = GT_FALSE;

    outFport = 0;
    tarDev   = 0;
    lbh      = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_XBAR_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarSetUcRoute(dev, fport, outFport, tarDev, lbh);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarSetUcRoute(dev, fport, outFport, tarDev, lbh);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarClearUcRoute
(
    IN GT_U8 xbarDevNum,
    IN GT_U8 inFport
)
*/
UTF_TEST_CASE_MAC(gtXbarClearUcRoute)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_XBAR_VALID_FPORT_CNS;


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
            st = gtXbarClearUcRoute(dev, fport);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
        }

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtXbarClearUcRoute(dev, fport);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtXbarClearUcRoute(dev, fport);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_XBAR_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarClearUcRoute(dev, fport);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarClearUcRoute(dev, fport);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarSetMirrorRoute
(
    IN GT_U8        xbarDevNum,
    IN GT_U8        outFport,
    IN GT_U8        tarDev
)
*/
UTF_TEST_CASE_MAC(gtXbarSetMirrorRoute)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with tarDev[0 / xbarDevNum]
    Expected: GT_OK.
    1.1.2. Call with out of range tarDev [PRV_CPSS_MAX_PP_DEVICES_CNS].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_XBAR_VALID_FPORT_CNS;

    GT_U8       tarDev = 0;


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
                1.1.1. Call with tarDev[0 / xbarDevNum]
                Expected: GT_OK.
            */
            /* Call with tarDev = 0 */
            tarDev = 0;

            st = gtXbarSetMirrorRoute(dev, fport, tarDev);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, tarDev);

            /* Call with tarDev = dev */
            tarDev = dev;

            st = gtXbarSetMirrorRoute(dev, fport, tarDev);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, tarDev);

            /*
                1.1.2. Call with out of range tarDev [PRV_CPSS_MAX_PP_DEVICES_CNS].
                Expected: NOT GT_OK.
            */
            tarDev = PRV_CPSS_MAX_PP_DEVICES_CNS;

            st = gtXbarSetMirrorRoute(dev, fport, tarDev);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, tarDev);
        }

        tarDev = 0;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtXbarSetMirrorRoute(dev, fport, tarDev);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtXbarSetMirrorRoute(dev, fport, tarDev);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    tarDev = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_XBAR_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarSetMirrorRoute(dev, fport, tarDev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarSetMirrorRoute(dev, fport, tarDev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarSetMcRoute
(
    IN GT_U8  xbarDevNum,
    IN GT_U16 mcId,
    IN GT_U8  mcFport[],
    IN GT_U8  mcFportListLen
)
*/
UTF_TEST_CASE_MAC(gtXbarSetMcRoute)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with mcId [0 / 4095], 
                   mcFport [maxFportNum(dev) / 0, 1, 2], 
                   mcFportListLen [1 / 3].
    Expected: GT_OK.
    1.2. Call with out of range mcId [4096] 
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range mcFportListLen [0] 
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range mcFport[0] [maxFportNum(dev)+1],
                   mcFportListLen [1] and other parameters from 1.1.
    Expected: NOT GT_OK.                                                
    1.5. Call with mcFport [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call gtXbarClearMcRoute with the same xbarDevNum from 1.1. 
                        to clears crossbar multicast routing table.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;

    GT_U16 mcId           = 0;
    GT_U8  mcFport[3];
    GT_U8  mcFportListLen = 0;


    cpssOsBzero((GT_VOID*) &mcFport, sizeof(mcFport[0])*3);
    
    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mcId [0 / 4095], 
                           mcFport [maxFportNum(dev) / 0, 1, 2], 
                           mcFportListLen [1 / 3].
            Expected: GT_OK.
        */
        /* Call with mcId = 0 */
        mcId           = 0;
        mcFport[0]     = UTF_GET_MAX_FPORTS_NUM_MAC(dev);
        mcFportListLen = 1;

        st = gtXbarSetMcRoute(dev, mcId, mcFport, mcFportListLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, mcFportListLen = %d", dev, mcId, mcFportListLen);

        /* Call with mcId = 4095 */
        mcId = 4095;

        mcFport[0] = 0;
        mcFport[1] = 1;
        mcFport[2] = 2;

        mcFportListLen = 3;

        st = gtXbarSetMcRoute(dev, mcId, mcFport, mcFportListLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, mcFportListLen = %d", dev, mcId, mcFportListLen);

        /*
            1.2. Call with out of range mcId [4096] 
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        mcId = 4096;

        st = gtXbarSetMcRoute(dev, mcId, mcFport, mcFportListLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, mcId);

        mcId = 4095;

        /*
            1.3. Call with out of range mcFportListLen [0] 
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        mcFportListLen = 0;

        st = gtXbarSetMcRoute(dev, mcId, mcFport, mcFportListLen);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, mcFportListLen = %d", dev, mcFportListLen);

        mcFportListLen = 3;

        /*
            1.4. Call with out of range mcFport[0] [maxFportNum(dev)+1],
                           mcFportListLen [1] and other parameters from 1.1.
            Expected: NOT GT_OK.                                                
        */
        mcFport[0]     = UTF_GET_MAX_FPORTS_NUM_MAC(dev) + 1;
        mcFportListLen = 1;

        st = gtXbarSetMcRoute(dev, mcId, mcFport, mcFportListLen);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, mcFport[0] = %d", dev, mcFport[0]);

        mcFport[0] = 0;

        /*
            1.5. Call with mcFport [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = gtXbarSetMcRoute(dev, mcId, NULL, mcFportListLen);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mcFport = NULL", dev);

        /*
            1.6. Call gtXbarClearMcRoute with the same xbarDevNum from 1.1. 
                                to clears crossbar multicast routing table.
            Expected: GT_OK.
        */
        st = gtXbarClearMcRoute(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtXbarClearMcRoute: %d", dev);
    }

    mcId           = 0;
    mcFport[0]     = UTF_GET_MAX_FPORTS_NUM_MAC(dev);
    mcFportListLen = 1;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarSetMcRoute(dev, mcId, mcFport, mcFportListLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarSetMcRoute(dev, mcId, mcFport, mcFportListLen);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarClearMcRoute
(
    IN GT_U8  xbarDevNum
)
*/
UTF_TEST_CASE_MAC(gtXbarClearMcRoute)
{
/*
    ITERATE_XBAR_DEVICES
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;
    
    
    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = gtXbarClearMcRoute(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarClearMcRoute(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarClearMcRoute(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarLocalSwitchingEn
(
    IN GT_U8   xbarDevNum,
    IN GT_U8   fport,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(gtXbarLocalSwitchingEn)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.    
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_XBAR_VALID_FPORT_CNS;

    GT_BOOL     enable = GT_FALSE;


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
                1.1.1. Call with enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.    
            */
            /* Call with enable = GT_FALSE */
            enable = GT_FALSE;

            st = gtXbarLocalSwitchingEn(dev, fport, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, enable);

            /* Call with enable = GT_TRUE */
            enable = GT_TRUE;

            st = gtXbarLocalSwitchingEn(dev, fport, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, enable);
        }

        enable = GT_FALSE;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtXbarLocalSwitchingEn(dev, fport, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtXbarLocalSwitchingEn(dev, fport, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    enable = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_XBAR_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarLocalSwitchingEn(dev, fport, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarLocalSwitchingEn(dev, fport, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarGetRxCounters
(
    IN  GT_U8    xbarDevNum,
    IN  GT_U8    fport,
    OUT GT_U32   *rxDiagCellCount,
    OUT GT_U32   *rxInvalidTargetDropCount
)
*/
UTF_TEST_CASE_MAC(gtXbarGetRxCounters)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with non-NULL rxDiagCellCount 
                     and non-NULL rxInvalidTargetDropCount.
    Expected: GT_OK.
    1.1.2. Call with rxDiagCellCount [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
    1.1.3. Call with rxInvalidTargetDropCount [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_XBAR_VALID_FPORT_CNS;

    GT_U32      rxDiagCellCount          = 0;
    GT_U32      rxInvalidTargetDropCount = 0;


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
                1.1.1. Call with non-NULL rxDiagCellCount 
                                 and non-NULL rxInvalidTargetDropCount.
                Expected: GT_OK.
            */
            st = gtXbarGetRxCounters(dev, fport, &rxDiagCellCount, &rxInvalidTargetDropCount);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

            /*
                1.1.2. Call with rxDiagCellCount [NULL]
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtXbarGetRxCounters(dev, fport, NULL, &rxInvalidTargetDropCount);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, rxDiagCellCount = NULL", dev, fport);

            /*
                1.1.3. Call with rxInvalidTargetDropCount [NULL] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtXbarGetRxCounters(dev, fport, &rxDiagCellCount, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, rxInvalidTargetDropCount = NULL", dev, fport);
        }

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtXbarGetRxCounters(dev, fport, &rxDiagCellCount, &rxInvalidTargetDropCount);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtXbarGetRxCounters(dev, fport, &rxDiagCellCount, &rxInvalidTargetDropCount);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_XBAR_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarGetRxCounters(dev, fport, &rxDiagCellCount, &rxInvalidTargetDropCount);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarGetRxCounters(dev, fport, &rxDiagCellCount, &rxInvalidTargetDropCount);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarGportCounterUpdate
(
    IN  GT_U8           xbarId,
    IN  GT_U8           fport
)
*/
UTF_TEST_CASE_MAC(gtXbarGportCounterUpdate)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_XBAR_VALID_FPORT_CNS;


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
            st = gtXbarGportCounterUpdate(dev, fport);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);
        }

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtXbarGportCounterUpdate(dev, fport);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtXbarGportCounterUpdate(dev, fport);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_XBAR_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarGportCounterUpdate(dev, fport);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarGportCounterUpdate(dev, fport);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarGportClearInt
(
    IN  GT_U8               xbarId,
    IN  GT_XBAR_INT_CAUSE   *intCause
)
*/
UTF_TEST_CASE_MAC(gtXbarGportClearInt)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with intCause {intType [GT_XBAR_FOX_INT_TYPE], 
                             intCause.xbarFoxIntCause [GT_XBAR_FOX_GEN_RES_0 / 
                                                       GT_XBAR_FOX_GPP_RES_0 / 
                                                       GT_XBAR_FOX_TXD_RES_0 / 
                                                       GT_FOX_G4_P1_TX_FIFO_UPD_RES_31]}.
    Expected: GT_OK.
    1.2. Call with intCause {intType [GT_XBAR_CAP_INT_TYPE], 
                             intCause. xbarCapIntCause [GT_CAP_GLB_SUM_RES_0 / 
                                                        GT_CAP_GLB_PORT_SUM_RES_0 / 
                                                        GT_CAP_PORT_GRP_SUM_RES_0 / 
                                                        GT_CAP_HG11_PRBS_GEN_RES_31] }.
    Expected: GT_OK.
    1.3. Call with out of range intCause->intType [0x5AAAAAA5] 
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with intCause->intType [GT_XBAR_FOX_INT_TYPE], 
                   out of range intCause->xbarFoxIntCause[0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
    1.5. Call with intCause->intType [GT_XBAR_FOX_INT_TYPE], 
                   out of range intCause->xbarCapIntCause[0x5AAAAAA5] (not relevant).
    Expected: GT_OK.
    1.6. Call with out of range intCause->intType [GT_XBAR_CAP_INT_TYPE], 
                   eventType->xbarCapIntCause[0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
    1.7. Call with out of range intCause->intType [GT_XBAR_CAP_INT_TYPE], 
                   eventType->xbarFoxIntCause[0x5AAAAAA5] (not relevant).
    Expected: GT_OK.
    1.8. Call with intCause [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;

    GT_XBAR_INT_CAUSE   intCause;

    cpssOsBzero((GT_VOID*) &intCause, sizeof(intCause));


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with intCause {intType [GT_XBAR_FOX_INT_TYPE], 
                                     intCause.xbarFoxIntCause [GT_XBAR_FOX_GEN_RES_0 / 
                                                               GT_XBAR_FOX_GPP_RES_0 / 
                                                               GT_XBAR_FOX_TXD_RES_0 / 
                                                               GT_FOX_G4_P1_TX_FIFO_UPD_RES_31]}.
            Expected: GT_OK.
        */
        /* Call with intCause.intCause.xbarFoxIntCause = GT_XBAR_FOX_GEN_RES_0 */
        intCause.intType                  = GT_XBAR_FOX_INT_TYPE;
        intCause.intCause.xbarFoxIntCause = GT_XBAR_FOX_GEN_RES_0;

        st = gtXbarGportClearInt(dev, &intCause);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with intCause.intCause.xbarFoxIntCause = GT_XBAR_FOX_GPP_RES_0 */
        intCause.intCause.xbarFoxIntCause = GT_XBAR_FOX_GPP_RES_0;

        st = gtXbarGportClearInt(dev, &intCause);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with intCause.intCause.xbarFoxIntCause = GT_XBAR_FOX_TXD_RES_0 */
        intCause.intCause.xbarFoxIntCause = GT_XBAR_FOX_TXD_RES_0;

        st = gtXbarGportClearInt(dev, &intCause);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with intCause.intCause.xbarFoxIntCause = GT_FOX_G4_P1_TX_FIFO_UPD_RES_31 */
        intCause.intCause.xbarFoxIntCause = GT_FOX_G4_P1_TX_FIFO_UPD_RES_31;

        st = gtXbarGportClearInt(dev, &intCause);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with intCause {intType [GT_XBAR_CAP_INT_TYPE], 
                                     intCause. xbarCapIntCause [GT_CAP_GLB_SUM_RES_0 / 
                                                                GT_CAP_GLB_PORT_SUM_RES_0 / 
                                                                GT_CAP_PORT_GRP_SUM_RES_0 / 
                                                                GT_CAP_HG11_PRBS_GEN_RES_31] }.
            Expected: GT_OK.
        */
        /* Call with intCause.intCause.xbarCapIntCause = GT_CAP_GLB_SUM_RES_0 */
        intCause.intType                  = GT_XBAR_CAP_INT_TYPE;
        intCause.intCause.xbarCapIntCause = GT_CAP_GLB_SUM_RES_0;

        st = gtXbarGportClearInt(dev, &intCause);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with intCause.intCause.xbarCapIntCause = GT_CAP_GLB_PORT_SUM_RES_0 */
        intCause.intCause.xbarCapIntCause = GT_CAP_GLB_PORT_SUM_RES_0;

        st = gtXbarGportClearInt(dev, &intCause);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with intCause.intCause.xbarCapIntCause = GT_CAP_PORT_GRP_SUM_RES_0 */
        intCause.intCause.xbarCapIntCause = GT_CAP_PORT_GRP_SUM_RES_0;

        st = gtXbarGportClearInt(dev, &intCause);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with intCause.intCause.xbarCapIntCause = GT_CAP_HG11_PRBS_GEN_RES_31 */
        intCause.intCause.xbarCapIntCause = GT_CAP_HG11_PRBS_GEN_RES_31;

        st = gtXbarGportClearInt(dev, &intCause);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.3. Call with out of range intCause->intType [0x5AAAAAA5] 
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        intCause.intType = XBAR_XBAR_INVALID_ENUM_CNS;

        st = gtXbarGportClearInt(dev, &intCause);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, intCause->intType = %d", dev, intCause.intType);

        intCause.intType = GT_XBAR_CAP_INT_TYPE;

        /*
            1.4. Call with intCause->intType [GT_XBAR_FOX_INT_TYPE], 
                           out of range intCause->xbarFoxIntCause[0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        intCause.intType                  = GT_XBAR_FOX_INT_TYPE;
        intCause.intCause.xbarFoxIntCause = XBAR_XBAR_INVALID_ENUM_CNS;

        st = gtXbarGportClearInt(dev, &intCause);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, intCause->intType = %d, intCause->xbarFoxIntCause = %d",
                                     dev, intCause.intType, intCause.intCause.xbarFoxIntCause);

        intCause.intCause.xbarFoxIntCause = GT_FOX_G4_P1_TX_FIFO_UPD_RES_31;

        /*
            1.5. Call with intCause->intType [GT_XBAR_FOX_INT_TYPE], 
                           out of range intCause->xbarCapIntCause[0x5AAAAAA5] (not relevant).
            Expected: GT_OK.
        */
        intCause.intType                  = GT_XBAR_FOX_INT_TYPE;
        intCause.intCause.xbarCapIntCause = XBAR_XBAR_INVALID_ENUM_CNS;

        st = gtXbarGportClearInt(dev, &intCause);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, intCause->intType = %d, intCause->xbarCapIntCause = %d",
                                     dev, intCause.intType, intCause.intCause.xbarCapIntCause);

        intCause.intType                  = GT_XBAR_CAP_INT_TYPE;
        intCause.intCause.xbarCapIntCause = GT_CAP_HG11_PRBS_GEN_RES_31;

        /*
            1.6. Call with out of range intCause->intType [GT_XBAR_CAP_INT_TYPE], 
                           eventType->xbarCapIntCause[0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        intCause.intType                  = GT_XBAR_CAP_INT_TYPE;
        intCause.intCause.xbarCapIntCause = XBAR_XBAR_INVALID_ENUM_CNS;

        st = gtXbarGportClearInt(dev, &intCause);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, intCause->intType = %d, intCause->xbarCapIntCause = %d",
                                     dev, intCause.intType, intCause.intCause.xbarCapIntCause);

        intCause.intCause.xbarCapIntCause = GT_CAP_HG11_PRBS_GEN_RES_31;

        /*
            1.7. Call with out of range intCause->intType [GT_XBAR_CAP_INT_TYPE], 
                           eventType->xbarFoxIntCause[0x5AAAAAA5] (not relevant).
            Expected: GT_OK.
        */
        intCause.intType                  = GT_XBAR_CAP_INT_TYPE;
        intCause.intCause.xbarFoxIntCause = XBAR_XBAR_INVALID_ENUM_CNS;

        st = gtXbarGportClearInt(dev, &intCause);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, intCause->intType = %d, intCause->xbarFoxIntCause = %d",
                                     dev, intCause.intType, intCause.intCause.xbarFoxIntCause);

        intCause.intType                  = GT_XBAR_FOX_INT_TYPE;
        intCause.intCause.xbarFoxIntCause = GT_FOX_G4_P1_TX_FIFO_UPD_RES_31;

        /*
            1.8. Call with intCause [NULL].
            Expected: GT_BAD_PTR.
        */
        st = gtXbarGportClearInt(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, intCause = NULL", dev);
    }

    intCause.intType                  = GT_XBAR_FOX_INT_TYPE;
    intCause.intCause.xbarFoxIntCause = GT_XBAR_FOX_GEN_RES_0;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarGportClearInt(dev, &intCause);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarGportClearInt(dev, &intCause);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarGetCellDropCounter
(
    IN   GT_U8             xbarDevNum,
    IN   GT_U8             inFport,
    IN   GT_U8             outFport,
    IN   GT_XBAR_CELL_TYPE cellType,
    OUT  GT_U8            *cellDropCount
)
*/
UTF_TEST_CASE_MAC(gtXbarGetCellDropCounter)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with outFport [0 / 2], 
                     cellType [GT_XBAR_NORMAL_CELLS / GT_XBAR_CONTROL_CELLS] 
                     and non-NULL cellDropCount.
    Expected: GT_OK.
    1.1.2. Call with out of range outFport [maxFportNum(dev)+1] 
                     and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.1.3. Call with out of range cellType [0x5AAAAAA5] 
                     and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.4. Call with cellDropCount [NULL] 
                     and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/ 
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_XBAR_VALID_FPORT_CNS;

    GT_U8             outFport      = 0;
    GT_XBAR_CELL_TYPE cellType      = GT_XBAR_NORMAL_CELLS;
    GT_U8             cellDropCount = 0;


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
                1.1.1. Call with outFport [0 / 2], 
                                 cellType [GT_XBAR_NORMAL_CELLS / GT_XBAR_CONTROL_CELLS] 
                                 and non-NULL cellDropCount.
                Expected: GT_OK.
            */
            /* Call with outFport = 0 */
            outFport = 0;
            cellType = GT_XBAR_NORMAL_CELLS;

            st = gtXbarGetCellDropCounter(dev, fport, outFport, cellType, &cellDropCount);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, outFport, cellType);

            /* Call with outFport = 2 */
            outFport = 2;
            cellType = GT_XBAR_CONTROL_CELLS;

            st = gtXbarGetCellDropCounter(dev, fport, outFport, cellType, &cellDropCount);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, outFport, cellType);

            /*
                1.1.2. Call with out of range outFport [maxFportNum(dev)+1] 
                                 and other parameters from 1.1.
                Expected: NOT GT_OK.
            */
            outFport = UTF_GET_MAX_FPORTS_NUM_MAC(dev) + 1;
            
            st = gtXbarGetCellDropCounter(dev, fport, outFport, cellType, &cellDropCount);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, outFport);

            outFport = 2;

            /*
                1.1.3. Call with out of range cellType [0x5AAAAAA5] 
                                 and other parameters from 1.1.
                Expected: GT_BAD_PARAM.
            */
            cellType = XBAR_XBAR_INVALID_ENUM_CNS;
            
            st = gtXbarGetCellDropCounter(dev, fport, outFport, cellType, &cellDropCount);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, cellType = %d", dev, fport, cellType);

            cellType = GT_XBAR_CONTROL_CELLS;

            /*
                1.1.4. Call with cellDropCount [NULL]
                                 and other parameters from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtXbarGetCellDropCounter(dev, fport, outFport, cellType, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, cellDropCount = NULL", dev, fport);
        }

        outFport = 0;
        cellType = GT_XBAR_NORMAL_CELLS;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtXbarGetCellDropCounter(dev, fport, outFport, cellType, &cellDropCount);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtXbarGetCellDropCounter(dev, fport, outFport, cellType, &cellDropCount);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    outFport = 0;
    cellType = GT_XBAR_NORMAL_CELLS;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_XBAR_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarGetCellDropCounter(dev, fport, outFport, cellType, &cellDropCount);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarGetCellDropCounter(dev, fport, outFport, cellType, &cellDropCount);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarGetDevIdRevId
(
    IN  GT_U8           xbarDevNum,
    OUT GT_XBAR_DEVICE  *xbarDevId,
    OUT GT_U8           *xbarRevId
)
*/
UTF_TEST_CASE_MAC(gtXbarGetDevIdRevId)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with non-NULL xbarDevId 
                   and non-NULL xbarRevId.
    Expected: GT_OK.
    1.2. Call with xbarDevId [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with xbarRevId [NULL]     
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;

    GT_XBAR_DEVICE  xbarDevId = GT_XBAR_98FX900A;
    GT_U8           xbarRevId = 0;

    
    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL xbarDevId 
                           and non-NULL xbarRevId.
            Expected: GT_OK.
        */
        st = gtXbarGetDevIdRevId(dev, &xbarDevId, &xbarRevId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev); 

        /*
            1.2. Call with xbarDevId [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = gtXbarGetDevIdRevId(dev, NULL, &xbarRevId);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, xbarDevId = NULL" ,dev); 

        /*
            1.3. Call with xbarRevId [NULL]     
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = gtXbarGetDevIdRevId(dev, &xbarDevId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, xbarRevId = NULL" ,dev); 
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarGetDevIdRevId(dev, &xbarDevId, &xbarRevId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarGetDevIdRevId(dev, &xbarDevId, &xbarRevId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarConfigDevDataExport
(
    IN    GT_U8     devNum,
    INOUT void      *configDevDataBuffer,
    INOUT GT_U32    *configDevDataBufferSize
)
*/
UTF_TEST_CASE_MAC(gtXbarConfigDevDataExport)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with configDevDataBuffer {xbarDevType [ GT_XBAR_98FX900A / GT_XBAR_98FX902A], 
                                        xbarRevision[0 / 1]} 
                   and configDevDataBufferSize [ sizeof(CPSS_XBAR_HA_CONFIG_DEV_DATA) }]
    Expected: GT_OK.
    1.2. Call with out of range configDevDataBuffer->xbarDevType [0x5AAAAAA5]   
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with out of range configDevDataBufferSize [sizeof (CPSS_XBAR_HA_CONFIG_DEV_DATA) - 1] 
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with configDevDataBuffer [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with configDevDataBufferSize [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;
    
    CPSS_XBAR_HA_CONFIG_DEV_DATA    configDevDataBuffer;
    GT_U32                          configDevDataBufferSize = 0;


    cpssOsBzero((GT_VOID*) &configDevDataBuffer, sizeof(configDevDataBuffer));
    
    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with configDevDataBuffer {xbarDevType [ GT_XBAR_98FX900A / GT_XBAR_98FX902A], 
                                                xbarRevision[0 / 1]} 
                           and configDevDataBufferSize [ sizeof(CPSS_XBAR_HA_CONFIG_DEV_DATA) }]
            Expected: GT_OK.
        */
        /* Call with configDevDataBuffer.xbarDevType = GT_XBAR_98FX900A */
        configDevDataBuffer.xbarDevType  = GT_XBAR_98FX900A;
        configDevDataBuffer.xbarRevision = 0;

        configDevDataBufferSize = sizeof(configDevDataBuffer);

        st = gtXbarConfigDevDataExport(dev, &configDevDataBuffer, &configDevDataBufferSize);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, configDevDataBufferSize = %d",
                                     dev, configDevDataBufferSize);

        /* Call with configDevDataBuffer.xbarDevType = GT_XBAR_98FX902A */
        configDevDataBuffer.xbarDevType  = GT_XBAR_98FX902A;
        configDevDataBuffer.xbarRevision = 1;

        configDevDataBufferSize = sizeof(configDevDataBuffer);

        st = gtXbarConfigDevDataExport(dev, &configDevDataBuffer, &configDevDataBufferSize);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, configDevDataBufferSize = %d",
                                     dev, configDevDataBufferSize);

        /*
            1.2. Call with out of range configDevDataBuffer->xbarDevType [0x5AAAAAA5]   
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        configDevDataBuffer.xbarDevType = XBAR_XBAR_INVALID_ENUM_CNS;
        
        st = gtXbarConfigDevDataExport(dev, &configDevDataBuffer, &configDevDataBufferSize);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, configDevDataBuffer->xbarDevType = %d",
                                    dev, configDevDataBuffer.xbarDevType);

        configDevDataBuffer.xbarDevType = GT_XBAR_98FX902A;

        /*
            1.3. Call with out of range configDevDataBufferSize [sizeof (CPSS_XBAR_HA_CONFIG_DEV_DATA) - 1] 
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        configDevDataBufferSize = sizeof (configDevDataBuffer) - 1;
        
        st = gtXbarConfigDevDataExport(dev, &configDevDataBuffer, &configDevDataBufferSize);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configDevDataBufferSize = %d",
                                         dev, configDevDataBufferSize);

        configDevDataBufferSize = sizeof (configDevDataBuffer);

        /*
            1.4. Call with configDevDataBuffer [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = gtXbarConfigDevDataExport(dev, NULL, &configDevDataBufferSize);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configDevDataBuffer = NULL", dev);

        /*
            1.5. Call with configDevDataBufferSize [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = gtXbarConfigDevDataExport(dev, &configDevDataBuffer, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configDevDataBufferSize = NULL", dev);
    }

    configDevDataBuffer.xbarDevType  = GT_XBAR_98FX900A;
    configDevDataBuffer.xbarRevision = 0;

    configDevDataBufferSize = sizeof(configDevDataBuffer);

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarConfigDevDataExport(dev, &configDevDataBuffer, &configDevDataBufferSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarConfigDevDataExport(dev, &configDevDataBuffer, &configDevDataBufferSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtXbarConfigDevDataImport
(
    IN GT_U8     devNum,
    IN void      *configDevDataBuffer,
    IN GT_U32    configDevDataBufferSize
)
*/
UTF_TEST_CASE_MAC(gtXbarConfigDevDataImport)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with configDevDataBuffer { xbarDevType [ GT_XBAR_98FX900A / GT_XBAR_98FX902A], 
                                         xbarRevision[0 / 1] }
                   and configDevDataBufferSize [ sizeof(CPSS_XBAR_HA_CONFIG_DEV_DATA) ].
    Expected: GT_OK.
    1.2. Call with out of range configDevDataBuffer->xbarDevType [0x5AAAAAA5] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with out of range configDevDataBufferSize [sizeof (CPSS_XBAR_HA_CONFIG_DEV_DATA) - 1] 
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with configDevDataBuffer [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;

    CPSS_XBAR_HA_CONFIG_DEV_DATA    configDevDataBuffer;
    GT_U32                          configDevDataBufferSize = 0;


    cpssOsBzero((GT_VOID*) &configDevDataBuffer, sizeof(configDevDataBuffer));

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with configDevDataBuffer { xbarDevType [ GT_XBAR_98FX900A / GT_XBAR_98FX902A], 
                                                 xbarRevision[0 / 1] }
                           and configDevDataBufferSize [ sizeof(CPSS_XBAR_HA_CONFIG_DEV_DATA) ].
            Expected: GT_OK.
        */
        /* Call with configDevDataBuffer.xbarDevType = GT_XBAR_98FX900A */
        configDevDataBuffer.xbarDevType  = GT_XBAR_98FX900A;
        configDevDataBuffer.xbarRevision = 0;

        configDevDataBufferSize = sizeof(configDevDataBuffer);

        st = gtXbarConfigDevDataImport(dev, &configDevDataBuffer, configDevDataBufferSize);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, configDevDataBufferSize = %d",
                                     dev, configDevDataBufferSize);

        /* Call with configDevDataBuffer.xbarDevType = GT_XBAR_98FX902A */
        configDevDataBuffer.xbarDevType  = GT_XBAR_98FX902A;
        configDevDataBuffer.xbarRevision = 1;

        configDevDataBufferSize = sizeof(configDevDataBuffer);

        st = gtXbarConfigDevDataImport(dev, &configDevDataBuffer, configDevDataBufferSize);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, configDevDataBufferSize = %d",
                                     dev, configDevDataBufferSize);

        /*
            1.2. Call with out of range configDevDataBuffer->xbarDevType [0x5AAAAAA5] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        configDevDataBuffer.xbarDevType = XBAR_XBAR_INVALID_ENUM_CNS;

        st = gtXbarConfigDevDataImport(dev, &configDevDataBuffer, configDevDataBufferSize);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, configDevDataBuffer->xbarDevType = %d", 
                                     dev, configDevDataBuffer.xbarDevType);

        configDevDataBuffer.xbarDevType = GT_XBAR_98FX902A;

        /*
            1.3. Call with out of range configDevDataBufferSize [sizeof (CPSS_XBAR_HA_CONFIG_DEV_DATA) - 1] 
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        configDevDataBufferSize = sizeof(configDevDataBuffer) - 1;

        st = gtXbarConfigDevDataImport(dev, &configDevDataBuffer, configDevDataBufferSize);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configDevDataBufferSize = %d", 
                                         dev, configDevDataBufferSize);

        configDevDataBufferSize = sizeof(configDevDataBuffer);

        /*
            1.4. Call with configDevDataBuffer [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = gtXbarConfigDevDataImport(dev, NULL, configDevDataBufferSize);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configDevDataBuffer = NULL", dev);
    }

    configDevDataBuffer.xbarDevType  = GT_XBAR_98FX900A;
    configDevDataBuffer.xbarRevision = 0;

    configDevDataBufferSize = sizeof(configDevDataBuffer);

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarConfigDevDataImport(dev, &configDevDataBuffer, configDevDataBufferSize);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarConfigDevDataImport(dev, &configDevDataBuffer, configDevDataBufferSize);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS  gtXbarArbConfigSwrr
(
    IN  GT_U8               xbarDevNum,
    IN  GT_U8               fPort,
    IN  GT_U8               tc,
    IN  GT_XBAR_ARB_MODE    arbMode,
    IN  GT_U32              swrrWeight
)
*/
UTF_TEST_CASE_MAC(gtXbarArbConfigSwrr)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS (98FX915)
    1.1.1. Call with tc [0 / 3], 
                     arbMode [GT_XBAR_ARB_WRR / GT_XBAR_ARB_SP], 
                     swrrWeight [0 / 31].
    Expected: GT_OK.
    1.1.2. Call gtXbarGetArbConfigSwrr with the same tc, 
                                            non-NULL arbModePtr and swrrWeightPtr.
    Expected: GT_OK and the same arbMode, swrrWeight.
    1.1.3. Call with out of range tc [4]
                     and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.4. Call with out of range arbMode [0x5AAAAAA5] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.5. Call with arbMode [GT_XBAR_ARB_WRR], 
                     out of range swrrWeight [32] 
                     and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.6. Call with arbMode [GT_XBAR_ARB_SP], 
                     out of range swrrWeight [32] (not relevant) 
                     and other parameters from 1.1.1.
    Expected: GT_OK.
*/
    GT_STATUS   st  = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_XBAR_VALID_FPORT_CNS;

    GT_U8               tc            = 0;
    GT_XBAR_ARB_MODE    arbMode       = GT_XBAR_ARB_WRR;
    GT_XBAR_ARB_MODE    arbModeGet    = GT_XBAR_ARB_WRR;
    GT_U32              swrrWeight    = 0;
    GT_U32              swrrWeightGet = 0;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_CPSS_XBAR_TYPE_98FX915A_CNS);
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
                1.1.1. Call with tc [0 / 3], 
                                 arbMode [GT_XBAR_ARB_WRR / GT_XBAR_ARB_SP], 
                                 swrrWeight [0 / 31].
                Expected: GT_OK.
            */
            /* Call with tc = 0 */
            tc         = 0;
            arbMode    = GT_XBAR_ARB_WRR;
            swrrWeight = 0;

            st = gtXbarArbConfigSwrr(dev, fport, tc, arbMode, swrrWeight);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, fport, tc, arbMode, swrrWeight);

            /*
                1.1.2. Call gtXbarGetArbConfigSwrr with the same tc, 
                                                        non-NULL arbModePtr and swrrWeightPtr.
                Expected: GT_OK and the same arbMode, swrrWeight.
            */
            st = gtXbarGetArbConfigSwrr(dev, fport, tc, &arbModeGet, &swrrWeightGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "gtXbarGetArbConfigSwrr: %d, %d", dev, fport);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(arbMode, arbModeGet,
                       "get another arbMode then was set: %d, %d", dev, fport);
            UTF_VERIFY_EQUAL2_STRING_MAC(swrrWeight, swrrWeightGet,
                       "get another swrrWeight then was set: %d, %d", dev, fport);

            /* Call with tc = 3 */
            tc         = 3;
            arbMode    = GT_XBAR_ARB_SP;
            swrrWeight = 31;

            st = gtXbarArbConfigSwrr(dev, fport, tc, arbMode, swrrWeight);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, fport, tc, arbMode, swrrWeight);
    
            /*
                1.1.2. Call gtXbarGetArbConfigSwrr with the same tc, 
                                                        non-NULL arbModePtr and swrrWeightPtr.
                Expected: GT_OK and the same arbMode, swrrWeight.
            */
            st = gtXbarGetArbConfigSwrr(dev, fport, tc, &arbModeGet, &swrrWeightGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "gtXbarGetArbConfigSwrr: %d, %d", dev, fport);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(arbMode, arbModeGet,
                       "get another arbMode then was set: %d, %d", dev, fport);
            
            /*
                1.1.3. Call with out of range tc [4]
                                 and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            tc = 4;

            st = gtXbarArbConfigSwrr(dev, fport, tc, arbMode, swrrWeight);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, tc);

            tc = 3;
    
            /*
                1.1.4. Call with out of range arbMode [0x5AAAAAA5] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            arbMode = XBAR_XBAR_INVALID_ENUM_CNS;

            st = gtXbarArbConfigSwrr(dev, fport, tc, arbMode, swrrWeight);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, arbMode = %d", dev, fport, arbMode);

            arbMode = GT_XBAR_ARB_SP;
    
            /*
                1.1.5. Call with arbMode [GT_XBAR_ARB_WRR], 
                                 out of range swrrWeight [32] 
                                 and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            arbMode    = GT_XBAR_ARB_WRR;
            swrrWeight = 32;

            st = gtXbarArbConfigSwrr(dev, fport, tc, arbMode, swrrWeight);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, arbMode = %d, swrrWeight = %d",
                                             dev, fport, arbMode, swrrWeight);

            swrrWeight = 31;
    
            /*
                1.1.6. Call with arbMode [GT_XBAR_ARB_SP], 
                                 out of range swrrWeight [32] (not relevant) 
                                 and other parameters from 1.1.1.
                Expected: GT_OK.
            */
            arbMode    = GT_XBAR_ARB_SP;
            swrrWeight = 32;

            st = gtXbarArbConfigSwrr(dev, fport, tc, arbMode, swrrWeight);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, arbMode = %d, swrrWeight = %d",
                                         dev, fport, arbMode, swrrWeight);
        }

        tc         = 0;
        arbMode    = GT_XBAR_ARB_WRR;
        swrrWeight = 0;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtXbarArbConfigSwrr(dev, fport, tc, arbMode, swrrWeight);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtXbarArbConfigSwrr(dev, fport, tc, arbMode, swrrWeight);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    tc         = 0;
    arbMode    = GT_XBAR_ARB_WRR;
    swrrWeight = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_XBAR_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_CPSS_XBAR_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarArbConfigSwrr(dev, fport, tc, arbMode, swrrWeight);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarArbConfigSwrr(dev, fport, tc, arbMode, swrrWeight);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);   
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS  gtXbarGetArbConfigSwrr
(
    IN  GT_U8               xbarDevNum,
    IN  GT_U8               fPort,
    IN  GT_U8               tc,
    OUT GT_XBAR_ARB_MODE    *arbModePtr,
    OUT GT_U32              *swrrWeightPtr
)
*/
UTF_TEST_CASE_MAC(gtXbarGetArbConfigSwrr)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS (98FX915)
    1.1.1. Call with tc [0 / 3], 
                     non-NULL arbModePtr 
                     and non-NULL swrrWeight.
    Expected: GT_OK.
    1.1.2. Call with out of range tc [4] 
                     and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.3. Call with arbModePtr [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
    1.1.4. Call with swrrWeight [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_XBAR_VALID_FPORT_CNS;

    GT_U8               tc            = 0;
    GT_XBAR_ARB_MODE    arbMode       = GT_XBAR_ARB_WRR;
    GT_U32              swrrWeight    = 0;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_CPSS_XBAR_TYPE_98FX915A_CNS);
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
                1.1.1. Call with tc [0 / 3], 
                                 non-NULL arbModePtr 
                                 and non-NULL swrrWeight.
                Expected: GT_OK.
            */
            /* Call with tc = 0 */
            tc = 0;

            st = gtXbarGetArbConfigSwrr(dev, fport, tc, &arbMode, &swrrWeight);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, tc);

            /* Call with tc = 3 */
            tc = 3;

            st = gtXbarGetArbConfigSwrr(dev, fport, tc, &arbMode, &swrrWeight);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, tc);

            /*
                1.1.2. Call with out of range tc [4] 
                                 and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            tc = 4;

            st = gtXbarGetArbConfigSwrr(dev, fport, tc, &arbMode, &swrrWeight);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, tc);

            tc = 3;

            /*
                1.1.3. Call with arbModePtr [NULL] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtXbarGetArbConfigSwrr(dev, fport, tc, NULL, &swrrWeight);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, arbMode = NULL", dev, fport);

            /*
                1.1.4. Call with swrrWeight [NULL] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtXbarGetArbConfigSwrr(dev, fport, tc, &arbMode, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, swrrWeight = NULL", dev, fport);
        }

        tc = 0;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtXbarGetArbConfigSwrr(dev, fport, tc, &arbMode, &swrrWeight);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtXbarGetArbConfigSwrr(dev, fport, tc, &arbMode, &swrrWeight);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    tc = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_XBAR_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_CPSS_XBAR_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarGetArbConfigSwrr(dev, fport, tc, &arbMode, &swrrWeight);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarGetArbConfigSwrr(dev, fport, tc, &arbMode, &swrrWeight);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);   
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS  gtXbarSetByPassMode
(
    IN  GT_U8                   xbarDevNum,
    IN  GT_XBAR_BYPASS_TYPE     bypassType
)
*/
UTF_TEST_CASE_MAC(gtXbarSetByPassMode)
{
/*
    ITERATE_XBAR_DEVICES (98FX915)
    1.1. Call with bypassType [GT_XBAR_BYPASS_NOT_ACTIVE /  
                               GT_XBAR_BYPASS_ACTIVE_STANDBY / 
                               GT_XBAR_BYPASS_ACTIVE_STANDBY_G1_CONNECTED / 
                               GT_XBAR_BYPASS_ACTIVE_ACTIVE]
    Expected: GT_OK.
    1.2. Call gtXbarGetByPassMode with non-NULL bypassTypePtr.
    Expected: GT_OK and the same bypassType.
    1.3. Call with out of range bypassType [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;    
    GT_U8       dev;

    GT_XBAR_BYPASS_TYPE     bypassType    = GT_XBAR_BYPASS_NOT_ACTIVE;
    GT_XBAR_BYPASS_TYPE     bypassTypeGet = GT_XBAR_BYPASS_NOT_ACTIVE;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_CPSS_XBAR_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with bypassType [GT_XBAR_BYPASS_NOT_ACTIVE /  
                                       GT_XBAR_BYPASS_ACTIVE_STANDBY / 
                                       GT_XBAR_BYPASS_ACTIVE_STANDBY_G1_CONNECTED / 
                                       GT_XBAR_BYPASS_ACTIVE_ACTIVE]
            Expected: GT_OK.
        */
        /* Call with bypassType = GT_XBAR_BYPASS_NOT_ACTIVE */
        bypassType = GT_XBAR_BYPASS_NOT_ACTIVE;

        st = gtXbarSetByPassMode(dev, bypassType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, bypassType);

        /*
            1.2. Call gtXbarGetByPassMode with non-NULL bypassTypePtr.
            Expected: GT_OK and the same bypassType.
        */
        st = gtXbarGetByPassMode(dev, &bypassTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtXbarGetByPassMode: %d", dev);

        /* Veifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(bypassType, bypassTypeGet,
                   "get another bypassType then was set: %d", dev);

        /* Call with bypassType = GT_XBAR_BYPASS_ACTIVE_STANDBY */
        bypassType = GT_XBAR_BYPASS_ACTIVE_STANDBY;

        st = gtXbarSetByPassMode(dev, bypassType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, bypassType);

        /*
            1.2. Call gtXbarGetByPassMode with non-NULL bypassTypePtr.
            Expected: GT_OK and the same bypassType.
        */
        st = gtXbarGetByPassMode(dev, &bypassTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtXbarGetByPassMode: %d", dev);

        /* Veifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(bypassType, bypassTypeGet,
                   "get another bypassType then was set: %d", dev);

        /* Call with bypassType = GT_XBAR_BYPASS_ACTIVE_STANDBY_G1_CONNECTED */
        bypassType = GT_XBAR_BYPASS_ACTIVE_STANDBY_G1_CONNECTED;

        st = gtXbarSetByPassMode(dev, bypassType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, bypassType);

        /*
            1.2. Call gtXbarGetByPassMode with non-NULL bypassTypePtr.
            Expected: GT_OK and the same bypassType.
        */
        st = gtXbarGetByPassMode(dev, &bypassTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtXbarGetByPassMode: %d", dev);

        /* Veifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(bypassType, bypassTypeGet,
                   "get another bypassType then was set: %d", dev);

        /* Call with bypassType = GT_XBAR_BYPASS_ACTIVE_ACTIVE */
        bypassType = GT_XBAR_BYPASS_ACTIVE_ACTIVE;

        st = gtXbarSetByPassMode(dev, bypassType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, bypassType);

        /*
            1.2. Call gtXbarGetByPassMode with non-NULL bypassTypePtr.
            Expected: GT_OK and the same bypassType.
        */
        st = gtXbarGetByPassMode(dev, &bypassTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtXbarGetByPassMode: %d", dev);

        /* Veifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(bypassType, bypassTypeGet,
                   "get another bypassType then was set: %d", dev);

        /*
            1.3. Call with out of range bypassType [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        bypassType = XBAR_XBAR_INVALID_ENUM_CNS;

        st = gtXbarSetByPassMode(dev, bypassType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, bypassType);
    }

    bypassType = GT_XBAR_BYPASS_NOT_ACTIVE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    
    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_CPSS_XBAR_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarSetByPassMode(dev, bypassType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarSetByPassMode(dev, bypassType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS  gtXbarGetByPassMode
(
    IN  GT_U8                   xbarDevNum,
    OUT GT_XBAR_BYPASS_TYPE     *bypassTypePtr
)
*/
UTF_TEST_CASE_MAC(gtXbarGetByPassMode)
{
/*
    ITERATE_XBAR_DEVICES (98FX915)
    1.1. Call with non-NULL bypassTypePtr.
    Expected: GT_OK.
    1.2. Call with bypassTypePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;    
    GT_U8       dev;

    GT_XBAR_BYPASS_TYPE bypassType = GT_XBAR_BYPASS_NOT_ACTIVE;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_CPSS_XBAR_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL bypassTypePtr.
            Expected: GT_OK.
        */
        st = gtXbarGetByPassMode(dev, &bypassType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with bypassTypePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = gtXbarGetByPassMode(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, bypassTypePtr = NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    
    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_CPSS_XBAR_TYPE_98FX915A_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtXbarGetByPassMode(dev, &bypassType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtXbarGetByPassMode(dev, &bypassType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of gtXbar suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(gtXbar)
    UTF_SUIT_DECLARE_TEST_MAC(gtGetNumXbarFport)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarFportEnable)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarEnableAllFports)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarSetMacTxXonXoffThresholds)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarSetBcCellTarget)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarSetUcRoute)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarClearUcRoute)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarSetMirrorRoute)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarSetMcRoute)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarClearMcRoute)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarLocalSwitchingEn)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarGetRxCounters)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarGportCounterUpdate)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarGportClearInt)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarGetCellDropCounter)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarGetDevIdRevId)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarConfigDevDataExport)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarConfigDevDataImport)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarArbConfigSwrr)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarGetArbConfigSwrr)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarSetByPassMode)
    UTF_SUIT_DECLARE_TEST_MAC(gtXbarGetByPassMode)
UTF_SUIT_END_TESTS_MAC(gtXbar)

