/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxSalMirrorUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxSalMirror, that provides
*       CPSS Dx-Sal implementation of Mirror APIs.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxSal/mirror/cpssDxSalMirror.h>
#include <cpss/generic/cos/cpssCosTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define MIRROR_INVALID_ENUM_CNS    0x5AAAAAA5

/* Max length of mirrPorts array */
#define MIRROR_PORTS_ARRAY_LEN_CNS 10

/* Statistic Sniff: maximum ratio of Rx sniffed packets */
#define MAX_STATISTIC_SNIFF_RATIO_CNS   2047

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalMirrSetRxAnalyzerPort
(
    IN  GT_U8   dev,
    IN  GT_U8   analyzerPort,
    IN  GT_U8   analyzerDev,
    IN  GT_U32  analyzerTc
)
*/
UTF_TEST_CASE_MAC(cpssDxSalMirrSetRxAnalyzerPort)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with analyzerPort [0],
                            analyzerDev [0],
                            analyzerTc [0 / CPSS_4_TC_RANGE_CNS-1 = 3].
    Expected: GT_OK.
    1.2. Call with analyzerPort [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with analyzerDev [PRV_CPSS_MAX_PP_DEVICES_CNS],
                   other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call function with out of range analyzerTc [CPSS_4_TC_RANGE_CNS = 4],
                            other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U8    analyzerPort = 0;
    GT_U8    analyzerDev  = 0;
    GT_U32   analyzerTc   = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with analyzerPort [0],
                                    analyzerDev [0],
                                    analyzerTc [0 / CPSS_4_TC_RANGE_CNS-1 = 3].
            Expected: GT_OK.
        */

        /* Call with analyzerPort[0], analyzerDev[0], analyzerTc[0] */
        analyzerPort = 0;
        analyzerDev  = 0;
        analyzerTc   = 0;
        
        st = cpssDxSalMirrSetRxAnalyzerPort(dev, analyzerPort, analyzerDev, analyzerTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, analyzerPort, analyzerDev, analyzerTc);

        /* Call with analyzerPort[0], analyzerDev[0], analyzerTc[CPSS_4_TC_RANGE_CNS - 1] */
        analyzerTc   = CPSS_4_TC_RANGE_CNS - 1;
        
        st = cpssDxSalMirrSetRxAnalyzerPort(dev, analyzerPort, analyzerDev, analyzerTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, analyzerPort, analyzerDev, analyzerTc);

        /*
            1.2. Call with analyzerPort [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                           other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        analyzerPort = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        
        st = cpssDxSalMirrSetRxAnalyzerPort(dev, analyzerPort, analyzerDev, analyzerTc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, analyzerPort);

        analyzerPort = 0;

        /*
            1.3. Call with analyzerDev [PRV_CPSS_MAX_PP_DEVICES_CNS],
                           other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        analyzerDev = PRV_CPSS_MAX_PP_DEVICES_CNS;
        
        st = cpssDxSalMirrSetRxAnalyzerPort(dev, analyzerPort, analyzerDev, analyzerTc);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, analyzerDev = %d", dev, analyzerDev);

        analyzerDev = 0;

        /*
            1.4. Call function with out of range analyzerTc [CPSS_4_TC_RANGE_CNS = 4],
                                    other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        analyzerTc = CPSS_4_TC_RANGE_CNS;
        
        st = cpssDxSalMirrSetRxAnalyzerPort(dev, analyzerPort, analyzerDev, analyzerTc);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, analyzerTc = %d", dev, analyzerTc);

        analyzerTc = 0;
    }

    analyzerPort = 0;
    analyzerDev  = 0;
    analyzerTc   = 0;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalMirrSetRxAnalyzerPort(dev, analyzerPort, analyzerDev, analyzerTc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalMirrSetRxAnalyzerPort(dev, analyzerPort, analyzerDev, analyzerTc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalMirrSetTxAnalyzerPort
(
    IN  GT_U8   dev,
    IN  GT_U8   analyzerPort,
    IN  GT_U8   analyzerDev,
    IN  GT_U32  analyzerTc
)
*/
UTF_TEST_CASE_MAC(cpssDxSalMirrSetTxAnalyzerPort)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with analyzerPort [0],
                            analyzerDev [0],
                            analyzerTc [0 / CPSS_4_TC_RANGE_CNS-1 = 3].
    Expected: GT_OK.
    1.2. Call with analyzerPort [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with analyzerDev [PRV_CPSS_MAX_PP_DEVICES_CNS],
                   other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call function with out of range analyzerTc [CPSS_4_TC_RANGE_CNS = 4],
                            other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U8    analyzerPort = 0;
    GT_U8    analyzerDev  = 0;
    GT_U32   analyzerTc   = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with analyzerPort [0],
                                    analyzerDev [0],
                                    analyzerTc [0 / CPSS_4_TC_RANGE_CNS-1 = 3].
            Expected: GT_OK.
        */

        /* Call with analyzerPort[0], analyzerDev[0], analyzerTc[0] */
        analyzerPort = 0;
        analyzerDev  = 0;
        analyzerTc   = 0;
        
        st = cpssDxSalMirrSetTxAnalyzerPort(dev, analyzerPort, analyzerDev, analyzerTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, analyzerPort, analyzerDev, analyzerTc);

        /* Call with analyzerPort[0], analyzerDev[0], analyzerTc[CPSS_4_TC_RANGE_CNS - 1] */
        analyzerTc   = CPSS_4_TC_RANGE_CNS - 1;
        
        st = cpssDxSalMirrSetTxAnalyzerPort(dev, analyzerPort, analyzerDev, analyzerTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, analyzerPort, analyzerDev, analyzerTc);

        /*
            1.2. Call with analyzerPort [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                           other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        analyzerPort = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        
        st = cpssDxSalMirrSetTxAnalyzerPort(dev, analyzerPort, analyzerDev, analyzerTc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, analyzerPort);

        analyzerPort = 0;

        /*
            1.3. Call with analyzerDev [PRV_CPSS_MAX_PP_DEVICES_CNS],
                           other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        analyzerDev = PRV_CPSS_MAX_PP_DEVICES_CNS;
        
        st = cpssDxSalMirrSetTxAnalyzerPort(dev, analyzerPort, analyzerDev, analyzerTc);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, analyzerDev = %d", dev, analyzerDev);

        analyzerDev = 0;

        /*
            1.4. Call function with out of range analyzerTc [CPSS_4_TC_RANGE_CNS = 4],
                                    other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        analyzerTc = CPSS_4_TC_RANGE_CNS;
        
        st = cpssDxSalMirrSetTxAnalyzerPort(dev, analyzerPort, analyzerDev, analyzerTc);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, analyzerTc = %d", dev, analyzerTc);

        analyzerTc = 0;
    }

    analyzerPort = 0;
    analyzerDev  = 0;
    analyzerTc   = 0;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalMirrSetTxAnalyzerPort(dev, analyzerPort, analyzerDev, analyzerTc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalMirrSetTxAnalyzerPort(dev, analyzerPort, analyzerDev, analyzerTc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalMirrSetRxPort
(
    IN  GT_U8   dev,
    IN  GT_U8   mirrPortsArray[],
    IN  GT_U32  mirrPortsArrayLen
)
*/
UTF_TEST_CASE_MAC(cpssDxSalMirrSetRxPort)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with mirrPortsArray [0, 1, 2],
                            mirrPortsArrayLen [3].
    Expected: GT_OK.
    1.2. Check CPU port. Call with mirrPortsArray [CPSS_CPU_PORT_NUM_CNS],
                                   mirrPortsArrayLen [1].
    Expected: GT_OK.
    1.3. Call with mirrPortsArray [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   mirrPortsArrayLen [1].
    Expected: GT_BAD_PARAM.
    1.4. For cleanup call cpssDxSalMirrDelRxPort with mirrPortsArray [0,1,2],
                                                      mirrPortsArrayLen [3].
    Expected: GT_OK.
    1.5. Call function with mirrPortsArray [NULL],
                            mirrPortsArrayLen [1].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U8     mirrPortsArray[MIRROR_PORTS_ARRAY_LEN_CNS];
    GT_U32    mirrPortsArrayLen = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with mirrPortsArray [0, 1, 2],
                                    mirrPortsArrayLen [3].
            Expected: GT_OK.
        */
        mirrPortsArray[0] = 0;
        mirrPortsArray[2] = 1;
        mirrPortsArray[3] = 2;

        mirrPortsArrayLen = 3;
        
        st = cpssDxSalMirrSetRxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, mirrPortsArrayLen = %d", dev, mirrPortsArrayLen);

        /*
            1.2. Check CPU port. Call with mirrPortsArray [CPSS_CPU_PORT_NUM_CNS],
                                   mirrPortsArrayLen [1].
            Expected: GT_OK.
        */
        mirrPortsArray[0] = CPSS_CPU_PORT_NUM_CNS;
        mirrPortsArrayLen = 1;

        st = cpssDxSalMirrSetRxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, mirrPortsArrayLen = %d", dev, mirrPortsArrayLen);

        mirrPortsArray[0] = 0;

        /*
            1.3. Call with mirrPortsArray [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                           mirrPortsArrayLen [1].
            Expected: GT_BAD_PARAM.
        */
        mirrPortsArray[0] = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        mirrPortsArrayLen = 1;

        st = cpssDxSalMirrSetRxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, mirrPortsArray[0] = %d, mirrPortsArrayLen = %d",
                                     dev, mirrPortsArray[0], mirrPortsArrayLen);

        mirrPortsArray[0] = 0;

        /*
            1.4. For cleanup call cpssDxSalMirrDelRxPort with mirrPortsArray [0,1,2],
                                                              mirrPortsArrayLen [3].
            Expected: GT_OK.
        */
        mirrPortsArrayLen = 3;

        st = cpssDxSalMirrDelRxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssDxSalMirrDelRxPort: %d, mirrPortsArrayLen = %d", dev, mirrPortsArrayLen);

        /*
            1.5. Call function with mirrPortsArray [NULL],
                                    mirrPortsArrayLen [1].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalMirrSetRxPort(dev, NULL, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, mirrPortsArray = NULL, mirrPortsArrayLen = %d",
                                     dev, mirrPortsArrayLen);
    }

    mirrPortsArray[0] = 0;
    mirrPortsArray[2] = 1;
    mirrPortsArray[3] = 2;

    mirrPortsArrayLen = 3;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalMirrSetRxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalMirrSetRxPort(dev, mirrPortsArray, mirrPortsArrayLen);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalMirrSetTxPort
(
    IN  GT_U8   dev,
    IN  GT_U8   mirrPortsArray[],
    IN  GT_U32  mirrPortsArrayLen
)
*/
UTF_TEST_CASE_MAC(cpssDxSalMirrSetTxPort)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with mirrPortsArray [0, 1, 2],
                            mirrPortsArrayLen [3].
    Expected: GT_OK.
    1.2. Check CPU port. Call with mirrPortsArray [CPSS_CPU_PORT_NUM_CNS],
                                   mirrPortsArrayLen [1].
    Expected: GT_OK.
    1.3. Check more than 8 ports (unsupported). Call with mirrPortsArray [0,1,2,3,4,5,6,7,8],
                                                          mirrPortsArrayLen [9].
    Expected: non GT_OK.
    1.4. Call with mirrPortsArray [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   mirrPortsArrayLen [1].
    Expected: GT_BAD_PARAM.
    1.5. For cleanup call cpssDxSalMirrDelTxPort with mirrPortsArray [0,1,2,3,4,5,6,7,8],
                                                      mirrPortsArrayLen [9].
    Expected: GT_OK.
    1.6. Call function with mirrPortsArray [NULL],
                            mirrPortsArrayLen [1].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U8     mirrPortsArray[MIRROR_PORTS_ARRAY_LEN_CNS];
    GT_U32    mirrPortsArrayLen = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with mirrPortsArray [0, 1, 2],
                                    mirrPortsArrayLen [3].
            Expected: GT_OK.
        */
        mirrPortsArray[0] = 0;
        mirrPortsArray[2] = 1;
        mirrPortsArray[3] = 2;

        mirrPortsArrayLen = 3;
        
        st = cpssDxSalMirrSetTxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, mirrPortsArrayLen = %d", dev, mirrPortsArrayLen);

        /*
            1.2. Check CPU port. Call with mirrPortsArray [CPSS_CPU_PORT_NUM_CNS],
                                   mirrPortsArrayLen [1].
            Expected: GT_OK.
        */
        mirrPortsArray[0] = CPSS_CPU_PORT_NUM_CNS;
        mirrPortsArrayLen = 1;

        st = cpssDxSalMirrSetTxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, mirrPortsArrayLen = %d", dev, mirrPortsArrayLen);

        mirrPortsArray[0] = 0;

        /*
            1.3. Check more than 8 ports (unsupported). Call with mirrPortsArray [0,1,2,3,4,5,6,7,8],
                                                                  mirrPortsArrayLen [10].
            Expected: non GT_OK.
        */
        mirrPortsArray[4] = 4;
        mirrPortsArray[5] = 5;
        mirrPortsArray[6] = 6;
        mirrPortsArray[7] = 7;
        mirrPortsArray[8] = 8;

        mirrPortsArrayLen = 9;
        
        st = cpssDxSalMirrSetTxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, mirrPortsArrayLen = %d", dev, mirrPortsArrayLen);

        /*
            1.4. Call with mirrPortsArray [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                           mirrPortsArrayLen [1].
            Expected: GT_BAD_PARAM.
        */
        mirrPortsArray[0] = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        mirrPortsArrayLen = 1;

        st = cpssDxSalMirrSetTxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, mirrPortsArray[0] = %d, mirrPortsArrayLen = %d",
                                     dev, mirrPortsArray[0], mirrPortsArrayLen);

        mirrPortsArray[0] = 0;

        /*
            1.5. For cleanup call cpssDxSalMirrDelTxPort with mirrPortsArray [0,1,2,3,4,5,6,7,8],
                                                              mirrPortsArrayLen [9].
            Expected: GT_OK.
        */
        mirrPortsArrayLen = 9;

        st = cpssDxSalMirrDelTxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssDxSalMirrDelTxPort: %d, mirrPortsArrayLen = %d", dev, mirrPortsArrayLen);

        /*
            1.6. Call function with mirrPortsArray [NULL],
                                    mirrPortsArrayLen [1].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalMirrSetTxPort(dev, NULL, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, mirrPortsArray = NULL, mirrPortsArrayLen = %d",
                                     dev, mirrPortsArrayLen);
    }

    mirrPortsArray[0] = 0;
    mirrPortsArray[2] = 1;
    mirrPortsArray[3] = 2;

    mirrPortsArrayLen = 3;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalMirrSetTxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalMirrSetTxPort(dev, mirrPortsArray, mirrPortsArrayLen);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalMirrDelRxPort
(
    IN  GT_U8   dev,
    IN  GT_U8   mirrPortsArray[],
    IN  GT_U32  mirrPortsArrayLen
)
*/
UTF_TEST_CASE_MAC(cpssDxSalMirrDelRxPort)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with mirrPortsArray [0, 1, 2],
                            mirrPortsArrayLen [3].
    Expected: GT_OK.
    1.2. Check CPU port. Call with mirrPortsArray [CPSS_CPU_PORT_NUM_CNS],
                                   mirrPortsArrayLen [1].
    Expected: GT_OK.
    1.3. Call with mirrPortsArray [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   mirrPortsArrayLen [1].
    Expected: GT_BAD_PARAM.
    1.4. Call function with mirrPortsArray [NULL],
                            mirrPortsArrayLen [1].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U8     mirrPortsArray[MIRROR_PORTS_ARRAY_LEN_CNS];
    GT_U32    mirrPortsArrayLen = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with mirrPortsArray [0, 1, 2],
                                    mirrPortsArrayLen [3].
            Expected: GT_OK.
        */
        mirrPortsArray[0] = 0;
        mirrPortsArray[2] = 1;
        mirrPortsArray[3] = 2;

        mirrPortsArrayLen = 3;
        
        st = cpssDxSalMirrDelRxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, mirrPortsArrayLen = %d", dev, mirrPortsArrayLen);

        /*
            1.2. Check CPU port. Call with mirrPortsArray [CPSS_CPU_PORT_NUM_CNS],
                                   mirrPortsArrayLen [1].
            Expected: GT_OK.
        */
        mirrPortsArray[0] = CPSS_CPU_PORT_NUM_CNS;
        mirrPortsArrayLen = 1;

        st = cpssDxSalMirrDelRxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, mirrPortsArrayLen = %d", dev, mirrPortsArrayLen);

        mirrPortsArray[0] = 0;

        /*
            1.3. Call with mirrPortsArray [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                           mirrPortsArrayLen [1].
            Expected: GT_BAD_PARAM.
        */
        mirrPortsArray[0] = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        mirrPortsArrayLen = 1;

        st = cpssDxSalMirrDelRxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, mirrPortsArray[0] = %d, mirrPortsArrayLen = %d",
                                     dev, mirrPortsArray[0], mirrPortsArrayLen);

        mirrPortsArray[0] = 0;

        /*
            1.4. Call function with mirrPortsArray [NULL],
                                    mirrPortsArrayLen [1].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalMirrDelRxPort(dev, NULL, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, mirrPortsArray = NULL, mirrPortsArrayLen = %d",
                                     dev, mirrPortsArrayLen);
    }

    mirrPortsArray[0] = 0;
    mirrPortsArray[2] = 1;
    mirrPortsArray[3] = 2;

    mirrPortsArrayLen = 3;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalMirrDelRxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalMirrDelRxPort(dev, mirrPortsArray, mirrPortsArrayLen);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalMirrDelTxPort
(
    IN  GT_U8   dev,
    IN  GT_U8   mirrPortsArray[],
    IN  GT_U32  mirrPortsArrayLen
)
*/
UTF_TEST_CASE_MAC(cpssDxSalMirrDelTxPort)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with mirrPortsArray [0, 1, 2],
                            mirrPortsArrayLen [3].
    Expected: GT_OK.
    1.2. Check CPU port. Call with mirrPortsArray [CPSS_CPU_PORT_NUM_CNS],
                                   mirrPortsArrayLen [1].
    Expected: GT_OK.
    1.3. Call with mirrPortsArray [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                   mirrPortsArrayLen [1].
    Expected: GT_BAD_PARAM.
    1.4. Check more than 8 ports (unsupported). Call with mirrPortsArray [0,1,2,3,4,5,6,7,8],
                                                mirrPortsArrayLen [9].
    Expected: non GT_OK.
    1.5. Call function with mirrPortsArray [NULL],
                            mirrPortsArrayLen [1].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U8     mirrPortsArray[MIRROR_PORTS_ARRAY_LEN_CNS];
    GT_U32    mirrPortsArrayLen = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with mirrPortsArray [0, 1, 2],
                                    mirrPortsArrayLen [3].
            Expected: GT_OK.
        */
        mirrPortsArray[0] = 0;
        mirrPortsArray[2] = 1;
        mirrPortsArray[3] = 2;

        mirrPortsArrayLen = 3;
        
        st = cpssDxSalMirrDelTxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, mirrPortsArrayLen = %d", dev, mirrPortsArrayLen);

        /*
            1.2. Check CPU port. Call with mirrPortsArray [CPSS_CPU_PORT_NUM_CNS],
                                   mirrPortsArrayLen [1].
            Expected: GT_OK.
        */
        mirrPortsArray[0] = CPSS_CPU_PORT_NUM_CNS;
        mirrPortsArrayLen = 1;

        st = cpssDxSalMirrDelTxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, mirrPortsArrayLen = %d", dev, mirrPortsArrayLen);

        mirrPortsArray[0] = 0;

        /*
            1.3. Call with mirrPortsArray [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
                           mirrPortsArrayLen [1].
            Expected: GT_BAD_PARAM.
        */
        mirrPortsArray[0] = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        mirrPortsArrayLen = 1;

        st = cpssDxSalMirrDelTxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, mirrPortsArray[0] = %d, mirrPortsArrayLen = %d",
                                     dev, mirrPortsArray[0], mirrPortsArrayLen);

        mirrPortsArray[0] = 0;

        /*
            1.4. Check more than 8 ports (unsupported). Call with mirrPortsArray [0,1,2,3,4,5,6,7,8],
                                                                  mirrPortsArrayLen [10].
            Expected: non GT_OK.
        */
        mirrPortsArray[4] = 4;
        mirrPortsArray[5] = 5;
        mirrPortsArray[6] = 6;
        mirrPortsArray[7] = 7;
        mirrPortsArray[8] = 8;

        mirrPortsArrayLen = 9;
        
        st = cpssDxSalMirrDelTxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, mirrPortsArrayLen = %d", dev, mirrPortsArrayLen);

        /*
            1.5. Call function with mirrPortsArray [NULL],
                                    mirrPortsArrayLen [1].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalMirrDelTxPort(dev, NULL, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, mirrPortsArray = NULL, mirrPortsArrayLen = %d",
                                     dev, mirrPortsArrayLen);
    }

    mirrPortsArray[0] = 0;
    mirrPortsArray[2] = 1;
    mirrPortsArray[3] = 2;

    mirrPortsArrayLen = 3;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalMirrDelTxPort(dev, mirrPortsArray, mirrPortsArrayLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalMirrDelTxPort(dev, mirrPortsArray, mirrPortsArrayLen);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalMirrSetCpuRxMirrorParams
(
    IN  GT_U8       dev,
    IN  GT_BOOL     enable,
    IN  GT_U32      mirrorTc
)
*/
UTF_TEST_CASE_MAC(cpssDxSalMirrSetCpuRxMirrorParams)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with enable [GT_TRUE / GT_FALSE],
                            mirrorTc [0 / CPSS_4_TC_RANGE_CNS-1 = 3].
    Expected: GT_OK.
    1.2. Call function with out of range mirrorTc [CPSS_4_TC_RANGE_CNS = 4],
                            other params same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_BOOL   enable   = GT_FALSE;
    GT_U32    mirrorTc = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with enable [GT_TRUE / GT_FALSE],
                                    mirrorTc [0 / CPSS_4_TC_RANGE_CNS-1 = 3].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE], mirrorTc [0]. */
        enable   = GT_FALSE;
        mirrorTc = 0;
        
        st = cpssDxSalMirrSetCpuRxMirrorParams(dev, enable, mirrorTc);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, mirrorTc);

        /* Call with enable [GT_FALSE], mirrorTc [CPSS_4_TC_RANGE_CNS - 1]. */
        mirrorTc = CPSS_4_TC_RANGE_CNS - 1;
        
        st = cpssDxSalMirrSetCpuRxMirrorParams(dev, enable, mirrorTc);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, mirrorTc);

        /* Call with enable [GT_TRUE], mirrorTc [0]. */
        enable   = GT_TRUE;
        mirrorTc = 0;
        
        st = cpssDxSalMirrSetCpuRxMirrorParams(dev, enable, mirrorTc);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, mirrorTc);

        /* Call with enable [GT_TRUE], mirrorTc [CPSS_4_TC_RANGE_CNS - 1]. */
        mirrorTc = CPSS_4_TC_RANGE_CNS - 1;
        
        st = cpssDxSalMirrSetCpuRxMirrorParams(dev, enable, mirrorTc);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, mirrorTc);

        /*
            1.2. Call function with out of range mirrorTc [CPSS_4_TC_RANGE_CNS = 4],
                                    other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        mirrorTc = CPSS_4_TC_RANGE_CNS;
        
        st = cpssDxSalMirrSetCpuRxMirrorParams(dev, enable, mirrorTc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrorTc);
    }

    enable   = GT_TRUE;
    mirrorTc = 0;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalMirrSetCpuRxMirrorParams(dev, enable, mirrorTc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalMirrSetCpuRxMirrorParams(dev, enable, mirrorTc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalMirrSetStatisticalMirrRate
(
    IN    GT_U8   dev,
    IN    GT_U32  ratio
)
*/
UTF_TEST_CASE_MAC(cpssDxSalMirrSetStatisticalMirrRate)
{
/*
    ITERATE_DEVICES 
    1.1. Call function with ratio [0 / 100 / MAX_STATISTIC_SNIFF_RATIO_CNS = 2047].
    Expected: GT_OK.
    1.2. Call function with out of range ratio [MAX_STATISTIC_SNIFF_RATIO_CNS +1 = 2048],
                            other params same as in 1.1.
    Expected: non GT_OK.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U32    ratio = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with ratio [0 / 100 / MAX_STATISTIC_SNIFF_RATIO_CNS = 2047].
            Expected: GT_OK.
        */

        /* Call with ratio [0]. */
        ratio = 0;
        
        st = cpssDxSalMirrSetStatisticalMirrRate(dev, ratio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ratio);

        /* Call with ratio [0]. */
        ratio = 100;
        
        st = cpssDxSalMirrSetStatisticalMirrRate(dev, ratio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ratio);

        /* Call with ratio [MAX_STATISTIC_SNIFF_RATIO_CNS]. */
        ratio = MAX_STATISTIC_SNIFF_RATIO_CNS;
        
        st = cpssDxSalMirrSetStatisticalMirrRate(dev, ratio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ratio);
        /*
            1.2. Call function with out of range ratio [MAX_STATISTIC_SNIFF_RATIO_CNS +1 = 2048],
                                    other params same as in 1.1.
            Expected: non GT_OK.
        */
        ratio = MAX_STATISTIC_SNIFF_RATIO_CNS + 1;
        
        st = cpssDxSalMirrSetStatisticalMirrRate(dev, ratio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, ratio);
    }

    ratio = 0;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalMirrSetStatisticalMirrRate(dev, ratio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalMirrSetStatisticalMirrRate(dev, ratio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssDxSalMirror suit
*/
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxSalMirror)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalMirrSetRxAnalyzerPort)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalMirrSetTxAnalyzerPort)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalMirrSetRxPort)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalMirrSetTxPort)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalMirrDelRxPort)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalMirrDelTxPort)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalMirrSetCpuRxMirrorParams)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalMirrSetStatisticalMirrRate)
UTF_SUIT_END_TESTS_MAC(cpssDxSalMirror)

