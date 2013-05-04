/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmFabricVoqUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmFabricVoq, that provides
*       CPSS ExMxPm Fabric Connectivity Virtual Output Queue (VOQs) APIs.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricVoq.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define FABRIC_VOQ_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqFportTxEnableSet
(
    IN GT_U8      devNum,
    IN GT_U32     voqUnitNum,
    IN GT_FPORT   fport,
    IN GT_BOOL    enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqFportTxEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   fport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS]
                   and enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricVoqFportTxEnableGet with the same voqUnitNum,
                                                       fport
                                                       and non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.3. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range fport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    GT_FPORT    fport      = 0;
    GT_BOOL     enable     = GT_FALSE;
    GT_BOOL     enableGet  = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           fport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS]
                           and enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;
        fport      = 0;
        enable     = GT_FALSE;

        st = cpssExMxPmFabricVoqFportTxEnableSet(dev, voqUnitNum, fport, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqUnitNum, fport, enable);

        /*
            1.2. Call cpssExMxPmFabricVoqFportTxEnableGet with the same voqUnitNum,
                                                               fport
                                                               and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmFabricVoqFportTxEnableGet(dev, voqUnitNum, fport, &enableGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqFportTxEnableGet: %d, %d, %d", dev, voqUnitNum, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;
        fport      = CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS;
        enable     = GT_TRUE;

        st = cpssExMxPmFabricVoqFportTxEnableSet(dev, voqUnitNum, fport, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqUnitNum, fport, enable);

        /*
            1.2. Call cpssExMxPmFabricVoqFportTxEnableGet with the same voqUnitNum,
                                                               fport
                                                               and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmFabricVoqFportTxEnableGet(dev, voqUnitNum, fport, &enableGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqFportTxEnableGet: %d, %d, %d", dev, voqUnitNum, fport);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /*
            1.3. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqFportTxEnableSet(dev, voqUnitNum, fport, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with out of range fport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS;

        st = cpssExMxPmFabricVoqFportTxEnableSet(dev, voqUnitNum, fport, enable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);
    }

    voqUnitNum = 0;
    fport      = 0;
    enable     = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqFportTxEnableSet(dev, voqUnitNum, fport, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqFportTxEnableSet(dev, voqUnitNum, fport, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqFportTxEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U32     voqUnitNum,
    IN  GT_FPORT   fport,
    OUT GT_BOOL    *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqFportTxEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   fport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS]
                   and non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range fport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with enablePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    GT_FPORT    fport      = 0;
    GT_BOOL     enable     = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           fport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS]
                           and non-NULL enablePtr.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;
        fport      = 0;

        st = cpssExMxPmFabricVoqFportTxEnableGet(dev, voqUnitNum, fport, &enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, fport);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;
        fport      = CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS;

        st = cpssExMxPmFabricVoqFportTxEnableGet(dev, voqUnitNum, fport, &enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, fport);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqFportTxEnableGet(dev, voqUnitNum, fport, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with out of range fport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS;

        st = cpssExMxPmFabricVoqFportTxEnableGet(dev, voqUnitNum, fport, &enable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d",  dev, fport);

        fport = 0;

        /*
            1.4. Call with enablePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqFportTxEnableGet(dev, voqUnitNum, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    voqUnitNum = 0;
    fport      = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqFportTxEnableGet(dev, voqUnitNum, fport, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqFportTxEnableGet(dev, voqUnitNum, fport, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqDevTblSet
(
    IN GT_U8    devNum,
    IN GT_U32   voqUnitNum,
    IN GT_U32   trgDevsBmpArr[4]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqDevTblSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1]
                   and trgDevsBmpArr [0, 0 / 0xFFFFFFFF, 0xFFFFFFFF].
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with trgDevsBmpArr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    GT_U32      trgDevsBmpArr[4];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1]
                           and trgDevsBmpArr [0, 0 / 0xFFFFFFFF, 0xFFFFFFFF].
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;

        trgDevsBmpArr[0] = 0;
        trgDevsBmpArr[1] = 0;
        trgDevsBmpArr[2] = 0;
        trgDevsBmpArr[3] = 0;

        st = cpssExMxPmFabricVoqDevTblSet(dev, voqUnitNum, trgDevsBmpArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;

        trgDevsBmpArr[0] = 0xFFFFFFFF;
        trgDevsBmpArr[1] = 0xFFFFFFFF;
        trgDevsBmpArr[2] = 0xFFFFFFFF;
        trgDevsBmpArr[3] = 0xFFFFFFFF;

        st = cpssExMxPmFabricVoqDevTblSet(dev, voqUnitNum, trgDevsBmpArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqDevTblSet(dev, voqUnitNum, trgDevsBmpArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with trgDevsBmpArr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqDevTblSet(dev, voqUnitNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, trgDevsBmpArr = NULL", dev);
    }

    voqUnitNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqDevTblSet(dev, voqUnitNum, trgDevsBmpArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqDevTblSet(dev, voqUnitNum, trgDevsBmpArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqDevEnableSet
(
    IN GT_U8    devNum,
    IN GT_U32   voqUnitNum,
    IN GT_U8    trgDev,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqDevEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   trgDev [0 / 127]
                   and enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricVoqDevEnableGet with the same voqUnitNum,
                                                   trgDev
                                                   and non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.3. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range trgDev [128]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    GT_U8       trgDev     = 0;
    GT_BOOL     enable     = GT_FALSE;
    GT_BOOL     enableGet  = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           trgDev [0 / 127]
                           and enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;
        trgDev     = 0;
        enable     = GT_FALSE;

        st = cpssExMxPmFabricVoqDevEnableSet(dev, voqUnitNum, trgDev, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqUnitNum, trgDev, enable);

        /*
            1.2. Call cpssExMxPmFabricVoqDevEnableGet with the same voqUnitNum,
                                                           trgDev
                                                           and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmFabricVoqDevEnableGet(dev, voqUnitNum, trgDev, &enableGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqDevEnableGet: %d, %d, %d", dev, voqUnitNum, trgDev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;
        trgDev     = 127;
        enable     = GT_TRUE;

        st = cpssExMxPmFabricVoqDevEnableSet(dev, voqUnitNum, trgDev, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqUnitNum, trgDev, enable);

        /*
            1.2. Call cpssExMxPmFabricVoqDevEnableGet with the same voqUnitNum,
                                                           trgDev
                                                           and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmFabricVoqDevEnableGet(dev, voqUnitNum, trgDev, &enableGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqDevEnableGet: %d, %d, %d", dev, voqUnitNum, trgDev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /*
            1.3. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqDevEnableSet(dev, voqUnitNum, trgDev, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with out of range trgDev [128]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        trgDev = 128;

        st = cpssExMxPmFabricVoqDevEnableSet(dev, voqUnitNum, trgDev, enable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, trgDev = %d", dev, trgDev);
    }

    voqUnitNum = 0;
    trgDev     = 0;
    enable     = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqDevEnableSet(dev, voqUnitNum, trgDev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqDevEnableSet(dev, voqUnitNum, trgDev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqDevEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   voqUnitNum,
    IN  GT_U8    trgDev,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqDevEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   trgDev [0 / 127]
                   and non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range trgDev [128]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with enablePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    GT_U8       trgDev     = 0;
    GT_BOOL     enable     = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           trgDev [0 / 127]
                           and non-NULL enablePtr.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;
        trgDev     = 0;

        st = cpssExMxPmFabricVoqDevEnableGet(dev, voqUnitNum, trgDev, &enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, trgDev);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;
        trgDev     = 127;

        st = cpssExMxPmFabricVoqDevEnableGet(dev, voqUnitNum, trgDev, &enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, trgDev);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqDevEnableGet(dev, voqUnitNum, trgDev, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with out of range trgDev [128]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        trgDev = 128;

        st = cpssExMxPmFabricVoqDevEnableGet(dev, voqUnitNum, trgDev, &enable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, trgDev = %d", dev, trgDev);

        trgDev = 0;

        /*
            1.4. Call with enablePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqDevEnableGet(dev, voqUnitNum, trgDev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, trgDevsBmpArr = NULL", dev);
    }

    voqUnitNum = 0;
    trgDev     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqDevEnableGet(dev, voqUnitNum, trgDev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqDevEnableGet(dev, voqUnitNum, trgDev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqDevToFportMapSet
(
    IN GT_U8     devNum,
    IN GT_U32    voqUnitNum,
    IN GT_U8     trgDev,
    IN GT_FPORT  fportsArr[CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqDevToFportMapSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   trgDev [0 / 127]
                   and fportsArr [0, 0, 0, 0 / 23, 23, 23, 23].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricVoqDevToFportMapGet with the same voqUnitNum,
                                                       trgDev
                                                       and non-NULL fportsArr.
    Expected: GT_OK and the same fportsArr.
    1.3. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range trgDev [128]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range fportsArr [0][24]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with fportsArr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    GT_U8       trgDev     = 0;
    GT_FPORT    fportsArr   [CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS];
    GT_FPORT    fportsArrGet[CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS];
    GT_U8       arrIter    = 0;
    GT_BOOL     isEqual    = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           trgDev [0 / 127]
                           and fportsArr [0, 0, 0, 0 / 63, 63, 63, 63].
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;
        trgDev     = 0;

        for (arrIter = 0; arrIter < CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS; arrIter++)
        {
            fportsArr[arrIter] = 0;
        }

        st = cpssExMxPmFabricVoqDevToFportMapSet(dev, voqUnitNum, trgDev, fportsArr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, trgDev);

        /*
            1.2. Call cpssExMxPmFabricVoqDevToFportMapGet with the same voqUnitNum,
                                                               trgDev
                                                               and non-NULL fportsArr.
            Expected: GT_OK and the same fportsArr.
        */
        st = cpssExMxPmFabricVoqDevToFportMapGet(dev, voqUnitNum, trgDev, fportsArrGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqDevToFportMapGet: %d, %d, %d", dev, voqUnitNum, trgDev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) fportsArr,
                                     (GT_VOID*) fportsArrGet,
                                     sizeof(fportsArr[0]*CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another fportsArr than was set: %d", dev);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;
        trgDev     = 127;

        for (arrIter = 0; arrIter < CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS; arrIter++)
        {
            fportsArr[arrIter] = 23;
        }

        st = cpssExMxPmFabricVoqDevToFportMapSet(dev, voqUnitNum, trgDev, fportsArr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, trgDev);

        /*
            1.2. Call cpssExMxPmFabricVoqDevToFportMapGet with the same voqUnitNum,
                                                               trgDev
                                                               and non-NULL fportsArr.
            Expected: GT_OK and the same fportsArr.
        */
        st = cpssExMxPmFabricVoqDevToFportMapGet(dev, voqUnitNum, trgDev, fportsArrGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqDevToFportMapGet: %d, %d, %d", dev, voqUnitNum, trgDev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) fportsArr,
                                     (GT_VOID*) fportsArrGet,
                                     sizeof(fportsArr[0]*CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another fportsArr than was set: %d", dev);

        /*
            1.3. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqDevToFportMapSet(dev, voqUnitNum, trgDev, fportsArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with out of range trgDev [128]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        trgDev = 128;

        st = cpssExMxPmFabricVoqDevToFportMapSet(dev, voqUnitNum, trgDev, fportsArr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, trgDev = %d", dev, trgDev);

        trgDev = 0;

        /*
            1.5. Call with out of range fportsArr [0][64]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fportsArr[0] = 24;

        st = cpssExMxPmFabricVoqDevToFportMapSet(dev, voqUnitNum, trgDev, fportsArr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fportsArr[0] = %d", dev, fportsArr[0]);

        fportsArr[0] = 0;

        /*
            1.6. Call with fportsArr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqDevToFportMapSet(dev, voqUnitNum, trgDev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fportsArr = NULL", dev);
    }

    voqUnitNum = 0;
    trgDev     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqDevToFportMapSet(dev, voqUnitNum, trgDev, fportsArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqDevToFportMapSet(dev, voqUnitNum, trgDev, fportsArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqDevToFportMapGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    voqUnitNum,
    IN  GT_U8     trgDev,
    OUT GT_FPORT  fportsArr[CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqDevToFportMapGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   trgDev [0 / 127]
                   and non-NULL fportsArr.
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range trgDev [128]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with fportsArr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    GT_U8       trgDev     = 0;
    GT_FPORT    fportsArr[CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           trgDev [0 / 127]
                           and non-NULL fportsArr.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;
        trgDev     = 0;

        st = cpssExMxPmFabricVoqDevToFportMapGet(dev, voqUnitNum, trgDev, fportsArr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, trgDev);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;
        trgDev     = 127;

        st = cpssExMxPmFabricVoqDevToFportMapGet(dev, voqUnitNum, trgDev, fportsArr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, trgDev);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqDevToFportMapGet(dev, voqUnitNum, trgDev, fportsArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with out of range trgDev [128]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        trgDev = 128;

        st = cpssExMxPmFabricVoqDevToFportMapGet(dev, voqUnitNum, trgDev, fportsArr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, trgDev = %d", dev, trgDev);

        trgDev = 0;

        /*
            1.4. Call with fportsArr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqDevToFportMapGet(dev, voqUnitNum, trgDev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fportsArr = NULL", dev);
    }

    voqUnitNum = 0;
    trgDev     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqDevToFportMapGet(dev, voqUnitNum, trgDev, fportsArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqDevToFportMapGet(dev, voqUnitNum, trgDev, fportsArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqDevToConfigSetMapSet
(
    IN GT_U8     devNum,
    IN GT_U32    voqUnitNum,
    IN GT_U8     trgDev,
    IN GT_U32    configSetIndexArr[CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqDevToConfigSetMapSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   trgDev [0 / 127]
                   and configSetIndexArr [0, 1, 2, 3 / 3, 3, 3, 3].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricVoqDevToConfigSetMapGet with the same voqUnitNum,
                                                           trgDev
                                                           and non-NULL configSetIndexArr.
    Expected: GT_OK and the same configSetIndexArr.
    1.3. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range trgDev [128]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range configSetIndexArr[0] [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with configSetIndexArr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    GT_U8       trgDev     = 0;
    GT_U32      configSetIndexArr   [CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS];
    GT_U32      configSetIndexArrGet[CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS];
    GT_BOOL     isEqual    = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           trgDev [0 / 127]
                           and configSetIndexArr [0, 1, 2, 3 / 3, 3, 3, 3].
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;
        trgDev     = 0;

        configSetIndexArr[0] = 0;
        configSetIndexArr[1] = 1;
        configSetIndexArr[2] = 2;
        configSetIndexArr[3] = 3;

        st = cpssExMxPmFabricVoqDevToConfigSetMapSet(dev, voqUnitNum, trgDev, configSetIndexArr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, trgDev);

        /*
            1.2. Call cpssExMxPmFabricVoqDevToConfigSetMapGet with the same voqUnitNum,
                                                                   trgDev
                                                                   and non-NULL configSetIndexArr.
            Expected: GT_OK and the same configSetIndexArr.
        */
        st = cpssExMxPmFabricVoqDevToConfigSetMapGet(dev, voqUnitNum, trgDev, configSetIndexArrGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqDevToConfigSetMapGet: %d, %d, %d", dev, voqUnitNum, trgDev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) configSetIndexArr,
                                     (GT_VOID*) configSetIndexArrGet,
                                     sizeof(configSetIndexArr[0]*CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another configSetIndexArr than was set: %d", dev);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;
        trgDev     = 127;

        configSetIndexArr[0] = 3;
        configSetIndexArr[1] = 3;
        configSetIndexArr[2] = 3;
        configSetIndexArr[3] = 3;

        st = cpssExMxPmFabricVoqDevToConfigSetMapSet(dev, voqUnitNum, trgDev, configSetIndexArr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, trgDev);

        /*
            1.2. Call cpssExMxPmFabricVoqDevToConfigSetMapGet with the same voqUnitNum,
                                                                   trgDev
                                                                   and non-NULL configSetIndexArr.
            Expected: GT_OK and the same configSetIndexArr.
        */
        st = cpssExMxPmFabricVoqDevToConfigSetMapGet(dev, voqUnitNum, trgDev, configSetIndexArrGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqDevToConfigSetMapGet: %d, %d, %d", dev, voqUnitNum, trgDev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) configSetIndexArr,
                                     (GT_VOID*) configSetIndexArrGet,
                                     sizeof(configSetIndexArr[0]*CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another configSetIndexArr than was set: %d", dev);

        /*
            1.3. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqDevToConfigSetMapSet(dev, voqUnitNum, trgDev, configSetIndexArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with out of range trgDev [128]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        trgDev = 128;

        st = cpssExMxPmFabricVoqDevToConfigSetMapSet(dev, voqUnitNum, trgDev, configSetIndexArr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, trgDev = %d", dev, trgDev);

        trgDev = 0;

        /*
            1.5. Call with out of range configSetIndexArr[0] [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        configSetIndexArr[0] = 4;

        st = cpssExMxPmFabricVoqDevToConfigSetMapSet(dev, voqUnitNum, trgDev, configSetIndexArr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configSetIndexArr[0] = %d", dev, configSetIndexArr[0]);

        configSetIndexArr[0] = 0;

        /*
            1.6. Call with configSetIndexArr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqDevToConfigSetMapSet(dev, voqUnitNum, trgDev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configSetIndexArr = NULL", dev);
    }

    voqUnitNum = 0;
    trgDev     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqDevToConfigSetMapSet(dev, voqUnitNum, trgDev, configSetIndexArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqDevToConfigSetMapSet(dev, voqUnitNum, trgDev, configSetIndexArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqDevToConfigSetMapGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    voqUnitNum,
    IN  GT_U8     trgDev,
    OUT GT_FPORT  configSetIndexArr[CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqDevToConfigSetMapGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   trgDev [0 / 127]
                   and non-NULL configSetIndexArr [0, 1, 2, 3 / 3, 3, 3, 3].
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range trgDev [128]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with configSetIndexArr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    GT_U8       trgDev     = 0;
    GT_U32      configSetIndexArr[CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           trgDev [0 / 127]
                           and non-NULL configSetIndexArr [0, 1, 2, 3 / 3, 3, 3, 3].
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;
        trgDev     = 0;

        st = cpssExMxPmFabricVoqDevToConfigSetMapGet(dev, voqUnitNum, trgDev, configSetIndexArr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, trgDev);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;
        trgDev     = 127;

        st = cpssExMxPmFabricVoqDevToConfigSetMapGet(dev, voqUnitNum, trgDev, configSetIndexArr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, trgDev);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqDevToConfigSetMapGet(dev, voqUnitNum, trgDev, configSetIndexArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with out of range trgDev [128]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        trgDev = 128;

        st = cpssExMxPmFabricVoqDevToConfigSetMapGet(dev, voqUnitNum, trgDev, configSetIndexArr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, trgDev = %d", dev, trgDev);

        trgDev = 0;

        /*
            1.4. Call with configSetIndexArr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqDevToConfigSetMapGet(dev, voqUnitNum, trgDev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configSetIndexArr = NULL", dev);
    }

    voqUnitNum = 0;
    trgDev     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqDevToConfigSetMapGet(dev, voqUnitNum, trgDev, configSetIndexArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqDevToConfigSetMapGet(dev, voqUnitNum, trgDev, configSetIndexArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqTcToFabricTcMapSet
(
    IN GT_U8                               devNum,
    IN GT_U32                              voqUnitNum,
    IN CPSS_EXMXPM_FABRIC_PACKET_TYPE_ENT  pktType,
    IN GT_U32                              ingressTc,
    IN GT_U32                              fabricTc
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqTcToFabricTcMapSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   pktType [CPSS_EXMXPM_FABRIC_PACKET_TYPE_MC_E /
                            CPSS_EXMXPM_FABRIC_PACKET_TYPE_UC_E],
                   ingressTc [0 / 7]
                   and fabricTc [0 / 3].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricVoqTcToFabricTcMapGet with the same voqUnitNum,
                                                         pktType,
                                                         ingressTc
                                                         and non-NULL fabricTcPtr.
    Expected: GT_OK and the same fabricTc.
    1.3. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range pktType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range ingressTc [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range fabricTc [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                              voqUnitNum  = 0;
    CPSS_EXMXPM_FABRIC_PACKET_TYPE_ENT  pktType     = CPSS_EXMXPM_FABRIC_PACKET_TYPE_MC_E;
    GT_U32                              ingressTc   = 0;
    GT_U32                              fabricTc    = 0;
    GT_U32                              fabricTcGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           pktType [CPSS_EXMXPM_FABRIC_PACKET_TYPE_MC_E /
                                    CPSS_EXMXPM_FABRIC_PACKET_TYPE_UC_E],
                           ingressTc [0 / 7]
                           and fabricTc [0 / 3].
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;
        pktType    = CPSS_EXMXPM_FABRIC_PACKET_TYPE_MC_E;
        ingressTc  = 0;
        fabricTc   = 0;

        st = cpssExMxPmFabricVoqTcToFabricTcMapSet(dev, voqUnitNum, pktType, ingressTc, fabricTc);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, voqUnitNum, pktType, ingressTc, fabricTc);

        /*
            1.2. Call cpssExMxPmFabricVoqTcToFabricTcMapGet with the same voqUnitNum,
                                                                 pktType,
                                                                 ingressTc
                                                                 and non-NULL fabricTcPtr.
            Expected: GT_OK and the same fabricTc.
        */
        st = cpssExMxPmFabricVoqTcToFabricTcMapGet(dev, voqUnitNum, pktType, ingressTc, &fabricTcGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqTcToFabricTcMapGet: %d, %d, %d, %d", dev, voqUnitNum, pktType, ingressTc);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fabricTc, fabricTcGet,
                   "get another fabricTc than was set: %d", dev);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;
        pktType    = CPSS_EXMXPM_FABRIC_PACKET_TYPE_UC_E;
        ingressTc  = 7;
        fabricTc   = 3;

        st = cpssExMxPmFabricVoqTcToFabricTcMapSet(dev, voqUnitNum, pktType, ingressTc, fabricTc);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, voqUnitNum, pktType, ingressTc, fabricTc);

        /*
            1.2. Call cpssExMxPmFabricVoqTcToFabricTcMapGet with the same voqUnitNum,
                                                                 pktType,
                                                                 ingressTc
                                                                 and non-NULL fabricTcPtr.
            Expected: GT_OK and the same fabricTc.
        */
        st = cpssExMxPmFabricVoqTcToFabricTcMapGet(dev, voqUnitNum, pktType, ingressTc, &fabricTcGet);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqTcToFabricTcMapGet: %d, %d, %d, %d", dev, voqUnitNum, pktType, ingressTc);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fabricTc, fabricTcGet,
                   "get another fabricTc than was set: %d", dev);

        /*
            1.3. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqTcToFabricTcMapSet(dev, voqUnitNum, pktType, ingressTc, fabricTc);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with out of range pktType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pktType = FABRIC_VOQ_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricVoqTcToFabricTcMapSet(dev, voqUnitNum, pktType, ingressTc, fabricTc);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, pktType = %d", dev, pktType);

        pktType = CPSS_EXMXPM_FABRIC_PACKET_TYPE_MC_E;

        /*
            1.5. Call with out of range ingressTc [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ingressTc = 8;

        st = cpssExMxPmFabricVoqTcToFabricTcMapSet(dev, voqUnitNum, pktType, ingressTc, fabricTc);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ingressTc = %d", dev, ingressTc);

        ingressTc = 0;

        /*
            1.6. Call with out of range fabricTc [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fabricTc = 4;

        st = cpssExMxPmFabricVoqTcToFabricTcMapSet(dev, voqUnitNum, pktType, ingressTc, fabricTc);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fabricTc = %d", dev, fabricTc);
    }

    voqUnitNum = 0;
    pktType    = CPSS_EXMXPM_FABRIC_PACKET_TYPE_MC_E;
    ingressTc  = 0;
    fabricTc   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqTcToFabricTcMapSet(dev, voqUnitNum, pktType, ingressTc, fabricTc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqTcToFabricTcMapSet(dev, voqUnitNum, pktType, ingressTc, fabricTc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqTcToFabricTcMapGet
(
    IN  GT_U8                              devNum,
    IN  GT_U32                             voqUnitNum,
    IN CPSS_EXMXPM_FABRIC_PACKET_TYPE_ENT  pktType,
    IN  GT_U32                             ingressTc,
    OUT GT_U32                             *fabricTcPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqTcToFabricTcMapGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   pktType [CPSS_EXMXPM_FABRIC_PACKET_TYPE_MC_E /
                            CPSS_EXMXPM_FABRIC_PACKET_TYPE_UC_E],
                   ingressTc [0 / 7]
                   and non-NULL fabricTcPtr.
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range pktType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range ingressTc [8]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with fabricTcPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                              voqUnitNum = 0;
    CPSS_EXMXPM_FABRIC_PACKET_TYPE_ENT  pktType    = CPSS_EXMXPM_FABRIC_PACKET_TYPE_MC_E;
    GT_U32                              ingressTc  = 0;
    GT_U32                              fabricTc   = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           pktType [CPSS_EXMXPM_FABRIC_PACKET_TYPE_MC_E /
                                    CPSS_EXMXPM_FABRIC_PACKET_TYPE_UC_E],
                           ingressTc [0 / 7]
                           and non-NULL fabricTcPtr.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;
        pktType    = CPSS_EXMXPM_FABRIC_PACKET_TYPE_MC_E;
        ingressTc  = 0;

        st = cpssExMxPmFabricVoqTcToFabricTcMapGet(dev, voqUnitNum, pktType, ingressTc, &fabricTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqUnitNum, pktType, ingressTc);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;
        pktType    = CPSS_EXMXPM_FABRIC_PACKET_TYPE_UC_E;
        ingressTc  = 7;

        st = cpssExMxPmFabricVoqTcToFabricTcMapGet(dev, voqUnitNum, pktType, ingressTc, &fabricTc);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqUnitNum, pktType, ingressTc);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqTcToFabricTcMapGet(dev, voqUnitNum, pktType, ingressTc, &fabricTc);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with out of range pktType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        pktType = FABRIC_VOQ_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricVoqTcToFabricTcMapGet(dev, voqUnitNum, pktType, ingressTc, &fabricTc);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, pktType = %d", dev, pktType);

        pktType = CPSS_EXMXPM_FABRIC_PACKET_TYPE_MC_E;

        /*
            1.4. Call with out of range ingressTc [8]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ingressTc = 8;

        st = cpssExMxPmFabricVoqTcToFabricTcMapGet(dev, voqUnitNum, pktType, ingressTc, &fabricTc);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ingressTc = %d", dev, ingressTc);

        ingressTc = 0;

        /*
            1.5. Call with fabricTcPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqTcToFabricTcMapGet(dev, voqUnitNum, pktType, ingressTc, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fabricTcPtr = NULL", dev);
    }

    voqUnitNum = 0;
    pktType    = CPSS_EXMXPM_FABRIC_PACKET_TYPE_MC_E;
    ingressTc  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqTcToFabricTcMapGet(dev, voqUnitNum, pktType, ingressTc, &fabricTc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqTcToFabricTcMapGet(dev, voqUnitNum, pktType, ingressTc, &fabricTc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqControlTrafficQosSet
(
    IN  GT_U8                                        devNum,
    IN  GT_U32                                       voqUnitNum,
    IN  GT_BOOL                                      ctrlQosAssignEnable,
    IN  CPSS_EXMXPM_FABRIC_VOQ_CTRL_TRAFFIC_QOS_STC  *ctrlQosParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqControlTrafficQosSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   ctrlQosAssignEnable [GT_FALSE / GT_TRUE]
                   and ctrlQosParamsPtr {netPortToCpuTc [0 / 3],
                                         netPortToCpuDp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                                         cpuToNetPortTc [0 / 3],
                                         cpuToNetPortDp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                                         cpuToCpuTc [0 / 3],
                                         cpuToCpuDp [CPSS_DP_GREEN_E / CPSS_DP_RED_E]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricVoqControlTrafficQosGet with the same voqUnitNum
                                                           and non-NULL ctrlQosAssignEnablePtr,
                                                           ctrlQosParamsPtr.
    Expected: GT_OK and the same ctrlQosAssignEnable, ctrlQosParamsPtr..
    1.3. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range ctrlQosParamsPtr->netPortToCpuTc [4]
                   and other parameters from 1.1.
    Expected: GT_OUT_OF_RANGE.
    1.5. Call with out of range ctrlQosParamsPtr->netPortToCpuDp [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_OUT_OF_RANGE.
    1.6. Call with out of range ctrlQosParamsPtr->cpuToNetPortTc [4]
                   and other parameters from 1.1.
    Expected: GT_OUT_OF_RANGE.
    1.7. Call with out of range ctrlQosParamsPtr->cpuToNetPortDp [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_OUT_OF_RANGE.
    1.8. Call with out of range ctrlQosParamsPtr->cpuToCpuTc [4]
                   and other parameters from 1.1.
    Expected: GT_OUT_OF_RANGE.
    1.9. Call with out of range ctrlQosParamsPtr->cpuToCpuDp [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_OUT_OF_RANGE.
    1.10. Call with ctrlQosParamsPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                       voqUnitNum             = 0;
    GT_BOOL                                      ctrlQosAssignEnable    = GT_FALSE;
    GT_BOOL                                      ctrlQosAssignEnableGet = GT_FALSE;
    CPSS_EXMXPM_FABRIC_VOQ_CTRL_TRAFFIC_QOS_STC  ctrlQosParams;
    CPSS_EXMXPM_FABRIC_VOQ_CTRL_TRAFFIC_QOS_STC  ctrlQosParamsGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           ctrlQosAssignEnable [GT_FALSE / GT_TRUE]
                           and ctrlQosParamsPtr {netPortToCpuTc [0 / 3],
                                                 netPortToCpuDp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                                                 cpuToNetPortTc [0 / 3],
                                                 cpuToNetPortDp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                                                 cpuToCpuTc [0 / 3],
                                                 cpuToCpuDp [CPSS_DP_GREEN_E / CPSS_DP_RED_E]}.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum          = 0;
        ctrlQosAssignEnable = GT_FALSE;

        ctrlQosParams.netPortToCpuTc = 0;
        ctrlQosParams.netPortToCpuDp = CPSS_DP_GREEN_E;
        ctrlQosParams.cpuToNetPortTc = 0;
        ctrlQosParams.cpuToNetPortDp = CPSS_DP_GREEN_E;
        ctrlQosParams.cpuToCpuTc     = 0;
        ctrlQosParams.cpuToCpuDp     = CPSS_DP_GREEN_E;

        st = cpssExMxPmFabricVoqControlTrafficQosSet(dev, voqUnitNum, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, ctrlQosAssignEnable);

        /*
            1.2. Call cpssExMxPmFabricVoqControlTrafficQosGet with the same voqUnitNum
                                                                   and non-NULL ctrlQosAssignEnablePtr,
                                                                   ctrlQosParamsPtr.
            Expected: GT_OK and the same ctrlQosAssignEnable, ctrlQosParamsPtr..
        */
        st = cpssExMxPmFabricVoqControlTrafficQosGet(dev, voqUnitNum, &ctrlQosAssignEnableGet, &ctrlQosParamsGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqControlTrafficQosGet: %d, %d", dev, voqUnitNum);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosAssignEnable, ctrlQosAssignEnableGet,
                   "get another ctrlQosAssignEnable than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosParams.netPortToCpuTc, ctrlQosParamsGet.netPortToCpuTc,
                   "get another ctrlQosParamsPtr->netPortToCpuTc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosParams.netPortToCpuDp, ctrlQosParamsGet.netPortToCpuDp,
                   "get another ctrlQosParamsPtr->netPortToCpuDp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosParams.cpuToNetPortTc, ctrlQosParamsGet.cpuToNetPortTc,
                   "get another ctrlQosParamsPtr->cpuToNetPortTc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosParams.cpuToNetPortDp, ctrlQosParamsGet.cpuToNetPortDp,
                   "get another ctrlQosParamsPtr->cpuToNetPortDp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosParams.cpuToCpuTc, ctrlQosParamsGet.cpuToCpuTc,
                   "get another ctrlQosParamsPtr->cpuToCpuTc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosParams.cpuToCpuDp, ctrlQosParamsGet.cpuToCpuDp,
                   "get another ctrlQosParamsPtr->cpuToCpuDp than was set: %d", dev);

        /* Call with voqUnitNum [1] */
        voqUnitNum          = 1;
        ctrlQosAssignEnable = GT_TRUE;

        ctrlQosParams.netPortToCpuTc = 3;
        ctrlQosParams.netPortToCpuDp = CPSS_DP_RED_E;
        ctrlQosParams.cpuToNetPortTc = 3;
        ctrlQosParams.cpuToNetPortDp = CPSS_DP_RED_E;
        ctrlQosParams.cpuToCpuTc     = 3;
        ctrlQosParams.cpuToCpuDp     = CPSS_DP_RED_E;

        st = cpssExMxPmFabricVoqControlTrafficQosSet(dev, voqUnitNum, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, ctrlQosAssignEnable);

        /*
            1.2. Call cpssExMxPmFabricVoqControlTrafficQosGet with the same voqUnitNum
                                                                   and non-NULL ctrlQosAssignEnablePtr,
                                                                   ctrlQosParamsPtr.
            Expected: GT_OK and the same ctrlQosAssignEnable, ctrlQosParamsPtr..
        */
        st = cpssExMxPmFabricVoqControlTrafficQosGet(dev, voqUnitNum, &ctrlQosAssignEnableGet, &ctrlQosParamsGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqControlTrafficQosGet: %d, %d", dev, voqUnitNum);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosAssignEnable, ctrlQosAssignEnableGet,
                   "get another ctrlQosAssignEnable than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosParams.netPortToCpuTc, ctrlQosParamsGet.netPortToCpuTc,
                   "get another ctrlQosParamsPtr->netPortToCpuTc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosParams.netPortToCpuDp, ctrlQosParamsGet.netPortToCpuDp,
                   "get another ctrlQosParamsPtr->netPortToCpuDp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosParams.cpuToNetPortTc, ctrlQosParamsGet.cpuToNetPortTc,
                   "get another ctrlQosParamsPtr->cpuToNetPortTc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosParams.cpuToNetPortDp, ctrlQosParamsGet.cpuToNetPortDp,
                   "get another ctrlQosParamsPtr->cpuToNetPortDp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosParams.cpuToCpuTc, ctrlQosParamsGet.cpuToCpuTc,
                   "get another ctrlQosParamsPtr->cpuToCpuTc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ctrlQosParams.cpuToCpuDp, ctrlQosParamsGet.cpuToCpuDp,
                   "get another ctrlQosParamsPtr->cpuToCpuDp than was set: %d", dev);

        /*
            1.3. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqControlTrafficQosSet(dev, voqUnitNum, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with out of range ctrlQosParamsPtr->netPortToCpuTc [4]
                           and other parameters from 1.1.
            Expected: GT_OUT_OF_RANGE.
        */
        ctrlQosParams.netPortToCpuTc = 4;

        st = cpssExMxPmFabricVoqControlTrafficQosSet(dev, voqUnitNum, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, ctrlQosParamsPtr->netPortToCpuTc = %d",
                                     dev, ctrlQosParams.netPortToCpuTc);

        ctrlQosParams.netPortToCpuTc = 0;

        /*
            1.5. Call with out of range ctrlQosParamsPtr->netPortToCpuDp [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_OUT_OF_RANGE.
        */
        ctrlQosParams.netPortToCpuDp = FABRIC_VOQ_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricVoqControlTrafficQosSet(dev, voqUnitNum, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, ctrlQosParamsPtr->netPortToCpuDp = %d",
                                     dev, ctrlQosParams.netPortToCpuDp);

        ctrlQosParams.netPortToCpuDp = CPSS_DP_GREEN_E;

        /*
            1.6. Call with out of range ctrlQosParamsPtr->cpuToNetPortTc [4]
                           and other parameters from 1.1.
            Expected: GT_OUT_OF_RANGE.
        */
        ctrlQosParams.cpuToNetPortTc = 4;

        st = cpssExMxPmFabricVoqControlTrafficQosSet(dev, voqUnitNum, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, ctrlQosParamsPtr->cpuToNetPortTc = %d",
                                     dev, ctrlQosParams.cpuToNetPortTc);

        ctrlQosParams.cpuToNetPortTc = 0;

        /*
            1.7. Call with out of range ctrlQosParamsPtr->cpuToNetPortDp [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_OUT_OF_RANGE.
        */
        ctrlQosParams.cpuToNetPortDp = FABRIC_VOQ_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricVoqControlTrafficQosSet(dev, voqUnitNum, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, ctrlQosParamsPtr->cpuToNetPortDp = %d",
                                     dev, ctrlQosParams.cpuToNetPortDp);

        ctrlQosParams.cpuToNetPortDp = CPSS_DP_GREEN_E;

        /*
            1.8. Call with out of range ctrlQosParamsPtr->cpuToCpuTc [4]
                           and other parameters from 1.1.
            Expected: GT_OUT_OF_RANGE.
        */
        ctrlQosParams.cpuToCpuTc = 4;

        st = cpssExMxPmFabricVoqControlTrafficQosSet(dev, voqUnitNum, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, ctrlQosParamsPtr->cpuToCpuTc = %d",
                                     dev, ctrlQosParams.cpuToCpuTc);

        ctrlQosParams.cpuToCpuTc = 0;

        /*
            1.9. Call with out of range ctrlQosParamsPtr->cpuToCpuDp [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_OUT_OF_RANGE.
        */
        ctrlQosParams.cpuToCpuDp = FABRIC_VOQ_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricVoqControlTrafficQosSet(dev, voqUnitNum, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, ctrlQosParamsPtr->cpuToCpuDp = %d",
                                     dev, ctrlQosParams.cpuToCpuDp);

        ctrlQosParams.cpuToCpuDp = CPSS_DP_GREEN_E;

        /*
            1.10. Call with ctrlQosParamsPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqControlTrafficQosSet(dev, voqUnitNum, ctrlQosAssignEnable, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ctrlQosParamsPtr = NULL", dev);
    }

    voqUnitNum          = 0;
    ctrlQosAssignEnable = GT_FALSE;

    ctrlQosParams.netPortToCpuTc = 0;
    ctrlQosParams.netPortToCpuDp = CPSS_DP_GREEN_E;
    ctrlQosParams.cpuToNetPortTc = 0;
    ctrlQosParams.cpuToNetPortDp = CPSS_DP_GREEN_E;
    ctrlQosParams.cpuToCpuTc     = 0;
    ctrlQosParams.cpuToCpuDp     = CPSS_DP_GREEN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqControlTrafficQosSet(dev, voqUnitNum, ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqControlTrafficQosSet(dev, voqUnitNum, ctrlQosAssignEnable, &ctrlQosParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqControlTrafficQosGet
(
    IN  GT_U8                                           devNum,
    IN  GT_U32                                          voqUnitNum,
    OUT GT_BOOL                                         *ctrlQosAssignEnablePtr,
    OUT CPSS_EXMXPM_FABRIC_VOQ_CTRL_TRAFFIC_QOS_STC     *ctrlQosParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqControlTrafficQosGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1]
                   and non-NULL ctrlQosAssignEnablePtr,
                   non-NULL ctrlQosParamsPtr.
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with ctrlQosAssignEnablePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with ctrlQosParamsPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                       voqUnitNum          = 0;
    GT_BOOL                                      ctrlQosAssignEnable = GT_FALSE;
    CPSS_EXMXPM_FABRIC_VOQ_CTRL_TRAFFIC_QOS_STC  ctrlQosParams;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1]
                           and non-NULL ctrlQosAssignEnablePtr,
                           non-NULL ctrlQosParamsPtr.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;

        st = cpssExMxPmFabricVoqControlTrafficQosGet(dev, voqUnitNum, &ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;

        st = cpssExMxPmFabricVoqControlTrafficQosGet(dev, voqUnitNum, &ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqControlTrafficQosGet(dev, voqUnitNum, &ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with ctrlQosAssignEnablePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqControlTrafficQosGet(dev, voqUnitNum, NULL, &ctrlQosParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ctrlQosAssignEnablePtr = NULL", dev);


        /*
            1.4. Call with ctrlQosParamsPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqControlTrafficQosGet(dev, voqUnitNum, &ctrlQosAssignEnable, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ctrlQosParamsPtr = NULL", dev);
    }

    voqUnitNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqControlTrafficQosGet(dev, voqUnitNum, &ctrlQosAssignEnable, &ctrlQosParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqControlTrafficQosGet(dev, voqUnitNum, &ctrlQosAssignEnable, &ctrlQosParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqGlobalLimitsSet
(
    IN GT_U8   devNum,
    IN GT_U32  voqUnitNum,
    IN GT_U32  maxDescriptors,
    IN GT_U32  maxBuffers
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqGlobalLimitsSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   maxDescriptors [0 / 2047]
                   and maxBuffers [0 / 2047].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricVoqGlobalLimitsGet with the same voqUnitNum
                                                      and non-NULL maxDescriptorsPtr,
                                                      maxBuffersPtr.
    Expected: GT_OK and the same maxDescriptors, maxBuffers.
    1.3. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range maxDescriptors [2048]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range maxBuffers [2048]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum        = 0;
    GT_U32      maxDescriptors    = 0;
    GT_U32      maxBuffers        = 0;
    GT_U32      maxDescriptorsGet = 0;
    GT_U32      maxBuffersGet     = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           maxDescriptors [0 / 2047]
                           and maxBuffers [0 / 2047].
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum     = 0;
        maxDescriptors = 0;
        maxBuffers     = 0;

        st = cpssExMxPmFabricVoqGlobalLimitsSet(dev, voqUnitNum, maxDescriptors, maxBuffers);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqUnitNum, maxDescriptors, maxBuffers);

        /*
            1.2. Call cpssExMxPmFabricVoqGlobalLimitsGet with the same voqUnitNum
                                                              and non-NULL maxDescriptorsPtr,
                                                              maxBuffersPtr.
            Expected: GT_OK and the same maxDescriptors, maxBuffers.
        */
        st = cpssExMxPmFabricVoqGlobalLimitsGet(dev, voqUnitNum, &maxDescriptorsGet, &maxBuffersGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqGlobalLimitsGet: %d, %d", dev, voqUnitNum);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maxDescriptors, maxDescriptorsGet,
                   "get another maxDescriptors than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(maxBuffers, maxBuffersGet,
                   "get another maxBuffers than was set: %d", dev);

        /* Call with voqUnitNum [1] */
        voqUnitNum     = 1;
        maxDescriptors = 2047;
        maxBuffers     = 2047;

        st = cpssExMxPmFabricVoqGlobalLimitsSet(dev, voqUnitNum, maxDescriptors, maxBuffers);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqUnitNum, maxDescriptors, maxBuffers);

        /*
            1.2. Call cpssExMxPmFabricVoqGlobalLimitsGet with the same voqUnitNum
                                                              and non-NULL maxDescriptorsPtr,
                                                              maxBuffersPtr.
            Expected: GT_OK and the same maxDescriptors, maxBuffers.
        */
        st = cpssExMxPmFabricVoqGlobalLimitsGet(dev, voqUnitNum, &maxDescriptorsGet, &maxBuffersGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqGlobalLimitsGet: %d, %d", dev, voqUnitNum);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maxDescriptors, maxDescriptorsGet,
                   "get another maxDescriptors than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(maxBuffers, maxBuffersGet,
                   "get another maxBuffers than was set: %d", dev);

        /*
            1.3. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqGlobalLimitsSet(dev, voqUnitNum, maxDescriptors, maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with out of range maxDescriptors [2048]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        maxDescriptors = 2048;

        st = cpssExMxPmFabricVoqGlobalLimitsSet(dev, voqUnitNum, maxDescriptors, maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, maxDescriptors = %d", dev, maxDescriptors);

        maxDescriptors = 0;

        /*
            1.5. Call with out of range maxBuffers [2048]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        maxBuffers = 2048;

        st = cpssExMxPmFabricVoqGlobalLimitsSet(dev, voqUnitNum, maxDescriptors, maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, maxBuffers = %d", dev, maxBuffers);
    }

    voqUnitNum     = 0;
    maxDescriptors = 0;
    maxBuffers     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqGlobalLimitsSet(dev, voqUnitNum, maxDescriptors, maxBuffers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqGlobalLimitsSet(dev, voqUnitNum, maxDescriptors, maxBuffers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqGlobalLimitsGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  voqUnitNum,
    OUT GT_U32  *maxDescriptorsPtr,
    OUT GT_U32  *maxBuffersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqGlobalLimitsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1]
                   and non-NULL maxDescriptorsPtr,
                   non-NULL maxBuffersPtr.
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with maxDescriptorsPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with maxBuffersPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum     = 0;
    GT_U32      maxDescriptors = 0;
    GT_U32      maxBuffers     = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1]
                           and non-NULL maxDescriptorsPtr,
                           non-NULL maxBuffersPtr.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;

        st = cpssExMxPmFabricVoqGlobalLimitsGet(dev, voqUnitNum, &maxDescriptors, &maxBuffers);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;

        st = cpssExMxPmFabricVoqGlobalLimitsGet(dev, voqUnitNum, &maxDescriptors, &maxBuffers);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqGlobalLimitsGet(dev, voqUnitNum, &maxDescriptors, &maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with maxDescriptorsPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqGlobalLimitsGet(dev, voqUnitNum, NULL, &maxBuffers);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maxDescriptorsPtr = NULL", dev);

        /*
            1.4. Call with maxBuffersPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqGlobalLimitsGet(dev, voqUnitNum, &maxDescriptors, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maxBuffersPtr = NULL", dev);
    }

    voqUnitNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqGlobalLimitsGet(dev, voqUnitNum, &maxDescriptors, &maxBuffers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqGlobalLimitsGet(dev, voqUnitNum, &maxDescriptors, &maxBuffers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqStatCntrsConfigSet
(
    IN  GT_U8                                             devNum,
    IN  GT_U32                                            voqUnitNum,
    IN  GT_U32                                            setNum,
    IN  CPSS_EXMXPM_FABRIC_VOQ_STAT_CNTRS_SET_CONFIG_STC  *counterSetConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqStatCntrsConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   setNum [0 / 3]
                   and counterSetConfigPtr {cntrTypeSelect [CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_TYPE_BUFFERS_E /
                                                            CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_TYPE_DESCRIPTORS_E],
                                            dpType [CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_PP_DESC_E /
                                                    CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_FA_ASSIGN_E],
                                            countDpEnable [GT_FALSE / GT_TRUE],
                                            countedDp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                                            tcType [CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_PP_DESC_E /
                                                    CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_FA_ASSIGN_E],
                                            countTcEnable [GT_FALSE / GT_TRUE],
                                            countedTc [0 / 7],
                                            countFportEnable [GT_FALSE / GT_TRUE],
                                            countedFport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS],
                                            countDevEnable [GT_FALSE / GT_TRUE],
                                            countedDev [0 / devNum]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricVoqStatCntrsConfigGet with the same voqUnitNum,
                                                         setNum
                                                         and non-NULL counterSetConfigPtr.
    Expected: GT_OK and the same counterSetConfigPtr.
    1.3. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range setNum [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range counterSetConfigPtr->cntrTypeSelect [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with counterSetConfigPtr->countDpEnable [GT_TRUE],
                   out of range counterSetConfigPtr->dpType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with counterSetConfigPtr->countDpEnable [GT_FALSE],
                   out of range counterSetConfigPtr->dpType [0x5AAAAAA5] (ignored)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.8. Call with counterSetConfigPtr->countDpEnable [GT_TRUE],
                   out of range counterSetConfigPtr->countedDp [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with counterSetConfigPtr->countDpEnable [GT_FALSE],
                   out of range counterSetConfigPtr->countedDp [0x5AAAAAA5] (ignored)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.10. Call with counterSetConfigPtr->countTcEnable [GT_TRUE],
                    out of range counterSetConfigPtr->tcType [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.11. Call with counterSetConfigPtr->countTcEnable [GT_FALSE],
                    out of range counterSetConfigPtr->tcType [0x5AAAAAA5] (ignored)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.12. Call with counterSetConfigPtr->countTcEnable [GT_TRUE],
                    out of range counterSetConfigPtr->countedTc [8]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.13. Call with counterSetConfigPtr->countTcEnable [GT_FALSE],
                    out of range counterSetConfigPtr->countedTc [8] (ignored)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.14. Call with counterSetConfigPtr->countFportEnable [GT_TRUE],
                    out of range counterSetConfigPtr->countedFport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.15. Call with counterSetConfigPtr->countFportEnable [GT_FALSE],
                    out of range counterSetConfigPtr->countedFport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS] (ignored)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.16. Call with counterSetConfigPtr->countDevEnable [GT_TRUE],
                    out of range counterSetConfigPtr->countedDev [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.17. Call with counterSetConfigPtr->countDevEnable [GT_FALSE],
                    out of range counterSetConfigPtr->countedDev [PRV_CPSS_MAX_PP_DEVICES_CNS] (ignored)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.18. Call with counterSetConfigPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                            voqUnitNum = 0;
    GT_U32                                            setNum     = 0;
    CPSS_EXMXPM_FABRIC_VOQ_STAT_CNTRS_SET_CONFIG_STC  counterSetConfig;
    CPSS_EXMXPM_FABRIC_VOQ_STAT_CNTRS_SET_CONFIG_STC  counterSetConfigGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           setNum [0 / 3]
                           and counterSetConfigPtr {cntrTypeSelect [CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_TYPE_BUFFERS_E /
                                                                    CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_TYPE_DESCRIPTORS_E],
                                                    dpType [CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_PP_DESC_E /
                                                            CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_FA_ASSIGN_E],
                                                    countDpEnable [GT_FALSE / GT_TRUE],
                                                    countedDp [CPSS_DP_GREEN_E / CPSS_DP_RED_E],
                                                    tcType [CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_PP_DESC_E /
                                                            CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_FA_ASSIGN_E],
                                                    countTcEnable [GT_FALSE / GT_TRUE],
                                                    countedTc [0 / 7],
                                                    countFportEnable [GT_FALSE / GT_TRUE],
                                                    countedFport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS],
                                                    countDevEnable [GT_FALSE / GT_TRUE],
                                                    countedDev [0 / devNum]}.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;
        setNum     = 0;

        counterSetConfig.cntrTypeSelect   = CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_TYPE_BUFFERS_E;
        counterSetConfig.dpType           = CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_PP_DESC_E;
        counterSetConfig.countDpEnable    = GT_FALSE;
        counterSetConfig.countedDp        = CPSS_DP_GREEN_E;
        counterSetConfig.tcType           = CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_PP_DESC_E;
        counterSetConfig.countTcEnable    = GT_FALSE;
        counterSetConfig.countedTc        = 0;
        counterSetConfig.countFportEnable = GT_FALSE;
        counterSetConfig.countedFport     = CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS;
        counterSetConfig.countDevEnable   = GT_FALSE;
        counterSetConfig.countedDev       = 0;

        st = cpssExMxPmFabricVoqStatCntrsConfigSet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, setNum);

        /*
            1.2. Call cpssExMxPmFabricVoqStatCntrsConfigGet with the same voqUnitNum,
                                                                 setNum
                                                                 and non-NULL counterSetConfigPtr.
            Expected: GT_OK and the same counterSetConfigPtr.
        */
        st = cpssExMxPmFabricVoqStatCntrsConfigGet(dev, voqUnitNum, setNum, &counterSetConfigGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqStatCntrsConfigGet: %d, %d, %d", dev, voqUnitNum, setNum);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(counterSetConfig.cntrTypeSelect, counterSetConfigGet.cntrTypeSelect,
                   "get another counterSetConfigPtr->cntrTypeSelect than was set: %d", dev);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;
        setNum     = 3;

        counterSetConfig.cntrTypeSelect   = CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_TYPE_DESCRIPTORS_E;
        counterSetConfig.dpType           = CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_FA_ASSIGN_E;
        counterSetConfig.countDpEnable    = GT_TRUE;
        counterSetConfig.countedDp        = CPSS_DP_RED_E;
        counterSetConfig.tcType           = CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_FA_ASSIGN_E;
        counterSetConfig.countTcEnable    = GT_TRUE;
        counterSetConfig.countedTc        = 7;
        counterSetConfig.countFportEnable = GT_TRUE;
        counterSetConfig.countedFport     = 0;
        counterSetConfig.countDevEnable   = GT_TRUE;
        counterSetConfig.countedDev       = dev;

        st = cpssExMxPmFabricVoqStatCntrsConfigSet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, setNum);

        /*
            1.2. Call cpssExMxPmFabricVoqStatCntrsConfigGet with the same voqUnitNum,
                                                                 setNum
                                                                 and non-NULL counterSetConfigPtr.
            Expected: GT_OK and the same counterSetConfigPtr.
        */
        st = cpssExMxPmFabricVoqStatCntrsConfigGet(dev, voqUnitNum, setNum, &counterSetConfigGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqStatCntrsConfigGet: %d, %d, %d", dev, voqUnitNum, setNum);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(counterSetConfig.cntrTypeSelect, counterSetConfigGet.cntrTypeSelect,
                   "get another counterSetConfigPtr->cntrTypeSelect than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterSetConfig.dpType, counterSetConfigGet.dpType,
                   "get another counterSetConfigPtr->dpType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterSetConfig.countDpEnable, counterSetConfigGet.countDpEnable,
                   "get another counterSetConfigPtr->countDpEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterSetConfig.countedDp, counterSetConfigGet.countedDp,
                   "get another counterSetConfigPtr->countedDp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterSetConfig.tcType, counterSetConfigGet.tcType,
                   "get another counterSetConfigPtr->tcType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterSetConfig.countTcEnable, counterSetConfigGet.countTcEnable,
                   "get another counterSetConfigPtr->countTcEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterSetConfig.countedTc, counterSetConfigGet.countedTc,
                   "get another counterSetConfigPtr->countedTc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterSetConfig.countFportEnable, counterSetConfigGet.countFportEnable,
                   "get another counterSetConfigPtr->countFportEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterSetConfig.countedFport, counterSetConfigGet.countedFport,
                   "get another counterSetConfigPtr->countedFport than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterSetConfig.countDevEnable, counterSetConfigGet.countDevEnable,
                   "get another counterSetConfigPtr->countDevEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterSetConfig.countedDev, counterSetConfigGet.countedDev,
                   "get another counterSetConfigPtr->countedDev than was set: %d", dev);

        /*
            1.3. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqStatCntrsConfigSet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with out of range setNum [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        setNum = 4;

        st = cpssExMxPmFabricVoqStatCntrsConfigSet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, setNum = %d", dev, setNum);

        setNum = 0;

        /*
            1.5. Call with out of range counterSetConfigPtr->cntrTypeSelect [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        counterSetConfig.cntrTypeSelect = FABRIC_VOQ_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricVoqStatCntrsConfigSet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, counterSetConfigPtr->cntrTypeSelect = %d",
                                     dev, counterSetConfig.cntrTypeSelect);

        counterSetConfig.cntrTypeSelect = CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_TYPE_BUFFERS_E;

        /*
            1.6. Call with counterSetConfigPtr->countDpEnable [GT_TRUE],
                           out of range counterSetConfigPtr->dpType [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
            1.7. Call with counterSetConfigPtr->countDpEnable [GT_FALSE],
                           out of range counterSetConfigPtr->dpType [0x5AAAAAA5] (ignored)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        counterSetConfig.countDpEnable = GT_TRUE;
        counterSetConfig.dpType        = FABRIC_VOQ_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricVoqStatCntrsConfigSet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->dpType = %d, ->countDpEnable = %d",
                                     dev, counterSetConfig.dpType, counterSetConfig.countDpEnable);

        counterSetConfig.dpType = CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_PP_DESC_E;

        /*
            1.8. Call with counterSetConfigPtr->countDpEnable [GT_TRUE],
                           out of range counterSetConfigPtr->countedDp [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
            1.9. Call with counterSetConfigPtr->countDpEnable [GT_FALSE],
                           out of range counterSetConfigPtr->countedDp [0x5AAAAAA5] (ignored)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        counterSetConfig.countDpEnable = GT_TRUE;
        counterSetConfig.countedDp     = FABRIC_VOQ_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricVoqStatCntrsConfigSet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->countedDp = %d, ->countDpEnable = %d",
                                     dev, counterSetConfig.countedDp, counterSetConfig.countDpEnable);

        counterSetConfig.countedDp = CPSS_DP_GREEN_E;

        /*
            1.10. Call with counterSetConfigPtr->countTcEnable [GT_TRUE],
                            out of range counterSetConfigPtr->tcType [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
            1.11. Call with counterSetConfigPtr->countTcEnable [GT_FALSE],
                            out of range counterSetConfigPtr->tcType [0x5AAAAAA5] (ignored)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        counterSetConfig.countTcEnable = GT_TRUE;
        counterSetConfig.tcType        = FABRIC_VOQ_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricVoqStatCntrsConfigSet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->tcType = %d, ->countTcEnable = %d",
                                     dev, counterSetConfig.tcType, counterSetConfig.countTcEnable);

        counterSetConfig.tcType = CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_PP_DESC_E;

        /*
            1.12. Call with counterSetConfigPtr->countTcEnable [GT_TRUE],
                            out of range counterSetConfigPtr->countedTc [8]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
            1.13. Call with counterSetConfigPtr->countTcEnable [GT_FALSE],
                            out of range counterSetConfigPtr->countedTc [8] (ignored)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        counterSetConfig.countTcEnable = GT_TRUE;
        counterSetConfig.countedTc     = 8;

        st = cpssExMxPmFabricVoqStatCntrsConfigSet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->countedTc = %d, ->countTcEnable = %d",
                                         dev, counterSetConfig.countedTc, counterSetConfig.countTcEnable);

        counterSetConfig.countedTc = 0;

        /*
            1.14. Call with counterSetConfigPtr->countFportEnable [GT_TRUE],
                            out of range counterSetConfigPtr->countedFport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
            1.15. Call with counterSetConfigPtr->countFportEnable [GT_FALSE],
                            out of range counterSetConfigPtr->countedFport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS] (ignored)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        counterSetConfig.countFportEnable = GT_TRUE;
        counterSetConfig.countedFport     = CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS;

        st = cpssExMxPmFabricVoqStatCntrsConfigSet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->countedFport = %d, ->countFportEnable = %d",
                                         dev, counterSetConfig.countedFport, counterSetConfig.countFportEnable);

        counterSetConfig.countedFport = 0;

        /*
            1.16. Call with counterSetConfigPtr->countDevEnable [GT_TRUE],
                            out of range counterSetConfigPtr->countedDev [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
            1.17. Call with counterSetConfigPtr->countDevEnable [GT_FALSE],
                            out of range counterSetConfigPtr->countedDev [PRV_CPSS_MAX_PP_DEVICES_CNS] (ignored)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        counterSetConfig.countDevEnable = GT_TRUE;
        counterSetConfig.countedDev     = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmFabricVoqStatCntrsConfigSet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->countedDev = %d, ->countDevEnable = %d",
                                         dev, counterSetConfig.countedDev, counterSetConfig.countDevEnable);

        counterSetConfig.countedDev = 0;

        /*
            1.18. Call with counterSetConfigPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqStatCntrsConfigSet(dev, voqUnitNum, setNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterSetConfigPtr = NULL", dev);
    }

    voqUnitNum = 0;
    setNum     = 0;

    counterSetConfig.cntrTypeSelect   = CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_TYPE_BUFFERS_E;
    counterSetConfig.dpType           = CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_PP_DESC_E;
    counterSetConfig.countDpEnable    = GT_FALSE;
    counterSetConfig.countedDp        = CPSS_DP_GREEN_E;
    counterSetConfig.tcType           = CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_PP_DESC_E;
    counterSetConfig.countTcEnable    = GT_FALSE;
    counterSetConfig.countedTc        = 0;
    counterSetConfig.countFportEnable = GT_FALSE;
    counterSetConfig.countedFport     = CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS;
    counterSetConfig.countDevEnable   = GT_FALSE;
    counterSetConfig.countedDev       = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqStatCntrsConfigSet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqStatCntrsConfigSet(dev, voqUnitNum, setNum, &counterSetConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqStatCntrsConfigGet
(
    IN  GT_U8                                             devNum,
    IN  GT_U32                                            voqUnitNum,
    IN  GT_U32                                            setNum,
    OUT CPSS_EXMXPM_FABRIC_VOQ_STAT_CNTRS_SET_CONFIG_STC  *counterSetConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqStatCntrsConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   setNum [0 / 3]
                   and non-NULL counterSetConfigPtr.
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range setNum [4]
                   sand other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with counterSetConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                            voqUnitNum = 0;
    GT_U32                                            setNum     = 0;
    CPSS_EXMXPM_FABRIC_VOQ_STAT_CNTRS_SET_CONFIG_STC  counterSetConfig;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           setNum [0 / 3]
                           and non-NULL counterSetConfigPtr.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;
        setNum     = 0;

        st = cpssExMxPmFabricVoqStatCntrsConfigGet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, setNum);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;
        setNum     = 3;

        st = cpssExMxPmFabricVoqStatCntrsConfigGet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, setNum);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqStatCntrsConfigGet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with out of range setNum [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        setNum = 4;

        st = cpssExMxPmFabricVoqStatCntrsConfigGet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, setNum = %d", dev, setNum);

        setNum = 0;

        /*
            1.4. Call with counterSetConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqStatCntrsConfigGet(dev, voqUnitNum, setNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterSetConfigPtr = NULL", dev);
    }

    voqUnitNum = 0;
    setNum     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqStatCntrsConfigGet(dev, voqUnitNum, setNum, &counterSetConfig);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqStatCntrsConfigGet(dev, voqUnitNum, setNum, &counterSetConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqStatCntrsGet
(
    IN   GT_U8                                  devNum,
    IN   GT_U32                                 voqUnitNum,
    IN   GT_U32                                 setNum,
    OUT  CPSS_EXMXPM_FABRIC_VOQ_STAT_CNTRS_STC  *cntrsValPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqStatCntrsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   setNum [0 / 3]
                   and non-NULL cntrsValPtr.
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range setNum [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with cntrsValPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                 voqUnitNum = 0;
    GT_U32                                 setNum     = 0;
    CPSS_EXMXPM_FABRIC_VOQ_STAT_CNTRS_STC  cntrsVal;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           setNum [0 / 3]
                           and non-NULL cntrsValPtr.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;
        setNum     = 0;

        st = cpssExMxPmFabricVoqStatCntrsGet(dev, voqUnitNum, setNum, &cntrsVal);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, setNum);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;
        setNum     = 3;

        st = cpssExMxPmFabricVoqStatCntrsGet(dev, voqUnitNum, setNum, &cntrsVal);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, setNum);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqStatCntrsGet(dev, voqUnitNum, setNum, &cntrsVal);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with out of range setNum [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        setNum = 4;

        st = cpssExMxPmFabricVoqStatCntrsGet(dev, voqUnitNum, setNum, &cntrsVal);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, setNum = %d", dev, setNum);

        setNum = 0;

        /*
            1.4. Call with cntrsValPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqStatCntrsGet(dev, voqUnitNum, setNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntrsValPtr = NULL", dev);
    }

    voqUnitNum = 0;
    setNum     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqStatCntrsGet(dev, voqUnitNum, setNum, &cntrsVal);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqStatCntrsGet(dev, voqUnitNum, setNum, &cntrsVal);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqGetGlobalCountersGet
(
    IN   GT_U8                                   devNum,
    IN   GT_U32                                  voqUnitNum,
    OUT  CPSS_EXMXPM_FABRIC_VOQ_GLOBAL_CNTRS_STC *voqGlobalCountersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqGetGlobalCountersGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1]
                   and non-NULL voqGlobalCountersPtr.
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with voqGlobalCountersPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                  voqUnitNum = 0;
    CPSS_EXMXPM_FABRIC_VOQ_GLOBAL_CNTRS_STC voqGlobalCounters;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1]
                           and non-NULL voqGlobalCountersPtr.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;

        st = cpssExMxPmFabricVoqGetGlobalCountersGet(dev, voqUnitNum, &voqGlobalCounters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;

        st = cpssExMxPmFabricVoqGetGlobalCountersGet(dev, voqUnitNum, &voqGlobalCounters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqGetGlobalCountersGet(dev, voqUnitNum, &voqGlobalCounters);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with voqGlobalCountersPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqGetGlobalCountersGet(dev, voqUnitNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, voqGlobalCountersPtr = NULL", dev);
    }

    voqUnitNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqGetGlobalCountersGet(dev, voqUnitNum, &voqGlobalCounters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqGetGlobalCountersGet(dev, voqUnitNum, &voqGlobalCounters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet
(
    IN   GT_U8                                              devNum,
    IN   GT_U32                                             voqUnitNum,
    OUT  CPSS_EXMXPM_FABRIC_VOQ_GLOBAL_RESOURCES_CNTRS_STC  *cntrsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1]
                   and non-NULL cntrsPtr.
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with cntrsPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                              voqUnitNum = 0;
    CPSS_EXMXPM_FABRIC_VOQ_GLOBAL_RESOURCES_CNTRS_STC   cntrs;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1]
                           and non-NULL cntrsPtr.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;

        st = cpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet(dev, voqUnitNum, &cntrs);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;

        st = cpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet(dev, voqUnitNum, &cntrs);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet(dev, voqUnitNum, &cntrs);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with cntrsPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet(dev, voqUnitNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntrsPtr = NULL", dev);
    }

    voqUnitNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet(dev, voqUnitNum, &cntrs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet(dev, voqUnitNum, &cntrs);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqFportResourcesInUseCntrsGet
(
    IN   GT_U8                                             devNum,
    IN   GT_U32                                            voqUnitNum,
    IN   GT_FPORT                                          fport,
    OUT  CPSS_EXMXPM_FABRIC_VOQ_FPORT_RESOURCES_CNTRS_STC  *cntrsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqFportResourcesInUseCntrsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   fport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS]
                   and non-NULL cntrsPtr.
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range fport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with cntrsPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                                  voqUnitNum = 0;
    GT_FPORT                                                fport      = 0;
    OUT  CPSS_EXMXPM_FABRIC_VOQ_FPORT_RESOURCES_CNTRS_STC   cntrs;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           fport [0 / CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS]
                           and non-NULL cntrsPtr.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;
        fport      = 0;

        st = cpssExMxPmFabricVoqFportResourcesInUseCntrsGet(dev, voqUnitNum, fport, &cntrs);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, fport);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;
        fport      = CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS;

        st = cpssExMxPmFabricVoqFportResourcesInUseCntrsGet(dev, voqUnitNum, fport, &cntrs);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, fport);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqFportResourcesInUseCntrsGet(dev, voqUnitNum, fport, &cntrs);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with out of range fport [CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fport = CPSS_EXMXPM_FABRIC_UC_FPORT_NUM_CNS;

        st = cpssExMxPmFabricVoqFportResourcesInUseCntrsGet(dev, voqUnitNum, fport, &cntrs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fport = %d", dev, fport);

        fport = 0;

        /*
            1.4. Call with cntrsValPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqFportResourcesInUseCntrsGet(dev, voqUnitNum, fport, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntrsValPtr = NULL", dev);
    }

    voqUnitNum = 0;
    fport      = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqFportResourcesInUseCntrsGet(dev, voqUnitNum, fport, &cntrs);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqFportResourcesInUseCntrsGet(dev, voqUnitNum, fport, &cntrs);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqMcToConfigSetMapSet
(
    IN GT_U8   devNum,
    IN GT_U32  voqUnitNum,
    IN GT_U32  configSetIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqMcToConfigSetMapSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1]
                   and configSetIndex [0 / 3].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricVoqMcToConfigSetMapGet with the same voqUnitNum
                                                          and non-NULL configSetIndexPtr.
    Expected: GT_OK and the same configSetIndex.
    1.3. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range configSetIndex [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum        = 0;
    GT_U32      configSetIndex    = 0;
    GT_U32      configSetIndexGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1]
                           and configSetIndex [0 / 3].
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum     = 0;
        configSetIndex = 0;

        st = cpssExMxPmFabricVoqMcToConfigSetMapSet(dev, voqUnitNum, configSetIndex);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, configSetIndex);

        /*
            1.2. Call cpssExMxPmFabricVoqMcToConfigSetMapGet with the same voqUnitNum
                                                                  and non-NULL configSetIndexPtr.
            Expected: GT_OK and the same configSetIndex.
        */
        st = cpssExMxPmFabricVoqMcToConfigSetMapGet(dev, voqUnitNum, &configSetIndexGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqMcToConfigSetMapGet: %d, %d", dev, voqUnitNum);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(configSetIndex, configSetIndexGet,
                   "get another configSetIndex than was set: %d", dev);

        /* Call with voqUnitNum [1] */
        voqUnitNum     = 0;
        configSetIndex = 0;

        st = cpssExMxPmFabricVoqMcToConfigSetMapSet(dev, voqUnitNum, configSetIndex);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, configSetIndex);

        /*
            1.2. Call cpssExMxPmFabricVoqMcToConfigSetMapGet with the same voqUnitNum
                                                                  and non-NULL configSetIndexPtr.
            Expected: GT_OK and the same configSetIndex.
        */
        st = cpssExMxPmFabricVoqMcToConfigSetMapGet(dev, voqUnitNum, &configSetIndexGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqMcToConfigSetMapGet: %d, %d", dev, voqUnitNum);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(configSetIndex, configSetIndexGet,
                   "get another configSetIndex than was set: %d", dev);

        /*
            1.3. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqMcToConfigSetMapSet(dev, voqUnitNum, configSetIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with out of range configSetIndex [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        configSetIndex = 4;

        st = cpssExMxPmFabricVoqMcToConfigSetMapSet(dev, voqUnitNum, configSetIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configSetIndex = %d", dev, configSetIndex);
    }

    voqUnitNum     = 0;
    configSetIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqMcToConfigSetMapSet(dev, voqUnitNum, configSetIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqMcToConfigSetMapSet(dev, voqUnitNum, configSetIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqMcToConfigSetMapGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   voqUnitNum,
    OUT GT_U32   *configSetIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqMcToConfigSetMapGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1]
                   and non-NULL configSetIndexPtr.
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with configSetIndexPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum     = 0;
    GT_U32      configSetIndex = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1]
                           and non-NULL configSetIndexPtr.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;

        st = cpssExMxPmFabricVoqMcToConfigSetMapGet(dev, voqUnitNum, &configSetIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;

        st = cpssExMxPmFabricVoqMcToConfigSetMapGet(dev, voqUnitNum, &configSetIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqMcToConfigSetMapGet(dev, voqUnitNum, &configSetIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with configSetIndexPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqMcToConfigSetMapGet(dev, voqUnitNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configSetIndexPtr = NULL", dev);
    }

    voqUnitNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqMcToConfigSetMapGet(dev, voqUnitNum, &configSetIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqMcToConfigSetMapGet(dev, voqUnitNum, &configSetIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitSet
(
    IN GT_U8    devNum,
    IN GT_U32   voqUnitNum,
    IN GT_U32   configSetIndex,
    IN GT_U32   maxBuffers
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   configSetIndex [0 / 3]
                   and maxBuffers [1 / 2047].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet with the same voqUnitNum,
                                                                       configSetIndex
                                                                       and non-NULL maxBuffersPtr.
    Expected: GT_OK and the same maxBuffers.
    1.3. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range configSetIndex [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range maxBuffers [2048]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum     = 0;
    GT_U32      configSetIndex = 0;
    GT_U32      maxBuffers     = 0;
    GT_U32      maxBuffersGet  = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           configSetIndex [0 / 3]
                           and maxBuffers [1 / 2047].
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum     = 0;
        configSetIndex = 0;
        maxBuffers     = 1;

        st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, maxBuffers);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqUnitNum, configSetIndex, maxBuffers);

        /*
            1.2. Call cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet with the same voqUnitNum,
                                                                               configSetIndex
                                                                               and non-NULL maxBuffersPtr.
            Expected: GT_OK and the same maxBuffers.
        */
        st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, &maxBuffersGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet: %d, %d, %d", dev, voqUnitNum, configSetIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maxBuffers, maxBuffersGet,
                   "get another maxBuffers than was set: %d", dev);

        /* Call with voqUnitNum [1] */
        voqUnitNum     = 1;
        configSetIndex = 3;
        maxBuffers     = 2047;

        st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, maxBuffers);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqUnitNum, configSetIndex, maxBuffers);

        /*
            1.2. Call cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet with the same voqUnitNum,
                                                                               configSetIndex
                                                                               and non-NULL maxBuffersPtr.
            Expected: GT_OK and the same maxBuffers.
        */
        st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, &maxBuffersGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet: %d, %d, %d", dev, voqUnitNum, configSetIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maxBuffers, maxBuffersGet,
                   "get another maxBuffers than was set: %d", dev);

        /*
            1.3. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with out of range configSetIndex [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        configSetIndex = 4;

        st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configSetIndex = %d", dev, configSetIndex);

        configSetIndex = 0;

        /*
            1.5. Call with out of range maxBuffers [2048]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        maxBuffers = 2048;

        st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, maxBuffers = %d", dev, maxBuffers);
    }

    voqUnitNum     = 0;
    configSetIndex = 0;
    maxBuffers     = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, maxBuffers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, maxBuffers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   voqUnitNum,
    IN  GT_U32   configSetIndex,
    OUT GT_U32   *maxBuffersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   configSetIndex [0 / 3]
                   and non-NULL maxBuffersPtr.
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range configSetIndex [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with maxBuffersPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum     = 0;
    GT_U32      configSetIndex = 0;
    GT_U32      maxBuffers     = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           configSetIndex [0 / 3]
                           and non-NULL maxBuffersPtr.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum     = 0;
        configSetIndex = 0;

        st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, &maxBuffers);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, configSetIndex);

        /* Call with voqUnitNum [1] */
        voqUnitNum     = 1;
        configSetIndex = 3;

        st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, &maxBuffers);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, voqUnitNum, configSetIndex);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, &maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with out of range configSetIndex [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        configSetIndex = 4;

        st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, &maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configSetIndex = %d", dev, configSetIndex);

        configSetIndex = 0;

        /*
            1.4. Call with maxBuffersPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maxBuffersPtr = NULL", dev);
    }

    voqUnitNum     = 0;
    configSetIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, &maxBuffers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, &maxBuffers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet
(
    IN GT_U8                             devNum,
    IN GT_U32                            voqUnitNum,
    IN GT_U32                            configSetIndex,
    IN GT_U32                            fabricTc,
    IN CPSS_DP_LEVEL_ENT                 dp,
    IN GT_U32                            maxBuffers
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   configSetIndex [0 / 3],
                   fabricTc [0 / 3],
                   dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E]
                   and maxBuffers [1 / 2047].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet with the same voqUnitNum,
                                                                      configSetIndex,
                                                                      fabricTc,
                                                                      dp
                                                                      and non-NULL maxBuffersPtr.
    Expected: GT_OK and the same maxBuffers.
    1.3. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range configSetIndex [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range fabricTc [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range dp [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with out of range maxBuffers [2048]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32              voqUnitNum     = 0;
    GT_U32              configSetIndex = 0;
    GT_U32              fabricTc       = 0;
    CPSS_DP_LEVEL_ENT   dp             = CPSS_DP_GREEN_E;
    GT_U32              maxBuffers     = 0;
    GT_U32              maxBuffersGet  = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           configSetIndex [0 / 3],
                           fabricTc [0 / 3],
                           dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E]
                           and maxBuffers [1 / 2047].
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum     = 0;
        configSetIndex = 0;
        fabricTc       = 0;
        dp             = CPSS_DP_GREEN_E;
        maxBuffers     = 1;

        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, fabricTc, dp, maxBuffers);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, voqUnitNum, configSetIndex, fabricTc, dp, maxBuffers);

        /*
            1.2. Call cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet with the same voqUnitNum,
                                                                              configSetIndex,
                                                                              fabricTc,
                                                                              dp
                                                                              and non-NULL maxBuffersPtr.
            Expected: GT_OK and the same maxBuffers.
        */
        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, fabricTc, dp, &maxBuffersGet);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet: %d, %d, %d, %d, %d", dev, voqUnitNum, configSetIndex, fabricTc, dp);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maxBuffers, maxBuffersGet,
                   "get another maxBuffers than was set: %d", dev);

        /* Call with voqUnitNum [1] */
        voqUnitNum     = 1;
        configSetIndex = 3;
        fabricTc       = 3;
        dp             = CPSS_DP_RED_E;
        maxBuffers     = 2047;

        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, fabricTc, dp, maxBuffers);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, voqUnitNum, configSetIndex, fabricTc, dp, maxBuffers);

        /*
            1.2. Call cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet with the same voqUnitNum,
                                                                              configSetIndex,
                                                                              fabricTc,
                                                                              dp
                                                                              and non-NULL maxBuffersPtr.
            Expected: GT_OK and the same maxBuffers.
        */
        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, fabricTc, dp, &maxBuffersGet);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                   "cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet: %d, %d, %d, %d, %d", dev, voqUnitNum, configSetIndex, fabricTc, dp);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maxBuffers, maxBuffersGet,
                   "get another maxBuffers than was set: %d", dev);

        /*
            1.3. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, fabricTc, dp, maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with out of range configSetIndex [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        configSetIndex = 4;

        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, fabricTc, dp, maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configSetIndex = %d", dev, configSetIndex);

        configSetIndex = 0;

        /*
            1.5. Call with out of range fabricTc [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fabricTc = 4;

        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, fabricTc, dp, maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fabricTc = %d", dev, fabricTc);

        fabricTc = 0;

        /*
            1.6. Call with out of range dp [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        dp = FABRIC_VOQ_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, fabricTc, dp, maxBuffers);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, dp = %d", dev, dp);

        dp = CPSS_DP_GREEN_E;

        /*
            1.7. Call with out of range maxBuffers [2048]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        maxBuffers = 2048;

        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, fabricTc, dp, maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, maxBuffers = %d", dev, maxBuffers);
    }

    voqUnitNum     = 0;
    configSetIndex = 0;
    fabricTc       = 0;
    dp             = CPSS_DP_GREEN_E;
    maxBuffers     = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, fabricTc, dp, maxBuffers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet(dev, voqUnitNum, configSetIndex, fabricTc, dp, maxBuffers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            voqUnitNum,
    IN  GT_U32                            configSetIndex,
    IN  GT_U32                            fabricTc,
    IN  CPSS_DP_LEVEL_ENT                 dp,
    OUT GT_U32                            *maxBuffersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   configSetIndex [0 / 3],
                   fabricTc [0 / 3],
                   dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E]
                   and non-NULL maxBuffersPtr.
    Expected: GT_OK.
    1.2. Call with out of range voqUnitNum [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range configSetIndex [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range fabricTc [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range dp [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with maxBuffersPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32              voqUnitNum     = 0;
    GT_U32              configSetIndex = 0;
    GT_U32              fabricTc       = 0;
    CPSS_DP_LEVEL_ENT   dp             = CPSS_DP_GREEN_E;
    GT_U32              maxBuffers     = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           configSetIndex [0 / 3],
                           fabricTc [0 / 3],
                           dp [CPSS_DP_GREEN_E / CPSS_DP_RED_E]
                           and non-NULL maxBuffersPtr.
            Expected: GT_OK.
        */

        /* Call with voqUnitNum [0] */
        voqUnitNum     = 0;
        configSetIndex = 0;
        fabricTc       = 0;
        dp             = CPSS_DP_GREEN_E;

        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, fabricTc, dp, &maxBuffers);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, voqUnitNum, configSetIndex, fabricTc, dp);

        /* Call with voqUnitNum [1] */
        voqUnitNum     = 1;
        configSetIndex = 3;
        fabricTc       = 3;
        dp             = CPSS_DP_RED_E;

        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, fabricTc, dp, &maxBuffers);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, voqUnitNum, configSetIndex, fabricTc, dp);

        /*
            1.2. Call with out of range voqUnitNum [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, fabricTc, dp, &maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with out of range configSetIndex [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        configSetIndex = 4;

        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, fabricTc, dp, &maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configSetIndex = %d", dev, configSetIndex);

        configSetIndex = 0;

        /*
            1.4. Call with out of range fabricTc [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        fabricTc = 4;

        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, fabricTc, dp, &maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fabricTc = %d", dev, fabricTc);

        fabricTc = 0;

        /*
            1.5. Call with out of range dp [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        dp = FABRIC_VOQ_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, fabricTc, dp, &maxBuffers);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, dp = %d", dev, dp);

        dp = CPSS_DP_GREEN_E;

        /*
            1.6. Call with maxBuffersPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, fabricTc, dp, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maxBuffersPtr = NULL", dev);
    }

    voqUnitNum     = 0;
    configSetIndex = 0;
    fabricTc       = 0;
    dp             = CPSS_DP_GREEN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, fabricTc, dp, &maxBuffers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet(dev, voqUnitNum, configSetIndex, fabricTc, dp, &maxBuffers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqLastDropDevGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  voqUnitNum,
    OUT GT_U32  *lastDropDevPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqLastDropDevGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1] not NULL lastDropDevPtr.
    Exdpected: GT_OK.
    1.2. Call with voqUnitNum [2] (out of range).
    Expected: NOT GT_OK.
    1.3. Call with lastDropDevPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;


    GT_U32  voqUnitNum = 0;
    GT_U32  lastDropDev = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1] not NULL lastDropDevPtr.
            Exdpected: GT_OK.
        */
        /* Call with voqUnitNum [0] */
        voqUnitNum = 0;

        st = cpssExMxPmFabricVoqLastDropDevGet(dev, voqUnitNum, &lastDropDev);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /* Call with voqUnitNum [1] */
        voqUnitNum = 1;

        st = cpssExMxPmFabricVoqLastDropDevGet(dev, voqUnitNum, &lastDropDev);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. Call with voqUnitNum [2] (out of range).
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqLastDropDevGet(dev, voqUnitNum, &lastDropDev);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with lastDropDevPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqLastDropDevGet(dev, voqUnitNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lastDropDevPtr = NULL", dev);
    }

    voqUnitNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqLastDropDevGet(dev, voqUnitNum, &lastDropDev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqLastDropDevGet(dev, voqUnitNum, &lastDropDev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqSharedBuffersConfigSet
(
    IN  GT_U8                                               devNum,
    IN  GT_U32                                              voqUnitNum,
    IN  CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_SHARED_LIMIT_STC    *limitParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqSharedBuffersConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   limitParams { poolBufLimit [0 / 2047]
                                 usersDpMode [CPSS_EXMXPM_FABRIC_VOQ_SHARED_POOL_USERS_DP_0_E /
                                              CPSS_EXMXPM_FABRIC_VOQ_SHARED_POOL_USERS_DP_ALL_E] }
    Exdpected: GT_OK.
    1.2. cpssExMxPmFabricVoqSharedBuffersConfigGet with not NULL limitParamsPtrand other params from 1.1.
    Expected: GT_OK and the same limitParamsPtr as was set.
    1.3. Call with voqUnitNum [2] (out of range).
    Expected: NOT GT_OK.
    1.4. Call with limitParams->poolBufLimit [2048] (out of range).
    Expected: NOT GT_OK.
    1.5. Call with limitParams->usersDpMode [0x5AAAAAA5] (out of range).
    Expected: GT_BAD_PARAM.
    1.6. Call with limitParamsPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_SHARED_LIMIT_STC limitParams;
    CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_SHARED_LIMIT_STC limitParamsGet;


    cpssOsBzero((GT_VOID*) &limitParams, sizeof(limitParams));
    cpssOsBzero((GT_VOID*) &limitParamsGet, sizeof(limitParamsGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           lastDropDev { poolBufLimit [0 / 2047]
                                         usersDpMode [CPSS_EXMXPM_FABRIC_VOQ_SHARED_POOL_USERS_DP_0_E /
                                                      CPSS_EXMXPM_FABRIC_VOQ_SHARED_POOL_USERS_DP_ALL_E] }
            Exdpected: GT_OK.
        */
        /* iterate with voqUnitNum = 0 */
        voqUnitNum = 0;
        limitParams.poolBufLimit = 0;
        limitParams.usersDpMode = CPSS_EXMXPM_FABRIC_VOQ_SHARED_POOL_USERS_DP_0_E;

        st = cpssExMxPmFabricVoqSharedBuffersConfigSet(dev, voqUnitNum, &limitParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. cpssExMxPmFabricVoqSharedBuffersConfigGet with not NULL limitParamsPtrand other params from 1.1.
            Expected: GT_OK and the same limitParamsPtr as was set.
        */
        st = cpssExMxPmFabricVoqSharedBuffersConfigGet(dev, voqUnitNum, &limitParamsGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricVoqSharedBuffersConfigGet: %d, %d", dev, voqUnitNum);

        UTF_VERIFY_EQUAL1_STRING_MAC(limitParams.poolBufLimit, limitParamsGet.poolBufLimit, 
                                     "got another limitParams.poolBufLimit than wa sset: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(limitParams.usersDpMode, limitParamsGet.usersDpMode, 
                                     "got another limitParams.usersDpMode than wa sset: %d", dev);

        /* iterate with voqUnitNum = 1 */
        voqUnitNum = 1;
        limitParams.poolBufLimit = 2047;
        limitParams.usersDpMode = CPSS_EXMXPM_FABRIC_VOQ_SHARED_POOL_USERS_DP_ALL_E;

        st = cpssExMxPmFabricVoqSharedBuffersConfigSet(dev, voqUnitNum, &limitParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. cpssExMxPmFabricVoqSharedBuffersConfigGet with not NULL limitParamsPtrand other params from 1.1.
            Expected: GT_OK and the same limitParamsPtr as was set.
        */
        st = cpssExMxPmFabricVoqSharedBuffersConfigGet(dev, voqUnitNum, &limitParamsGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricVoqSharedBuffersConfigGet: %d, %d", dev, voqUnitNum);

        UTF_VERIFY_EQUAL1_STRING_MAC(limitParams.poolBufLimit, limitParamsGet.poolBufLimit, 
                                     "got another limitParams.poolBufLimit than wa sset: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(limitParams.usersDpMode, limitParamsGet.usersDpMode, 
                                     "got another limitParams.usersDpMode than wa sset: %d", dev);

        /*
            1.3. Call with voqUnitNum [2] (out of range).
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqSharedBuffersConfigSet(dev, voqUnitNum, &limitParams);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with limitParams->poolBufLimit [2048] (out of range).
            Expected: NOT GT_OK.
        */
        limitParams.poolBufLimit = 2048;

        st = cpssExMxPmFabricVoqSharedBuffersConfigSet(dev, voqUnitNum, &limitParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, limitParams.poolBufLimit = %d", dev, limitParams.poolBufLimit);

        limitParams.poolBufLimit = 0;

        /*
            1.5. Call with limitParams->usersDpMode [0x5AAAAAA5] (out of range).
            Expected: GT_BAD_PARAM.
        */
        limitParams.usersDpMode = FABRIC_VOQ_INVALID_ENUM_CNS;

        st = cpssExMxPmFabricVoqSharedBuffersConfigSet(dev, voqUnitNum, &limitParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, limitParams.usersDpMode = %d", dev, limitParams.usersDpMode);

        limitParams.usersDpMode = CPSS_EXMXPM_FABRIC_VOQ_SHARED_POOL_USERS_DP_0_E;

        /*
            1.6. Call with limitParamsPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqSharedBuffersConfigSet(dev, voqUnitNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, limitParams = NULL", dev);
    }

    voqUnitNum = 0;
    limitParams.poolBufLimit = 0;
    limitParams.usersDpMode = CPSS_EXMXPM_FABRIC_VOQ_SHARED_POOL_USERS_DP_0_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqSharedBuffersConfigSet(dev, voqUnitNum, &limitParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqSharedBuffersConfigSet(dev, voqUnitNum, &limitParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqSharedBuffersConfigGet
(
    IN  GT_U8                                               devNum,
    IN  GT_U32                                              voqUnitNum,
    OUT CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_SHARED_LIMIT_STC    *limitParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqSharedBuffersConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1],
                   not NULL limitParams.
    Exdpected: GT_OK.
    1.2. Call with voqUnitNum [2] (out of range).
    Expected: NOT GT_OK.
    1.3. Call with limitParamsPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      voqUnitNum = 0;
    CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_SHARED_LIMIT_STC limitParams;


    cpssOsBzero((GT_VOID*) &limitParams, sizeof(limitParams));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1],
                           not NULL limitParams.
            Exdpected: GT_OK.
        */
        /* iterate with voqUnitNum = 0 */
        voqUnitNum = 0;

        st = cpssExMxPmFabricVoqSharedBuffersConfigSet(dev, voqUnitNum, &limitParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /* iterate with voqUnitNum = 1 */
        voqUnitNum = 1;

        st = cpssExMxPmFabricVoqSharedBuffersConfigSet(dev, voqUnitNum, &limitParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. Call with voqUnitNum [2] (out of range).
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqSharedBuffersConfigSet(dev, voqUnitNum, &limitParams);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with limitParamsPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqSharedBuffersConfigSet(dev, voqUnitNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, limitParamsPtr = NULL", dev);
    }

    voqUnitNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqSharedBuffersConfigSet(dev, voqUnitNum, &limitParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqSharedBuffersConfigSet(dev, voqUnitNum, &limitParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqRandomTailDropSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  voqUnitNum,
    IN  GT_U32  voqRandomMask,
    IN  GT_U32  voqPriorityRandomMask
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqRandomTailDropSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1], 
                   voqRandomMask [0 / 127], 
                   voqPriorityRandomMask [0 / 127].
    Expected: GT_OK.
    1.2. Call cpssExMxPmFabricVoqRandomTailDropGet with not NULL pointers and voqUnitNum from 1.1.
    Expected: GT_OK and the same params as was set.
    1.3. Call with voqUnitNum [2] (out of range).
    Expected: NOT GT_OK.
    1.4. Call with voqRandomMask [128] (out of range).
    Expected: NOT GT_OK.
    1.5. Call with voqPriorityRandomMask [128] (out of range).
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32  voqUnitNum = 0;
    GT_U32  voqRandomMask = 0;
    GT_U32  voqRandomMaskGet = 0;
    GT_U32  voqPriorityRandomMask = 0;
    GT_U32  voqPriorityRandomMaskGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1], 
                           voqRandomMask [0 / 127], 
                           voqPriorityRandomMask [0 / 127].
            Expected: GT_OK.
        */
        /* iterate with voqUnitNum = 0 */
        voqUnitNum = 0;
        voqRandomMask = 0;
        voqPriorityRandomMask = 0;

        st = cpssExMxPmFabricVoqRandomTailDropSet(dev, voqUnitNum, voqRandomMask, voqPriorityRandomMask);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqUnitNum, voqRandomMask, voqPriorityRandomMask);

        /*
            1.2. Call cpssExMxPmFabricVoqRandomTailDropGet with not NULL pointers and voqUnitNum from 1.1.
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmFabricVoqRandomTailDropGet(dev, voqUnitNum, &voqRandomMaskGet, &voqPriorityRandomMaskGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricVoqRandomTailDropGet: %d, %d", dev, voqUnitNum);

        UTF_VERIFY_EQUAL1_STRING_MAC(voqRandomMask, voqRandomMaskGet, 
                                     "got another voqRandomMask than wa sset: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(voqPriorityRandomMask, voqPriorityRandomMaskGet, 
                                     "got another voqPriorityRandomMask than wa sset: %d", dev);

        /* iterate with voqUnitNum = 1 */
        voqUnitNum = 1;
        voqRandomMask = 127;
        voqPriorityRandomMask = 127;

        st = cpssExMxPmFabricVoqRandomTailDropSet(dev, voqUnitNum, voqRandomMask, voqPriorityRandomMask);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, voqUnitNum, voqRandomMask, voqPriorityRandomMask);

        /*
            1.2. Call cpssExMxPmFabricVoqRandomTailDropGet with not NULL pointers and voqUnitNum from 1.1.
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmFabricVoqRandomTailDropGet(dev, voqUnitNum, &voqRandomMaskGet, &voqPriorityRandomMaskGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmFabricVoqRandomTailDropGet: %d, %d", dev, voqUnitNum);

        UTF_VERIFY_EQUAL1_STRING_MAC(voqRandomMask, voqRandomMaskGet, 
                                     "got another voqRandomMask than wa sset: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(voqPriorityRandomMask, voqPriorityRandomMaskGet, 
                                     "got another voqPriorityRandomMask than wa sset: %d", dev);

        /*
            1.3. Call with voqUnitNum [2] (out of range).
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqRandomTailDropSet(dev, voqUnitNum, voqRandomMask, voqPriorityRandomMask);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.4. Call with voqRandomMask [128] (out of range).
            Expected: NOT GT_OK.
        */
        voqRandomMask = 128;

        st = cpssExMxPmFabricVoqRandomTailDropSet(dev, voqUnitNum, voqRandomMask, voqPriorityRandomMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, voqRandomMask = %d", dev, voqRandomMask);

        voqRandomMask = 0;

        /*
            1.5. Call with voqPriorityRandomMask [128] (out of range).
            Expected: NOT GT_OK.
        */
        voqPriorityRandomMask = 128;

        st = cpssExMxPmFabricVoqRandomTailDropSet(dev, voqUnitNum, voqRandomMask, voqPriorityRandomMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, voqPriorityRandomMask = %d", dev, voqPriorityRandomMask);
    }

    voqUnitNum = 0;
    voqRandomMask = 0;
    voqPriorityRandomMask = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqRandomTailDropSet(dev, voqUnitNum, voqRandomMask, voqPriorityRandomMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqRandomTailDropSet(dev, voqUnitNum, voqRandomMask, voqPriorityRandomMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmFabricVoqRandomTailDropGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  voqUnitNum,
    OUT GT_U32  *voqRandomMaskPtr,
    OUT GT_U32  *voqPriorityRandomMaskPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmFabricVoqRandomTailDropGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with voqUnitNum [0 / 1], 
                   not NULL voqRandomMask, voqPriorityRandomMask.
    Expected: GT_OK.
    1.2. Call with voqUnitNum [2] (out of range).
    Expected: NOT GT_OK.
    1.3. Call with voqRandomMask [NULL].
    Expected: GT_BAD_PTR.
    1.4. Call with voqPriorityRandomMask [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32  voqUnitNum = 0;
    GT_U32  voqRandomMask = 0;
    GT_U32  voqPriorityRandomMask = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with voqUnitNum [0 / 1], 
                           not NULL voqRandomMask, voqPriorityRandomMask.
            Expected: GT_OK.
        */
        voqUnitNum = 0;

        st = cpssExMxPmFabricVoqRandomTailDropGet(dev, voqUnitNum, &voqRandomMask, &voqPriorityRandomMask);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 1;

        st = cpssExMxPmFabricVoqRandomTailDropGet(dev, voqUnitNum, &voqRandomMask, &voqPriorityRandomMask);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        /*
            1.2. Call with voqUnitNum [2] (out of range).
            Expected: NOT GT_OK.
        */
        voqUnitNum = 2;

        st = cpssExMxPmFabricVoqRandomTailDropGet(dev, voqUnitNum, &voqRandomMask, &voqPriorityRandomMask);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, voqUnitNum);

        voqUnitNum = 0;

        /*
            1.3. Call with voqRandomMask [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqRandomTailDropGet(dev, voqUnitNum, NULL, &voqPriorityRandomMask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, voqRandomMask = NULL", dev);

        /*
            1.4. Call with voqPriorityRandomMask [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmFabricVoqRandomTailDropGet(dev, voqUnitNum, &voqRandomMask, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, voqPriorityRandomMask = NULL", dev);
    }

    voqUnitNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmFabricVoqRandomTailDropGet(dev, voqUnitNum, &voqRandomMask, &voqPriorityRandomMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmFabricVoqRandomTailDropGet(dev, voqUnitNum, &voqRandomMask, &voqPriorityRandomMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmFabricVoq suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmFabricVoq)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqFportTxEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqFportTxEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqDevTblSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqDevEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqDevEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqDevToFportMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqDevToFportMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqDevToConfigSetMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqDevToConfigSetMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqTcToFabricTcMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqTcToFabricTcMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqControlTrafficQosSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqControlTrafficQosGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqGlobalLimitsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqGlobalLimitsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqStatCntrsConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqStatCntrsConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqStatCntrsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqGetGlobalCountersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqFportResourcesInUseCntrsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqMcToConfigSetMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqMcToConfigSetMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqLastDropDevGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqSharedBuffersConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqSharedBuffersConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqRandomTailDropSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmFabricVoqRandomTailDropGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmFabricVoq)

