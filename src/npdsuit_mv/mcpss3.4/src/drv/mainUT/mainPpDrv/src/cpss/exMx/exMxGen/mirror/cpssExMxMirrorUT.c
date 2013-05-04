/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxBrgVlanUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxBrgVlan.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/exMx/exMxGen/mirror/cpssExMxMirror.h>
#include <cpss/generic/mirror/private/prvCpssMirrorTypes.h>
#include <cpss/generic/cos/cpssCosTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxMirrorRxAnalyzerPortSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   analyzerPort,
    IN  GT_U8   analyzerDevNum,
    IN  GT_U8   analyzerTc
)
*/
UTF_TEST_CASE_MAC(cpssExMxMirrorRxAnalyzerPortSet)
{
/*
ITERATE_DEVICES
1.1. Call function with correct values of other parameters:
    analizerPort [2], analizerDevNum [18], analizerTc[3].
    Expected: GT_OK.
1.2. Call function cpssExMxMirrorRxAnalyzerPortGet with non NULL pointers.
    Expected: GT_OK and analyzerPort, analyzerDevNum and analyzerTc the same as written.
1.3. Call function with out of range analizerPort [PRV_CPSS_MAX_PP_PORTS_NUM_CNS].
    Expected: GT_OUT_OF_RANGE.
1.4. Call function with out of range analizerDevNum [PRV_CPSS_MAX_PP_DEVICES_CNS].
    Expected: GT_OUT_OF_RANGE.
1.5. Call function with out of range analizerTc [CPSS_TC_RANGE_CNS].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;

    GT_U8   dev;
    GT_U8   analyzerPort;
    GT_U8   analyzerDevNum;
    GT_U8   analyzerTc;

    GT_U8   retAnalyzerPort;
    GT_U8   retAnalyzerDevNum;
    GT_U8   retAnalyzerTc;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with correct values of other parameters:  */
        /* analyzerPort [2], analyzerDevNum [18], analyzerTc[3].        */
        /* Expected: GT_OK.                                             */
        analyzerPort = 2;
        analyzerDevNum = 18;
        analyzerTc = 3;

        st = cpssExMxMirrorRxAnalyzerPortSet(dev, analyzerPort,
                                             analyzerDevNum, analyzerTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, analyzerPort,
                                    analyzerDevNum, analyzerTc);

        /* 1.2. Call function cpssExMxMirrorRxAnalyzerPortGet with
        non NULL pointers. Expected: GT_OK and analyzerPort,
        analyzerDevNum and analyzerTc the same as written. */
        st = cpssExMxMirrorRxAnalyzerPortGet(dev, &retAnalyzerPort, &retAnalyzerDevNum, &retAnalyzerTc);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                     "cpssExMxMirrorRxAnalyzerPortGet: %d", dev);
        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(analyzerPort, retAnalyzerPort,
                                         "get another analyzerPort than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(analyzerDevNum, retAnalyzerDevNum,
                                         "get another analyzerDevNum than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(analyzerTc, retAnalyzerTc,
                                         "get another analyzerTc than was set: %d", dev);
        }

        /* 1.3. Call function with out of range analyzerPort            */
        /* [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. Expected: GT_OUT_OF_RANGE.  */
        analyzerPort = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxMirrorRxAnalyzerPortSet(dev, analyzerPort,
                                             analyzerDevNum, analyzerTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, analyzerPort,
                                    analyzerDevNum, analyzerTc);

        analyzerPort = 2; /* restore */

        /* 1.4. Call function with out of range analyzerDevNum          */
        /* [PRV_CPSS_MAX_PP_DEVICES_CNS]. Expected: GT_OUT_OF_RANGE.    */
        analyzerDevNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxMirrorRxAnalyzerPortSet(dev, analyzerPort,
                                             analyzerDevNum, analyzerTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, analyzerPort,
                                    analyzerDevNum, analyzerTc);

        analyzerDevNum = 18; /* restore */

        /* 1.5. Call function with out of range analyzerTc          */
        /* [CPSS_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.             */
        analyzerTc = CPSS_TC_RANGE_CNS;

        st = cpssExMxMirrorRxAnalyzerPortSet(dev, analyzerPort,
                                             analyzerDevNum, analyzerTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, analyzerPort,
                                    analyzerDevNum, analyzerTc);

    }

    analyzerPort = 12;
    analyzerDevNum = 8;
    analyzerTc = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxMirrorRxAnalyzerPortSet(dev, analyzerPort,
                                             analyzerDevNum, analyzerTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, analyzerPort,
                                    analyzerDevNum, analyzerTc);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxMirrorRxAnalyzerPortSet(dev, analyzerPort,
                                         analyzerDevNum, analyzerTc);
    UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, analyzerPort,
                                analyzerDevNum, analyzerTc);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxMirrorRxAnalyzerPortGet
(
    IN  GT_U8   devNum,
    OUT GT_U8   *analyzerPortPtr,
    OUT GT_U8   *analyzerDevNumPtr,
    OUT GT_U8   *analyzerTcPtr
)
*/

UTF_TEST_CASE_MAC(cpssExMxMirrorRxAnalyzerPortGet)
{
/*
ITERATE_DEVICES
1.1. Call function with non-NULL pointers. Expected: GT_OK.
1.2. Call function with analyzerPortPtr [NULL]. Expected: GT_BAD_PTR.
1.3. Call function with analyzerDevNumPtr [NULL]. Expected: GT_BAD_PTR.
1.4. Call function with analyzerTcPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8   dev;
    GT_U8   retAnalyzerPort;
    GT_U8   retAnalyzerDevNum;
    GT_U8   retAnalyzerTc;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL pointers. Expected: GT_OK. */
        st = cpssExMxMirrorRxAnalyzerPortGet(dev, &retAnalyzerPort, &retAnalyzerDevNum, &retAnalyzerTc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with analyzerPortPtr [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxMirrorRxAnalyzerPortGet(dev, NULL, &retAnalyzerDevNum, &retAnalyzerTc);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, analyzerPortPtr = NULL", dev);

        /* 1.3. Call function with analyzerDevNumPtr [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxMirrorRxAnalyzerPortGet(dev, &retAnalyzerPort, NULL, &retAnalyzerTc);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, analyzerDevNumPtr = NULL", dev);

        /* 1.4. Call function with analyzerTcPtr [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxMirrorRxAnalyzerPortGet(dev, &retAnalyzerPort, &retAnalyzerDevNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, analyzerTcPtr = NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxMirrorRxAnalyzerPortGet(dev, &retAnalyzerPort, &retAnalyzerDevNum, &retAnalyzerTc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxMirrorRxAnalyzerPortGet(dev, &retAnalyzerPort, &retAnalyzerDevNum, &retAnalyzerTc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxMirrorTxAnalyzerPortSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   analyzerPort,
    IN  GT_U8   analyzerDevNum,
    IN  GT_U8   analyzerTc
)
*/
UTF_TEST_CASE_MAC(cpssExMxMirrorTxAnalyzerPortSet)
{
/*
ITERATE_DEVICES
1.1. Call function with correct values of other parameters:
    analyzerPort [2], analyzerDevNum [18], analyzerTc[3].
    Expected: GT_OK.
1.2. Call function cpssExMxMirrorTxAnalyzerPortGet with non NULL pointers.
    Expected: GT_OK and analyzerPort, analyzerDevNum and analyzerTc (tc for 98Mx1x6/8 devices only) 
    the same as written.
1.3. Call function with out of range analyzerPort [PRV_CPSS_MAX_PP_PORTS_NUM_CNS].
    Expected: GT_OUT_OF_RANGE.
1.4. Call function with out of range analyzerDevNum [PRV_CPSS_MAX_PP_DEVICES_CNS].
    Expected: GT_OUT_OF_RANGE.
1.5. Call function with out of range analyzerTc [CPSS_TC_RANGE_CNS].
    Expected: GT_BAD_PARAM for 981x6/8 devices and GT_OK for others.
*/
    GT_STATUS   st = GT_OK;

    GT_U8   dev;
    GT_U8   analyzerPort;
    GT_U8   analyzerDevNum;
    GT_U8   analyzerTc;

    GT_U8   retAnalyzerPort;
    GT_U8   retAnalyzerDevNum;
    GT_U8   retAnalyzerTc;

    CPSS_PP_FAMILY_TYPE_ENT   devFamily;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. Call function with correct values of other parameters:  */
        /* analyzerPort [2], analyzerDevNum [18], analyzerTc[3].        */
        /* Expected: GT_OK.                                             */
        analyzerPort = 2;
        analyzerDevNum = 18;
        analyzerTc = 3;

        st = cpssExMxMirrorTxAnalyzerPortSet(dev, analyzerPort,
                                             analyzerDevNum, analyzerTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, analyzerPort,
                                    analyzerDevNum, analyzerTc);

        /* 1.2. Call function cpssExMxMirrorTxAnalyzerPortGet with
        non NULL pointers. Expected: GT_OK and analyzerPort, analyzerDevNum
        and analyzerTc (tc for 981x6/8) the same as written. */
        st = cpssExMxMirrorTxAnalyzerPortGet(dev, &retAnalyzerPort, &retAnalyzerDevNum, &retAnalyzerTc);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                     "cpssExMxMirrorTxAnalyzerPortGet: %d", dev);
        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(analyzerPort, retAnalyzerPort,
                                         "get another analyzerPort than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(analyzerDevNum, retAnalyzerDevNum,
                                         "get another analyzerDevNum than was set: %d", dev);
            if(CPSS_PP_FAMILY_SAMBA_E == devFamily)
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(analyzerTc, retAnalyzerTc,
                                             "get another analyzerTc than was set: %d", dev);
            }
        }

        /* 1.3. Call function with out of range analyzerPort            */
        /* [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. Expected: GT_OUT_OF_RANGE.  */
        analyzerPort = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxMirrorTxAnalyzerPortSet(dev, analyzerPort,
                                             analyzerDevNum, analyzerTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, analyzerPort,
                                    analyzerDevNum, analyzerTc);

        analyzerPort = 2; /* restore */

        /* 1.4. Call function with out of range analyzerDevNum          */
        /* [PRV_CPSS_MAX_PP_DEVICES_CNS]. Expected: GT_OUT_OF_RANGE.    */
        analyzerDevNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxMirrorTxAnalyzerPortSet(dev, analyzerPort,
                                             analyzerDevNum, analyzerTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, analyzerPort,
                                    analyzerDevNum, analyzerTc);

        analyzerDevNum = 18; /* restore */

        /* 1.5. Call function with out of range analyzerTc          */
        /* [CPSS_TC_RANGE_CNS]. Expected: GT_BAD_PARAM for 981x6/8  */
        /* devices and GT_OK for others                             */
        analyzerTc = CPSS_TC_RANGE_CNS;

        st = cpssExMxMirrorTxAnalyzerPortSet(dev, analyzerPort,
                                             analyzerDevNum, analyzerTc);
        if(CPSS_PP_FAMILY_SAMBA_E == devFamily)
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, analyzerPort,
                                        analyzerDevNum, analyzerTc);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                "non-Samba/Rumba device: %d, %d, %d, %d",
                dev, analyzerPort, analyzerDevNum, analyzerTc);
        }
    }

    analyzerPort = 12;
    analyzerDevNum = 8;
    analyzerTc = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxMirrorTxAnalyzerPortSet(dev, analyzerPort,
                                             analyzerDevNum, analyzerTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, analyzerPort,
                                    analyzerDevNum, analyzerTc);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxMirrorTxAnalyzerPortSet(dev, analyzerPort,
                                         analyzerDevNum, analyzerTc);
    UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, analyzerPort,
                                analyzerDevNum, analyzerTc);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxMirrorTxAnalyzerPortGet
(
    IN   GT_U8   devNum,
    OUT  GT_U8   *analyzerPortPtr,
    OUT  GT_U8   *analyzerDevNumPtr,
    OUT  GT_U8   *analyzerTcPtr
)
*/

UTF_TEST_CASE_MAC(cpssExMxMirrorTxAnalyzerPortGet)
{
/*
ITERATE_DEVICES
1.1. Call function with non-NULL pointers. Expected: GT_OK.
1.2. Call function with analyzerPortPtr [NULL]. Expected: GT_BAD_PTR.
1.3. Call function with analyzerDevNumPtr [NULL]. Expected: GT_BAD_PTR.
1.4. Call function with analyzerTcPtr [NULL]. Expected: GT_BAD_PTR  for 98Mx1x6/8 devices and GT_OK for others.
*/
    GT_STATUS   st = GT_OK;

    GT_U8   dev;
    GT_U8   retAnalyzerPort;
    GT_U8   retAnalyzerDevNum;
    GT_U8   retAnalyzerTc;

    CPSS_PP_FAMILY_TYPE_ENT   devFamily;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. Call function with non-NULL pointers. Expected: GT_OK. */
        st = cpssExMxMirrorTxAnalyzerPortGet(dev, &retAnalyzerPort, &retAnalyzerDevNum, &retAnalyzerTc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with analyzerPortPtr [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxMirrorTxAnalyzerPortGet(dev, NULL, &retAnalyzerDevNum, &retAnalyzerTc);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, analyzerPortPtr = NULL", dev);

        /* 1.3. Call function with analyzerDevNumPtr [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxMirrorTxAnalyzerPortGet(dev, &retAnalyzerPort, NULL, &retAnalyzerTc);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, analyzerDevNumPtr = NULL", dev);

        /* 1.4. Call function with analyzerTcPtr [NULL]. Expected: GT_BAD_PTR for 981x6/8 devices.
        and GT_OK for others*/
        st = cpssExMxMirrorTxAnalyzerPortGet(dev, &retAnalyzerPort, &retAnalyzerDevNum, NULL);
        if(CPSS_PP_FAMILY_SAMBA_E == devFamily)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, analyzerTcPtr = NULL", dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "non-Samba/Rumba device: %d, analyzerTcPtr = NULL", dev);
        }
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxMirrorTxAnalyzerPortGet(dev, &retAnalyzerPort, &retAnalyzerDevNum, &retAnalyzerTc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxMirrorTxAnalyzerPortGet(dev, &retAnalyzerPort, &retAnalyzerDevNum, &retAnalyzerTc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxMirrorRxPortSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  mirrPort
)
*/
UTF_TEST_CASE_MAC(cpssExMxMirrorRxPortSet)
{
/*
ITERATE_DEVICES
1.1. Call function with mirrPort [5].
    Expected: GT_OK.
1.2. Call function cpssExMxMirrorRxPortGet with mirrPort [5]
    and non-NULL mirrorEnPtr. Expected: GT_OK and mirrorEnPtr [GT_TRUE].
1.3. Call function with out of range mirrPort
    [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. Expected: GT_BAD_PARAM.
1.4. Call function with mirrPort = CPU port
    [CPSS_CPU_PORT_NUM_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;

    GT_U8   dev;
    GT_U8   mirrPort;
    GT_BOOL mirrorEn;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with mirrPort [5].    */
        /* Expected: GT_OK.                         */
        mirrPort = 0;

        st = cpssExMxMirrorRxPortSet(dev, mirrPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrPort);

        /* 1.2. Call function cpssExMxMirrorRxPortGet with
        mirrPort [5] and non-NULL mirrorEnPtr.
        Expected: GT_OK and mirrorEnPtr [GT_TRUE]. */
        st = cpssExMxMirrorRxPortGet(dev, mirrPort, &mirrorEn);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrPort);
        if(GT_OK == st)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, mirrorEn,
                "Expected port to be a source RX Sniffer, but it's not", dev, mirrPort);
        }

        /* 1.3. Call function with out of range mirrPort            */
        /* [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. Expected: GT_BAD_PARAM. */
        mirrPort = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxMirrorRxPortSet(dev, mirrPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort);

        /* 1.4. Call function with mirrPort = CPU port      */
        /* [CPSS_CPU_PORT_NUM_CNS]. Expected: GT_BAD_PARAM.    */
        mirrPort = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxMirrorRxPortSet(dev, mirrPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort);
    }

    mirrPort = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxMirrorRxPortSet(dev, mirrPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* mirrPort == 12 */

    st = cpssExMxMirrorRxPortSet(dev, mirrPort);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxMirrorRxPortGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  mirrPort,
    OUT GT_BOOL *mirrorEnPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxMirrorRxPortGet)
{
/*
ITERATE_DEVICES
1.1. Call function with mirrPort [15] and non-NULL mirrorEnPtr. Expected: GT_OK.
1.2. Call function with out of range mirrPort [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] and non-NULL mirrorEnPtr. Expected: GT_BAD_PARAM.
1.3. Call function with mirrPort = CPU port [CPSS_CPU_PORT_NUM_CNS] and non-NULL mirrorEnPtr. Expected: GT_BAD_PARAM.
1.4. Call function with mirrPort [15] and mirrorEnPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8   dev;
    GT_U8   mirrPort;
    GT_BOOL mirrorEn;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with mirrPort [15].   */
        /* Expected: GT_OK.                         */
        mirrPort = 0;

        st = cpssExMxMirrorRxPortGet(dev, mirrPort, &mirrorEn);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrPort);

        /* 1.2. Call function with out of range mirrPort            */
        /* [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. Expected: GT_BAD_PARAM. */
        mirrPort = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxMirrorRxPortGet(dev, mirrPort, &mirrorEn);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort);

        /* 1.3. Call function with mirrPort = CPU port      */
        /* [CPSS_CPU_PORT_NUM_CNS]. Expected: GT_BAD_PARAM. */
        mirrPort = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxMirrorRxPortGet(dev, mirrPort, &mirrorEn);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort);

        /* 1.4. Call function with mirrPort [15] and mirrorEnPtr [NULL].
        Expected: GT_BAD_PTR. */
        mirrPort = 0;

        st = cpssExMxMirrorRxPortGet(dev, mirrPort, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, mirrPort);
    }

    mirrPort = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxMirrorRxPortGet(dev, mirrPort, &mirrorEn);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* mirrPort == 15 */

    st = cpssExMxMirrorRxPortGet(dev, mirrPort, &mirrorEn);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxMirrorTxPortSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   mirrPort,
    IN  GT_U8   mirrDev
)
*/
UTF_TEST_CASE_MAC(cpssExMxMirrorTxPortSet)
{
/*
ITERATE_DEVICES
1.1. Call function with mirrPort
    [from 0 to PRV_CPSS_MIRROR_MAX_PORTS_CNS - 2] and mirrDev [0].
    Expected: GT_OK.
1.2. Call function with mirrPort = CPU port
    [CPSS_CPU_PORT_NUM_CNS] while mirrDev [2]. Expected: GT_OK.
1.3. Call function cpssExMxMirrorTxPortsGet with non-NULL mirrorPortsArray,
    mirrorDevsArray and numOfPorts. Expected: GT_OK and numOfPorts = 8,
    mirrorDevsArray = {0,0,0,0,0,0,0,2},
    mirrorPortsArray = {0,1,2,3,4,5,6, CPSS_CPU_PORT_NUM_CNS}.
1.4. Call function with mirrPort [PRV_CPSS_MIRROR_MAX_PORTS_CNS]
    and mirrDev [0]. Expected: GT_FULL, because we've set already
    maximum available number of Tx ports
    [PRV_CPSS_MIRROR_MAX_PORTS_CNS-1] for mirrDev == 0.
1.5. Call function cpssExMxMirrorTxPortDel with mirrDev[0] and
    mirrPort [from 0 to  PRV_CPSS_MIRROR_MAX_PORTS_CNS-1] and
    mirrDev [2] and mirrPort [CPSS_CPU_PORT_NUM_CNS] to delete
    all ports that was set from the set of Tx mirrored ports. Expected: GT_OK.
1.6. Call function cpssExMxMirrorTxPortsGet with non-NULL mirrorPortsArray,
    mirrorDevsArray and numOfPorts. Expected: GT_OK and numOfPorts = 0.
1.7. Call function with out of range mirrPort
    [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. Expected: GT_BAD_PARAM.
1.8. Call function with out of range mirrDev
    [PRV_CPSS_MAX_PP_DEVICES_CNS]. Expected: GT_OUT_OF_RANGE.

*/
    GT_STATUS   st = GT_OK;

    GT_U8   dev;
    GT_U8   mirrPort;
    GT_U8   mirrDev;

    GT_U8   mirrorPortsArray [PRV_CPSS_MIRROR_MAX_PORTS_CNS];
    GT_U8   mirrorDevsArray [PRV_CPSS_MIRROR_MAX_PORTS_CNS];
    GT_U32  numOfPorts;
    GT_U8   numOfVirtPorts;
    GT_U8   numOfMirrPorts;
    GT_U8   i;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNumVirtualPortsGet(dev, &numOfVirtPorts);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfNumVirtualPortsGet: %d", dev);

        numOfMirrPorts = ((PRV_CPSS_MIRROR_MAX_PORTS_CNS - 1) < numOfVirtPorts) ? (PRV_CPSS_MIRROR_MAX_PORTS_CNS - 1) : numOfVirtPorts;

        /* 1.1. Call function with mirrPort                               */
        /* [from 0 to PRV_CPSS_MIRROR_MAX_PORTS_CNS-2] and mirrDev [0].   */
        /* Expected: GT_OK.                                               */
        
        mirrDev = 0;
        for (mirrPort = 0; mirrPort < numOfMirrPorts; ++mirrPort)
        {
            st = cpssExMxMirrorTxPortSet(dev, mirrPort, mirrDev);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mirrPort, mirrDev);
        }

        /* 1.2. Call function with mirrPort = CPU port                  */
        /* [CPSS_CPU_PORT_NUM_CNS] while mirrDev [2]. Expected: GT_OK.  */
        mirrPort = CPSS_CPU_PORT_NUM_CNS;
        mirrDev = 2;

        st = cpssExMxMirrorTxPortSet(dev, mirrPort, mirrDev);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mirrPort, mirrDev);

        /* 1.3. Call function cpssExMxMirrorTxPortsGet with non-NULL mirrorPortsArray, */
        /* mirrorDevsArray and numOfPorts. Expected: GT_OK and numOfPorts = 8,         */
        /* mirrorDevsArray = {0,0,0,0,0,0,0,2},                                        */
        /* mirrorPortsArray = {0,1,2,3,4,5,6, CPSS_CPU_PORT_NUM_CNS}.                  */
        st = cpssExMxMirrorTxPortsGet(dev, mirrorPortsArray, mirrorDevsArray, &numOfPorts);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxMirrorTxPortsGet: %d", dev);
        if(GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(numOfMirrPorts + 1, numOfPorts,
                                         "get another numOfPorts than expected: %d", dev);

            for(i = 0; i < numOfMirrPorts; ++i)
            {
                if ((0 != mirrorDevsArray [i]) || (i != mirrorPortsArray [i]))
                {
                    break;
                }
            }
            if ( (i < numOfMirrPorts) ||
                 (2 != mirrorDevsArray [numOfMirrPorts]) ||
                 (CPSS_CPU_PORT_NUM_CNS != mirrorPortsArray [numOfMirrPorts]))
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(numOfMirrPorts + 1, numOfPorts,
                    "get another mirrorDevsArray or mirrorPortsArray than was set: %d", dev);
            }
        }

        /* 1.4. Call function with mirrPort [PRV_CPSS_MIRROR_MAX_PORTS_CNS]     */
        /* and mirrDev [0]. Expected: GT_FULL, because we've set already        */
        /* maximum available number of Tx ports                                 */
        /* [PRV_CPSS_MIRROR_MAX_PORTS_CNS-1] for mirrDev == 0.                  */
        if (PRV_CPSS_MIRROR_MAX_PORTS_CNS < numOfPorts)
        {
            mirrPort = PRV_CPSS_MIRROR_MAX_PORTS_CNS - 1;
            mirrDev = 0;

            st = cpssExMxMirrorTxPortSet(dev, mirrPort, mirrDev);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_FULL, st, dev, mirrPort, mirrDev);
        }
        
        /* 1.5. Call function cpssExMxMirrorTxPortDel with mirrDev[0]       */
        /* and mirrPort [from 0 to  PRV_CPSS_MIRROR_MAX_PORTS_CNS-1] and    */
        /* mirrDev [2] and mirrPort [CPSS_CPU_PORT_NUM_CNS] to delete all   */
        /* ports that was set from the set of Tx mirrored ports.            */
        /* Expected: GT_OK.                                                 */
        mirrDev = 0;
        for ( mirrPort = 0; mirrPort < numOfMirrPorts; ++mirrPort)
        {
            st = cpssExMxMirrorTxPortDel(dev, mirrPort, mirrDev);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxMirrorTxPortDel: %d, %d, %d",
                                         dev, mirrPort, mirrDev);
        }
        mirrDev = 2;
        mirrPort = CPSS_CPU_PORT_NUM_CNS;
        st = cpssExMxMirrorTxPortDel(dev, mirrPort, mirrDev);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxMirrorTxPortDel: %d, %d, %d",
                                     dev, mirrPort, mirrDev);

        /* 1.6. Call function cpssExMxMirrorTxPortsGet with non-NULL mirrorPortsArray,
        mirrorDevsArray and numOfPorts. Expected: GT_OK and numOfPorts = 0. */
        st = cpssExMxMirrorTxPortsGet(dev, mirrorPortsArray, mirrorDevsArray, &numOfPorts);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxMirrorTxPortsGet: %d", dev);
        if(GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(0, numOfPorts,
                                         "get another numOfPorts than expected: %d", dev);
        }

        /* 1.7. Call function with out of range mirrPort                    */
        /* [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. Expected: GT_BAD_PARAM.         */
        mirrPort = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxMirrorTxPortSet(dev, mirrPort, mirrDev);
        if(dev == mirrDev)
        {
            /* when the specific device is tested , the device can check exactly
               the type of error*/
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort, mirrDev);
        }
        else
        {
            /* when the other device is tested , the device can't check exactly
               the type of error , and check range of bits in HW */
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, mirrPort, mirrDev);
        }

        mirrPort = 0; /* restore */

        /* 1.8. Call function with out of range mirrDev                     */
        /* [PRV_CPSS_MAX_PP_DEVICES_CNS]. Expected: GT_OUT_OF_RANGE.        */
        mirrDev = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxMirrorTxPortSet(dev, mirrPort, mirrDev);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, mirrPort, mirrDev);
    }

    mirrPort = 0;
    mirrDev = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxMirrorTxPortSet(dev, mirrPort, mirrDev);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort, mirrDev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* mirrPort == 0 */
    /* mirrDev == 0*/

    st = cpssExMxMirrorTxPortSet(dev, mirrPort, mirrDev);
    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort, mirrDev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxMirrorTxPortsGet
(
    IN   GT_U8         devNum,
    OUT  GT_U8         *mirrorPortsArray,
    OUT  GT_U8         *mirrorDevsArray,
    OUT  GT_U32        *numOfPorts
)
*/
UTF_TEST_CASE_MAC(cpssExMxMirrorTxPortsGet)
{
/*
ITERATE_DEVICES
1.1. Call function with non-NULL mirrorPortsArray, mirrorDevsArray and numOfPorts. Expected: GT_OK.
1.2. Call function with mirrorPortsArray [NULL]. Expected: GT_BAD_PTR.
1.3. Call function with mirrorDevsArray [NULL]. Expected: GT_BAD_PTR.
1.4. Call function with numOfPorts [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8   dev;
    GT_U8   mirrorPortsArray [PRV_CPSS_MIRROR_MAX_PORTS_CNS];
    GT_U8   mirrorDevsArray [PRV_CPSS_MIRROR_MAX_PORTS_CNS];
    GT_U32  numOfPorts;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL mirrorPortsArray, mirrorDevsArray and numOfPorts. Expected: GT_OK. */
        st = cpssExMxMirrorTxPortsGet(dev, mirrorPortsArray, mirrorDevsArray, &numOfPorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with mirrorPortsArray [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxMirrorTxPortsGet(dev, NULL, mirrorDevsArray, &numOfPorts);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mirrorPortsArray = NULL", dev);

        /* 1.3. Call function with mirrorDevsArray [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxMirrorTxPortsGet(dev, mirrorPortsArray, NULL, &numOfPorts);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mirrorDevsArray = NULL", dev);

        /* 1.4. Call function with numOfPorts [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxMirrorTxPortsGet(dev, mirrorPortsArray, mirrorDevsArray, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfPortsPtr = NULL", dev);
    }


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxMirrorTxPortsGet(dev, mirrorPortsArray, mirrorDevsArray, &numOfPorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxMirrorTxPortsGet(dev, mirrorPortsArray, mirrorDevsArray, &numOfPorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/

/*
GT_STATUS cpssExMxMirrorRxPortDel
(
    IN  GT_U8   devNum,
    IN  GT_U8   mirrPort
)
*/
UTF_TEST_CASE_MAC(cpssExMxMirrorRxPortDel)
{
/*
ITERATE_DEVICES
1.1. Call function with mirrPort [3].
    Expected: GT_OK.
1.2. Call function with out of range mirrPort
    [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. Expected: GT_BAD_PARAM.
1.3. Call function with mirrPort = CPU port
    [CPSS_CPU_PORT_NUM_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;

    GT_U8   dev;
    GT_U8   mirrPort;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with mirrPort [3].    */
        /* Expected: GT_OK.                         */
        mirrPort = 0;

        st = cpssExMxMirrorRxPortDel(dev, mirrPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mirrPort);

        /* 1.2. Call function with out of range mirrPort             */
        /* [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. Expected: GT_BAD_PARAM.  */
        mirrPort = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxMirrorRxPortDel(dev, mirrPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort);

        /* 1.3. Call function with mirrPort = CPU port      */
        /* [CPSS_CPU_PORT_NUM_CNS]. Expected: GT_BAD_PARAM. */
        mirrPort = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxMirrorRxPortDel(dev, mirrPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort);
    }

    mirrPort = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxMirrorRxPortDel(dev, mirrPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* mirrPort == 4 */

    st = cpssExMxMirrorRxPortDel(dev, mirrPort);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxMirrorTxPortDel
(
    IN  GT_U8   devNum,
    IN  GT_U8   mirrPort,
    IN  GT_U8   mirrDev
)
*/
UTF_TEST_CASE_MAC(cpssExMxMirrorTxPortDel)
{
/*
ITERATE_DEVICES
1.1. Call function cpssExMxMirrorTxPortSet
    with mirrPort [0 and CPSS_CPU_PORT_NUM_CNS]
    and mirrDev [0]. Expected: GT_OK.
1.2. Call this function with mirrDev [0] and
    mirrPort [0]. Expected: GT_OK.
1.3. Call this function with mirrDev [0] and
    mirrPort [CPSS_CPU_PORT_NUM_CNS]. Expected: GT_OK.
1.4. Call this function one more time with mirrDev [0]
    and mirrPort [0 and CPSS_CPU_PORT_NUM_CNS].
    Expected: GT_OK (because it is ok to "remove"
    configuration from port that do'nt have this configuration.
1.5. Call function with out of range mirrPort
    [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. Expected: GT_BAD_PARAM.
1.6. Call function with out of range mirrDev
    [PRV_CPSS_MAX_PP_DEVICES_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;

    GT_U8   dev;
    GT_U8   mirrPort;
    GT_U8   mirrDev;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        mirrDev = 0;

        /* 1.1. Call function cpssExMxMirrorTxPortSet   */
        /* with mirrPort [0 and CPSS_CPU_PORT_NUM_CNS]  */
        /* and mirrDev [0]. Expected: GT_OK.            */
        mirrPort = 0;
        st = cpssExMxMirrorTxPortSet(dev, mirrPort, mirrDev);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxMirrorTxPortSet: %d, %d, %d",
                                     dev, mirrPort, mirrDev);

        mirrPort = CPSS_CPU_PORT_NUM_CNS;
        st = cpssExMxMirrorTxPortSet(dev, mirrPort, mirrDev);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxMirrorTxPortSet: %d, %d, %d",
                                     dev, mirrPort, mirrDev);

        /* 1.2. Call this function with mirrDev [0] and         */
        /* mirrPort [0]. Expected: GT_OK.                       */
        mirrPort = 0;
        st = cpssExMxMirrorTxPortDel(dev, mirrPort, mirrDev);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mirrPort, mirrDev);

        /* 1.3. Call this function with mirrDev [0] and         */
        /* mirrPort [CPSS_CPU_PORT_NUM_CNS]. Expected: GT_OK.   */
        mirrPort = CPSS_CPU_PORT_NUM_CNS;
        st = cpssExMxMirrorTxPortDel(dev, mirrPort, mirrDev);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mirrPort, mirrDev);

        /* 1.4. Call this function one more time with mirrDev [0]   */
        /* and mirrPort [0 and CPSS_CPU_PORT_NUM_CNS].              */
        /* Expected: GT_OK (because it is ok to "remove"            */
        /* configuration from port that do'nt have this             */
        /* configuration                                            */
        mirrPort = 0;
        st = cpssExMxMirrorTxPortDel(dev, mirrPort, mirrDev);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mirrPort, mirrDev);

        mirrPort = CPSS_CPU_PORT_NUM_CNS;
        st = cpssExMxMirrorTxPortDel(dev, mirrPort, mirrDev);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mirrPort, mirrDev);

        /* 1.5. Call function with out of range mirrPort                    */
        /* [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. Expected: GT_BAD_PARAM.         */
        mirrPort = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxMirrorTxPortDel(dev, mirrPort, mirrDev);
        if(dev == mirrDev)
        {
            /* when the specific device is tested , the device can check exactly
               the type of error*/
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort, mirrDev);
        }
        else
        {
            /* when the other device is tested , the device can't check exactly
               the type of error , and check range of bits in HW */
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, mirrPort, mirrDev);
        }

        mirrPort = 0; /* restore */

        /* 1.6. Call function with out of range mirrDev                     */
        /* [PRV_CPSS_MAX_PP_DEVICES_CNS]. Expected: GT_BAD_PARAM.           */
        mirrDev = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxMirrorTxPortDel(dev, mirrPort, mirrDev);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, mirrPort, mirrDev);
    }

    mirrPort = 0;
    mirrDev = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxMirrorTxPortDel(dev, mirrPort, mirrDev);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort, mirrDev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* mirrPort == 0 */
    /* mirrDev == 0*/

    st = cpssExMxMirrorTxPortDel(dev, mirrPort, mirrDev);
    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, mirrPort, mirrDev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxMirrorCpuRxMirrorParamsSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable,
    IN  GT_U8       mirrorTc
)
*/
UTF_TEST_CASE_MAC(cpssExMxMirrorCpuRxMirrorParamsSet)
{
/*
ITERATE_DEVICES
1.1. Call function with mirrorTc [3/ 7] and
    enable [GT_TRUE and GT_FALSE]. Expected: GT_OK.
1.2. Call function cpssExMxMirrorCpuRxMirrorParamsGet
    with non-NULL mirrorTcPtr and enablePtr.
    Expected: GT_OK and mirrorTc and enable the same as have just been written.
1.3. Call function with out of range mirrorTc
    [CPSS_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;

    GT_U8   dev;
    GT_BOOL enable;
    GT_U8   mirrTc;

    GT_BOOL retEnable;
    GT_U8   retMirrTc;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with mirrorTc [3/ 7] and         */
        /* enable [GT_TRUE and GT_FALSE]. Expected: GT_OK.  */

        /* 1.2. Call function cpssExMxMirrorCpuRxMirrorParamsGet    */
        /* with non-NULL mirrorTcPtr and enablePtr.                 */
        /* Expected: GT_OK and mirrorTc and enable the same as      */
        /* have just been written.                                  */

        /* 1.1. for mirrTc = 7, enable =GT_FALSE */
        mirrTc = 7;
        enable = GT_FALSE;

        st = cpssExMxMirrorCpuRxMirrorParamsSet(dev, enable, mirrTc);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, mirrTc);

        /* 1.2. for mirrTc = 7, enable =GT_FALSE */
        st = cpssExMxMirrorCpuRxMirrorParamsGet(dev, &retEnable, &retMirrTc);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "cpssExMxMirrorCpuRxMirrorParamsGet: %d", dev);
        if(GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, retEnable,
                "get another enable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(mirrTc, retMirrTc,
                "get another mirrTc than was set: %d", dev);
        }

        /* 1.1. for mirrTc = 3, enable =GT_TRUE */
        mirrTc = 3;
        enable = GT_TRUE;

        st = cpssExMxMirrorCpuRxMirrorParamsSet(dev, enable, mirrTc);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, enable, mirrTc);

        /* 1.2. for mirrTc = 3, enable =GT_TRUE */
        st = cpssExMxMirrorCpuRxMirrorParamsGet(dev, &retEnable, &retMirrTc);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "cpssExMxMirrorCpuRxMirrorParamsGet: %d", dev);
        if(GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, retEnable,
                "get another enable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(mirrTc, retMirrTc,
                "get another mirrTc than was set: %d", dev);
        }

        /* 1.3. Call function with out of range mirrorTc    */
        /* [CPSS_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.        */
        mirrTc = CPSS_TC_RANGE_CNS;
        /* enbale == GT_TRUE */

        st = cpssExMxMirrorCpuRxMirrorParamsSet(dev, enable, mirrTc);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, enable, mirrTc);
    }

    mirrTc = 1;
    enable = GT_TRUE;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxMirrorCpuRxMirrorParamsSet(dev, enable, mirrTc);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, enable, mirrTc);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* mirrTc == 1          */
    /* enable == GT_TRUE    */

    st = cpssExMxMirrorCpuRxMirrorParamsSet(dev, enable, mirrTc);
    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, enable, mirrTc);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxMirrorCpuRxMirrorParamsGet
(
    IN   GT_U8       devNum,
    OUT  GT_BOOL     *enablePtr,
    OUT  GT_U8       *mirrorTcPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxMirrorCpuRxMirrorParamsGet)
{
/*
ITERATE_DEVICES
1.1. Call function with non-NULL mirrorTcPtr and enablePtr. Expected: GT_OK.
1.2. Call function with mirrorTcPtr [NULL]. Expected: GT_BAD_PTR.
1.3. Call function with enablePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8   dev;
    GT_BOOL retEnable;
    GT_U8   retMirrTc;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL mirrorTcPtr and enablePtr. Expected: GT_OK. */
        st = cpssExMxMirrorCpuRxMirrorParamsGet(dev, &retEnable, &retMirrTc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with mirrorTcPtr [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxMirrorCpuRxMirrorParamsGet(dev, NULL, &retMirrTc);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);

        /* 1.3. Call function with enablePtr [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxMirrorCpuRxMirrorParamsGet(dev, &retEnable, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mirrTcPtr = NULL", dev);
    }


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxMirrorCpuRxMirrorParamsGet(dev, &retEnable, &retMirrTc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxMirrorCpuRxMirrorParamsGet(dev, &retEnable, &retMirrTc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cppExMxMirror suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxMirror)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxMirrorRxAnalyzerPortSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxMirrorRxAnalyzerPortGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxMirrorTxAnalyzerPortSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxMirrorTxAnalyzerPortGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxMirrorRxPortSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxMirrorRxPortGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxMirrorTxPortSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxMirrorTxPortsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxMirrorRxPortDel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxMirrorTxPortDel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxMirrorCpuRxMirrorParamsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxMirrorCpuRxMirrorParamsGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxMirror)
