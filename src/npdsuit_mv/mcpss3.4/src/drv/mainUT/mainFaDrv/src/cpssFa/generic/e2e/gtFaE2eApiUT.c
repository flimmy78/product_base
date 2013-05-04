/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtFaE2eApiUT.c
*
* DESCRIPTION:
*       Prestera API implementation for Prestera Fabric Adapter
*       End to End Flow Control (E2E).
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpssFa/generic/e2e/gtFaE2e.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/*----------------------------------------------------------------------------*/
/*
GT_STATUS  gtFaE2eEnable
(     
    IN GT_U8                devNum,
    IN GT_BOOL              e2eEnable,
    IN GT_FA_E2E_VOQ_STC    e2eVoqStc
)
*/
UTF_TEST_CASE_MAC(gtFaE2eEnable)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with e2eEnable [GT_TRUE / GT_FALSE]
                            and e2eVoqStc {e2eEqCheckEn [GT_TRUE / GT_FALSE],
                                           e2eDqCheckEn [GT_TRUE / GT_FALSE]}.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL           e2eEnable = GT_FALSE;
    GT_FA_E2E_VOQ_STC e2eVoqStc;


    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with e2eEnable [GT_TRUE / GT_FALSE]
                                    and e2eVoqStc {e2eEqCheckEn [GT_TRUE / GT_FALSE],
                                                   e2eDqCheckEn [GT_TRUE / GT_FALSE]}.
            Expected: GT_OK.
        */

        /* Call with e2eEnable [GT_FALSE] */
        e2eEnable = GT_FALSE;

        e2eVoqStc.e2eEqCheckEn = GT_FALSE;
        e2eVoqStc.e2eDqCheckEn = GT_FALSE;

        st = gtFaE2eEnable(dev, e2eEnable, e2eVoqStc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, e2eEnable);

        /* Call with e2eEnable [GT_TRUE] */
        e2eEnable = GT_TRUE;

        e2eVoqStc.e2eEqCheckEn = GT_FALSE;
        e2eVoqStc.e2eDqCheckEn = GT_FALSE;

        st = gtFaE2eEnable(dev, e2eEnable, e2eVoqStc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, e2eEnable);

        e2eVoqStc.e2eEqCheckEn = GT_TRUE;
        e2eVoqStc.e2eDqCheckEn = GT_TRUE;

        st = gtFaE2eEnable(dev, e2eEnable, e2eVoqStc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, e2eEnable);
    }

    e2eEnable = GT_TRUE;

    e2eVoqStc.e2eEqCheckEn = GT_TRUE;
    e2eVoqStc.e2eDqCheckEn = GT_TRUE;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaE2eEnable(dev, e2eEnable, e2eVoqStc);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaE2eEnable(dev, e2eEnable, e2eVoqStc);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaE2eSetConfiguration
(
    IN  GT_U8                 devNum,
    IN  GT_FA_E2E_PP_DEV_CFG  *e2ePpDevCfg_PTR
)
*/
UTF_TEST_CASE_MAC(gtFaE2eSetConfiguration)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with non-NULL e2ePpDevCfg_PTR {ppDevNum [0 / getMaxDevNum(dev) - 1],
                                                      ppNumOfPorts [0 / getMaxPortNum(dev) - 1],
                                                      ppNumOfTc [2 / 8],
                                                      ppNumOfDp [2 / 3]}.
    Expected: GT_OK.
    1.2. Call function with out of range  e2ePpDevCfg_PTR->ppDevNum [getMaxDevNum(dev)]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.3. Call function with out of range e2ePpDevCfg_PTR->ppNumOfPorts [getMaxPortNum(dev)]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.4. Call function with out of range e2ePpDevCfg_PTR->ppNumOfTc [0 / 9]
                            and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.5. Call function with out of range e2ePpDevCfg_PTR->ppNumOfDp [0 / 4]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.6. Call function with e2ePpDevCfg_PTR [NULL]
                            and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_FA_E2E_PP_DEV_CFG e2ePpDevCfg;
    GT_U8                maxFportsNum = 0;
    GT_FA_MAX_NUM_DEV    maxDevNum    = FA_64_DEV_LBH;


    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* Getting max number of fabric ports */
        st = prvUtfNumFaPortsGet(dev, &maxFportsNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfNumFaPortsGet: %d", dev);

        /* Getting max number of packet processors */
        st = prvUtfFaMaxNumPpGet(dev, &maxDevNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFaMaxNumPpGet: %d", dev);

        /* 
            1.1. Call function with non-NULL e2ePpDevCfg_PTR {ppDevNum [0 / getMaxDevNum(dev) - 1],
                                                              ppNumOfPorts [0 / getMaxPortNum(dev) - 1],
                                                              ppNumOfTc [2 / 8],
                                                              ppNumOfDp [2 / 3]}.
            Expected: GT_OK.
        */

        /* Call with ->ppDevNum = 0, ->ppNumOfPorts = 0, ppNumOfTc = 2, ppNumOfDp = 2 */
        e2ePpDevCfg.ppDevNum     = 0;
        e2ePpDevCfg.ppNumOfPorts = 0;
        e2ePpDevCfg.ppNumOfTc    = 2;
        e2ePpDevCfg.ppNumOfDp    = 2;

        st = gtFaE2eSetConfiguration(dev, &e2ePpDevCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with ->ppDevNum = getMaxDevNum-1, ->ppNumOfPorts = getMaxPortNum-1, ppNumOfTc = 8, ppNumOfDp = 3 */
        e2ePpDevCfg.ppDevNum     = maxDevNum - 1;
        e2ePpDevCfg.ppNumOfPorts = maxFportsNum - 1;
        e2ePpDevCfg.ppNumOfTc    = 8;
        e2ePpDevCfg.ppNumOfDp    = 3;

        st = gtFaE2eSetConfiguration(dev, &e2ePpDevCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with out of range  e2ePpDevCfg_PTR->ppDevNum [getMaxDevNum(dev)]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        e2ePpDevCfg.ppDevNum = maxDevNum;
        
        st = gtFaE2eSetConfiguration(dev, &e2ePpDevCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, e2ePpDevCfg_PTR->ppDevNum = %d", dev, e2ePpDevCfg.ppDevNum);

        e2ePpDevCfg.ppDevNum = 0;

        /*
            1.3. Call function with out of range e2ePpDevCfg_PTR->ppNumOfPorts [getMaxPortNum(dev)]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        e2ePpDevCfg.ppNumOfPorts = maxFportsNum;
        
        st = gtFaE2eSetConfiguration(dev, &e2ePpDevCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, e2ePpDevCfg_PTR->ppNumOfPorts = %d", dev, e2ePpDevCfg.ppNumOfPorts);

        e2ePpDevCfg.ppNumOfPorts = 0;

        /*
            1.4. Call function with out of range e2ePpDevCfg_PTR->ppNumOfTc [0 / 9]
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with ->ppNumOfTc = 0 */
        e2ePpDevCfg.ppNumOfTc = 0;
        
        st = gtFaE2eSetConfiguration(dev, &e2ePpDevCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, e2ePpDevCfg_PTR->ppNumOfTc = %d", dev, e2ePpDevCfg.ppNumOfTc);

        /* Call with ->ppNumOfTc = 9 */
        e2ePpDevCfg.ppNumOfTc = 9;
        
        st = gtFaE2eSetConfiguration(dev, &e2ePpDevCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, e2ePpDevCfg_PTR->ppNumOfTc = %d", dev, e2ePpDevCfg.ppNumOfTc);

        e2ePpDevCfg.ppNumOfTc = 2;

        /*
            1.5. Call function with out of range e2ePpDevCfg_PTR->ppNumOfDp [0 / 4]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        /* Call with ->ppNumOfDp = 0 */
        e2ePpDevCfg.ppNumOfDp = 0;
        
        st = gtFaE2eSetConfiguration(dev, &e2ePpDevCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, e2ePpDevCfg_PTR->ppNumOfDp = %d", dev, e2ePpDevCfg.ppNumOfDp);

        /* Call with ->ppNumOfDp = 4 */
        e2ePpDevCfg.ppNumOfDp = 4;
        
        st = gtFaE2eSetConfiguration(dev, &e2ePpDevCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, e2ePpDevCfg_PTR->ppNumOfDp = %d", dev, e2ePpDevCfg.ppNumOfDp);

        e2ePpDevCfg.ppNumOfDp = 2;

        /*
            1.6. Call function with e2ePpDevCfg_PTR [NULL]
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        st = gtFaE2eSetConfiguration(dev, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, e2ePpDevCfg_PTR = NULL", dev);
    }

    e2ePpDevCfg.ppDevNum     = 0;
    e2ePpDevCfg.ppNumOfPorts = 0;
    e2ePpDevCfg.ppNumOfTc    = 2;
    e2ePpDevCfg.ppNumOfDp    = 2;


    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaE2eSetConfiguration(dev, &e2ePpDevCfg);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaE2eSetConfiguration(dev, &e2ePpDevCfg);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaE2eGetStatus
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           ppDevNum,
    IN  GT_U8                           ppPortNum,
    IN  GT_U8                           ppTc,
    OUT GT_FA_E2E_PP_PORT_TC_STATUS     *e2eStatusPtr
)
*/
UTF_TEST_CASE_MAC(gtFaE2eGetStatus)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with ppDevNum [0 / getMaxDevNum(dev) - 1],
                            ppPortNum [0 / getMaxPortNum(dev) - 1],
                            ppTc [0 / CPSS_TC_RANGE_CNS - 1]
                            and non-NULL e2eStatusPtr.
    Expected: GT_OK.
    1.2. Call function with out of range ppDevNum [getMaxDevNum(dev)]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.3. Call function with out of range ppPortNum [getMaxPortNum(dev)]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.4. Call function with out of range ppTc [CPSS_TC_RANGE_CNS = 8]
                            and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.5. Call function with e2eStatusPtr [NULL]
                            and other parameters the same as in 1.1.
                            Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U8                       ppDevNum     = 0;
    GT_U8                       ppPortNum    = 0;
    GT_U8                       ppTc         = 0;
    GT_FA_E2E_PP_PORT_TC_STATUS e2eStatus;
    GT_U8                       maxFportsNum = 0;
    GT_FA_MAX_NUM_DEV           maxDevNum    = FA_64_DEV_LBH;


    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* Getting max number of fabric ports */
        st = prvUtfNumFaPortsGet(dev, &maxFportsNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfNumFaPortsGet: %d", dev);

        /* Getting max number of packet processors */
        st = prvUtfFaMaxNumPpGet(dev, &maxDevNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFaMaxNumPpGet: %d", dev);

        /* 
            1.1. Call function with ppDevNum [0 / getMaxDevNum(dev) - 1],
                                    ppPortNum [0 / getMaxPortNum(dev) - 1],
                                    ppTc [0 / CPSS_TC_RANGE_CNS - 1]
                                    and non-NULL e2eStatusPtr.
            Expected: GT_OK.
        */

        /* Call with ppDevNum = 0, ppPortNum = 0, ppTc = 2 */
        ppDevNum  = 0;
        ppPortNum = 0;
        ppTc      = 2;
        
        st = gtFaE2eGetStatus(dev, ppDevNum, ppPortNum, ppTc, &e2eStatus);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ppDevNum, ppPortNum, ppTc);

        /* Call with ppDevNum = getMaxDevNum-1, ppPortNum = getMaxPortNum-1, ppTc = 7 */
        ppDevNum  = maxDevNum - 1;
        ppPortNum = maxFportsNum - 1;
        ppTc      = 7;
        
        st = gtFaE2eGetStatus(dev, ppDevNum, ppPortNum, ppTc, &e2eStatus);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ppDevNum, ppPortNum, ppTc);

        /*
            1.2. Call function with out of range ppDevNum [getMaxDevNum(dev)]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        ppDevNum = maxDevNum;
        
        st = gtFaE2eGetStatus(dev, ppDevNum, ppPortNum, ppTc, &e2eStatus);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ppDevNum);

        ppDevNum = 0;

        /*
            1.3. Call function with out of range ppPortNum [getMaxPortNum(dev)]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        ppPortNum = maxFportsNum;
        
        st = gtFaE2eGetStatus(dev, ppDevNum, ppPortNum, ppTc, &e2eStatus);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ppPortNum = %d", dev, ppPortNum);

        ppPortNum = 0;

        /*
            1.4. Call function with out of range ppTc [CPSS_TC_RANGE_CNS = 8]
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        ppTc = 8;
        
        st = gtFaE2eGetStatus(dev, ppDevNum, ppPortNum, ppTc, &e2eStatus);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ppTc = %d", dev, ppTc);

        ppTc = 0;

        /*
            1.5. Call function with e2eStatusPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        st = gtFaE2eGetStatus(dev, ppDevNum, ppPortNum, ppTc, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, e2eStatusPtr = NULL", dev);
    }

    ppDevNum  = 0;
    ppPortNum = 0;
    ppTc      = 2;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaE2eGetStatus(dev, ppDevNum, ppPortNum, ppTc, &e2eStatus);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaE2eGetStatus(dev, ppDevNum, ppPortNum, ppTc, &e2eStatus);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtFaE2eDevRemove
(     
    IN GT_U8    devNum,    
    IN GT_U8    ppDevNum
)
*/
UTF_TEST_CASE_MAC(gtFaE2eDevRemove)
{
/*
    ITERATE_FA_DEVICES
    1.1. Call function with ppDevNum [0 / getMaxDevNum(dev) - 1].
    Expected: GT_OK.
    1.2. Call function with out of range ppDevNum [getMaxDevNum(dev)].
    Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U8             ppDevNum     = 0;
    GT_U8             maxFportsNum = 0;
    GT_FA_MAX_NUM_DEV maxDevNum    = FA_64_DEV_LBH;


    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active device fabric adapters. */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_TRUE))
    {
        /* Getting max number of fabric ports */
        st = prvUtfNumFaPortsGet(dev, &maxFportsNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfNumFaPortsGet: %d", dev);

        /* Getting max number of packet processors */
        st = prvUtfFaMaxNumPpGet(dev, &maxDevNum);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFaMaxNumPpGet: %d", dev);

        /* 
            1.1. Call function with ppDevNum [0 / getMaxDevNum(dev) - 1].
            Expected: GT_OK.
        */

        /* Call with ppDevNum = 0 */
        ppDevNum = 0;
        
        st = gtFaE2eDevRemove(dev, ppDevNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ppDevNum);

        /* Call with ppDevNum = getMaxDevNum-1 */
        ppDevNum = maxDevNum - 1;
        
        st = gtFaE2eDevRemove(dev, ppDevNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ppDevNum);

        /*
            1.2. Call function with out of range ppDevNum [getMaxDevNum(dev)].
            Expected: NON GT_OK.
        */
        ppDevNum = maxDevNum;

        st = gtFaE2eDevRemove(dev, ppDevNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ppDevNum);
    }

    ppDevNum = 0;

    /* 2. For not-active device fabric adapters and device device fabric adapters */
    /* from non-applicable type check that function returns NOT GT_OK.            */

    /* prepare device fabric adapter iterator */
    st = prvUtfNextFaReset(&dev, UTF_ALL_FA_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active device fabric adapters */
    while (GT_OK == prvUtfNextFaGet(&dev, GT_FALSE))
    {
        st = gtFaE2eDevRemove(dev, ppDevNum);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 3. Call function with out of bound value for device fabric adapter id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = gtFaE2eDevRemove(dev, ppDevNum);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
* Configuration of gtFaE2eApi suit
*/
UTF_SUIT_BEGIN_TESTS_MAC(gtFaE2eApi)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaE2eEnable)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaE2eSetConfiguration)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaE2eGetStatus)
    UTF_SUIT_DECLARE_TEST_MAC(gtFaE2eDevRemove)
UTF_SUIT_END_TESTS_MAC(gtFaE2eApi)

